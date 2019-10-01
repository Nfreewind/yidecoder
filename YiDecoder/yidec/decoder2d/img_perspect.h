
#ifndef _YY_IMG_PERSPECT_H_
#define _YY_IMG_PERSPECT_H_

typedef struct
{
	float x0, x1, x2, x3, y0, y1, y2, y3;
	float dx1, dx2, dx3, dy1, dy2, dy3;
	float a11, a12, a13, a21, a22, a23, a31, a32;
	float sx, sy, scale;
	int valid;
}YyImgPerspect;

void img_perspect_calc(YyImgPerspect *p);
void img_perspect_k2pix(YyImgPerspect *p, float gxf, float gyf, float *px, float *py);

#endif //_YY_IMG_PERSPECT_H_
