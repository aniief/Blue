#include "define.h"
#include "main.h"
#include "stc15.h"
#include "INTRINS.H"
void cls_buzz()
{
	Y5;P06 = 0;P04 = 0;
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
	EA = 1;
}
void Timer0Int() interrupt 1
{
	led_f_t++;//led灯的运行间隔(400ms---1200ms)
	if(led_f_t == led_t[run_mode])
	{
		led_f_t = 0;
		led_f = 1;
	}
	
	pwm_t++;//调节亮度的pwm
	if(pwm_t <= light)
	{
		led = 0x00;
	}
	else led = 0xff;
	if(pwm_t == 4)
		pwm_t = 0;
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
				if(led_i==9) led_i = 0;
				break;
			case 1:
				if(led_f==1)
				{
					led_f=0;
					led_i++;
				}
				Y4;P0 = led|(0xff>>led_i);Y0;
				if(led_i==9) led_i = 0;
				break;
			case 2:
				if(led_f==1)
				{
					led_f=0;
					led_i++;
				}
				Y4;P0 = led | ~((0x01<<led_i)|(0x80>>led_i));Y0;
				if(led_i==4) led_i = 0;
				break;
			case 3:
				if(led_f==1)
				{
					led_f=0;
					led_i++;
				}
				Y4;P0 = led|~((0x10<<led_i)|(0x08>>led_i));Y0;
				if(led_i==4) led_i = 0;
				break;
			}
		
}

void main()
{
	cls_buzz();
	Timer0Init();
	led_t[0] = led_t[1] = led_t[2] = led_t[3] = 500;
	light = 3;
	run_mode=0;
	while(1)
	{
		
	}
}