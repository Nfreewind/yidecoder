
#ifndef _YY_BASIC_H_
#define _YY_BASIC_H_

#include "stdio.h"
#include "string.h"

#define YY_MAX_DAT_NUM 768				//ԭʼ����������
#define YY_MAX_MIN_MUL 10				//��Ч���ݵ���С������������ݵ���߱�����ϵ��ȡֵ10 ���ʾ�����������С���ݱ������ó���10��

#define YY_MAX_SYM_LEN 256				//������������

#define YY_SUS_MAX_NUM 1024				//�����ԭʼ���ݻ�����ȡ�õ�������������ĸ�������
#define YY_SUS_MIN_LEN 12				//���������������ո�������
#define YY_SUS_MAX_LEN YY_MAX_DAT_NUM	//���������������ո������� 1525

#define YY_VAL_MAX_NUM 12				//��������ش�С

typedef enum
{
	YY_EAN13	= 0x00,
	YY_ISBN		= 0x01,
	YY_EAN8		= 0x02,
	YY_UPCA		= 0x03,
	YY_UPCE		= 0x04,
	YY_CODEBAR	= 0x05,
	YY_CODE128	= 0x06,
	YY_CODE39	= 0x07,
	YY_CODE93	= 0x08,
	YY_CODE11	= 0x09,
	YY_IL25		= 0x0A,
	YY_I25		= 0x0B,
	YY_M25		= 0x0C,
	YY_MSI		= 0x0D,
	YY_PHARMA	= 0x0E,
	YY_ADD		= 0x40,
	YY_QR		= 0x80,
	YY_PDF417	= 0x81,
	YY_DM		= 0x82,
	YY_INFORM1	= 0xFC,
	YY_INFORM2	= 0xFD,
	YY_SET		= 0xFE,
	YY_NONE		= 0xFF,
}YyCodeType;

typedef struct	//�����������뻺��������ʼ����ֹ���ݸ�ʽ(�������ҿհ�������)
{
	unsigned short head;	//ͷλ��
	unsigned short len;		//����
}YyStartEnd;

typedef struct
{
	unsigned char data[YY_MAX_SYM_LEN];
	unsigned char len;
	unsigned char type;
	unsigned char ttl;
}YySymbol;

typedef struct
{
	unsigned short *data;
	YyStartEnd *sus;
	YySymbol *sym;
}YyInterface;

typedef struct
{
	unsigned char edit_insert[14];			//���룬��һ�ֽ�Ϊλ�ã���1��ʼ��0��ʾ�����ã�0xFF��ʾβ������
	unsigned char edit_delete[2];			//ɾ������һ�ֽ�Ϊλ�ã��ڶ��ֽ�Ϊ����
	unsigned char edit_repsrc[8];			//�滻Դ
	unsigned char edit_repdst[8];			//�滻ֵ
}YyEditSet;

