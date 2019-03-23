#include "reg52.h"
#include "intrins.h"
#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();}
/****���߿�ʼ*****/
//SCLΪ1ʱ����SDA
//�����ߵĸı�ֻ����SCLΪ�͵�ƽʱ�ı䣬�����SCL=0;
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
/*******����ֹͣ****/
//SCLΪ1ʱ����SDA
//
void IIC_Stop()
{
	SDA = 0;
	SCL = 1;
	somenop;
	SDA = 1;

}
/*****����Ӧ��*****/
//���շ���SCLΪ��ʱ��SDAΪ��
//���Ȱ�SDA��0���ڽ�SCL�øߣ������SDA=1�ͷ���������
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
	if(SDA)//��Ӧ����η��겻�ܷ���
	{
		SCL = 0;
		IIC_Stop();
		return 0;
	}
	else		//Ӧ���˾���
	{
		SCL = 0;
		return 1;
	}
}
//�ȷ�����SDA���ڽ�SCL���ߡ�SDAֻ����SCLΪ0�Ǳ仯
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