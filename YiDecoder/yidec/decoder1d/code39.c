
#include "decoder1d/code39.h"

const unsigned short CODE39_CODE_1[44] =
{
	0x34, 0x121, 0x61, 0x160, 0x31, 0x130, 0x70, 0x25, 0x124, 0x64,
	0x109, 0x49, 0x148, 0x19, 0x118, 0x58, 0x0d, 0x10c, 0x4c, 0x1c,
	0x103, 0x43, 0x142, 0x13, 0x112, 0x52, 0x07, 0x106, 0x46, 0x16,
	0x181, 0xc1, 0x1c0, 0x91, 0x190, 0xd0, 0x85, 0x184, 0xc4, 0xa8,
	0xa2, 0x8a, 0x2a, 0x94
};

const unsigned short CODE39_CODE_2[44] =
{
	0x58, 0x109, 0x10c, 0x0d, 0x118, 0x19, 0x1c, 0x148, 0x49, 0x4c,
	0x121, 0x124, 0x25, 0x130, 0x31, 0x34, 0x160, 0x61, 0x64, 0x70,
	0x181, 0x184, 0x85, 0x190, 0x91, 0x94, 0x1c0, 0xc1, 0xc4, 0xd0,
	0x103, 0x106, 0x07, 0x112, 0x13, 0x16, 0x142, 0x43, 0x46, 0x2a,
	0x8a, 0xa2, 0xa8, 0x52
};

const unsigned char CODE39_CODE_ASCII[44] =
{
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
	65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
	75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
	85, 86, 87, 88, 89, 90, 45, 46, 32, 36,
	47, 43, 37, 42
};

const unsigned char CODE39_CODE_ALL_ASCII00[26] =
{
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26
};

const unsigned char CODE39_CODE_ALL_ASCII01[26] =
{
	27, 28, 29, 30, 31, 59, 60, 61, 62, 63, 91, 92, 93, 94, 95, 123, 124, 125, 126, 127, 0, 64, 96, 127, 127, 127
};

const unsigned char CODE39_CODE_ALL_ASCII02[26] =
{
	97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122
};

const unsigned char CODE39_CODE_ALL_ASCII03[26] =
{
	33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58
};

unsigned char code39_to_code32(unsigned char c)
{
	unsigned char v = 0;
	if (c >= '0' && c <= '9')
		v = c - '0';
	else if (c >= 'B' && c <= 'D')
		v = c - 'B' + 10;
	else if (c >= 'F' && c <= 'H')
		v = c - 'F' + 13;
	else if (c >= 'J' && c <= 'N')
		v = c - 'J' + 16;
	else if (c >= 'P' && c <= 'Z')
		v = c - 'P' + 21;
	return v;
}

