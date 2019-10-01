
#include "decoder2d/img_scanner_qr.h"
#include "quirc.h"

unsigned char qr_main(unsigned char *imget, unsigned int gridn)
{
	struct quirc_code code;
	struct quirc_data data;
	quirc_decode_error_t err;

	code.size = gridn;
	for (int y = 0; y < gridn; y++)
	{
		for (int x = 0; x < gridn; x++)
		{
			int i = y * gridn + x;
			if (imget[i])
				code.cell_bitmap[i >> 3] &= ~(1 << (i & 7));
			else
				code.cell_bitmap[i >> 3] |= (1 << (i & 7));
		}
	}

	err = quirc_decode(&code, &data);
	if (err)
		return 0;

	if (data.payload_len >= YY_MAX_SYM_LEN - 1)
		return 0;

	memcpy(yy_symbol.data, data.payload, data.payload_len);
	yy_symbol.type = YY_QR;
	yy_symbol.len = data.payload_len;
	yy_symbol.data[yy_symbol.len] = 0;

	return 1;
}

void yy_qr_stream_finder(YyQRFinderLineCollection *qr_finder_lines, int *widstream, int *posstream, int streamlen, int line, int yline, int dir)
{
	int res, wid_sum5 = 0;
	for (int i = 0; i < 5; i++)
		wid_sum5 += widstream[i];
	for (int i = 5; i < streamlen; i++)
	{
		wid_sum5 += widstream[i] - widstream[i - 5];

		int pair, errcnt = 0;

		pair = widstream[i] + widstream[i - 1];
		res = yy_decode_e(pair, wid_sum5, 7);
		errcnt += abs(res - 2);

		if (errcnt >= 2)
			continue;

		pair = widstream[i - 1] + widstream[i - 2];
		res = yy_decode_e(pair, wid_sum5, 7);
		errcnt += abs(res - 4);

		if (errcnt >= 2)
			continue;

		pair = widstream[i - 2] + widstream[i - 3];
		res = yy_decode_e(pair, wid_sum5, 7);
		errcnt += abs(res - 4);

		if (errcnt >= 2)
			continue;

		pair = widstream[i - 3] + widstream[i - 4];
		res = yy_decode_e(pair, wid_sum5, 7);
		errcnt += abs(res - 2);

		if (errcnt >= 2)
			continue;

		qr_finder_lines->qr_finder_line[qr_finder_lines->count].wid_sum5 = wid_sum5;
		qr_finder_lines->qr_finder_line[qr_finder_lines->count].wid_cen = widstream[i - 2];

		if (dir == 0)
		{
			qr_finder_lines->qr_finder_line[qr_finder_lines->count].pos[0] = line * 32 + (posstream[i - 2] - widstream[i - 2] / 2);
			qr_finder_lines->qr_finder_line[qr_finder_lines->count].pos[1] = yline * 32;
		}
		else if (dir == 1)
		{
			qr_finder_lines->qr_finder_line[qr_finder_lines->count].pos[0] = line * 32;
			qr_finder_lines->qr_finder_line[qr_finder_lines->count].pos[1] = yline * 32 + (posstream[i - 2] - widstream[i - 2] / 2);
		}
		else if (dir == 2)
		{
			qr_finder_lines->qr_finder_line[qr_finder_lines->count].pos[0] = line * 32 - (posstream[i - 2] - widstream[i - 2] / 2);
			qr_finder_lines->qr_finder_line[qr_finder_lines->count].pos[1] = yline * 32 + (posstream[i - 2] - widstream[i - 2] / 2);
		}
		else if (dir == 3)
		{
			qr_finder_lines->qr_finder_line[qr_finder_lines->count].pos[0] = line * 32 + (posstream[i - 2] - widstream[i - 2] / 2);
			qr_finder_lines->qr_finder_line[qr_finder_lines->count].pos[1] = yline * 32 + (posstream[i - 2] - widstream[i - 2] / 2);
		}
		qr_finder_lines->count++;

		if (qr_finder_lines->count >= qr_finder_lines->maxlen)
			break;
	}
}

void yy_qr_finder_cluster(YyQRFinderLineCollection *qr_finder_lines, YyQRFinderCluster *neighbor, YyQRFinderClusterCollection *cluster)
{
	int *mask = (int *)calloc(qr_finder_lines->count, sizeof(int));

	for (int i = 0; i < qr_finder_lines->count; i++)
	{
		if (mask[i])
			continue;
		int len = qr_finder_lines->qr_finder_line[i].wid_cen;
		neighbor->count = 0;
		if (neighbor->maxlen == 0)
			break;
		neighbor->qr_finder_line[neighbor->count++] = qr_finder_lines->qr_finder_line + i;
		if (neighbor->count >= neighbor->maxlen)
			break;
		for (int j = i + 1; j < qr_finder_lines->count; j++)
		{
			if (mask[j])
				continue;
			YyQRFinderLine *a = neighbor->qr_finder_line[neighbor->count - 1];
			YyQRFinderLine *b = qr_finder_lines->qr_finder_line + j;

			/*The clustering threshold is proportional to the size of the lines,
			since minor noise in large areas can interrupt patterns more easily
			at high resolutions.*/
			int thresh = (a->wid_cen + b->wid_cen) / 6;

			if (abs(a->wid_cen - b->wid_cen) > thresh)
				continue;
			if (abs(a->pos[0] - b->pos[0]) > thresh)
				continue;
			if (abs(a->pos[1] - b->pos[1]) > thresh)
				continue;

			len += b->wid_cen;
			neighbor->qr_finder_line[neighbor->count++] = qr_finder_lines->qr_finder_line + j;
			if (neighbor->count >= neighbor->maxlen)
				break;
		}

		if (neighbor->count < 3)
			continue;

		if (neighbor->count * neighbor->count * 32 * 3 < len)
			continue;

		for (int j = 0; j < neighbor->count; j++)
			mask[neighbor->qr_finder_line[j] - qr_finder_lines->qr_finder_line] = 1;

		cluster->qr_finder_cluster[cluster->count].qr_finder_line = neighbor->qr_finder_line;
		cluster->qr_finder_cluster[cluster->count].count = neighbor->count;
		cluster->count++;
		if (cluster->count >= cluster->maxlen)
			break;

		neighbor->qr_finder_line += neighbor->count;
		neighbor->maxlen -= neighbor->count;
	}

	free(mask);
}

int yy_qr_finder_lines_are_crossing_hv(YyQRFinderLine *hline, YyQRFinderLine *vline)
{
	return hline->pos[0] <= vline->pos[0] && vline->pos[0] <= hline->pos[0] + hline->wid_cen
		&& vline->pos[1] <= hline->pos[1] && hline->pos[1] <= vline->pos[1] + vline->wid_cen;
}

int yy_qr_finder_lines_are_crossing_45(YyQRFinderLine *hline, YyQRFinderLine *vline)
{
	return vline->pos[0] + vline->pos[1] <= hline->pos[0] + hline->pos[1] && hline->pos[0] + hline->pos[1] <= vline->pos[0] + vline->pos[1] + 2 * vline->wid_cen
		&& hline->pos[1] - hline->pos[0] <= vline->pos[1] - vline->pos[0] && vline->pos[1] - vline->pos[0] <= hline->pos[1] - hline->pos[0] + 2 * hline->wid_cen;
}

float yy_qr_finder_ratio(int dx, int dy, int type)
{
	float theta;
	if (dx < 0)
		dx = -dx;
	if (dy < 0)
		dy = -dy;
	if (dy < dx)
		theta = atan2f(dy, dx);
	else
		theta = atan2f(dx, dy);

	if (type)
		return cosf(3.14159265f / 4 - theta);
	else
		return cosf(theta);
}

