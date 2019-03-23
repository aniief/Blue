#include "main.h"
#include "ds1302.c"
void main()
{
	Timer0Init();
	UartInit();
	InitTime();
	EA = 1;
	while(1)
	{
		 GetTime();
	}
}
void Timer0Int() interrupt 1
{
	display();
}
void display()
{
	P0=0x00;Y6;Y0;
	P0=table[nu[ds_i]];Y7;Y0;
	P0=0x01<<ds_i;Y6;Y0;
	ds_i++;ds_i&=7;
}
void Rev_Byte (void) interrupt 4
{
	//ET0=0;
	if(RI)
	{
		RI = 0;
		st[rev_i] = SBUF;
		rev_i++;
	}
	if(rev_i == 5)
	{
		rev_i=0;
		SendChar(st[0]);
		SendChar(st[1]);
		SendChar(st[2]);
		SendChar(st[3]);
		SendChar(st[4]);
		if((st[0] == '-')&&(st[2] == '-'))
		{
			if(st[1] == '1')
			{
				st[3] -= '0';
				st[4] -= '0';
				time_h = 16*st[3]+st[4];
				if((time_h>=0)&&(time_h<61))
				{
					//Write_Ds1302_Byte(0x8e,0x00);
					Write_Ds1302_Byte(0x84,time_h);
				}
			}
			if(st[1] == '2')
			{
				st[3] -= '0';
				st[4] -= '0';
				time_m = 16*st[3]+st[4];
				if((time_m>=0)&&(time_m<61))
				{
					//Write_Ds1302_Byte(0x8e,0x00);
					Write_Ds1302_Byte(0x82,time_m);
				}
			}
			if(st[1] == '3')
			{
				st[3] -= '0';
				st[4] -= '0';
				time_s = 16*st[3]+st[4];
				if((time_s>=0)&&(time_s<61))
				{
					//Write_Ds1302_Byte(0x8e,0x00);
					Write_Ds1302_Byte(0x80,time_s);
				}
			}
		}
	}
	if(TI)
	{
		TI = 0;
	}
	//ET0=1;
}
void SendChar(uchar dat)//发送
{
	SBUF = dat;
	while(TI == 0);
	TI = 0;
}
void InitTime()
{
	Write_Ds1302_Byte(0x8e,0x00);
	Write_Ds1302_Byte(0x80,0x55);
	Write_Ds1302_Byte(0x82,0x59);
	Write_Ds1302_Byte(0x84,0x23);
}
void GetTime()
{
	time_s = Read_Ds1302_Byte(0x81);
	time_m = Read_Ds1302_Byte(0x83);
	time_h = Read_Ds1302_Byte(0x85);
	nu[0] = time_h >> 4;nu[1] = time_h&0x0f;
	nu[3] = time_m >> 4;nu[4] = time_m&0x0f;
	nu[6] = time_s >> 4;nu[7] = time_s&0x0f;
}
void UartInit(void)		//9600bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xE0;		//设定定时初值
	T2H = 0xFE;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	ES = 1;
}
void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
}
