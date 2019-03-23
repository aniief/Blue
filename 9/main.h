#include "define.h"
//段码
uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
uchar nu[8];
//按键用
uint key_f_t;//
uchar key_f;//10ms标志
uchar Trg,Cont,ReadData;//储存扫描的按键数据
//led用
uchar run_mode;//当前运行的模式
uint led_f_t;
uchar led_f;//流转标志
uchar led_run_f=0;//停止/运行标志
uchar led_i;//灯编号
//pwm设置亮度使用
uchar light,pwm_t,led;//亮度等级、pww_t、具有pwm性质的变量
uchar light_dat;//Rb2读取的数据
//数码管用
uint ds_i;//数码管编号
uchar ds_t;
uchar ds_f=0xff;//数码管亮灭标志
//很多都在用
uchar mode = 0;//储存模式
uchar set;//设置编号
uint ms;//100us 转ms
uchar ms_t;//
uint led_t[4];//变化间隔

void Timer0Init(void);//定时器0,1ms
void cls_buzz();//关闭蜂鸣器、继电器
void Delay1ms();//1ms延时函数
void Timer0Init(void);//定时器0初始化函数， 0.1ms
void CA();//计算每个数码管应显示的数值
void led_run();
void display();
void display_time();
void display_mode();
void KeyScan();
void key_deal();
void Write_24C02(uchar addr, uchar dat);//写24C02寄存器
uchar Read_24C02(uchar addr);
void load();
void Read_AIN3();
void get_light();