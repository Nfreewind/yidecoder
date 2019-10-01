
#ifndef _YYDECODER_H_
#define _YYDECODER_H_

#include "basic.h"
#include "symedit.h"
#include "symvalid.h"
#include "decoderset.h"
#include "decoder1d/ean13.h"
#include "decoder1d/ean8.h"
#include "decoder1d/upca.h"
#include "decoder1d/upce.h"
#include "decoder1d/codebar.h"
#include "decoder1d/code128.h"
#include "decoder1d/code39.h"
#include "decoder1d/code93.h"
#include "decoder1d/code11.h"
#include "decoder1d/interleave25.h"
#include "decoder1d/industry25.h"
#include "decoder1d/matrix25.h"
#include "decoder1d/msi.h"
#include "decoder1d/pharma.h"
#include "decoder1d/addition.h"
#include "decoder2d/img_scanner_qr.h"
#include "decoder2d/img_scanner_line.h"

void yy_decoder_get_sym(unsigned char *type, unsigned char *len, unsigned char *outsym);
unsigned char yy_decoder_main(unsigned short *yy_data_buf);
unsigned char yy_decoder_2d_main(YyImgInterface *imgif, YyImgRegion *region);

#endif


