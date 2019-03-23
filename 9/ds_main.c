#include "define.h"
#include "main.h"
#include "stc15.h"
#include "INTRINS.H"

void cls_buzz()
{
	Y5;P06 = 0;P04 = 0;
}
void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 95;
	do
	{
		while (--j);
	} while (--i);
}

void Timer0Init(void)		//0.1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xAE;		//设置定时初值
	TH0 = 0xFB;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}
void Timer0Int() interrupt 1
{
	//led灯
	led_f_t++;//led灯的运行间隔(400ms---1200ms)
	if(led_f_t == led_t[run_mode]*10)
	{
		led_f_t = 0;
		led_f = 1;
	}
	
	//数码管
	ds_t++;
	if(ds_t == 8000)
	{
		ds_t = 0;
		ds_f = ~ds_f;
	}
		
	//调节亮度
	pwm_t++;//调节亮度的pwm
	if(pwm_t <= light)
	{
		led = 0x00;
	}
	else led = 0xff;
	if(pwm_t == 4)
		pwm_t = 0;
//	led_run();
}

void led_run()
{
		switch(run_mode)
		{
			case 0:
				if(led_f==1)
				{
					led_f=0;
					led_i++;
				}
				Y4;P0 = led|(0xff<<led_i);Y0;
				if(led_i==9) {led_i = 0;run_mode++;run_mode=run_mode%4;}
				break;
			case 1:
				if(led_f==1)
				{
					led_f=0;
					led_i++;
				}
				Y4;P0 = led|(0xff>>led_i);Y0;
				if(led_i==9) {led_i = 0;run_mode++;run_mode=run_mode%4;}
				break;
			case 2:
				if(led_f==1)
				{
					led_f=0;
					led_i++;
				}
				Y4;P0 = led | ~((0x01<<led_i)|(0x80>>led_i));Y0;
				if(led_i==5) {led_i = 0;run_mode++;run_mode=run_mode%4;}
				break;
			case 3:
				if(led_f==1)
				{
					led_f=0;
					led_i++;
				}
				Y4;P0 = led|~((0x10<<led_i)|(0x08>>led_i));Y0;
				if(led_i==5) {led_i = 0;run_mode++;run_mode=run_mode%4;}
				break;
			}
		
}

void display()
{
	if(ds_f)
	{
		if(ds_i<=3)
			display_mode();
	}
	if(ds_i >=4)
		display_time();
	ds_i++;
	if(ds_i==9)  ds_i = 0;
	Delay1ms();
}

void display_mode()
{
	Y6;P0 = 0x00;Y0;
	Y7;P0 = table[nu[ds_i]];Y0;
	Y6;P0 = 0x01<<ds_i;Y0;
}
void display_time()
{
	Y6;P0 = 0x00;Y0;
	Y7;P0 = table[nu[ds_i]];Y0;
	Y6;P0 = 0x01<<ds_i;Y0;
}

void main()
{
	cls_buzz();
	Timer0Init();
	light = 1;
	mode = 3;
	set = 1;
	run_mode=0;
	while(1)
	{
		display();
	}
}