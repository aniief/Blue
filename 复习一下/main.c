#include "stc15.h"
#include "define.h"
#include "iic.c"
#include "main.h"
void main()
{
	cls_buzz();
	//Timer0Init();
	while(1)
	{
		display();
		ReadA1();
	}
}
void ReadA1()
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	light = IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
}
void display()
{
	Y6;P0 = 0x00;Y0;
	Y7;P0 = table[light/100];Y0;
	Y6;P0 = 0x80>>2;Y0;
	delay(500);
	Y6;Y6;P0 = 0x00;Y0;
	Y7;P0 = table[light%100/10];Y0;
	Y6;P0 = 0x80>>1;Y0;
	delay(500);
	Y6;P0 = 0x00;Y0;
	Y7;P0 = table[light%10];Y0;
	Y6;P0 = 0x80>>0;Y0;
	delay(500);
	Y6;P0 = 0x00;Y0;
	
}
void cls_buzz()
{
	Y5;P04=0;P06=0;Y0;
}
void delay(uint t)
{
	while(t--);
}