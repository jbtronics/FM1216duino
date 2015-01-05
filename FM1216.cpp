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
	bitWrite(BB, P6, !b);
	update();
}

void FM1216::setHighSens(boolean b)
{
	bitWrite(BD, B6, b);
	update();
}

void FM1216::ForceMute(boolean b)
{
	bitWrite(BD, B5, b);
	update();
}

void FM1216::setChargePump(uint8_t u)
{
	bitWrite(CB, CP, u);
	update();
}

void FM1216::disablePLL(boolean b)
{
	bitWrite(CB, OS, b);
	update();
}

void FM1216::setStepSize(uint8_t step)
{
	switch(step)
	{
	case Step_50:
		StepHz = 50000;
		bitClear(CB, RSB);
		bitClear(CB, RSA);
		break;
	case Step_31:
		StepHz = 31250;
		bitSet(CB, RSB);
		bitClear(CB, RSA);
		break;
	case  Step_166:
		StepHz = 166700;
		bitClear(CB, RSB);
		bitSet(CB, RSA);
		break;
	case Step_62:
		StepHz = 62500;
		bitSet(CB, RSB);
		bitSet(CB, RSA);
		break;
	}
	update();
}

void FM1216::setBand(uint8_t band)
{
	
	switch (band){
	case BAND_L:
		bitSet(BB, P0);
		bitClear(BB, P1);
		bitClear(BB, P2);
		break;
	case BAND_M:
		bitClear(BB, P0);
		bitSet(BB, P1);
		bitClear(BB, P2);
		break;
	case BAND_H:
		bitClear(BB, P0);
		bitClear(BB, P1);
		bitSet(BB, P2);
		break;
	}
	update();
}

void FM1216::setAGC(uint8_t agc)
{
	bitWrite(AB, AL0, bitRead(agc, 0));
	bitWrite(AB, AL1, bitRead(agc, 1));
	bitWrite(AB, AL2, bitRead(agc, 2));
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
	bitWrite(CD, C7,g);
	update();
}

uint32_t FM1216::getFreq()
{
	return Freq;
}

boolean FM1216::getStereo()
{
	return !bitRead(BB, P6);
}

void FM1216::getStatus()
{
	i2c_rep_start(TUNER_R);
	SB = i2c_read(false);
	i2c_stop();
	
	i2c_rep_start(IF_R);
	DD = i2c_read(true);
	i2c_stop();
}

boolean FM1216::isStereoTrans()
{
	getStatus();
	if (bitRead(SB, SA2) && !bitRead(SB, SA1) && !bitRead(SB, SA0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

uint8_t FM1216::getPowerOnReset()
{
	uint8_t tmp = 0;
	bitWrite(tmp, 0, bitRead(SB, POR));
	bitWrite(tmp, 1, bitRead(DD, PONR));
	return tmp;
}

boolean FM1216::getFreqLocked()
{
	getStatus();
	return bitRead(SB, FL);
}

boolean FM1216::isAGC()
{
	getStatus();
	return bitRead(SB, AGC);
}

boolean FM1216::isFmHigh()
{
	getStatus();
	return bitRead(DD, FMIFL);
}

boolean FM1216::isVHigh()
{
	getStatus();
	return bitRead(DD, VIFL);
}

uint8_t FM1216::getAFCStatus()
{
	uint8_t tmp;
	bitWrite(tmp, 0, bitRead(DD, AFC1));
	bitWrite(tmp, 1, bitRead(DD, AFC2));
	bitWrite(tmp, 2, bitRead(DD, AFC3));
	bitWrite(tmp, 3, bitRead(DD, AFC4));
}

boolean FM1216::isFinAFC()
{
	return bitRead(DD, AFCWIN);
}

void FM1216::scan(boolean up, voidFuncPtr func)
{
	uint8_t bak = BD;
	setHighSens(false);
	ForceMute(true);
	/*
	if (up)
	{
		setFreq(getFreq() + 400 );
	}
	else
	{
		setFreq(getFreq() - 400);
	}
	*/
	
	while(isFmHigh()||isVHigh())
	{
		if (up)
		{
			setFreq(getFreq() + 50);
		}
		else
		{ 
			setFreq(getFreq() - 50);
		}
		func();
		delay(25);
	}
	
	while (!isFmHigh()&&!isVHigh())
	{
		if (up)
		{
			setFreq(getFreq() + 50);
		}
		else
		{
			setFreq(getFreq() - 50);
		}
		func();
		delay(25);
	}
	BD = bak;
	ForceMute(false);
}


void FM1216::Serialdebug()
{
	Serial.println("Debug FM1216ME-Tuner");
	Serial.println("--------------------");
	Serial.print("DB1= ");
	Serial.println(DB1);
	Serial.print("DB2= ");
	Serial.println(DB2);
	Serial.print("CB= ");
	Serial.println(CB);
	Serial.print("BB= ");
	Serial.println(BB);
	Serial.print("AB= ");
	Serial.println(AB);
	Serial.print("SB= ");
	Serial.println(SB);
	Serial.println("");
	Serial.println("");
	//IF-Bytes
	Serial.print("BD= ");
	Serial.println(BD);
	Serial.print("CD= ");
	Serial.println(CD);
	Serial.print("ED= ");
	Serial.println(ED);
	Serial.print("DD= ");
	Serial.println(DD);
	Serial.println("---------------");

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