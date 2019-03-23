#include"stc15.h"
#include"define.h"
#define KeyPort P3
uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
uchar readkey,Kb;
uchar key_f,key_t;
uchar Key1,Key2,Key3,Key4,Pre1,Pre2,Pre3,Pre4,LastKey1,LastKey2,LastKey3,LastKey4;
sbit R1 = P4^4;
sbit R2 = P4^2;
sbit R3 = P3^5;
sbit R4 = P3^4;
uchar nu;
uchar KeyScan();
void key_deal();
void Timer0Init();
void main()
{
	Timer0Init();
	Y4;P0 = 0xff;Y0;
	Y5;P0 = 0x00;Y0;
	Timer0Init();
	while(1)
	{
		key_deal();
		Y6;P0=0x00;Y0;
		Y7;P0=table[nu];Y0;
		Y6;P0=0x01;Y0;
	}
}
void key_deal()
{
	if(key_f == 1)
	{
		key_f = 0;
		Kb = KeyScan();
		if(Kb != 66)
		{
			nu+=2;
			nu%=10;
		}
	}
}
uchar KeyScan()
{
	R1 = 0;R2 = 1;R3 = 1;R4 = 1;
	readkey = ~KeyPort;
	readkey &= 0x0f;
	Pre1 = readkey&(readkey^Key1);
	Key1 = readkey;
	if(Pre1&0x08)
		return 9;
	if(Pre1&0x04)
		return 6;
	if(Pre1&0x02)
		return 3;
	if(Pre1&0x01)
		return 0;
	
	R1 = 1;R2 = 0;R3 = 1;R4 = 1;
	readkey = ~KeyPort;
	Pre2 = readkey&(readkey^Key2);
	Key2 = readkey;
	if(Pre2&0x08)
		return 10;//shezhi
	if(Pre2&0x04)
		return 7;
	if(Pre2&0x02)
		return 4;
	if(Pre2&0x01)
		return 1;
	
	R1 = 1;R2 = 1;R3 = 0;R4 = 1;
	readkey = ~KeyPort;
	Pre3 = Key3&(Key3^LastKey3);
	Key3 = readkey;
	if(Pre3&0x08)
		return 11;//clr
	if(Pre3&0x04)
		return 8;
	if(Pre3&0x02)
		return 5;
	if(Pre3&0x01)
		return 2;
	
	R1 = 1;R2 = 1;R3 = 1;R4 = 0;
	readkey = ~KeyPort;
	Pre4 = Key4&(Key4^LastKey4);
	Key4 = readkey;
	if(Pre4&0x08)
		return 66;
	if(Pre4&0x04)
		return 66;
	if(Pre4&0x02)
		return 66;
	if(Pre4&0x01)
		return 66;
	return 66;
}
void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	EA = 1;
	ET0 = 1;
}
void Timer0Int() interrupt 1
{
	key_t++;
	if(key_t == 10)
	{
		key_t = 0;
		key_f = 1;
	}
}