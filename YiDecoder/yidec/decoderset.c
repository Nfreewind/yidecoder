
#include "decoderset.h"

unsigned char yy_decset_state = 0;

const char YY_DECODER_SET_DEFAULT[] =
{
	//GROBAL
	0, 0x41, 8, 3, 6, 0x62, 12, '\n',

	//GROBAL_ID
	0, 'e', 'f', 'd', 'b', 'c', 'k', 'g', 'a', 'h', 'm', 'i', 'j', 'n', 'l',

	//YY_EAN13
	1, 1, 1, 0,

	//YY_EAN8
	1, 1, 1,

	//YY_UPCA
	1, 1, 1, 0, 0,

	//YY_UPCE
	1, 1, 1, 0, 0,

	//YY_CODEBAR
	1, 0, 1, 0, 5,

	//YY_CODE128
	1, 1, 0,

	//YY_CODE39
	1, 0, 1, 2, 0, 1,

	//YY_CODE93
	1, 1, 1,

	//YY_CODE11
	0, 1, 0, 5,

	//YY_IL25
	1, 3, 1, 6,

	//YY_I25
	0, 4,

	//YY_M25
	0, 0, 1, 6,

	//YY_MSI
	0, 1, 0, 4,

	//YY_ADD
	0,

	//GLOBAL
	3, 0,

	//YY_DECODER_SETTING
	4, 1, 0x01, 0,

	//YY_QR
	1, 1,

	//GLOBAL
	1, 3, 1, 1, 6, 30, 50, 1, 1, 1,

	//YY_PHARMA
	0, 0, 13, 2,

	//RESERVED(28)
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

	//YY_EDIT
	0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

};

void yy_decset_save(void)
{
	//save yy_decset to file
}

void yy_decset_load(void)
{
	//load yy_decset from file
}

void yy_decset_init(void)
{
	yy_decset_load();
	if (yy_decset.CONTINUOUS == 0xFF)
		memcpy((void *)&yy_decset, YY_DECODER_SET_DEFAULT, sizeof(YyDecoderSet));

	yy_decset.CONTINUOUS = 0;
}

void yy_decset_default(unsigned char idx, unsigned char val)
{
	((unsigned char *)&yy_decset)[idx] = val;

	beep_sound(4);
	delay_ms(100);
	beep_sound(6);
	delay_ms(100);
	beep_sound(8);
	delay_ms(100);
	beep_sound(0);
}

unsigned char yy_decset_get_state(void)
{
	return yy_decset_state;
}

void yy_decset_edit_set(unsigned char idx, unsigned char val)
{
	static unsigned char pre_idx = 16;

	unsigned char idxn;
	unsigned char idxm;
	unsigned char *item;
	unsigned char itemlen;
	unsigned char i;

	//记录上次的，进行追加
	if (idx >= 16)
		idx = pre_idx;
	if (idx >= 16)
	{
		beep_sound(8);
		delay_ms(100);
		beep_sound(7);
		delay_ms(100);
		beep_sound(0);
		return;
	}
	pre_idx = idx;
	idxn = idx / 4;
	idxm = idx % 4;

	//idx=0..15，0..3对应EDIT_SET[0]，4..7对应EDIT_SET[1]
	//设置方法：
	//idx为操作的设置项，val=0x00清空该设置项，idx=0x10&&val!=0x00则逐个追加
	//如：0x00,0x00;0x10,0x02;0x10,0x30;0x10,0x31;
	//则EDIT_SET[0]配置为在位置2插入"01"

	switch (idxm)
	{
	case 0:
		item = yy_decset.EDIT_SET[idxn].edit_insert;
		itemlen = sizeof(yy_decset.EDIT_SET[idxn].edit_insert) - 1;
		break;
	case 1:
		item = yy_decset.EDIT_SET[idxn].edit_delete;
		itemlen = sizeof(yy_decset.EDIT_SET[idxn].edit_delete);
		break;
	case 2:
		item = yy_decset.EDIT_SET[idxn].edit_repsrc;
		itemlen = sizeof(yy_decset.EDIT_SET[idxn].edit_repsrc) - 1;
		break;
	case 3:
		item = yy_decset.EDIT_SET[idxn].edit_repdst;
		itemlen = sizeof(yy_decset.EDIT_SET[idxn].edit_repdst) - 1;
		break;
	}

	if (val == 0)
	{
		yy_decset_state = 1;
		memset(item, 0, itemlen);
		beep_sound(4);
		delay_ms(100);
		beep_sound(5);
		delay_ms(100);
		beep_sound(7);
		delay_ms(100);
		beep_sound(0);
	}
	else
	{
		for (i = 0; i < itemlen; i++)
		{
			if (item[i] == 0)
			{
				item[i] = val;
				break;
			}
		}
		if (i == itemlen)
		{
			beep_sound(8);
			delay_ms(100);
			beep_sound(7);
			delay_ms(100);
			beep_sound(0);
		}
		else
		{
			beep_sound(8);
			delay_ms(100);
			beep_sound(10);
			delay_ms(100);
			beep_sound(0);
		}
	}
}

void yy_decset_edit_mask(unsigned char val)
{
	unsigned char mask = val >> 4;
	unsigned char onoff = val & 0x0F;

	//val高4位是MASK位0..15，低4位是开(1)/关(0)
	//MASK=15，则对全部码制生效

	if (mask == 0x0F)
	{
		if (onoff)
			yy_decset.EDIT_MASK = 0x7F;
		else
			yy_decset.EDIT_MASK = 0x00;
	}
	else
	{
		if (onoff)
			yy_decset.EDIT_MASK |= (1 << mask);
		else
			yy_decset.EDIT_MASK &= ~(1 << mask);
	}

	beep_sound(4);
	delay_ms(100);
	beep_sound(6);
	delay_ms(100);
	beep_sound(8);
	delay_ms(100);
	beep_sound(0);
}

unsigned char yy_decset_set(YySymbol *yy_symbol)
{
	char *setcode = (char *)yy_symbol->data;
	unsigned char idx, val, itemnum, i;

	if (yy_symbol->type != YY_SET)
	{
		yy_decset_state = 0;
		return 0;
	}

	if (strcmp(setcode, "INF*#01#VER") == 0)
	{
		yy_symbol->type = YY_INFORM1;
		return 0;
	}

	if (strcmp(setcode, "INF*#02#COM") == 0)
	{
		yy_symbol->type = YY_INFORM2;
		return 0;
	}

	itemnum = strlen(setcode);

	if (itemnum / 4 == 0 || itemnum % 4 != 0)
	{
		beep_sound(8);									//非法设置码，有提示音，但不输出
		delay_ms(100);
		beep_sound(7);
		delay_ms(100);
		beep_sound(0);
		return 1;
	}

	for (i = 0; i < itemnum; i += 4)
	{
		if (sscanf(setcode + i, "%2hhX%2hhX", &idx, &val) != 2)
		{
			beep_sound(8);								//非法设置码，有提示音，但不输出
			delay_ms(100);
			beep_sound(7);
			delay_ms(100);
			beep_sound(0);
			return 1;
		}

		if (idx == 0xFF)
			yy_decset_save();
		else if (idx <= 0x7D)
			yy_decset_default(idx, val);				//0x00-0x7D：非EDIT设置项，直接赋值
		else if (idx <= 0x7E)
			yy_decset_edit_mask(val);					//0x7E-0x7E：EDIT_MASK设置项
		else if (idx <= 0x8F)
			yy_decset_edit_set(idx - 0x80, val);		//0x7F-0x8F：EDIT_SET设置项
	}

	return 1;
}


