#include <reg52.h>

sbit BUZZ = P2^5;

unsigned char T1RH = 0;
unsigned char T1RL = 0;

void openBuzzer(unsigned int frequ)
{
	unsigned int reload;
	reload = 65536 - (11059200/12)/(frequ*2);
	RCAP2H = (unsigned char)(reload>>8);
	RCAP2L = (unsigned char)reload;
	
	ET2 = 1;
	TR2 = 1;
}

void stopBuzzer()
{
	ET2 = 0;
	TR2 = 0;
	BUZZ = 1;
}

void interruptTimer2() interrupt 5
{

	TF2 = 0;
	BUZZ = ~BUZZ;
}