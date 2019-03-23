#include "stc15.h"
#include "main.h"
#include "intrins.h"
#include "onewire.c"
void main()
{
	sys_init();
	Timer0Init();
	while(1)
	{
		if(ms == 999)
			ReadTemperature();
		beep();
		keydeal();
		ca();
	}
}
void sys_init()//ϵͳ��ʼ�����رշ������ͼ̵���
{
	Y5;P04 = 0;P06 = 0;Y0;
}

void Timer0Int(void) interrupt 1
{
	ms++;								//ms����
	if(ms == 1000){			//����һ��
		ds_f = ~ds_f;			//�������˸��־
		ms = 0;
		s++;							//��
		if(s == 60)				//60s����1��
		{
			s = 0;
			m ++;
			if(m == 60){		//60�ִﵽһСʱ
				m = 0;
				h++;
				if(h == 24)
				{
					h=0;
				}
			}
		}
	}
	//key
	key_t++;
	if(key_t == 10)			//������10ms����
	{
		key_t = 0;
		key_f = 1;
	}

	if(beep_f == 1)		//����������
	{
		beep_t++;
		if(beep_t == 200)	//0.2s����
		{
			beep_i++;
			beep_t = 0;
			led = ~led;
			P0 = 0xff;Y4;P00 = led;Y0;
		}
		if(beep_i == 25)//25�ε��ﱨ��5��
		{
			beep_i = 0;
			beep_f = 0;
			Y4;P0 = 0xff;Y0;
		}
	}
	
	if(jump == 1)		//��ʾ�¶�
	{
		display_temp();
	}
	else
	{
		if(a_shan)
			display_alm();//��ʾ����ʱ��
		else display_time();//��ʾʱ��ʱ��
	}
}
void Timer0Init(void)		//1����@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xCD;		//���ö�ʱ��ֵ
	TH0 = 0xD4;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;
	EA = 1;
}

void display_time()//ʱ��ʱ�����ʾ
{
	if(ds_i<=1)			//Сʱ����ʾ
	{
		if(t_shan == 1)//��˸
		{
			P0 = 0x00;Y6;_nop_();Y7;
			if(ds_f) P0 = table[time[ds_i]];
			else P0 = 0xff;			Y0;
			P0 = 0x01<<ds_i;Y6;_nop_();Y0;
		}
		else						//����˸
		{
			P0=0x00;Y6;_nop_();
			Y7;P0 = table[time[ds_i]];Y0;
			P0 = 0x01<<ds_i;Y6;_nop_();Y0;
		}
	}
	else if((ds_i ==3)||(ds_i == 4))//���ӵ���ʾ
	{
		if(t_shan == 2)	//��˸
		{
			P0 = 0x00;Y6;_nop_();Y7;
			if(ds_f) P0 = table[time[ds_i]];
			else P0 = 0xff;			Y0;
			P0 = 0x01<<ds_i;Y6;_nop_();Y0;
		}
		else						//����˸
		{
			P0=0x00;Y6;_nop_();
			Y7;P0 = table[time[ds_i]];Y0;
			P0 = 0x01<<ds_i;Y6;_nop_();Y0;
		}
	}
	else if((ds_i ==6)||(ds_i == 7))//�����ʾ
	{
		if(t_shan == 3)	//��˸
		{
			P0 = 0x00;Y6;_nop_();Y7;
			if(ds_f) P0 = table[time[ds_i]];
			else P0 = 0xff;			Y0;
			P0 = 0x01<<ds_i;Y6;_nop_();Y0;
		}
		else						//����˸
		{
			P0=0x00;Y6;_nop_();
			Y7;P0 = table[time[ds_i]];Y0;
			P0 = 0x01<<ds_i;Y6;_nop_();Y0;
		}
	}
	else							//������˸�ķָ���
	{
		P0 = 0x00;Y6;_nop_();
		Y7;P0 = table[time[ds_i]];Y0;
		P0 = 0x01<<ds_i;Y6;_nop_();Y0;
	}
	ds_i++; if(ds_i==8) ds_i = 0;//
}