typedef struct
{
	//0 GROBAL
	unsigned char CONTINUOUS;				//����ģʽ��0=�رգ�1=���������2=���������
	unsigned char MULTIVALID;				//����У�飺1=1��У�飬2=2��У�飬���5��bit[7]��ʾ��ͬ���ƻ�����bit[6]��ʾEAN13��UPCA�����У��
	unsigned char TIMETOLIVE;				//У��ʱ�ޣ���λ100ms
	unsigned char TIMEHOLDON;				//����ʱ�䣺��λ100ms
	unsigned char MULTILINES;				//�������ã�0=����ɨ��2=20�ߣ�6=60�ߣ�10=100��
	unsigned char SOUND_ON;					//�������ã�����λ����������λʱ����0xFF=���ܲ���ģʽ
	unsigned char SLEEP_TIME;				//˯��ʱ�䣺0=�رգ���λ15s
	unsigned char TAILEDCHAR;				//�����ַ���\n��\t
	
	//8 GLOBAL_ID
	unsigned char ID_ON_LR;					//ȫ���ַ���0=�رգ�1=��2=��
	unsigned char EAN13_ID;
	unsigned char EAN13_ISBN_ID;
	unsigned char EAN8_ID;
	unsigned char UPCA_ID;
	unsigned char UPCE_ID;
	unsigned char CODEBAR_ID;
	unsigned char CODE128_ID;
	unsigned char CODE39_ID;
	unsigned char CODE93_ID;
	unsigned char CODE11_ID;
	unsigned char IL25_ID;
	unsigned char I25_ID;
	unsigned char M25_ID;
	unsigned char MSI_ID;
	
	//23 YY_EAN13
	unsigned char EAN13_ON;
	unsigned char EAN13_VALIDATION;
	unsigned char EAN13_VALIDATION_OUT;
	unsigned char EAN13_TO_ISBN;
	
	//27 YY_EAN8
	unsigned char EAN8_ON;
	unsigned char EAN8_VALIDATION;
	unsigned char EAN8_VALIDATION_OUT;
	
	//30 YY_UPCA
	unsigned char UPCA_ON;
	unsigned char UPCA_VALIDATION;
	unsigned char UPCA_VALIDATION_OUT;
	unsigned char UPCA_TRANS;				//ת�����ܣ�0=�رգ�1=ת��EAN13
	unsigned char UPCA_EXPANSION;			//��չ���ܣ�0=�رգ�1=��ȥǰ���㣬2=��ȥ��һ���㣬3=��ȥϵͳ�ַ�
	
	//35 YY_UPCE
	unsigned char UPCE_ON;
	unsigned char UPCE_VALIDATION;
	unsigned char UPCE_VALIDATION_OUT;
	unsigned char UPCE_TRANS;				//��չ���ܣ�0=�رգ�1=ת��EAN13��2=��չUPCA
	unsigned char UPCE_EXPANSION;			//��չ���ܣ�0=�رգ�1=��ȥǰ���㣬2=��ȥ��һ���㣬3=��ȥϵͳ�ַ�
	
	//40 YY_CODEBAR
	unsigned char CODEBAR_ON;
	unsigned char CODEBAR_VALIDATION;
	unsigned char CODEBAR_VALIDATION_OUT;
	unsigned char CODEBAR_START_END;		//��ʼ��ֹ����0=�����ͣ�1=ABCD/ABCD��2=abcd/abcd��3=ABCD/TN*E��4=abcd/tn*e
	unsigned char CODEBAR_MIN_LEN;
	
	//45 YY_CODE128
	unsigned char CODE128_ON;
	unsigned char CODE128_MIN_LEN;
	unsigned char CODE128_EXPANSION;		//��չ���ܣ�0=�رգ�1=��ȥǰ���㣬2=��ȥ��һ���㣬3=��ȥϵͳ�ַ�
	
	//48 YY_CODE39
	unsigned char CODE39_ON;
	unsigned char CODE39_VALIDATION;
	unsigned char CODE39_VALIDATION_OUT;
	unsigned char CODE39_DATAFORMAT;		//���ݴ��͸�ʽ��0=��׼��1=ȫASCII��2=תCODE32���
	unsigned char CODE39_START_END;			//��ʼ��ֹ����0=�����ͣ�1=����
	unsigned char CODE39_MIN_LEN;
	
	//54 YY_CODE93
	unsigned char CODE93_ON;
	unsigned char CODE93_VALIDATION;
	unsigned char CODE93_MIN_LEN;
	
	//57 YY_CODE11
	unsigned char CODE11_ON;
	unsigned char CODE11_VALIDATION;		//У�飺0=�رգ�1=1λ��2=2λ
	unsigned char CODE11_VALIDATION_OUT;	//У�������0=����У��أ�1=��1λ��2=��2λ��3=����
	unsigned char CODE11_MIN_LEN;
	
	//61 YY_IL25
	unsigned char IL25_ON;
	unsigned char IL25_VALIDATION;			//У�飺0=�رգ�1=����USS��2=����OPCC��3=����BradescoУ�鲢ת��
	unsigned char IL25_VALIDATION_OUT;
	unsigned char IL25_MIN_LEN;
	
	//65 YY_I25
	unsigned char I25_ON;
	unsigned char I25_MIN_LEN;
	
	//67 YY_M25
	unsigned char M25_ON;
	unsigned char M25_VALIDATION;
	unsigned char M25_VALIDATION_OUT;
	unsigned char M25_MIN_LEN;
	
	//71 YY_MSI
	unsigned char MSI_ON;	
	unsigned char MSI_VALIDATION;			//У�飺0=�رգ�1=1λ��MOD10��,2=2λ��MOD10/10��,3=2λ(MOD11/10)
	unsigned char MSI_VALIDATION_OUT;		//У�������0=����У��أ�1=��1λ��2=��2λ��3=����
	unsigned char MSI_MIN_LEN;
	
	//75 YY_ADD
	unsigned char ADD_ON;
	
	//76 GLOBAL
	unsigned char LIGHT_GATE;
	unsigned char KB_TRANS;
	
	//78 DECODER_2D_SETTING
	unsigned char TIMEHOLDON_2D;			//����ʱ�䣺��λ100ms��0��ʾ��YY������һ��
	unsigned char PRIOR_2D;					//���ȼ���0=��YY������
	unsigned char SETTINGCODE_2D;			//������ID
	unsigned char LASEROFF_2D;				//�Զ��رռ���
	
	//82 YY_QR
	unsigned char YY_QR_ON;
	unsigned char YY_QR_45;
	
	//84 GLOBAL
	unsigned char EDGE_REFINE;				//��������֧��
	unsigned char LED_SUPPLEMENT;			//����ƣ�0=�رգ�1=�ڲ���2=�ⲿ��3=��+��
	unsigned char LED_AIMING;				//��׼�ƣ�0=�رգ�1=����
	unsigned char TRIG_MODE;				//����ģʽ��0=����������1=����+���ڣ�2=����+����+����ͷ
	unsigned char TRIG_TIME;				//����ʱ�䣺0=���ޣ�1=5�룬2=10�룬6=30��
	unsigned char TRIG_THRES;				//���ȴ�����ֵ
	unsigned char TABLE_FITIN;				//�Զ����0�رգ�1-255��ʾTAB�����ʱ����λ1ms
	unsigned char RDY_SOUND;				//������ʾ����0�رգ�1����
	unsigned char HEAD_CORRECT;				//������ʼ������0�رգ�1����
	unsigned char LOCK_REFINE;				//���ean��upca��������ǿ��0�رգ�1����
	
	//94 YY_PHARMA
	unsigned char PHARMA_ON;
	unsigned char PHARMA_MODE;
	unsigned char PHARMA_MIN_BAR;
	unsigned char PHARMA_MIN_LEN;
	
	//98 RESERVED
	unsigned char RESERVED[28];
	
	//126 YY_EDIT
	unsigned short EDIT_MASK;				//�༭����Щ������Ч��bit0=EAN13��bit2=EAN8
	YyEditSet EDIT_SET[4];					//�༭�������4��
	
}YyDecoderSet;

extern YySymbol yy_symbol;					//������
extern YyDecoderSet yy_decset;				//�ڲ�ȫ��������

void yy_sus_init(YyStartEnd *s);
int yy_sus_expansion(YyStartEnd *yy_start_end, unsigned short j);
int yy_sus_find(unsigned short *widstream, YyStartEnd *yy_start_end);
void yy_sus_reverse(YyInterface *yy_interface);
void yy_global_id(YySymbol *yy_symbol);
int yy_decode_e(int e, int s, int n);
int yy_decode_e_pack(unsigned short *widstream, int widstreamlength , unsigned char *bufferout, int buffersize, int patternnum, int intervalstep);
int yy_decode_match(unsigned char *code, int idx, int pack, int shift, unsigned char type);
unsigned char yy_check(unsigned char *Result, unsigned char Number, unsigned char ResultMode, unsigned char DecoderNum);

#endif


