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


// FM1216.h

#ifndef _FM1216_h
#define _FM1216_h

#include <Arduino.h>
//#include <Wire/Wire.h>


//Config Soft I2C Library
#define SDA_PORT	PORTC
#define SDA_PIN		4
#define SCL_PORT	PORTC
#define SCL_PIN		5
#define I2C_FASTMODE 1

//Adresses Write (Pin6=GND)
#define TUNER	0xC0	
#define IF		0x86

//Adresses Read
#define TUNER_R	0xC1
#define IF_R	0x87

//Charge Pump Currents
#define CP_280 1		//280uA for fast tuning
#define CP_60  0		//60uA recommended for FM Mode

//Step Sizes
#define Step_50		0	//50kHz
#define Step_31		1	//31.25kHz 
#define Step_166	2	//166.7kHz
#define Step_62		3	//62.5kHz

//Bands
#define BAND_L 0		//Low Band 48.25-160MHz FM-Band: 87.5-108.00MHz
#define BAND_M 1		//Mid Band 160-442MHz 
#define BAND_H 2		//High Band 442-863.25MHz 

//AGC Take-Over-Point Voltage (in dbï¿½V)
#define AGC_115		0
#define AGC_115_2	1
#define AGC_112		2	//default mode at POR
#define AGC_109		3	//Recommended for negative modulation
#define AGC_106		4	//Recommended for positive modulation
#define AGC_103		5
#define AGC_EXT		6	//External AGC 
#define AGC_OFF		7	//disable AGC
	
#endif

class FM1216
{
private:
	uint8_t DB1, DB2, CB, BB, AB;	//Tuner Registers
	//uint8_t SB;						//Statusbyte Tuner
	uint8_t BD, CD, ED;				//IF Registers
	//uint8_t DD;						//Data Byte
	uint16_t StepHz;
	uint16_t IFFreq;
	uint32_t Freq;


public:
	void init();
	void update();
	void setFreq(uint32_t kHzFreq); //set Frequency in kHz
	uint32_t getFreq();				//get Frequency in kHz
	void setStereo(boolean b);		//true=Stereo false=mono
	void setHighSens(boolean b);	//High Sensitivity
	void ForceMute(boolean b);		//Forced Mute Audio
	void setChargePump(uint8_t u);	//ChargePump current
	void disablePLL(boolean b);
	void setStepSize(uint8_t u);	//sets Tuning Step-Size (see defines Step_xxx)
	void setBand(uint8_t band);		//set Band
	void setAGC(uint8_t agc);		//set AGC Take-Over Voltage
	void setGain(boolean g);		//activate Audio Gain
	

};

