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
*	\brief		Hardware definitions and functions header file for the beer heater.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		1/26/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	\defgroup	hardware Hardware Drivers
*
*	@{
*/

#ifndef _HARDWARE_H_
#define _HARDWARE_H_


#define TCA9554A_ADDRESS	0x38


//#include "main.h"

typedef struct 
{
	uint16_t	Ms;
	uint8_t		Sec;
	uint8_t		Min;
	uint8_t		Hr;
	uint16_t	Day;
} thetime;

extern volatile uint8_t ShowTime;
extern uint8_t USB_ISR_SEMAPHORE;


/** initalizes the hardware used for the north pointer
*	- GPIO directions.
*	- Pin change interrupt enabled for A/D data ready pin.
*	- Timer 0 interrupts every 10ms for USB functions.
*	- Analog supplies on.
*	- Gyros powered up and out of test mode.
*/
void HardwareInit( void );






void DelayMS(uint16_t ms);
void DelaySEC(uint16_t SEC);

void GetTime( thetime *time );
void SetTime( thetime time );

/*
void StartTimer(void);
void StopTimer(uint16_t *FinalMS, uint16_t *FinalSEC);
void RestartTimer(uint16_t *FinalMS, uint16_t *FinalSEC);*/

void LED(uint8_t LEDState);

//void WaitForAnyKey(void);
//uint8_t GetKeyPress(void);
//bool AnyKeyPressed(void);

void Beep(uint16_t BeepTimeMS);
void BuzzerOn(void);
void BuzzerOff(void);

void Relay(uint8_t RelayState);
uint8_t GetButtonState( void );


#endif

/** @} */
