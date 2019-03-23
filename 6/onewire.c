/*
  程序说明: 单总线驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台
  日    期: 2011-8-9
*/

#include "onewire.h"

//单总线延时函数
void Delay_OneWire(unsigned int t)
{
  while(t--);
}

//DS18B20芯片初始化
bit Init_DS18B20(void)
{
	bit initflag = 0;
	EA = 0;
	DQ = 1;
	Delay_OneWire(1);
	DQ = 0;
	Delay_OneWire(365); 
	DQ = 1;
	Delay_OneWire(12); 
	initflag = DQ;
	Delay_OneWire(180);
	EA = 1;
	return initflag;
}

//通过单总线向DS18B20写一个字节
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	DQ = 1;
	for(i=0;i<8;i++)
	{
		EA = 0;
		DQ = 0;
		Delay_OneWire(1);
		DQ = dat&0x01;
		Delay_OneWire(45);
		DQ = 1;
		Delay_OneWire(1);
		dat >>= 1;
		EA = 1;
	}
	Delay_OneWire(1);
}

//从DS18B20读取一个字节
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  DQ = 1;
	for(i=0;i<8;i++)
	{
		EA = 0;
		DQ = 0;
		dat >>= 1;
		Delay_OneWire(1);
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}
		EA = 1;
		Delay_OneWire(45);
	}
	return dat;
}