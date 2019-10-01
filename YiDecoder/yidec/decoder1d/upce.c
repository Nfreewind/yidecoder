
#include "decoder1d/upce.h"

const unsigned char UPCE_HEAD[120] =
{
	1, 1, 1, 0, 0, 0,
	1, 1, 0, 1, 0, 0,
	1, 1, 0, 0, 1, 0,
	1, 1, 0, 0, 0, 1,
	1, 0, 1, 1, 0, 0,
	1, 0, 0, 1, 1, 0,
	1, 0, 0, 0, 1, 1,
	1, 0, 1, 0, 1, 0,
	1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1,

	0, 0, 0, 1, 1, 1,
	0, 0, 1, 0, 1, 1,
	0, 0, 1, 1, 0, 1,
	0, 0, 1, 1, 1, 0,
	0, 1, 0, 0, 1, 1,
	0, 1, 1, 0, 0, 1,
	0, 1, 1, 1, 0, 0,
	0, 1, 0, 1, 0, 1,
	0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0
};

unsigned char yy_upce(YyInterface *yy_interface)
{
	int i = 0, j = 0, j_0 = 0, j_1 = 0;

	unsigned short *stream = yy_interface->data;
	YyStartEnd *sus = yy_interface->sus;

	unsigned char yy_internal_buf[128];
	unsigned char yy_internal_buf_ean13[33] = {0};
	unsigned short data_buf[33] = {0};
	unsigned short data_buf_1[33] = {0};
	unsigned char out_buf[33] = {0};

	if (yy_decset.UPCE_ON == 0)
		return 0;

	if (sus->len != 33)
		return 0;

	if (yy_decset.HEAD_CORRECT)
		stream[sus->head] = stream[sus->head + sus->len - 1];

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

	if (!yy_decset.HEAD_CORRECT && stream[sus->head - 1] < mindata * 8)
		return 0;
	if (stream[sus->head + sus->len] < mindata * 8)
		return 0;

	j = (unsigned char)yy_decode_e_pack(&stream[sus->head], 33, yy_internal_buf, 33, 51, 0);

	unsigned char f_reverse = 3;
	if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 1 && yy_internal_buf[27] == 1 && yy_internal_buf[28] == 1
			&& yy_internal_buf[29] == 1 && yy_internal_buf[30] == 1 && yy_internal_buf[31] == 1 && yy_internal_buf[32] == 1)
		f_reverse = 1;
	else if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 1 && yy_internal_buf[3] == 1 && yy_internal_buf[4] == 1
			 && yy_internal_buf[5] == 1 && yy_internal_buf[30] == 1 && yy_internal_buf[31] == 1 && yy_internal_buf[32] == 1)
		f_reverse = 2;

	if (f_reverse == 3)
		return 0;

	j = 0;
	j_0 = 0;
	j_1 = 0;
	for (i = 3; i <= 26; i++)
		data_buf[j_0++] = stream[sus->head + i];

	for (i = 6; i <= 29; i++)
		data_buf_1[j_1++] = stream[sus->head + i];

	for (i = 0; i < 12; i++)
	{
		unsigned short swit = data_buf_1[i];
		data_buf_1[i] = data_buf_1[23 - i];
		data_buf_1[23 - i] = swit;
	}

	j_0 = yy_decode_e_pack(data_buf, 24, yy_internal_buf, 4, 7, 0);
	j_1 = yy_decode_e_pack(data_buf_1, 24, yy_internal_buf_ean13, 4, 7, 0);

	for (i = 0; i < j_0; i++)
	{
		if (yy_internal_buf[i] >= 5 || yy_internal_buf[i] == 0)
			return 0;
	}

	for (i = 0; i < j_0; i += 4)
	{
		if ((yy_internal_buf[i] + yy_internal_buf[i + 1] + yy_internal_buf[i + 2] + yy_internal_buf[i + 3]) != 7)
		{
			for (j = 0; j < j_0; j++)
				yy_internal_buf[j] = 0;
			break;
		}
	}


	for (i = 0; i < j_1; i++)
	{
		if (yy_internal_buf_ean13[i] >= 5 || yy_internal_buf_ean13[i] == 0)
			return 0;
	}

	for (i = 0; i < j_1; i += 4)
	{
		if ((yy_internal_buf_ean13[i] + yy_internal_buf_ean13[i + 1] + yy_internal_buf_ean13[i + 2] + yy_internal_buf_ean13[i + 3]) != 7)
		{
			for (j = 0; j < j_1; j++)
				yy_internal_buf_ean13[j] = 0;
			break;
		}
	}

	j = 0;
	for (i = 0; i < 24; i += 4)
	{
		yy_internal_buf[j] = yy_decode_match(yy_internal_buf, i, 1, 1, 0) | yy_decode_match(yy_internal_buf, i, 3, 1, 0);
		yy_internal_buf_ean13[j++] = yy_decode_match(yy_internal_buf_ean13, i, 1, 1, 0) | yy_decode_match(yy_internal_buf, i, 3, 1, 0);
	}

	unsigned char f_success = 0;
	for (i = 0; i < 6; i++)
	{
		f_reverse = 1;
		f_success = 0;
		for (j = 0; j < 30; j++)
		{
			if (yy_internal_buf[i] == EAN13_CODE[j])
			{
				yy_internal_buf[i] = j % 10;
				f_success = 1;
				out_buf[i] = j / 10;
				break;
			}
		}
		if (f_success == 0)
			break;
	}


	if (f_success == 0)
	{
		for (i = 0; i < 6; i++)
		{
			f_reverse = 2;
			f_success = 0;
			for (j = 0; j < 30; j++)
			{
				if (yy_internal_buf_ean13[i] == EAN13_CODE[j])
				{
					yy_internal_buf_ean13[i] = j % 10;
					f_success = 1;
					out_buf[i] = j / 10;
					break;
				}
			}
			if (f_success == 0)
				return 0;
		}
	}

	unsigned char head = 0;
	unsigned char veri = 0;
	unsigned char m = 0, s1 = 0, s2 = 0;
	f_success = 0;
	j = 0;
	for (i = 0; i < 120; i += 6)
	{
		if (out_buf[0] == UPCE_HEAD[i] && out_buf[1] == UPCE_HEAD[i + 1] && out_buf[2] == UPCE_HEAD[i + 2] && out_buf[3] == UPCE_HEAD[i + 3] && out_buf[4] == UPCE_HEAD[i + 4] && out_buf[5] == UPCE_HEAD[i + 5])
		{
			f_success = 1;
			if (i >= 60)
				head = 1;
			if (head == 0)
				veri = j % 10;
			else
				veri = (j - 10) % 10;
			break;
		}
		j++;
	}

	if (f_success == 0)
		return 0;

	if (f_reverse == 2)
	{
		for (i = 0; i < 6; i++)
			yy_internal_buf[i] = yy_internal_buf_ean13[i];
	}

	for (i = 0; i < 6; i++)
		yy_internal_buf[6 - i] = yy_internal_buf[5 - i];
	yy_internal_buf[0] = head;
	yy_internal_buf[7] = veri;

	switch (yy_internal_buf[6])
	{
	case 0:
		yy_internal_buf_ean13[0] = 0 ;
		yy_internal_buf_ean13[1] = yy_internal_buf[0] ;
		yy_internal_buf_ean13[2] = yy_internal_buf[1] ;
		yy_internal_buf_ean13[3] = yy_internal_buf[2] ;
		yy_internal_buf_ean13[4] = 0 ;
		yy_internal_buf_ean13[5] = 0 ;
		yy_internal_buf_ean13[6] = 0 ;
		yy_internal_buf_ean13[7] = 0 ;
		yy_internal_buf_ean13[8] = 0 ;
		yy_internal_buf_ean13[9] = yy_internal_buf[3] ;
		yy_internal_buf_ean13[10] = yy_internal_buf[4] ;
		yy_internal_buf_ean13[11] = yy_internal_buf[5] ;
		yy_internal_buf_ean13[12] = yy_internal_buf[7] ;
		break;

	case 1:
		yy_internal_buf_ean13[0] = 0 ;
		yy_internal_buf_ean13[1] = yy_internal_buf[0] ;
		yy_internal_buf_ean13[2] = yy_internal_buf[1] ;
		yy_internal_buf_ean13[3] = yy_internal_buf[2] ;
		yy_internal_buf_ean13[4] = 1 ;
		yy_internal_buf_ean13[5] = 0 ;
		yy_internal_buf_ean13[6] = 0 ;
		yy_internal_buf_ean13[7] = 0 ;
		yy_internal_buf_ean13[8] = 0 ;
		yy_internal_buf_ean13[9] = yy_internal_buf[3] ;
		yy_internal_buf_ean13[10] = yy_internal_buf[4] ;
		yy_internal_buf_ean13[11] = yy_internal_buf[5] ;
		yy_internal_buf_ean13[12] = yy_internal_buf[7] ;
		break;

	case 2:
		yy_internal_buf_ean13[0] = 0 ;
		yy_internal_buf_ean13[1] = yy_internal_buf[0] ;
		yy_internal_buf_ean13[2] = yy_internal_buf[1] ;
		yy_internal_buf_ean13[3] = yy_internal_buf[2] ;
		yy_internal_buf_ean13[4] = 2 ;
		yy_internal_buf_ean13[5] = 0 ;
		yy_internal_buf_ean13[6] = 0 ;
		yy_internal_buf_ean13[7] = 0 ;
		yy_internal_buf_ean13[8] = 0 ;
		yy_internal_buf_ean13[9] = yy_internal_buf[3] ;
		yy_internal_buf_ean13[10] = yy_internal_buf[4] ;
		yy_internal_buf_ean13[11] = yy_internal_buf[5] ;
		yy_internal_buf_ean13[12] = yy_internal_buf[7] ;
		break;

	case 3:
		yy_internal_buf_ean13[0] = 0 ;
		yy_internal_buf_ean13[1] = yy_internal_buf[0] ;
		yy_internal_buf_ean13[2] = yy_internal_buf[1] ;
		yy_internal_buf_ean13[3] = yy_internal_buf[2] ;
		yy_internal_buf_ean13[4] = yy_internal_buf[3] ;
		yy_internal_buf_ean13[5] = 0 ;
		yy_internal_buf_ean13[6] = 0 ;
		yy_internal_buf_ean13[7] = 0 ;
		yy_internal_buf_ean13[8] = 0 ;
		yy_internal_buf_ean13[9] = 0 ;
		yy_internal_buf_ean13[10] = yy_internal_buf[4] ;
		yy_internal_buf_ean13[11] = yy_internal_buf[5] ;
		yy_internal_buf_ean13[12] = yy_internal_buf[7] ;
		break;

	case 4:
		yy_internal_buf_ean13[0] = 0 ;
		yy_internal_buf_ean13[1] = yy_internal_buf[0] ;
		yy_internal_buf_ean13[2] = yy_internal_buf[1] ;
		yy_internal_buf_ean13[3] = yy_internal_buf[2] ;
		yy_internal_buf_ean13[4] = yy_internal_buf[3] ;
		yy_internal_buf_ean13[5] = yy_internal_buf[4] ;
		yy_internal_buf_ean13[6] = 0 ;
		yy_internal_buf_ean13[7] = 0 ;
		yy_internal_buf_ean13[8] = 0 ;
		yy_internal_buf_ean13[9] = 0 ;
		yy_internal_buf_ean13[10] = 0 ;
		yy_internal_buf_ean13[11] = yy_internal_buf[5] ;
		yy_internal_buf_ean13[12] = yy_internal_buf[7] ;
		break;

	case 5:
		yy_internal_buf_ean13[0] = 0 ;
		yy_internal_buf_ean13[1] = yy_internal_buf[0] ;
		yy_internal_buf_ean13[2] = yy_internal_buf[1] ;
		yy_internal_buf_ean13[3] = yy_internal_buf[2] ;
		yy_internal_buf_ean13[4] = yy_internal_buf[3] ;
		yy_internal_buf_ean13[5] = yy_internal_buf[4] ;
		yy_internal_buf_ean13[6] = yy_internal_buf[5] ;
		yy_internal_buf_ean13[7] = 0 ;
		yy_internal_buf_ean13[8] = 0 ;
		yy_internal_buf_ean13[9] = 0 ;
		yy_internal_buf_ean13[10] = 0 ;
		yy_internal_buf_ean13[11] = 5 ;
		yy_internal_buf_ean13[12] = yy_internal_buf[7] ;
		break;

	case 6:
		yy_internal_buf_ean13[0] = 0 ;
		yy_internal_buf_ean13[1] = yy_internal_buf[0] ;
		yy_internal_buf_ean13[2] = yy_internal_buf[1] ;
		yy_internal_buf_ean13[3] = yy_internal_buf[2] ;
		yy_internal_buf_ean13[4] = yy_internal_buf[3] ;
		yy_internal_buf_ean13[5] = yy_internal_buf[4] ;
		yy_internal_buf_ean13[6] = yy_internal_buf[5] ;
		yy_internal_buf_ean13[7] = 0 ;
		yy_internal_buf_ean13[8] = 0 ;
		yy_internal_buf_ean13[9] = 0 ;
		yy_internal_buf_ean13[10] = 0 ;
		yy_internal_buf_ean13[11] = 6 ;
		yy_internal_buf_ean13[12] = yy_internal_buf[7] ;
		break;

	case 7:
		yy_internal_buf_ean13[0] = 0 ;
		yy_internal_buf_ean13[1] = yy_internal_buf[0] ;
		yy_internal_buf_ean13[2] = yy_internal_buf[1] ;
		yy_internal_buf_ean13[3] = yy_internal_buf[2] ;
		yy_internal_buf_ean13[4] = yy_internal_buf[3] ;
		yy_internal_buf_ean13[5] = yy_internal_buf[4] ;
		yy_internal_buf_ean13[6] = yy_internal_buf[5] ;
		yy_internal_buf_ean13[7] = 0 ;
		yy_internal_buf_ean13[8] = 0 ;
		yy_internal_buf_ean13[9] = 0 ;
		yy_internal_buf_ean13[10] = 0 ;
		yy_internal_buf_ean13[11] = 7 ;
		yy_internal_buf_ean13[12] = yy_internal_buf[7] ;
		break;

	case 8:
		yy_internal_buf_ean13[0] = 0 ;
		yy_internal_buf_ean13[1] = yy_internal_buf[0] ;
		yy_internal_buf_ean13[2] = yy_internal_buf[1] ;
		yy_internal_buf_ean13[3] = yy_internal_buf[2] ;
		yy_internal_buf_ean13[4] = yy_internal_buf[3] ;
		yy_internal_buf_ean13[5] = yy_internal_buf[4] ;
		yy_internal_buf_ean13[6] = yy_internal_buf[5] ;
		yy_internal_buf_ean13[7] = 0 ;
		yy_internal_buf_ean13[8] = 0 ;
		yy_internal_buf_ean13[9] = 0 ;
		yy_internal_buf_ean13[10] = 0 ;
		yy_internal_buf_ean13[11] = 8 ;
		yy_internal_buf_ean13[12] = yy_internal_buf[7] ;
		break;

	case 9:
		yy_internal_buf_ean13[0] = 0 ;
		yy_internal_buf_ean13[1] = yy_internal_buf[0] ;
		yy_internal_buf_ean13[2] = yy_internal_buf[1] ;
		yy_internal_buf_ean13[3] = yy_internal_buf[2] ;
		yy_internal_buf_ean13[4] = yy_internal_buf[3] ;
		yy_internal_buf_ean13[5] = yy_internal_buf[4] ;
		yy_internal_buf_ean13[6] = yy_internal_buf[5] ;
		yy_internal_buf_ean13[7] = 0 ;
		yy_internal_buf_ean13[8] = 0 ;
		yy_internal_buf_ean13[9] = 0 ;
		yy_internal_buf_ean13[10] = 0 ;
		yy_internal_buf_ean13[11] = 9 ;
		yy_internal_buf_ean13[12] = yy_internal_buf[7] ;
		break;
	
	default:
		return 0;
	}

	if (yy_decset.UPCE_VALIDATION == 1)
	{
		if (!yy_check(yy_internal_buf_ean13, 13, 0, YY_UPCE))
			return 0;
	}

	yy_internal_buf_ean13[11] = yy_internal_buf_ean13[12];
	
	for (i = 0; i < 8; i++)
		yy_internal_buf[i] += '0';
	
	for (i = 0; i < 12; i++)
		yy_internal_buf_ean13[i] += '0';

	yy_internal_buf_ean13[12] = 0;

	yy_interface->sym->type = YY_UPCE;

	switch (yy_decset.UPCE_TRANS)
	{
	case 0:
		break;
	case 1:
		for (i = 12; i > 0; i--)
			yy_internal_buf[i] = yy_internal_buf_ean13[i - 1];
		yy_internal_buf[0] = '0';
		yy_interface->sym->type = YY_EAN13;
		break;
	case 2:
		strcpy((char *)yy_internal_buf, (char *)yy_internal_buf_ean13);
		yy_interface->sym->type = YY_UPCA;
		break;
	default:
		break;
	}

	if (yy_interface->sym->type == YY_UPCE)
	{
		if (yy_decset.UPCE_VALIDATION_OUT == 0)
			yy_internal_buf[7] = 0;
		else
			yy_internal_buf[8] = 0;
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


	if (yy_interface->sym->type == YY_UPCE)
	{
		switch (yy_decset.UPCE_EXPANSION)
		{
		case 0:
			break;
		case 1:
			for (i = 0; i < 7; i++)
			{
				if (yy_internal_buf[i] != '0')
					break;
			}
			for (j = 0; j < 8; j++)
				yy_internal_buf[j] = yy_internal_buf[i + j];
			break;
		case 2:
			if (yy_internal_buf[0] == '0')
			{
				for (i = 0; i < 8; i++)
					yy_internal_buf[i] = yy_internal_buf[i + 1];
			}
			break;
		case 3:
			for (i = 0; i < 8; i++)
				yy_internal_buf[i] = yy_internal_buf[i + 1];
			break;
		default:
			break;
		}
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



