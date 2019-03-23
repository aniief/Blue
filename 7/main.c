/*******蓝桥杯第七届省赛试题********/
//	@作者	： Fan
//	@email	： imzjfan@163.com
//	@time  	： 2019-3-11
/*********************************/

#include "main.h"		//函数变量定义	
#include "define.h"		//宏定义
#include "stc15.h"		//
#include "onewire.c"	//官方驱动
void main()
{
	cls_buzz();			
	init_temp();		//初始化DS18B20  将其精度更改为9位，这样转换时间为93.75ms
	Timer0Init();		//定时器0初始化
	while(1)
	{
		if(ds_f == 0)		//显示运行模式和剩余时间
		{
			nu[1] = mode;
			nu[5] = left_t/100;
			nu[6] = left_t%100/10;
			nu[7] = left_t%10;
			deal_key();
		}
		else if(ds_f == 1) 	//读取并显示温度
		{
			get_temp();	
		}
	}
}

void Timer0Int() interrupt 1 //0.1ms
{
	if(run_f&&(left_t != 0))			//处于运行状态  输出PWM和点灯	
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
		 * 此处由于题目中要求的P3^4无法直观的观察到期要求的PWM是否实现
		 * 故将pwm输出放在L5~L7。可直观的观察到PWM占空比的变化
		 * 按照题目要求应改为
		 * 上述的pwm取0,1反过来
		 * sbit pwm = P3^4;
		 * led = ~(1<<mode);
		 * 下面要移到  if(key_f == 1)里面更好点
		 */
		led = (led&0x0f)|(pwm&0xf0);
		led = (led&0xf0)|((~(1<<mode))&0x0f);  //把低四位置零后  第四位用来显示模式
		P0 = led;Y4;Y0;
	}
	else					//当前停止    停止PWM输出+关灯
	{
		pwm = 0xff;
	}
	key_t++;
	if(key_t % 10 == 0)		//1ms  数码管动态显示
	{	
		display();
	}
	if(key_t == 100) 			//	10ms 按键消抖
	{
		key_t = 0;
		key_f = 1;
		if(run_f && (left_t != 0))	//	运行ing   处理运行计时
		{
			time_s++;
			if(time_s == 100) //	1s  定时 -- 
			{
				time_s = 0;
				left_t --;
			}
		}
		else 
		{
			P0 = 0xff;Y4;Y0;		
		}
		temp_t += 10;	//温度转换等待时间 这里是10ms运行一次，所以要+=10；
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
			if(ds_f == 0) 	// 显示温度
			{
				ds_f = 1;
				nu[1] = 4;
				nu[7] = 11;
			}
			else			//显示模式和时间
			{
				ds_f = 0;
				nu[1] = mode;
				nu[5] = left_t/100;
				nu[6] = left_t%100/10;
				nu[7] = left_t%10;
			}
		}
		else if(Pre&0x02)	//S6  停止
		{
			run_f = 0;
			left_t = 0;
		}
		else if(Pre&0x04)	//S5 加时间
		{
			run_f = 1;
			left_t += 60;
			left_t %= 180;
		}
		else if(Pre&0x08)	//S4	换模式
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