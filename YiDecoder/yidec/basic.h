
#ifndef _YY_BASIC_H_
#define _YY_BASIC_H_

#include "stdio.h"
#include "string.h"

#define YY_MAX_DAT_NUM 768				//原始数据最大个数
#define YY_MAX_MIN_MUL 10				//有效数据的最小数据与最大数据的最高倍数关系若取值10 则表示最大数据与最小数据倍数不得超过10倍

#define YY_MAX_SYM_LEN 256				//解码结果最大个数

#define YY_SUS_MAX_NUM 1024				//定义从原始数据缓存区取得的疑似条码区域的个数上限
#define YY_SUS_MIN_LEN 12				//定义疑似条码条空个数下限
#define YY_SUS_MAX_LEN YY_MAX_DAT_NUM	//定义疑似条码条空个数上限 1525

#define YY_VAL_MAX_NUM 12				//定义输出池大小

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

typedef struct	//定义疑似条码缓存区的起始和终止数据格式(附带左右空白区数据)
{
	unsigned short head;	//头位置
	unsigned short len;		//个数
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
	unsigned char edit_insert[14];			//插入，第一字节为位置，从1开始，0表示不启用，0xFF表示尾部插入
	unsigned char edit_delete[2];			//删除，第一字节为位置，第二字节为个数
	unsigned char edit_repsrc[8];			//替换源
	unsigned char edit_repdst[8];			//替换值
}YyEditSet;

typedef struct
{
	//0 GROBAL
	unsigned char CONTINUOUS;				//连续模式：0=关闭，1=连续输出，2=连续不输出
	unsigned char MULTIVALID;				//多重校验：1=1重校验，2=2重校验，最多5，bit[7]表示不同码制互锁，bit[6]表示EAN13和UPCA免多重校验
	unsigned char TIMETOLIVE;				//校验时限：单位100ms
	unsigned char TIMEHOLDON;				//锁定时间：单位100ms
	unsigned char MULTILINES;				//多线设置：0=来回扫，2=20线，6=60线，10=100线
	unsigned char SOUND_ON;					//声音设置：高四位音调，低四位时长，0xFF=智能播报模式
	unsigned char SLEEP_TIME;				//睡眠时间：0=关闭，单位15s
	unsigned char TAILEDCHAR;				//结束字符：\n或\t
	
	//8 GLOBAL_ID
	unsigned char ID_ON_LR;					//全局字符：0=关闭，1=左，2=右
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
	unsigned char UPCA_TRANS;				//转换功能：0=关闭，1=转换EAN13
	unsigned char UPCA_EXPANSION;			//扩展功能：0=关闭，1=截去前导零，2=截去第一个零，3=截去系统字符
	
	//35 YY_UPCE
	unsigned char UPCE_ON;
	unsigned char UPCE_VALIDATION;
	unsigned char UPCE_VALIDATION_OUT;
	unsigned char UPCE_TRANS;				//扩展功能：0=关闭，1=转换EAN13，2=扩展UPCA
	unsigned char UPCE_EXPANSION;			//扩展功能：0=关闭，1=截去前导零，2=截去第一个零，3=截去系统字符
	
	//40 YY_CODEBAR
	unsigned char CODEBAR_ON;
	unsigned char CODEBAR_VALIDATION;
	unsigned char CODEBAR_VALIDATION_OUT;
	unsigned char CODEBAR_START_END;		//起始终止符：0=不传送，1=ABCD/ABCD，2=abcd/abcd，3=ABCD/TN*E，4=abcd/tn*e
	unsigned char CODEBAR_MIN_LEN;
	
	//45 YY_CODE128
	unsigned char CODE128_ON;
	unsigned char CODE128_MIN_LEN;
	unsigned char CODE128_EXPANSION;		//扩展功能：0=关闭，1=截去前导零，2=截去第一个零，3=截去系统字符
	
	//48 YY_CODE39
	unsigned char CODE39_ON;
	unsigned char CODE39_VALIDATION;
	unsigned char CODE39_VALIDATION_OUT;
	unsigned char CODE39_DATAFORMAT;		//数据传送格式：0=标准，1=全ASCII，2=转CODE32输出
	unsigned char CODE39_START_END;			//起始终止符：0=不传送，1=传送
	unsigned char CODE39_MIN_LEN;
	
	//54 YY_CODE93
	unsigned char CODE93_ON;
	unsigned char CODE93_VALIDATION;
	unsigned char CODE93_MIN_LEN;
	
	//57 YY_CODE11
	unsigned char CODE11_ON;
	unsigned char CODE11_VALIDATION;		//校验：0=关闭，1=1位，2=2位
	unsigned char CODE11_VALIDATION_OUT;	//校验输出：0=根据校验关，1=关1位，2=关2位，3=不关
	unsigned char CODE11_MIN_LEN;
	
	//61 YY_IL25
	unsigned char IL25_ON;
	unsigned char IL25_VALIDATION;			//校验：0=关闭，1=开启USS，2=开启OPCC，3=开启Bradesco校验并转换
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
	unsigned char MSI_VALIDATION;			//校验：0=关闭，1=1位（MOD10）,2=2位（MOD10/10）,3=2位(MOD11/10)
	unsigned char MSI_VALIDATION_OUT;		//校验输出：0=根据校验关，1=关1位，2=关2位，3=不关
	unsigned char MSI_MIN_LEN;
	
	//75 YY_ADD
	unsigned char ADD_ON;
	
	//76 GLOBAL
	unsigned char LIGHT_GATE;
	unsigned char KB_TRANS;
	
	//78 DECODER_2D_SETTING
	unsigned char TIMEHOLDON_2D;			//锁定时间：单位100ms，0表示与YY解码器一致
	unsigned char PRIOR_2D;					//优先级，0=仅YY解码器
	unsigned char SETTINGCODE_2D;			//设置码ID
	unsigned char LASEROFF_2D;				//自动关闭激光
	
	//82 YY_QR
	unsigned char YY_QR_ON;
	unsigned char YY_QR_45;
	
	//84 GLOBAL
	unsigned char EDGE_REFINE;				//带框条码支持
	unsigned char LED_SUPPLEMENT;			//补光灯：0=关闭，1=内部，2=外部，3=内+外
	unsigned char LED_AIMING;				//瞄准灯：0=关闭，1=开启
	unsigned char TRIG_MODE;				//触发模式：0=连续工作，1=按键+串口，2=按键+串口+摄像头
	unsigned char TRIG_TIME;				//触发时间：0=无限，1=5秒，2=10秒，6=30秒
	unsigned char TRIG_THRES;				//亮度触发阈值
	unsigned char TABLE_FITIN;				//自动填表，0关闭，1-255表示TAB后的延时，单位1ms
	unsigned char RDY_SOUND;				//开机提示音，0关闭，1开启
	unsigned char HEAD_CORRECT;				//条码起始修正，0关闭，1开启
	unsigned char LOCK_REFINE;				//针对ean和upca的锁定增强，0关闭，1开启
	
	//94 YY_PHARMA
	unsigned char PHARMA_ON;
	unsigned char PHARMA_MODE;
	unsigned char PHARMA_MIN_BAR;
	unsigned char PHARMA_MIN_LEN;
	
	//98 RESERVED
	unsigned char RESERVED[28];
	
	//126 YY_EDIT
	unsigned short EDIT_MASK;				//编辑对哪些码制生效，bit0=EAN13，bit2=EAN8
	YyEditSet EDIT_SET[4];					//编辑设置项，共4轮
	
}YyDecoderSet;

extern YySymbol yy_symbol;					//输出结果
extern YyDecoderSet yy_decset;				//内部全局设置项

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


