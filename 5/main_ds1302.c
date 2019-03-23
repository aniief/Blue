#include "main.h"
#include "ds1302.c"
#include "reg52.h"
#include "define.h"
sfr AUXR = 0x8e;
void main()
{
	P0=0x00;Y5;Y0;
	Timer0Init();
	Init_Time();
	while(1)
	{
		get_time();
	}
}
void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}
void Timer0Int() interrupt 1
{
	display();
}
void display()
{
	P0 = 0x00;Y6;Y0;
	P0 = table[time[ds_i]];Y7;Y0;
	P0 = 0x01<<ds_i;Y6;Y0;
	ds_i ++;
	ds_i &= 7;
}
void Init_Time()
{
	Write_Ds1302_Byte(0x8E,0x00);//关闭写保护
	Write_Ds1302_Byte(0x80,0x00);//0秒
	Write_Ds1302_Byte(0x82,0x30);//30分
	Write_Ds1302_Byte(0x84,0x08);//8时
}
void get_time()
{
	//Write_Ds1302(0XBF);
	s = Read_Ds1302_Byte(0x81);
	m = Read_Ds1302_Byte(0x83);
	h = Read_Ds1302_Byte(0x85);
	time[0] = h>>4;time[1] = h&0x0f;
	time[3] = m>>4;time[4] = m&0x0f;
	time[6] = s>>4;time[7] = s&0x0f;
}