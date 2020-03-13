
//ADC mistake	define
#define FaultValueL		20
#define FaultValueH		1010	

//系统参数存放的地址
#define	ADD_BASE_NUM							0x0001

//低报预警数据地址
#define	ADD_hydrothion_LAlarm 		0x0001
#define	ADD_fire_LAlarm						0x0002
#define	ADD_co_LAlarm							0x0003
#define	ADD_o2_LAlarm_Base							0x0004	//4，5，6，7

//高报预警数据地址
#define	ADD_hydrothion_HAlarm 		0x0008
#define	ADD_fire_HAlarm						0x0009
#define	ADD_co_HAlarm							0x000a
#define	ADD_o2_HAlarm_Base							0x000b	 //0x0b，0x0c，0x0d，0x0e

//零点值存放的地址
#define ADD_hydrothion_ZERO				0x000f	// 0x0f 0x10 存放的是16 位的ADC值	
#define	ADD_fire_ZERO							0x0011	// 0x11 0x12
#define	ADD_co_ZERO								0x0013	// 0x13 0x14
#define	ADD_o2_ZERO								0x0015	// 0x15 0x16

//背光设置参数存放位置
#define	ADD_Backlight_Arg					0x0017	

//密码存放位置
#define	ADD_PassWork_Base					0x0018 //0x18, 0x19, 0x1a, 0x1b  密码位置

#define	ADD_Language							0x001C


/////扇区0 中存放元素的个数	

#define	Sector_Argurment_Num			28			

/******************************************************************************************************/
//历史报警记录存放设置
//总共有24 个扇区 每个扇区是512字节
//23号扇区：存放各条记录在eeprom中的位置

//2-6号扇区：存放可燃气体数据  (共5个扇区)
//7-11号扇区：存放CO气体数据	  (共5个扇区)
//12-16号扇区：存放H2S气体数据	(共5个扇区)
//17-23号扇区：存放O2气体数据	(共7个扇区)

#define ADD_FIRE_SP_BASE				0x2C00	//指向可燃气体的位置 （0x2C00 0x2C01）
#define	ADD_C0_SP_BASE					0x2C02 	//指向C0气体的位置		(0x2C02 0x2C03)
#define	ADD_H2S_SP_BASE					0x2C04 	//指向C0气体的位置		(0x2C04 0x2C05)
#define	ADD_O2_SP_BASE					0x2C06 	//指向C0气体的位置		(0x2C06 0x2C07)


#define ADD_FIRE_COUNT_BASE				0x2C08	//记录可燃气体的数量 （0x2C08 0x2C09）
#define	ADD_C0_COUNT_BASE					0x2C0A 	//记录一氧化碳气体的数量	(0x2C0A 0x2C0B)
#define	ADD_H2S_COUNT_BASE				0x2C0C 	//记录硫化氢气体的数量	(0x2C0C 0x2C0D)
#define	ADD_O2_COUNT_BASE					0x2C0E 	//记录氧气气体的数量		(0x2C0E 0x2C0F)


#define		ADD_FIRE_BASE					0x200				//可燃气体的起始位置
#define		ADD_C0_BASE					  0xC00				//一氧化碳的起始位置
#define		ADD_H2S_BASE					0x1600			//硫化氢的起始位置
#define		ADD_O2_BASE						0x2000			//氧气的起始位置



/*****************************************************************************************************/
//最后一个扇区的数据存放格式
//SP的地址
#define	ADD_FIRE_NUM		0
#define	ADD_C0_NUM 			2
#define	ADD_H2S_NUM			4
#define	ADD_02_NUM			6
//数量的地址
#define	ADD_FIRE_COUNT	8
#define	ADD_C0_COUNT		10
#define	ADD_H2S_COUNT		12
#define	ADD_02_COUNT		14

//最后一个扇区中的保存的数据量
#define	Record_Argurment_Num  16




//时间
extern uint16_t tnum;
extern uint16_t tcount;
extern uint16_t shake_num ; 
extern uint16_t sound_delaay_num;		//定时器倒计时计数值


/////////////////////
extern uint8_t Seond ;
extern uint8_t Minute ;
extern uint8_t Hour;	
extern uint8_t Day;	
extern uint8_t Month;
extern uint8_t Year;

//adc采样值
extern uint16_t fire_gas_adc;
extern uint16_t co_gas_adc;
extern uint16_t o2_gas_adc;
extern uint16_t hydrothion_gas_adc;

//按键值
extern uint8_t KeyValue;
extern uint16_t pop_up_time;
extern uint16_t another_pop_time;
extern uint16_t backlight_time;
extern uint16_t begin_time;
//密码
uint8_t PassWordChange_ST;
uint8_t PassWord[4] = {1, 1, 1, 1};
uint8_t PassTemp[4];
uint8_t PassFlag;


//adc 值
uint8_t fire_buffer[10];
uint8_t hydrothion_buffer[10];
uint8_t o2_buffer[10];
uint8_t co_buffer[10];

uint8_t fire_temp;
uint8_t hydrothion_temp;
uint16_t co_temp;
float o2_temp;





uint8_t falue_flag ;	//4路通道的异常状态值
uint8_t Low_alarm_flag;	//4路低浓度报警状态值
uint8_t High_alarm_flag; //4路高浓度报警状态值
uint8_t Handle_sound_flag; //4路声音处理标志位



//系统工作流程
uint8_t SystemWorkSt;			//系统的工作状态指示
enum
{
	IDLE = 0,
	SYSTEM_SEL_TEST,
	NORMAL_WORK_PAGE,
	PASSWORD_PAGE,
	MENU_SET,

	POWER_STATE,
};



