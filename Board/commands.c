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
*	\brief		Command interpreter application specific functions
*	\author		Pat Satyshur
*	\version	1.1
*	\date		1/13/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	beer_heater_main
*
*	@{
*/

#include "main.h"
//#include "commands.h"


//The number of commands
const uint8_t NumCommands = 11;

//Handler function declerations

//LED control function
static int _F1_Handler (void);
const char _F1_NAME[] PROGMEM 			= "led";
const char _F1_DESCRIPTION[] PROGMEM 	= "Turn LED on or off";
const char _F1_HELPTEXT[] PROGMEM 		= "led <number>";

//Jump to DFU bootloader
static int _F2_Handler (void);
const char _F2_NAME[] PROGMEM 			= "dfu";
const char _F2_DESCRIPTION[] PROGMEM 	= "Jump to bootloader";
const char _F2_HELPTEXT[] PROGMEM 		= "'dfu' has no parameters";

//Read a register
static int _F3_Handler (void);
const char _F3_NAME[] PROGMEM 			= "regread";
const char _F3_DESCRIPTION[] PROGMEM 	= "Read a register from the TCS3414FN";
const char _F3_HELPTEXT[] PROGMEM 		= "regread <register #>";

//Set time on the internal timer
static int _F4_Handler (void);
const char _F4_NAME[] PROGMEM 			= "settime";
const char _F4_DESCRIPTION[] PROGMEM 	= "Set the time";
const char _F4_HELPTEXT[] PROGMEM 		= "settime <day> <hr> <min> <sec>";

//Read the time from the internal timer
static int _F5_Handler (void);
const char _F5_NAME[] PROGMEM 			= "gettime";
const char _F5_DESCRIPTION[] PROGMEM 	= "Get the time from the internal timer";
const char _F5_HELPTEXT[] PROGMEM 		= "'gettime' has not parameters";

//Write a register
static int _F6_Handler (void);
const char _F6_NAME[] PROGMEM 			= "regwrite";
const char _F6_DESCRIPTION[] PROGMEM 	= "write to a register";
const char _F6_HELPTEXT[] PROGMEM 		= "regwrite <register> <data>";

//Get a set of data from the devices
static int _F8_Handler (void);
const char _F8_NAME[] PROGMEM 			= "data";
const char _F8_DESCRIPTION[] PROGMEM 	= "Get a data set";
const char _F8_HELPTEXT[] PROGMEM 		= "'data' has no parameters";

//Read a register from the memory
static int _F9_Handler (void);
const char _F9_NAME[] PROGMEM 			= "memread";
const char _F9_DESCRIPTION[] PROGMEM 	= "Read data from memory";
const char _F9_HELPTEXT[] PROGMEM 		= "memread <register>";

//Pressure sensor functions
static int _F10_Handler (void);
const char _F10_NAME[] PROGMEM 			= "pres";
const char _F10_DESCRIPTION[] PROGMEM 	= "Pressure sensor functions";
const char _F10_HELPTEXT[] PROGMEM 		= "pres <function>";

//Get temperatures from the ADC
static int _F11_Handler (void);
const char _F11_NAME[] PROGMEM 			= "temp";
const char _F11_DESCRIPTION[] PROGMEM 	= "Get temperatures from the ADC";
const char _F11_HELPTEXT[] PROGMEM 		= "'temp' has no parameters";

//Scan the TWI bus for devices
static int _F12_Handler (void);
const char _F12_NAME[] PROGMEM 			= "twiscan";
const char _F12_DESCRIPTION[] PROGMEM 	= "Scan for TWI devices";
const char _F12_HELPTEXT[] PROGMEM 		= "'twiscan' has no parameters";

//Command list
const CommandListItem AppCommandList[] PROGMEM =
{
	{ _F1_NAME,		1,  1,	_F1_Handler,	_F1_DESCRIPTION,	_F1_HELPTEXT	},		//led
	{ _F2_NAME, 	0,  0,	_F2_Handler,	_F2_DESCRIPTION,	_F2_HELPTEXT	},		//dfu
	{ _F3_NAME, 	1,  1,	_F3_Handler,	_F3_DESCRIPTION,	_F3_HELPTEXT	},		//regread
	{ _F4_NAME, 	4,  4,	_F4_Handler,	_F4_DESCRIPTION,	_F4_HELPTEXT	},		//settime
	{ _F5_NAME, 	0,  0,	_F5_Handler,	_F5_DESCRIPTION,	_F5_HELPTEXT	},		//gettime
	{ _F6_NAME, 	2,  2,	_F6_Handler,	_F6_DESCRIPTION,	_F6_HELPTEXT	},		//writereg	
	{ _F8_NAME,		0,  0,	_F8_Handler,	_F8_DESCRIPTION,	_F8_HELPTEXT	},		//data
	{ _F9_NAME,		1,  1,	_F9_Handler,	_F9_DESCRIPTION,	_F9_HELPTEXT	},		//memread
	{ _F10_NAME,	1,  1,	_F10_Handler,	_F10_DESCRIPTION,	_F10_HELPTEXT	},		//pres
	{ _F11_NAME,	0,  0,	_F11_Handler,	_F11_DESCRIPTION,	_F11_HELPTEXT	},		//temp
	{ _F12_NAME,	0,  0,	_F12_Handler,	_F12_DESCRIPTION,	_F12_HELPTEXT	},		//twiscan
};

//Command functions

//LED control function
static int _F1_Handler (void)
{
	LED((uint8_t)argAsInt(1));
	return 0;
}

//Jump to DFU bootloader
static int _F2_Handler (void)
{
	printf_P(PSTR("Jumping to bootloader. A manual reset will be required\nPress 'y' to continue..."));
	
	if(WaitForAnyKey() == 'y')
	{
		printf_P(PSTR("Jump\n"));
		DelayMS(100);
		Jump_To_Bootloader();
	}
	
	printf_P(PSTR("Canceled\n"));
	return 0;
}

//Read a register
static int _F3_Handler (void)
{
	uint8_t DataToReceive = 0;
	uint8_t DataToSend = argAsInt(1);
	
	if(tcs3414_ReadReg(DataToSend, &DataToReceive) == 0)
	{
		printf_P(PSTR("reg[0x%02X]: 0x%02X\n"), DataToSend, DataToReceive);
	}
	else
	{
		printf_P(PSTR("Error\n"));
	}
	
	return 0;
}

//Set time on the internal timer
static int _F4_Handler (void)
{
	TimeAndDate CurrentTime;
	
	CurrentTime.month	= 0;
	CurrentTime.day		= argAsInt(1);
	CurrentTime.year	= 0;
	CurrentTime.hour	= argAsInt(2);
	CurrentTime.min		= argAsInt(3);
	CurrentTime.sec		= argAsInt(4);
	CurrentTime.dow		= 0;
	
	SetTime(CurrentTime);
	
	printf_P(PSTR("Done\n"));
	return 0;
}

//Read the time from the internal timer
static int _F5_Handler (void)
{
	TimeAndDate CurrentTime;
	
	GetTime(&CurrentTime);
	printf_P(PSTR("%02u Days %02u:%02u:%02u\n"), CurrentTime.day, CurrentTime.hour, CurrentTime.min, CurrentTime.sec);

	return 0;
}

//Write a register
static int _F6_Handler (void)
{
	uint8_t RegToWrite = argAsInt(1);
	uint32_t DataToWrite = argAsInt(2);

	if(tcs3414_WriteReg(RegToWrite, DataToWrite) == 0)
	{
		printf_P(PSTR("OK\n"));
	}
	else
	{
		printf_P(PSTR("Error\n"));
	}

	return 0;
}

//Get a set of data from the devices
static int _F8_Handler (void)
{
	uint16_t LS_Data[4];

	//StartTimer();
	if(tcs3414_GetData(&LS_Data[0], &LS_Data[1], &LS_Data[2], &LS_Data[3]) == 0)
	{
		printf_P(PSTR("red:	0x%04X\n"), LS_Data[0]);
		printf_P(PSTR("green:	0x%04X\n"), LS_Data[1]);
		printf_P(PSTR("blue:	0x%04X\n"), LS_Data[2]);
		printf_P(PSTR("clear:	0x%04X\n"), LS_Data[3]);
	}
	//StopTimer();

	return 0;
}

//Read a register from the memory
static int _F9_Handler (void)
{
	uint8_t RegToRead = argAsInt(1);
	if(RegToRead == 1)
	{
		AT45DB321D_Select();
	}
	else if(RegToRead == 0)
	{
		AT45DB321D_Deselect();
	}
	else if(RegToRead == 2)	//Read status
	{
		//SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_FALLING | SPI_SAMPLE_TRAILING | SPI_MODE_MASTER);		
		SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_RISING | SPI_SAMPLE_LEADING | SPI_MODE_MASTER);		//Mode 0,0 is good
		AT45DB321D_Select();
		SPI_SendByte(AT45DB321D_CMD_READ_STATUS);
		printf("Stat: 0x%02X\n", SPI_ReceiveByte());
		AT45DB321D_Deselect();
	}
	else if(RegToRead == 3)	//Read IDs
	{
		//SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_FALLING | SPI_SAMPLE_TRAILING | SPI_MODE_MASTER);		
		SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_RISING | SPI_SAMPLE_LEADING | SPI_MODE_MASTER);		//Mode 0,0 is good
		AT45DB321D_Select();
		SPI_SendByte(AT45DB321D_CMD_READ_DEVICE_ID);
		printf("ID[1]: 0x%02X\n", SPI_ReceiveByte());
		printf("ID[2]: 0x%02X\n", SPI_ReceiveByte());
		printf("ID[3]: 0x%02X\n", SPI_ReceiveByte());
		printf("ID[4]: 0x%02X\n", SPI_ReceiveByte());
		AT45DB321D_Deselect();
	}

	return 0;
}

