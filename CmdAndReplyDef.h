#pragma once
#include "TransComDef.h"

enum CommandID
{
	MSG_HEADER = 0X7E8118E7,        //报文头
	MSG_TAIL = 0X8F9009F8,          //报文尾
	//共用
	SELF_CHECK_CMD = 0X1A00,	    //设备自检
	SYSTEM_TIME_CALIBRATION_CMD = 0X1A01,	//系统校时
	DEVICE_RESET_CMD = 0X1A02,	    //设备复位
	LOG_CMD = 0X1A03,	            //日志生成清空
	BEAT_CMD = 0X1A04,	            //心跳
	DEVICE_INFO_CMD = 0X1A0E,       //单机信息获取
	IP_CMD = 0X1A0F,			    //IP地址获取
	SELF_CHECK_REPLY = 0XA100,	    //上报自检结果
	RESPONSE_REPLY = 0XA101,	    //上报应答指令
	DEVICE_TIME_REPLY = 0XA103,	    //上报设备时间
	BEAT_REPLY = 0XA104,	        //心跳应答
	DEVICE_INFO_REPLY = 0XA10E,     //上报单机信息
	IP_REPLY = 0XA10F,			    //上报本机地址
	//电磁频谱检测
	START_PANORAMA_CMD = 0X1B00,	//启动全景搜索任务
	STOP_PANORAMA_CMD = 0X1B01,	    //停止全景搜索任务
	PANORAMA_DATA_REPLY = 0XB100,   //上报全景数据
	PA_DEVICE_STATUS_REPLY = 0XB104,//上报设备状态信息
	//电磁目标凝视
	START_IF_CMD = 0X1B10,	        //启动中频数据采集任务
	STOP_IF_CMD = 0X1B11,	        //停止中频数据采集任务
	IF_FILE_REPLY = 0XB110,	        //上报中频数据采集文件
	IF_DEVICE_STATUS_REPLY = 0XB114,//上报设备状态信息
	//IQ
	IQ_frameHeader = 0x7E7E7E7E,
	IQ_packHeader = 0X7E7E0705,
	IQ_dataHeader = 0X360B,
};

#pragma pack(1)
struct DataBlock
{
	std::size_t len; //长度
	void* addr;	     //地址
};

struct paramheader
{
	uint32_t sweepNumber;            //扫描次数
	uint32_t blocksNumber;           //扫描块数
	uint32_t tranferSampleNumber;    //采集点数
	uint32_t timerTiggerType;        //触发类型
	int64_t timeStamp;               //触发时刻
	uint32_t sweepInterval;          //扫描间隔
	int64_t segmentInterval;         //频点间隔
	uint32_t segmentNumber;          //频点个数
};

struct SystemDeviceCode
{
	transType systemCodeLength;    //系统代号长度
	unsigned char systemCode[20]; //系统代号名
	transType deviceCodeLength;    //单机代号长度
	unsigned char deviceCode[20]; //单机代号名
};

struct IQMessage
{
	int32_t header;      //帧头
	int32_t	length;      //帧长
	int64_t	index;       //帧号
	IQData pack;
};

struct Message
{
	transType header;              //报文头
	transType length;              //当前包数据长度
	unsigned short destAdress;    //目的地址
	unsigned short sourceAdress;  //源地址
	unsigned char domainId;       //域ID
	unsigned char topicId;        //主题ID
	unsigned short infoType;      //信息类别号
	uint64_t  sendTime;           //发报日期时间
	transType sequenceNum;         //序列号
	transType totalPackCounts;     //包总数
	transType currentPackNum;      //当前包号
	transType dataLength;          //数据总长度
	unsigned short version;       //版本号
	char reserved[6];             //保留字段
	BYTE* dataBlock;              //报文内容
	transType checkSum;            //校验和   
	transType tail;                //报文尾
};

struct MsgHeader
{
	transType infoLength;        //信息长度
	int64_t taskSeriaNumber;    //任务流水号
};

struct SelfCheckCmd
{
	MsgHeader msgHeader;
	unsigned char selfCheckType;   //自检类型
	unsigned short selfCheckCycle; //自检周期
	unsigned char selfCheckCount;  //自检数量
	unsigned char deviceId[4];    //被检单机代码
};

struct SystemTimeCalibParam
{
	unsigned char systemTimeType;  //校时模式
	SYSTEMTIME systemTime1;         //时间
};

struct SystemTimeCalibCmd
{
	MsgHeader msgHeader;
	SystemTimeCalibParam sysTimeCalParam;
};

struct LogCmd
{
	MsgHeader msgHeader;
	unsigned char execMode;     //执行方式
};

struct HeartCmd
{
	MsgHeader msgHeader;
	unsigned char heart; //心跳
};

struct DeviceInfoCmd
{
	MsgHeader msgHeader;
	SystemDeviceCode code;//系统设备代号
};

struct IpAddressCmd
{
	MsgHeader msgHeader;
	SystemDeviceCode code;//系统设备代号
};

struct StartPanoCmd
{
	MsgHeader msgHeader;
	StartPanoParam startpanoParam;
};

struct StopPanoCmd
{
	MsgHeader msgHeader;
	StopTaskParam stopPanoParam;
};

struct StartIFCmd
{
	MsgHeader msgHeader;
	StartIFParam startIFParam;
};

struct StopIFCmd
{
	MsgHeader msgHeader;
	StopTaskParam stopIFParam;
};

struct SelfCheckReply
{
	MsgHeader msgHeader;
	unsigned char deviceCode[4];  //被检单机代码
	unsigned char count;           //数量
	unsigned char moudleNumnber;   //模块号
	unsigned short erroCode;       //故障原因编码
};

struct CmdReply
{
	MsgHeader msgHeader;
	unsigned short infoType; //被应答信息类别号
	unsigned short erroType; //错误类型
};

struct DeviceTimeReply
{
	MsgHeader msgHeader;
	SYSTEMTIME time;//时间
};

struct HeartReply
{
	MsgHeader msgHeader;
	unsigned char heart; //心跳
};

struct DeviceInfoReply
{
	MsgHeader msgHeader;
	SystemDeviceCode code;    //系统设备代号
	unsigned char version[3]; //版本号
	SYSTEMTIME time;          //程序编译时间
};

struct IpAddressReply
{
	MsgHeader msgHeader;
	SystemDeviceCode code; //系统设备代号
	unsigned char ip[4];   //ip地址
};

struct PanoThrdReply
{
	MsgHeader msgHeader;        //门限信息头
	unsigned short dataCount;   //门限信息数量
	int64_t startFrequency;     //门限开始频率
	int64_t  bandWidth;         //门限频段宽度
	transType thredLine = 0;    //门限谱线数
	float* level = nullptr;     //门限幅度电平
};

struct PanoReply
{
	MsgHeader msgHeader;
	unsigned short dataCount; //信息数量
	PanoData panoData;
	PanoThrdReply  panoThrdReply; //门限
};

struct DeviceStatusReply
{
	MsgHeader msgHeader;
	unsigned char count;
	TrDeviceStatus deviceStatus;
};
#pragma pack()