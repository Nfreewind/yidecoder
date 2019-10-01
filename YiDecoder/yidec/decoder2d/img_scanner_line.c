
#include "img_scanner_line.h"

unsigned short yy_data_buf[YY_MAX_DAT_NUM];
unsigned int yy_data_idx, yy_data_h_max, yy_data_v_max;

void yy_img_scanner_line(YyImgInterface *imgif, YyImgRegion *region)
{
	unsigned char *img = imgif->img;
	unsigned char *imdisp = img;
	int width = imgif->w;
	int height = imgif->h;
	
	unsigned char *horizon;
	unsigned char *vertical;

	vertical = (unsigned char *)malloc(height);

	yy_img_scanner_conf.winsize = 49;
	yy_img_scanner_conf.winsize_half = 25;
	yy_img_scanner_conf.dthres = 7;
	yy_img_scanner_conf.len = width;
	yy_img_scanner_conf.maxstreamlen = YY_MAX_DAT_NUM;
	yy_img_scanner_conf.bin = (unsigned char *)malloc(width);
	yy_img_scanner_conf.dimg = (char *)malloc(width);
	yy_img_scanner_conf.widstream = (int *)malloc(yy_img_scanner_conf.maxstreamlen * sizeof(int));
	yy_img_scanner_conf.posstream = (int *)malloc(yy_img_scanner_conf.maxstreamlen * sizeof(int));

	yy_data_idx = 0;
	yy_data_h_max = 0;
	yy_data_v_max = 0;
	memset(yy_data_buf, 0, YY_MAX_DAT_NUM * 2);

	int line_d = height / 80;
	for (int line = 4 * line_d; line <= 12 * line_d; line += line_d)
	{
		horizon = img + width * line;
		
		yy_img_scanner_conf.img = horizon;
		yy_img_scanner_ddif();
	
		//for (int x = 0; x < width; x++)
		//	imdisp[width * line + x] = yy_img_scanner_conf.bin[x] * 255;

		if (yy_data_h_max < yy_img_scanner_conf.streamlen)
			yy_data_h_max = yy_img_scanner_conf.streamlen;
		
		for (int i = 0; i < yy_img_scanner_conf.streamlen; i++)
		{
			yy_data_buf[yy_data_idx++] = yy_img_scanner_conf.widstream[i];
			if (yy_data_idx >= YY_MAX_DAT_NUM)
			{
				free(yy_img_scanner_conf.widstream);
				free(yy_img_scanner_conf.posstream);
				free(yy_img_scanner_conf.bin);
				free(yy_img_scanner_conf.dimg);
				free(vertical);
				return;
			}
		}
	}

	yy_img_scanner_conf.len = height;
	line_d = width / 50;
	for (int line = 3 * line_d; line <= 7 * line_d; line += line_d)
	{
		for (int y = 0; y < height; y++)
			vertical[y] = img[y * width + line];

		yy_img_scanner_conf.img = vertical;
		yy_img_scanner_ddif();
	
		//for (int x = 0; x < height; x++)
		//	imdisp[x * width + line] = yy_img_scanner_conf.bin[x] * 255;

		if (yy_data_v_max < yy_img_scanner_conf.streamlen)
			yy_data_v_max = yy_img_scanner_conf.streamlen;
		
		for (int i = 0; i < yy_img_scanner_conf.streamlen; i++)
		{
			yy_data_buf[yy_data_idx++] = yy_img_scanner_conf.widstream[i];
			if (yy_data_idx >= YY_MAX_DAT_NUM)
			{
				free(yy_img_scanner_conf.widstream);
				free(yy_img_scanner_conf.posstream);
				free(yy_img_scanner_conf.bin);
				free(yy_img_scanner_conf.dimg);
				free(vertical);
				return;
			}
		}
	}

	free(yy_img_scanner_conf.widstream);
	free(yy_img_scanner_conf.posstream);
	free(yy_img_scanner_conf.bin);
	free(yy_img_scanner_conf.dimg);
	free(vertical);
}
