
#include "yy/img_suspect.h"

unsigned char yy_img_sus_grid(YyImgInterface *imgif, YyImgRegion *region)
{
	int idx = 0;
	int maxidx[4] = { 0 };
	int maxedge[4] = { 0 };

	unsigned char *img = imgif->img;
	int width = imgif->w;
	int height = imgif->h;

	int THRES_G = 1200;
	int THRES_T = 3000;
	int GRID_X = width / YY_IMG_SUS_GRID_N;
	int GRID_Y = height / YY_IMG_SUS_GRID_N;

	int min_x, min_y, max_x, max_y;
	min_x = 2147483647;
	min_y = 2147483647;
	max_x = 0;
	max_y = 0;

	int *edge = (int *)calloc(YY_IMG_SUS_GRID_N * YY_IMG_SUS_GRID_N, sizeof(int));
	int *edge_cnt = (int *)calloc(YY_IMG_SUS_GRID_N, sizeof(int));
	unsigned char *edge_id = (unsigned char *)calloc(YY_IMG_SUS_GRID_N * YY_IMG_SUS_GRID_N, sizeof(unsigned char));

	for (int ygrid = 0; ygrid < YY_IMG_SUS_GRID_N; ygrid++)
	{
		for (int xgrid = 0; xgrid < YY_IMG_SUS_GRID_N; xgrid++)
		{
			for (int y = GRID_Y * ygrid; y < GRID_Y * ygrid + GRID_Y - 1; y++)
			{
				for (int x = GRID_X * xgrid; x < GRID_X * xgrid + GRID_X - 1; x++)
				{
					int difx = (int)img[y * width + x] - img[y * width + x + 1];
					int dify = (int)img[y * width + x] - img[(y + 1) * width + x];
					if (difx > 10 || difx < -10)
						edge[idx] += 1;
					if (dify > 10 || dify < -10)
						edge[idx] += 1;
				}
			}
			//int minidx = 0;
			//int minval = 2147483647;
			//for (int i = 0; i < 4; i++)
			//{
			//	if (minval > maxedge[i])
			//	{
			//		minval = maxedge[i];
			//		minidx = i;
			//	}
			//}
			//if (minval < edge[idx])
			//{
			//	maxedge[minidx] = edge[idx];
			//	maxidx[minidx] = idx;
			//}
			//printf("%d ", edge[idx]);
			idx++;
		}
		//printf("\n");
	}
	//printf("\n");

	idx = 1;
	for (int i = 0; i < YY_IMG_SUS_GRID_N * YY_IMG_SUS_GRID_N; i++)
	{
		if (edge[i] < THRES_G)
			continue;
		edge_id[i] = idx++;
#if YY_IMG_SUS_DISP
		int startx = GRID_X * (i % YY_IMG_SUS_GRID_N);
		int starty = GRID_Y * (i / YY_IMG_SUS_GRID_N);
		for (int y = 0; y < GRID_Y; y++)
			img[(starty + y) * width + startx] = 127;
		for (int y = 0; y < GRID_Y; y++)
			img[(starty + y) * width + startx + GRID_X - 2] = 127;
		for (int x = 0; x < GRID_X; x++)
			img[starty * width + startx + x] = 127;
		for (int x = 0; x < GRID_X; x++)
			img[(starty + GRID_Y - 2) * width + startx + x] = 127;
#endif
	}

	for (int i = 0; i < YY_IMG_SUS_GRID_N * YY_IMG_SUS_GRID_N; i++)
	{
		int yy = i / YY_IMG_SUS_GRID_N;
		int xx = i % YY_IMG_SUS_GRID_N;
		if (xx == YY_IMG_SUS_GRID_N - 1 || yy == YY_IMG_SUS_GRID_N - 1)
			continue;
		if (edge_id[i] == 0)
			continue;

		if (edge_id[i + 1] && edge_id[i] > edge_id[i + 1])
			edge_id[i] = edge_id[i + 1];
		if (edge_id[i + YY_IMG_SUS_GRID_N] && edge_id[i] > edge_id[i + YY_IMG_SUS_GRID_N])
			edge_id[i] = edge_id[i + YY_IMG_SUS_GRID_N];

		if (edge_id[i + 1])
			edge_id[i + 1] = edge_id[i];
		if (edge_id[i + YY_IMG_SUS_GRID_N])
			edge_id[i + YY_IMG_SUS_GRID_N] = edge_id[i];
	}
	for (int i = YY_IMG_SUS_GRID_N * YY_IMG_SUS_GRID_N - 1; i >= 0; i--)
	{
		int yy = i / YY_IMG_SUS_GRID_N;
		int xx = i % YY_IMG_SUS_GRID_N;
		if (xx == 0 || yy == 0)
			continue;
		if (edge_id[i] == 0)
			continue;

		if (edge_id[i - 1] && edge_id[i] > edge_id[i - 1])
			edge_id[i] = edge_id[i - 1];
		if (edge_id[i - YY_IMG_SUS_GRID_N] && edge_id[i] > edge_id[i - YY_IMG_SUS_GRID_N])
			edge_id[i] = edge_id[i - YY_IMG_SUS_GRID_N];

		if (edge_id[i - 1])
			edge_id[i - 1] = edge_id[i];
		if (edge_id[i - YY_IMG_SUS_GRID_N])
			edge_id[i - YY_IMG_SUS_GRID_N] = edge_id[i];
	}

	for (int i = 0; i < YY_IMG_SUS_GRID_N * YY_IMG_SUS_GRID_N; i++)
	{
		if (edge_id[i] > 0 && edge_id[i] <= YY_IMG_SUS_GRID_N)
			edge_cnt[edge_id[i] - 1] += edge[i];
	}
	//printf("%d %d %d %d\n", edge_cnt[0], edge_cnt[1], edge_cnt[2], edge_cnt[3]);

	int max_cnt = 0;
	int max_id = 0;
	for (int i = 0; i < YY_IMG_SUS_GRID_N; i++)
	{
		if (max_cnt < edge_cnt[i])
		{
			max_cnt = edge_cnt[i];
			max_id = i + 1;
		}
	}
	//printf("%d\n", max_id);

	if (max_cnt < THRES_T)
	{
		free(edge);
		free(edge_cnt);
		free(edge_id);
		return 0;
	}

	for (int i = 0; i < YY_IMG_SUS_GRID_N * YY_IMG_SUS_GRID_N; i++)
	{
		if (edge_id[i] != max_id)
			continue;

		int startx = GRID_X * (i % YY_IMG_SUS_GRID_N);
		int starty = GRID_Y * (i / YY_IMG_SUS_GRID_N);

		if (min_x > startx)
			min_x = startx;
		if (min_y > starty)
			min_y = starty;
		if (max_x < startx + GRID_X)
			max_x = startx + GRID_X;
		if (max_y < starty + GRID_Y)
			max_y = starty + GRID_Y;
	}

	free(edge);
	free(edge_cnt);
	free(edge_id);

	min_x -= GRID_X / 2;
	min_y -= GRID_Y / 2;
	max_x += GRID_X / 2;
	max_y += GRID_Y / 2;

	int len_x = max_x - min_x;
	int len_y = max_y - min_y;

	if (len_x > len_y)
	{
		min_y -= (len_x - len_y) / 2;
		max_y += (len_x - len_y) / 2;
	}
	else if (len_x < len_y)
	{
		min_x -= (len_y - len_x) / 2;
		max_x += (len_y - len_x) / 2;
	}

	if (min_x < 0)
		min_x = 0;
	if (min_y < 0)
		min_y = 0;
	if (max_x > width)
		max_x = width;
	if (max_y > height)
		max_y = height;

#if YY_IMG_SUS_DISP
	for (int y = min_y; y < max_y; y++)
		img[y * width + min_x] = 255;
	for (int y = min_y; y < max_y; y++)
		img[y * width + max_x - 1] = 255;
	for (int x = min_x; x < max_x; x++)
		img[min_y * width + x] = 255;
	for (int x = min_x; x < max_x; x++)
		img[(max_y - 1) * width + x] = 255;
	for (int y = min_y; y < max_y; y++)
		img[y * width + min_x + 1] = 255;
	for (int y = min_y; y < max_y; y++)
		img[y * width + max_x - 2] = 255;
	for (int x = min_x; x < max_x; x++)
		img[(min_y + 1) * width + x] = 255;
	for (int x = min_x; x < max_x; x++)
		img[(max_y - 2) * width + x] = 255;
#endif

	region->w = max_x - min_x;
	region->h = max_y - min_y;
	region->x = min_x;
	region->y = min_y;

	return 1;
}

unsigned char yy_img_lookfor_sus(YyImgInterface *img, YyImgRegion *region)
{
	return yy_img_sus_grid(img, region);
}