//Pressure sensor functions
static int _F10_Handler (void)
{
	uint16_t A0;
	uint16_t B1;
	uint16_t B2;
	uint16_t C12;
	uint16_t pres;
	uint16_t temp;
	uint8_t RegToRead = argAsInt(1);
	switch(RegToRead)
	{
		case 0:
			MPL115A1_Deselect();
			break;
			
		case 1:
			MPL115A1_Select();
			break;
			
		case 2:
			MPL115A1_GetCalData(&A0, &B1, &B2, &C12);
		
			printf("A0: 0x%04X\n", A0);
			printf("B1: 0x%04X\n", B1);
			printf("B2: 0x%04X\n", B2);
			printf("C12: 0x%04X\n", C12);
		
			/*SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_RISING | SPI_SAMPLE_LEADING | SPI_MODE_MASTER);
		
			MPL115A1_Select();
			
			SPI_SendByte(0x80 | (MPL115AL_REG_CAL_A0_MSB<<1));
			printf("A0: 0x%02X", SPI_ReceiveByte());
			SPI_SendByte(0x80 | (MPL115AL_REG_CAL_A0_LSB<<1));
			printf("%02X\n", SPI_ReceiveByte());
			
			SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B1_MSB<<1));
			printf("B1: 0x%02X", SPI_ReceiveByte());
			SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B1_LSB<<1));
			printf("%02X\n", SPI_ReceiveByte());
			
			SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B2_MSB<<1));
			printf("B2: 0x%02X", SPI_ReceiveByte());
			SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B2_LSB<<1));
			printf("%02X\n", SPI_ReceiveByte());
			
			SPI_SendByte(0x80 | (MPL115AL_REG_CAL_C12_MSB<<1));
			printf("C12: 0x%02X", SPI_ReceiveByte());
			SPI_SendByte(0x80 | (MPL115AL_REG_CAL_C12_LSB<<1));
			printf("%02X\n", SPI_ReceiveByte());
			
			MPL115A1_Deselect();*/
			break;
			
		case 3:
			MPL115A1_GetConversion(&pres, &temp);
			printf("pres: 0x%04X\n", pres);
			printf("temp: 0x%04X\n", temp);
			
			break;
	
		case 4:
			MPL115A1_GetPressure(&pres);
			break;
	
	}
	
	
	return 0;
}

