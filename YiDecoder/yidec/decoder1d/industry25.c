
#include "decoder1d/industry25.h"

unsigned char yy_i25(YyInterface *yy_interface)
{
	int i = 0, j = 0, k = 0;

	unsigned short *stream = yy_interface->data;
	YyStartEnd *sus = yy_interface->sus;

	unsigned char yy_internal_buf[768];

	if (yy_decset.I25_ON == 0)
		return 0;

	if ((sus->len - 7) % 10 != 0 && (sus->len - 11) % 10 != 0)
		return 0;
	if (sus->len < 47 || sus->len > 360)
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

	j = yy_decode_e_pack(&stream[sus->head], 4, yy_internal_buf, 4, 4, 0);

	unsigned char f_reverse = 3;
	unsigned char Mode = 0;
	if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 1 && yy_internal_buf[3] == 1)
	{
		j = yy_decode_e_pack(&stream[sus->head + sus->len - 3], 3, yy_internal_buf, 3, 4, 0);
		if (yy_internal_buf[0] > 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 1)
		{
			f_reverse = 1;
			Mode = 1;
		}
	}

	if (f_reverse == 3)
	{
		j = yy_decode_e_pack(&stream[sus->head + sus->len - 4], 4, yy_internal_buf, 4, 4, 0);
		if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 1 && yy_internal_buf[3] == 1)
		{
			j = yy_decode_e_pack(&stream[sus->head], 3, yy_internal_buf, 3, 4, 0);
			if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] > 1)
			{
				f_reverse = 2;
				Mode = 1;
			}
		}
	}

	if (Mode == 0)
	{
		j = yy_decode_e_pack(&stream[sus->head], 6, yy_internal_buf, 6, 8, 0);
		if (yy_internal_buf[0] > 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] > 1 && yy_internal_buf[3] == 1 && yy_internal_buf[4] == 1 && yy_internal_buf[5] == 1)
		{
			j = yy_decode_e_pack(&stream[sus->head + sus->len - 5], 5, yy_internal_buf, 5, 7, 0);
			if (yy_internal_buf[0] > 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 1 && yy_internal_buf[3] == 1 && yy_internal_buf[4] > 1)
			{
				f_reverse = 1;
				Mode = 2;
			}
		}

		if (f_reverse == 3)
		{
			j = yy_decode_e_pack(&stream[sus->head + sus->len - 6], 6, yy_internal_buf, 6, 8, 0);
			if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 1 && yy_internal_buf[3] > 1 && yy_internal_buf[4] == 1 && yy_internal_buf[5] > 1)
			{

				j = yy_decode_e_pack(&stream[sus->head], 5, yy_internal_buf, 5, 7, 0);
				if (yy_internal_buf[0] > 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 1 && yy_internal_buf[3] == 1 && yy_internal_buf[4] > 1)
				{
					f_reverse = 2;
					Mode = 2;
				}
			}
		}
	}

	if (Mode == 0)
		return 0;

	if (Mode == 1)
	{
		if (f_reverse == 1)
		{
			j = yy_decode_e_pack(&stream[sus->head + 4], sus->len - 7, yy_internal_buf, 10, 14, 0);
		}
		else
		{
			j = yy_decode_e_pack(&stream[sus->head + 3], sus->len - 7, yy_internal_buf, 10, 14, 0);
			for (i = 0; i < j / 2; i++)
			{
				unsigned char swit = yy_internal_buf[j - 1 - i];
				yy_internal_buf[j - 1 - i] = yy_internal_buf[i];
				yy_internal_buf[i] = swit;
			}
		}
	}
	else
	{
		if (f_reverse == 1)
		{
			j = yy_decode_e_pack(&stream[sus->head + 6], sus->len - 11, yy_internal_buf, 10, 14, 0);
		}
		else
		{
			j = yy_decode_e_pack(&stream[sus->head + 5], sus->len - 11, yy_internal_buf, 10, 14, 0);
			for (i = 0; i < j / 2; i++)
			{
				unsigned char swit = yy_internal_buf[j - 1 - i];
				yy_internal_buf[j - 1 - i] = yy_internal_buf[i];
				yy_internal_buf[i] = swit;
			}
		}

	}

	k = 0;
	for (i = 0; i < j; i += 10)
	{
		if (yy_decode_match(yy_internal_buf, i + 1, 5, 2, 3))
			return 0;

		yy_internal_buf[k++] = yy_decode_match(yy_internal_buf, i, 5, 2, 3);
	}

	for (i = 0; i < k; i++)
	{
		for (j = 0; j < 10; j++)
		{
			if (yy_internal_buf[i] == IL25_CODE[j])
			{
				yy_internal_buf[i] = j;
				break;
			}
		}
		if (j == 10)
			return 0;
	}
	
	for (i = 0; i < k; i++)
		yy_internal_buf[i] += 48;
	yy_internal_buf[k] = 0;

	if (k < yy_decset.I25_MIN_LEN)
		return 0;

	yy_interface->sym->type = YY_I25;
	sprintf((char *)yy_interface->sym->data, "%s", yy_internal_buf);

	return 1;
}
