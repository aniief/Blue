/*******���ű����߽�ʡ������********/
//	@����	�� Fan
//	@email	�� imzjfan@163.com
//	@time  	�� 2019-3-11
/*********************************/

#include "main.h"		//������������	
#include "define.h"		//�궨��
#include "stc15.h"		//
#include "onewire.c"	//�ٷ�����
void main()
{
	cls_buzz();			
	init_temp();		//��ʼ��DS18B20  ���侫�ȸ���Ϊ9λ������ת��ʱ��Ϊ93.75ms
	Timer0Init();		//��ʱ��0��ʼ��
	while(1)
	{
		if(ds_f == 0)		//��ʾ����ģʽ��ʣ��ʱ��
		{
			nu[1] = mode;
			nu[5] = left_t/100;
			nu[6] = left_t%100/10;
			nu[7] = left_t%10;
			deal_key();
		}
		else if(ds_f == 1) 	//��ȡ����ʾ�¶�
		{
			get_temp();	
		}
	}
}

void Timer0Int() interrupt 1 //0.1ms
{
	if(run_f&&(left_t != 0))			//��������״̬  ���PWM�͵��	
	{
		ms++;
		if(ms == pwm_f[mode])
		{
			pwm = 0xff;		//PWM
		}
		else if(ms == 10)
		{
			ms = 0;			
			pwm = 0;		//PWM
		}
		/*
		 * �˴�������Ŀ��Ҫ���P3^4�޷�ֱ�۵Ĺ۲쵽��Ҫ���PWM�Ƿ�ʵ��
		 * �ʽ�pwm�������L5~L7����ֱ�۵Ĺ۲쵽PWMռ�ձȵı仯
		 * ������ĿҪ��Ӧ��Ϊ
		 * ������pwmȡ0,1������
		 * sbit pwm = P3^4;
		 * led = ~(1<<mode);
		 * ����Ҫ�Ƶ�  if(key_f == 1)������õ�
		 */
		led = (led&0x0f)|(pwm&0xf0);
		led = (led&0xf0)|((~(1<<mode))&0x0f);  //�ѵ���λ�����  ����λ������ʾģʽ
		P0 = led;Y4;Y0;
	}
	else					//��ǰֹͣ    ֹͣPWM���+�ص�
	{
		pwm = 0xff;
	}
	key_t++;
	if(key_t % 10 == 0)		//1ms  ����ܶ�̬��ʾ
	{	
		display();
	}
	if(key_t == 100) 			//	10ms ��������
	{
		key_t = 0;
		key_f = 1;
		if(run_f && (left_t != 0))	//	����ing   �������м�ʱ
		{
			time_s++;
			if(time_s == 100) //	1s  ��ʱ -- 
			{
				time_s = 0;
				left_t --;
			}
		}
		else 
		{
			P0 = 0xff;Y4;Y0;		
		}
		temp_t += 10;	//�¶�ת���ȴ�ʱ�� ������10ms����һ�Σ�����Ҫ+=10��
	}
}
void KeyScan()
{
	TheKey = ~P3;
	Pre = TheKey&(TheKey^LastKey);
	LastKey = TheKey; 
}
void deal_key()
{
	if(key_f == 1)
	{
		key_f = 0;
		KeyScan();
		if(Pre&0x01)		//		S7
		{
			if(ds_f == 0) 	// ��ʾ�¶�
			{
				ds_f = 1;
				nu[1] = 4;
				nu[7] = 11;
			}
			else			//��ʾģʽ��ʱ��
			{
				ds_f = 0;
				nu[1] = mode;
				nu[5] = left_t/100;
				nu[6] = left_t%100/10;
				nu[7] = left_t%10;
			}
		}
		else if(Pre&0x02)	//S6  ֹͣ
		{
			run_f = 0;
			left_t = 0;
		}
		else if(Pre&0x04)	//S5 ��ʱ��
		{
			run_f = 1;
			left_t += 60;
			left_t %= 180;
		}
		else if(Pre&0x08)	//S4	��ģʽ
		{
			mode++;
			mode %= 3;
		}
	}
}

void display()
{
	P0 = 0x00;Y6;Y0;
	P0 = table[nu[ds_i]];Y7;Y0;
	P0 = 0x01<<ds_i;Y6;Y0;
	ds_i ++;
	ds_i &= 7;
}
void Timer0Init()//0.1ms
{
	TMOD = 0x00;
	AUXR |= 0x80;
	TH0 = 0xFB;
	TL0 = 0xAE;
	TR0 = 1;
	EA = 1;
	ET0 = 1;
}

void cls_buzz()
{
	Y5;P0=0x00;Y0;
	Y4;P0=0xff;Y0;
}
void get_temp()
{
	uchar LSB,MSB;
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0X44);
	temp_t = 0;
	while(temp_t <= 95)
	{
		deal_key();
	}
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0XBE);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	temp = (LSB >> 4)|(MSB << 4);
	nu[5] = temp/10;
	nu[6] = temp%10;
}
void init_temp()
{
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0X4E);
	Write_DS18B20(100);
	Write_DS18B20(0);
	Write_DS18B20(0x1f);
}