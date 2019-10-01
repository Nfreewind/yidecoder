
#include <iostream>
#include <opencv2/opencv.hpp>

#include <stdlib.h>

extern "C"
{
	#include "yidec/yydecoder.h"
}

unsigned char decoder_mainloop(unsigned char *frame)
{
	unsigned char res = 0;
	unsigned char type = 0xFF;
	unsigned char len = 0;
	unsigned char outsym[YY_MAX_SYM_LEN] = { 0 };

	YyImgInterface imgif;
	YyImgRegion region;
	imgif.img = frame;
	imgif.imsamp = frame;
	imgif.scale = 1;
	imgif.w = 640;
	imgif.h = 480;
	region.x = 0;
	region.y = 0;
	region.w = 640;
	region.h = 480;

	yy_symvalid_clear();

	yy_img_scanner_line(&imgif, &region);

	res = yy_decoder_main(yy_data_buf);
	if (res == 1)
	{
		yy_decoder_get_sym(&type, &len, outsym);
		printf("%s\n", (char *)outsym);
	}

	res = yy_decoder_2d_main(&imgif, &region);
	if (res == 1)
	{
		yy_decoder_get_sym(&type, &len, outsym);
		printf("%s\n", (char *)outsym);
	}

	yy_symvalid_ttl_wait(0);

	return res;
}

#define CAM 0

void main()
{
#if CAM == 1
	cv::VideoCapture cam(1);
#endif
	
	yy_decset_init();

	while (1)
	{
		cv::Mat c, img, imdown;

#if CAM == 1
		cam >> c;
		cvtColor(c, img, CV_RGB2GRAY);
		static int fcnt = 0;
		char file[30];
		sprintf(file, "20180215/%d.bmp", fcnt++);
		cv::imwrite(file, img);
#else 
		static int fcnt = 0;
		char file[30];
		sprintf(file, "%d.bmp", fcnt++);
		c = cv::imread(file);
		if (c.dims == 0)
			break;
		cv::cvtColor(c, img, CV_RGB2GRAY);
#endif

		decoder_mainloop(img.data);
		cv::imshow("img", img);
		
		int key = cv::waitKey(500);
		if (key == 's')
			cv::imwrite("save.bmp", img);
		else if (key == 'q')
			break;
	}
}
