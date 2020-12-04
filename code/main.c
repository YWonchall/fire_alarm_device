#include <reg52.h>

sbit LED1 = P2^1;
sbit LED2 = P2^2;

bit flagOff = 0;
unsigned char flagOn = 0;
unsigned int preTmp1 = 40;
unsigned int preSmk1 = 20;
unsigned int preTmp2 = 60;
unsigned int preSmk2 = 30;
unsigned int bkp[2];
unsigned int smk;
unsigned int tmp;
unsigned int sum;
unsigned char RxdBuf = '4';
unsigned char flag = 0;
unsigned char cnt;
unsigned char T0RH, T0RL;

extern unsigned int getTmp();
extern unsigned int ADC0832(unsigned char);
extern void sendData();
extern void openBuzzer(unsigned int);
extern void stopBuzzer();
extern void Motor(unsigned char);
extern void InitDS18B20();
extern void Display1602(unsigned int,unsigned int,unsigned int,unsigned int);	//œ‘ æŒ¬∂»£¨—ÃŒÌ÷µ
extern void Init1602();
extern void configUART(unsigned int);
void checkSta();

void main()
{
	unsigned int i = 0;
	unsigned int j = 0;
	EA = 1;
	InitDS18B20();
	Init1602();
	configUART(9600);
	bkp[0] = getTmp();
	bkp[1] = ADC0832(1);
	for(j=0;j<33000;j++);
	while(1)						
	{
		for(cnt=0;cnt<50;cnt++)			
		{
			smk = ADC0832(1);	
			sum=sum+smk;			
		}
		smk=sum/50;				
		sum=0;
		tmp = getTmp();
		if((bkp[0]!=tmp/10)||(bkp[1]!=smk))
		{
			bkp[0] = tmp/10;
			bkp[1] = smk;
			sendData();
		}
		checkSta();
		switch(flag)
		{
			case 0:
			{
				Display1602(preTmp1, preSmk1, tmp, smk);
				stopBuzzer();
				LED1 = 1;
				LED2 = 1;
				Motor(0);
				break;
			}
			case 1:
			{
				openBuzzer(4000);
				LED1 = 0;
				LED2 = 1;
				Motor(1);
				Display1602(preTmp2, preSmk2, tmp, smk);
				break;
			}
			case 2:
			{
				openBuzzer(4000);
				LED2 = 0;
				LED1 = 1;
				Motor(2);
				Display1602(preTmp2, preSmk2, tmp, smk);
				break;
			}
			default:
				break;
		}	
		for(i=0;i<20000;i++);
	}
}

void checkSta()
{
	switch(RxdBuf)
	{
		case '0': flagOff = 1;flagOn = 0; break;
		case '1': flagOn = 1; break;
		case '2': flagOn = 2; break;
		case '3': flagOff = 0; flagOn = 0; break;
		default: break;
	}
	if(((((tmp/10)>=preTmp1)||(smk>=preSmk1))&&flagOff==0)||flagOn>0)
	{
		if(((tmp/10)>=preTmp2||smk>=preSmk2)||flagOn==2)
			flag = 2;
		else
			flag = 1;
	}
	else
		flag = 0;
}


