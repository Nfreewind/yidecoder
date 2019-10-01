
#ifndef __YYDECODER_H
#define __YYDECODER_H

#include "stdio.h"
#include "string.h"
#include "yy/img_scanner_qr.h"
#include "yy/img_scanner_line.h"

#define YY_MAX_SYM_LEN 256				//解码结果最大个数

typedef enum
{
	YY_QR		= 0xFC,
	YY_NONE		= 0xFF,
}YyCodeType;

typedef struct
{
	unsigned char data[YY_MAX_SYM_LEN];
	unsigned char len;
	unsigned char type;
	unsigned char ttl;
}YySymbol;

extern YySymbol yy_symbol;				//输出结果

void yy_symvalid_clear(void);
void yy_symvalid_ttl(void);
void yy_symvalid_ttl_wait(unsigned char status);
unsigned char yy_symvalid_get_state(void);
unsigned char yy_symvalid_get_ttl_state(void);
void yy_decoder_get_sym(unsigned char *type, unsigned char *len, unsigned char *outsym);
unsigned char yy_decoder_main(YyImgInterface *imgif, YyImgRegion *region);

#endif


