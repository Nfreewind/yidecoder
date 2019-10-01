
#include "basic.h"

YyDecoderSet yy_decset = { 0xFF };								//全局设置
YySymbol yy_symbol;									//输出结果

void yy_sus_init(YyStartEnd *s)
{
	unsigned short i;
	for (i = 0; i < YY_SUS_MAX_NUM; i++)
	{
		s[i].head = 0;
		s[i].len = 0;
	}
}

int yy_sus_expansion(YyStartEnd *yy_start_end, unsigned short j)
{
	int i, k = j;

	if (k >= YY_SUS_MAX_NUM)
		return k;

	if (yy_decset.EDGE_REFINE == 0)
		return k;

	for (i = 0; i < j; i++)
	{
		if (yy_decset.EDGE_REFINE & 0x02)
		{
			yy_start_end[k].head = yy_start_end[i].head;
			yy_start_end[k].len = yy_start_end[i].len - 1;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head + 1;
			yy_start_end[k].len = yy_start_end[i].len - 1;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head;
			yy_start_end[k].len = yy_start_end[i].len - 2;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head + 2;
			yy_start_end[k].len = yy_start_end[i].len - 2;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
		}

		if (yy_start_end[i].len == 44 || yy_start_end[i].len == 60)
		{
			yy_start_end[k].head = yy_start_end[i].head;
			yy_start_end[k].len = yy_start_end[i].len - 1;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head + 1;
			yy_start_end[k].len = yy_start_end[i].len - 1;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
		}
		else if (yy_start_end[i].len == 45 || yy_start_end[i].len == 61)
		{
			yy_start_end[k].head = yy_start_end[i].head;
			yy_start_end[k].len = yy_start_end[i].len - 2;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head + 2;
			yy_start_end[k].len = yy_start_end[i].len - 2;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head + 1;
			yy_start_end[k].len = yy_start_end[i].len - 2;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
		}
		else if (yy_start_end[i].len == 47 || yy_start_end[i].len == 63)
		{
			yy_start_end[k].head = yy_start_end[i].head + 4;
			yy_start_end[k].len = yy_start_end[i].len - 4;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head;
			yy_start_end[k].len = yy_start_end[i].len - 4;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head + 2;
			yy_start_end[k].len = yy_start_end[i].len - 4;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head + 3;
			yy_start_end[k].len = yy_start_end[i].len - 4;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head + 1;
			yy_start_end[k].len = yy_start_end[i].len - 4;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
		}
		else if (yy_start_end[i].len == 49 || yy_start_end[i].len == 65)
		{
			yy_start_end[k].head = yy_start_end[i].head + 4;
			yy_start_end[k].len = yy_start_end[i].len - 6;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head + 2;
			yy_start_end[k].len = yy_start_end[i].len - 6;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
			yy_start_end[k].head = yy_start_end[i].head + 3;
			yy_start_end[k].len = yy_start_end[i].len - 6;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
		}
		else if (yy_start_end[i].len == 51 || yy_start_end[i].len == 67)
		{
			yy_start_end[k].head = yy_start_end[i].head + 4;
			yy_start_end[k].len = yy_start_end[i].len - 8;
			if (++k >= YY_SUS_MAX_NUM)
				return k;
		}
	}
	return k;
}