void yy_qr_finder_edge(YyQRFinderCenter *qr_center, YyImgCurfit *fit)
{
	int dx1 = qr_center[1].pos[0] - qr_center[0].pos[0];
	int dy1 = qr_center[1].pos[1] - qr_center[0].pos[1];

	int dx2 = qr_center[2].pos[0] - qr_center[0].pos[0];
	int dy2 = qr_center[2].pos[1] - qr_center[0].pos[1];

	int sum50 = qr_center[0].wid_sum5;
	int sum51 = qr_center[1].wid_sum5;
	int sum52 = qr_center[2].wid_sum5;
	int sum50x = qr_center[0].wid_sum5x;
	int sum51x = qr_center[1].wid_sum5x;
	int sum52x = qr_center[2].wid_sum5x;
	int sum50y = qr_center[0].wid_sum5y;
	int sum51y = qr_center[1].wid_sum5y;
	int sum52y = qr_center[2].wid_sum5y;

	float r1 = yy_qr_finder_ratio(dx1, dy1, qr_center[0].type);
	float r2 = yy_qr_finder_ratio(dx2, dy2, qr_center[0].type);

	float dis1 = r1 / sqrtf(dx1 * dx1 + dy1 * dy1) / 2;
	float dis2 = r2 / sqrtf(dx2 * dx2 + dy2 * dy2) / 2;

	float x1s0 = dx1 * dis1 * sum50x;
	float y1s0 = dy1 * dis1 * sum50y;
	float x2s0 = dx2 * dis2 * sum50x;
	float y2s0 = dy2 * dis2 * sum50y;
	float x1s1 = dx1 * dis1 * sum51x;
	float y1s1 = dy1 * dis1 * sum51y;
	float x2s1 = dx2 * dis2 * sum51x;
	float y2s1 = dy2 * dis2 * sum51y;
	float x1s2 = dx1 * dis1 * sum52x;
	float y1s2 = dy1 * dis1 * sum52y;
	float x2s2 = dx2 * dis2 * sum52x;
	float y2s2 = dy2 * dis2 * sum52y;

	fit[0].px[0] = (qr_center[0].pos[0] - x2s0) / 32;
	fit[0].py[0] = (qr_center[0].pos[1] - y2s0) / 32;
	fit[0].px[1] = (qr_center[1].pos[0] - x2s1) / 32;
	fit[0].py[1] = (qr_center[1].pos[1] - y2s1) / 32;
	fit[0].offs = (sum50 + sum51) / 64 / 14 + 1;
	fit[0].devi = 0.5f;
	yy_img_curfit_line(&fit[0]);
	yy_img_curfit_line_type(&fit[0], dx2, dy2);
	fit[0].px[0] -= (dx1 * dis1 * sum50) / 32;
	fit[0].py[0] -= (dy1 * dis1 * sum50) / 32;
	fit[0].px[1] += (dx1 * dis1 * sum51) / 32;
	fit[0].py[1] += (dy1 * dis1 * sum51) / 32;

	fit[1].px[0] = (qr_center[0].pos[0] - x1s0) / 32;
	fit[1].py[0] = (qr_center[0].pos[1] - y1s0) / 32;
	fit[1].px[1] = (qr_center[2].pos[0] - x1s2) / 32;
	fit[1].py[1] = (qr_center[2].pos[1] - y1s2) / 32;
	fit[1].offs = (sum50 + sum52) / 64 / 14 + 1;
	fit[1].devi = 0.5f;
	yy_img_curfit_line(&fit[1]);
	yy_img_curfit_line_type(&fit[1], dx1, dy1);
	fit[1].px[0] -= (dx2 * dis2 * sum50) / 32;
	fit[1].py[0] -= (dy2 * dis2 * sum50) / 32;
	fit[1].px[1] += (dx2 * dis2 * sum51) / 32;
	fit[1].py[1] += (dy2 * dis2 * sum51) / 32;

	fit[2].px[0] = (qr_center[2].pos[0] - x1s2 + x2s2) / 32;
	fit[2].py[0] = (qr_center[2].pos[1] - y1s2 + y2s2) / 32;
	fit[2].px[1] = (qr_center[2].pos[0] + x1s2 + x2s2) / 32;
	fit[2].py[1] = (qr_center[2].pos[1] + y1s2 + y2s2) / 32;
	fit[2].offs = sum52 / 32 / 10 + 2;
	fit[2].devi = 0.3f;
	yy_img_curfit_line(&fit[2]);
	yy_img_curfit_line_type(&fit[2], -dx2, -dy2);

	fit[3].px[0] = (qr_center[1].pos[0] + x1s1 - x2s1) / 32;
	fit[3].py[0] = (qr_center[1].pos[1] + y1s1 - y2s1) / 32;
	fit[3].px[1] = (qr_center[1].pos[0] + x1s1 + x2s1) / 32;
	fit[3].py[1] = (qr_center[1].pos[1] + y1s1 + y2s1) / 32;
	fit[3].offs = sum51 / 32 / 10 + 2;
	fit[3].devi = 0.3f;
	yy_img_curfit_line(&fit[3]);
	yy_img_curfit_line_type(&fit[3], -dx1, -dy1);

	float isect[2];
	yy_img_curfit_line_isect(&fit[2], &fit[3], isect);
	fit[2].px[2] = isect[0];
	fit[2].py[2] = isect[1];
	fit[3].px[2] = isect[0];
	fit[3].py[2] = isect[1];
}

float yy_qr_nnum_calc_xy(int dx, int dy, int sum5)
{
	float dis = sqrtf(dx * dx + dy * dy);
	float theta;
	if (dx < 0)
		dx = -dx;
	if (dy < 0)
		dy = -dy;
	if (dy < dx)
		theta = atan2f(dy, dx);
	else
		theta = atan2f(dx, dy);
	float sum5f = sum5 * cosf(theta);
	float nnum = (dis / sum5f + 1) * 7;
	return nnum;
}

int yy_qr_nnum_calc(YyQRFinderCenter *qr_center)
{
	int dx = qr_center[1].pos[0] - qr_center[0].pos[0];
	int dy = qr_center[1].pos[1] - qr_center[0].pos[1];
	int sum5 = (qr_center[1].wid_sum5 + qr_center[0].wid_sum5) / 2;

	int dx2 = qr_center[2].pos[0] - qr_center[0].pos[0];
	int dy2 = qr_center[2].pos[1] - qr_center[0].pos[1];
	int sum52 = (qr_center[2].wid_sum5 + qr_center[0].wid_sum5) / 2;

	float nnum = yy_qr_nnum_calc_xy(dx, dy, sum5);
	float nnum2 = yy_qr_nnum_calc_xy(dx2, dy2, sum52);

	return ((int)((nnum + nnum2 + 2) / 8)) * 4 + 1;
}

const unsigned char yy_qr_finder_set_data[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 1, 1, 1, 1, 1, 0, 1,
	1, 0, 1, 0, 0, 0, 1, 0, 1,
	1, 0, 1, 0, 0, 0, 1, 0, 1,
	1, 0, 1, 0, 0, 0, 1, 0, 1,
	1, 0, 1, 1, 1, 1, 1, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
};
void yy_qr_finder_set(unsigned char *imget, unsigned int gridn, unsigned int st)
{
	for (int gy = 0; gy < 9; gy++)
	{
		for (int gx = 0; gx < 9; gx++)
		{
			imget[gy * gridn + gx + st] = yy_qr_finder_set_data[gy * 9 + gx] * 255;
		}
	}
}

