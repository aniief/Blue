#include "stc15.h"
#include "ds1302.c"
#include "main.h"
#include "iic.c"
void main()
{
	cls_buzz();
	Init_Time();
	read_hum();
	Timer0Init();
	while(1)
	{
		get_hum();
		get_time();
		key_deal();
	}
}
void Timer0Int() interrupt 1
{
	key_t++;
	if(key_t == 10)
	{
		key_t = 0;
		key_f = 1;
	}
/**********�ֶ�ģʽ�µĴ���*********/
	if(mode == 0)
	{
		//con&0x10 ֻ�����̵�����ԭ��״̬������������ر�
		//������ alm_f ��  alm_on��ͬ����
		con = (con&0x10)|((alm_f&alm_on)<<6);
		con &= 0x50;P0 = con;Y5;Y0;
		
		if(led2_f == 1)			//����
		{
			P0 = 0xfd;Y4;Y0;		   //��L2
			con = (con&0x40)|0x10; //&0x40 �رշ��������������   |0x10�򿪼̵���
			P0 = con;Y5;Y0;
		}
		else 							//�ص�
		{ con = con&0x40;			//�رռ̵���,����������ԭ״̬
			P0 = 0xff;Y4;Y0;		//��L2
			P0 = con;Y5;Y0;	
		}
	}
	/**********�Զ�ģʽ�µĴ���*************/
	else if(mode == 1)
	{
		//�Զ�ģʽ��ֻ��Ҫ���Ƽ̵�����ֱ�Ӷ�P0�ڲ������ɡ�
		if(led1_f == 1)			//���
		{
			P0 = 0xfe;Y4;Y0;	//��L1
			P0 = 0x10;Y5;Y0; //�򿪼̵���
		}
		else 
			{	P0 = 0xff;Y4;Y0;//�ر�L1
				P0 = 0x00;Y5;Y0;//�رռ̵���
			}
	}
	display();//ÿ1msɨ��һ������ܣ�ÿ8msɨ��һ��
}
void KeyScan()//����ɨ�裬Ҫ�������Ķ���//�϶�������д�����Ķ���
{
	readkey = ~P3;
	Pre = readkey&(readkey^TheKey);
	TheKey = readkey;
}
void key_deal()
{
	if(key_f == 1)		
	{
		key_f = 0;
		KeyScan();
		//�ֶ�ģʽ��һЩ����  ��Ӧ�Ĳ���
		if(mode == 0)	
		{
			if(Pre&0x01)		//S7
			{
				mode = 1;  		//�л����Զ�ģʽ
				alm_f = 0x00; //�رվ���
			}
			
			if(Pre&0x02) 		//S6  ��/�ر� �����ѹ��ܡ�
			{
				alm_on ++;
				alm_on %= 2;
			}
			if(Pre&0x04) 		//S5  L2����λ ��1
			{
				led2_f = 1;
			}
			else if(Pre&0x08)//S4  L2����λ ��0  
			{
				led2_f = 0;
			}
		}
		//�Զ�ģʽ��һЩ����  ��Ӧ�Ĳ���
		else if(mode == 1)
		{
			if(Pre&0x01)		//S7 
			{
				if(set == 0) 	// ���� ���� �ſ��Ը���ģʽ
				{							//�л�Ϊ �ֶ�ģʽ
					led2_f = 0;	
					mode = 0;
					con = 0;
				}
			}
			if(Pre&0x02) //S6  ���� �л�
			{
				if(set == 0) //��������ģʽ
				{
					set = 1;					//��������
					hum_set = hum_save;
					nu[6] = hum_set/10;
					nu[7] = hum_set%10;
				}
				else if(set == 1)	//������ģʽ
				{
					set = 0;		//�˳����ò��������
					hum_save = hum_set;
					save_hum();
				}
			}
			if(Pre&0x04) // ��ֵ++
			{
				if(set == 1)
				{
					if(hum_set<100)
					{
						hum_set++;
						nu[6] = hum_set/10;
						nu[7] = hum_set%10;
					}
				}
			}
			else if(Pre&0x08)//��ֵ--
			{
				if(set == 1)
				{
					if(hum_set > 0)
					{
						hum_set--;
						nu[6] = hum_set/10;
						nu[7] = hum_set%10;
					}
				}
			}
		}
	}
}

void display()
{
	if(set == 0)  //time
	{
		P0 = 0x00;Y6;Y0;
		P0 = table[time[ds_i]];Y7;Y0;
		P0 = 0x01<<ds_i;Y6;Y0;
		ds_i ++;
		ds_i %= 8;
	}
	else if(set == 1) //shezhi
	{
		P0 = 0x00;Y6;Y0;
		P0 = table[nu[ds_i]];Y7;Y0;
		P0 = 0x01<<ds_i;Y6;Y0;
		ds_i ++;
		ds_i %= 8;
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

void Init_Time()
{
	Write_Ds1302_Byte(0x8E,0x00);//�ر�д����
	Write_Ds1302_Byte(0x80,0x00);//0��
	Write_Ds1302_Byte(0x82,0x30);//30��
	Write_Ds1302_Byte(0x84,0x08);//8ʱ
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x03);
	IIC_WaitAck();
	IIC_Stop();
	
}
void cls_buzz()
{
	Y5;P0 = 0x00;Y0;
}
void get_time()
{
	s = Read_Ds1302_Byte(0x81);
	m = Read_Ds1302_Byte(0x83);
	h = Read_Ds1302_Byte(0x85);
	time[0] = h>>4;time[1] = h&0x0f;
	time[3] = m>>4;time[4] = m&0x0f;
}
void get_hum()
{
	ET0 = 0;				//��ȡ��ʱ��Ҫ���жϴ��

	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	hum = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	ET0 = 1;
	hum_per = hum*100/256;
	time[6] = hum_per/10;
	time[7] = hum_per%10;
	if(mode == 0)//�ֶ�ģʽ
	{
		if(hum_per < hum_save)
		{
			alm_f = 1;		//����������
		}
		else alm_f = 0;
	}
	if(mode == 1)//�Զ�ģʽ
	{
		if(hum_per < hum_save)
		{
			led1_f = 1;
		}
		else led1_f = 0;
	}
}
void read_hum()	//EEPROM��ȡʪ��
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_Stop();
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	hum_save = IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
}
void save_hum()	//EEPROM����ʪ��
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_SendByte(hum_save);
	IIC_WaitAck();
	IIC_Stop();
}