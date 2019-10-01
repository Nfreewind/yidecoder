
#ifndef _YY_IMG_SCANNER_H_
#define _YY_IMG_SCANNER_H_

#include "math.h"
#include "stdlib.h"

typedef struct
{
	unsigned char *img;
	unsigned char *imsamp;
	unsigned char *imdisp;
	int w;
	int h;
	int scale;
}YyImgInterface;

typedef struct
{
	int x;
	int y;
	int w;
	int h;
}YyImgRegion;

typedef struct
{
	int winsize;			//窗口平均二值化尺寸
	int winsize_half;
	int bound1;				//边缘查找范围
	int bound2;				//亚像素边缘计算范围
	int noise;				//边缘检测噪声阈值
	int dthres;				//边缘检测阈值
	int margin;				//边缘检测留边范围
	int multix;				//放大倍数
	int len;
	unsigned char *img;
	unsigned char *bin;
	char *dimg;
	int maxstreamlen;		//最大边缘个数
	int streamlen;			//输出 边缘个数
	int *posstream;			//输出 条纹位置
	int *widstream;			//输出 宽度流
} YyImgScannerConf;

typedef struct
{
	int merge_wid;
	int merge_count;
	int *posstream;
	int *widstream;
	int streamlen;
	int *inputpos;
	int *inputwid;
	int inputlen;
}YyImgScannerPackConf;

extern YyImgScannerConf yy_img_scanner_conf;
extern YyImgScannerPackConf yy_img_scanner_pack_conf;

void yy_img_scanner();
void yy_img_scanner_fast();
void yy_img_scanner_pack();
void yy_img_scanner_ddif();

#endif //_YY_IMG_SCANNER_H_
