#include "define.h"

uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xc6};
uchar time[8]={10,10,10,10,10,10,10,10};
uchar alm[8]={10,10,10,10,10,10,10,10};
uchar h=23,m=59,s=50,ah=0,am=0,as=0;
uint ms=0;
uchar ds_i,ds_t;
bit ds_f;
uchar t_shan,a_shan;
uchar key_t,key_f;
uchar ReadKey,Key,Dif;
uchar beep_i=0,beep_f;
uint beep_t;
bit led;
uint temp;
uchar temp_i,jump;

void sys_init();
void Timer0Init();
void Timer0Int(void);
void display_time();
void display_alm();
void display_temp();
void ca();
void keyscan();
void keydeal();
void delay(uint);
void ReadTemperature();
void beep();