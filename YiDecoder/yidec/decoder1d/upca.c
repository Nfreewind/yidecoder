
#include "decoder1d/upca.h"

unsigned char yy_upca(YyInterface *yy_interface)
{
	int i = 0, j = 0;

	unsigned short *stream = yy_interface->data;
	YyStartEnd *sus = yy_interface->sus;

	unsigned char yy_internal_buf[64];
	unsigned short stream_buf[59];
	unsigned short data_buf[59];
	unsigned char out_buf[59];

	if (yy_decset.UPCA_ON == 0)
		return 0;

	if (sus->len != 59)
		return 0;

	unsigned int mindata = stream[sus->head + 1];
	unsigned int maxdata = stream[sus->head + 1];
	for (i = sus->head + 2; i <= sus->head + sus->len - 2; i++)
	{
		if (stream[i] > maxdata)
			maxdata = stream[i];
		if (stream[i] < mindata)
			mindata = stream[i];
	}
	if (maxdata > mindata * YY_MAX_MIN_MUL)
		return 0;

	for (i = 0; i < 59; i++)
		stream_buf[i] = stream[sus->head + i];

	float add_sum = stream_buf[1] + stream_buf[2] + stream_buf[27]
					+ stream_buf[28] + stream_buf[29] + stream_buf[30]
					+ stream_buf[31] + stream_buf[56] + stream_buf[57] + stream_buf[58];

	float awb_bar_sum = add_sum / 2;
	float awb_spa_sum = add_sum / 2;
	float awb_bar = stream_buf[2] + stream_buf[28] + stream_buf[30] + stream_buf[56] + stream_buf[58];
	float awb_spa = stream_buf[1] + stream_buf[27] + stream_buf[29] + stream_buf[31] + stream_buf[57];

	awb_bar = awb_bar_sum / awb_bar;
	awb_spa = awb_spa_sum / awb_spa;

	for (i = 0; i < 59; i++)
	{
		if (i % 2 == 0)
			stream_buf[i] = ((float)stream_buf[i] * awb_bar + 0.5f);
		else
			stream_buf[i] = ((float)stream_buf[i] * awb_spa + 0.5f);
	}

	j = (unsigned char)yy_decode_e_pack(stream_buf, 59, yy_internal_buf, 59, 95, 0);

	if (yy_internal_buf[27] != 1 || yy_internal_buf[28] != 1 || yy_internal_buf[29] != 1 || yy_internal_buf[30] != 1 || yy_internal_buf[31] != 1)
		return 0;

	j = 0;
	for (i = 3; i <= 26; i++)
		data_buf[j++] = stream_buf[i];
	for (i = 32; i <= 55; i++)
		data_buf[j++] = stream_buf[i];

	j = (unsigned char)yy_decode_e_pack(data_buf, 48, yy_internal_buf, 4, 7, 0);
	for (i = 0; i < j; i++)
	{
		if (yy_internal_buf[i] >= 5 || yy_internal_buf[i] == 0)
			return 0;
	}

	for (i = 0; i < j; i += 4)
	{
		if ((yy_internal_buf[i] + yy_internal_buf[i + 1] + yy_internal_buf[i + 2] + yy_internal_buf[i + 3]) != 7)
			return 0;
	}

	j = 0;
	for (i = 0; i < 24; i += 4)
		yy_internal_buf[j++] = yy_decode_match(yy_internal_buf, i, 1, 1, 0) | yy_decode_match(yy_internal_buf, i, 3, 1, 0);
	for (i = 24; i < 48; i += 4)
		yy_internal_buf[j++] = yy_decode_match(yy_internal_buf, i, 0, 1, 0) | yy_decode_match(yy_internal_buf, i, 2, 1, 0);

	unsigned char f_reverse = 3;
	for (i = 0; i <= 9; i++)
	{
		if (yy_internal_buf[0] == EAN13_CODE[i])
			f_reverse = 1;
		if (yy_internal_buf[0] == EAN13_CODE[50 + i])
			f_reverse = 2;
		if (f_reverse != 3)
			break;
	}

	if (f_reverse == 3)
		return 0;

	unsigned char f_success = 0;
	unsigned char head = 0;
	if (f_reverse == 1)
	{
		for (i = 0; i < 12; i++)
		{
			f_success = 0;
			for (j = 0; j < 30; j++)
			{
				if (yy_internal_buf[i] == EAN13_CODE[j])
				{
					yy_internal_buf[i] = j % 10;
					f_success = 1;
					out_buf[i] = j / 10;
					if (i >= 6)
					{
						if (out_buf[i] != 2)
							return 0;
					}
					break;
				}
			}
			if (f_success == 0)
				return 0;
		}
	}
	else
	{
		for (i = 0; i < 12; i++)
		{
			f_success = 0;
			for (j = 30; j < 60; j++)
			{
				if (yy_internal_buf[i] == EAN13_CODE[j])
				{
					yy_internal_buf[i] = j % 10;
					f_success = 1;
					out_buf[i] = (j / 10) - 3;
					if (i < 6)
					{
						if (out_buf[i] != 2)
							return 0;
					}
					break;
				}
			}
			if (f_success == 0)
				return 0;
		}
	}

	if (f_reverse == 2)
	{
		for (i = 0; i < 6; i++)
		{
			unsigned char obj;
			obj = out_buf[i];
			out_buf[i] = out_buf[11 - i];
			out_buf[11 - i] = obj;

			obj = yy_internal_buf[i];
			yy_internal_buf[i] = yy_internal_buf[11 - i];
			yy_internal_buf[11 - i] = obj;
		}
	}

	f_success = 0;
	j = 0;
	for (i = 0; i < 60; i += 6)
	{
		if (out_buf[i % 6] == EAN13_HEAD[i] && out_buf[(i + 1) % 6] == EAN13_HEAD[i + 1] && out_buf[(i + 2) % 6] == EAN13_HEAD[i + 2]
				&& out_buf[(i + 3) % 6] == EAN13_HEAD[i + 3] && out_buf[(i + 4) % 6] == EAN13_HEAD[i + 4] && out_buf[(i + 5) % 6] == EAN13_HEAD[i + 5])
		{
			head = j;
			f_success = 1;
			break;
		}
		j++;
	}

	if (f_success == 0)
		return 0;

	if (head != 0)
		return 0;

	for (i = 0; i < 12; i++)
		yy_internal_buf[12 - i] = yy_internal_buf[11 - i];

	yy_internal_buf[0] = head;

	if (yy_decset.UPCA_VALIDATION == 1)
	{
		if (!yy_check(yy_internal_buf, 13, 0, YY_UPCA))
			return 0;
	}

	for (i = 0; i < 12; i++)
		yy_internal_buf[i] = yy_internal_buf[i + 1] + 48;

	yy_interface->sym->type = YY_UPCA;

	switch (yy_decset.UPCA_TRANS)
	{
	case 0:
		break;
	case 1:
		for (i = 12; i > 0; i--)
			yy_internal_buf[i] = yy_internal_buf[i - 1];
		yy_internal_buf[0] = '0';
		yy_interface->sym->type = YY_EAN13;
		break;
	default:
		break;
	}

	if (yy_interface->sym->type == YY_UPCA)
	{
		if (yy_decset.UPCA_VALIDATION_OUT == 0)
			yy_internal_buf[11] = 0;
		else
			yy_internal_buf[12] = 0;
	}

	if (yy_interface->sym->type == YY_EAN13)
	{
		if (yy_decset.EAN13_VALIDATION_OUT == 0)
			yy_internal_buf[12] = 0;
		else
			yy_internal_buf[13] = 0;
	}


	if (yy_interface->sym->type == YY_UPCA)
	{
		switch (yy_decset.UPCA_EXPANSION)
		{
		case 0:
			break;
		case 1:
			for (i = 0; i < 11; i++)
			{
				if (yy_internal_buf[i] != '0')
					break;
			}
			for (j = 0; j < 12; j++)
				yy_internal_buf[j] = yy_internal_buf[i + j];
			break;
		case 2:
			if (yy_internal_buf[0] == '0')
			{
				for (i = 0; i < 12; i++)
					yy_internal_buf[i] = yy_internal_buf[i + 1];
			}
			break;
		case 3:
			for (i = 0; i < 12; i++)
				yy_internal_buf[i] = yy_internal_buf[i + 1];
			break;
		default:
			break;
		}
	}

	sprintf((char *)yy_interface->sym->data, "%s", yy_internal_buf);

	return 1;
}