unsigned char yy_img_scanner_qr_hv(YyImgInterface *imgif, YyImgRegion *region)
{
	unsigned char *img = imgif->img;
	unsigned char *imdisp = imgif->imdisp;
	int width = imgif->w;
	int height = imgif->h;

	if (width < 23 || height < 23)
		return 0;

	YyQRFinderLineCollection qr_finder_lines[2];
	qr_finder_lines[0].count = 0;
	qr_finder_lines[1].count = 0;
	qr_finder_lines[0].maxlen = 1000;
	qr_finder_lines[1].maxlen = 1000;
	qr_finder_lines[0].qr_finder_line = (YyQRFinderLine *)malloc(qr_finder_lines[0].maxlen * sizeof(YyQRFinderLine));
	qr_finder_lines[1].qr_finder_line = (YyQRFinderLine *)malloc(qr_finder_lines[1].maxlen * sizeof(YyQRFinderLine));

	yy_img_scanner_conf.winsize = 25;
	yy_img_scanner_conf.winsize_half = 13;
	yy_img_scanner_conf.len = width;
	yy_img_scanner_conf.maxstreamlen = 250;
	yy_img_scanner_conf.bin = (unsigned char *)malloc(width);
	yy_img_scanner_conf.dimg = (char *)malloc(width);
	yy_img_scanner_conf.widstream = (int *)malloc(yy_img_scanner_conf.maxstreamlen * sizeof(int));
	yy_img_scanner_conf.posstream = (int *)malloc(yy_img_scanner_conf.maxstreamlen * sizeof(int));

	YyQRFinderClusterCollection cluster[2];
	cluster[0].count = 0;
	cluster[0].maxlen = 100;
	cluster[0].qr_finder_cluster = (YyQRFinderCluster *)malloc(cluster[0].maxlen * sizeof(YyQRFinderCluster));
	cluster[1].count = 0;
	cluster[1].maxlen = 100;
	cluster[1].qr_finder_cluster = (YyQRFinderCluster *)malloc(cluster[1].maxlen * sizeof(YyQRFinderCluster));

	YyQRFinderCluster neighbor;
	neighbor.count = 0;
	neighbor.maxlen = 1000;
	YyQRFinderLine **neighbor_buf = (YyQRFinderLine **)malloc(neighbor.maxlen * sizeof(YyQRFinderLine *));
	neighbor.qr_finder_line = neighbor_buf;

	//横向扫描11311定位标志
	unsigned char *horizon;
	for (int line = region->y + 5; line < region->y + region->h - 5; line++)
	{
		horizon = img + width * line;
		yy_img_scanner_conf.img = horizon;
		yy_img_scanner_fast();

#if YY_IMG_SCANNER_QR_DISP
		for (int x = 0; x < width; x++)
			imdisp[line * width + x] = yy_img_scanner_conf.bin[x] * 160;
#endif

		if (yy_img_scanner_conf.streamlen < 6)
		{
			line++;
			continue;
		}

		yy_qr_stream_finder(&qr_finder_lines[0], yy_img_scanner_conf.widstream, yy_img_scanner_conf.posstream, yy_img_scanner_conf.streamlen, 0, line, 0);

		if (qr_finder_lines[0].count >= qr_finder_lines[0].maxlen)
			break;
	}

	yy_qr_finder_cluster(&qr_finder_lines[0], &neighbor, &cluster[0]);

#if YY_IMG_SCANNER_QR_DISP
	for (int i = 0; i < cluster[0].count; i++)
	{
		for (int y = 0; y < cluster[0].qr_finder_cluster[i].count; y++)
		{
			YyQRFinderLine *a = cluster[0].qr_finder_cluster[i].qr_finder_line[y];
			imdisp[a->pos[1] / 32 * width + a->pos[0] / 32] = 255;
		}
	}
#endif

#if YY_IMG_SCANNER_QR_TIME
	time_stamp();
#endif

	//标记不足3个
	if (cluster[0].count < 3)
	{
		free(yy_img_scanner_conf.widstream);
		free(yy_img_scanner_conf.posstream);
		free(yy_img_scanner_conf.bin);
		free(yy_img_scanner_conf.dimg);

		free(neighbor_buf);
		free(cluster[0].qr_finder_cluster);
		free(cluster[1].qr_finder_cluster);

		free(qr_finder_lines[0].qr_finder_line);
		free(qr_finder_lines[1].qr_finder_line);
		return 0;
	}

	//纵向疑似区域预测
	YyImgRegion *hor_sus = (YyImgRegion *)malloc(cluster[0].count * sizeof(YyImgRegion));
	for (int i = 0; i < cluster[0].count; i++)
	{
		int x = 0, y = 0, w = 0;
		for (int j = 0; j < cluster[0].qr_finder_cluster[i].count; j++)
		{
			x += cluster[0].qr_finder_cluster[i].qr_finder_line[j]->pos[0] / 32;
			y += cluster[0].qr_finder_cluster[i].qr_finder_line[j]->pos[1] / 32;
			w += cluster[0].qr_finder_cluster[i].qr_finder_line[j]->wid_cen / 32;
		}
		hor_sus[i].x = x / cluster[0].qr_finder_cluster[i].count;
		hor_sus[i].y = (y - w * 3 / 2) / cluster[0].qr_finder_cluster[i].count - yy_img_scanner_conf.margin - 1;
		hor_sus[i].w = w / cluster[0].qr_finder_cluster[i].count + 2;
		hor_sus[i].h = w * 3 / cluster[0].qr_finder_cluster[i].count + yy_img_scanner_conf.margin * 2 + 3;

		if (hor_sus[i].x >= width || hor_sus[i].y >= height)
		{
			hor_sus[i].w = hor_sus[i].h = 0;
			continue;
		}

		if (hor_sus[i].x < 0)
			hor_sus[i].x = 0;
		if (hor_sus[i].y < 0)
			hor_sus[i].y = 0;
		if (hor_sus[i].x + hor_sus[i].w > width)
			hor_sus[i].w = width - hor_sus[i].x;
		if (hor_sus[i].y + hor_sus[i].h > height)
			hor_sus[i].h = height - hor_sus[i].y;

#if YY_IMG_SCANNER_QR_DISP
		for (int x = hor_sus[i].x; x <= hor_sus[i].x + hor_sus[i].w; x++)
		{
			for (int y = hor_sus[i].y; y < hor_sus[i].y + hor_sus[i].h; y++)
				imdisp[width * y + x] = 255;
		}
#endif
	}

	unsigned char *vertical;
	vertical = (unsigned char *)malloc(height);
	for (int i = 0; i < cluster[0].count; i++)
	{
		for (int line = hor_sus[i].x; line < hor_sus[i].x + hor_sus[i].w; line++)
		{
			for (int y = hor_sus[i].y; y < hor_sus[i].y + hor_sus[i].h; y++)
				vertical[y] = img[y * width + line];

			yy_img_scanner_conf.len = hor_sus[i].h;
			yy_img_scanner_conf.img = vertical + hor_sus[i].y;
			yy_img_scanner();

#if YY_IMG_SCANNER_QR_DISP
			for (int y = hor_sus[i].y; y < hor_sus[i].y + hor_sus[i].h; y++)
				imdisp[y * width + line] = yy_img_scanner_conf.bin[y - hor_sus[i].y] * 160;
#endif

			/*for(int f = 0; f < yy_img_scanner_conf.streamlen; f++)
			printf("%d ", yy_img_scanner_conf.widstream[f]);
			printf("\n");*/

			if (yy_img_scanner_conf.streamlen < 6)
				continue;

			yy_qr_stream_finder(&qr_finder_lines[1], yy_img_scanner_conf.widstream, yy_img_scanner_conf.posstream, yy_img_scanner_conf.streamlen, line, hor_sus[i].y, 1);

			if (qr_finder_lines[1].count >= qr_finder_lines[1].maxlen)
				break;
		}
		if (qr_finder_lines[1].count >= qr_finder_lines[1].maxlen)
			break;
	}

	yy_qr_finder_cluster(&qr_finder_lines[1], &neighbor, &cluster[1]);

#if YY_IMG_SCANNER_QR_DISP
	for (int i = 0; i < cluster[1].count; i++)
	{
		for (int y = 0; y < cluster[1].qr_finder_cluster[i].count; y++)
		{
			YyQRFinderLine *a = cluster[1].qr_finder_cluster[i].qr_finder_line[y];
			imdisp[a->pos[1] / 32 * width + a->pos[0] / 32] = 255;
		}
	}
#endif

	free(hor_sus);
	free(vertical);
	free(yy_img_scanner_conf.widstream);
	free(yy_img_scanner_conf.posstream);
	free(yy_img_scanner_conf.bin);
	free(yy_img_scanner_conf.dimg);

#if YY_IMG_SCANNER_QR_TIME
	time_stamp();
#endif

	if (cluster[1].count < 3)
	{
		free(neighbor_buf);
		free(cluster[0].qr_finder_cluster);
		free(cluster[1].qr_finder_cluster);

		free(qr_finder_lines[0].qr_finder_line);
		free(qr_finder_lines[1].qr_finder_line);
		return 0;
	}

	//定位标志中心点检测,聚类后取出权值最大的三个
	YyQRFinderCenter center[4];
	center[0].cross_cnt = 0;
	center[1].cross_cnt = 0;
	center[2].cross_cnt = 0;
	for (int i = 0; i < cluster[0].count; i++)
	{
		if (cluster[0].qr_finder_cluster[i].count < 3)
			continue;

		int staa = 0, enda = cluster[0].qr_finder_cluster[i].count - 1;
		if (cluster[0].qr_finder_cluster[i].count >= 5)
		{
			staa = 1;
			enda = cluster[0].qr_finder_cluster[i].count - 2;
		}

		YyQRFinderLine *a = cluster[0].qr_finder_cluster[i].qr_finder_line[cluster[0].qr_finder_cluster[i].count / 2];
		YyQRFinderLine *a1 = cluster[0].qr_finder_cluster[i].qr_finder_line[staa];
		YyQRFinderLine *a2 = cluster[0].qr_finder_cluster[i].qr_finder_line[enda];
		int ax = a2->pos[0] - a1->pos[0];
		int ay = a2->pos[1] - a1->pos[1];
		int lena = sqrtf(ax * ax + ay * ay);

		for (int j = 0; j < cluster[1].count; j++)
		{
			if (cluster[1].qr_finder_cluster[j].count < 3)
				continue;

			YyQRFinderLine *b = cluster[1].qr_finder_cluster[j].qr_finder_line[cluster[1].qr_finder_cluster[j].count / 2];

			if (yy_qr_finder_lines_are_crossing_hv(a, b))
			{
				int stab = 0, endb = cluster[1].qr_finder_cluster[j].count - 1;
				if (cluster[1].qr_finder_cluster[j].count >= 5)
				{
					stab = 1;
					endb = cluster[1].qr_finder_cluster[j].count - 2;
				}

				YyQRFinderLine *b1 = cluster[1].qr_finder_cluster[j].qr_finder_line[stab];
				YyQRFinderLine *b2 = cluster[1].qr_finder_cluster[j].qr_finder_line[endb];
				int bx = b2->pos[0] - b1->pos[0];
				int by = b2->pos[1] - b1->pos[1];
				int lenb = sqrtf(bx * bx + by * by);

				int mul = ax * bx + ay * by;
				if (mul < 0)
					mul = -mul;

				float angle, ratio;
				angle = (float)mul / lena / lenb;
				ratio = (float)lena / lenb;

				if (angle > 0.8f)
					continue;

				if (ratio < 0.33f || ratio > 3.0f)
					continue;

				int cross_cnt = 0;
				int midx = 0, midy = 0;
				int cen = 0, sum5x = 0, sum5y = 0;
				for (int ii = 0; ii < cluster[0].qr_finder_cluster[i].count; ii++)
				{
					for (int jj = 0; jj < cluster[1].qr_finder_cluster[j].count; jj++)
					{
						a = cluster[0].qr_finder_cluster[i].qr_finder_line[ii];
						b = cluster[1].qr_finder_cluster[j].qr_finder_line[jj];
						if (yy_qr_finder_lines_are_crossing_hv(a, b))
						{
							cross_cnt++;
							midx += a->pos[0] + b->pos[0] + a->wid_cen / 2;
							midy += a->pos[1] + b->pos[1] + b->wid_cen / 2;
							cen += a->wid_cen + b->wid_cen;
							sum5x += a->wid_sum5;
							sum5y += b->wid_sum5;
#if YY_IMG_SCANNER_QR_DISP
							imdisp[width * (a->pos[1] / 32) + a->pos[0] / 32] = 255;
							imdisp[width * (b->pos[1] / 32) + b->pos[0] / 32] = 255;
#endif

						}
					}
				}
				int minp = 0, mincnt = center[0].cross_cnt;
				for (int p = 1; p < 3; p++)
				{
					if (mincnt > center[p].cross_cnt)
					{
						mincnt = center[p].cross_cnt;
						minp = p;
					}
				}
				if (center[minp].cross_cnt < cross_cnt)
				{
					center[minp].pos[0] = midx / cross_cnt / 2;
					center[minp].pos[1] = midy / cross_cnt / 2;
					center[minp].wid_cen = cen / cross_cnt / 2;
					center[minp].wid_sum5x = sum5x / cross_cnt;
					center[minp].wid_sum5y = sum5y / cross_cnt;
					center[minp].wid_sum5 = (sum5x + sum5y) / cross_cnt / 2;
					center[minp].cross_cnt = cross_cnt;
					center[minp].type = 0;
				}
				break;
			}
		}
	}

	free(neighbor_buf);
	free(cluster[0].qr_finder_cluster);
	free(cluster[1].qr_finder_cluster);

	free(qr_finder_lines[0].qr_finder_line);
	free(qr_finder_lines[1].qr_finder_line);

	if (center[2].cross_cnt == 0)
		return 0;

	return yy_qr_correct(imgif, region, center);
}

