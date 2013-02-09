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
*	\brief		Header file for the AT45DB321D dataflash driver.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/8/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#ifndef _AT45DB321D_H_
#define _AT45DB321D_H_

#include "stdint.h"

#define AT45DB321D_CMD_ARRAY_READ_LEGACY		0xE8
#define AT45DB321D_CMD_ARRAY_READ_HF			0x0B
#define AT45DB321D_CMD_ARRAY_READ_LF			0x03
#define AT45DB321D_CMD_PAGE_READ				0xD2
#define AT45DB321D_CMD_BUFFER1_READ_HS			0xD4
#define AT45DB321D_CMD_BUFFER1_READ_LS			0xD1
#define AT45DB321D_CMD_BUFFER2_READ_HS			0xD6
#define AT45DB321D_CMD_BUFFER2_READ_LS			0xD3

#define AT45DB321D_CMD_MEMORY_TO BUFFER1_WRITE		0x53
#define AT45DB321D_CMD_MEMORY_TO_BUFFER2_WRITE		0x55
#define AT45DB321D_CMD_MEMORY_TO_BUFFER1_COMPARE	0x60
#define AT45DB321D_CMD_MEMORY_TO_BUFFER2_COMPARE	0x61
#define AT45DB321D_CMD_PAGE_REWRITE_BUFFER1			0x58
#define AT45DB321D_CMD_PAGE_REWRITE_BUFFER2			0x59
#define AT45DB321D_CMD_READ_STATUS					0xD7
#define AT45DB321D_CMD_READ_DEVICE_ID				0x9F

#define AT45DB321D_CMD_POWERDOWN					0xB9
#define AT45DB321D_CMD_POWERUP						0xAB

#define AT45DB321D_CMD_BUFFER1_WRITE			0x84
#define AT45DB321D_CMD_BUFFER2_WRITE			0x87
#define AT45DB321D_CMD_BUFFER1_TO_MAIN_ERASE	0x83
#define AT45DB321D_CMD_BUFFER2_TO_MAIN_ERASE	0x86
#define AT45DB321D_CMD_BUFFER1_TO_MAIN_NOERASE	0x88
#define AT45DB321D_CMD_BUFFER2_TO_MAIN_NOERASE	0x89
#define AT45DB321D_CMD_PAGE_ERASE				0x81
#define AT45DB321D_CMD_BLOCK_ERASE				0x50
#define AT45DB321D_CMD_SECTOR_ERASE				0x7C
#define AT45DB321D_CMD_PAGE_PROGRAM_BUFFER1		0x82
#define AT45DB321D_CMD_PAGE_PROGRAM_BUFFER2		0x85

#define AT45DB321D_CMD_READ_PROTECTED_SECTORS	0x32
#define AT45DB321D_CMD_READ_LOCKED_SECTORS		0x35
#define AT45DB321D_CMD_READ_SECURITY_REG		0x77

//All four of these need to be sent to erase the chip
#define AT45DB321D_CMD_CHIP_ERASE1				0xC7
#define AT45DB321D_CMD_CHIP_ERASE2				0x94
#define AT45DB321D_CMD_CHIP_ERASE3				0x80
#define AT45DB321D_CMD_CHIP_ERASE4				0x9A

void AT45DB321D_Init(void);

void AT45DB321D_Select(void);
void AT45DB321D_Deselect(void);

void AT45DB321D_Protect(void);
void AT45DB321D_Unprotect(void);

void AT45DB321D_ReadProtectedSectors(uint8_t *ProtectData);
void AT45DB321D_ProtectSectors(uint8_t *ProtectData);

void AT45DB321D_ReadLockedSectors(uint8_t *ProtectData);
void AT45DB321D_LockSectors(uint8_t *ProtectData);			//I probably will not implement this

void AT45DB321D_ReadSecurityRegister(uint8_t *SecurityReg);
void AT45DB321D_WriteSecurityRegister(uint8_t *SecurityReg);

void AT45DB321D_Powerdown(void);
void AT45DB321D_Powerup(void);

void AT45DB321D_SwitchTo512(void);	//This only needs to happen once, it cannot be undone.

#endif
/** @} */
