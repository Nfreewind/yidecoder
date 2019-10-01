
#ifndef _YY_SYMVALID_H_
#define _YY_SYMVALID_H_

#include "basic.h"

void yy_symvalid_clear(void);
void yy_symvalid_ttl(void);
void yy_symvalid_ttl_wait(unsigned char status);
unsigned char yy_symvalid_get_state(void);
unsigned char yy_symvalid_get_ttl_state(void);
unsigned short yy_symvalid_get_lock_type(void);
void yy_symvalid_ttl_reset(unsigned char type);
unsigned char yy_symvalid(YySymbol *yy_symbol);
unsigned char yy_symvalid_continuous(YySymbol *yy_symbol);

#endif


