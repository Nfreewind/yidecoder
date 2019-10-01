
#include "decoder1d/addition.h"

unsigned char yy_addition(YyInterface *yy_interface)
{
	int i = 0, j = 0, k = 0;

	unsigned short *stream = yy_interface->data;
	YyStartEnd *sus = yy_interface->sus;

	unsigned char yy_internal_buf[64];

	unsigned char f_type = 0;

	if (sus->len == 13)
		f_type = 1;
	if (sus->len == 31)
		f_type = 2;

	if (f_type == 0)
		return 0;
	if (f_type == 1 && (yy_decset.ADD_ON & 0x01) == 0)
		return 0;
	if (f_type == 2 && (yy_decset.ADD_ON & 0x02) == 0)
		return 0;

	unsigned int mindata = stream[sus->head];
	unsigned int maxdata = stream[sus->head];
	for (i = sus->head + 1; i <= sus->head + sus->len - 1; i++)
	{
		if (stream[i] > maxdata)
			maxdata = stream[i];
		if (stream[i] < mindata)
			mindata = stream[i];
	}
	if (maxdata > mindata * YY_MAX_MIN_MUL)
		return 0;

	if (f_type == 1)
		j = (unsigned char)yy_decode_e_pack(&stream[sus->head], sus->len, yy_internal_buf, 13, 20, 0);
	else
		j = (unsigned char)yy_decode_e_pack(&stream[sus->head], sus->len, yy_internal_buf, 31, 47, 0);

	for (i = 0; i < j; i++)
	{
		if (yy_internal_buf[i] >= 5 || yy_internal_buf[i] == 0)
			return 0;
	}

	unsigned char f_reverse = 3;
	if (f_type == 1)
	{
		if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 2 && (yy_internal_buf[3] + yy_internal_buf[4] + yy_internal_buf[5]
				+ yy_internal_buf[6]) == 7 && yy_internal_buf[7] == 1 && yy_internal_buf[8] == 1)
			f_reverse = 1;
		else
			f_reverse = 2;
	}
	else
	{
		if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 2 && (yy_internal_buf[3] + yy_internal_buf[4] + yy_internal_buf[5]
				+ yy_internal_buf[6]) == 7 && yy_internal_buf[7] == 1 && yy_internal_buf[8] == 1 && yy_internal_buf[13] == 1 && yy_internal_buf[14] == 1
				&& yy_internal_buf[19] == 1 && yy_internal_buf[20] == 1 && yy_internal_buf[25] == 1 && yy_internal_buf[26] == 1)
			f_reverse = 1;
		else
			f_reverse = 2;
	}
	if (f_reverse == 2)
	{
		for (i = 0; i < j / 2; i++)
		{
			unsigned char obj = yy_internal_buf[i];
			yy_internal_buf[i] = yy_internal_buf[j - 1 - i];
			yy_internal_buf[j - 1 - i] = obj;
		}
	}

	if (f_type == 1)
	{
		if (yy_internal_buf[0] != 1 || yy_internal_buf[1] != 1 || yy_internal_buf[2] != 2 || yy_internal_buf[7] != 1 || yy_internal_buf[8] != 1)
			return 0;
	}
	else
	{
		if (yy_internal_buf[0] != 1 || yy_internal_buf[1] != 1 || yy_internal_buf[2] != 2 || yy_internal_buf[7] != 1 || yy_internal_buf[8] != 1 || yy_internal_buf[13] != 1
				|| yy_internal_buf[14] != 1  || yy_internal_buf[19] != 1 || yy_internal_buf[20] != 1  || yy_internal_buf[25] != 1 || yy_internal_buf[26] != 1)
			return 0;
	}

	if (f_type == 1)
	{
		yy_internal_buf[0] = yy_internal_buf[3];
		yy_internal_buf[1] = yy_internal_buf[4];
		yy_internal_buf[2] = yy_internal_buf[5];
		yy_internal_buf[3] = yy_internal_buf[6];

		yy_internal_buf[4] = yy_internal_buf[9];
		yy_internal_buf[5] = yy_internal_buf[10];
		yy_internal_buf[6] = yy_internal_buf[11];
		yy_internal_buf[7] = yy_internal_buf[12];
		j = 8;
	}
	else
	{
		yy_internal_buf[0] = yy_internal_buf[3];
		yy_internal_buf[1] = yy_internal_buf[4];
		yy_internal_buf[2] = yy_internal_buf[5];
		yy_internal_buf[3] = yy_internal_buf[6];

		yy_internal_buf[4] = yy_internal_buf[9];
		yy_internal_buf[5] = yy_internal_buf[10];
		yy_internal_buf[6] = yy_internal_buf[11];
		yy_internal_buf[7] = yy_internal_buf[12];

		yy_internal_buf[8] = yy_internal_buf[15];
		yy_internal_buf[9] = yy_internal_buf[16];
		yy_internal_buf[10] = yy_internal_buf[17];
		yy_internal_buf[11] = yy_internal_buf[18];

		yy_internal_buf[12] = yy_internal_buf[21];
		yy_internal_buf[13] = yy_internal_buf[22];
		yy_internal_buf[14] = yy_internal_buf[23];
		yy_internal_buf[15] = yy_internal_buf[24];

		yy_internal_buf[16] = yy_internal_buf[27];
		yy_internal_buf[17] = yy_internal_buf[28];
		yy_internal_buf[18] = yy_internal_buf[29];
		yy_internal_buf[19] = yy_internal_buf[30];
		j = 20;
	}

	k = 0;
	for (i = 0; i < j; i += 4)
		yy_internal_buf[k++] = yy_decode_match(yy_internal_buf, i, 1, 1, 0) | yy_decode_match(yy_internal_buf, i, 3, 1, 0);

	for (i = 0; i < k; i++)
	{
		for (j = 0; j < 30; j++)
		{
			if (yy_internal_buf[i] == EAN13_CODE[j])
			{
				yy_internal_buf[i] = (j % 10) + 48;
				break;
			}
		}
		if (j == 30)
			return 0;
	}
	yy_internal_buf[k] = 0;

	yy_interface->sym->type = YY_ADD;
	sprintf((char *)yy_interface->sym->data, "%s", yy_internal_buf);

	return 1;
}