int yy_sus_find(unsigned short *widstream, YyStartEnd *yy_start_end)
{
	int i = 0, j = 0, k = 0;
	int n = 0, m = 0;
	int buf[YY_SUS_MAX_NUM] = {0};

	for (i = 0; i < YY_MAX_DAT_NUM - 1 - YY_SUS_MIN_LEN; i++)
	{
		if (widstream[i] == 0)
			break;
		if (i < YY_SUS_MIN_LEN)
		{
			if (widstream[i] > widstream[i + 1] && widstream[i] > widstream[i + 2] && widstream[i] > widstream[i + 3] && widstream[i] > widstream[i + 4] && widstream[i] > widstream[i + 5] && widstream[i] > widstream[i + 6] && widstream[i] > widstream[i + 7] && widstream[i] > widstream[i + 8] && widstream[i] > widstream[i + 9] && widstream[i] > widstream[i + 10] && widstream[i] > widstream[i + 11] && widstream[i] > widstream[i + 12])
			{
				if (j < YY_SUS_MAX_NUM - 1)
					buf[j++] = i;
			}
		}
		else
		{
			if ((widstream[i] > widstream[i + 1] && widstream[i] > widstream[i + 2] && widstream[i] > widstream[i + 3] && widstream[i] > widstream[i + 4]
					&& widstream[i] > widstream[i + 5] && widstream[i] > widstream[i + 6] && widstream[i] > widstream[i + 7] && widstream[i] > widstream[i + 8]
					&& widstream[i] > widstream[i + 9] && widstream[i] > widstream[i + 10] && widstream[i] > widstream[i + 11] && widstream[i] > widstream[i + 12]) == 1
					|| (widstream[i] > widstream[i - 1] && widstream[i] > widstream[i - 2] && widstream[i] > widstream[i - 3] && widstream[i] > widstream[i - 4]
						&& widstream[i] > widstream[i - 5] && widstream[i] > widstream[i - 6] && widstream[i] > widstream[i - 7] && widstream[i] > widstream[i - 8]
						&& widstream[i] > widstream[i - 9] && widstream[i] > widstream[i - 10] && widstream[i] > widstream[i - 11] && widstream[i] > widstream[i - 12]) == 1)
			{
				if (j < YY_SUS_MAX_NUM - 1)
					buf[j++] = i;
			}
		}
	}

	if (widstream[YY_MAX_DAT_NUM - 1] > widstream[YY_MAX_DAT_NUM - 2] && widstream[YY_MAX_DAT_NUM - 1] > widstream[YY_MAX_DAT_NUM - 3]
			&& widstream[YY_MAX_DAT_NUM - 1] > widstream[YY_MAX_DAT_NUM - 4] && widstream[YY_MAX_DAT_NUM - 1] > widstream[YY_MAX_DAT_NUM - 5]
			&& widstream[YY_MAX_DAT_NUM - 1] > widstream[YY_MAX_DAT_NUM - 6] && widstream[YY_MAX_DAT_NUM - 1] > widstream[YY_MAX_DAT_NUM - 7]
			&& widstream[YY_MAX_DAT_NUM - 1] > widstream[YY_MAX_DAT_NUM - 8] && widstream[YY_MAX_DAT_NUM - 1] > widstream[YY_MAX_DAT_NUM - 9]
			&& widstream[YY_MAX_DAT_NUM - 1] > widstream[YY_MAX_DAT_NUM - 10] && widstream[YY_MAX_DAT_NUM - 1] > widstream[YY_MAX_DAT_NUM - 11]
			&& widstream[YY_MAX_DAT_NUM - 1] > widstream[YY_MAX_DAT_NUM - 12])
	{
		if (j < YY_SUS_MAX_NUM - 1)
			buf[j++] = YY_MAX_DAT_NUM - 1;
	}
	m = 0;
	for (i = 0; i < j - 1; i++)
	{
		k = 0;
		for (n = i + 1; n < j; n++)
		{
			if (k > widstream[buf[i]])
				break;
			if (k < widstream[buf[n]])
				k = widstream[buf[n]];
			if ((buf[n] - buf[i] + 1) < YY_SUS_MIN_LEN)
				continue;
			if (m < YY_SUS_MAX_NUM && k <= widstream[buf[n]])
			{
				yy_start_end[m].head = buf[i] + 1;
				yy_start_end[m].len = buf[n] - yy_start_end[m].head;
				m++;
			}
		}
	}
	return m;
}

void yy_sus_reverse(YyInterface *yy_interface)
{
	unsigned short *widstream = yy_interface->data;
	YyStartEnd *sus = yy_interface->sus;
	unsigned short j, t;
	for (j = 0; j < sus->len / 2; j++)
	{
		t = widstream[sus->head + sus->len - 1 - j];
		widstream[sus->head + sus->len - 1 - j] = widstream[sus->head + j];
		widstream[sus->head + j] = t;
	}
}

void yy_global_id(YySymbol *yy_symbol)
{
	char id;
	unsigned char len;

	if (yy_symbol->type > YY_MSI)
		return;

	id = ((char *)&yy_decset)[9 + yy_symbol->type];
	len = strlen((char *)yy_symbol->data);

	switch (yy_decset.ID_ON_LR)
	{
	case 0:
		break;
	case 1:
		yy_symbol->data[len + 1] = 0;
		for (; len > 0; len--)
			yy_symbol->data[len] = yy_symbol->data[len - 1];
		yy_symbol->data[0] = id;
		break;
	case 2:
		yy_symbol->data[len] = id;
		yy_symbol->data[len + 1] = 0;
		break;
	default:
		break;
	}
}

