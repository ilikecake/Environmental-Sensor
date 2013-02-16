/*   This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
*	\brief		Hardware definitions and functions for the environmental sensor.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/3/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

#define HARDWARE_TIMER_0_TOP_VALUE	124

//Global variables needed for the timer
TimeAndDate TimerStartTime;
volatile uint16_t TimerStartMS;
volatile uint8_t TimerRemainder;
volatile uint8_t TimerRunning;

//Global variables needed for the RTC
TimeAndDate TheTime;
volatile uint16_t ElapsedMS;




void HardwareInit( void )
{
	//Initalize variables
	ElapsedMS		= 0x0000;
	TheTime.sec		= 0;
	TheTime.min		= 0;
	TheTime.hour	= 0;
	TheTime.dow		= 0;
	TheTime.day		= 0;
	TheTime.month	= 0;
	TheTime.year	= 0;
	TimerRunning = 0;
	
	//Disable watchdog if enabled by bootloader/fuses
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	
	//Disable JTAG (this command must be sent twice)
	MCUCR = 0x80;
	MCUCR = 0x80;

	//Disable clock division
	clock_prescale_set(clock_div_1);

	//Hardware Initialization
	
	
	//Setup timer 0 for 1ms interrupts
	//CTC Mode
	//Clock is Fcpu/64
	//OCR0A interrupt ~every 1ms
	TCCR0A = 0x02;
	TCCR0B = 0x03;
	TIMSK0 = 0x02;
	OCR0A = HARDWARE_TIMER_0_TOP_VALUE;
	
	//Enable interrupts globally
	sei();
	
	//Setup GPIO Pins
	
	//PORT B:
	//	0: Dataflash CS line			(Out, high)
	//	4: Pressure sensor CS line		(Out, high)
	//	6: Pressure sensor sleep line	(Out, low)
	DDRB	= 1 | (1<<4) | (1<<6);
	PORTB	= 1 | (1<<4);
	
	//PORT C:
	//	4: Config line 1			(Input, pullup)
	//	5: Config line 2			(Input, pullup)
	//	7: Light sensor interrupt 	(Input, pullup)
	DDRC	= 0x00;
	PORTC	= (1<<4) | (1<<5) | (1<<7);
	
	//PORT D:
	//	2:	LED				(Out, low)
	//	7:	HwB Button		(Input, high-Z)
	DDRD	= (1<<2);
	PORTD	= 0x00;
	
	//Enable USB and interrupts
	InitSPIMaster(0,0);
	I2CSoft_Init();
	USB_Init();
	
	//Initalize peripherals
	tcs3414_Init();
	MPL115A1_Init();
	AT45DB321D_Init();
	
	return;
}

void LED(uint8_t LEDState)
{
	if(LEDState == 1)
	{
		PORTD |= (1<<2);
	}
	else
	{
		PORTD &= ~(1<<2);
	}
	return;
}

void DelayMS(uint16_t ms)
{
	uint16_t WaitMS = 0;
	
	if(ms == 0) return;
	
	//Delay is too long, call delaySec?
	if (ms >= 1000)
	{
		return;
	}
	
	//Look for milisecond overflow
	if((ms + ElapsedMS) > 1000)
	{
		WaitMS = ms + ElapsedMS - 1001;
	}
	else
	{
		WaitMS = ms + ElapsedMS;
	}
	
	while (WaitMS != ElapsedMS)
	{
		asm volatile ("nop");
	}
	return;
}

void GetTime( TimeAndDate *TimeToReturn )
{
	//The rest of this struct is not used right now
	TimeToReturn->day 	= 	TheTime.day;
	TimeToReturn->hour 	= 	TheTime.hour;
	TimeToReturn->min 	= 	TheTime.min;
	TimeToReturn->sec 	= 	TheTime.sec;
	return;
}

void SetTime( TimeAndDate TimeToSet )
{
	TheTime.day = TimeToSet.day;
	TheTime.hour = TimeToSet.hour;
	TheTime.min = TimeToSet.min;
	TheTime.sec = TimeToSet.sec;
	return;
}

void StartTimer(void)
{
	TimerStartTime.day = TheTime.day;
	TimerStartTime.hour = TheTime.hour;
	TimerStartTime.min = TheTime.min;
	TimerStartTime.sec = TheTime.sec;
	TimerStartMS = ElapsedMS;
	TimerRemainder = TCNT0;
	TimerRunning = 1;

	return;
}


void StopTimer(void)
{
	//TODO: add a check to see if the timer is running.
	TimeAndDate TimerEndTime;
	uint8_t TimerEndRemainder = 0;
	uint16_t ElapsedUS;
	uint16_t TimerEndMS;
	
	if(TimerRunning == 1)
	{
		//Get final timer value
		TimerEndTime.day = TheTime.day;
		TimerEndTime.hour = TheTime.hour;
		TimerEndTime.min = TheTime.min;
		TimerEndTime.sec = TheTime.sec;
		TimerEndMS = ElapsedMS;
		TimerEndRemainder = TCNT0;
		
		
		
		printf_P(PSTR("Time: %02u sec %04u ms %04u us\n"), TimerEndTime.sec-TimerStartTime.sec, TimerEndMS-TimerStartMS , (HARDWARE_TIMER_0_TOP_VALUE - TimerRemainder) + TimerEndRemainder);
		
		
		
		
		
		
		
		//TimerEndMS
		//ElapsedUS = (HARDWARE_TIMER_0_TOP_VALUE - TimerRemainder) + TimerEndRemainder;
		//if(ElapsedUS > 1000)
		//{
		//	ElapsedUS = ElapsedUS - 1000;
		//}
		
		/*if(TimerEndTime.day != TimerStartTime.day)
		{
			TimerEndTime.day = TimerEndTime.day - TimerStartTime.day;
			TimerEndTime.hour = TimerEndTime.hour + (24 - TimerStartTime.hour);
			TimerEndTime.min = TimerEndTime.min + (60 - TimerStartTime.min);
			TimerEndTime.sec = TimerEndTime.sec + (60 - TimerStartTime.sec);
		}
		
		if(
		
		
		
		
		
			if(TimerEndTime.hour = TimerStartTime.hour)
			{
				if(TimerEndTime.min = TimerStartTime.min)
				{
					if(TimerEndTime.sec = TimerStartTime.sec)
					{
					
					}*/
		
		
		
		//Reset the timer value
		TimerRunning = 0;
	}
	return;
}




//Timer interrupt 0 for basic timing stuff
ISR(TIMER0_COMPA_vect)
{
	uint16_t inByte;
	ElapsedMS++;
	
	//Handle USB stuff
	//This happens every ~8 ms
	if( ((ElapsedMS & 0x0007) == 0x0000) )
	{
		//receive and process a character from the USB CDC interface
		inByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		if((inByte > 0) && (inByte < 255))
		{
			CommandGetInputChar(inByte);	//NOTE: this limits the device to recieve a single character every 8ms (I think). This should not be a problem for user input.
		}
		
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}
	
	if(ElapsedMS >= 1000)
	{
		ElapsedMS = 0;
		TheTime.sec += 1;
		if(TheTime.sec > 59)
		{
			TheTime.sec = 0;
			TheTime.min += 1;
			if(TheTime.min > 59)
			{
				TheTime.min = 0;
				TheTime.hour += 1;
				if(TheTime.hour > 24)
				{
					TheTime.hour = 0;
					TheTime.day += 1;
					//This is gonna get complicated at this point. For now I will ignore this. 
				}
			}
		}
	}
}

/** @} */
