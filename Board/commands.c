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

//Humidity sensor functions
static int _F11_Handler (void);
const char _F11_NAME[] PROGMEM 			= "rh";
const char _F11_DESCRIPTION[] PROGMEM 	= "Humidity sensor functions";
const char _F11_HELPTEXT[] PROGMEM 		= "rh <cnd> <val>";

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
	{ _F10_NAME,	0,  0,	_F10_Handler,	_F10_DESCRIPTION,	_F10_HELPTEXT	},		//pres
	{ _F11_NAME,	1,  2,	_F11_Handler,	_F11_DESCRIPTION,	_F11_HELPTEXT	},		//rh
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
	uint8_t stat;
	
	stat = tcs3414_ReadReg(DataToSend, &DataToReceive);
	
	if(stat == 0)
	{
		printf_P(PSTR("reg[0x%02X]: 0x%02X\n"), DataToSend, DataToReceive);
	}
	else
	{
		printf_P(PSTR("Error: 0x%02X\n"), stat);
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
		InitSPIMaster(0,0);		//Mode 0,0 is good
		AT45DB321D_Select();
		SPISendByte(AT45DB321D_CMD_READ_STATUS);
		printf("Stat: 0x%02X\n", SPISendByte(0x00));
		AT45DB321D_Deselect();
	}
	else if(RegToRead == 3)	//Read IDs
	{
		//SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_FALLING | SPI_SAMPLE_TRAILING | SPI_MODE_MASTER);		
		InitSPIMaster(0,0);		//Mode 0,0 is good
		AT45DB321D_Select();
		SPISendByte(AT45DB321D_CMD_READ_DEVICE_ID);
		printf("ID[1]: 0x%02X\n", SPISendByte(0x00));
		printf("ID[2]: 0x%02X\n", SPISendByte(0x00));
		printf("ID[3]: 0x%02X\n", SPISendByte(0x00));
		printf("ID[4]: 0x%02X\n", SPISendByte(0x00));
		AT45DB321D_Deselect();
	}

	return 0;
}

//Pressure sensor functions
static int _F10_Handler (void)
{
	int16_t Pressure_kPa;
	MPL115A1_GetPressure(&Pressure_kPa);
	printf_P(PSTR("Pressure: %u.%u kPa\n"), ((int16_t)Pressure_kPa)>>4, ((((int16_t)Pressure_kPa)&0x000F)*1000)/(16) );
	return 0;
}

//Humidity sensor functions
static int _F11_Handler (void)
{
	uint8_t temp;
	uint8_t stat;
	uint8_t InputCmd	= argAsInt(1);
	uint8_t InputVal	= argAsInt(2);
	int16_t RecievedData;
	
	if(InputCmd == 1)
	{
		stat = SHT25_ReadUserReg(&temp);
		if(stat != SOFT_I2C_STAT_OK)
		{
			printf_P(PSTR("ERROR: 0x%02X\n"), stat);
		}
		else
		{
			printf_P(PSTR("REG: 0x%02X\n"), temp);
		}
		return 0;
	}
	else if(InputCmd == 2)
	{
		printf_P(PSTR("Writing 0x%02X..."), InputVal);
		stat = SHT25_WriteUserReg(InputVal);
		printf_P(PSTR("0x%02X\n"), stat);
		return 0;
	}
	else if(InputCmd == 3)
	{
		printf_P(PSTR("Device reset\n"));
		SHT25_Reset();
	}
	else if(InputCmd == 4)
	{
		stat = SHT25_ReadTemp(&RecievedData);
		if(stat == SHT25_RETURN_STATUS_OK)
		{
			printf_P(PSTR("Temp %d.%02u C\n"), RecievedData/100, RecievedData%100);
		}
		else if(stat == SHT25_RETURN_STATUS_CRC_ERROR)
		{
			printf_P(PSTR("CRC Error\n"));
		}
		else
		{
			printf_P(PSTR("Timeout\n"));
		}
	}
	else if(InputCmd == 5)
	{
		stat = SHT25_ReadRH(&RecievedData);
		if(stat == SHT25_RETURN_STATUS_OK)
		{
			printf_P(PSTR("RH: %u.%02u%%\n"), RecievedData/100, RecievedData%100);
		}
		else if(stat == SHT25_RETURN_STATUS_CRC_ERROR)
		{
			printf_P(PSTR("CRC Error\n"));
		}
		else
		{
			printf_P(PSTR("Timeout\n"));
		}
	}
	
	
	
	return 0;
}

//Scan the TWI bus for devices
static int _F12_Handler (void)
{
	I2CSoft_Scan();
	return  0;
}

/** @} */