//Get temperatures from the ADC
static int _F11_Handler (void)
{
	uint8_t SendData[3];
	
	/*printf_P(PSTR("Taking measurements...\n"));
	
	//Turn on excitation current to red thermistor
	SendData[0] = (AD7794_IO_DIR_IOUT1 | AD7794_IO_10UA);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);

	//Set up channel 2 (red thermistor)
	//	-Unipolar
	//	-Gain of 2
	//	-Internal 1.17V reference
	//	-Buffered
	SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_2);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN2);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	printf_P(PSTR("Red: %lu counts\n"), AD7794GetData() );
	
	//Turn on excitation current to black thermistor
	SendData[0] = (AD7794_IO_DIR_IOUT2 | AD7794_IO_10UA);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);
	
	//Set up channel 3 (black thermistor)
	//	-Unipolar
	//	-Gain of 2
	//	-Internal 1.17V reference
	//	-Buffered
	SendData[1] = (AD7794_CRH_UNIPOLAR|AD7794_CRH_GAIN_2);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN3);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	printf_P(PSTR("Black: %lu counts\n"), AD7794GetData() );

	//Turn off excitation currents
	SendData[0] = (AD7794_IO_DIR_NORMAL | AD7794_IO_OFF);
	AD7794WriteReg(AD7794_CR_REG_IO, SendData);
	
	//Measure input voltage
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN6);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	printf_P(PSTR("Heater Voltage: %lu counts\n"), AD7794GetData() );
	
	//Measure internal temperature
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_TEMP);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	printf_P(PSTR("Internal Temperature: %lu counts\n"), AD7794GetData() );
	
	//Measure heater current
	SendData[1] = (AD7794_CRH_BIPOLAR|AD7794_CRH_GAIN_1);
	SendData[0] = (AD7794_CRL_REF_INT|AD7794_CRL_REF_DETECT|AD7794_CRL_BUFFER_ON|AD7794_CRL_CHANNEL_AIN1);
	AD7794WriteReg(AD7794_CR_REG_CONFIG, SendData);
	SendData[1] = AD7794_MRH_MODE_SINGLE;
	SendData[0] = (AD7794_MRL_CLK_INT_NOOUT | AD7794_MRL_UPDATE_RATE_10_HZ);
	AD7794WriteReg(AD7794_CR_REG_MODE, SendData);
	AD7794WaitReady();
	printf_P(PSTR("Heater Current: %lu counts\n"), AD7794GetData() );
	
	printf_P(PSTR("waiting for key\n"));
	WaitForAnyKey();
	printf_P(PSTR("done\n"));
	//GetNewCommand();
	//AD7794InternalTempCal(2525);*/
	
	return 0;
}

//Scan the TWI bus for devices
static int _F12_Handler (void)
{
	I2CSoft_Scan();
	return  0;
}

//I think this is handled elsewhere...
/*ISR(USART_RX_vect)
{
	uint8_t c;
	c = UDR0;				//Get char from UART recieve buffer
	CommandGetInput(c);
	//UDR0 = c;				//Send char out on UART transmit buffer
}*/

/** @} */