unsigned char yy_code39(YyInterface *yy_interface)
{
	int i = 0, j = 0, k = 0;

	unsigned short stream[384];
	YyStartEnd sus[1];
	sus->head = 1;
	sus->len = yy_interface->sus->len;

	unsigned char yy_internal_buf[384];
	unsigned short *yy_internal_buf_16 = stream;
	unsigned char *yy_internal_buf_ALLASCII = (unsigned char *)stream;

	if (yy_decset.CODE39_ON == 0)
		return 0;

	if (((sus->len + 1) % 10) != 0)
		return 0;
	if (sus->len < 49 || sus->len > 379)
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

	if (!yy_decset.HEAD_CORRECT && stream[sus->head - 1] < mindata * 8)
		return 0;
	if (stream[sus->head + sus->len] < mindata * 8)
		return 0;

	j = yy_decode_e_pack(&stream[sus->head], sus->len, yy_internal_buf, 9, 11, 1);

	k = 0;
	for (i = 0; i < j; i += 9)
		yy_internal_buf_16[k++] = yy_decode_match(yy_internal_buf, i, 9, 1, 2);

	if (yy_internal_buf_16[0] != yy_internal_buf_16[k - 1])
		return 0;

	unsigned char f_reverse = 3;
	if (yy_internal_buf_16[0] == 0x94)
		f_reverse = 1;
	else if (yy_internal_buf_16[0] == 0x52)
		f_reverse = 2;

	if (f_reverse == 3)
		return 0;

	if (f_reverse == 1)
	{
		for (i = 0; i < k; i++)
		{
			for (j = 0; j < 44; j++)
			{
				if (yy_internal_buf_16[i] == CODE39_CODE_1[j])
				{
					yy_internal_buf[i] = j;
					break;
				}
			}
			if (j == 44)
				return 0;
		}
	}
	else
	{
		j = k / 2;
		for (i = 0; i < j; i++)
		{
			unsigned short swit = yy_internal_buf_16[k - 1 - i];
			yy_internal_buf_16[k - 1 - i] = yy_internal_buf_16[i];
			yy_internal_buf_16[i] = swit;
		}
		for (i = 0; i < k; i++)
		{
			for (j = 0; j < 44; j++)
			{
				if (yy_internal_buf_16[i] == CODE39_CODE_2[j])
				{
					yy_internal_buf[i] = j;
					break;
				}
			}
			if (j == 44)
				return 0;
		}
	}

	for (i = 1; i < k - 1; i++)
	{
		if (yy_internal_buf[i] == 43)
			return 0;
	}

	if (yy_decset.CODE39_VALIDATION == 1)
	{
		if (!yy_check(yy_internal_buf, k, 0, YY_CODE39))
			return 0;
	}

	if (yy_decset.CODE39_VALIDATION_OUT == 0)
	{
		yy_internal_buf[k - 2] = yy_internal_buf[k - 1];
		k--;
	}

	for (i = 0; i < k; i++)
		yy_internal_buf[i] = CODE39_CODE_ASCII[yy_internal_buf[i]];
	yy_internal_buf[k] = 0;

	if (yy_decset.CODE39_START_END == 0)
	{
		k -= 2;
		for (i = 0; i < k; i++)
			yy_internal_buf[i] = yy_internal_buf[i + 1];
		yy_internal_buf[k] = 0;
	}

	unsigned char f_subset = 0;
	if (yy_decset.CODE39_DATAFORMAT == 1)
	{
		j = 0;
		for (i = 0; i < k; i++)
		{
			switch (f_subset)
			{
			case 0:
				yy_internal_buf_ALLASCII[j] = yy_internal_buf[i];
				break;
			case 1:
				if (yy_internal_buf[i] >= 65 && yy_internal_buf[i] <= 90 && i != 1)
					yy_internal_buf_ALLASCII[--j] = CODE39_CODE_ALL_ASCII00[yy_internal_buf[i] - 'A'];
				else
					yy_internal_buf_ALLASCII[j] = yy_internal_buf[i];
				break;
			case 2:
				if (yy_internal_buf[i] >= 65 && yy_internal_buf[i] <= 90 && i != 1)
					yy_internal_buf_ALLASCII[--j] = CODE39_CODE_ALL_ASCII01[yy_internal_buf[i] - 'A'];
				else
					yy_internal_buf_ALLASCII[j] = yy_internal_buf[i];
				break;
			case 3:
				if (yy_internal_buf[i] >= 65 && yy_internal_buf[i] <= 90 && i != 1)
					yy_internal_buf_ALLASCII[--j] = CODE39_CODE_ALL_ASCII02[yy_internal_buf[i] - 'A'];
				else
					yy_internal_buf_ALLASCII[j] = yy_internal_buf[i];
				break;
			case 4:
				if (yy_internal_buf[i] >= 65 && yy_internal_buf[i] <= 90 && i != 1)
					yy_internal_buf_ALLASCII[--j] = CODE39_CODE_ALL_ASCII03[yy_internal_buf[i] - 'A'];
				else
					yy_internal_buf_ALLASCII[j] = yy_internal_buf[i];
				break;
			}

			f_subset = 0;
			if (yy_internal_buf_ALLASCII[j] == 36)
				f_subset = 1;
			if (yy_internal_buf_ALLASCII[j] == 37)
				f_subset = 2;
			if (yy_internal_buf_ALLASCII[j] == 43)
				f_subset = 3;
			if (yy_internal_buf_ALLASCII[j] == 47)
				f_subset = 4;
			j++;
		}
		k = j;
		yy_internal_buf_ALLASCII[k] = 0;
		strcpy((char *)yy_internal_buf, (char *)yy_internal_buf_ALLASCII);
	}
	else if (yy_decset.CODE39_DATAFORMAT == 2)
	{
		if (k == 6)
		{
			unsigned int code12val = 0;
			code12val += code39_to_code32(yy_internal_buf[5]);
			code12val += 32 * code39_to_code32(yy_internal_buf[4]);
			code12val += 32 * 32 * code39_to_code32(yy_internal_buf[3]);
			code12val += 32 * 32 * 32 * code39_to_code32(yy_internal_buf[2]);
			code12val += 32 * 32 * 32 * 32 * code39_to_code32(yy_internal_buf[1]);
			code12val += 32 * 32 * 32 * 32 * 32 * code39_to_code32(yy_internal_buf[0]);
			sprintf((char *)yy_internal_buf, "%09u", code12val);
			k = 9;
			yy_internal_buf[k] = 0;
		}
	}

	if (k < yy_decset.CODE39_MIN_LEN)
		return 0;

	yy_interface->sym->type = YY_CODE39;
	sprintf((char *)yy_interface->sym->data, "%s", yy_internal_buf);

	return 1;
}
