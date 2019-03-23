#include "define.h"
#include "iic.h"
uchar times;
uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xc6};
void Write_24C02(uchar,uchar);
uchar Read_24C02(uchar);
void main()
{
	times = Read_24C02(0x01);
	times ++;
	times = times%10;
	Write_24C02(0x01,times);
	Y6;P0 = 0x00;Y0;
	Y7;P0 = table[times];Y0;
	Y6;P0 = 0x80;Y0;
}
uchar Read_24C02(uchar addr)
{
	uchar tmp;
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
	tmp = IIC_ReadByte();	//读取目标数据
	IIC_Ack(0); 		//产生非应答信号		
	IIC_Stop();		//IIC总线停止信号			
	return tmp;
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