
#ifndef _YY_DECODERSET_H_
#define _YY_DECODERSET_H_

#include "basic.h"

#define beep_sound(x)
#define delay_ms(x)

void yy_decset_save(void);
void yy_decset_load(void);
void yy_decset_init(void);
unsigned char yy_decset_get_state(void);
unsigned char yy_decset_set(YySymbol *yy_symbol);

#endif


