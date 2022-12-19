#include"stdafx.h"
#include "CommandParser.h"
#include "MessageFrame.h"
#include "BytesBuffer.h"

const std::set<uint32_t> CommandParser::CMD_NAME =
{
	//共用
	SELF_CHECK_CMD,	              //设备自检
	SYSTEM_TIME_CALIBRATION_CMD,  //系统校时
	DEVICE_RESET_CMD,	          //设备复位
	LOG_CMD,	                  //日志生成清空
	BEAT_CMD,	                  //心跳
	DEVICE_INFO_CMD,        //单机信息获取
	IP_CMD,			        //IP地址获取
	SELF_CHECK_REPLY,	    //上报自检结果
	RESPONSE_REPLY,	        //上报应答指令
	DEVICE_TIME_REPLY,	    //上报设备时间
	BEAT_REPLY,	            //心跳应答
	DEVICE_INFO_REPLY,      //上报单机信息
	IP_REPLY,			    //上报本机地址
	//电磁频谱检测
	START_PANORAMA_CMD,	    //启动全景搜索任务
	STOP_PANORAMA_CMD,	    //停止全景搜索任务
	PANORAMA_DATA_REPLY,    //上报全景数据
	PA_DEVICE_STATUS_REPLY, //上报设备状态信息
	//电磁目标凝视
	START_IF_CMD,	        //启动中频数据采集任务
	STOP_IF_CMD,	        //停止中频数据采集任务
	IF_FILE_REPLY,	        //上报中频数据采集文件
	IF_DEVICE_STATUS_REPLY, //上报设备状态信息
};

CommandParser::CommandParser(AutoReplyHandler autoReplyHandler, const ReceiveCallBacks& cpcbs)
	:m_cpcbs(cpcbs),
	m_autoReplyHandler(autoReplyHandler),
	m_buffer(std::make_unique<BytesBuffer>())
{
}

CommandParser::~CommandParser()
{
}

void CommandParser::inputRawData(const RawData& db)
{
	if (db.len < 0 || db.addr == nullptr)
		return;
	std::vector<char> recBuf(db.len);
	memcpy(recBuf.data(), db.addr, db.len);
	auto frameList = m_buffer->inputBuffer(recBuf);
	for (auto frame : frameList)
	{
		if (isCmdFrame(frame->infoType()))
		{
			extractDataBlock(frame);
		}
	}
}

bool CommandParser::extractDataBlock(std::shared_ptr<MessageFrame> frame)
{
	if (frame == nullptr)
		return false;
	std::vector<char> dataBlock;
	if (!frame->extractBlock(dataBlock))
		return false;
	switch (frame->infoType())
	{
	case START_PANORAMA_CMD:
		parserStartPanacmd(dataBlock);
		break;
	case STOP_PANORAMA_CMD:
		parserStopPanacmd(dataBlock);
		break;
	case START_IF_CMD:
		parserStartIFcmd(dataBlock);
		break;
	case STOP_IF_CMD:
		parserStopIFcmd(dataBlock);
		break;
	case BEAT_CMD:
		parserBeatcmd(dataBlock);
		break;
	case SYSTEM_TIME_CALIBRATION_CMD:
		parserSystemTimecmd(dataBlock);
		break;
	case DEVICE_INFO_CMD:
		parserDevcieInfocmd(dataBlock);
		break;
	case IP_CMD:
		parserDevcieIPcmd(dataBlock);
		break;
	case LOG_CMD:
		parserLogCmd(dataBlock);
		break;
	default:
		break;
	}
	return true;
}

void CommandParser::parserStartPanacmd(const std::vector<char>& data)
{
	StartPanoCmd cmd;
	memcpy(&cmd, data.data(), sizeof(StartPanoCmd));
	auto taskParam = cmd.startpanoParam;
	m_cpcbs.startPana(taskParam);
}

void CommandParser::parserStopPanacmd(const std::vector<char>& data)
{
	StopPanoCmd cmd;
	memcpy(&cmd, data.data(), sizeof(StopPanoCmd));
	if (data.size() < cmd.stopPanoParam.infCount)
		return;
	auto taskParam = cmd.stopPanoParam;
	std::vector<unsigned short> channel(taskParam.infCount);
	memcpy(channel.data(), data.data() + sizeof(MsgHeader) + offsetof(StopTaskParam, channel), taskParam.infCount * sizeof(unsigned short));
	taskParam.channel = channel.data();
	m_cpcbs.stopPana(taskParam);
}

