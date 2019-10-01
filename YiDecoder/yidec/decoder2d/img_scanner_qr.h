
#ifndef _YY_IMG_SCANNER_QR_H_
#define _YY_IMG_SCANNER_QR_H_

#include "basic.h"
#include "decoder2d/img_scanner.h"
#include "decoder2d/img_curfit.h"
#include "decoder2d/img_perspect.h"

#define YY_IMG_SCANNER_QR_DISP 0
#define YY_IMG_SCANNER_QR_TIME 0
#define YY_IMG_SCANNER_QR_RSET 0
#define YY_IMG_SCANNER_QR_EDGE 0

#if YY_IMG_SCANNER_QR_TIME
extern void time_stamp(void);
#endif

typedef struct
{
	int pos[2];
	int wid_sum5x;
	int wid_sum5y;
	int wid_sum5;
	int wid_cen;
	int cross_cnt;
	int type;		//0:ÐÐÁÐ,1:45¶È
}YyQRFinderCenter;

typedef struct
{
	int pos[2];
	int wid_sum5;
	int wid_cen;
}YyQRFinderLine;

typedef struct
{
	YyQRFinderLine *qr_finder_line;
	int count;
	int maxlen;
}YyQRFinderLineCollection;

typedef struct
{
	YyQRFinderLine **qr_finder_line;
	int count;
	int maxlen;
}YyQRFinderCluster;

typedef struct
{
	YyQRFinderCluster *qr_finder_cluster;
	int count;
	int maxlen;
}YyQRFinderClusterCollection;

int yy_qr_decode_e(int e, int s, int n);
void yy_qr_stream_finder(YyQRFinderLineCollection *qr_finder_lines, int *widstream, int *posstream, int streamlen, int line, int yline, int dir);
void yy_qr_finder_cluster(YyQRFinderLineCollection *qr_finder_lines, YyQRFinderCluster *neighbor, YyQRFinderClusterCollection *cluster);
int yy_qr_finder_lines_are_crossing_hv(YyQRFinderLine *hline, YyQRFinderLine *vline);
int yy_qr_finder_lines_are_crossing_45(YyQRFinderLine *hline, YyQRFinderLine *vline);
float yy_qr_finder_ratio(int dx, int dy, int type);
void yy_qr_finder_edge(YyQRFinderCenter *qr_center, YyImgCurfit *fit);
float yy_qr_nnum_calc_xy(int dx, int dy, int sum5);
int yy_qr_nnum_calc(YyQRFinderCenter *qr_center);
void yy_qr_finder_set(unsigned char *imget, unsigned int gridn, unsigned int st);

unsigned char yy_img_scanner_qr_hv(YyImgInterface *imgif, YyImgRegion *region);
unsigned char yy_img_scanner_qr_45(YyImgInterface *imgif, YyImgRegion *region);
unsigned char yy_qr_correct(YyImgInterface *imgif, YyImgRegion *region, YyQRFinderCenter *center);
unsigned char yy_img_scanner_qr(YyImgInterface *imgif, YyImgRegion *region);

#endif //_YY_IMG_SCANNER_QR_H_
