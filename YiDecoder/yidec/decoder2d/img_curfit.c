
#include "decoder2d/img_curfit.h"

void yy_img_curfit_line(YyImgCurfit *fit)
{
	float ATA[4] = { 0 };
	float ATL[2] = { 0 };

	//计算ATA,ATL
	for (int i = 0; i < fit->pnum; i++)
	{
		ATA[0] += fit->px[i] * fit->px[i];
		ATA[1] += fit->px[i] * fit->py[i];
		ATA[3] += fit->py[i] * fit->py[i];
		ATL[0] += fit->px[i];
		ATL[1] += fit->py[i];
	}
	ATA[2] = ATA[1];

	//inv(ATA)
	float sdet = 1 / (ATA[0] * ATA[3] - ATA[1] * ATA[2]);
	if (fabsf(sdet) * fit->pnum  * fit->pnum < 0.00005f)
	{
		float t = ATA[0] * sdet;
		ATA[0] = ATA[3] * sdet;
		ATA[3] = t;
		ATA[1] = -ATA[1] * sdet;
		ATA[2] = -ATA[2] * sdet;

		fit->arg[0] = ATA[0] * ATL[0] + ATA[1] * ATL[1];
		fit->arg[1] = ATA[2] * ATL[0] + ATA[3] * ATL[1];
	}
	else
	{
		//若sdet太小，求逆存在奇异值问题，改用y=kx+b求解
		ATA[1] = ATL[0];
		ATL[0] = ATA[2];
		ATA[2] = ATA[1];
		ATA[3] = fit->pnum;

		sdet = 1 / (ATA[0] * ATA[3] - ATA[1] * ATA[2]);

		float t = ATA[0] * sdet;
		ATA[0] = ATA[3] * sdet;
		ATA[3] = t;
		ATA[1] = -ATA[1] * sdet;
		ATA[2] = -ATA[2] * sdet;

		float k = ATA[0] * ATL[0] + ATA[1] * ATL[1];
		float b = ATA[2] * ATL[0] + ATA[3] * ATL[1];

		if (b == 0)
			b = 1e-10f;

		//方程为arg[0]x+arg[1]y=1，用k和b转换
		fit->arg[0] = -k / b;
		fit->arg[1] = 1 / b;
	}
}

unsigned char yy_img_curfit_line_isect(YyImgCurfit *fit0, YyImgCurfit *fit1, float *p)
{
	float d;
	float x, y;
	d = fit0->arg[0] * fit1->arg[1] - fit0->arg[1] * fit1->arg[0];
	if (d == 0)
		return 1;
	x = fit1->arg[1] - fit0->arg[1];
	y = fit0->arg[0] - fit1->arg[0];
	p[0] = x / d;
	p[1] = y / d;
	return 0;
}

void yy_img_curfit_line_type(YyImgCurfit *fit, int dx, int dy)
{
	if (fabs(fit->arg[0]) <= fabs(fit->arg[1]))
	{
		fit->type = 0;
		if (dy >= 0)
			fit->ccwt = -1;
		else
			fit->ccwt = 1;
	}
	else
	{
		fit->type = 1;
		if (dx >= 0)
			fit->ccwt = -1;
		else
			fit->ccwt = 1;
	}
}

void yy_img_curfit_line_filter(YyImgCurfit *fit)
{
	int idx = fit->base;
	float nor = 4 / (fit->arg[0] * fit->arg[0] + fit->arg[1] * fit->arg[1]);
	for (int i = fit->base; i < fit->pnum; i++)
	{
		float d = fit->arg[0] * fit->px[i] + fit->arg[1] * fit->py[i] - 1;
		if (fit->type == 0 && d * fit->arg[1] * fit->ccwt > 0
			|| fit->type == 1 && d * fit->arg[0] * fit->ccwt > 0
			|| d * d * nor < fit->offs)
		{
			fit->px[idx] = fit->px[i];
			fit->py[idx] = fit->py[i];
			idx++;
		}
	}
	fit->pnum = idx;
}

void yy_img_curfit_line_select(YyImgCurfit *fit)
{
	int idx = 0;
	float nor = 1 / (fit->arg[0] * fit->arg[0] + fit->arg[1] * fit->arg[1]);
	for (int i = 0; i < fit->pnum; i++)
	{
		float d = fit->arg[0] * fit->px[i] + fit->arg[1] * fit->py[i] - 1;
		if (d * d * nor < fit->offs)
		{
			fit->px[idx] = fit->px[i];
			fit->py[idx] = fit->py[i];
			idx++;
		}
	}
	fit->pnum = idx;
}

