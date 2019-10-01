
#include "decoder1d/codebar.h"

const unsigned char CODEBAR_CODE_00[20] =
{
	0x03, 0x06, 0x09, 0x60, 0x12, 0x42, 0x21, 0x24, 0x30, 0x48,
	0x0c, 0x18, 0x45, 0x51, 0x54, 0x15, 0x1a, 0x29, 0x0b, 0x0e
};

const unsigned char CODEBAR_CODE_01[20] =
{
	0x60, 0x30, 0x48, 0x03, 0x24, 0x21, 0x42, 0x12, 0x06, 0x09,
	0x18, 0x0c, 0x51, 0x45, 0x15, 0x54, 0x2c, 0x4a, 0x68, 0x38
};

const unsigned char CODEBAR_CODE_02[20] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'-', '$', ':', '/', '.', '+', 'A', 'B', 'C', 'D'
};

unsigned char yy_codebar(YyInterface *yy_interface)
{
	int i = 0, j = 0, k = 0;

	unsigned short stream[512];
	YyStartEnd sus[1];
	sus->head = 1;
	sus->len = yy_interface->sus->len;

	unsigned char yy_internal_buf[256];

	if (yy_decset.CODEBAR_ON == 0)
		return 0;

	if (((sus->len - 7) % 8) != 0)
		return 0;
	if (sus->len < 55 || sus->len > 495)
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

	if (!yy_decset.HEAD_CORRECT && stream[sus->head - 1] < mindata * 8)
		return 0;
	if (stream[sus->head + sus->len] < mindata * 8)
		return 0;

	j = yy_decode_e_pack(&stream[sus->head], sus->len, yy_internal_buf, 7, 10, 1);

	k = 0;
	for (i = 0; i < j; i++)
	{
		if (yy_internal_buf[i] > 3)
			return 0;
	}

	for (i = 0; i < j; i += 7)
		yy_internal_buf[k++] = yy_decode_match(yy_internal_buf, i, 7, 1, 2);

	unsigned char f_reverse = 3;
	if ((yy_internal_buf[0] == 0x1a || yy_internal_buf[0] == 0x29 || yy_internal_buf[0] == 0x0b || yy_internal_buf[0] == 0x0e)
			&& (yy_internal_buf[k - 1] == 0x1a || yy_internal_buf[k - 1] == 0x29 || yy_internal_buf[k - 1] == 0x0b || yy_internal_buf[k - 1] == 0x0e))
		f_reverse = 1;
	else if ((yy_internal_buf[0] == 0x2c || yy_internal_buf[0] == 0x4a || yy_internal_buf[0] == 0x68 || yy_internal_buf[0] == 0x38) &&
			 (yy_internal_buf[k - 1] == 0x2c || yy_internal_buf[k - 1] == 0x4a || yy_internal_buf[k - 1] == 0x68 || yy_internal_buf[k - 1] == 0x38))
		f_reverse = 2;

	if (f_reverse == 3)
		return 0;

	if (f_reverse == 1)
	{
		for (i = 0; i < k; i++)
		{
			for (j = 0; j < 20; j++)
			{
				if (yy_internal_buf[i] == CODEBAR_CODE_00[j])
				{
					yy_internal_buf[i] = j;
					break;
				}
			}
			if (j == 20)
				return 0;
		}
	}
	else
	{
		j = k / 2;
		for (i = 0; i < j; i++)
		{
			unsigned char swit = yy_internal_buf[k - 1 - i];
			yy_internal_buf[k - 1 - i] = yy_internal_buf[i];
			yy_internal_buf[i] = swit;
		}

		for (i = 0; i < k; i++)
		{
			for (j = 0; j < 20; j++)
			{
				if (yy_internal_buf[i] == CODEBAR_CODE_01[j])
				{
					yy_internal_buf[i] = j;
					break;
				}
			}
			if (j == 20)
				return 0;
		}
	}


	if (yy_decset.CODEBAR_VALIDATION == 1)
	{
		if (!yy_check(yy_internal_buf, k, 0, YY_CODEBAR))
			return 0;
	}
	if (yy_decset.CODEBAR_VALIDATION_OUT == 0)
	{
		yy_internal_buf[k - 2] = yy_internal_buf[k - 1];
		k--;
	}

	for (i = 0; i < k; i++)
		yy_internal_buf[i] = CODEBAR_CODE_02[yy_internal_buf[i]];
	yy_internal_buf[k] = 0;

	for (i = 1; i < k - 1; i++)
	{
		if (yy_internal_buf[i] == 'A')
			return 0;
		if (yy_internal_buf[i] == 'B')
			return 0;
		if (yy_internal_buf[i] == 'C')
			return 0;
		if (yy_internal_buf[i] == 'D')
			return 0;
	}

	switch (yy_decset.CODEBAR_START_END)
	{
	case 0:
		k -= 2;
		for (i = 0; i < k; i++)
			yy_internal_buf[i] = yy_internal_buf[i + 1];
		yy_internal_buf[k] = 0;
		break;
	case 1:
		break;
	case 2:
		yy_internal_buf[0] += 'a' - 'A';
		yy_internal_buf[k - 1] += 'a' - 'A';
		break;
	case 3:
		if (yy_internal_buf[k - 1] == 'A')
			yy_internal_buf[k - 1] = 'T';
		else if (yy_internal_buf[k - 1] == 'B')
			yy_internal_buf[k - 1] = 'N';
		else if (yy_internal_buf[k - 1] == 'C')
			yy_internal_buf[k - 1] = '*';
		else if (yy_internal_buf[k - 1] == 'D')
			yy_internal_buf[k - 1] = 'E';
		break;
	case 4:
		yy_internal_buf[0] += 'a' - 'A';
		if (yy_internal_buf[k - 1] == 'A')
			yy_internal_buf[k - 1] = 't';
		else if (yy_internal_buf[k - 1] == 'B')
			yy_internal_buf[k - 1] = 'n';
		else if (yy_internal_buf[k - 1] == 'C')
			yy_internal_buf[k - 1] = '*';
		else if (yy_internal_buf[k - 1] == 'D')
			yy_internal_buf[k - 1] = 'e';
		break;
	}

	if (k < yy_decset.CODEBAR_MIN_LEN)
		return 0;

	yy_interface->sym->type = YY_CODEBAR;
	sprintf((char *)yy_interface->sym->data, "%s", yy_internal_buf);

	return 1;
}



