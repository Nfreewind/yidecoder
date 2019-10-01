
#include "yydecoder.h"

void yy_decoder_get_sym(unsigned char *type, unsigned char *len, unsigned char *outsym)
{
	*type = yy_symbol.type;
	*len = strlen((char *)yy_symbol.data);
	strcpy((char *)outsym, (char *)yy_symbol.data);
}

unsigned char yy_decoder_main(unsigned short *yy_data_buf)
{
	int i = 0, j = 0, k = 0;

	YyStartEnd yy_start_end[YY_SUS_MAX_NUM];		//ԭʼ�������뻺��������
	YyInterface yy_interface;						//�������ӿ�

	yy_sus_init(yy_start_end);						//��ʼ���������뻺����
	j = yy_sus_find(yy_data_buf, yy_start_end);		//�����������룬�����ҵ��ĸ���
	j = yy_sus_expansion(yy_start_end, j);			//��չ�������룬�����ҵ��ĸ���

	yy_interface.data = yy_data_buf;
	yy_interface.sym = &yy_symbol;

	for (i = 0; i < j; i++)
	{
		yy_interface.sus = &yy_start_end[i];

		for (k = 0; k < 1; k++)
		{
			if (yy_ean13(&yy_interface) == 1)
				break;
			if (yy_ean8(&yy_interface) == 1)
				break;
			if (yy_upca(&yy_interface) == 1)
				break;
			if (yy_upce(&yy_interface) == 1)
				break;
			if (yy_codebar(&yy_interface) == 1)
				break;
			if (yy_code128(&yy_interface) == 1)
				break;
			if (yy_code39(&yy_interface) == 1)
				break;
			if (yy_code93(&yy_interface) == 1)
				break;
			if (yy_il25(&yy_interface) == 1)
				break;
			if (yy_i25(&yy_interface) == 1)
				break;
			if (yy_m25(&yy_interface) == 1)
				break;
			if (yy_msi(&yy_interface) == 1)
				break;
			if (yy_pharma(&yy_interface) == 1)
				break;
			if (yy_addition(&yy_interface) == 1)
				break;
		}

		if (k == 1)							//����ʧ��
			continue;

		if (yy_decset.CONTINUOUS)			//����ģʽ
			return yy_symvalid_continuous(&yy_symbol);

		if (yy_symvalid(&yy_symbol))		//����У��
			break;
	}

	if (j == 0 || i == j)					//����ʧ�ܻ����У��ʧ��
	{
		if (yy_decset.LOCK_REFINE)
		{
			unsigned short locktype = yy_symvalid_get_lock_type();
			if (locktype & (1 << YY_EAN13))
			{
				if (yy_ean13_lock(&yy_interface))
					yy_symvalid_ttl_reset(YY_EAN13);
			}
			if (locktype & (1 << YY_EAN8))
			{
				if (yy_ean8_lock(&yy_interface))
					yy_symvalid_ttl_reset(YY_EAN8);
			}
			if (locktype & (1 << YY_UPCA))
			{
				if (yy_upca_lock(&yy_interface))
					yy_symvalid_ttl_reset(YY_UPCA);
			}
		}
		return 0;
	}

	yy_symvalid_ttl_wait(1);				//��ͣ������ʱ�����û������ɺ��ٿ���

	if (yy_decset_set(&yy_symbol))			//������
		return 2;

	yy_symedit(&yy_symbol);					//����༭
	yy_global_id(&yy_symbol);				//����ַ�

	return 1;
}

unsigned char yy_decoder_2d_main(YyImgInterface *imgif, YyImgRegion *region)
{
	if (yy_img_scanner_qr(imgif, region) == 0)
		return 0;

	if (!yy_symvalid(&yy_symbol))
		return 0;

	yy_symvalid_ttl_wait(1);				//��ͣ������ʱ�����û������ɺ��ٿ���

	if (yy_decset.SETTINGCODE_2D)
	{
		char settingcode[9];
		sprintf(settingcode, "SET*#%02X#", yy_decset.SETTINGCODE_2D);
		if (memcmp(yy_symbol.data, settingcode, 8) == 0)
		{
			YySymbol yy_symbol;
			strcpy((char *)(yy_symbol.data), (char *)(yy_symbol.data + 8));
			yy_symbol.type = YY_SET;
			if (yy_decset_set(&yy_symbol))			//������
				return 2;
		}
	}

	return 1;
}
