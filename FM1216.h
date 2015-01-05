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

typedef void(*voidFuncPtr)(void);

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

//Power on Reset Status
#define POR_Tuner	1	//Tuner Power-on-Reset flag =1
#define POR_InF		2	//IF Power-on-Reset
#define POR_Both	3	//Both's Power-on-Reset flag=1

//AFC-Status
#define AFC_m187		7	//F0-187.5kHz
#define AFC_m162		6	//F0-162.5kHz
#define AFC_m137		5	//F0-137.5kHz
#define AFC_m112		4	//F0-112.5kHz
#define AFC_m87			3	//F0-87.5kHz
#define AFC_m62			2	//F0-62.5kHz
#define AFC_m37			1	//F0-37.5kHz
#define AFC_m12			0	//F0-12.5kHz
#define AFC_p12			15	//F0+12.5kHz
#define AFC_p37			14	//F0+37.5kHz
#define	AFC_p62			13	//F0+62.5kHz
#define AFC_p87			12	//F0+87.5kHz
#define AFC_p112		11	//F0+112.5kHz
#define AFC_p137		10	//F0+137.5kHz
#define AFC_p162		9	//F0+162.5kHz
#define AFC_p187		8	//F0+187.5kHz

//internal bitnames
//Tunerregister
//Controlbyte
#define CP			6	//Chargepump
#define T2			5	//Testmodebit 1
#define T1			4	//2
#define T0			3	//3
#define RSA			2	//Stepsize A
#define RSB			1	
#define OS			0	//PLL deactivate

//Bandswitchbyte
#define P6			6	//Stereo
#define P4			4	//FM
#define P3			3	//FM
#define P2			2	//Band H
#define P1			1	//Band M
#define P0			0	//Band L

//Auxiliarybyte
#define ATC			7	//AGC Timeconstant
#define AL2			6	//AGC Takeover
#define AL1			5	//AGC Takeover
#define AL0			4	//AGC Takeover

//Statusbyte
#define POR			7	//Power on Reset
#define FL			6	//Frequency Lock
#define AGC			3	//AGC active
#define SA2			2	//FM Transmitted in Stereo?
#define SA1			1	//FM Transmitted in Stereo?
#define SA0			0	//FM Transmitted in Stereo?

//IF-Register
//B Data
#define B7			7	//L' Sound Switch
#define B6			6	//FM High-Sens
#define B5			5	//Force Audio Mute
#define B4			4	//FM-Radio
#define B3			3	//FM-Radio
#define B2			2	//QSS Mode
#define B1			1	//no Mute of FM AF OUTPUT
#define B0			0	//Sound Trap

//C Data
#define C7			7	//0dB Audio Gain
#define C6			6	//De-emphasis OFF
#define C5			5	//De-emphasis OFF

//E Data
#define E7			7	//VIF AGC OUTPUT
#define E6			6	//L Standard PLL Gating
#define E5			5	//IF Gain normal
#define E4			4	//Video IF Frequency
#define E3			3	//Video IF Frequency
#define E2			2	//Video IF Frequency
#define E1			1	//TV Sound Carrier Frequency
#define E0			0	//TV Sound Carrier Frequency

//D Data
#define AFCWIN		7	//Freq inside AFC window
#define VIFL		6	//Video IF Level High=1
#define FMIFL		5	//FM IF Level High?
#define AFC4		4	//AFC status
#define AFC3		3	//AFC status
#define AFC2		2	//AFC status
#define AFC1		1	//AFC status
#define PONR		0	//Power-on reset?

#endif

class FM1216
{
private:
	uint8_t DB1, DB2, CB, BB, AB;	//Tuner Registers
	uint8_t SB;						//Statusbyte Tuner
	uint8_t BD, CD, ED;				//IF Registers
	uint8_t DD;						//Data Byte
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
	boolean getStereo();			//returns true if Stereo
	void getStatus();				//Gets Statusbytes of Tuner and IF
	boolean isStereoTrans();		//returns true if Stereo is transmitted 
	uint8_t getPowerOnReset();		//returns Power on Reset status of IF and Tuner
	boolean getFreqLocked();		//returns true if PLL-Ref is locked for 1ms
	boolean isAGC();				//returns true if AGC is active
	boolean isFmHigh();				//returns true if FM-IF is high
	boolean isVHigh();				//returns true if Video-IF is high
	uint8_t getAFCStatus();			//returns the difference bettween nominal IF and AFC IF(see defines AFC_xxxx)
	boolean isFinAFC();				//returns true if the IF-Freq is in the AFC-window
	void scan(boolean up);			//scan and tune to the next Station
	void scan(boolean up, voidFuncPtr print); //like scan. Calls the function between scan cycles
	

	void Serialdebug();				//Prints Registervalue over Serial
};

