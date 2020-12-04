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