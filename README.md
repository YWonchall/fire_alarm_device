# 基于51单片机的火灾报警装置

> 这应该算是我本科阶段做的第一个项目，入门单片机的时候拿51做的一个简单的火灾报警装置，由于年代久远，很多细节可能都忘了，不过有什么问题欢迎大家留言，看到后还是会尽力帮大家解决。

## 写在前面

这个装置用的是最简单的51单片机外加几个简单的传感器，项目大致流程就是：

收集传感器信息 —> 信息处理 —> 执行对应动作

## 目录

- 火灾报警装置功能介绍
- 原理图
- 源代码

## 功能介绍

### 总览



- 装置的总览图， 各种零件标注如下：
  - 1、温度阈值
  - 2、烟雾浓度阈值
  - 3、实时温度
  - 4、实时烟雾浓度
  - 5、蜂鸣器用于报警
  - 6、黄、红色LED灯指示
  - 7、风扇用于疏散烟雾
  - 8、烟雾传感器
  - 9、温度传感器

### 功能一：温度实时监测


- LCD1602中第二行显示的为当前环境的温度和烟雾浓度
- 这两个数值会随环境变化而实时变化

### 功能二：两级阈值及报警

- LCD1062第一行为温度和烟雾浓度的阈值（上图为一级阈值）
- 当温度或烟雾浓度超过对应一级阈值后，蜂鸣器会报警，并且亮起黄色警报灯，风扇启动一级转速，同时阈值变为二级阈值（如下图）
- 当温度或烟雾浓度超过二级阈值后，蜂鸣器持续报警，亮起红色指示灯，风扇全速转动
- 当温度和烟雾浓度都降到一级阈值以下，自动关闭所有报警措施

- 一级、二级阈值均可自行设定


### 功能三：手机远程遥控


- 本装置装有HC-05蓝牙模块，可与手机通信（当时不是很了解，HC-05不支持IOS，还特意借了个安卓调试）
- 手机通过串口助手连上蓝牙后，装置会不断向手机发送当前的温度和烟雾浓度
- 手机可通过向装置发送0、1、2指令分别控制装置的关闭、一级警报、二级警报。

**所有展示图片均在images文件夹下**

## 原理图

- 原理图用的是AD17自行绘制的，毕竟不是学这个的，所以画的不是很专业，大致一看即可。
- github有pdf版和源文件，需要可自己去下载。

## 源码

- 源码部分每个模块对应了一个.c文件，共7个：
  - 主逻辑函数main.c
  - 蜂鸣器模块Buzzer.c
  - 温度模块DS18B20.c
  - 蓝牙模块HC-05.c
  - 液晶显示LCD1602.c
  - 风扇驱动Motor.c
  - 烟雾传感模块MQ-2.c

### main.c

```c
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
extern void Display1602(unsigned int,unsigned int,unsigned int,unsigned int);	//显示温度，烟雾值
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



```



### Buzzer.c

```c
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
```



### DS18B20.c

```c
#include <reg52.h>

sbit DQ = P2^0;

void Delay_DS18B20(int num)
{
  while(num--) ;
}
void InitDS18B20(void)
{
  unsigned char x=0;
  DQ = 1;         
  Delay_DS18B20(8);    
  DQ = 0;         
  Delay_DS18B20(80);   
  DQ = 1;        
  Delay_DS18B20(14);
  x = DQ;          
  Delay_DS18B20(20);
}
unsigned char ReadOneChar(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;    
    dat>>=1;
    DQ = 1;     
    if(DQ)
    dat|=0x80;
    Delay_DS18B20(4);
  }
  return(dat);
}
void WriteOneChar(unsigned char dat)
{
  unsigned char i=0;
  for (i=8; i>0; i--)
  {
    DQ = 0;
    DQ = dat&0x01;
    Delay_DS18B20(5);
    DQ = 1;
    dat>>=1;
  }
}
unsigned int ReadTemperature()
{
  unsigned char a=0;
  unsigned char b=0;
  unsigned int t=0;
  float tt=0;
  InitDS18B20();
  WriteOneChar(0xCC);  
  WriteOneChar(0x44); 
  InitDS18B20();
  WriteOneChar(0xCC);  
  WriteOneChar(0xBE);  
  a=ReadOneChar();     
  b=ReadOneChar();    
  t=b;
  t<<=8;
  t=t|a;
  tt=t*0.0625;
  t= tt*10+0.5;     
  return t;
}



unsigned int getTmp()
{
	unsigned int c;
	c=ReadTemperature()-5;	
	if(c<0) c=0;					
	if(c>=999) c=999;
	return c;
}
```



