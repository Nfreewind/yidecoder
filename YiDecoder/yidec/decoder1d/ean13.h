
#ifndef _YY_EAN13_H_
#define _YY_EAN13_H_

#include "basic.h"

extern const unsigned char EAN13_CODE[60];
extern const unsigned char EAN13_HEAD[60];

unsigned char yy_ean13(YyInterface *yy_interface);
unsigned char yy_ean13_lock(YyInterface *yy_interface);

#endif