unsigned char yy_img_scanner_qr_45(YyImgInterface *imgif, YyImgRegion *region)
{
	unsigned char *img = imgif->img;
	unsigned char *imdisp = img;
	int width = imgif->w;
	int height = imgif->h;

	if (width < 23 || height < 23)
		return 0;

	YyQRFinderLineCollection qr_finder_lines[2];
	qr_finder_lines[0].count = 0;
	qr_finder_lines[1].count = 0;
	qr_finder_lines[0].maxlen = 1000;
	qr_finder_lines[1].maxlen = 1000;
	qr_finder_lines[0].qr_finder_line = (YyQRFinderLine *)malloc(qr_finder_lines[0].maxlen * sizeof(YyQRFinderLine));
	qr_finder_lines[1].qr_finder_line = (YyQRFinderLine *)malloc(qr_finder_lines[1].maxlen * sizeof(YyQRFinderLine));

	yy_img_scanner_conf.winsize = 25;
	yy_img_scanner_conf.winsize_half = 13;
	yy_img_scanner_conf.len = width;
	yy_img_scanner_conf.maxstreamlen = 250;
	yy_img_scanner_conf.bin = (unsigned char *)malloc(width);
	yy_img_scanner_conf.dimg = (char *)malloc(width);
	yy_img_scanner_conf.widstream = (int *)malloc(yy_img_scanner_conf.maxstreamlen * sizeof(int));
	yy_img_scanner_conf.posstream = (int *)malloc(yy_img_scanner_conf.maxstreamlen * sizeof(int));

	YyQRFinderClusterCollection cluster[2];
	cluster[0].count = 0;
	cluster[0].maxlen = 100;
	cluster[0].qr_finder_cluster = (YyQRFinderCluster *)malloc(cluster[0].maxlen * sizeof(YyQRFinderCluster));
	cluster[1].count = 0;
	cluster[1].maxlen = 100;
	cluster[1].qr_finder_cluster = (YyQRFinderCluster *)malloc(cluster[1].maxlen * sizeof(YyQRFinderCluster));

	YyQRFinderCluster neighbor;
	neighbor.count = 0;
	neighbor.maxlen = 1000;
	YyQRFinderLine **neighbor_buf = (YyQRFinderLine **)malloc(neighbor.maxlen * sizeof(YyQRFinderLine *));
	neighbor.qr_finder_line = neighbor_buf;

	//右上-左下45度扫描11311定位标志
	unsigned char *vertical;
	vertical = (unsigned char *)malloc(height);
	for (int line = region->x; line < region->x + region->w; line++)
	{
		int startx = line + region->h / 2;
		int starty = region->y;
		int lenxy = startx - region->x + 1;

		if (lenxy > region->h)
			lenxy = region->h;

		if (startx >= region->x + region->w)
		{
			starty = region->y + startx - region->x - region->w + 1;
			startx = region->x + region->w - 1;
			lenxy -= starty - region->y;
		}

		for (int y = 0; y < lenxy; y++)
		{
			//if (starty + y < region->y || startx + y < region->x
			//	|| starty + y >= region->y + region->h || startx - y >= region->x + region->w)
			//	printf("Error");
			vertical[y] = img[(starty + y) * width + startx - y];
		}

		yy_img_scanner_conf.len = lenxy;
		yy_img_scanner_conf.img = vertical;
		yy_img_scanner_fast();

#if YY_IMG_SCANNER_QR_DISP
		for (int y = 0; y < lenxy; y++)
			imdisp[(starty + y) * width + startx - y] = yy_img_scanner_conf.bin[y] * 255;
#endif

		if (yy_img_scanner_conf.streamlen < 6)
		{
			line++;
			continue;
		}

		yy_qr_stream_finder(&qr_finder_lines[0], yy_img_scanner_conf.widstream, yy_img_scanner_conf.posstream, yy_img_scanner_conf.streamlen, startx, starty, 2);

		if (qr_finder_lines[0].count >= qr_finder_lines[0].maxlen)
			break;
	}

	yy_qr_finder_cluster(&qr_finder_lines[0], &neighbor, &cluster[0]);

#if YY_IMG_SCANNER_QR_DISP
	for (int i = 0; i < cluster[0].count; i++)
	{
		for (int y = 0; y < cluster[0].qr_finder_cluster[i].count; y++)
		{
			YyQRFinderLine *a = cluster[0].qr_finder_cluster[i].qr_finder_line[y];
			imdisp[a->pos[1] / 32 * width + a->pos[0] / 32] = 0;
		}
	}
#endif

#if YY_IMG_SCANNER_QR_TIME
	time_stamp();
#endif

	//标记不足3个
	if (cluster[0].count < 3)
	{
		free(vertical);

		free(yy_img_scanner_conf.widstream);
		free(yy_img_scanner_conf.posstream);
		free(yy_img_scanner_conf.bin);
		free(yy_img_scanner_conf.dimg);

		free(neighbor_buf);
		free(cluster[0].qr_finder_cluster);
		free(cluster[1].qr_finder_cluster);

		free(qr_finder_lines[0].qr_finder_line);
		free(qr_finder_lines[1].qr_finder_line);
		return 0;
	}

	//45度疑似区域预测
	YyImgRegion *hor_sus = (YyImgRegion *)malloc(cluster[0].count * sizeof(YyImgRegion));
	for (int i = 0; i < cluster[0].count; i++)
	{
		int x = 0, y = 0, w = 0;
		for (int j = 0; j < cluster[0].qr_finder_cluster[i].count; j++)
		{
			x += cluster[0].qr_finder_cluster[i].qr_finder_line[j]->pos[0] / 32;
			y += cluster[0].qr_finder_cluster[i].qr_finder_line[j]->pos[1] / 32;
			w += cluster[0].qr_finder_cluster[i].qr_finder_line[j]->wid_cen / 32;
		}
		hor_sus[i].x = (x - w * 2) / cluster[0].qr_finder_cluster[i].count - yy_img_scanner_conf.margin;
		hor_sus[i].y = (y - w * 1) / cluster[0].qr_finder_cluster[i].count - yy_img_scanner_conf.margin;
		hor_sus[i].w = w * 4 / cluster[0].qr_finder_cluster[i].count + yy_img_scanner_conf.margin * 2;
		hor_sus[i].h = hor_sus[i].w;

		if (hor_sus[i].x >= width || hor_sus[i].y >= height)
		{
			hor_sus[i].w = hor_sus[i].h = 0;
			continue;
		}

		if (hor_sus[i].x < 0)
			hor_sus[i].x = 0;
		if (hor_sus[i].y < 0)
			hor_sus[i].y = 0;
		if (hor_sus[i].x + hor_sus[i].w > width)
			hor_sus[i].w = width - hor_sus[i].x;
		if (hor_sus[i].y + hor_sus[i].h > height)
			hor_sus[i].h = height - hor_sus[i].y;

#if YY_IMG_SCANNER_QR_DISP
		for (int x = hor_sus[i].x; x <= hor_sus[i].x + hor_sus[i].w; x++)
		{
			for (int y = hor_sus[i].y; y < hor_sus[i].y + hor_sus[i].h; y++)
				imdisp[width * y + x] = 255;
		}
#endif
	}

	for (int i = 0; i < cluster[0].count; i++)
	{
		for (int line = hor_sus[i].x; line < hor_sus[i].x + hor_sus[i].w; line++)
		{
			int startx = line + hor_sus[i].h / 2;
			int starty = hor_sus[i].y;
			int lenxy = startx - hor_sus[i].x + 1;

			if (lenxy > hor_sus[i].h)
				lenxy = hor_sus[i].h;

			if (startx >= hor_sus[i].x + hor_sus[i].w)
			{
				starty = hor_sus[i].y + startx - hor_sus[i].x - hor_sus[i].w + 1;
				startx = hor_sus[i].x + hor_sus[i].w - 1;
				lenxy -= starty - hor_sus[i].y;
			}

			starty = hor_sus[i].y + hor_sus[i].h - 1 - (starty - hor_sus[i].y);
			startx -= lenxy - 1;
			starty -= lenxy - 1;

			for (int y = 0; y < lenxy; y++)
			{
				//if (starty + y < hor_sus[i].y || startx + y < hor_sus[i].x
				//	|| starty + y >= hor_sus[i].y + hor_sus[i].h || startx + y >= hor_sus[i].x + hor_sus[i].w)
				//	printf("Error");
				vertical[y] = img[(starty + y) * width + startx + y];
			}

			yy_img_scanner_conf.len = lenxy;
			yy_img_scanner_conf.img = vertical;
			yy_img_scanner();

#if YY_IMG_SCANNER_QR_DISP
			for (int y = 0; y < lenxy; y++)
				imdisp[(starty + y) * width + startx + y] = yy_img_scanner_conf.bin[y] * 255;
#endif

			if (yy_img_scanner_conf.streamlen < 6)
				continue;

			yy_qr_stream_finder(&qr_finder_lines[1], yy_img_scanner_conf.widstream, yy_img_scanner_conf.posstream, yy_img_scanner_conf.streamlen, startx, starty, 3);

			if (qr_finder_lines[1].count >= qr_finder_lines[1].maxlen)
				break;
		}
		if (qr_finder_lines[1].count >= qr_finder_lines[1].maxlen)
			break;
	}

	yy_qr_finder_cluster(&qr_finder_lines[1], &neighbor, &cluster[1]);

#if YY_IMG_SCANNER_QR_DISP
	for (int i = 0; i < cluster[1].count; i++)
	{
		for (int y = 0; y < cluster[1].qr_finder_cluster[i].count; y++)
		{
			YyQRFinderLine *a = cluster[1].qr_finder_cluster[i].qr_finder_line[y];
			imdisp[a->pos[1] / 32 * width + a->pos[0] / 32] = 0;
		}
	}
#endif

	free(hor_sus);
	free(vertical);
	free(yy_img_scanner_conf.widstream);
	free(yy_img_scanner_conf.posstream);
	free(yy_img_scanner_conf.bin);
	free(yy_img_scanner_conf.dimg);

#if YY_IMG_SCANNER_QR_TIME
	time_stamp();
#endif

	if (cluster[1].count < 3)
	{
		free(neighbor_buf);
		free(cluster[0].qr_finder_cluster);
		free(cluster[1].qr_finder_cluster);

		free(qr_finder_lines[0].qr_finder_line);
		free(qr_finder_lines[1].qr_finder_line);
		return 0;
	}

	//定位标志中心点检测,聚类后取出权值最大的三个
	YyQRFinderCenter center[4];
	center[0].cross_cnt = 0;
	center[1].cross_cnt = 0;
	center[2].cross_cnt = 0;
	for (int i = 0; i < cluster[0].count; i++)
	{
		if (cluster[0].qr_finder_cluster[i].count < 3)
			continue;

		int staa = 0, enda = cluster[0].qr_finder_cluster[i].count - 1;
		if (cluster[0].qr_finder_cluster[i].count >= 5)
		{
			staa = 1;
			enda = cluster[0].qr_finder_cluster[i].count - 2;
		}

		YyQRFinderLine *a = cluster[0].qr_finder_cluster[i].qr_finder_line[cluster[0].qr_finder_cluster[i].count / 2];
		YyQRFinderLine *a1 = cluster[0].qr_finder_cluster[i].qr_finder_line[staa];
		YyQRFinderLine *a2 = cluster[0].qr_finder_cluster[i].qr_finder_line[enda];
		int ax = a2->pos[0] - a1->pos[0];
		int ay = a2->pos[1] - a1->pos[1];
		int lena = sqrtf(ax * ax + ay * ay);

		for (int j = 0; j < cluster[1].count; j++)
		{
			if (cluster[1].qr_finder_cluster[j].count < 3)
				continue;

			YyQRFinderLine *b = cluster[1].qr_finder_cluster[j].qr_finder_line[cluster[1].qr_finder_cluster[j].count / 2];

			if (yy_qr_finder_lines_are_crossing_45(a, b))
			{
				int stab = 0, endb = cluster[1].qr_finder_cluster[j].count - 1;
				if (cluster[1].qr_finder_cluster[j].count >= 5)
				{
					stab = 1;
					endb = cluster[1].qr_finder_cluster[j].count - 2;
				}

				YyQRFinderLine *b1 = cluster[1].qr_finder_cluster[j].qr_finder_line[stab];
				YyQRFinderLine *b2 = cluster[1].qr_finder_cluster[j].qr_finder_line[endb];
				int bx = b2->pos[0] - b1->pos[0];
				int by = b2->pos[1] - b1->pos[1];
				int lenb = sqrtf(bx * bx + by * by);

				int mul = ax * bx + ay * by;
				if (mul < 0)
					mul = -mul;

				float angle, ratio;
				angle = (float)mul / lena / lenb;
				ratio = (float)lena / lenb;

				if (angle > 0.8f)
					continue;

				if (ratio < 0.33f || ratio > 3.0f)
					continue;

				int cross_cnt = 0;
				int midx = 0, midy = 0;
				int cen = 0, sum5x = 0, sum5y = 0;
				for (int ii = 0; ii < cluster[0].qr_finder_cluster[i].count; ii++)
				{
					for (int jj = 0; jj < cluster[1].qr_finder_cluster[j].count; jj++)
					{
						a = cluster[0].qr_finder_cluster[i].qr_finder_line[ii];
						b = cluster[1].qr_finder_cluster[j].qr_finder_line[jj];
						if (yy_qr_finder_lines_are_crossing_45(a, b))
						{
							cross_cnt++;
							midx += a->pos[0] + b->pos[0] - a->wid_cen / 2 + b->wid_cen / 2;
							midy += a->pos[1] + b->pos[1] + a->wid_cen / 2 + b->wid_cen / 2;
							cen += a->wid_cen + b->wid_cen;
							sum5x += a->wid_sum5;
							sum5y += b->wid_sum5;
#if YY_IMG_SCANNER_QR_DISP
							imdisp[width * (a->pos[1] / 32) + a->pos[0] / 32] = 255;
							imdisp[width * (b->pos[1] / 32) + b->pos[0] / 32] = 255;
#endif

						}
					}
				}
				int minp = 0, mincnt = center[0].cross_cnt;
				for (int p = 1; p < 3; p++)
				{
					if (mincnt > center[p].cross_cnt)
					{
						mincnt = center[p].cross_cnt;
						minp = p;
					}
				}
				if (center[minp].cross_cnt < cross_cnt)
				{
					center[minp].pos[0] = midx / cross_cnt / 2;
					center[minp].pos[1] = midy / cross_cnt / 2;
					center[minp].wid_cen = cen / cross_cnt / 2;
					center[minp].wid_sum5x = sum5x / cross_cnt * 1.414f;
					center[minp].wid_sum5y = sum5y / cross_cnt * 1.414f;
					center[minp].wid_sum5 = (sum5x + sum5y) / cross_cnt / 2 * 1.414f;
					center[minp].cross_cnt = cross_cnt;
					center[minp].type = 1;
				}
				break;
			}
		}
	}

	free(neighbor_buf);
	free(cluster[0].qr_finder_cluster);
	free(cluster[1].qr_finder_cluster);

	free(qr_finder_lines[0].qr_finder_line);
	free(qr_finder_lines[1].qr_finder_line);

	if (center[2].cross_cnt == 0)
		return 0;

	return yy_qr_correct(imgif, region, center);
}

