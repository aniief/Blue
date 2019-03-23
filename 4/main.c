/*******************************/
//2019-03-08;Fri;
//尝试使用指针//不会用//找不到合适的地方用//不用也好
//在中断外的一些操作如果被中断打断会出错或者不合适，那就ET0 = 0;
//这个题目较简单
/*******************************/
#include "reg52.h"
#include "main.h"
#include "intrins.h"
#include "iic.c"

void main()
{
	sys_init();//系统初始化,关闭蜂鸣器LED灯，给数码管数组赋初值。
	init_PCF8591();//配置PCF8591，选取通道1
	Timer0Init();//定时器0初始化
	while(1)
	{
		key_deal();//按键相关
		get_PCF8591();//读取光敏电阻
	}
}

void Timer0Int() interrupt 1
{
	key_t++;
	if(key_t == 10)//按键
	{
		key_t = 0;
		key_f = 1;
	}
	if(cont_f == 1)//正在计数
	{
		ms++;
		if(ms == 100) //100ms   10ml = 0.01L
		{
			ms = 0;
			water ++;
			if(water < 9999)//数据在规定范围内
			{
				nu[4] = table[water/1000];
				nu[5] = table[water%1000/100]&0x7f;
				nu[6] = table[water%100/10];
				nu[7] = table[water%10];
			}
			else//结束并显示费用
			{
				cont_f = 0;
				P0 = 0;Y5;Y0;
				money = water / 2;
				nu[4] = table[money/1000];
				nu[5] = table[money%1000/100]&0x7f;//个位后需要跟一个小数点
				nu[6] = table[money%100/10];
				nu[7] = table[money%10];
			}
		}
	}
	if(led_f == 0)//灯灭 的时候 在去判断是否要点亮
	{				//使用标志位可以减少锁存器开关次数
		if(temp >= 64)//要点亮
		{
			led_f = 1;
			P0 = 0xfe;Y4;Y0;
		}
	}
	else //灯亮的时候  判断是否要灭灯
	{
		if(temp < 64) //要灭灯
		{
			led_f = 0;
			P0 = 0xff;Y4;Y0;
		}
	}
	display();
}

void display()
{
	P0 = 0x00;Y6;Y0;
	P0 = nu[ds_i];Y7;Y0;
	P0 = 0x01<<ds_i;Y6;Y0;
	Y0;
	ds_i ++;
	ds_i &= 7;
}
void key_deal()
{
	if(key_f == 1)
	{
		KeyScan();
		key_f = 0;
		if(Pre&0x01)//S7
		{
			if(cont_f == 0)//start count
			{
				ET0 = 0;
				cont_f = 1;
				water = 0;
				P0 = 1<<4;Y5;Y0;
				ET0 = 1;
			}
		}
		if(Pre&0x02)//S6
		{
			if(cont_f == 1)
			{
				ET0 = 0;
				P0 = 0;Y5;Y0;
				cont_f = 0;
				money = water / 2;
				nu[4] = table[money/1000];
				nu[5] = table[money%1000/100]&0x7f;
				nu[6] = table[money%100/10];
				nu[7] = table[money%10];
				ET0 = 1;
			}
		}
	}
}
void KeyScan()
{
	readkey = ~P3;
	Pre = readkey&(readkey^LastKey);
	LastKey = readkey;
}
void Timer0Init()
{
	TMOD &= 0xf0;
	AUXR |= 0x80;
	TH0 = 0xD4;
	TL0 = 0xCC;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}
void sys_init()
{
	P0=0x00;Y5;Y0;
	Y4;P0=0xff;Y0;
	nu[0] = table[11];
	nu[1] = table[0]&0x7f;
	nu[2] = table[5];
	nu[3] = table[0];
	nu[4] =	nu[6] = nu[7] = table[0];
	nu[5] = table[0]&0x7f;
}
void init_PCF8591()
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_Stop();
}
void get_PCF8591()
{
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	ET0 = 0;
	temp = IIC_RecByte();
	ET0 = 1;
	IIC_SendAck(1);
	IIC_Stop();
	/*
	nu[0] = table[temp/100];
	nu[1] = table[temp%100/10];
	nu[2] = table[temp%10];
	*/
}