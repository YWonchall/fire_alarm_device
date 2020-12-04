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