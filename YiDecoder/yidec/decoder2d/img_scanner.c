
#include "decoder2d/img_scanner.h"

YyImgScannerConf yy_img_scanner_conf = { 19, 10, 2, 2, 1, 12, 5, 32 };
YyImgScannerPackConf yy_img_scanner_pack_conf = { 32 };

//亚像素边缘提取 x > bound1 + bound2
int yy_img_scanner_edge(char type, int x)
{
	//return x * 32;
	char *dimg = yy_img_scanner_conf.dimg;
	int bound1 = yy_img_scanner_conf.bound1;
	int bound2 = yy_img_scanner_conf.bound2; 
	int noise = yy_img_scanner_conf.noise;
	int dthres = yy_img_scanner_conf.dthres;
	int multix = yy_img_scanner_conf.multix;

	int maxx = 0;
	int maxv = 0;
	for (int xx = x - 1; xx >= x - bound1; xx--)
	{
		char dimgtype = type * dimg[xx];
		if (dimgtype < -noise)
			break;
		if (maxv < dimgtype)
		{
			maxv = dimgtype;
			maxx = xx;
		}
	}
	for (int xx = x; xx <= x + bound1; xx++)
	{
		char dimgtype = type * dimg[xx];
		if (dimgtype < -noise)
			break;
		if (maxv < dimgtype)
		{
			maxv = dimgtype;
			maxx = xx;
		}
	}
	if (maxx == 0)
		return 0;

	int realedge = 0;
	int dimgsum = 0;
	for (int xx = maxx - 1; xx >= maxx - bound2; xx--)
	{
		char dimgtype = type * dimg[xx];
		if (dimgtype < -noise)
			break;
		if (dimgtype > 0)
		{
			realedge += dimg[xx] * xx;
			dimgsum += dimg[xx];
		}
	}
	for (int xx = maxx; xx <= maxx + bound2; xx++)
	{
		char dimgtype = type * dimg[xx];
		if (dimgtype < -noise)
			break;
		if (dimgtype > 0)
		{
			realedge += dimg[xx] * xx;
			dimgsum += dimg[xx];
		}
	}
	if (dimgsum > -dthres && dimgsum < dthres)
		return 0;

	return realedge * multix / dimgsum;
}

void yy_img_scanner()
{
	int winsize = yy_img_scanner_conf.winsize;
	int winsize_half = yy_img_scanner_conf.winsize_half;
	int margin = yy_img_scanner_conf.margin;
	int multix = yy_img_scanner_conf.multix;
	int len = yy_img_scanner_conf.len;
	unsigned char *img = yy_img_scanner_conf.img;
	unsigned char *bin = yy_img_scanner_conf.bin;
	char *dimg = yy_img_scanner_conf.dimg;
	int *widstream = yy_img_scanner_conf.widstream;
	int *posstream = yy_img_scanner_conf.posstream;
	int maxstreamlen = yy_img_scanner_conf.maxstreamlen;

	if (len < winsize)
		winsize_half = len / 4;

	winsize = winsize_half * 2 - 1;

	int winsum = 0;
	for (int x = 0; x < winsize; x++)
		winsum += img[x];
	int gate = winsum / winsize;
	for (int x = 1; x < winsize_half; x++)
	{
		bin[x] = img[x] > gate;
		dimg[x] = (img[x] - img[x - 1]) / 2;
	}
	for (int x = winsize_half; x <= len - winsize_half; x++)
	{
		winsum += img[x + winsize_half - 1] - img[x - winsize_half];
		gate = winsum / winsize;
		bin[x] = img[x] > gate;
		dimg[x] = (img[x] - img[x - 1]) / 2;
	}
	for (int x = len - winsize_half + 1; x < len; x++)
	{
		bin[x] = img[x] > gate;
		dimg[x] = (img[x] - img[x - 1]) / 2;
	}

	int eidx = 0;
	int edge_n = 0, edge_p = 0;
	for (int x = margin; x < len - margin; x++)
	{
		if (bin[x] != bin[x - 1])
		{
			edge_n = yy_img_scanner_edge(bin[x] ? 1 : -1, x);
			if (edge_n && edge_n > edge_p)
			{
				widstream[eidx] = edge_n - edge_p;
				posstream[eidx] = (edge_n + edge_p) / 2;
				edge_p = edge_n;
				//cout << widstream[eidx] << " ";
				//if (eidx % 20 == 19)
				//	cout << endl;
				eidx++;
				if (eidx >= maxstreamlen - 1)
					break;
			}
		}
	}
	widstream[eidx] = (len - margin) * multix - edge_p;
	posstream[eidx] = ((len - margin) * multix + edge_p) / 2;
	//cout << widstream[eidx] << endl;

	yy_img_scanner_conf.streamlen = eidx + 1;
	//cout << eidx + 1 << endl;
}

