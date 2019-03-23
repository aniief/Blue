#include "reg52.h"
#include "main.h"
#include "intrins.h"
#include "iic.c"

void main()
{
	sys_init();
	init_PCF8591();
	Timer0Init();
	while(1)
	{
		get_PCF8591();
	}
}
void Timer0Int() interrupt 1
{
	//get_PCF8591();
	display();
}
void display()
{
	P0 = 0x00;Y6;Y0;
	P0 = nu[ds_i];Y7;Y0;
	P0 = 0x01<<ds_i;Y6;Y0;
	Y0;
	ds_i ++;
	ds_i &= 7;
}
void Timer0Init()
{
	TMOD &= 0xf0;
	AUXR |= 0x80;
	TH0 = 0xD4;
	TL0 = 0xCC;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}
void sys_init()
{
	P0=0x00;Y5;Y0;
	Y4;P0=0xff;Y0;
}
void init_PCF8591()
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_Stop();
}
void get_PCF8591()
{
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	ET0 = 0;
	temp = IIC_RecByte();
	ET0 = 1;
	IIC_SendAck(1);
	IIC_Stop();
	
	nu[0] = table[temp/100];
	nu[1] = table[temp%100/10];
	nu[2] = table[temp%10];
	
}