
#include "symedit.h"

void strrep(char *dst, char *str, char *str1, char *str2)
{
	char *temp = str;
	unsigned int len1 = strlen(str1);

	dst[0] = 0;

	if (len1 != 0)
	{
		while (1)
		{
			char *find = strstr(temp, str1);
			if (find == 0)
				break;

			strncat(dst, temp, (unsigned int)(find - temp));
			strcat(dst, str2);
			temp = find + len1;
		}
	}

	strcat(dst, temp);
}

void yy_symedit(YySymbol *yy_symbol)
{
	int t;
	char s[YY_MAX_SYM_LEN] = {0};

	char *sym = (char *)yy_symbol->data;

	if (yy_symbol->type >= YY_SET)
		return;

	if ((yy_decset.EDIT_MASK & (1 << yy_symbol->type)) == 0)
		return;

	//≤Â»Î
	for (t = 0; t < 4; t++)
	{
		unsigned int pos;

		if (yy_decset.EDIT_SET[t].edit_insert[0] == 0xFF)
			pos = strlen(sym);
		else
			pos = (unsigned int)yy_decset.EDIT_SET[t].edit_insert[0] - 1;

		if (strlen(sym) < pos)
			continue;
		strncpy(s, sym, pos);
		s[pos] = 0;
		strcat(s, (char *)(&yy_decset.EDIT_SET[t].edit_insert[1]));
		strcat(s, (char *)(sym + pos));
		strcpy(sym, s);
	}

	//…æ≥˝
	for (t = 0; t < 4; t++)
	{
		unsigned int pos = (unsigned int)yy_decset.EDIT_SET[t].edit_delete[0] - 1;
		if (strlen(sym) <= pos)
			continue;
		strncpy(s, sym, pos);
		s[pos] = 0;
		if (strlen(sym) > pos + (unsigned int)yy_decset.EDIT_SET[t].edit_delete[1])
			strcat(s, sym + pos + (unsigned int)yy_decset.EDIT_SET[t].edit_delete[1]);
		strcpy(sym, s);
	}

	//ÃÊªª
	for (t = 0; t < 4; t++)
	{
		strrep(s, sym, (char *)yy_decset.EDIT_SET[t].edit_repsrc, (char *)yy_decset.EDIT_SET[t].edit_repdst);
		strcpy(sym, s);
	}
}


