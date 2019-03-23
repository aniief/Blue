#include "stc15.h"
#include "define.h"
#include "iic.c"
#include "main.h"
uchar i,addr,dat;
void main()
{
	cls_buzz();
	Write();
	Delay1s();
	//Timer0Init();
	while(1)
	{
		Read();
		dat = dat%10;
		Write();
		Y6;P0 = 0x00;Y0;
		Y7;P0 = table[dat];Y0;
		Y6;P0 = 0xff;Y0;	
		Delay1s();
	}
}
void Delay1s()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 43;
	j = 6;
	k = 203;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Write()
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_SendByte(dat+1);
	IIC_WaitAck();
	IIC_Stop();
}
void Read()
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	dat = IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
}
void cls_buzz()
{
	Y5;P04=0;P06=0;Y0;
}