void display_alm()//����ʱ����ʾ
{
	if(ds_i<=1)
	{
		if(a_shan == 1)
		{
			P0 = 0x00;Y6;_nop_();		Y7;
			if(ds_f) P0 = table[alm[ds_i]];
			else P0 = 0xff;			Y0;
			Y6;P0 = 0x01<<ds_i;Y0;
		}
		else
		{
			Y6;P0=0x00;Y0;
			Y7;P0 = table[alm[ds_i]];Y0;
			Y6;P0 = 0x01<<ds_i;Y0;
		}
	}
	else if((ds_i ==3)||(ds_i == 4))
	{
		if(a_shan == 2)
		{
			P0 = 0x00;Y6;_nop_();		Y7;
			if(ds_f) P0 = table[alm[ds_i]];
			else P0 = 0xff;			Y0;
			Y6;P0 = 0x01<<ds_i;Y0;
		}
		else
		{
			Y6;P0=0x00;Y0;
			Y7;P0 = table[alm[ds_i]];Y0;
			Y6;P0 = 0x01<<ds_i;Y0;
		}
	}
	else if((ds_i ==6)||(ds_i == 7))
	{
		if(a_shan == 3)
		{
			P0 = 0x00;Y6;_nop_();		Y7;
			if(ds_f) P0 = table[alm[ds_i]];
			else P0 = 0xff;			Y0;
			Y6;P0 = 0x01<<ds_i;Y0;
		}
		else
		{
			Y6;P0=0x00;Y0;
			Y7;P0 = table[alm[ds_i]];Y0;
			Y6;P0 = 0x01<<ds_i;Y0;
		}
	}
	else
	{
		P0 = 0x00;Y6;_nop_();
		Y7;P0 = table[alm[ds_i]];Y0;
		Y6;P0 = 0x01<<ds_i;Y0;
	}
	ds_i++; if(ds_i==8) ds_i = 0;
}
void display_temp()
{
	P0 = 0x00;Y6;_nop_();Y7;
	if(temp_i == 0)
		P0 = table[11];
	else if(temp_i == 1)
		P0 = table[temp%10];
	else if(temp_i == 2)
		P0 = table[temp/10];
	Y0;
	Y6;P0=0x80>>temp_i;Y0;
	temp_i++;if(temp_i==3) temp_i = 0;
}
void ca()
{
	time[0] = h/10;time[1] = h%10;
	time[3] = m/10;time[4] = m%10;
	time[6] = s/10;time[7] = s%10;
	alm[0] = ah/10;alm[1] = ah%10;
	alm[3] = am/10;alm[4] = am%10;
	alm[6] = as/10;alm[7] = as%10;
}
void keyscan()
{
	ReadKey = KeyPort^0xff;
	Dif = ReadKey & (ReadKey ^ Key);
	Key = ReadKey;
}
void keydeal()
{
	if(key_f == 1)
	{
		key_f = 0;
		keyscan();
		if(beep_f == 1)
		{
			if(Dif&0x1f)
			{
				beep_f = 0;
				beep_i = 0;
				Y4;P00 = 0xff;Y0;
				return;
			}
		}
		if(Dif&0x01)//S7ʱ������
		{
			a_shan = 0;
			t_shan ++;
			if(t_shan == 4)
				t_shan = 0;
		}
		if(Dif&0x02)//S7ʱ������
		{
			t_shan = 0;
			a_shan ++;
			if(a_shan == 4)
				a_shan = 0;
		}
		if(Dif&0x04)//S5�Ӳ���
		{
			if(t_shan == 1)
			{
				h++;
				if(h == 24)
					h=0;
			}
			if(t_shan == 2)
			{
				m++;
				if(m == 60)
					m=0;
			}
			if(t_shan == 3)
			{
				s++;
				if(s == 60)
					s=0;
			}
			if(a_shan == 1)
			{
				ah++;
				if(ah == 24)
					ah=0;
			}
			if(a_shan == 2)
			{
				am++;
				if(am == 60)
					am=0;
			}
			if(a_shan == 3)
			{
				as++;
				if(as == 60)
					as=0;
			}
		}
		if(Dif&0x08)//S4������
		{
			while(Dif||Key)
			{
				if(key_f == 1)
				{
					key_f = 0;
					keyscan();
				}
				if((t_shan||a_shan)==0)
				{
					jump = 1;
					if(ms%200 == 0)
					ReadTemperature();
				}
			}
			jump = 0;
			if(t_shan == 1)
			{
				if(h == 0)
					h = 24;
				h--;
			}
			if(t_shan == 2)
			{
				if(m == 0)
					m=60;
				m--;
			}
			if(t_shan == 3)
			{
				if(s == 0)
					s=60;
				s--;
			}
			if(a_shan == 1)
			{
				if(ah == 0)
					ah=24;
				ah--;
			}
			if(a_shan == 2)
			{
				if(am == 0)
					am=60;
				am--;
			}
			if(a_shan == 3)
			{
				if(as == 0)
					as=60;
				as--;
			}
		}
	}
}

void ReadTemperature()
{
	unsigned char LSB,MSB ;
	
	Init_DS18B20(); 
	Write_DS18B20(0xCC); 
	Write_DS18B20(0x44); 
	Init_DS18B20(); 
	Write_DS18B20(0xCC); 
	Write_DS18B20(0xBE); 
	LSB = Read_DS18B20(); 
	MSB = Read_DS18B20(); 
	Init_DS18B20(); 
	temp = 0x0000;
	temp = MSB;
	temp <<= 8;
	temp = temp | LSB;
	temp >>= 4;
	temp = temp + (LSB&0x0f)*0.0625;
}
void beep()
{
	if((s==as)&&(m==am)&&(h==ah))
	{
		if(!(t_shan||a_shan))
			beep_f=1;
	}
}