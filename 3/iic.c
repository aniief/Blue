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
	tmp = IIC_ReadByte();	//��ȡĿ������
	IIC_Ack(0); 		//������Ӧ���ź�		
	IIC_Stop();		//IIC����ֹͣ�ź�			
	return tmp;
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