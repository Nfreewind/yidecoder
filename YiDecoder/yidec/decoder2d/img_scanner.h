
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
	int winsize;			//����ƽ����ֵ���ߴ�
	int winsize_half;
	int bound1;				//��Ե���ҷ�Χ
	int bound2;				//�����ر�Ե���㷶Χ
	int noise;				//��Ե���������ֵ
	int dthres;				//��Ե�����ֵ
	int margin;				//��Ե������߷�Χ
	int multix;				//�Ŵ���
	int len;
	unsigned char *img;
	unsigned char *bin;
	char *dimg;
	int maxstreamlen;		//����Ե����
	int streamlen;			//��� ��Ե����
	int *posstream;			//��� ����λ��
	int *widstream;			//��� �����
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
