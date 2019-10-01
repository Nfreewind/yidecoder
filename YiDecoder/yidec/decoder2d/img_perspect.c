
#include "decoder2d/img_perspect.h"

//解投影变换矩阵A(a(ij));A将采样网格的点变换到图像坐标
//(xn,yn)是4组对应点;分别是左上,右上,右下,左下
void img_perspect_calc(YyImgPerspect *p)
{
	p->dx1 = p->x1 - p->x2;
	p->dx2 = p->x3 - p->x2;
	p->dx3 = p->x0 - p->x1 + p->x2 - p->x3;
	p->dy1 = p->y1 - p->y2;
	p->dy2 = p->y3 - p->y2;
	p->dy3 = p->y0 - p->y1 + p->y2 - p->y3;

	if (p->dx3 == 0 && p->dy3 == 0)
	{
		p->a31 = 0;
		p->a32 = 0;
	}
	else
	{
		p->a31 = (p->dx3 * p->dy2 - p->dx2 * p->dy3) / (p->dx1 * p->dy2 - p->dx2 * p->dy1);
		p->a32 = (p->dx1 * p->dy3 - p->dx3 * p->dy1) / (p->dx1 * p->dy2 - p->dx2 * p->dy1);
	}
	p->a21 = p->y1 - p->y0 + p->a31 * p->y1;
	p->a22 = p->y3 - p->y0 + p->a32 * p->y3;
	p->a11 = p->x1 - p->x0 + p->a31 * p->x1;
	p->a12 = p->x3 - p->x0 + p->a32 * p->x3;
	p->a13 = p->x0;
	p->a23 = p->y0;
}

void img_perspect_k2pix(YyImgPerspect *p, float gxf, float gyf, float *px, float *py)
{
	float imu = p->a11 * gxf + p->a12 * gyf + p->a13;
	float imv = p->a21 * gxf + p->a22 * gyf + p->a23;
	float imw = p->a31 * gxf + p->a32 * gyf + 1;
	*px = imu / imw;
	*py = imv / imw;
}
