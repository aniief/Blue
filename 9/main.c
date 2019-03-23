/*****************************************************/
//蓝桥杯第九届省赛《彩灯控制》
//作者；Fan
//Email：577594077@qq.com
//
/*****************************************************/
#include "define.h"
#include "main.h"
#include "stc15.h"
#include "iic.c"
#include "INTRINS.H"
//关闭蜂鸣器、继电器
void cls_buzz()
{
	Y5;P06 = 0;P04 = 0;Y0;
}
/*****1ms延时函数****/
void Delay1ms()		
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
/****定时器0初始化函数， 0.1ms*****/
//
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
/*****定时器0中断服务函数*****/
void Timer0Int() interrupt 1		//0.1ms
{
	//led灯
	led_f_t++; //led灯的运行间隔 (400ms---1200ms)
	if(led_f_t == led_t[run_mode]*10)	//这里*10 是因为储存的数是  40~120
	{
		led_f_t = 0;
		led_f = 1;
	}
	//按键
	key_f_t++;
	if(key_f_t==100)		//10ms
	{
		get_light();
		key_f_t = 0;
		key_f = 1;

		//数码管
		//要求0.8秒闪烁一次，这里10ms +1
		ds_t++;				
		if(ds_t == 80)  //10ms*80 = 800ms
		{
			ds_t = 0;
			ds_f = ~ds_f;
		}
	}
	

	
	/****模拟pwm****
	//这里周期为0.4ms
	*****/
	pwm_t++;
	if(pwm_t <= light)
	{
		led = 0x00;
	}
	else led = 0xff;
	if(pwm_t == 4)
		pwm_t = 0;

	//led和smg的显示
	ms_t++;
	if(ms_t==10)		//1ms  数码管显示
	{
		ms_t = 0;
		display();
	}
	led_run();	//此函数运行频率一定要小，要保证每次pwm输出改变后都运行一次
}

/**********计算每个数码管应显示的数值****/
void CA()
{
	nu[0] = 10;
	nu[1] = mode+1;
	nu[2] = 10;
	nu[3] = 11;
	
	if(led_t[mode]>=1000)
		nu[4] = led_t[mode]/1000;
	else nu[4] = 11;
	
	nu[5] = led_t[mode]%1000/100;
	nu[6] = led_t[mode]%100/10;
	nu[7] = led_t[mode]%10;
}