void yy_img_curfit_line_edge_region(YyImgInterface *imgif, YyImgRegion *region, YyImgCurfit *fit, int checkinit, int *edge_thres)
{
	int idx = fit->pnum;
	if (idx >= fit->maxlen - 1)
		return;

	int dir = 4;

	int checkoffs = fit[0].offs;
	if (checkinit < 0)
		checkinit = checkoffs;

	if (fit->type == 0)
	{
		if (region->w < 0)
			dir = -dir;
		for (int i = region->x; dir * i < dir * (region->x + region->w); i += dir)
		{
			int j = (1 - fit->arg[0] * i) / fit->arg[1] + 0.5f;
			if (j > fit[0].offs && j < imgif->h - 1 - fit[0].offs)
			{
				int max = 0, maxn;
				unsigned char last = imgif->img[(j + fit->ccwt * (fit[0].offs + 1)) * imgif->w + i];
				for (int n = fit->ccwt * fit[0].offs; fit->ccwt * n >= -(fit[0].offs + 1) / 2; n -= fit->ccwt)
				{
					unsigned char now = imgif->img[(j + n) * imgif->w + i];
					int d = (int)last - now;
					last = now;
					if (max < d)
					{
						max = d;
						maxn = n;
						continue;
					}
					if (max > *edge_thres)
						break;
				}
				if (max > *edge_thres)
				{
					if(*edge_thres < max / 2)
						*edge_thres = max / 2;
					float x = i;
					float y = j + maxn + fit->ccwt * 0.5f;
					if (idx < fit->pnum + checkinit)
					{
						fit->px[idx] = x;
						fit->py[idx] = y;
						idx++;
						if (idx >= fit->maxlen)
							break;
					}
					else
					{
						float dx = fit->px[idx - checkoffs] - x;
						float dy = fit->py[idx - checkoffs] - y;
						float k = dy / dx;
						float k0 = -fit->arg[0] / fit->arg[1];
						float dk = atanf(k) - atanf(k0);
						if (dk > -fit->devi && dk < fit->devi)
						{
							fit->px[idx] = x;
							fit->py[idx] = y;
							idx++;
							if (idx >= fit->maxlen)
								break;
						}
					}
				}
			}
		}
	}
	else
	{
		if (region->h < 0)
			dir = -dir;
		for (int i = region->y; dir * i < dir * (region->y + region->h); i += dir)
		{
			int j = (1 - fit->arg[1] * i) / fit->arg[0] + 0.5f;
			if (j > fit[0].offs && j < imgif->w - 1 - fit[0].offs)
			{
				int max = 0, maxn;
				unsigned char last = imgif->img[i * imgif->w + (j + fit->ccwt * (fit[0].offs + 1))];
				for (int n = fit->ccwt * fit[0].offs; fit->ccwt * n >= -(fit[0].offs + 1) / 2; n -= fit->ccwt)
				{
					unsigned char now = imgif->img[i * imgif->w + (j + n)];
					int d = (int)last - now;
					last = now;
					if (max < d)
					{
						max = d;
						maxn = n;
						continue;
					}
					if (max > *edge_thres)
						break;
				}
				if (max > *edge_thres)
				{
					if (*edge_thres < max / 2)
						*edge_thres = max / 2;
					float x = j + maxn + fit->ccwt * 0.5f;
					float y = i;
					if (idx < fit->pnum + checkinit)
					{
						fit->px[idx] = x;
						fit->py[idx] = y;
						idx++;
						if (idx >= fit->maxlen)
							break;
					}
					else
					{
						float dx = fit->px[idx - checkoffs] - x;
						float dy = fit->py[idx - checkoffs] - y;
						float k = dx / dy;
						float k0 = -fit->arg[1] / fit->arg[0];
						float dk = atanf(k) - atanf(k0);
						if (dk > -fit->devi && dk < fit->devi)
						{
							fit->px[idx] = x;
							fit->py[idx] = y;
							idx++;
							if (idx >= fit->maxlen)
								break;
						}
					}
				}
			}
		}
	}
	fit->pnum = idx;
}

