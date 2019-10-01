
#include "decoder1d/code128.h"

#define CODE128_FNC_1 210
#define CODE128_FNC_2 211
#define CODE128_FNC_3 212
#define CODE128_FNC_4 213

#define CODE128_START_A 103
#define CODE128_START_B 104
#define CODE128_START_C 105
#define CODE128_STOP 217

#define CODE128_SHIFT 200
#define CODE128_CODE_A 201
#define CODE128_CODE_B 202
#define CODE128_CODE_C 203

const unsigned char CODE128_A_CODE[107] =
{
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
	42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
	62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
	72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
	82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
	92, 93, 94, 95, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 212, 211, 200, 203,
	202, 213, 210, 214, 215, 216, 217,
};

const unsigned char CODE128_B_CODE[107] =
{
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
	42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
	62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
	72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
	82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
	92, 93, 94, 95, 96, 97, 98, 99, 100, 101,
	102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
	122, 123, 124, 125, 126, 127, 212, 211, 200, 203,
	213, 201, 210, 214, 215, 216, 217
};

const unsigned char CODE128_C_CODE[107] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
	30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
	40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
	50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
	60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
	70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
	90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
	202, 201, 210, 214, 215, 216, 217
};

const unsigned short CODE128_D_CODE[108] =
{
	3344, 4433, 4444, 3334, 3345, 4434, 3443, 3454, 4543, 4333, 4344, 5433, 2345, 3434, 3445, 2454, 3543, 3554, 4553, 4324, 4335, 3453, 4542,
	4334, 4234, 5323, 5334, 4343, 5432, 5443, 3333, 3355, 5533, 2245, 4423, 4445, 2354, 4532, 4554, 3244, 5422, 5444, 2334, 2356, 4534, 2443,
	2465, 4643, 4443, 3246, 5424, 3442, 3464, 3444, 4223, 4245, 6423, 4332, 4354, 6532, 4552, 4355, 7422, 2234, 2256, 3323, 3356, 5523, 5534,
	2343, 2365, 3432, 3465, 5632, 5643, 6533, 4322, 5442, 6522, 4752, 2236, 3325, 3336, 2563, 3652, 3663, 5233, 6322, 6333, 3335, 3553, 5333,
	2225, 2247, 4425, 2552, 2574, 5222, 5244, 2445, 2554, 4225, 5224, 3255, 3233, 3235, 5642, 3224
};

const unsigned char CODE128_V_CODE[108] =
{
	6, 6, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 8, 6, 6, 6, 6, 8, 6, 6, 6, 6,
	6, 6, 8, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 4, 4, 6, 6, 6
};

unsigned char code128_obj(unsigned short *stream)
{
	unsigned char i = 0;
	unsigned short s = 0, e1 = 0, e2 = 0, e3 = 0, e4 = 0, E1 = 0, E2 = 0, E3 = 0, E4 = 0, codeword = 0;

	s = stream[0] + stream[1] + stream[2] + stream[3] + stream[4] + stream[5];
	e1 = stream[0] + stream[1];
	e2 = stream[1] + stream[2];
	e3 = stream[2] + stream[3];
	e4 = stream[3] + stream[4];

	E1 = yy_decode_e(e1, s, 11);
	E2 = yy_decode_e(e2, s, 11);
	E3 = yy_decode_e(e3, s, 11);
	E4 = yy_decode_e(e4, s, 11);

	if (E1 < 2 || E2 < 2 || E3 < 2 || E4 < 2 || E1 > 7 || E2 > 7 || E3 > 7 || E4 > 7)
		return 0xff;


	codeword = E1 * 1000 + E2 * 100 + E3 * 10 + E4;
	for (i = 0; i < 108; i++)
	{
		if (codeword == CODE128_D_CODE[i])
			break;
	}

	if (i >= 108)
		i = 0xff;

	return i;
}

