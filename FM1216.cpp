/*
FM1216-Library controls a  FM1216ME Tuner over I2C
Copyright(C) Dec14    Jan Böhmer

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/


#include "FM1216.h"
#include "SoftI2CMaster.h"

void FM1216::init()
{
	i2c_init();
	//Wire.begin();

	//Set Tuner to a high TV-Freq
	DB1= 0x1F;
	DB2= 0xE2;
	CB = 0x86;
	BB = 0x44;
	AB = 0xA0;

	delay(10);
	

	//Activate FM-Mode
	BD = 0x4C;
	CD = 0x10;
	ED = 0x40;
	AB = 00;
	CB = 0x88;
	BB = 0x59;

	IFFreq = 10700; // default 10,7MHz
	StepHz = 50000;   //default 50KHz Step-Size

	update();
}

void FM1216::setStereo(boolean b)
{
	bitWrite(BB, 6, !b);
	update();
}

void FM1216::setHighSens(boolean b)
{
	bitWrite(BD, 6, b);
	update();
}

void FM1216::ForceMute(boolean b)
{
	bitWrite(BD, 5, b);
	update();
}

void FM1216::setChargePump(uint8_t u)
{
	bitWrite(CB, 6, u);
	update();
}

void FM1216::disablePLL(boolean b)
{
	bitWrite(CB, 0, b);
	update();
}

void FM1216::setStepSize(uint8_t step)
{
	switch(step)
	{
	case Step_50:
		StepHz = 50000;
		bitClear(CB, 1);
		bitClear(CB, 2);
		break;
	case Step_31:
		StepHz = 31250;
		bitSet(CB, 1);
		bitClear(CB, 2);
		break;
	case  Step_166:
		StepHz = 166700;
		bitClear(CB, 1);
		bitSet(CB, 2);
		break;
	case Step_62:
		StepHz = 62500;
		bitSet(CB, 1);
		bitSet(CB, 2);
		break;
	}
	update();
}

void FM1216::setBand(uint8_t band)
{
	
	switch (band){
	case BAND_L:
		bitSet(BB, 0);
		bitClear(BB, 1);
		bitClear(BB, 2);
		break;
	case BAND_M:
		bitClear(BB, 0);
		bitSet(BB, 1);
		bitClear(BB, 2);
		break;
	case BAND_H:
		bitClear(BB, 0);
		bitClear(BB, 1);
		bitSet(BB, 2);
		break;
	}
	update();
}

void FM1216::setAGC(uint8_t agc)
{
	bitWrite(AB, 4, bitRead(agc, 0));
	bitWrite(AB, 5, bitRead(agc, 1));
	bitWrite(AB, 6, bitRead(agc, 2));
	update();
}

void FM1216::setFreq(uint32_t kHzFreq)
{
	Freq = kHzFreq;
	if (kHzFreq >= 442000)
	{
		setBand(BAND_H);
	}
	else if (kHzFreq >= 160000 && kHzFreq < 442000)
	{
		setBand(BAND_M);
	}
	else
	{
		setBand(BAND_L);
	}
	kHzFreq = kHzFreq + (uint16_t) IFFreq;
	kHzFreq = kHzFreq * 1000;
	kHzFreq = kHzFreq / StepHz;
	DB1 = highByte(kHzFreq);
	DB2 = lowByte(kHzFreq);	
	
	update();

}

void FM1216::setGain(boolean g)
{
	bitWrite(CD, 7,g);
	update();
}

uint32_t FM1216::getFreq()
{
	return Freq;
}

void FM1216::update()
{
	
	i2c_start(TUNER);
	i2c_write(DB1);
	i2c_write(DB2);
	i2c_write(CB);
	i2c_write(BB);
	i2c_write(AB);
	i2c_stop();

	i2c_start(IF);
	i2c_write(0x00);
	i2c_write(BD);
	i2c_write(CD);
	i2c_write(ED);
	i2c_stop();
	

	/*
	Wire.beginTransmission(TUNER);
	Wire.write(DB1);
	Wire.write(DB2);
	Wire.write(CB);
	Wire.write(BB);
	Wire.write(AB);
	Wire.endTransmission();

	Wire.beginTransmission(IF);
	Wire.write(0x00);
	Wire.write(BD);
	Wire.write(CD);
	Wire.write(ED);
	Wire.endTransmission();
	*/
}