### HC-05.c

```c
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
```



### LCD1602.c

```c
#include <reg52.h>

sbit LCDRS = P1^0;
sbit LCDEN= P1^1;
unsigned char code Init1[]="SET T:00   E:000";
unsigned char code Init2[]="NOW T:00.0 E:000";


void LCDdelay(unsigned int z)
{
  unsigned int x,y;
  for(x=z;x>0;x--)
    for(y=10;y>0;y--);
}


void write_com(unsigned char com)
{
  LCDRS=0;
  P0=com;
  LCDdelay(5);
  LCDEN=1;
  LCDdelay(5);
  LCDEN=0;
}

void write_data(unsigned char date)
{
  LCDRS=1;
  P0=date;
  LCDdelay(5);
  LCDEN=1;
  LCDdelay(5);
  LCDEN=0;
}

void Init1602()
{
  unsigned char i=0;
  write_com(0x38);
  write_com(0x0c);
  write_com(0x06);
  write_com(0x01);
  write_com(0x80);

  for(i=0;i<16;i++)
  {
		write_data(Init1[i]);
  }
  write_com(0x80+0x40);
  for(i=0;i<16;i++)
  {
		write_data(Init2[i]);
  }
}

void Display1602(unsigned int preTmp, unsigned int preSmk,
									unsigned int tmp, unsigned int smk)
{
	write_com(0x80+6);
	write_data(0x30+preTmp/10);
	write_data(0x30+preTmp%10);

	write_com(0x80+13);
	write_data(0x30+preSmk/100);
	write_data(0x30+preSmk%100/10);
	write_data(0x30+preSmk%10);

	write_com(0x80+0x40+6);
	write_data(0x30+tmp/100);
	write_data(0x30+tmp%100/10);
	write_data('.');
	write_data(0x30+tmp%10);

	write_com(0x80+0x40+13);
	write_data(0x30+smk/100);
	write_data(0x30+smk%100/10);
	write_data(0x30+smk%10);
}

```



### Motor.c

```c
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

```



### MQ-2.c

```c
#include <reg52.h>
#include <intrins.h>

sbit CS = P1^4;
sbit Clk = P1^2;
sbit DATI = P1^3;
sbit DATO = P1^3;

unsigned int ADC0832(unsigned char CH)
{
     unsigned int i,test,adval,dat1;
     adval = 0x00;
     test = 0x00;
     Clk = 0;      
     DATI = 1;
    _nop_();
    CS = 0;
    _nop_();
    Clk = 1;
   _nop_();


   if ( CH == 0x00 )     
   {
       Clk = 0;
       DATI = 1;      
       _nop_();
      Clk = 1;
        _nop_();
        Clk = 0;
      DATI = 0;      
      _nop_();
      Clk = 1;
      _nop_();
    } 
    else
    {
       Clk = 0;
    DATI = 1;      
      _nop_();
      Clk = 1;
      _nop_();
      Clk = 0;
      DATI = 1;      
    _nop_();
     Clk = 1;
     _nop_();
   }

      Clk = 0;
      DATI = 1;
   for( i = 0;i < 8;i++ )     
    {
       _nop_();
       adval <<= 1;
       Clk = 1;
       _nop_();
       Clk = 0;
       if (DATO)
          adval |= 0x01;
      else
          adval |= 0x00;
    }
      for (i = 0; i < 8; i++)      
      {
           test >>= 1;
           if (DATO)
              test |= 0x80;
           else 
              test |= 0x00;
          _nop_();
          Clk = 1;
          _nop_();
          Clk = 0;
      }
      //if (adval == test)     
       dat1 = test;
       nop_();
       CS = 1;       
       DATO = 1;
       Clk = 1;
       return dat1;
}
```



整个项目的所有源码文件均在我的github上，感兴趣的帮点个star~

附带说明文档和营销方案

github：https://github.com/YWonchall/fire-alarm-device
