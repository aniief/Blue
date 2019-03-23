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
	IIC_Start();		//IIC������ʼ�ź�							
	IIC_SendByte(0x90); 	//PCF8591��д�豸��ַ		
	IIC_WaitAck();  	//�ȴ��ӻ�Ӧ��		
	IIC_SendByte(0x03); 	//д��PCF8591�Ŀ����ֽ�		
	IIC_WaitAck();  	//�ȴ��ӻ�Ӧ��						
	IIC_Stop(); 		//IIC����ֹͣ�ź�					
	
	IIC_Start();		//IIC������ʼ�ź�									
	IIC_SendByte(0x91); 	//PCF8591�Ķ��豸��ַ		
	IIC_WaitAck(); 		//�ȴ��ӻ�Ӧ��		
	dat = IIC_RecByte();	//��ȡPCF8591ͨ��3������ 			
	IIC_Ack(0); 		//������Ӧ���ź�				
	IIC_Stop(); 		//IIC����ֹͣ�ź�					
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