void CommandParser::parserStartIFcmd(const std::vector<char>& data)
{
	StartIFCmd cmd;
	memcpy(&cmd, data.data(), sizeof(StartIFCmd));
	auto taskParam = cmd.startIFParam;
	auto offsetifParam = sizeof(MsgHeader) + offsetof(StartIFParam, ifParam);
	auto parambytesize = taskParam.segmentNumber * sizeof(IFParam);
	if (data.size() < parambytesize)
		return;
	std::vector<IFParam> params(taskParam.segmentNumber);
	memcpy(params.data(), data.data() + offsetifParam, parambytesize);
	taskParam.ifParam = params.data();
	unsigned short dataReportMethod; //数据回送方式
	auto offsetReportMethod = offsetifParam + parambytesize;
	memcpy(&dataReportMethod, data.data() + offsetReportMethod, sizeof(unsigned short));
	taskParam.dataReportMethod = dataReportMethod;
	unsigned short fileNameLength; //文件名长度
	auto offsetfileNameLength = offsetReportMethod + sizeof(unsigned short);
	memcpy(&fileNameLength, data.data() + offsetfileNameLength, sizeof(unsigned short));
	if (fileNameLength <= 200)
	{
		std::vector<char> fileName(fileNameLength);//文件名
		auto offsetfileName = offsetfileNameLength + sizeof(unsigned short);
		memcpy(fileName.data(), data.data() + offsetfileName, fileNameLength);
		taskParam.fileNameLength = fileNameLength;
		taskParam.fileName = fileName.data();
	}
	m_cpcbs.startIF(taskParam);
}

void CommandParser::parserStopIFcmd(const std::vector<char>& data)
{
	StopIFCmd cmd;
	memcpy(&cmd, data.data(), sizeof(StopIFCmd));
	if (data.size() < cmd.stopIFParam.infCount)
		return;
	auto taskParam = cmd.stopIFParam;
	std::vector<unsigned short> channel(taskParam.infCount);
	memcpy(channel.data(), data.data() + sizeof(MsgHeader) + offsetof(StopTaskParam, channel), taskParam.infCount * sizeof(unsigned short));
	taskParam.channel = channel.data();
	m_cpcbs.stopIF(taskParam);
}

void CommandParser::parserBeatcmd(const std::vector<char>& data)
{
	HeartCmd cmd;
	cmd.heart = 1;
	memcpy(&cmd, data.data(), sizeof(HeartCmd));
	m_autoReplyHandler(BEAT_REPLY, SystemDeviceCode{});
}

void CommandParser::parserSystemTimecmd(const std::vector<char>& data)
{
	SystemTimeCalibCmd cmd;
	memcpy(&cmd, data.data(), sizeof(SystemTimeCalibCmd));
	m_autoReplyHandler(DEVICE_TIME_REPLY, SystemDeviceCode{});
}

//单机信息获取
void CommandParser::parserDevcieInfocmd(const std::vector<char>& data)
{
	DeviceInfoCmd cmd;
	memcpy(&cmd, data.data(), sizeof(DeviceInfoCmd));
	m_autoReplyHandler(DEVICE_INFO_REPLY, cmd.code);
}

void CommandParser::parserDevcieIPcmd(const std::vector<char>& data)
{
	IpAddressCmd cmd;
	memcpy(&cmd, data.data(), sizeof(IpAddressCmd));
	m_autoReplyHandler(IP_REPLY, cmd.code);
}

void CommandParser::parserLogCmd(const std::vector<char>& data)
{
	LogCmd cmd;
	memcpy(&cmd, data.data(), sizeof(LogCmd));
	m_cpcbs.changeLog((LogExecMode)cmd.execMode);
}

bool CommandParser::detectReply(std::shared_ptr<MessageFrame> frame, uint32_t sequenceNum) const
{
	if (frame == nullptr)
		return false;
	return (frame->index() == sequenceNum);
}