unsigned char yy_qr_affine_data[] = {
	1, 1, 1,
	1, 0, 1,
	1, 1, 1,
};

unsigned char yy_qr_finderinner0_data[] = {
	1, 0, 1,
	0, 0, 1,
	1, 1, 1,
};

unsigned char yy_qr_finderinner1_data[] = {
	1, 0, 1,
	1, 0, 0,
	1, 1, 1,
};

unsigned char yy_qr_finderinner2_data[] = {
	1, 1, 1,
	0, 0, 1,
	1, 0, 1,
};

unsigned char yy_qr_fitblock(YyImgInterface *imgif, YyImgRegion *region, int dsize,
	float imu, float imv, float *gxout, float *gyout)
{
	unsigned char *img = imgif->img;
	int width = imgif->w;
	int height = imgif->h;

	int sx = imu + 0.5f;
	int sy = imv + 0.5f;
	//img[sy * width + sx] = 255;

	int dsize1 = dsize / 2;
	int dsize2 = dsize - dsize1;

	if (sx < 2 || sy < 2
		|| sx > width - 3 || sy > height - 3)
		return 0;

	int avg = 0;
	for (int yy = -dsize; yy <= dsize; yy++)
	{
		for (int xx = -dsize; xx <= dsize; xx++)
		{
			avg += img[(sy + yy) * width + (sx + xx)];
		}
	}
	avg /= (2 * dsize + 1) * (2 * dsize + 1);

	int ax, ay, aw;

	for (int r = 0; r < 3; r++)
	{
		ax = 0, ay = 0, aw = 0;
		for (int yy = -dsize1; yy <= dsize2; yy++)
		{
			for (int xx = -dsize1; xx <= dsize2; xx++)
			{
				if (img[(sy + yy) * width + (sx + xx)] < avg)
				{
					int ww = avg - img[(sy + yy) * width + (sx + xx)];
					aw += ww;
					ax += ww * xx;
					ay += ww * yy;
					//if(r == 2)
					//	img[(sy + yy) * width + (sx + xx)] = 255;
				}
			}
		}

		if (aw == 0)
			return 0;

		imu = sx + ax / (float)aw;
		imv = sy + ay / (float)aw;
		if (sx == (int)(imu + 0.5f) && sy == (int)(imv + 0.5f))
			break;
		sx = imu + 0.5f;
		sy = imv + 0.5f;
		if (sx < 2 || sy < 2
			|| sx > width - 3 || sy > height - 3)
			return 0;
	}

	*gxout = imu;
	*gyout = imv;
	//img[(int)(imv + 0.5f) * width + (int)(imu + 0.5f)] = 255;

	return 1;
}

