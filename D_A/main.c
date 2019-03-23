#include "main.h"
#include "iic.c"
void main()
{
	cls_buzz();
	InitAIN();
	Timer0Init();
	while(1)
	{
		GetAIN();
	}
}
void Timer0Int() interrupt 1
{
	display();
}
void InitAIN()
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x40);
	IIC_WaitAck();
	IIC_SendByte(200);
	IIC_WaitAck();
	IIC_Stop();
}
void GetAIN()
{
	EA = 0;
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	whats = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	EA = 1;
	nu[0] = whats /100;
	nu[1] = whats %100/10;
	nu[2] = whats %10;
}
void display()
{
	P0=0x00;Y6;Y0;
	P0=table[nu[ds_i]];Y7;Y0;
	P0=0x01<<ds_i;Y6;Y0;
	ds_i++;ds_i%=3;
}
void cls_buzz()
{
	Y5;P0=0x00;Y0;
}
void Timer0Init(void)		//0.1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xAE;		//设置定时初值
	TH0 = 0xFB;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}