unsigned char yy_code128(YyInterface *yy_interface)
{
	int i = 0, j = 0, k = 0;

	unsigned short stream[384];
	YyStartEnd sus[1];
	sus->head = 1;
	sus->len = yy_interface->sus->len;

	unsigned char yy_internal_buf[64];
	unsigned char yy_internal_buf_part[64];

	if (((sus->len - 7) % 6) != 0)
		return 0;

	if (sus->len < 19 || sus->len > 379)
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

	unsigned short obj = code128_obj(&stream[sus->head]);
	if (obj == 0xff)
		return 0;

	unsigned char f_reverse = 3;
	if (obj == 103 || obj == 104 || obj == 105)
		f_reverse = 1;
	else if (obj == 107)
		f_reverse = 2;

	if (f_reverse == 3)
		return 0;

	if (f_reverse == 2)
	{
		YyInterface yy_interface_internal[1];
		yy_interface_internal->data = stream;
		yy_interface_internal->sus = sus;
		yy_sus_reverse(yy_interface_internal);
	}

	unsigned short stop[6] = {0};
	stop[0] = stream[sus->head + sus->len - 7];
	stop[1] = stream[sus->head + sus->len - 6];
	stop[2] = stream[sus->head + sus->len - 5];
	stop[3] = stream[sus->head + sus->len - 4];
	stop[4] = stream[sus->head + sus->len - 3];
	stop[5] = stream[sus->head + sus->len - 2];
	obj = code128_obj(stop);
	if (obj != 106)
		return 0;

	for (i = 0; i < sus->len - 7; i += 6)
	{
		yy_internal_buf_part[j] = code128_obj(&stream[sus->head + i]);
		if (yy_internal_buf_part[j] == 0xff)
			return 0;
		j++;
	}

	if (!yy_check(yy_internal_buf_part, j, 0, YY_CODE128))
		return 0;

	const unsigned char *code128_subset = CODE128_A_CODE;
	unsigned char code128_fnc1 = 0;
	unsigned char code128_fnc2 = 0;
	unsigned char code128_fnc3 = 0;
	unsigned char code128_fnc4 = 0;
	unsigned char codeword = 0;
	unsigned char f_shift = 0;
	k = 0;
	for (i = 0; i < j - 1; i++)
	{
		codeword = yy_internal_buf_part[i];
		if (codeword == CODE128_START_A)
		{
			code128_subset = CODE128_A_CODE;
			continue;
		}
		if (codeword == CODE128_START_B)
		{
			code128_subset = CODE128_B_CODE;
			continue;
		}
		if (codeword == CODE128_START_C)
		{
			code128_subset = CODE128_C_CODE;
			continue;
		}
		if (code128_subset[codeword] == CODE128_CODE_A)
		{
			code128_subset = CODE128_A_CODE;
			code128_fnc4 &= 0xf0;
			continue;
		}
		if (code128_subset[codeword] == CODE128_CODE_B)
		{
			code128_subset = CODE128_B_CODE;
			code128_fnc4 &= 0xf0;
			continue;
		}
		if (code128_subset[codeword] == CODE128_CODE_C)
		{
			code128_subset = CODE128_C_CODE;
			code128_fnc4 &= 0xf0;
			continue;
		}
		if (code128_subset[codeword] == CODE128_FNC_1)
		{
			code128_fnc1 = 1;
			code128_fnc4 &= 0xf0;
			continue;
		}
		if (code128_subset[codeword] == CODE128_FNC_2)
		{
			code128_fnc2 = 1;
			code128_fnc4 &= 0xf0;
			continue;
		}
		if (code128_subset[codeword] == CODE128_FNC_3)
		{
			code128_fnc3 = 1;
			code128_fnc4 &= 0xf0;
			continue;
		}
		if (code128_subset[codeword] == CODE128_FNC_4)
		{
			if (code128_fnc4 == 0x00)
			{
				code128_fnc4 = 0x01;
				continue;
			}
			if (code128_fnc4 == 0x01 && code128_subset[yy_internal_buf_part[i - 1]] == CODE128_FNC_4)
			{
				code128_fnc4 = 0x10;
				continue;
			}
			if (code128_fnc4 == 0x10)
			{
				code128_fnc4 = 0x11;
				continue;
			}
			if (code128_fnc4 == 0x11 && code128_subset[yy_internal_buf_part[i - 1]] == CODE128_FNC_4)
			{
				code128_fnc4 = 0x00;
				continue;
			}
		}

		if (code128_subset[codeword] == CODE128_SHIFT)
		{
			code128_fnc4 &= 0xf0;
			if (code128_subset == CODE128_A_CODE)
			{
				code128_subset = CODE128_B_CODE;
			}
			else
			{
				code128_subset = CODE128_A_CODE;
			}
			f_shift = 1;
			continue;
		}

		if (i == (j - 1))
		{
			yy_internal_buf[k++] = codeword;
			break;
		}
		if (code128_subset == CODE128_C_CODE)
		{
			yy_internal_buf[k++] = 48 + code128_subset[codeword] / 10;
			yy_internal_buf[k++] = 48 + code128_subset[codeword] % 10;
			code128_fnc4 &= 0xf0;
		}
		else
		{
			switch (code128_fnc4)
			{
			case 0x00:
				yy_internal_buf[k++] = code128_subset[codeword];
				break;
			case 0x01:
				yy_internal_buf[k++] = code128_subset[codeword] + 128;
				code128_fnc4 &= 0xf0;
				break;
			case 0x10:
				yy_internal_buf[k++] = code128_subset[codeword] + 128;
				break;
			case 0x11:
				yy_internal_buf[k++] = code128_subset[codeword];
				code128_fnc4 &= 0xf0;
				break;
			default:
				break;
			}
		}

		if (f_shift == 1)
		{
			if (code128_subset == CODE128_B_CODE)
				code128_subset = CODE128_A_CODE;
			else
				code128_subset = CODE128_B_CODE;
			f_shift = 0;
		}
	}

	if (k == 0)
		return 0;

	yy_internal_buf[k] = 0;

	if (code128_fnc3)
	{
		yy_interface->sym->type = YY_SET;
	}
	else
	{
		if (yy_decset.CODE128_ON == 0)
			return 0;
		if (k < yy_decset.CODE128_MIN_LEN)
			return 0;
		yy_interface->sym->type = YY_CODE128;

		switch (yy_decset.CODE128_EXPANSION)
		{
		case 0:
			break;
		case 1:
			for (i = 0; i < k - 1; i++)
			{
				if (yy_internal_buf[i] != '0')
					break;
			}
			for (j = 0; j < k; j++)
				yy_internal_buf[j] = yy_internal_buf[i + j];
			break;
		case 2:
			if (yy_internal_buf[0] == '0')
			{
				for (i = 0; i < k; i++)
					yy_internal_buf[i] = yy_internal_buf[i + 1];
			}
			break;
		case 3:
			for (i = 0; i < k; i++)
				yy_internal_buf[i] = yy_internal_buf[i + 1];
			break;
		default:
			break;
		}
	}

	sprintf((char *)yy_interface->sym->data, "%s", yy_internal_buf);

	return 1;
}



