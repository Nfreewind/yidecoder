
#ifndef _YY_IMG_SCANNER_LINE_H_
#define _YY_IMG_SCANNER_LINE_H_

#include "basic.h"
#include "decoder2d/img_scanner.h"
#include "stdlib.h"
#include "string.h"

extern unsigned short yy_data_buf[YY_MAX_DAT_NUM];
extern unsigned int yy_data_idx, yy_data_h_max, yy_data_v_max;

void yy_img_scanner_line(YyImgInterface *imgif, YyImgRegion *region);
void yy_img_scanner_line_blt(YyImgInterface *imgif, YyImgRegion *region);

#endif //_YY_IMG_SCANNER_LINE_H_
