#include <reg52.h>

extern unsigned int smk;
extern unsigned int tmp;
extern unsigned char RxdBuf;
unsigned char *ptrTxd;
unsigned char cntTxd = 0;
unsigned char NowData[] = {'T','e','m','p','e','t','u','r','e',':','0','0','.','0',
 												' ',' ','S','m','o','k','e',':','0','0','\n'};
void configUART(unsigned int);
												
void sendData()
{
	NowData[10] = tmp/100 + 48;
	NowData[11] = tmp%100/10 + 48;
	NowData[13] = tmp%10 + 48;
	NowData[22] = smk/10 + 48;
	NowData[23] = smk%10 + 48;
	
	ptrTxd = NowData;
	cntTxd = sizeof(NowData);
	TI = 1;
	
}


void configUART(unsigned int baud)
{
	SCON = 0x50;
	TMOD &= 0x0F;
	TMOD |= 0x20;
	TH1 = 256 - (11059200/12/32)/baud;
	TL1 = TH1;
	ET1 = 0;
	ES = 1;
	TR1 = 1;
}

void interruptUART() interrupt 4
{
	if(RI)
	{
		RI = 0;
		RxdBuf = SBUF;
	}
	if(TI)
	{
		TI = 0;
		if(cntTxd > 0)
		{
			SBUF = *ptrTxd;
			cntTxd--;
			ptrTxd++;
	  }
  }
}