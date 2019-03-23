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
/**********手动模式下的处理*********/
	if(mode == 0)
	{
		//con&0x10 只保留继电器的原来状态，其余外设均关闭
		//蜂鸣器 alm_f 和  alm_on共同决定
		con = (con&0x10)|((alm_f&alm_on)<<6);
		con &= 0x50;P0 = con;Y5;Y0;
		
		if(led2_f == 1)			//开灯
		{
			P0 = 0xfd;Y4;Y0;		   //打开L2
			con = (con&0x40)|0x10; //&0x40 关闭蜂鸣器意外的所有   |0x10打开继电器
			P0 = con;Y5;Y0;
		}
		else 							//关灯
		{ con = con&0x40;			//关闭继电器,蜂鸣器保持原状态
			P0 = 0xff;Y4;Y0;		//关L2
			P0 = con;Y5;Y0;	
		}
	}
	/**********自动模式下的处理*************/
	else if(mode == 1)
	{
		//自动模式在只需要控制继电器，直接对P0口操作即可。
		if(led1_f == 1)			//点灯
		{
			P0 = 0xfe;Y4;Y0;	//打开L1
			P0 = 0x10;Y5;Y0; //打开继电器
		}
		else 
			{	P0 = 0xff;Y4;Y0;//关闭L1
				P0 = 0x00;Y5;Y0;//关闭继电器
			}
	}
	display();//每1ms扫描一个数码管，每8ms扫描一次
}
void KeyScan()//按键扫描，要背下来的东西//肯定不是我写的来的东西
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
		//手动模式的一些按键  对应的操作
		if(mode == 0)	
		{
			if(Pre&0x01)		//S7
			{
				mode = 1;  		//切换到自动模式
				alm_f = 0x00; //关闭警报
			}
			
			if(Pre&0x02) 		//S6  打开/关闭 “提醒功能”
			{
				alm_on ++;
				alm_on %= 2;
			}
			if(Pre&0x04) 		//S5  L2控制位 置1
			{
				led2_f = 1;
			}
			else if(Pre&0x08)//S4  L2控制位 置0  
			{
				led2_f = 0;
			}
		}
		//自动模式的一些按键  对应的操作
		else if(mode == 1)
		{
			if(Pre&0x01)		//S7 
			{
				if(set == 0) 	// 不在 设置 才可以更改模式
				{							//切换为 手动模式
					led2_f = 0;	
					mode = 0;
					con = 0;
				}
			}
			if(Pre&0x02) //S6  设置 切换
			{
				if(set == 0) //不在设置模式
				{
					set = 1;					//进入设置
					hum_set = hum_save;
					nu[6] = hum_set/10;
					nu[7] = hum_set%10;
				}
				else if(set == 1)	//在设置模式
				{
					set = 0;		//退出设置并保存参数
					hum_save = hum_set;
					save_hum();
				}
			}
			if(Pre&0x04) // 阈值++
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
			else if(Pre&0x08)//阈值--
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

void Init_Time()
{
	Write_Ds1302_Byte(0x8E,0x00);//关闭写保护
	Write_Ds1302_Byte(0x80,0x00);//0秒
	Write_Ds1302_Byte(0x82,0x30);//30分
	Write_Ds1302_Byte(0x84,0x08);//8时
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
	ET0 = 0;				//读取的时候不要让中断打断

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
	if(mode == 0)//手动模式
	{
		if(hum_per < hum_save)
		{
			alm_f = 1;		//蜂鸣器提醒
		}
		else alm_f = 0;
	}
	if(mode == 1)//自动模式
	{
		if(hum_per < hum_save)
		{
			led1_f = 1;
		}
		else led1_f = 0;
	}
}
void read_hum()	//EEPROM读取湿度
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
void save_hum()	//EEPROM保存湿度
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