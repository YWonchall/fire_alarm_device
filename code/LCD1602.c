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
