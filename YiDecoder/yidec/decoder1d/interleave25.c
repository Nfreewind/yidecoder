
#include "decoder1d/interleave25.h"

const unsigned char IL25_CODE[10] =
{
	0x06, 0x11, 0x09, 0x18, 0x05, 0x14, 0x0c, 0x03, 0x12, 0x0a
};

unsigned char yy_il25_to_bradesco(unsigned char *yy_internal_buf)
{
	int i, j, r;
	int remainder = 0;
	unsigned char mul = 4;

	int k = strlen((char *)yy_internal_buf);
	if (k != 44)
		return 0;
	if (yy_internal_buf[0] != '8')
	{
		//Boleto_bancario
		remainder = 0;
		mul = 4;
		for (i = 0; i < 44; i++)
		{
			if (i == 4)
				continue;
			remainder += (yy_internal_buf[i] - '0') * mul;
			mul--;
			if (mul == 1)
				mul = 9;
		}
		remainder = 11 - remainder % 11;
		if (remainder == 0 || remainder == 1 || remainder == 10 || remainder == 11)
			remainder = 1;
		if (yy_internal_buf[4] != remainder + '0')
			return 0;

		for (i = 0; i < 47; i++)
		{
			if (i == 0)
				j = 0;
			else if (i == 4)
				j = 19;
			else if (i == 10)
				j = 24;
			else if (i == 21)
				j = 34;
			else if (i == 32)
				j = 4;
			else
				j++;
			yy_internal_buf[44 + i] = yy_internal_buf[j];
		}

		remainder = 0;
		mul = 2;
		for (i = 0; i < 9; i++)
		{
			r = (yy_internal_buf[44 + i] - '0') * mul;
			if (r > 9)
				r -= 9;
			remainder += r;
			mul = 3 - mul;
		}
		remainder = 10 - remainder % 10;
		if (remainder == 10)
			remainder = 0;
		yy_internal_buf[53] = remainder + '0';

		remainder = 0;
		for (i = 0; i < 10; i++)
		{
			r = (yy_internal_buf[54 + i] - '0') * mul;
			if (r > 9)
				r -= 9;
			remainder += r;
			mul = 3 - mul;
		}
		remainder = 10 - remainder % 10;
		if (remainder == 10)
			remainder = 0;
		yy_internal_buf[64] = remainder + '0';

		remainder = 0;
		for (i = 0; i < 10; i++)
		{
			r = (yy_internal_buf[65 + i] - '0') * mul;
			if (r > 9)
				r -= 9;
			remainder += r;
			mul = 3 - mul;
		}
		remainder = 10 - remainder % 10;
		if (remainder == 10)
			remainder = 0;
		yy_internal_buf[75] = remainder + '0';

		for (i = 0; i < 47; i++)
			yy_internal_buf[i] = yy_internal_buf[44 + i];
		yy_internal_buf[47] = 0;
	}
	else
	{
		if (yy_internal_buf[2] == '6' || yy_internal_buf[2] == '7')
		{
			//Concessionaria_67
			remainder = 0;
			mul = 2;
			for (i = 0; i < 44; i++)
			{
				if (i == 3)
					continue;
				r = (yy_internal_buf[i] - '0') * mul;
				if (r > 9)
					r -= 9;
				remainder += r;
				mul = 3 - mul;
			}
			remainder = 10 - remainder % 10;
			if (remainder == 10)
				remainder = 0;
			if (yy_internal_buf[3] != remainder + '0')
				return 0;

			for (i = 0; i < 48; i++)
			{
				if (i == 0)
					j = 0;
				else if (i == 12)
					j = 11;
				else if (i == 24)
					j = 22;
				else if (i == 36)
					j = 33;
				else
					j++;
				yy_internal_buf[44 + i] = yy_internal_buf[j];
			}

			for (j = 0; j < 4; j++)
			{
				remainder = 0;
				mul = 2;
				for (i = 0; i < 11; i++)
				{
					r = (yy_internal_buf[44 + 12 * j + i] - '0') * mul;
					if (r > 9)
						r -= 9;
					remainder += r;
					mul = 3 - mul;
				}
				remainder = 10 - remainder % 10;
				if (remainder == 10)
					remainder = 0;
				yy_internal_buf[44 + 12 * j + i] = remainder + '0';
			}

			for (i = 0; i < 48; i++)
				yy_internal_buf[i] = yy_internal_buf[44 + i];
			yy_internal_buf[48] = 0;
		}
		else if (yy_internal_buf[2] == '8' || yy_internal_buf[2] == '9')
		{
			//Concessionaria_89
			remainder = 0;
			mul = 4;
			for (i = 0; i < 44; i++)
			{
				if (i == 3)
					continue;
				remainder += (yy_internal_buf[i] - '0') * mul;
				mul--;
				if (mul == 1)
					mul = 9;
			}
			remainder = 11 - remainder % 11;
			if (remainder == 0 || remainder == 1)
				remainder = 1;
			else if (remainder == 10 || remainder == 11)
				remainder = 0;
			if (yy_internal_buf[3] != remainder + '0')
				return 0;

			for (i = 0; i < 48; i++)
			{
				if (i == 0)
					j = 0;
				else if (i == 12)
					j = 11;
				else if (i == 24)
					j = 22;
				else if (i == 36)
					j = 33;
				else
					j++;
				yy_internal_buf[44 + i] = yy_internal_buf[j];
			}

			for (j = 0; j < 4; j++)
			{
				remainder = 0;
				mul = 4;
				for (i = 0; i < 11; i++)
				{
					remainder += (yy_internal_buf[44 + 12 * j + i] - '0') * mul;
					mul--;
					if (mul == 1)
						mul = 9;
				}
				remainder = 11 - remainder % 11;
				if (remainder == 0 || remainder == 1)
					remainder = 1;
				else if (remainder == 10 || remainder == 11)
					remainder = 0;
				yy_internal_buf[44 + 12 * j + i] = remainder + '0';
			}

			for (i = 0; i < 48; i++)
				yy_internal_buf[i] = yy_internal_buf[44 + i];
			yy_internal_buf[48] = 0;
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

unsigned char yy_il25(YyInterface *yy_interface)
{
	int i = 0, j = 0, k = 0;

	unsigned short *stream = yy_interface->data;
	YyStartEnd *sus = yy_interface->sus;

	unsigned char yy_internal_buf[768];

	if (yy_decset.IL25_ON == 0)
		return 0;

	if ((sus->len - 7) % 10 != 0)
		return 0;
	if (sus->len < 37 || sus->len > 254)
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

	k = ((sus->len - 7) / 10) * 14 + 8;
	j = yy_decode_e_pack(&stream[sus->head], sus->len, yy_internal_buf, sus->len, k, 0);

	unsigned char f_reverse = 3;
	if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 1 && yy_internal_buf[3] == 1)
	{
		if (yy_internal_buf[sus->len - 3] > 1 && yy_internal_buf[sus->len - 2] == 1 && yy_internal_buf[sus->len - 1] == 1)
			f_reverse = 1;
	}

	if (f_reverse == 3)
	{
		if (yy_internal_buf[sus->len - 1] == 1 && yy_internal_buf[sus->len - 2] == 1 && yy_internal_buf[sus->len - 3] == 1 && yy_internal_buf[sus->len - 4] == 1)
		{
			if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] > 1)
				f_reverse = 2;
		}
	}
	if (f_reverse == 3)
		return 0;

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

	k = 0;
	for (i = 0; i < j; i += 10)
	{
		yy_internal_buf[k++] = yy_decode_match(yy_internal_buf, i, 5, 2, 3);
		yy_internal_buf[k++] = yy_decode_match(yy_internal_buf, i + 1, 5, 2, 3);
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

	if (yy_decset.IL25_VALIDATION)
	{
		if (!yy_check(yy_internal_buf, k, yy_decset.IL25_VALIDATION, YY_IL25))
			return 0;
	}

	if (yy_decset.IL25_VALIDATION_OUT == 0)
		k--;

	for (i = 0; i < k; i++)
		yy_internal_buf[i] += 48;
	yy_internal_buf[k] = 0;

	if (k < yy_decset.IL25_MIN_LEN)
		return 0;

	if (yy_decset.IL25_VALIDATION == 3)
	{
		if (yy_il25_to_bradesco(yy_internal_buf) == 0)
			return 0;
	}

	yy_interface->sym->type = YY_IL25;
	sprintf((char *)yy_interface->sym->data, "%s", yy_internal_buf);

	return 1;
}