void yy_img_scanner_fast()
{
	int winsize = yy_img_scanner_conf.winsize;
	int winsize_half = yy_img_scanner_conf.winsize_half;
	int margin = yy_img_scanner_conf.margin;
	int multix = yy_img_scanner_conf.multix;
	int len = yy_img_scanner_conf.len;
	unsigned char *img = yy_img_scanner_conf.img;
	unsigned char *bin = yy_img_scanner_conf.bin;
	char *dimg = yy_img_scanner_conf.dimg;
	int *widstream = yy_img_scanner_conf.widstream;
	int *posstream = yy_img_scanner_conf.posstream;
	int maxstreamlen = yy_img_scanner_conf.maxstreamlen;

	if (len < winsize)
		winsize_half = len / 2;

	winsize_half = winsize_half / 2 * 2;
	winsize = winsize_half * 2 - 4;

	int winsize_q = winsize / 4;
	int winsum = 0;
	for (int x = 0; x < winsize; x += 4)
		winsum += img[x];
	int gate = winsum / winsize_q;
	for (int x = 1; x < winsize_half; x++)
	{
		bin[x] = img[x] > gate;
	}
	for (int x = winsize_half; x <= len - winsize_half; x += 4)
	{
		winsum += img[x + winsize_half - 4] - img[x - winsize_half];
		gate = winsum / winsize_q;
		bin[x] = img[x] > gate;
		bin[x + 1] = img[x + 1] > gate;
		bin[x + 2] = img[x + 2] > gate;
		bin[x + 3] = img[x + 3] > gate;
	}
	for (int x = len - winsize_half + 1; x < len; x++)
	{
		bin[x] = img[x] > gate;
	}

	int eidx = 0;
	int edge_n = 0, edge_p = 0;
	for (int x = margin; x < len - margin; x++)
	{
		if (bin[x] != bin[x - 1])
		{
			//dimg[x - 4] = (img[x - 4] - img[x - 5]) / 2;
			//dimg[x - 3] = (img[x - 3] - img[x - 4]) / 2;
			dimg[x - 2] = (img[x - 2] - img[x - 3]) / 2;
			dimg[x - 1] = (img[x - 1] - img[x - 2]) / 2;
			dimg[x] = (img[x] - img[x - 1]) / 2;
			dimg[x + 1] = (img[x + 1] - img[x]) / 2;
			dimg[x + 2] = (img[x + 2] - img[x + 1]) / 2;
			//dimg[x + 3] = (img[x + 3] - img[x + 2]) / 2;
			//dimg[x + 4] = (img[x + 4] - img[x + 3]) / 2;

			edge_n = yy_img_scanner_edge(bin[x] ? 1 : -1, x);
			if (edge_n && edge_n > edge_p)
			{
				widstream[eidx] = edge_n - edge_p;
				posstream[eidx] = (edge_n + edge_p) / 2;
				edge_p = edge_n;
				//cout << widstream[eidx] << " ";
				//if (eidx % 20 == 19)
				//	cout << endl;
				eidx++;
				if (eidx >= maxstreamlen - 1)
					break;
			}
		}
	}
	widstream[eidx] = (len - margin) * multix - edge_p;
	posstream[eidx] = ((len - margin) * multix + edge_p) / 2;
	//cout << widstream[eidx] << endl;

	yy_img_scanner_conf.streamlen = eidx + 1;
	//cout << eidx + 1 << endl;
}