void yy_img_curfit_line_edge_1ep(YyImgInterface *imgif, YyImgCurfit *fit)
{
	int idx = 0, idx2 = 0;
	int xs = fit->px[0];
	int ys = fit->py[0];
	int xe = fit->px[1];
	int ye = fit->py[1];
	int xee = fit->px[2];
	int yee = fit->py[2];
	int edge_thres = 10;

	if (xee < 0 || xee >= imgif->w || yee < 0 || yee >= imgif->h)
		return;

	fit->pnum = 0;

	YyImgRegion region;
	if (fit->type == 0)
	{
		int xm = (xs + xe) / 2;
		region.x = xm;
		region.w = xs - xm;
		yy_img_curfit_line_edge_region(imgif, &region, fit, -1, &edge_thres);
		region.x = xm;
		region.w = xe - xm;
		yy_img_curfit_line_edge_region(imgif, &region, fit, -1, &edge_thres);

		fit->base = fit->pnum;

		fit->devi = 0.5f;
		region.x = xe;
		region.w = xe - xs;
		int segnum = (xee - region.x) / region.w;
		for (int i = 0; i < segnum; i++)
		{
			yy_img_curfit_line(fit);
			yy_img_curfit_line_edge_region(imgif, &region, fit, 0, &edge_thres);
			region.x += region.w;
			yy_img_curfit_line(fit);
			yy_img_curfit_line_filter(fit);
		}
		region.w = xee - region.x;
		yy_img_curfit_line(fit);
		yy_img_curfit_line_edge_region(imgif, &region, fit, 0, &edge_thres);
		yy_img_curfit_line(fit);
		yy_img_curfit_line_filter(fit);
	}
	else
	{
		int ym = (ys + ye) / 2;
		region.y = ym;
		region.h = ys - ym;
		yy_img_curfit_line_edge_region(imgif, &region, fit, -1, &edge_thres);
		region.y = ym;
		region.h = ye - ym;
		yy_img_curfit_line_edge_region(imgif, &region, fit, -1, &edge_thres);

		fit->base = fit->pnum;

		fit->devi = 0.5f;
		region.y = ye;
		region.h = ye - ys;
		int segnum = (yee - region.y) / region.h;
		for (int i = 0; i < segnum; i++)
		{
			yy_img_curfit_line(fit);
			yy_img_curfit_line_edge_region(imgif, &region, fit, 0, &edge_thres);
			region.y += region.h;
			yy_img_curfit_line(fit);
			yy_img_curfit_line_filter(fit);
		}
		region.h = yee - region.y;
		yy_img_curfit_line(fit);
		yy_img_curfit_line_edge_region(imgif, &region, fit, 0, &edge_thres);
		yy_img_curfit_line(fit);
		yy_img_curfit_line_filter(fit);
	}
	yy_img_curfit_line(fit);
	yy_img_curfit_line_filter(fit);
	yy_img_curfit_line(fit);
	yy_img_curfit_line_select(fit);
	yy_img_curfit_line(fit);
}

void yy_img_curfit_line_edge_2ep(YyImgInterface *imgif, YyImgCurfit *fit)
{
	int idx = 0, idx2 = 0;
	int edge_thres;
	int t;
	int xs = fit->px[0];
	int ys = fit->py[0];
	int xe = fit->px[1];
	int ye = fit->py[1];
	if (xs > xe)
	{
		t = xe;
		xe = xs;
		xs = t;
	}
	if (ys > ye)
	{
		t = ye;
		ye = ys;
		ys = t;
	}

	fit->pnum = 0;
	fit->base = 0;

	YyImgRegion region;
	if (fit->type == 0)
	{
		int xm = (xs + xe) / 2;
		region.x = xs;
		region.y = ys;
		region.w = xm - xs + 1;
		region.h = ye - ys + 1;
		edge_thres = 10;
		yy_img_curfit_line_edge_region(imgif, &region, fit, -1, &edge_thres);
		region.x = xe;
		region.w = xm - xe;
		edge_thres = 10;
		yy_img_curfit_line_edge_region(imgif, &region, fit, -1, &edge_thres);
	}
	else
	{
		int ym = (ys + ye) / 2;
		region.x = xs;
		region.y = ys;
		region.w = xe - xs + 1;
		region.h = ym - ys + 1;
		edge_thres = 10;
		yy_img_curfit_line_edge_region(imgif, &region, fit, -1, &edge_thres);
		region.y = ye;
		region.h = ym - ye;
		edge_thres = 10;
		yy_img_curfit_line_edge_region(imgif, &region, fit, -1, &edge_thres);
	}
	yy_img_curfit_line(fit);
	yy_img_curfit_line_filter(fit);
	yy_img_curfit_line(fit);
	yy_img_curfit_line_select(fit);
	yy_img_curfit_line(fit);
}

void yy_img_curfit_line_draw(YyImgInterface *imgif, YyImgCurfit *fit)
{
	if (fit->type == 0)
	{
		for (int i = 0; i < imgif->w; i++)
		{
			int j = (1 - fit->arg[0] * i) / fit->arg[1] + 0.5f;
			if(j >= 0 && j < imgif->h)
				imgif->imdisp[j * imgif->w + i] = 0;
		}
	}
	else
	{
		for (int i = 0; i < imgif->h; i++)
		{
			int j = (1 - fit->arg[1] * i) / fit->arg[0] + 0.5f;
			if (j >= 0 && j < imgif->w)
				imgif->imdisp[i * imgif->w + j] = 0;
		}
	}
}

void yy_img_curfit_point_draw(YyImgInterface *imgif, YyImgCurfit *fit)
{
	for (int i = 0; i < fit->pnum; i++)
	{
		int x = fit->px[i] + 0.5f;
		int y = fit->py[i] + 0.5f;
		imgif->imdisp[y * imgif->w + x] = 255;
	}
}

