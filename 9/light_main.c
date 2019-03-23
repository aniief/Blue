#include "define.h"
#include "main.h"
#include "stc15.h"
#include "iic.c"
#include "INTRINS.H"
uchar dat;
void InitUart()
{
	TMOD = 0x20;
	TH1 = 0xfd;
	TL1 = 0xfd;
	AUXR = 0x00;
	TR1 = 1;
	SCON = 0x50;
	ES = 1;
	EA = 1;
}
void SendByte(uchar dat)
{
	SBUF = dat;
	while(TI == 0);
	TI = 0;
}
void Read_AIN3()
{
	IIC_Start();		//IIC总线起始信号							
	IIC_SendByte(0x90); 	//PCF8591的写设备地址		
	IIC_WaitAck();  	//等待从机应答		
	IIC_SendByte(0x03); 	//写入PCF8591的控制字节		
	IIC_WaitAck();  	//等待从机应答						
	IIC_Stop(); 		//IIC总线停止信号					
	
	IIC_Start();		//IIC总线起始信号									
	IIC_SendByte(0x91); 	//PCF8591的读设备地址		
	IIC_WaitAck(); 		//等待从机应答		
	dat = IIC_RecByte();	//读取PCF8591通道3的数据 			
	IIC_Ack(0); 		//产生非应答信号				
	IIC_Stop(); 		//IIC总线停止信号					
}
void Delay1000ms()		//@11.0592MHz
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

void main()
{
	Y5;P06 = 0;P04 = 0;Y0;
	InitUart();
	while(1)
	{
		Read_AIN3();
		SendByte(dat);
		Delay1000ms();
	}
}