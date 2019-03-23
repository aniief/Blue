/**********������ʡ����Ŀ***********/
//*********8:20��ʼ*****************/
#include "reg52.h"
#include "define.h"
#include "main.h"
#include "onewire.c"
#include "iic.h"
sfr AUXR = 0x8e;
sfr P4 = 0xC0; 
void main()
{
	cls_buzz();//�ر�����
	init_temp();//�¶ȳ�ʼ��
	Timer0Init();//��ʱ��0��ʼ��
	while(1)
	{
		get_temp();
	}
}
void Timer0Init()//100us��ʱ��
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xAE;		//���ö�ʱ��ֵ
	TH0 = 0xFB;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;
	EA = 1;
}

void Timer0Int() interrupt 1
{
	ms++;
	if(ms == 7)//�͵�ƽռ30%
	{
		pwm = 0;
	}
	if(ms == 10)//����1ms
	{
		temp_t++;//�¶�ת���ȴ�ʱ��
		ms = 0;	//
		
		key_t++;
		if(key_t == 10)//10ms����
		{
			key_t = 0;
			key_f = 1;
		}
		display();
		pwm = 1; 	//pwm��1
	}
	deal_alm();	//1ms�ж�һ���¶��Ƿ񳬹�����
}
void init_temp() //�ϵ��ȡ����ʼ��DS18B20
{
	temp_max = Read_24C02(0x00);//��EEPROM�ж�ȡ������
	nu[0] = temp_max/10;
	nu[1] = temp_max%10;
	temp_min=Read_24C02(0x01);
	nu[2] = temp_min/10;
	nu[3] = temp_min%10;
	Init_DS18B20();	//����DS18B20ת������Ϊ9λ
	Write_DS18B20(0xCC);
	Write_DS18B20(0x4E);
	Write_DS18B20(0x5a);
	Write_DS18B20(0x00);
	Write_DS18B20(0x1f);//0001 1111  9λ
	if(temp_max<temp_min)
	{
		temp_max = temp_min;
	}
	temp = temp_min; //��ǰ�¶ȳ�ʼ��Ϊ���ֵ
}
void deal_alm()//�ж��¶��Ƿ񳬹�����������
{
	if(temp>temp_max)		//��������
	{
		P0 = ~pwm;Y4;Y0;	//���PWM
		alm = 1;			//��Ϊ״̬1
	}
	else if(temp<temp_min)	//С������
	{
		Y5;P0 = 1<<4;Y0;	//�򿪼̵���
		alm = 2;
	}
	else if(alm == 1)		//״̬1����  �رյ�
	{
		alm = 0;
		Y4;P0 = 0xff;Y0;	//������PWM��1��
	}
	else if(alm == 2)		//״̬2����  �رռ̵���
	{
		Y5;P0 = 0x00;Y0;	//�رռ̵���
		alm = 0;
	}
}
void key_scan()
{
	ReadKey = ~KeyPort;
	Dif = ReadKey & (ReadKey^Key);
	Key = ReadKey;
}
void key_deal()//��������
{
	if(key_f == 1)
	{
		key_f = 0;
		key_scan();
		if(Dif & 0x01)//S7
		{
			if(temp_max<100)
			{
				temp_max++;
				Write_24C02(0x00,temp_max);
				nu[0] = temp_max/10;
				nu[1] = temp_max%10;
			}
		}
		if(Dif & 0x02)
		{
			if(temp_max>temp_min)
			{
				temp_max--;
				Write_24C02(0x00,temp_max);
				nu[0] = temp_max/10;
				nu[1] = temp_max%10;
			}
		}
		if(Dif & 0x04)
		{
			if(temp_min<temp_max)
			{
				temp_min++;
				Write_24C02(0x01,temp_min);
				nu[2] = temp_min/10;
				nu[3] = temp_min%10;
			}
		}
		if(Dif & 0x08)
		{
			if(temp_min>0)
			{
				temp_min--;
				Write_24C02(0x01,temp_min);
				nu[2] = temp_min/10;
				nu[3] = temp_min%10;
			}
		}
	}
}
void display()
{
	P0 = 0x00;Y6;Y0;
	P0 = 0xff;Y7;P0 = table[nu[ds_i]];Y0;
	P0 = 0x00;Y6;P0 = 0x01<<ds_i;Y0;
	ds_i++;
	ds_i &= 7;
}
void get_temp()
{
	uchar LSB,MSB;
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44);
	temp_t=0;
	while(temp_t<100)
	{
		key_deal();
	}
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	temp = 0x0000;
	temp = (MSB<<4)|(LSB>>4);
	//temp += (LSB&0x0f)*0.0625;
	nu[6] = temp/10;
	nu[7] = temp%10;
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
void cls_buzz()
{
	Y5;P0=0x00;Y0;
}