unsigned char yy_model33_shift(YyImgInterface *imgif, YyImgRegion *region, YyImgPerspect *hom, unsigned char *model,
	float gx, float gy, float dg, float *gxout, float *gyout)
{
	unsigned char *img = imgif->img;
	int width = imgif->w;
	int height = imgif->h;

	float gridx[9], gridy[9];
	int idx = 0;
	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			img_perspect_k2pix(hom, gx + x * dg, gy + y * dg, &gridx[idx], &gridy[idx]);
			idx++;
		}
	}

	int dxy0 = (fabsf(gridx[8] - gridx[0]) + fabsf(gridy[8] - gridy[0])) / 4;
	int dxy = dxy0;
	if (dxy < 1)
		dxy = 1;

	int sumx = 0, sumy = 0;

	for (; dxy > 0; dxy /= 2)
	{
		int emin = 100000, shiftx = 0, shifty = 0;
		for (int y = -dxy; y <= dxy; y += dxy)
		{
			for (int x = -dxy; x <= dxy; x += dxy)
			{
				int e = 0;
				for (int i = 0; i < 9; i++)
				{
					int imu1 = gridx[i] + sumx + x;
					int imv1 = gridy[i] + sumy + y;
					int imu2 = imu1 + 1;
					int imv2 = imv1 + 1;

					int p = img[imv1 * width + imu1]
						+ img[imv1 * width + imu2]
						+ img[imv2 * width + imu1]
						+ img[imv2 * width + imu2];
					e += model[i] ? -p : p;
				}
				if (emin > e)
				{
					emin = e;
					shiftx = x;
					shifty = y;
				}
			}
		}
		sumx += shiftx;
		sumy += shifty;
	}
	for (int i = 0; i < 9; i++)
	{
		gridx[i] += sumx;
		gridy[i] += sumy;

		//int imu1 = gridx[i] + 0.5f;
		//int imv1 = gridy[i] + 0.5f;
		//img[imv1 * width + imu1] = 255;
	}

	yy_qr_fitblock(imgif, region, dxy0, gridx[4], gridy[4], gxout, gyout);

	return 1;
}

int yy_qr_loc_check(YyImgInterface *imgif, YyImgRegion *region, YyImgPerspect *hom, int gridn, int stx, int sty)
{
	unsigned char *img = imgif->img;
	int width = imgif->w;
	int height = imgif->h;

	float gxf, gyf;
	unsigned char imloc[49];
	int avg = 0, errcnt = 0;
	for (int gy = 0; gy < 7; gy++)
	{
		for (int gx = 0; gx < 7; gx++)
		{
			gyf = (gy + sty + 0.5f - hom->sy) / hom->scale;
			gxf = (gx + stx + 0.5f - hom->sx) / hom->scale;

			float imu, imv;
			img_perspect_k2pix(hom, gxf, gyf, &imu, &imv);

			if ((!(imu > 0 && imu < width - 1)) || (!(imv > 0 && imv < height - 1)))
				return -1;

			//Bilinear interpolation
			int imu1 = imu;
			int imv1 = imv;
			int imu2 = imu1 + 1;
			int imv2 = imv1 + 1;

			float k1 = (imu2 - imu) * (imv2 - imv);
			float k2 = (imu - imu1) * (imv2 - imv);
			float k3 = (imu2 - imu) * (imv - imv1);
			float k4 = (imu - imu1) * (imv - imv1);

			unsigned char p = k1 * img[imv1 * width + imu1]
				+ k2 * img[imv1 * width + imu2]
				+ k3 * img[imv2 * width + imu1]
				+ k4 * img[imv2 * width + imu2];

			avg += p;
			imloc[gy * 7 + gx] = p;
		}
	}
	avg /= 49;
	for (int gy = 0; gy < 7; gy++)
	{
		for (int gx = 0; gx < 7; gx++)
		{
			if ((imloc[gy * 7 + gx] >= avg) != yy_qr_finder_set_data[(gy + 1) * 9 + gx + 1])
				errcnt++;
		}
	}
	return errcnt;
}

int yy_qr_get_gridn(YyQRFinderCenter *center, YyImgPerspect *hom)
{
	//投影变换的逆变换矩阵B(b(ij));实际上放大了det(A)倍,但不影响后续计算
	float b11, b12, b13, b21, b22, b23, b31, b32, b33;
	b11 = hom->a22 - hom->a23 * hom->a32;
	b12 = hom->a13 * hom->a32 - hom->a12;
	b13 = hom->a12 * hom->a23 - hom->a13 * hom->a22;
	b21 = hom->a23 * hom->a31 - hom->a21;
	b22 = hom->a11 - hom->a13 * hom->a31;
	b23 = hom->a13 * hom->a21 - hom->a11 * hom->a23;
	b31 = hom->a21 * hom->a32 - hom->a22 * hom->a31;
	b32 = hom->a12 * hom->a31 - hom->a11 * hom->a32;
	b33 = hom->a11 * hom->a22 - hom->a12 * hom->a21;

	//根据3个定位点的中心坐标,反投影到采样网格,从而确定二维码边长
	float cu0 = center[0].pos[0] / 32.0f;
	float cv0 = center[0].pos[1] / 32.0f;
	float cu1 = center[1].pos[0] / 32.0f;
	float cv1 = center[1].pos[1] / 32.0f;
	float cu2 = center[2].pos[0] / 32.0f;
	float cv2 = center[2].pos[1] / 32.0f;

	float cx0 = b11 * cu0 + b12 * cv0 + b13;
	float cy0 = b21 * cu0 + b22 * cv0 + b23;
	float cz0 = b31 * cu0 + b32 * cv0 + b33;
	float cx1 = b11 * cu1 + b12 * cv1 + b13;
	float cy1 = b21 * cu1 + b22 * cv1 + b23;
	float cz1 = b31 * cu1 + b32 * cv1 + b33;
	float cx2 = b11 * cu2 + b12 * cv2 + b13;
	float cy2 = b21 * cu2 + b22 * cv2 + b23;
	float cz2 = b31 * cu2 + b32 * cv2 + b33;

	cx0 /= cz0;
	cy0 /= cz0;
	cx1 /= cz1;
	cy1 /= cz1;
	cx2 /= cz2;
	cy2 /= cz2;

	//二维码边长计算公式,21,25,29...
	return (int)((1 + 21 / (2 + cx0 + cy0 - cx1 + cy1 + cx2 - cy2)) / 4) * 4 + 1;
}

