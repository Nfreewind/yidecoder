
#include "decoder1d/code93.h"

const unsigned char CODE93_CODE[48] =
{
	0x8a, 0xa4, 0xa2, 0xa1, 0x94, 0x92, 0x91, 0xa8, 0x89, 0x85,
	0xd4, 0xd2, 0xd1, 0xca, 0xc9, 0xc5, 0xb4, 0xb2, 0xb1, 0x9a,
	0x8d, 0xac, 0xa6, 0xa3, 0x96, 0x8b, 0xda, 0xd9, 0xd6, 0xd3,
	0xcb, 0xcd, 0xb6, 0xb3, 0x9b, 0x9d, 0x97, 0xea, 0xe9, 0xe5,
	0xb7, 0xbb, 0xd7, 0x93, 0xed, 0xeb, 0x99, 0xaf
};

const unsigned char CODE93_CODE_ASCII[48] =
{
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
	65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
	75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
	85, 86, 87, 88, 89, 90, 45, 46, 32, 36,
	47, 43, 37, 200, 201, 202, 203, 42
};

unsigned char yy_code93(YyInterface *yy_interface)
{
	int i = 0, j = 0, k = 0;

	unsigned short stream[256];
	YyStartEnd sus[1];
	sus->head = 1;
	sus->len = yy_interface->sus->len;

	unsigned char yy_internal_buf[256];

	if (yy_decset.CODE93_ON == 0)
		return 0;

	if (((sus->len - 1) % 6) != 0)
		return 0;

	if (sus->len < 13 || sus->len > 253)
		return 0;

	memcpy(stream, &yy_interface->data[yy_interface->sus->head - 1], (sus->len + 2) * 2);

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

	j = yy_decode_e_pack(&stream[sus->head], sus->len - 7, yy_internal_buf, 6, 9, 0);
	j += yy_decode_e_pack(&stream[sus->head + sus->len - 7], 7, &yy_internal_buf[j], 7, 10, 0);

	unsigned char f_reverse = 3;
	if ((yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 1 && yy_internal_buf[3] == 1 && yy_internal_buf[4] == 4
			&& yy_internal_buf[5] == 1) && (yy_internal_buf[j - 7] == 1 && yy_internal_buf[j - 6] == 1 && yy_internal_buf[j - 5] == 1
											&& yy_internal_buf[j - 4] == 1 && yy_internal_buf[j - 3] == 4 && yy_internal_buf[j - 2] == 1 && yy_internal_buf[j - 1] == 1))
		f_reverse = 1;
	else if ((yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 4 && yy_internal_buf[3] == 1 && yy_internal_buf[4] == 1
			  && yy_internal_buf[5] == 1 && yy_internal_buf[6] == 1) && (yy_internal_buf[j - 6] == 1 && yy_internal_buf[j - 5] == 4
					  && yy_internal_buf[j - 4] == 1 && yy_internal_buf[j - 3] == 1 && yy_internal_buf[j - 2] == 1 && yy_internal_buf[j - 1] == 1))
		f_reverse = 2;

	if (f_reverse == 3)
		return 0;

	if (f_reverse == 2)
	{
		j = 0;
		j = yy_decode_e_pack(&stream[sus->head], 7, &yy_internal_buf[j], 7, 10, 0);
		j += yy_decode_e_pack(&stream[sus->head + 7], sus->len - 7, &yy_internal_buf[j], 6, 9, 0);
		for (i = 0; i < j / 2; i++)
		{
			unsigned char swit = yy_internal_buf[j - 1 - i];
			yy_internal_buf[j - 1 - i] = yy_internal_buf[i];
			yy_internal_buf[i] = swit;
		}
	}
	j--;

	k = 0;
	for (i = 0; i < j; i += 6)
		yy_internal_buf[k++] = yy_decode_match(yy_internal_buf, i, 0, 0, 0) | yy_decode_match(yy_internal_buf, i, 2, 0, 0) | yy_decode_match(yy_internal_buf, i, 4, 0, 0);

	for (i = 0; i < k; i++)
	{
		for (j = 0; j < 48; j++)
		{
			if (yy_internal_buf[i] == CODE93_CODE[j])
			{
				yy_internal_buf[i] = j;
				break;
			}
		}
		if (j >= 48)
			return 0;
	}

	if (yy_decset.CODE93_VALIDATION == 1)
	{
		if (!yy_check(yy_internal_buf, k, 0, YY_CODE93))
			return 0;
	}

	j = 0;
	for (i = 1; i < k - 3; i++)
	{
		switch (CODE93_CODE_ASCII[yy_internal_buf[i]])
		{
		case 200:
			i++;
			yy_internal_buf[j] = CODE39_CODE_ALL_ASCII00[CODE93_CODE_ASCII[yy_internal_buf[i]] - 'A'];
			break;
		case 201:
			i++;
			yy_internal_buf[j] = CODE39_CODE_ALL_ASCII01[CODE93_CODE_ASCII[yy_internal_buf[i]] - 'A'];
			break;
		case 202:
			i++;
			yy_internal_buf[j] = CODE39_CODE_ALL_ASCII03[CODE93_CODE_ASCII[yy_internal_buf[i]] - 'A'];
			break;
		case 203:
			i++;
			yy_internal_buf[j] = CODE39_CODE_ALL_ASCII02[CODE93_CODE_ASCII[yy_internal_buf[i]] - 'A'];
			break;
		default :
			yy_internal_buf[j] = CODE93_CODE_ASCII[yy_internal_buf[i]];
			break;
		}
		j++;
	}

	yy_internal_buf[j] = 0;

	if (j < yy_decset.CODE93_MIN_LEN)
		return 0;

	yy_interface->sym->type = YY_CODE93;
	sprintf((char *)yy_interface->sym->data, "%s", yy_internal_buf);

	return 1;
}
