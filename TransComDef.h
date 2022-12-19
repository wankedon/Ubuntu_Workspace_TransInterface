#pragma once
#include <stdint.h>
#ifdef PLATFROM_WINDOWS
#include "winsock2.h"
#include <windows.h>
#else
typedef unsigned char BYTE;
#endif
typedef unsigned int transType;

//日志模式
enum LogExecMode
{
	CLEAR_LOG = 0, //清空
	DO_LOG = 1,    //记录
	UNDO_LOG = 2,  //不记录
};

#pragma pack(1)
#ifdef PLATFROM_WINDOWS
#else
struct SYSTEMTIME
{
	short wYear;
	short wMonth;
	short wDayOfWeek;
	short wDay;
	short wHour;
	short wMinute;
	short wSecond;
	short wMilliseconds;
};
#endif

//通过总线发送的数块
struct RawData
{
	int len = 0;          //长度
	char* addr = nullptr; //地址
};

//开始全景任务参数
struct StartPanoParam
{
	int64_t startFrequency;     //开始频率
	int64_t  bandWidth;         //频段宽度
	int64_t  resolutionBW;      //分辨率
	unsigned short detectMode;  //检波模式
	unsigned short spectrumLine;//谱线数
	unsigned short interval;    //回传间隔
	unsigned char dataReport;   //数据上报模式
};

//IQ参数
struct IFParam
{
	int64_t centerFreq;        //中心频率
	int64_t sampleRate;        //采样率
	int32_t attenuationGain;   //增益衰减
	int32_t antenna;           //天线选择
	unsigned short tiggerType; //触发类型
	short tiggerValue;         //触发电平
};

//开始IQ任务参数
struct StartIFParam
{
	uint32_t sweepNumber;            //扫描次数
	uint32_t blocksNumber;           //扫描块数
	uint32_t tranferSampleNumber;    //采集点数
	uint32_t timerTiggerType;        //触发类型
	int64_t timeStamp;               //触发时刻
	uint32_t sweepInterval;          //扫描间隔
	int64_t segmentInterval;         //频点间隔
	uint32_t segmentNumber;          //频点个数
	IFParam* ifParam;                //中频参数
	unsigned short dataReportMethod; //数据回送方式
	unsigned short fileNameLength;   //文件名长度
	char* fileName;                  //文件名
};

//停止全景/IQ任务参数
struct StopTaskParam
{
	unsigned short infCount; //信息数量
	unsigned short* channel; //通道号
};

//设备状态
struct TrDeviceStatus
{
	unsigned char moudleNumnber;    //模块号
	unsigned short electricCurrent; //电流
	unsigned short voltage;         //电压
	unsigned short temperature;     //温度
};

//设备
struct TrDevice
{
	char* name = nullptr;   //设备名称
	char* ip = nullptr;   //ip地址
	TrDeviceStatus status;
};

//设备列表
struct DevList
{
	int deviceSize = 0;
	TrDevice* devicePtr = nullptr;
};

//全景信号
struct PaSignal
{
	int64_t frequency;   //频点
	int64_t  bandWidth;  //带宽
	float maxLevel;      //最大功率
	float minLevel;      //最小功率
	float averageLevel;  //平均功率
	int64_t occurTime;   //发现时间
	int32_t occurCount;  //发现次数
};

//全景数据
struct PanoData
{
	int64_t startFrequency;         //开始频率
	int64_t  bandWidth;             //频段宽度
	transType spectrumLine = 0;      //谱线数
	SYSTEMTIME systime;             //全景采集时刻
	int32_t signalCount = 0;        //信号数量
	float* level = nullptr;         //幅度电平
	PaSignal* signalList = nullptr; //信号
	transType spectrumLine_thd = 0; //门限谱线数
	float* level_thd = nullptr;     //门限幅度电平
};

//IQ数据
struct IQData
{
	int32_t packheader;      //数据包标志
	int32_t packbytes;       //数据包长度
	int64_t packindex;       //数据包号
	int16_t dataheader;      //数据标志
	int16_t ddcId;           //信道号
	int16_t channelType;     //信道类型
	int16_t dataType;        //数据类型
	SYSTEMTIME samplingTime; //采集时间
	float signalFrequ;       //信号频率
	float samplingRate;      //采样频率
	int32_t signalID;        //信号ID
	int32_t dataBytes = 0;   //数据长度
	BYTE* dataPtr = nullptr; //IQ数据
};
#pragma pack()

//界面接收任务控制
typedef void ChangeLogModeHandler(const LogExecMode& mode, void* ptr);    //改变日志模式
typedef uint32_t StartPanoHandler(const StartPanoParam& param, void* ptr);//开始全景
typedef uint32_t StopPanoHandler(const StopTaskParam& param, void* ptr);  //停止全景
typedef uint32_t StartIFHandler(const StartIFParam& param, void* ptr);    //开始IQ
typedef uint32_t StopIFHandler(const StopTaskParam& param, void* ptr);    //停止IQ
struct ControlCallBacks
{
	void* owner = nullptr;
	ChangeLogModeHandler* changeLogMode = nullptr;
	StartPanoHandler* startPano = nullptr;
	StopPanoHandler* stopPano = nullptr;
	StartIFHandler* startIF = nullptr;
	StopIFHandler* stopIF = nullptr;
};

//界面上报任务数据
typedef void ReportPanoDataHandler(const PanoData& panodata, void* ptr);//上报全景数据
typedef void ReportIQDataHandler(const IQData& iqdata, void* ptr);      //上报IQ数据
typedef void ReportDevInfoHandler(const DevList& devcies, void* ptr);   //上报设备信息
struct ReportCallBacks
{
	void* owner = nullptr;
	ReportPanoDataHandler* reportPanoData = nullptr;
	ReportIQDataHandler* reportIQData = nullptr;
	ReportDevInfoHandler* reportDevInfo = nullptr;
};

//Node通过总线发送数据
typedef void SendByBusHandler(char* pData, int nLength, const int infoType, void* ptr);//通过总线发送数据
struct BusSendCallBack
{
	void* owner = nullptr;
	SendByBusHandler* sendByBus = nullptr;
};