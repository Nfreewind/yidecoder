
#include "decoder1d/pharma.h"

unsigned char yy_pharma(YyInterface *yy_interface)
{
	unsigned short *stream = yy_interface->data;
	YyStartEnd *sus = yy_interface->sus;

	unsigned char yy_internal_buf[128];

	int i = 0, j = 0;
	unsigned short *buf_bar = (unsigned short *)&yy_internal_buf[32];
	unsigned short *buf_spa = (unsigned short *)&yy_internal_buf[64];
	unsigned int result = 0;

	if (yy_decset.PHARMA_ON == 0)
		return 0;

	if (sus->len < yy_decset.PHARMA_MIN_BAR)
		return 0;

	if (sus->len > 31 || sus->len % 2 == 0)
		return 0;

	if (stream[sus->head + sus->len] < stream[sus->head + sus->len - 2] * 4)
		return 0;

	for (i = 0; i <= sus->len / 2; i++)
	{
		buf_bar[i] = stream[sus->head + i * 2];
		buf_spa[i] = stream[sus->head + i * 2 + 1];
	}

	j = (unsigned char)yy_decode_e_pack(buf_spa, sus->len / 2, yy_internal_buf, sus->len / 2, sus->len / 2, 0);
	for (i = 0; i < j; i++)
	{
		if (yy_internal_buf[i] != 1)
			return 0;
	}

	if (buf_bar[0] >= buf_spa[0] * 2)
		return 0;
	if (buf_bar[0] <= buf_spa[0] / 3)
		return 0;
	if (buf_bar[j] >= buf_spa[j - 1] * 2)
		return 0;
	if (buf_bar[j] <= buf_spa[j - 1] / 3)
		return 0;
	for (i = 0; i < j - 1; i++)
	{
		if (buf_bar[i + 1] >= buf_spa[i] + buf_spa[i + 1])
			return 0;
		if (buf_bar[i + 1] <= (buf_spa[i] + buf_spa[i + 1]) / 6)
			return 0;
	}

	if (buf_bar[0] >= buf_spa[0])
		yy_internal_buf[i + 1] = 2;
	else
		yy_internal_buf[i + 1] = 1;
	if (buf_bar[j] >= buf_spa[j - 1])
		yy_internal_buf[i + 1] = 2;
	else
		yy_internal_buf[i + 1] = 1;
	for (i = 0; i < j - 1; i++)
	{
		if (buf_bar[i + 1] >= (buf_spa[i] + buf_spa[i + 1]) / 2)
			yy_internal_buf[i + 1] = 2;
		else
			yy_internal_buf[i + 1] = 1;
	}

	for (i = 0; i <= j; i++)
		result += (1 << i) * yy_internal_buf[i];

	yy_interface->sym->type = YY_PHARMA;
	sprintf((char *)yy_interface->sym->data, "%u", result);

	return 1;
}

