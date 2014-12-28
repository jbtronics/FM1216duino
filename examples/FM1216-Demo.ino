
#include "FM1216.h"


FM1216 tuner;

void setup()
{
	tuner.init();
	tuner.setFreq(89300);
	Serial.begin(19200);
}

void loop()
{
	
	unsigned long n = Serial.parseInt();
	if (n)
	{
		tuner.setFreq(n);
		Serial.print("OK: ");
		Serial.println(n);
	}
	
}