int yy_decode_e(int e, int s, int n)
{
	int E = ((e * n * 2 + 1) / s + 1) / 2;
	return E;
}

int yy_decode_e_pack(unsigned short *widstream, int widstreamlength, unsigned char *bufferout, int patternnum, int patternsize, int intervalstep)
{
	int i = 0, j = 0, k = 0;
	int sum = 0;
	for (i = 0; i < widstreamlength; i += patternnum)
	{
		sum = 0;
		for (j = 0; j < patternnum; j++)
			sum = sum + (unsigned int)widstream[i + j];

		for (j = 0; j < patternnum; j++)
			bufferout[k++] = yy_decode_e(widstream[i + j], sum, patternsize);

		i += intervalstep;
	}
	return k;
}

int yy_decode_match(unsigned char *code, int idx, int pack, int shift, unsigned char type)
{
	int i, res = 0;

	if (type)
	{
		for (i = 0; i < pack; i++)
		{
			if ((type == 1 && code[idx + shift * i] == 1) || (type == 2 && code[idx + shift * i] > 1) || (type == 3 && code[idx + shift * i] != 1))
				res |= 1 << (pack - i - 1);
		}
	}
	else
	{
		const unsigned char codetab[] = {0x00, 0x80, 0xc0, 0xe0, 0xf0};

		if (pack > 4)
			return 0;

		int sum = shift;
		for (i = 0; i < pack; i++)
			sum += code[idx + i];

		res = codetab[code[idx + pack]] >> sum;
	}

	return res;
}

