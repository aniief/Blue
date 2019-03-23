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

//ͨ����������DS18B20дһ���ֽ�
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

//��DS18B20��ȡһ���ֽ�
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