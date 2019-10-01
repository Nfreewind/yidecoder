
#include "decoder1d/msi.h"

unsigned char yy_msi(YyInterface *yy_interface)
{
	int i = 0, j = 0, k = 0, s = 0;

	unsigned short *stream = yy_interface->data;
	YyStartEnd *sus = yy_interface->sus;

	unsigned char yy_internal_buf[384];
	unsigned char yy_internal_buf2[384];
	unsigned short *databuf = (unsigned short *)yy_internal_buf2;

	if (yy_decset.MSI_ON == 0)
		return 0;

	if (((sus->len - 5) % 8) != 0)
		return 0;
	if (sus->len < 37 || sus->len > 365)
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

	databuf[0] = stream[sus->head];
	databuf[1] = stream[sus->head + 1];
	databuf[2] = stream[sus->head + sus->len - 3];
	databuf[3] = stream[sus->head + sus->len - 2];
	databuf[4] = stream[sus->head + sus->len - 1];

	j = yy_decode_e_pack(databuf, 5, yy_internal_buf, 5, 7, 0);

	for (i = 0; i < j; i++)
	{
		if (yy_internal_buf[i] > 1)
			yy_internal_buf[i] = 2;
		else
			yy_internal_buf[i] = 1;
	}

	unsigned char f_reverse = 3;
	if (yy_internal_buf[0] == 2 && yy_internal_buf[1] == 1 && yy_internal_buf[2] == 1 && yy_internal_buf[3] == 2 && yy_internal_buf[4] == 1)
		f_reverse = 1;
	else if (yy_internal_buf[0] == 1 && yy_internal_buf[1] == 2 && yy_internal_buf[2] == 1 && yy_internal_buf[3] == 1 && yy_internal_buf[4] == 2)
		f_reverse = 2;

	if (f_reverse == 3)
		return 0;

	if (f_reverse == 1)
	{
		j = yy_decode_e_pack(&stream[sus->head + 2], sus->len - 5, yy_internal_buf, 8, 12, 0);
	}
	else
	{
		j = yy_decode_e_pack(&stream[sus->head + 3], sus->len - 5, yy_internal_buf, 8, 12, 0);
		for (i = 0; i < j / 2; i++)
		{
			s = yy_internal_buf[j - 1 - i];
			yy_internal_buf[j - 1 - i] = yy_internal_buf[i];
			yy_internal_buf[i] = s;
		}
	}

	k = 0;
	for (i = 0; i < j; i += 2)
	{
		if (yy_internal_buf[i] == 2 && yy_internal_buf[i + 1] == 1)
			s = 1;
		if (yy_internal_buf[i] == 1 && yy_internal_buf[i + 1] == 2)
			s = 0;
		if (yy_internal_buf[i] == 1 && yy_internal_buf[i + 1] == 1)
			return 0;
		if (yy_internal_buf[i] == 2 && yy_internal_buf[i + 1] == 2)
			return 0;
		yy_internal_buf[k] = s;
		k++;
	}

	s = 0;
	for (i = 0; i < k; i += 4)
		yy_internal_buf[s++] = yy_decode_match(yy_internal_buf, i, 4, 1, 1);
	k = s;


	int S1 = 0, C = 0, K = 0, MODE11_C = 0, MODE11_K = 0;
	int ii = 0, jj = 0, kk = 0;
	unsigned short Add = 0;
	s = 0;
	S1 = 0;
	if (k % 2 == 0)
	{
		for (i = 0; i < k; i += 2)
		{

			if (i < k - 2) S1 += yy_internal_buf[i + 1];
			yy_internal_buf2[s] = yy_internal_buf[i];
			s++;
		}
	}
	else
	{
		for (i = 1; i < k; i += 2)
		{

			if (i < k) S1 += yy_internal_buf[i - 1];
			yy_internal_buf2[s] = yy_internal_buf[i];
			s++;
		}
	}

	jj = 0;
	kk = 0;
	for (ii = s - 1; ii >= 0; ii--)
	{
		if (((yy_internal_buf2[ii] * 2) + jj) >= 10)
		{

			kk = ((yy_internal_buf2[ii] * 2) % 10) + jj;
			yy_internal_buf2[ii] = kk;
			jj = 1;
			if (ii == 0)
			{

				for (i = 0; i < s; i++)
				{

					yy_internal_buf2[s - i] = yy_internal_buf2[s - i - 1];
				}
				yy_internal_buf2[0] = 1;
				s += 1;
			}
		}
		else
		{

			yy_internal_buf2[ii] = yy_internal_buf2[ii] * 2 + jj;
			jj = 0;
		}
	}
	Add = 0;

	for (i = 0; i < s; i++)
		Add += yy_internal_buf2[i];

	Add += S1;
	S1 = Add % 10;
	S1 = (10 - S1) % 10;

	Add = 0;
	s = 0;
	C = 0;
	if ((k - 1) % 2 == 0)
	{
		for (i = 0; i < k - 1; i += 2)
		{

			if (i < k - 3) C += yy_internal_buf[i + 1];
			yy_internal_buf2[s] = yy_internal_buf[i];
			s++;
		}
	}
	else
	{
		for (i = 1; i < k - 1; i += 2)
		{

			if (i < k - 1) C += yy_internal_buf[i - 1];
			yy_internal_buf2[s] = yy_internal_buf[i];
			s++;
		}
	}

	jj = 0;
	kk = 0;
	for (ii = s - 1; ii >= 0; ii--)
	{
		if (((yy_internal_buf2[ii] * 2) + jj) >= 10)
		{
			kk = ((yy_internal_buf2[ii] * 2) % 10) + jj;
			yy_internal_buf2[ii] = kk;
			jj = 1;
			if (ii == 0)
			{

				for (i = 0; i < s; i++)
				{

					yy_internal_buf2[s - i] = yy_internal_buf2[s - i - 1];
				}
				yy_internal_buf2[0] = 1;
				s += 1;
			}
		}
		else
		{

			yy_internal_buf2[ii] = yy_internal_buf2[ii] * 2 + jj;
			jj = 0;
		}
	}
	Add = 0;

	for (i = 0; i < s; i++)
		Add += yy_internal_buf2[i];

	Add += C;
	C = Add % 10;
	C = (10 - C) % 10;

	s = 0;
	K = 0;
	if (k % 2 == 0)
	{
		for (i = 0; i < k; i += 2)
		{

			if (i < k - 2) K += yy_internal_buf[i + 1];
			yy_internal_buf2[s] = yy_internal_buf[i];
			s++;
		}
	}
	else
	{
		for (i = 1; i < k; i += 2)
		{

			if (i < k) K += yy_internal_buf[i - 1];
			yy_internal_buf2[s] = yy_internal_buf[i];
			s++;
		}
	}

	jj = 0;
	kk = 0;
	for (ii = s - 1; ii >= 0; ii--)
	{
		if (((yy_internal_buf2[ii] * 2) + jj) >= 10)
		{

			kk = ((yy_internal_buf2[ii] * 2) % 10) + jj;
			yy_internal_buf2[ii] = kk;
			jj = 1;
			if (ii == 0)
			{

				for (i = 0; i < s; i++)
				{

					yy_internal_buf2[s - i] = yy_internal_buf2[s - i - 1];
				}
				yy_internal_buf2[0] = 1;
				s += 1;
			}
		}
		else
		{

			yy_internal_buf2[ii] = yy_internal_buf2[ii] * 2 + jj;
			jj = 0;
		}
	}
	Add = 0;

	for (i = 0; i < s; i++)
		Add += yy_internal_buf2[i];

	Add += K;
	K = Add % 10;
	K = (10 - K) % 10;

	Add = 0;
	s = 0;
	j = 1;
	MODE11_C = 0;
	for (i = 0; i < k - 2; i++)
	{
		if (j > 6)
			j = 2;
		else
			j++;

		Add += yy_internal_buf[k - 2 - i - 1] * j;
	}
	MODE11_C = Add % 11;
	MODE11_C = (11 - MODE11_C) % 11;


	s = 0;
	MODE11_K = 0;
	if (k % 2 == 0)
	{
		for (i = 0; i < k; i += 2)
		{

			if (i < k - 2) MODE11_K += yy_internal_buf[i + 1];
			yy_internal_buf2[s] = yy_internal_buf[i];
			s++;
		}
	}
	else
	{
		for (i = 1; i < k; i += 2)
		{

			if (i < k) MODE11_K += yy_internal_buf[i - 1];
			yy_internal_buf2[s] = yy_internal_buf[i];
			s++;
		}
	}

	jj = 0;
	kk = 0;
	for (ii = s - 1; ii >= 0; ii--)
	{

		if (((yy_internal_buf2[ii] * 2) + jj) >= 10)
		{

			kk = ((yy_internal_buf2[ii] * 2) % 10) + jj;
			yy_internal_buf2[ii] = kk;
			jj = 1;
			if (ii == 0)
			{

				for (i = 0; i < s; i++)
				{

					yy_internal_buf2[s - i] = yy_internal_buf2[s - i - 1];
				}
				yy_internal_buf2[0] = 1;
				s += 1;
			}
		}
		else
		{

			yy_internal_buf2[ii] = yy_internal_buf2[ii] * 2 + jj;
			jj = 0;
		}
	}
	Add = 0;

	for (i = 0; i < s; i++)
		Add += yy_internal_buf2[i];

	Add += MODE11_K;
	MODE11_K = Add % 10;
	MODE11_K = (10 - MODE11_K) % 10;

	switch (yy_decset.MSI_VALIDATION)
	{
	case 0:
		break;
	case 1:
		if (S1 != yy_internal_buf[k - 1])
			return 0;
		break;
	case 2:
		if (C != yy_internal_buf[k - 2])
			return 0;
		if (K != yy_internal_buf[k - 1])
			return 0;
		break;
	case 3:
		if (MODE11_C != yy_internal_buf[k - 2])
			return 0;
		if (MODE11_K != yy_internal_buf[k - 1])
			return 0;
		break;
	default:
		break;
	}

	switch (yy_decset.MSI_VALIDATION_OUT)
	{
	case 0:
		if (yy_decset.MSI_VALIDATION == 1)
			k -= 1;
		else if (yy_decset.MSI_VALIDATION == 2 || yy_decset.MSI_VALIDATION == 3)
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
		if (yy_internal_buf[i] == ':')
			yy_internal_buf[i] = 'X';
	}
	yy_internal_buf[k] = 0;

	if (k < yy_decset.MSI_MIN_LEN)
		return 0;

	yy_interface->sym->type = YY_MSI;
	sprintf((char *)yy_interface->sym->data, "%s", yy_internal_buf);

	return 1;
}
