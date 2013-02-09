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
*	\brief		MPL115A1 digital barometer driver.
*	\author		Pat Satyshur
*	\version	1.0
*	\date		2/8/2013
*	\copyright	Copyright 2013, Pat Satyshur
*	\ingroup 	hardware
*
*	@{
*/

#include "main.h"

void MPL115A1_Init(void)
{
	MPL115A1_Sleep(0);
	MPL115A1_Deselect();
	return;
}

void MPL115A1_Sleep(uint8_t ToSleep)
{
	if(ToSleep == 1)
	{
		PORTB &= ~(1<<6);
	}
	else
	{
		PORTB |= (1<<6);
	}
	return;
}

void MPL115A1_Select(void)
{
	PORTB &= ~(1<<4);
	return;
}

void MPL115A1_Deselect(void)
{
	PORTB |= 1<<4;
	return;
}

void MPL115A1_GetCalData(uint16_t *A0, uint16_t *B1, uint16_t *B2, uint16_t *C12)
{
	uint8_t tempData;
	
	//TODO: remove this line later?
	SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_RISING | SPI_SAMPLE_LEADING | SPI_MODE_MASTER);
		
	MPL115A1_Select();
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_A0_MSB<<1));
	*A0 = (SPI_ReceiveByte() << 8);
	//printf("A0: 0x%02X", SPI_ReceiveByte());
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_A0_LSB<<1));
	*A0 |= SPI_ReceiveByte();
	//printf("%02X\n", SPI_ReceiveByte());
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B1_MSB<<1));
	*B1 = (SPI_ReceiveByte() << 8);
	//printf("B1: 0x%02X", SPI_ReceiveByte());
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B1_LSB<<1));
	*B1 |= SPI_ReceiveByte();
	//printf("%02X\n", SPI_ReceiveByte());
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B2_MSB<<1));
	*B2 = (SPI_ReceiveByte() << 8);
	//printf("B2: 0x%02X", SPI_ReceiveByte());
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_B2_LSB<<1));
	*B2 |= SPI_ReceiveByte();
	//printf("%02X\n", SPI_ReceiveByte());
	
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_C12_MSB<<1));
	*C12 = (SPI_ReceiveByte() << 8);
	//printf("C12: 0x%02X", SPI_ReceiveByte());
	SPI_SendByte(0x80 | (MPL115AL_REG_CAL_C12_LSB<<1));
	*C12 |= SPI_ReceiveByte();
	//printf("%02X\n", SPI_ReceiveByte());
	
	MPL115A1_Deselect();
	return;
}

void MPL115A1_GetConversion(uint16_t *PressureData, uint16_t *TemperatureData)
{
	SPI_Init(SPI_SPEED_FCPU_DIV_2 | SPI_ORDER_MSB_FIRST | SPI_SCK_LEAD_RISING | SPI_SAMPLE_LEADING | SPI_MODE_MASTER);
	
	//Start conversions
	MPL115A1_Select();
	SPI_SendByte(MPL115AL_REG_CONVERT<<1);
	SPI_SendByte(0x00);
	MPL115A1_Deselect();
	
	DelayMS(4);
	
	MPL115A1_Select();
	SPI_SendByte(0x80 | (MPL115AL_REG_PRESSURE_MSB << 1));
	*PressureData = (SPI_ReceiveByte() << 8);
	
	SPI_SendByte(0x80 | (MPL115AL_REG_PRESSURE_LSB << 1));
	*PressureData |= (SPI_ReceiveByte());
	
	SPI_SendByte(0x80 | (MPL115AL_REG_TEMP_MSB << 1));
	*TemperatureData = (SPI_ReceiveByte() << 8);
	
	SPI_SendByte(0x80 | (MPL115AL_REG_TEMP_LSB << 1));
	*TemperatureData |= (SPI_ReceiveByte());
	
	MPL115A1_Deselect();
	
	//Values are 10 bit number, left aligned.
	*PressureData = *PressureData >> 6;
	*TemperatureData = *TemperatureData >> 6;
	
	return;
}

/** Calculate the Temperature compensated barometric pressure. The temperature compensation is performed using 64-bit numbers and the formula \f$ \frac{2^{21}*A_{0} + 2^{11}*B_{1}*P+C_{12}*T*P+2^{10}B_{2}*T}{2^{24}} \f$
*	See the MPL115A1 data sheet for details.
*/
void MPL115A1_GetPressure(uint16_t *Pressure_kPa)
{
	uint16_t A0;
	uint16_t B1;
	uint16_t B2;
	uint16_t C12;
	uint16_t pres;
	uint16_t temp;
	
	uint16_t out1;
	
	//To prevent integer rounding errors, we add up the numerator and denominator seperately, and divide them at the end.
	int64_t numerator;
	int64_t denominator;

	denominator = 16777216ll;
	MPL115A1_GetCalData(&A0, &B1, &B2, &C12);
	MPL115A1_GetConversion(&pres, &temp);
	
	printf("A0: 0x%04X\n", A0);
	printf("B1: 0x%04X\n", B1);
	printf("B2: 0x%04X\n", B2);
	printf("C12: 0x%04X\n", C12);
	printf("pres: 0x%04X\n", pres);
	printf("temp: 0x%04X\n", temp);
	
	//Correct for 2's compliment
	if((A0 & 0x8000) == 0x8000)
	{
		printf("A0-\n");
		numerator = -1*(65536l-A0)*(2097152l);
	}
	else
	{
		printf("A0+\n");
		numerator = (A0)*(2097152l);
	}

	if((B1 & 0x8000) == 0x8000)
	{
		printf("B1-\n");
		numerator = numerator - ((65536l-B1)*(2048)*pres);
	}
	else
	{
		printf("B1+\n");
		numerator = numerator + ((B1)*(2048)*pres);
	}

	if((C12 & 0x8000) == 0x8000)
	{
		printf("C12-\n");
		numerator = numerator - ((65536l-C12)*temp*pres);
	}
	else
	{
		printf("C12+\n");
		numerator = numerator + (C12 * temp * pres);
	}
	
	if((B2 & 0x8000) == 0x8000)
	{
		printf("B2-\n");
		numerator = numerator - ((65536l-B2)*(1024)*temp);
	}
	else
	{
		printf("B2+\n");
		numerator = numerator + (B2*1024*temp);
	}
	
	out1 = (uint16_t)(numerator/denominator);
	
	printf("Output: %u\n", out1);


	return;
}

/** @} */