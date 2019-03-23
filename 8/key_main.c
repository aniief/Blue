#include "stc15.h"
#include "intrins.h"
#include "define.h"
uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xc5};
uchar key_f,key_t;
uchar nu;
void Key_Deal();
void KeyScan();
void sys_init();
void Timer0Init();
void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

void main()
{
	sys_init();
	Timer0Init();
	while(1)
	{
		Y6;P0 = 0x00;Y0;
		Y7;P0 = table[nu];Y0;
		P0 = 0x01;Y6;Y0;
		Key_Deal();
	}
}
#define KeyPort P3
uchar ReadKey,Key,Dif;
void KeyScan()
{
	ReadKey = ~KeyPort;
	Dif = ReadKey&(ReadKey^Key);//Last = 1, Key = 0;  Dif = 1;  �а������¡�   Last  = 0;
	Key = ReadKey;
}

void Key_Deal()
{
	if(key_f == 1)
	{
		key_f = 0;
		KeyScan();
		if(Dif&0x08)
		{
			nu++;
		}
		if(Dif&0x04)
		{
			nu--;
		}
		nu = nu%10;
		if(Dif&0x02)
		{
			while(Dif||Key)
			{
				if(key_f == 1)
				{
					key_f = 0;
					KeyScan();
				}
				Y6;P0 = 0x00;Y0;
				Y7;P0 = table[10];Y0;
				Y6;P0 = 0x55;Y0;
				Delay1ms();
				
			}
			Y6;P0 = 0x00;Y0;
		}
	}
}

void sys_init()
{
	Y5;P04 = 0;P06 = 0;Y0;
}
void Timer0Init(void)		//1����@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xCD;		//���ö�ʱ��ֵ
	TH0 = 0xD4;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;
	EA = 1;
}
void Timer0Int(void) interrupt 1
{
	key_t++;
	if(key_t == 10)
	{
		key_t = 0;
		key_f = 1;
	}
}