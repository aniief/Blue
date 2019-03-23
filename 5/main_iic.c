#include "stc15.h"
#include "ds1302.c"
#include "main.h"
#include "iic.c"
void InitUart()
{
	TMOD = 0X20;
	TH1 = 0XFD;
	TL1 = 0XFD;
	AUXR = 0x00;
	TR1 = 1;
	SCON = 0X50;
	ES = 1;
	EA = 1;
}

void SendChar(uchar dat)
{
	SBUF = dat;
	while(TI == 0);
	TI = 0;
}
void get_hum()
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x03);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	hum = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
}
void main()
{
	cls_buzz();
	InitUart();
	while(1)
	{
		get_hum();
		SendChar(hum);
	}
}
void cls_buzz()
{
	Y5;P0 = 0x00;Y0;
}