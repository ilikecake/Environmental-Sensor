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
*	\brief		Header file for the datalogger.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/17/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	Datalogger
*
*	@{
*/

#ifndef _DATALOGGER_H_
#define _DATALOGGER_H_

#include "stdint.h"


#define DATALOGGER_PAGE_SIZE			528		//This should be the same as the dataflash page size.
#define DATALOGGER_DATASET_SIZE			18
#define DATALOGGER_USE_CRC				0



#define DATALOGGER_FIND_RESET_POINTERS		0x01	//Tells the 'Datalogger_FindLastDataSet' function to reset the pointers after identifying the last data set
#define DATALOGGER_FIND_ERASE_DATA			0x02	//Tells the 'Datalogger_FindLastDataSet' function to erase the data that it finds
#define DATALOGGER_FIND_ASSUME_DATA_SIZE	0x04	//Tells the 'Datalogger_FindLastDataSet' function to assume the current data set size for all found data (not recommended)


#define DATALOGGER_HEADER1_PREFIX	0xA0
#define DATALOGGER_HEADER2_SUFFIX	0x00
//TODO: Add exclude sectors

/*typedef struct 
{
	uint8_t Day;
	uint8_t Hour;
	uint8_t Min;
	uint8_t Sec;
	uint16_t Temp;
	uint16_t Humidity;
	uint16_t Pressure;
	uint16_t RedLightLevel;
	uint16_t GreenLightLevel;
	uint16_t BlueLightLevel;
	uint16_t ClearLightLevel;
} DataSetStruct;*/

/** Look for the last data set written in flash to continue writing.
 *  Find the size of the dataset and save it to a global variable
 *	Also calculate how many datasets we can store in a page
 */
void Datalogger_Init(void);

/** Add a set of data to be saved. This function will automatically write the data to flash when a page gets full.*/
void Datalogger_AddDataSet(uint8_t DataSet[]);

/** Save a partial set of data to flash. Call this if the controller needs to be reset. */
void Datalogger_SaveDataToFlash(void);

/** Locate the last set of data written to flash */
void Datalogger_FindLastDataSet(uint8_t CommandSet);

//how to do this?
void Datalogger_RetrieveDataFromFlash(uint8_t DataSet[]);






#endif
/** @} */