unsigned char yy_upca_lock(YyInterface *yy_interface)
{
	unsigned short *widstream = yy_interface->data;
	unsigned char yy_internal_buf[64];

	int j, k;
	int wid_sum3 = 0, wid_sum3_next;
	for (int i = 0; i < 3; i++)
		wid_sum3 += widstream[i];
	for (int i = 3; i < YY_SUS_MAX_NUM - 28; i++)
	{
		if (widstream[i] == 0)
			break;

		wid_sum3 += widstream[i] - widstream[i - 3];

		int pair;

		pair = widstream[i] + widstream[i - 1];
		if (yy_decode_e(pair, wid_sum3, 3) != 2)
			continue;

		pair = widstream[i - 1] + widstream[i - 2];
		if (yy_decode_e(pair, wid_sum3, 3) != 2)
			continue;

		wid_sum3_next = widstream[i + 25] + widstream[i + 26] + widstream[i + 27];

		if (wid_sum3_next / wid_sum3 >= 2 || wid_sum3 / wid_sum3_next >= 2)
			continue;

		pair = widstream[i + 25] + widstream[i + 26];
		if (yy_decode_e(pair, wid_sum3_next, 3) != 2)
			continue;

		pair = widstream[i + 26] + widstream[i + 27];
		if (yy_decode_e(pair, wid_sum3_next, 3) != 2)
			continue;

		wid_sum3_next = wid_sum3 + wid_sum3_next;
		for (j = i + 1; j < i + 25; j++)
		{
			if (widstream[j] > wid_sum3_next)
				break;
		}
		if (j != i + 25)
			continue;

		j = (unsigned char)yy_decode_e_pack(&widstream[i + 1], 24, yy_internal_buf, 4, 7, 0);
		for (k = 0; k < j; k++)
		{
			if (yy_internal_buf[k] >= 5 || yy_internal_buf[k] == 0)
				break;
		}
		if (k != j)
			continue;

		for (k = 0; k < j; k += 4)
		{
			if ((yy_internal_buf[k] + yy_internal_buf[k + 1] + yy_internal_buf[k + 2] + yy_internal_buf[k + 3]) != 7)
				break;
		}
		if (k != j)
			continue;

		return 1;
	}

	return 0;
}


