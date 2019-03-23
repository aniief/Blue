#include "stc15.h"
#include "onewire.c"
#include "main.h"
void main()
{
	sys_init();//ϵͳ��ʼ�����ر��޹�����ͳ�ʼ��DS18B20
	Timer0Init();//��ʱ����ʼ��
	while(1)
	{
		get_temp();//��ȡ�¶ȣ���������Ƕ�뵽�ȴ��¶�ת����ʱ����
	}
}
void Timer0Int() interrupt 1 
{
	key_t++;		//����
	if(key_t == 10)
	{
		key_t = 0;
		key_f = 1;
	}
	ms++;//ms��ʱ
	if(temp_zone == 0)//�¶�������0
	{
		if(ms >= 800)	//0.8s��˸
		{
			ms = 0 ;
			led = ~led;
		}
	}
	if(temp_zone == 1)//�¶�������1
	{
		if(ms >= 400)//0.4s��˸
		{
			ms = 0 ;
			led = ~led;
		}
	}
	if(temp_zone == 2)//�¶�������2
	{
		if(ms >= 200)//0.2s��˸
		{
			ms = 0 ;
			led = ~led;
		}
	}
	if(Wr == 0)//�¶������޵������Ƿ����
	{
		if(set == 0)
			alm_deal();//����������led�Ⱥͼ̵���
	}
	temp_t++;//�¶�ת���ȴ�ʱ��
	display();//��ʾ
}
void alm_deal()//�ƹ���ƣ����ж���
{
	P0 = 0xfd|led;Y4;Y0;
	if(temp_zone == 0)
	{
		P0 = 0x00;Y5;Y0;
	}
	else if(temp_zone == 1)
	{
		P0 = 0x00;Y5;Y0;
	}
	else if(temp_zone == 2)
	{
		P0 = (1<<4);Y5;Y0;
	}
}
void display()
{
	P0 = 0x00;Y6;Y0;
	P0 = table[nu[ds_i]];Y7;Y0;
	P0 = 0x01<<ds_i;Y6;Y0;
	ds_i++;
	ds_i %= 8;
}
void get_temp()
{
	uchar LSB,MSB;
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44);
	temp_t = 0;
	while(temp_t < 94)//�ȴ�94ms���¶�ת����Ҫ
	{
		key_deal();
	}
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE);
	LSB = Read_DS18B20();
	MSB = Read_DS18B20();
	temp = (MSB<<4)|(LSB>>4);
	if((LSB<<4) == 0x80)
		temp ++;
	if(set ==0) // δ��������ģʽ�²ſ��Ը���nu[]��ֵ����Ȼ������ģʽ���޷�������ʾ
	{
		nu[6] = temp/10;
		nu[7] = temp%10;
		if(temp>temp_max)	//�ж��¶���������
			temp_zone = 2;
		else if(temp<temp_min)
			temp_zone = 0;
		else 
			temp_zone = 1;
		nu[1] = temp_zone;
	}
}
void key_deal()
{
	if(key_f == 1)
	{
		key_f = 0;
		Kb = KeyScan();
		if(Kb != 66)
		{
			if(set == 0)//��ǰ���¶���ʾ����
			{
				if(Kb == 10)	//���¡����á������¶����ý���
				{
					Y4;P0 = 0xff;Y0;//����ģʽ�µƹⲻӦ������
					set = 1;
					nu[0]=10;nu[1]=11;nu[2]=11;nu[3]=11;	//�������ȴ����빤������
					nu[4]=11;nu[5]=10;nu[6]=11;nu[7]=11;
				}
			}
			else if(set == 1)//��ǰ���¶����ý���
			{
				if(Kb == 10)//���¡����á����Ա�������
				{
					if(set_i == 4)//�������
					{
						if(temp_max>=temp_min)//����������ȷ---���Ա�������
						{
							Y4;P0 = 0xff;Y0;Wr = 0;
							set = 0;
							set_i = 0;
							nu[0]=10;nu[1]=temp_zone;nu[2]=10;nu[3]=11;
							nu[4]=11;nu[5]=11;nu[6]=temp/10;nu[7]=temp%10;
						}
						else//�����������ô���
						{
							Wr = 1;//��ʾ����ֹled��������������˸
							Y4;P0 = ~(1<<1);Y0;
							set_i=0;
							nu[0]=10;nu[1]=11;nu[2]=11;nu[3]=11;//�������ȴ���������
							nu[4]=11;nu[5]=10;nu[6]=11;nu[7]=11;
						}
					}
				}
				else//���µĲ��ǡ����á�
				{
					if(Kb == 11)//����
					{
						set_i = 0;
						nu[0]=10;nu[1]=11;nu[2]=11;nu[3]=11;//�������ȴ���������
						nu[4]=11;nu[5]=10;nu[6]=11;nu[7]=11;
					}
					else if(set_i == 0)	//�����һ������
					{
						temp_max = Kb*10;//max��ʮλ
						nu[1]=Kb;
						set_i++;
					}
					else if(set_i == 1)//����ڶ�������
					{
						temp_max += Kb;//max�ĸ�λ
						nu[2]=Kb;
						set_i++;
					}
					else if(set_i == 2)//�������������
					{
						temp_min = Kb*10;
						nu[6]=Kb;
						set_i++;
					}
					else if(set_i == 3)//������ĸ�����
					{
						temp_min += Kb;
						nu[7]=Kb;
						set_i++;
					}
				}
			}
		}
	}
}
uchar KeyScan()
{
	R1 = 0;R2 = 1;R3 = 1;R4 = 1;
	readkey = ~KeyPort;
	Pre1 = readkey&(readkey^LastKey1);
	LastKey1 = readkey;
	if(Pre1&0x08)
		return 9;
	if(Pre1&0x04)
		return 6;
	if(Pre1&0x02)
		return 3;
	if(Pre1&0x01)
		return 0;
	
	R1 = 1;R2 = 0;R3 = 1;R4 = 1;
	readkey = ~KeyPort;
	Pre2 = readkey&(readkey^LastKey2);
	LastKey2 = readkey;
	if(Pre2&0x08)
		return 10;//shezhi
	if(Pre2&0x04)
		return 7;
	if(Pre2&0x02)
		return 4;
	if(Pre2&0x01)
		return 1;
	
	R1 = 1;R2 = 1;R3 = 0;R4 = 1;
	readkey = ~KeyPort;
	Pre3 = readkey&(readkey^LastKey3);
	LastKey3 = readkey;
	if(Pre3&0x08)
		return 11;//clr
	if(Pre3&0x04)
		return 8;
	if(Pre3&0x02)
		return 5;
	if(Pre3&0x01)
		return 2;
	
	R1 = 1;R2 = 1;R3 = 1;R4 = 0;
	readkey = ~KeyPort;
	Pre4 = readkey&(readkey^LastKey4);
	LastKey4 = readkey;
	if(Pre4&0x08)
		return 66;
	if(Pre4&0x04)
		return 66;
	if(Pre4&0x02)
		return 66;
	if(Pre4&0x01)
		return 66;
	return 66;
}
void Timer0Init(void)		//1����@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xCD;		//���ö�ʱ��ֵ
	TH0 = 0xD4;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	EA = 1;
	ET0 = 1;
}

void sys_init()
{
	Y4;P0 = 0xff;Y0;
	Y5;P0 = 0x00;Y0;
	Init_Temp();
}
void Init_Temp()
{
	Init_DS18B20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x4E);
	Write_DS18B20(100);
	Write_DS18B20(0);
	Write_DS18B20(0x1F);
	temp_max = 30;
	temp_min = 20;
}