#include <reg52.h>

sbit MOTOR = P3^5;

unsigned char HighRH, HighRL, LowRH, LowRL;

void configPWM(unsigned int fre, unsigned char dc)
{
	unsigned high, low;
	unsigned long tmp;
	
	tmp = (11059200/12)/fre;
	high = (tmp*dc)/100;
	low = tmp - high;
	high = 65536 - high +12;
	low = 65536 - low + 12;
	HighRH = (unsigned char)(high>>8);
	HighRL = (unsigned char)(high);
	LowRH = (unsigned char)(low>>8);
	LowRL = (unsigned char)(low);
	
	TMOD &= 0xF0;
	TMOD |= 0x01;
	TH0 = HighRH;
	TL0 = HighRL;
	ET0 = 1;
	TR0 = 1;
}

void closePWM()
{
	ET0 = 0;
	TR0 = 0;
	MOTOR = 1;
}
void Motor(unsigned char sta)
{
	switch(sta)
	{
		case 0: {closePWM(); break;}
		case 1:	{configPWM(200, 40); break;}
		case 2: {closePWM(); MOTOR = 0; break;}
		default: break;
	}
}


void interruptTimer0() interrupt 1 
{
	if(MOTOR == 1)
	{
		TH0 = LowRH;
		TL0 = LowRL;
		MOTOR = 0;
	}
	else
	{
		TH0 = HighRH;
		TL0 = HighRL;
		MOTOR = 1;
	}
}
