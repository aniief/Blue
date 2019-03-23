#ifndef __DEFINE_H_
#define __DEFINE_H_

#include "stc15.h"

typedef unsigned char uchar;
typedef unsigned int uint;

#define Y0 P2 = 0x1f
#define Y4 P2 = (P2&0x1f)|0x80
#define Y5 P2 = (P2&0x1f)|0xa0
#define Y6 P2 = (P2&0x1f)|0xc0
#define Y7 P2 = (P2&0x1f)|0xe0


#endif