/*****四个模式下led运行***/
void led_run()
{
	if(led_run_f)
	{
		P0 = 0xff;
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
}

/*****设置模式在的数码管显示******/
void display()
{
	if(set == 1)
	{
		if(ds_f)
		{
			if(ds_i<=3)
				display_mode();
		}
		if(ds_i>=4)
			display_time();
		ds_i++;
		if(ds_i == 9) ds_i = 0;
	}
	else if(set ==2)
	{
		if(ds_f)
		{
			if(ds_i>=4)
				display_time();
		}
		if(ds_i<=3)
			display_mode();
		ds_i++;
		if(ds_i == 9) ds_i = 0;
	}
	else 
	{
		Y6;P0 = 0x00;Y0;
	}
}

/*******显示运行模式*******/
void display_mode()
{
	Y6;P0 = 0x00;Y0;
	Y7;P0 = table[nu[ds_i]];Y0;
	Y6;P0 = 0x01<<ds_i;Y0;
}
/*******显示对应模式的流水间隔****/
void display_time()
{
	Y6;P0 = 0x00;Y0;
	Y7;P0 = table[nu[ds_i]];Y0;
	Y6;P0 = 0x01<<ds_i;Y0;
}
/******扫描按键并保存*****/
void KeyScan()
{
	ReadData = ~KeyPort;
	Trg = ReadData & (ReadData ^ Cont);
	Cont = ReadData;
}
/*****处理按键扫描得到的数据*****/
void key_deal()
{
	if(key_f == 1)
	{
		key_f = 0;
		KeyScan();
		if(Trg & 0x01)//S7 暂停/启动
		{
			led_run_f = ~led_run_f;
			Y4;P0 = 0xff;Y0;
			run_mode = 0;
			led_i=0;
		}		
		if(Trg & 0x02)//S6   设置
		{
			set++;set = set%3;
		}		
		if(Trg & 0x04)//S5  加按键
		{
			if(set == 1)
			{
				mode ++;
				mode = mode%4;
			}
			else if(set == 2)
			{
				if(led_t[mode]<=1100)
				{
					led_t[mode]+=100;
					Write_24C02(0x00+mode+1,led_t[mode]);
				}
			}
		}
		if(Trg & 0x08)//S4  减按键
		{
			while(Trg||Cont)
			{
				if(key_f == 1)
				{
					key_f = 0;
					KeyScan();
					if(set == 0)
					{
						Y6;P0 = 0x00;Y0;
						Y7;P0 = table[light];Y0;
						Y6;P0 = 0x80;Y0;
						Delay1ms();
						Y6;P0 = 0x00;Y0;
						Y7;P0 = table[10];Y0;
						Y6;P0 = 0x40;Y0;
						Delay1ms();
					}
				}
			} 
			if(set == 1)//设置运行模式
			{
				mode += 3;
				mode = mode%4;
			}
			else if(set == 2)//时间间隔
			{
				if(led_t[mode]>=500)
				{
					led_t[mode]-=100;
					Write_24C02(0x00+mode+1,led_t[mode]/100);
				}
			}
		}
	}
}



/******写24C02*****/
//参数：addr:地址；dat:数据
void Write_24C02(unsigned char addr, unsigned char dat)
{
	IIC_Start();		//IIC总线起始信号					
	IIC_SendByte(0xa0); 	//24C02的写设备地址
	IIC_WaitAck();		//等待从机应答	
	IIC_SendByte(addr); 	//内存字节字节
	IIC_WaitAck(); 		//等待从机应答	
	IIC_SendByte(dat); 	//写入目标数据
	IIC_WaitAck();		//等待从机应答	
	IIC_Stop();		//IIC总线停止信号		
}
/******读24C02********/
//参数：addr:地址
unsigned char Read_24C02(unsigned char addr)
{
	unsigned char tmp;
	//进行一个伪写操作
	IIC_Start();		//IIC总线起始信号					
	IIC_SendByte(0xa0); 	//24C02写设备地址
	IIC_WaitAck();		//等待从机应答	
	IIC_SendByte(addr); 	//内存自己地址
	IIC_WaitAck(); 		//等待从机应答	
	//进行字节读操作
	IIC_Start();		//IIC总线起始信号					
	IIC_SendByte(0xa1); 	//24C02读设备地址
	IIC_WaitAck();		//等待从机应答	
	tmp = IIC_RecByte();	//读取目标数据
	IIC_Ack(0); 		//产生非应答信号		
	IIC_Stop();		//IIC总线停止信号			
	return tmp;
}
/*******读PCF8591通道三数据******/
void Read_AIN3()
{
	IIC_Start();		//IIC总线起始信号							
	IIC_SendByte(0x90); 	//PCF8591的写设备地址		
	IIC_WaitAck();  	//等待从机应答		
	IIC_SendByte(0x03); 	//写入PCF8591的控制字节		
	IIC_WaitAck();  	//等待从机应答						
	IIC_Stop(); 		//IIC总线停止信号					
	
	IIC_Start();		//IIC总线起始信号									
	IIC_SendByte(0x91); 	//PCF8591的读设备地址		
	IIC_WaitAck(); 		//等待从机应答		
	light_dat = IIC_RecByte();	//读取PCF8591通道3的数据 			
	IIC_Ack(0); 		//产生非应答信号				
	IIC_Stop(); 		//IIC总线停止信号					
}
/*****处理Rb2读数，转换为4个等级的亮度*****/
void get_light()
{
	Read_AIN3();
	if(light_dat>=186) light = 4;
	else if(light_dat>=124) light = 3;
	else if(light_dat>=62) light = 2;
	else light = 1;
}
/******开机从24C02加载数据*****/
void load()
{
	led_t[0] = Read_24C02(0x01)*100;
	if(led_t[0] > 1200)	led_t[0] = 1200;
	if(led_t[0] < 400)	led_t[0] = 400;
	led_t[1] = Read_24C02(0x02)*100;
	if(led_t[1] > 1200)	led_t[1] = 1200;
	if(led_t[1] < 400)	led_t[1] = 400;
	led_t[2] = Read_24C02(0x03)*100;
	if(led_t[2] > 1200)	led_t[2] = 1200;
	if(led_t[2] < 400)	led_t[2] = 400;
	led_t[3] = Read_24C02(0x04)*100;
	if(led_t[3] > 1200)	led_t[3] = 1200;
	if(led_t[3] > 400)	led_t[3] = 400;
}
void main()
{
	cls_buzz();
	Timer0Init();
	load();
	while(1)
	{
		key_deal();
		CA();
	}
}