#include "reg52.h"
#include "ds18b20.h"
typedef unsigned char uchar;
typedef unsigned int uint;
#define Y0 P2 = 0x1f
#define Y4 P2 = (P2&0x1f)|0x80
#define Y5 P2 = (P2&0x1f)|0xa0
#define Y6 P2 = (P2&0x1f)|0xc0
#define Y7 P2 = (P2&0x1f)|0xe0
uint temp_t;
uint temp;
uchar ds_i;
uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
uchar nu[4];
void get_temp();
void display();
void Timer0Init();
void set_DS18B20();

void main()
{
	set_DS18B20();
	Timer0Init();
	while(1)
	{
		get_temp();
	}
}
void set_DS18B20()
{
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x4E);
	Write_DS18B20(0x5a);
	Write_DS18B20(0x00);
	Write_DS18B20(0x1f);
}
void get_temp()
{
	uchar LSB,MSB;
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44);
	temp_t = 0;
	while(temp_t  <= 95)
	{
		//////
	}
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	temp = 0x0000;
	temp = (MSB<<4)|(LSB>>4);
	temp *= 10;
	temp += (LSB&0x0f)*0.625;
	nu[2] = temp/100;
	nu[1] = temp%100/10;
	nu[0] = temp%10;
}
void Timer0Init(void)		//1����@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xCD;		//���ö�ʱ��ֵ
	TH0 = 0xD4;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	EA = 1;
	ET0 = 1;
}

void Timer0Int() interrupt 1
{
	temp_t++;
	display();
}
void display()
{
	P0 = 0x00;Y6;Y0;
	if(ds_i == 1)
	{
		Y7;P0 = table[nu[ds_i]]&0x7f;Y0;
	}
	else 
	{
		P0 = 0xff;Y7;P0 = table[nu[ds_i]];Y0;
	}
	P0 = 0x00;Y6;P0 = 0x80>>ds_i;Y0;
	ds_i++;
	if(ds_i == 3)
		ds_i = 0;
}