unsigned char yy_check(unsigned char *Result, unsigned char Number, unsigned char ResultMode, unsigned char DecoderNum) //数据地址，数据个数，校验方式(只有一种校验的情况下该值无效)，解码器序列号
{
	int yy_c = 0, yy_cc = 0;
	int i = 0, j = 0, flay = 0;

	switch (DecoderNum)
	{
	case YY_EAN13:
	case YY_UPCA:
	case YY_UPCE:
		yy_c = Result[0] + Result[1] * 3 + Result[2] + Result[3] * 3 + Result[4] + Result[5] * 3 + Result[6] + Result[7] * 3 + Result[8] + Result[9] * 3 + Result[10] + Result[11] * 3;
		yy_c = (10 - (yy_c % 10)) % 10;
		if (yy_c == Result[12])
			return 1;

		return 0;

	case YY_EAN8:
		yy_c = Result[0] * 3 + Result[1] + Result[2] * 3 + Result[3] + Result[4] * 3 + Result[5] + Result[6] * 3;
		yy_c = (10 - (yy_c % 10)) % 10;
		if (yy_c == Result[7])
			return 1;

		return 0;

	case YY_CODE39:
		for (i = 0; i < Number - 1; i++)
		{
			yy_c = yy_c + Result[i];
		}
		yy_c = yy_c % 43;
		if (yy_c == Result[Number - 1]) return 1;
		return 0;


	case YY_CODE128:
		yy_c = Result[0];
		for (i = 1; i < Number - 1; i++)
			yy_c += Result[i] * i;
		yy_c = yy_c % 103;
		if (yy_c == Result[Number - 1])
			return 1;

		return 0;

	case YY_CODE93:
		j = 0;
		for (i = Number - 4; i > 0; i--)
		{
			j++;
			j = j % 20;
			if (j == 0) j = 20;
			yy_c += Result[i] * j;
		}
		yy_c = yy_c % 47;
		yy_cc = yy_c;
		j = 1;
		for (i = Number - 4; i > 0; i--)
		{
			j++;
			j = j % 15;
			if (j == 0) j = 15;
			yy_cc = yy_cc + Result[i] * j;

		}

		yy_cc = yy_cc % 47;

		if (yy_c != Result[Number - 3] && yy_cc != Result[Number - 2])
			return 0;

		return 1;

	case YY_CODEBAR:
		for (i = 0; i < Number - 2; i++)  yy_c += Result[i];
		yy_c += Result[i - 1];
		yy_c = yy_c % 16;
		yy_c = (16 - yy_c) % 16;

		flay = 1;
		if (Number % 2 == 0)
			flay = 2;
		j = 0;
		for (i = 1; i < Number - 2; i++)
		{
			if (flay == 2)
			{
				yy_cc += (Result[i] * 2);
				if (Result[i] >= 5)
					j++;
				flay = 1;
			}
			else
			{
				yy_cc += Result[i];
				if (Result[i] >= 10)
					j++;
				flay = 2;
			}
		}
		yy_cc = yy_cc + j;
		yy_cc = yy_cc % 10;
		yy_cc = (10 - yy_cc) % 10;


		if (Result[Number  - 2] != yy_c  && Result[Number  - 2] != yy_cc)
			return 0;
		return 1;

	case YY_IL25:
		if (ResultMode == 0) //uss mod10
		{
			yy_c = 0;
			yy_cc = 0;
			for (i = 0; i < Number; i += 2)
				yy_c += Result[i];
			for (i = 1; i < Number - 2; i += 2)
				yy_cc += Result[i];

			yy_c = yy_c * 3 ;
			yy_c = 10 - (yy_c + yy_cc) % 10;

			if (yy_c == Result[Number - 1])
				return 1;
		}

		if (ResultMode == 1) //opcc mode10
		{
			yy_c = 0;
			yy_cc = 0;
			for (i = 0; i < Number; i += 2)
			{
				yy_c += (Result[i] * 2) / 10;
				yy_c += (Result[i] * 2) % 10;
			}
			for (i = 1; i < Number - 2; i += 2)
				yy_cc += Result[i];
			yy_cc = 10 - (yy_c + yy_cc) % 10;
			if (yy_cc == 10)
				yy_cc = 0;
			if (Result[Number - 1] != yy_cc)
				return 1;
		}

		return 0;

	case YY_I25:

		return 0;

	case YY_M25:
		if (ResultMode == 1)
		{
			yy_c = 0;
			yy_cc = 0;
			if (Number % 2 == 0)
			{
				for (i = 0; i < Number; i += 2)
					yy_c += Result[i];
				for (i = 1; i < Number - 2; i += 2)
					yy_cc += Result[i];
			}
			else
			{
				for (i = 0; i < Number - 2; i += 2)
					yy_cc += Result[i];
				for (i = 1; i < Number; i += 2)
					yy_c += Result[i];
			}
			yy_c *= 3 ;
			yy_c = 10 - (yy_c + yy_cc) % 10;
			if (yy_c == 10)
				yy_c = 0;
			if (yy_c == Result[Number  - 1])
				return 1;
			return 0;

		}

		if (ResultMode == 2)
		{
			yy_c = 0;
			yy_cc = 0;
			for (i = 0; i < Number; i += 2)
			{
				yy_c += (Result[i] * 2) / 10;
				yy_c += (Result[i] * 2) % 10;
			}
			for (i = 1; i < Number - 2; i += 2)
				yy_cc += Result[i];
			yy_c = 10 - (yy_c + yy_cc) % 10;
			if (yy_c == 10)
				yy_c = 0;

			if (yy_c == Result[Number - 1])
				return 1;

		}

		return 0;


	case YY_CODE11:

		if (ResultMode == 1) //mod11
		{
			j = 0;
			yy_c = 0;
			for (i = Number - 2; i >= 0; i--)
			{
				j++;
				j = j % 10;
				if (j == 0)
					j = 10;
				yy_c += Result[i] * j;
			}
			yy_c = yy_c % 11;
			if (yy_c == Result[Number - 1]) return 1;
		}

		if (ResultMode == 2) //mode11/mode11
		{
			for (i = Number - 3; i >= 0; i--)
			{
				j++;
				j = j % 10;
				if (j == 0)
					j = 10;
				yy_c += Result[i] * j;
			}
			yy_c = yy_c % 11;

			yy_cc = 0;
			j = 0;
			for (i = Number - 2; i >= 0; i--)
			{
				j++;
				j = j % 9;
				if (j == 0)
					j = 9;
				yy_cc += Result[i] * j;
			}
			yy_cc = yy_cc % 11;


			if (yy_c == Result[Number - 2] && yy_cc == Result[Number - 1])
				return 1;

		}

		return 0;
		
	case YY_MSI:

		if (ResultMode == 0) //mod10
		{


		}

		if (ResultMode == 1) //mode10/mode10
		{


		}

		if (ResultMode == 1) //mode11/mode10
		{


		}

		return 0;

	default:
		return 0;
	}
	return 0;
}