void yy_img_scanner_ddif()
{
	int winsize = yy_img_scanner_conf.winsize;
	int winsize_half = yy_img_scanner_conf.winsize_half;
	int margin = yy_img_scanner_conf.margin;
	int multix = yy_img_scanner_conf.multix;
	int len = yy_img_scanner_conf.len;
	unsigned char *img = yy_img_scanner_conf.img;
	unsigned char *bin = yy_img_scanner_conf.bin;
	char *dimg = yy_img_scanner_conf.dimg;
	int *widstream = yy_img_scanner_conf.widstream;
	int *posstream = yy_img_scanner_conf.posstream;
	int maxstreamlen = yy_img_scanner_conf.maxstreamlen;

	for (int x = 0; x < len - 1; x++)
	{
		dimg[x] = (img[x + 1] - img[x]) / 2;
	}

	int thres_min = 8;
	int last_sign = 1;
	int last_avg_width = 20;
	int y11_thres = thres_min;
	int y21, y22;
	int eidx = 0;
	int edge_n = 0, edge_p = 0;
	for (int x = 0; x < len - 2; x++)
	{
		//check for 1st sign change, with a denoise threshold
		if (last_sign * dimg[x + 1] > -thres_min)
			continue;

		y22 = dimg[x + 1] - dimg[x];
		y21 = dimg[x + 2] - dimg[x + 1];
		if (y21 * y22 <= 0)//二阶导异号
		{
			int t;
			if (y11_thres <= thres_min)
			{
				t = thres_min;
			}
			else
			{
				int dx = (x + 2) * multix - edge_p;
				t = y11_thres * (8 - dx / last_avg_width) / 8;
				if (t <= thres_min)
					t = y11_thres = thres_min;
			}

			if (dimg[x + 1] > -t && dimg[x + 1] < t)
				continue;

			//update 1st sign and 1st threshold, y11_thres=0.33*y11
			last_sign = (dimg[x + 1] > 0) * 2 - 1;
			y11_thres = last_sign * dimg[x + 1] / 3;

			edge_n = (x + 2) * multix;
			//亚像素边缘调整
			if (y21 == y22)
				edge_n -= multix / 2;
			else
				edge_n -= y21 * multix / (y21 - y22);

			if (edge_n <= edge_p)
				continue;

			last_avg_width = (last_avg_width + edge_n - edge_p) / 2;

			widstream[eidx] = edge_n - edge_p;
			posstream[eidx] = (edge_n + edge_p) / 2;
			edge_p = edge_n;
			//cout << widstream[eidx] << " ";
			//if (eidx % 20 == 19)
			//	cout << endl;
			eidx++;
			if (eidx >= maxstreamlen - 1)
				break;
		}
	}
	widstream[eidx] = (len - 1) * multix - edge_p;
	posstream[eidx] = ((len - 1) * multix + edge_p) / 2;
	//cout << widstream[eidx] << endl;

	yy_img_scanner_conf.streamlen = eidx + 1;
	//cout << eidx + 1 << endl;
}

void yy_img_scanner_pack()
{
	int merge_wid = yy_img_scanner_pack_conf.merge_wid;
	int merge_count = yy_img_scanner_pack_conf.merge_count;
	int *posstream = yy_img_scanner_pack_conf.posstream;
	int *widstream = yy_img_scanner_pack_conf.widstream;
	int *inputpos = yy_img_scanner_pack_conf.inputpos;
	int *inputwid = yy_img_scanner_pack_conf.inputwid;
	int streamlen = yy_img_scanner_pack_conf.streamlen;
	int inputlen = yy_img_scanner_pack_conf.inputlen;

	if (merge_count == 0)
	{
		yy_img_scanner_pack_conf.posstream = inputpos;
		yy_img_scanner_pack_conf.widstream = inputwid;
		yy_img_scanner_pack_conf.streamlen = inputlen;
		yy_img_scanner_pack_conf.merge_count++;
		return;
	}

	int idx = 0, idx_input = 0;
	while (1)
	{
		if (idx >= streamlen || idx_input >= inputlen)
			break;

		int posd = posstream[idx] - inputpos[idx_input];
		float widd = fabsf(widstream[idx] - inputwid[idx_input] * (float)merge_count) / widstream[idx];
		if (posd >= -merge_wid && posd <= merge_wid && widd < 0.12f)
		{
			//cout << idx << " A-B " << idx_input << " " << widd << endl;
			widstream[idx] += inputwid[idx_input];
			posstream[idx] = inputpos[idx_input];
			idx++;
			idx_input++;
		}
		else if (posd > 0)
		{
			//cout << idx_input << " Bdiscard" << endl;
			idx_input++;
		}
		else
		{
			//cout << idx << " Adiscard" << endl;
			widstream[idx] = widstream[idx] * (merge_count + 1) / merge_count;
			idx++;
		}
	}
	yy_img_scanner_pack_conf.merge_count++;
}

