#include "define.h"
#include "stc15.h"
#include "INTRINS.H"
#include "iic.c"

//uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};//段码
uchar times;
uchar da;
void cls_buzz()
{
	Y5;P06 = 0;P04 = 0;Y0;
}
void Write_24C02(unsigned char addr, unsigned char dat)
{
	IIC_Start();		//IIC总线起始信号					
	IIC_SendByte(0xa0); 	//24C02的写设备地址
	IIC_WaitAck();		//等待从机应答	
	IIC_SendByte(addr); 	//内存字节字节
	IIC_WaitAck(); 		//等待从机应答	
	IIC_SendByte(dat); 	//写入目标数据
	IIC_WaitAck();		//等待从机应答	
	IIC_Stop();		//IIC总线停止信号		
}
unsigned char Read_24C02(unsigned char addr)
{
	unsigned char tmp;
	//进行一个伪写操作
	IIC_Start();		//IIC总线起始信号					
	IIC_SendByte(0xa0); 	//24C02写设备地址
	IIC_WaitAck();		//等待从机应答	
	IIC_SendByte(addr); 	//内存自己地址
	IIC_WaitAck(); 		//等待从机应答	
	//进行字节读操作
	IIC_Start();		//IIC总线起始信号					
	IIC_SendByte(0xa1); 	//24C02读设备地址
	IIC_WaitAck();		//等待从机应答	
	tmp = IIC_RecByte();	//读取目标数据
	IIC_Ack(0); 		//产生非应答信号		
	IIC_Stop();		//IIC总线停止信号			
	return tmp;
}
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
void IntUart() interrupt 4
{
	if(RI == 1)
	{
		RI = 0;
		da = SBUF;
		SendByte(da);
		SendByte(Read_24C02(da));
	}
	
}
void Delay5ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 54;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
}


void main()
{
	cls_buzz();
	InitUart();
	Write_24C02(0x01,0x12);
	Delay5ms();
	Write_24C02(0x02,0x34);
	Delay5ms();
	Write_24C02(0x05,0x56);
	Delay5ms();
	Write_24C02(0x07,0x78);
	Delay5ms();
	while(1);
}