#include "reg52.h"
#include "define.h"
sfr AUXR = 0x8e;
sfr P4 = 0xC0;
uchar code table[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
uchar nu[9] = {10,10,10,10,10,10,10,10,11};

void cls_buzz();
void Timer1Init();
void Timer0Init();
void SysInit();
void SysRun();

uint ds_t;
bit ds_f;
void display();
void undisplay();
void display_pro();
void LoadNu(uchar,uchar,uhar,uchar);


#define KeyPort P3
uchar KeyValue;
uchar key_t;
bit key_f;
sbit C1 = P4^4;
sbit C2 = P4^2;
uchar TheKey1,PreKey1,LastKey1,TheKey2,PreKey2,LastKey2;
uchar ScanKey();
void KeyDeal();

uchar mode;
#define TIME 1
#define VOLTE 2
#define FREN 3
#define CHECK 4
uchar time_set;
uchar volt_set;
uchar fren_set;
uchar VH_temp,VL_temp;
uchar lastvolt,thevolt;

uchar times;

uchar event_h,event_m,event_s,event;
uchar volt;
uchar volt_f;
uint volt_t;
uchar VL,VH;
void InitVolt();
uchar GetVolt();
void DealVolt();

uchar time_s,time_m,time_h;
void InitTime();
void GetTime();

bit check_set;

void Write();

uchar Read_24C02(uchar addr);
void Write_24C02(uchar addr,uchar dat);
void LoadEvent();
uint ms;
uint f;
uchar ds_i;
#define T0 P34