/*
  ����˵��: ��������������
  �������: Keil uVision 4.10 
  Ӳ������: CT107��Ƭ���ۺ�ʵѵƽ̨
  ��    ��: 2011-8-9
*/

#include "onewire.h"

//��������ʱ����
void Delay_OneWire(unsigned int t)
{
  while(t--);
}

//DS18B20оƬ��ʼ��
bit Init_DS18B20(void)
{
	bit initflag = 0;
	DQ = 1;
	Delay_OneWire(1);
	DQ = 0;
	Delay_OneWire(360); 
	DQ = 1;
	Delay_OneWire(45); 
	initflag = DQ;    
	Delay_OneWire(180);
  
	return initflag;
}

//ͨ����������DS18B20дһ���ֽ�
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		Delay_OneWire(1);
		DQ = dat&0x01;
		Delay_OneWire(45);
		DQ = 1;
		Delay_OneWire(1);
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//��DS18B20��ȡһ���ֽ�
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		Delay_OneWire(1);
		dat >>= 1;
		DQ = 1;
		Delay_OneWire(1);
		if(DQ)
		{
			dat |= 0x80;
		}
		Delay_OneWire(43);
	}
	return dat;
}