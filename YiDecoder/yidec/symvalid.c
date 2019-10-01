
#include "symvalid.h"

YySymbol yy_valid_syms[YY_VAL_MAX_NUM];
unsigned char yy_valid_idx = 0;
unsigned char yy_valid_wait = 0;
unsigned char yy_valid_addition = 0;
unsigned char yy_valid_led_state = 0;
unsigned char yy_valid_ttl_state = 0;
unsigned short yy_valid_lock_type = 0;

void yy_symvalid_clear(void)
{
	int t;
	for (t = 0; t < YY_VAL_MAX_NUM; t++)
		yy_valid_syms[t].type = YY_NONE;
	yy_valid_addition = 0;
	yy_valid_led_state = 0;
	yy_valid_ttl_state = 0;
	yy_valid_lock_type = 0;
}

void yy_symvalid_ttl(void)
{
	int t;
	if (yy_valid_wait)
		return;

	if (yy_valid_ttl_state)
		yy_valid_ttl_state--;
	else
		yy_valid_addition = 0;

	for (t = 0; t < YY_VAL_MAX_NUM; t++)
	{
		if (--yy_valid_syms[t].ttl == 0)
		{
			if (yy_valid_syms[t].type < 0x10)
				yy_valid_lock_type &= ~(1 << yy_valid_syms[t].type);
			yy_valid_syms[t].type = YY_NONE;
		}
	}
}

void yy_symvalid_ttl_wait(unsigned char status)
{
	yy_valid_wait = status;
}

unsigned char yy_symvalid_get_state(void)
{
	if (yy_valid_led_state)
	{
		yy_valid_led_state--;
		return 0;
	}
	else
	{
		return yy_valid_ttl_state;
	}
}

unsigned char yy_symvalid_get_ttl_state(void)
{
	return yy_valid_ttl_state;
}

unsigned short yy_symvalid_get_lock_type(void)
{
	return yy_valid_lock_type;
}

void yy_symvalid_ttl_reset(unsigned char type)
{
	int t;
	unsigned char TIMEHOLDON;

	if (yy_decset.LOCK_REFINE && (type == YY_EAN13 || type == YY_ISBN
								  || type == YY_EAN8 || type == YY_UPCA))
		TIMEHOLDON = yy_decset.TIMEHOLDON - 1;
	else
		TIMEHOLDON = yy_decset.TIMEHOLDON;

	for (t = 0; t < YY_VAL_MAX_NUM; t++)
	{
		if (yy_valid_syms[t].type == type)
		{
			yy_valid_syms[t].ttl = TIMEHOLDON;
			if (yy_valid_ttl_state < TIMEHOLDON)
				yy_valid_ttl_state = TIMEHOLDON;
		}
	}
}

unsigned char yy_symvalid(YySymbol *yy_symbol)
{
	int t, cnt = 0;
	unsigned char M, TIMEHOLDON, res = 0;

	//有M次相同的数据，则输出。一重校验M=2，二重校验M=3
	if (yy_symbol->type == YY_QR || ((yy_decset.MULTIVALID & 0x40) && (yy_symbol->type == YY_EAN13 || yy_symbol->type == YY_ISBN || yy_symbol->type == YY_UPCA)))
		M = 1;
	else
		M = (yy_decset.MULTIVALID & 0x07) + 1;

	if (yy_decset.TIMEHOLDON_2D && yy_symbol->type == YY_QR)
		TIMEHOLDON = yy_decset.TIMEHOLDON_2D;
	else if (yy_decset.LOCK_REFINE && (yy_symbol->type == YY_EAN13 || yy_symbol->type == YY_ISBN
									   || yy_symbol->type == YY_EAN8 || yy_symbol->type == YY_UPCA))
		TIMEHOLDON = yy_decset.TIMEHOLDON - 1;
	else
		TIMEHOLDON = yy_decset.TIMEHOLDON;

	//本次输入的码，计算重复数
	for (t = 0; t < YY_VAL_MAX_NUM; t++)
	{
		if (yy_decset.MULTIVALID & 0x80)
		{
			if (yy_valid_syms[t].type == 0xFF)
				continue;
		}
		else
		{
			if (yy_symbol->type != yy_valid_syms[t].type)
				continue;
		}

		if (strcmp((char *)(yy_symbol->data), (char *)(yy_valid_syms[t].data)))
			continue;

		cnt++;
	}

	//取出
	//已有M-1个，则本次的是第M个，需输出
	if (cnt + 1 == M)
	{
		if (yy_symbol->type == YY_ADD && yy_valid_addition == 0)
		{
			for (t = 0; t < YY_VAL_MAX_NUM; t++)
			{
				if (yy_valid_syms[t].type == YY_ADD)
					yy_valid_syms[t].ttl = yy_decset.TIMETOLIVE;
			}
			return 0; //本次数据不存入，重置附加码时间，等待EAN先输出
		}
		else
		{
			res = 1;
			if (yy_symbol->type == YY_EAN13 || yy_symbol->type == YY_ISBN
					|| yy_symbol->type == YY_EAN8 || yy_symbol->type == YY_UPCA || yy_symbol->type == YY_UPCE)
				yy_valid_addition = 1;
			else
				yy_valid_addition = 0;
		}
	}

	//存入
	//已有个数小于M，则存入。若等于M，则处于锁定状态
	if (cnt < M)
	{
		yy_symbol->ttl = yy_decset.TIMETOLIVE;
		memcpy((char *)(&yy_valid_syms[yy_valid_idx]), (char *)(yy_symbol), sizeof(YySymbol));
		if (++yy_valid_idx == YY_VAL_MAX_NUM)
			yy_valid_idx = 0;
	}

	//处于已锁定状态，再次读码时，让绿灯闪一下，附加码除外
	if (res == 1 && yy_valid_ttl_state && yy_symbol->type != YY_ADD)
		yy_valid_led_state = 1;

	//HOLDON状态下，再次收到相同的，重置HOLDON时间，或成功输出，TTL进入HOLDON状态
	if (cnt >= M || res == 1)
	{
		for (t = 0; t < YY_VAL_MAX_NUM; t++)
		{
			if (yy_decset.MULTIVALID & 0x80)
			{
				if (yy_valid_syms[t].type == 0xFF)
					continue;
			}
			else
			{
				if (yy_symbol->type != yy_valid_syms[t].type)
					continue;
			}

			if (strcmp((char *)(yy_symbol->data), (char *)(yy_valid_syms[t].data)))
				continue;

			yy_valid_syms[t].ttl = TIMEHOLDON;
		}
		if (yy_valid_ttl_state < TIMEHOLDON)
			yy_valid_ttl_state = TIMEHOLDON;
		if (yy_symbol->type < 0x10)
			yy_valid_lock_type |= 1 << yy_symbol->type;
	}

	return res;
}

unsigned char yy_symvalid_continuous(YySymbol *yy_symbol)
{
	unsigned char res = 1;

	if (yy_symbol->type != yy_valid_syms[0].type)
		res = 0;

	if (strcmp((char *)(yy_symbol->data), (char *)(yy_valid_syms[0].data)))
		res = 0;

	if (res == 0)
		memcpy((char *)(&yy_valid_syms[0]), (char *)(yy_symbol), sizeof(YySymbol));

	return res;
}


