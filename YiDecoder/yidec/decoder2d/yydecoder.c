
#include "yy/yydecoder.h"
#include "sx/sxdecoder.h"

YySymbol yy_symbol;									//输出结果

void yy_decoder_get_sym(unsigned char *type, unsigned char *len, unsigned char *outsym)
{
	*type = yy_symbol.type;
	*len = yy_symbol.len;
	memcpy((char *)outsym, (char *)yy_symbol.data, yy_symbol.len);
	outsym[yy_symbol.len] = 0;
}

#define YY_VAL_MAX_NUM 3				//定义输出池大小
YySymbol yy_valid_syms[YY_VAL_MAX_NUM];
unsigned char yy_valid_idx = 0;
unsigned char yy_valid_wait = 0;
unsigned char yy_valid_out_state = 0;
unsigned char yy_valid_ttl_state = 0;

void yy_symvalid_clear(void)
{
	int t;
	for(t = 0; t < YY_VAL_MAX_NUM; t++)
	{
		yy_valid_syms[t].type = YY_NONE;
	}
}

void yy_symvalid_ttl(void)
{
	int t;
	if(yy_valid_out_state)
		yy_valid_out_state--;
	if(yy_valid_wait)
		return;
	if(yy_valid_ttl_state)
		yy_valid_ttl_state--;
	for(t = 0; t < YY_VAL_MAX_NUM; t++)
	{
		if(--yy_valid_syms[t].ttl == 0)
			yy_valid_syms[t].type = YY_NONE;
	}
}

void yy_symvalid_ttl_wait(unsigned char status)
{
	yy_valid_wait = status;
}

unsigned char yy_symvalid_get_state(void)
{
	if(yy_valid_out_state)
		return 0;
	else
		return yy_valid_ttl_state;
}

unsigned char yy_symvalid_get_ttl_state(void)
{
	return yy_valid_ttl_state;
}

unsigned char yy_symvalid(YySymbol *yy_symbol)
{
	int t, cnt = 0;
	unsigned char M = 1, res = 0;
		
	//本次输入的码，计算重复数
	for(t = 0; t < YY_VAL_MAX_NUM; t++)
	{
		if(yy_symbol->type != yy_valid_syms[t].type)
			continue;
		
		if(strcmp((char *)(yy_symbol->data), (char *)(yy_valid_syms[t].data)))
			continue;
		
		cnt++;
		break;
	}
	
	//取出
	if(cnt == 0)
	{
		res = 1;
	
		yy_symbol->ttl = sx_decset.YY_TIMEHOLDON;
		memcpy((char *)(&yy_valid_syms[yy_valid_idx]), (char *)(yy_symbol), sizeof(YySymbol));
		if(++yy_valid_idx == YY_VAL_MAX_NUM)
			yy_valid_idx = 0;
	}
	else
	{
		yy_valid_syms[t].ttl = sx_decset.YY_TIMEHOLDON;
	}
	
	yy_valid_ttl_state = sx_decset.YY_TIMEHOLDON;
		
	if(res != 0)
		yy_valid_out_state = 2;
	
	return res;
}

unsigned char yy_decoder_main(YyImgInterface *imgif, YyImgRegion *region)
{
	if(yy_img_scanner_qr(imgif, region) == 0)
		return 0;
	
	if(sx_decset.YY_TIMEHOLDON == 0)
	{
		if(!sx_symvalid((SunXiaSymbol *)&yy_symbol))
			return 0;
	}
	else
	{
		if(!yy_symvalid(&yy_symbol))
			return 0;
	}
	
	yy_symvalid_ttl_wait(1);				//暂停锁定计时，设置或输出完成后再开启
	
	if(sx_decset.YY_SETTINGCODE)
	{
		char settingcode[9];
		sprintf(settingcode, "SET*#%02X#", sx_decset.YY_SETTINGCODE);
		if(memcmp(yy_symbol.data, settingcode, 8) == 0)
		{
			SunXiaSymbol sx_symbol;
			strcpy((char *)(sx_symbol.data), (char *)(yy_symbol.data + 8));
			sx_symbol.type = SX_SET;
			if(sx_decset_set(&sx_symbol))			//设置码
				return 2;
		}
	}

	return 1;
}
