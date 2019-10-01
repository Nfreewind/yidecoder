
#ifndef _YY_IMG_CURFIT_H_
#define _YY_IMG_CURFIT_H_

#include "decoder2d/img_scanner.h"

typedef struct
{
	float arg[2];
	float *px;
	float *py;
	int maxlen;
	int pnum;
	int base;
	int type;
	int ccwt;
	int offs;
	float devi;
}YyImgCurfit;

void yy_img_curfit_line(YyImgCurfit *fit);
unsigned char yy_img_curfit_line_isect(YyImgCurfit *fit0, YyImgCurfit *fit1, float *p);
void yy_img_curfit_line_type(YyImgCurfit *fit, int dx, int dy);
void yy_img_curfit_line_filter(YyImgCurfit *fit);
void yy_img_curfit_line_select(YyImgCurfit *fit);
void yy_img_curfit_line_edge_region(YyImgInterface *imgif, YyImgRegion *region, YyImgCurfit *fit, int checkinit, int *edge_thres);
void yy_img_curfit_line_edge_1ep(YyImgInterface *imgif, YyImgCurfit *fit);
void yy_img_curfit_line_edge_2ep(YyImgInterface *imgif, YyImgCurfit *fit);
void yy_img_curfit_line_draw(YyImgInterface *imgif, YyImgCurfit *fit);
void yy_img_curfit_point_draw(YyImgInterface *imgif, YyImgCurfit *fit);

#endif //_YY_IMG_CURFIT_H_