//功能菜单流程
uint8_t sub_point = 0;
uint8_t SubMenuWorkSt;			//系统的工作状态指示

enum
{
	SUB_MENU_IDLE = 0,
	SUB_MENU_RECORD,				//记录查看	
	SUB_MENU_LOWALARM,			//低报设置
	SUB_MENU_HIGHTALARM,		//高报设置

	SUB_MENU_CALIBRATION,				//零点设置
	SUB_MENU_SENSOR_ALARM_CNT,	//标定
	SUB_MENU_TIME_SET,					//时间设置

	SUB_MENU_BACKLIGHT,					//背光设置
	SUB_MENU_PASSWORK,					//密码设置
	SUB_MENU_CHANEL,						//通道信息

	SUB_MENU_LNGUAGE,						//语言设置
	
	
	//记录查看
	SUB_H2S_RECORD,
	SUB_02_RECORD,
	SUB_CO_RECORD,
	SUB_FIRE_RECORD,
	
	
	//低报
	SUB_LOWALARM_HySet,
	SUB_LOWALARM_O2Set,
	SUB_LOWALARM_CoSet,
	SUB_LOWALARM_FireSet,
	
	//高报
	SUB_HIGHTALARM_HySet,
	SUB_HIGHTALARM_O2Set,
	SUB_HIGHTALARM_CoSet,
	SUB_HIGHTALARM_FireSet,
	
	//零点
	SUB_ZERO_HySet,
	SUB_ZERO_O2Set,
	SUB_ZERO_CoSet,
	SUB_ZERO_FireSet,	
	
	//密码修改
	SUB_PASSWORK_CHANGE,
	
	
	SUB_CHOSE_END,		//记录最后一项
	SUB_SAVE_WINDOWN,
	
	SUB_RECODR_DELECT,
};


//保存界面上的变量
enum
{
	Nothing = 0,
	hydrothion_LAlarm_flag,
	fire_LAlarm_flag,
	o2_LAlarm_flag,
	co_LAlarm_flag,
	hydrothion_HAlarm_flag,
	fire_HAlarm_flag,
	o2_HAlarm_flag,
	co_HAlarm_flag,
	backlight_arg_flag,
	passwork_flag,
	language_flag,
};







//系统中要保存的参数
	//低保预警参数
uint8_t hydrothion_LAlarm;
uint8_t fire_LAlarm;
uint8_t co_LAlarm;

union
{
	float x;
	uint8_t s[4];
}o2_LAlarm;



//高保预警参数
uint8_t hydrothion_HAlarm;
uint8_t fire_HAlarm;
uint8_t co_HAlarm;

union
{
	float x;
	uint8_t s[4];
}o2_HAlarm;

//原始ADC值
uint16_t original_hydrothion_adc;
uint16_t original_fire_adc;
uint16_t original_co_adc;
uint16_t original_o2_adc;

//背光参数
uint8_t backlight_arg;
uint8_t TS_Story;
		
//语言选择
uint8_t language = 0;
uint8_t La_Story;

enum
{
	Chinese = 0,
	English ,
};

//声音选择
uint8_t sound_chose;

enum
{
	sound_welcom = 2,				//欢迎
	sound_powerup,					//正在启动	
	sound_fire_alarm,				//请注意可燃
	sound_o2_alarm,					//请注意氧气
	sound_hydrothion_alarm,	//请注意硫化氢
	sound_co_alarm,					//请注意一氧化碳
	sound_air,							//请注意气体
	sound_ndcb,							//浓度超标
	sound_ndgd,							//浓度过低
	sound_errorcode,				//密码错误
	sound_success_save,			//保存成功
	sound_fail_save,				//保存失败
	sound_charge_low,				//请注意电量过低请充电
	sound_charge1_low,				//请注意电量过低请充电
	
	sound_english_welcom,		//English welcom
	sound_english_start,		//English start
	sound_english_fire,			//可燃气体
	sound_english_o2,				//氧气
	sound_english_hydrothion,	//硫化氢
	sound_english_co,					//一氧化碳
	sound_english_air,				//气体
	sound_english_ndcb,				//浓度超标
	sound_english_ndgd,				//浓度过低
	sound_english_errorcode,	//密码错误
	sound_english_success_save,	//保存成功
	sound_english_fail_save,		//保存失败
	sound_english_charge_low,		//电量过低
	sound_other,
	sound_ll,
};




//历史记录相关的变量信息
uint16_t fire_sp;	 	//可燃气体记录位置存储变量
uint16_t co_sp;			//一氧化碳气体记录位置变量
uint16_t h2s_sp;		//硫化氢记录位置变量
uint16_t o2_sp;			//氧气记录位置变量

uint8_t max_fire;		//报警过程中最大的数值
uint16_t max_co;		
uint8_t max_h2s;		
float max_o2;
float min_o2;			

uint16_t fire_record_num;	//记录的数量
uint16_t co_record_num;
uint16_t h2s_record_num;
uint16_t o2_record_num;


uint8_t record_statue;
uint16_t sp_record_temp;	//查看记录时用来暂存 sp 的值

//系统运行时间
uint16_t sum_minute; //运行的时间 分钟为单位

uint8_t original_minute;		//起始分钟
uint8_t original_houre;			//起始时
uint8_t original_day;			//起始日
uint8_t original_month;		//起始月


//系统启动状态
uint8_t Selfcheck_System;
enum
{
	ON_IDLE=0,
	WELCOM  ,
	ISBEGINNING,
	SELF_CHECK,
	CHANEL_MEG,
};

//振动的标志位
 uint8_t fflag ;


extern bit F_NewKey;


void Handle_SubMenu(void);

