#include "stc15.h"

#define Y5 P2 = (P2&0x1f)|0xa0
#define Y6 P2 = (P2&0x1f)|0xc0
#define Y7 P2 = (P2&0x1f)|0xe0
#define Y0 P2 &= 0x1f
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf}; 
uchar nu[8] = {1,2,3,4,5,10,10,10};
uint ms;
uint f;
uchar ds_i;
#define T0 P34
void Timer0Init()
{
	TMOD &= 0xF0;
	TMOD |= 0x04;//TMOD = 0xX4;
	TH0 = 0x00;
	TL0 = 0x00;
}
void Timer1Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x50;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xCD;		//设置定时初值
	TH1 = 0xD4;		//设置定时初值
	TF1 = 0;		//清除TF0标志
	TR1 = 1;		//定时器0开始计时
	EA = 1;
	ET1 = 1;
}

void display()
{
	P0 = 0x00;Y6;Y0;
	P0 = table[nu[ds_i]];Y7;Y0;
	P0 = 0x01<<ds_i;Y6;Y0;
	ds_i++;
	ds_i %= 8;
}

void Timer1Int() interrupt 3
{
	ms++;
	if(ms == 1000)
	{
		TR0 = 0;
		ms = 0;
		f=((TH0<<8)|TL0);
		nu[0] = f/10000;
		nu[1] = f%10000/1000;
		nu[2] = f%1000/100;
		nu[3] = f%100/10;
		nu[4] = f%10;
		f=0;
		TH0 = 0x00;
		TL0 = 0x00;
		TR0 = 1;
	}
	display();
}

void main()
{
	P0=0x00;Y5;Y0;
	Timer1Init();
	Timer0Init();
	while(1)
	{
		
	}
}