unsigned char yy_qr_correct(YyImgInterface *imgif, YyImgRegion *region, YyQRFinderCenter *center)
{
	unsigned char *img = imgif->img;
	unsigned char *imdisp = imgif->imdisp;
	int width = imgif->w;
	int height = imgif->h;

	int mul_min = 2147483647, lena, lenb;
	int pcen = 0, qcen1 = 1, qcen2 = 2;
	for (int p = 0; p < 3; p++)
	{
		int q1 = (p + 1) % 3;
		int q2 = (p + 2) % 3;
		int ax = center[q1].pos[0] - center[p].pos[0];
		int ay = center[q1].pos[1] - center[p].pos[1];
		int bx = center[q2].pos[0] - center[p].pos[0];
		int by = center[q2].pos[1] - center[p].pos[1];
		int mul = ax * bx + ay * by;
		if (mul < 0)
			mul = -mul;
		if (mul < mul_min)
		{
			mul_min = mul;
			pcen = p;

			int cross = ax * by - ay * bx;

			if (cross > 0)
			{
				qcen1 = q1;
				qcen2 = q2;
			}
			else
			{
				qcen1 = q2;
				qcen2 = q1;
			}

			lena = sqrtf(ax * ax + ay * ay);
			lenb = sqrtf(bx * bx + by * by);
		}
	}

	if (lena < 21 * 32 || lenb < 21 * 32)
		return 0;

	float angle, ratio;
	angle = (float)mul_min / lena / lenb;
	ratio = (float)lena / lenb;

	if (angle > 0.5f)
		return 0;

	if (ratio < 0.67f || ratio > 1.5f)
		return 0;

	YyQRFinderCenter center_temp[3];
	center_temp[0] = center[pcen];
	center_temp[1] = center[qcen1];
	center_temp[2] = center[qcen2];
	center[0] = center_temp[0];
	center[1] = center_temp[1];
	center[2] = center_temp[2];
	pcen = 0;
	qcen1 = 1;
	qcen2 = 2;

#if YY_IMG_SCANNER_QR_TIME
	time_stamp();
#endif

#if YY_IMG_SCANNER_QR_DISP
	for (int p = 0; p < 3; p++)
	{
		imdisp[width * (center[p].pos[1] / 32) + center[p].pos[0] / 32] = 127;
		imdisp[width * (center[p].pos[1] / 32 + 1) + center[p].pos[0] / 32 + 1] = 255;
		imdisp[width * (center[p].pos[1] / 32 - 1) + center[p].pos[0] / 32 + 1] = 255;
		imdisp[width * (center[p].pos[1] / 32 + 1) + center[p].pos[0] / 32 - 1] = 255;
		imdisp[width * (center[p].pos[1] / 32 - 1) + center[p].pos[0] / 32 - 1] = 255;
	}
#endif

	if (imgif->scale != 1)
	{
		int s = imgif->scale;
		imgif->img = imgif->imsamp;
		imgif->imdisp = imgif->imsamp;
		imgif->w *= s;
		imgif->h *= s;
		img = imgif->img;
		width = imgif->w;
		height = imgif->h;

		center[0].pos[0] *= s;
		center[0].pos[1] *= s;
		center[1].pos[0] *= s;
		center[1].pos[1] *= s;
		center[2].pos[0] *= s;
		center[2].pos[1] *= s;

		center[0].wid_cen *= s;
		center[0].wid_sum5 *= s;
		center[1].wid_cen *= s;
		center[1].wid_sum5 *= s;
		center[2].wid_cen *= s;
		center[2].wid_sum5 *= s;

		center[0].wid_sum5x *= s;
		center[0].wid_sum5y *= s;
		center[1].wid_sum5x *= s;
		center[1].wid_sum5y *= s;
		center[2].wid_sum5x *= s;
		center[2].wid_sum5y *= s;
	}

	YyImgCurfit fit[4];
	for (int i = 0; i < 4; i++)
	{
		fit[i].pnum = 2;
		fit[i].maxlen = 500;
		fit[i].px = malloc(fit[i].maxlen * sizeof(float));
		fit[i].py = malloc(fit[i].maxlen * sizeof(float));
	}
	yy_qr_finder_edge(center, fit);

	for (int k = 0; k < 4; k++)
	{
		if (fit[k].px[0] < 0 || fit[k].px[1] < 0 || fit[k].py[0] < 0 || fit[k].py[1] < 0
			|| fit[k].px[0] > width - 1 || fit[k].px[1] > width - 1 || fit[k].py[0] > height - 1 || fit[k].py[1] > height - 1
			|| (k >= 2 && (fit[k].px[2] < 0 || fit[k].py[2] < 0 || fit[k].px[2] > width - 1 || fit[k].py[2] > height - 1)))
		{
			for (int i = 0; i < 4; i++)
			{
				free(fit[i].px);
				free(fit[i].py);
			}
			return 0;
		}
	}

	yy_img_curfit_line_edge_2ep(imgif, &fit[0]);
	yy_img_curfit_line_edge_2ep(imgif, &fit[1]);
	yy_img_curfit_line_edge_1ep(imgif, &fit[2]);
	yy_img_curfit_line_edge_1ep(imgif, &fit[3]);

#if YY_IMG_SCANNER_QR_DISP// || 1
	yy_img_curfit_line_draw(imgif, &fit[0]);
	yy_img_curfit_point_draw(imgif, &fit[0]);
	yy_img_curfit_line_draw(imgif, &fit[1]);
	yy_img_curfit_point_draw(imgif, &fit[1]);
	yy_img_curfit_line_draw(imgif, &fit[2]);
	yy_img_curfit_point_draw(imgif, &fit[2]);
	yy_img_curfit_line_draw(imgif, &fit[3]);
	yy_img_curfit_point_draw(imgif, &fit[3]);
#endif

	for (int i = 0; i < 4; i++)
	{
		free(fit[i].px);
		free(fit[i].py);
	}

	float corner[8];
	yy_img_curfit_line_isect(&fit[0], &fit[1], &corner[0]);
	yy_img_curfit_line_isect(&fit[1], &fit[2], &corner[2]);
	yy_img_curfit_line_isect(&fit[2], &fit[3], &corner[4]);
	yy_img_curfit_line_isect(&fit[3], &fit[0], &corner[6]);

#if YY_IMG_SCANNER_QR_TIME
	time_stamp();
#endif

	//边界检查
	for (int i = 0; i < 4; i++)
	{
		if (!(corner[i * 2] > 0 && corner[i * 2] < width - 1))
			return 0;
		if (!(corner[i * 2 + 1] > 0 && corner[i * 2 + 1] < height - 1))
			return 0;
	}

	//顶点ABCD必须满足线段AC与线段BD相交
	//错误案例626,234,125,227,432,171,112,164
	float cornerd1, cornerd2;
	float ptx[2], pty[2];
	fit[0].pnum = 2;
	fit[0].maxlen = 2;
	fit[0].px = ptx;
	fit[0].py = pty;

	ptx[0] = corner[0];
	pty[0] = corner[1];
	ptx[1] = corner[4];
	pty[1] = corner[5];
	yy_img_curfit_line(fit);
	cornerd1 = fit->arg[0] * corner[2] + fit->arg[1] * corner[3] - 1;
	cornerd2 = fit->arg[0] * corner[6] + fit->arg[1] * corner[7] - 1;
	if (cornerd1 * cornerd2 >= 0)
		return 0;

	ptx[0] = corner[2];
	pty[0] = corner[3];
	ptx[1] = corner[6];
	pty[1] = corner[7];
	yy_img_curfit_line(fit);
	cornerd1 = fit->arg[0] * corner[0] + fit->arg[1] * corner[1] - 1;
	cornerd2 = fit->arg[0] * corner[4] + fit->arg[1] * corner[5] - 1;
	if (cornerd1 * cornerd2 >= 0)
		return 0;

	YyImgPerspect hom;
	YyImgPerspect homtab[4];

	//corner是左上,左下,右下,右上,故调整一下次序
	hom.x0 = corner[0];
	hom.x1 = corner[6];
	hom.x2 = corner[4];
	hom.x3 = corner[2];
	hom.y0 = corner[1];
	hom.y1 = corner[7];
	hom.y2 = corner[5];
	hom.y3 = corner[3];

	img_perspect_calc(&hom);

	int gridn = yy_qr_get_gridn(center, &hom);
	int gridr = (gridn + 3) / 4;
	if (gridr < 6 || gridr > 16)
		return 0;

	hom.sx = 0;
	hom.sy = 0;
	hom.scale = gridn;

	//若二维码尺寸太小，可用大图采样
	/*int codescale;
	codescale = (lena + lenb) / (gridn - 7);
	printf("%d\n", codescale);
	if (codescale < 2 * 2 * 32)
	{
	imgif->img = imgif->imsamp;
	imgif->w *= 2;
	imgif->h *= 2;
	img = imgif->img;
	width = imgif->w ;
	height = imgif->h;

	for (int i = 0; i < 8; i++)
	corner[i] = corner[i] * 2 + 1;

	center[0].pos[0] *= 2;
	center[0].pos[1] *= 2;
	center[1].pos[0] *= 2;
	center[1].pos[1] *= 2;
	center[2].pos[0] *= 2;
	center[2].pos[1] *= 2;

	center[0].pos[0] += 32;
	center[0].pos[1] += 32;
	center[1].pos[0] += 32;
	center[1].pos[1] += 32;
	center[2].pos[0] += 32;
	center[2].pos[1] += 32;

	hom.x0 = corner[0];
	hom.x1 = corner[6];
	hom.x2 = corner[4];
	hom.x3 = corner[2];
	hom.y0 = corner[1];
	hom.y1 = corner[7];
	hom.y2 = corner[5];
	hom.y3 = corner[3];

	img_perspect_calc(&hom);
	}*/

	//定位标志校验，解决边框问题
	//若边缘定位后，定位标志采样误差比原始定位标志的结果更差，则使用原始定位结果
	for (int c = 0; c < 3; c++)
	{
		int stx = 0, sty = 0;
		if (c == 1)
			stx = gridn - 7;
		else if (c == 2)
			sty = gridn - 7;

		int errcnt, errcnt2;
		errcnt = yy_qr_loc_check(imgif, region, &hom, gridn, stx, sty);
		if (errcnt < 4)
			continue;
		if (errcnt == -1)
			return 0;

		YyImgPerspect hom2;
		hom2 = hom;

		float affx, affy;
		float imu, imv;
		affx = (stx + 3.5f) / gridn;
		affy = (sty + 3.5f) / gridn;
		img_perspect_k2pix(&hom, affx, affy, &imu, &imv);

		if ((!(imu > 0 && imu < width - 1)) || (!(imv > 0 && imv < height - 1)))
			return 0;

		if (c == 0)
		{
			hom2.x0 += center[c].pos[0] / 32.0f - imu;
			hom2.y0 += center[c].pos[1] / 32.0f - imv;
		}
		else if (c == 1)
		{
			hom2.x1 += center[c].pos[0] / 32.0f - imu;
			hom2.y1 += center[c].pos[1] / 32.0f - imv;
			hom2.x2 += center[c].pos[0] / 32.0f - imu;
			hom2.y2 += center[c].pos[1] / 32.0f - imv;
		}
		else
		{
			hom2.x2 += center[c].pos[0] / 32.0f - imu;
			hom2.y2 += center[c].pos[1] / 32.0f - imv;
			hom2.x3 += center[c].pos[0] / 32.0f - imu;
			hom2.y3 += center[c].pos[1] / 32.0f - imv;
		}
		img_perspect_calc(&hom2);

		errcnt2 = yy_qr_loc_check(imgif, region, &hom2, gridn, stx, sty);
		if (errcnt2 == -1)
			return 0;
		//printf("errcnt:%d  errcnt2:%d\n", errcnt, errcnt2);
		if (errcnt2 + 4 < errcnt)
			hom = hom2;
	}

	if (gridr >= 12)
	{
		//版本7以上，分4块
		// 0 1 2
		// 3 4 5
		// 6 7 8
		float affx[9], affy[9];
		affx[0] = (6.5f) / gridn;
		affy[0] = (6.5f) / gridn;
		affx[1] = 0.5f;
		affy[1] = (6.5f) / gridn;
		affx[2] = (gridn - 6.5f) / gridn;
		affy[2] = (6.5f) / gridn;
		affx[3] = (6.5f) / gridn;
		affy[3] = 0.5f;
		affx[4] = 0.5f;
		affy[4] = 0.5f;
		affx[5] = (gridn - 6.5f) / gridn;
		affy[5] = 0.5f;
		affx[6] = (6.5f) / gridn;
		affy[6] = (gridn - 6.5f) / gridn;
		affx[7] = 0.5f;
		affy[7] = (gridn - 6.5f) / gridn;
		affx[8] = (gridn - 6.5f) / gridn;
		affy[8] = (gridn - 6.5f) / gridn;

		float imu[9], imv[9];
		unsigned char restab[9];
		restab[0] = yy_model33_shift(imgif, region, &hom, yy_qr_finderinner0_data, affx[0], affy[0], 1.0f / gridn, &imu[0], &imv[0]);
		restab[1] = yy_model33_shift(imgif, region, &hom, yy_qr_affine_data, affx[1], affy[1], 1.0f / gridn, &imu[1], &imv[1]);
		restab[2] = yy_model33_shift(imgif, region, &hom, yy_qr_finderinner1_data, affx[2], affy[2], 1.0f / gridn, &imu[2], &imv[2]);
		restab[3] = yy_model33_shift(imgif, region, &hom, yy_qr_affine_data, affx[3], affy[3], 1.0f / gridn, &imu[3], &imv[3]);
		restab[4] = yy_model33_shift(imgif, region, &hom, yy_qr_affine_data, affx[4], affy[4], 1.0f / gridn, &imu[4], &imv[4]);
		restab[5] = yy_model33_shift(imgif, region, &hom, yy_qr_affine_data, affx[5], affy[5], 1.0f / gridn, &imu[5], &imv[5]);
		restab[6] = yy_model33_shift(imgif, region, &hom, yy_qr_finderinner2_data, affx[6], affy[6], 1.0f / gridn, &imu[6], &imv[6]);
		restab[7] = yy_model33_shift(imgif, region, &hom, yy_qr_affine_data, affx[7], affy[7], 1.0f / gridn, &imu[7], &imv[7]);
		restab[8] = yy_model33_shift(imgif, region, &hom, yy_qr_affine_data, affx[8], affy[8], 1.0f / gridn, &imu[8], &imv[8]);

		if (restab[1] && restab[3] && restab[4])
		{
			homtab[0].x0 = imu[0];
			homtab[0].x1 = imu[1];
			homtab[0].x2 = imu[4];
			homtab[0].x3 = imu[3];
			homtab[0].y0 = imv[0];
			homtab[0].y1 = imv[1];
			homtab[0].y2 = imv[4];
			homtab[0].y3 = imv[3];
			homtab[0].sx = 6.5f;
			homtab[0].sy = 6.5f;
			homtab[0].scale = gridn * 0.5f - 6.5f;
			img_perspect_calc(&homtab[0]);
		}
		else
		{
			homtab[0] = hom;
		}

		if (restab[1] && restab[4] && restab[5])
		{
			homtab[1].x0 = imu[1];
			homtab[1].x1 = imu[2];
			homtab[1].x2 = imu[5];
			homtab[1].x3 = imu[4];
			homtab[1].y0 = imv[1];
			homtab[1].y1 = imv[2];
			homtab[1].y2 = imv[5];
			homtab[1].y3 = imv[4];
			homtab[1].sx = gridn * 0.5f;
			homtab[1].sy = 6.5f;
			homtab[1].scale = gridn * 0.5f - 6.5f;
			img_perspect_calc(&homtab[1]);
		}
		else
		{
			homtab[1] = hom;
		}

		if (restab[3] && restab[4] && restab[7])
		{
			homtab[2].x0 = imu[3];
			homtab[2].x1 = imu[4];
			homtab[2].x2 = imu[7];
			homtab[2].x3 = imu[6];
			homtab[2].y0 = imv[3];
			homtab[2].y1 = imv[4];
			homtab[2].y2 = imv[7];
			homtab[2].y3 = imv[6];
			homtab[2].sx = 6.5f;
			homtab[2].sy = gridn * 0.5f;
			homtab[2].scale = gridn * 0.5f - 6.5f;
			img_perspect_calc(&homtab[2]);
		}
		else
		{
			homtab[2] = hom;
		}

		if (restab[4] && restab[5] && restab[7] && restab[8])
		{
			homtab[3].x0 = imu[4];
			homtab[3].x1 = imu[5];
			homtab[3].x2 = imu[8];
			homtab[3].x3 = imu[7];
			homtab[3].y0 = imv[4];
			homtab[3].y1 = imv[5];
			homtab[3].y2 = imv[8];
			homtab[3].y3 = imv[7];
			homtab[3].sx = gridn * 0.5f;
			homtab[3].sy = gridn * 0.5f;
			homtab[3].scale = gridn * 0.5f - 6.5f;
			img_perspect_calc(&homtab[3]);
		}
		else
		{
			homtab[3] = hom;
		}
	}
	else if (gridr >= 7)
	{
		//版本2以上，右下角有对齐标志
		// 0 1
		// 3 2
		float affx[4], affy[4];
		affx[2] = (gridn - 6.5f) / gridn;
		affy[2] = (gridn - 6.5f) / gridn;

		float imu[4], imv[4];
		if (yy_model33_shift(imgif, region, &hom, yy_qr_affine_data, affx[2], affy[2], 1.0f / gridn, &imu[2], &imv[2]))
		{
			affx[0] = (6.5f) / gridn;
			affy[0] = (6.5f) / gridn;
			affx[1] = (gridn - 6.5f) / gridn;
			affy[1] = (6.5f) / gridn;
			affx[3] = (6.5f) / gridn;
			affy[3] = (gridn - 6.5f) / gridn;
			yy_model33_shift(imgif, region, &hom, yy_qr_finderinner0_data, affx[0], affy[0], 1.0f / gridn, &imu[0], &imv[0]);
			yy_model33_shift(imgif, region, &hom, yy_qr_finderinner1_data, affx[1], affy[1], 1.0f / gridn, &imu[1], &imv[1]);
			yy_model33_shift(imgif, region, &hom, yy_qr_finderinner2_data, affx[3], affy[3], 1.0f / gridn, &imu[3], &imv[3]);
			hom.x0 = imu[0];
			hom.x1 = imu[1];
			hom.x2 = imu[2];
			hom.x3 = imu[3];
			hom.y0 = imv[0];
			hom.y1 = imv[1];
			hom.y2 = imv[2];
			hom.y3 = imv[3];
			hom.sx = 6.5f;
			hom.sy = 6.5f;
			hom.scale = gridn - 13;
			img_perspect_calc(&hom);
		}
	}

	unsigned char *imget = malloc(gridn * gridn);
	unsigned int *gatex = calloc(gridr, 4);
	unsigned int *gatey = calloc(gridr, 4);

	float gyf, gxf;
	for (int gy = 0; gy < gridn; gy++)
	{
		for (int gx = 0; gx < gridn; gx++)
		{
			if (gridr >= 12)
			{
				if (gx <= gridn / 2 && gy <= gridn / 2)
					hom = homtab[0];
				else if (gy <= gridn / 2)
					hom = homtab[1];
				else if (gx <= gridn / 2)
					hom = homtab[2];
				else
					hom = homtab[3];
			}

			gyf = (gy + 0.5f - hom.sy) / hom.scale;
			gxf = (gx + 0.5f - hom.sx) / hom.scale;

			float imu, imv;
			img_perspect_k2pix(&hom, gxf, gyf, &imu, &imv);
			if ((!(imu > 0 && imu < width - 1)) || (!(imv > 0 && imv < height - 1)))
				continue;

			//Bilinear interpolation
			int imu1 = imu;
			int imv1 = imv;
			int imu2 = imu1 + 1;
			int imv2 = imv1 + 1;

			float k1 = (imu2 - imu) * (imv2 - imv);
			float k2 = (imu - imu1) * (imv2 - imv);
			float k3 = (imu2 - imu) * (imv - imv1);
			float k4 = (imu - imu1) * (imv - imv1);

			unsigned char p = k1 * img[imv1 * width + imu1]
				+ k2 * img[imv1 * width + imu2]
				+ k3 * img[imv2 * width + imu1]
				+ k4 * img[imv2 * width + imu2];

			//img[imv1 * width + imu1] = (1 - k1) * img[imv1 * width + imu1] + k1 * 255;
			//img[imv1 * width + imu2] = (1 - k2) * img[imv1 * width + imu2] + k2 * 255;
			//img[imv2 * width + imu1] = (1 - k3) * img[imv2 * width + imu1] + k3 * 255;
			//img[imv2 * width + imu2] = (1 - k4) * img[imv2 * width + imu2] + k4 * 255;

			gatex[(gx + 3) / 4] += p;
			gatey[(gy + 3) / 4] += p;
			imget[gy * gridn + gx] = p;
		}
	}

	for (int i = 1; i < gridr; i++)
	{
		gatex[i] /= 4 * gridn;
		gatey[i] /= 4 * gridn;
	}
	gatex[0] = gatex[1];
	gatey[0] = gatey[1];

	for (int gy = 0; gy < gridn; gy++)
	{
		for (int gx = 0; gx < gridn; gx++)
		{
			imget[gy * gridn + gx] = imget[gy * gridn + gx] * 2 > gatex[(gx + 3) / 4] + gatey[(gy + 3) / 4] ? 255 : 0;
		}
	}

	free(gatex);
	free(gatey);

#if YY_IMG_SCANNER_QR_EDGE
	//四周增加一个像素的白边
	gridn += 2;
	unsigned char *imedg = malloc(gridn * gridn);
	for (int i = 0; i < gridn; i++)
	{
		imedg[i] = 255;
		imedg[(gridn - 1) * gridn + i] = 255;
		imedg[i * gridn] = 255;
		imedg[i * gridn + gridn - 1] = 255;
	}
	for (int gy = 1; gy < gridn - 1; gy++)
	{
		for (int gx = 1; gx < gridn - 1; gx++)
		{
			imedg[gy * gridn + gx] = imget[(gy - 1) * (gridn - 2) + (gx - 1)];
		}
	}
	free(imget);
	imget = imedg;

#if YY_IMG_SCANNER_QR_RSET
	//重置3个定位标志
	yy_qr_finder_set(imget, gridn, 0);
	yy_qr_finder_set(imget, gridn, gridn - 9);
	yy_qr_finder_set(imget, gridn, (gridn - 9) * gridn);
#endif

#endif

	if (qr_main(imget, gridn) == 0)
	{
		free(imget);
		return 0;
	}
	free(imget);

#if YY_IMG_SCANNER_QR_TIME
	time_stamp();
#endif

	return 1;
}

unsigned char yy_img_scanner_qr(YyImgInterface *imgif, YyImgRegion *region)
{
	unsigned char res;

	res = yy_img_scanner_qr_hv(imgif, region);

	if (res == 0)
		res = yy_img_scanner_qr_45(imgif, region);

	return res;
}
