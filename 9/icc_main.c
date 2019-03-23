#include "define.h"
#include "stc15.h"
#include "INTRINS.H"
#include "iic.c"

//uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};//����
uchar times;
uchar da;
void cls_buzz()
{
	Y5;P06 = 0;P04 = 0;Y0;
}
void Write_24C02(unsigned char addr, unsigned char dat)
{
	IIC_Start();		//IIC������ʼ�ź�					
	IIC_SendByte(0xa0); 	//24C02��д�豸��ַ
	IIC_WaitAck();		//�ȴ��ӻ�Ӧ��	
	IIC_SendByte(addr); 	//�ڴ��ֽ��ֽ�
	IIC_WaitAck(); 		//�ȴ��ӻ�Ӧ��	
	IIC_SendByte(dat); 	//д��Ŀ������
	IIC_WaitAck();		//�ȴ��ӻ�Ӧ��	
	IIC_Stop();		//IIC����ֹͣ�ź�		
}
unsigned char Read_24C02(unsigned char addr)
{
	unsigned char tmp;
	//����һ��αд����
	IIC_Start();		//IIC������ʼ�ź�					
	IIC_SendByte(0xa0); 	//24C02д�豸��ַ
	IIC_WaitAck();		//�ȴ��ӻ�Ӧ��	
	IIC_SendByte(addr); 	//�ڴ��Լ���ַ
	IIC_WaitAck(); 		//�ȴ��ӻ�Ӧ��	
	//�����ֽڶ�����
	IIC_Start();		//IIC������ʼ�ź�					
	IIC_SendByte(0xa1); 	//24C02���豸��ַ
	IIC_WaitAck();		//�ȴ��ӻ�Ӧ��	
	tmp = IIC_RecByte();	//��ȡĿ������
	IIC_Ack(0); 		//������Ӧ���ź�		
	IIC_Stop();		//IIC����ֹͣ�ź�			
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