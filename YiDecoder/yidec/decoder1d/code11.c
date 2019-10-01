
#include "decoder1d/code11.h"

const unsigned char CODE11_CODE[11] =
{
	0x01, 0x11, 0x09, 0x18, 0x05, 0x14, 0x0c, 0x03, 0x12, 0x10, 0x04
};

const unsigned char CODE11_CODE_ASCII[11] =
{
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 45
};

unsigned char yy_code11(YyInterface *yy_interface)
{
	int i = 0, j = 0, k = 0;

	unsigned short stream[256];
	YyStartEnd sus[1];
	sus->head = 1;
	sus->len = yy_interface->sus->len;

	unsigned char yy_internal_buf[256];

	if (yy_decset.CODE11_ON == 0)
		return 0;

	if (((sus->len + 1) % 6) != 0)
		return 0;

	if (sus->len < 41 || sus->len > 251)
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

	j = yy_decode_e_pack(&stream[sus->head], sus->len, yy_internal_buf, 5, 7, 1);
	if ((yy_internal_buf[0] != yy_internal_buf[j - 5]) || (yy_internal_buf[1] != yy_internal_buf[j - 4]) || (yy_internal_buf[2] != yy_internal_buf[j - 3])
			|| (yy_internal_buf[3] != yy_internal_buf[j - 2]) || (yy_internal_buf[4] != yy_internal_buf[j - 1]))
		return 0;

	unsigned char f_reverse = 3;
	if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 1 && yy_internal_buf[2] > 1 && yy_internal_buf[3] > 1 && yy_internal_buf[4] == 1)
		f_reverse = 1;
	else if (yy_internal_buf[0] == 1 && yy_internal_buf[1] > 1 && yy_internal_buf[2] > 1 && yy_internal_buf[3] == 1 && yy_internal_buf[4] == 1)
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

	k = 0;
	for (i = 0; i < j; i += 5)
		yy_internal_buf[k++] = yy_decode_match(yy_internal_buf, i, 5, 1, 2);

	int kk = 0;
	for (i = 1; i < k - 1; i++)
	{
		for (j = 0; j < 11; j++)
		{
			if (yy_internal_buf[i] == CODE11_CODE[j])
			{
				yy_internal_buf[kk] = j;
				break;
			}
		}
		if (j >= 11) return 0;
		kk++;
	}
	k = kk;

	if (yy_decset.CODE11_VALIDATION)
	{
		if (!yy_check(yy_internal_buf, k, yy_decset.CODE11_VALIDATION, YY_CODE11))
			return 0;
	}

	switch (yy_decset.CODE11_VALIDATION_OUT)
	{
	case 0:
		if (yy_decset.CODE11_VALIDATION == 1)
			k -= 1;
		else if (yy_decset.CODE11_VALIDATION == 2)
			k -= 2;
		break;
	case 1:
		k -= 1;
		break;
	case 2:
		k -= 2;
		break;
	}

	for (i = 0; i < k; i++)
	{
		yy_internal_buf[i] += 48;
		if (yy_internal_buf[i] == 58)
			yy_internal_buf[i] = '-';
	}
	yy_internal_buf[k] = 0;

	if (k < yy_decset.CODE11_MIN_LEN)
		return 0;

	yy_interface->sym->type = YY_CODE11;
	sprintf((char *)yy_interface->sym->data, "%s", yy_internal_buf);

	return 1;
}

