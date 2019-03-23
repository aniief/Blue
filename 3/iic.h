#include "reg52.h"
#include "intrins.h"
#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();}
/****总线开始*****/
//SCL为1时拉低SDA
//数据线的改变只能在SCL为低电平时改变，故最后SCL=0;
//
sbit SCL = P2^0;
sbit SDA = P2^1;
void IIC_Start()
{
	SDA = 1;
	SCL = 1;
	somenop;
	SDA = 0;
	somenop;
	SCL = 0;
}
/*******总线停止****/
//SCL为1时拉高SDA
//
void IIC_Stop()
{
	SDA = 0;
	SCL = 1;
	somenop;
	SDA = 1;

}
/*****关于应答*****/
//接收方在SCL为高时，SDA为低
//是先把SDA置0，在将SCL置高，最后在SDA=1释放数据总线
void IIC_Ack(unsigned char ackbit)
{
	if(ackbit)
		SDA = 0;
	else
		SDA = 1;
	somenop;
	SCL = 1;
	somenop;
	SCL = 0;
	SDA = 1;
	somenop;
}
bit IIC_WaitAck()
{
	SDA = 1;
	somenop;
	SCL = 1;
	somenop;
	if(SDA)//非应答，这次发完不能发了
	{
		SCL = 0;
		IIC_Stop();
		return 0;
	}
	else		//应答了就了
	{
		SCL = 0;
		return 1;
	}
}
//先放数据SDA，在将SCL拉高。SDA只能在SCL为0是变化
void IIC_SendByte(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)
			SDA = 1;
		else SDA = 0;
		somenop;
		SCL = 1;
		dat <<= 1;
		somenop;
		SCL = 0;
	}
}
unsigned char IIC_ReadByte()
{
	unsigned char da,i;
	for(i=0;i<8;i++)
	{
		SCL = 1;
		somenop;
		da <<= 1;
		if(SDA)
			da |= 0x01;
		SCL = 0;
		somenop;
	}
	return da;
}