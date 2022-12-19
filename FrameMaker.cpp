#include"stdafx.h"
#include "FrameMaker.h"
#include <numeric>
#include "tools.h"

FrameMaker::FrameMaker()
	:m_sequenceNum(1),
	m_iqFrameIndex(1),
	m_udpBlock{ 0,nullptr },
	m_busBlock{ 0,nullptr }
{
	initMessage();
}

FrameMaker::~FrameMaker()
{
}


void FrameMaker::initMessage()
{
	m_sendmessage.header = MSG_HEADER;
	setSourceAdress(41);
	m_sendmessage.domainId = 1;
	m_sendmessage.topicId = 2;
	m_sendmessage.totalPackCounts = 1;
	m_sendmessage.currentPackNum = 1;
	m_sendmessage.version = charToshort(1.0, 25);
	m_sendmessage.checkSum = 0;
	m_sendmessage.tail = MSG_TAIL;
}

DataBlock FrameMaker::makeSelfCheckFrame()
{
	SelfCheckReply selfCheckReply;
	selfCheckReply.msgHeader.infoLength = 50;
	selfCheckReply.msgHeader.taskSeriaNumber = 33;
	selfCheckReply.count = 20;
	selfCheckReply.moudleNumnber = 1;
	selfCheckReply.erroCode = 1;
	selfCheckReply.deviceCode[0] = '1';
	selfCheckReply.deviceCode[1] = '1';
	selfCheckReply.deviceCode[2] = '1';
	selfCheckReply.deviceCode[3] = '1';
	//to Buffer
	std::vector<unsigned char> dataBuf;
	pushBackToBuffer(dataBuf, &selfCheckReply, sizeof(SelfCheckReply));
	m_sendmessage.dataLength = dataBuf.size();
	setSourceAdress(41);
	return makeSendCmdFrame(SELF_CHECK_REPLY, dataBuf);
}

DataBlock FrameMaker::makePanoDataFrame(const PanoData& panoData)
{
	PanoReply panoReply;
	panoReply.dataCount = 1;
	panoReply.panoData = panoData;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &panoReply, offsetof(PanoReply, panoData));
	pushBackToBuffer(dataBuf, &panoData, offsetof(PanoData, systime));
	if (panoData.spectrumLine > 0 && panoData.level)
	{
		pushBackToBuffer(dataBuf, panoData.level, panoData.spectrumLine * sizeof(float));
	}
	pushBackToBuffer(dataBuf, &panoData.systime, offsetof(PanoData, level) - offsetof(PanoData, systime));
	if (panoData.signalCount > 0&& panoData.signalList)
	{
		pushBackToBuffer(dataBuf, panoData.signalList, panoData.signalCount * sizeof(PaSignal));
	}
	auto panoThrdReply = makePanoThrdReply(panoData);
	pushBackToBuffer(dataBuf, &panoThrdReply, offsetof(PanoThrdReply, level));
	if (panoThrdReply.thredLine > 0 && panoThrdReply.level)
	{
		pushBackToBuffer(dataBuf, panoData.level_thd, panoData.spectrumLine_thd * sizeof(float));
	}
	setSourceAdress(41);
	return makeSendCmdFrame(PANORAMA_DATA_REPLY, dataBuf);
}

PanoThrdReply FrameMaker::makePanoThrdReply(const PanoData& panoData)
{
	PanoThrdReply panoThrdReply;
	panoThrdReply.dataCount = 1;
	panoThrdReply.startFrequency = panoData.startFrequency;
	panoThrdReply.bandWidth = panoData.bandWidth;
	panoThrdReply.msgHeader.infoLength = offsetof(PanoThrdReply, thredLine) + panoThrdReply.thredLine * sizeof(float);
	panoThrdReply.msgHeader.taskSeriaNumber = getCurrentUtcInt64();
	panoThrdReply.thredLine = panoData.spectrumLine_thd;
	panoThrdReply.level = panoData.level_thd;
	return panoThrdReply;
}

DataBlock FrameMaker::makeIQDataFrame(const IQData& dataPack)
{
	if (dataPack.packbytes < 0)
		return{ 0, nullptr };
	IQMessage iqMessage;
	iqMessage.header = IQ_frameHeader;
	iqMessage.length = offsetof(IQMessage, pack) + dataPack.packbytes;
	iqMessage.index = m_iqFrameIndex++;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &dataPack, offsetof(IQData, dataPtr));
	if (dataPack.dataBytes > 0&& dataPack.dataPtr)
	{
		pushBackToBuffer(dataBuf, dataPack.dataPtr, dataPack.dataBytes);
	}
	auto offsetpack = offsetof(IQMessage, pack);
	m_udpBuf.resize(iqMessage.length);
	memcpy(m_udpBuf.data(), &iqMessage, offsetpack);
	memcpy(m_udpBuf.data() + offsetpack, dataBuf.data(), dataBuf.size());
	m_udpBlock.len = m_udpBuf.size();
	m_udpBlock.addr = m_udpBuf.data();
	return m_udpBlock;
}

DataBlock FrameMaker::makeTaskCmdReplyFrame(const uint32_t infoType, const uint32_t erroType)
{
	CmdReply cmdReply;
	cmdReply.infoType = infoType;
	cmdReply.erroType = erroType;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &cmdReply, sizeof(CmdReply));
	setSourceAdress(41);
	return makeSendCmdFrame(RESPONSE_REPLY, dataBuf);
}

DataBlock FrameMaker::makeBeatReplyFrame()
{
	HeartReply reply;
	reply.heart = 1;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &reply, sizeof(HeartReply));
	setSourceAdress(41);
	return makeSendCmdFrame(BEAT_REPLY, dataBuf);
}

DataBlock FrameMaker::makeDevcieStatusReplyFrame(const TrDevice& dev)
{
	DeviceStatusReply reply;
	reply.count = 1;
	reply.deviceStatus = dev.status;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &reply, sizeof(DeviceStatusReply));
	uint32_t cmdId = (dev.name == "4100_3900") ? IF_DEVICE_STATUS_REPLY : PA_DEVICE_STATUS_REPLY;
	setSourceAdress(41);
	return makeSendCmdFrame(cmdId, dataBuf);
}

DataBlock FrameMaker::makeDeviceTimeReplyFrame()
{
	DeviceTimeReply reply;
	reply.time = getSystemTime();
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &reply, sizeof(DeviceTimeReply));
	setSourceAdress(41);
	return makeSendCmdFrame(DEVICE_TIME_REPLY, dataBuf);
}

DataBlock FrameMaker::makeDeviceInfoReplyFrame(const SystemDeviceCode& code)
{
	DeviceInfoReply reply;
	reply.code = code;
	reply.version[0] = '1';
	reply.version[1] = '0';
	reply.version[2] = '1';
	reply.time = getSystemTime();
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &reply, sizeof(DeviceInfoReply));
	setSourceAdress(41);
	return makeSendCmdFrame(DEVICE_INFO_REPLY, dataBuf);
}

DataBlock FrameMaker::makeDeviceIPReplyFrame(const std::string& ip, const SystemDeviceCode& code)
{
	IpAddressReply reply;
	reply.code = code;
	auto ipchar = strIpToChars(ip);
	for (int i = 0; i < 4; i++)
	{
		reply.ip[i] = ipchar[i];
	}
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &reply, sizeof(IpAddressReply));
	setSourceAdress(41);
	return makeSendCmdFrame(IP_REPLY, dataBuf);
}

DataBlock FrameMaker::makeLogCmdFrame()
{
	m_sequenceNum--;
	LogCmd cmd;
	cmd.execMode = 2;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &cmd, sizeof(LogCmd));
	setSourceAdress(36);
	return makeSendCmdFrame(LOG_CMD, dataBuf);
}

DataBlock FrameMaker::makeStartPanoCmdFrame()
{
	m_sequenceNum--;
	StartPanoCmd cmd;
	cmd.startpanoParam.startFrequency = 20;
	cmd.startpanoParam.bandWidth = 40;
	cmd.startpanoParam.resolutionBW = 2800;
	cmd.startpanoParam.detectMode = 2;
	cmd.startpanoParam.spectrumLine = 2048;
	cmd.startpanoParam.interval = 400;
	cmd.startpanoParam.dataReport = 2;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &cmd, sizeof(StartPanoCmd));
	setSourceAdress(36);
	return makeSendCmdFrame(START_PANORAMA_CMD, dataBuf);
}

DataBlock FrameMaker::makeStopPanoCmdFrame()
{
	m_sequenceNum--;
	StopPanoCmd cmd;
	cmd.stopPanoParam.infCount = 1;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &cmd, sizeof(StopPanoCmd));
	setSourceAdress(36);
	return makeSendCmdFrame(STOP_PANORAMA_CMD, dataBuf);
}

DataBlock FrameMaker::makeStopIFCmdFrame()
{
	m_sequenceNum--;
	StopIFCmd cmd;
	cmd.stopIFParam.infCount = 1;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &cmd, sizeof(StopIFCmd));
	setSourceAdress(36);
	return makeSendCmdFrame(STOP_IF_CMD, dataBuf);
}

DataBlock FrameMaker::makeStartIFCmdFrame()
{
	m_sequenceNum --;
	StartIFCmd cmd;
	cmd.startIFParam.sweepNumber = 25;
	cmd.startIFParam.blocksNumber = 4;
	cmd.startIFParam.tranferSampleNumber = 1024;
	cmd.startIFParam.timerTiggerType = 0x04;
	cmd.startIFParam.timeStamp = 20;
	cmd.startIFParam.sweepInterval = 1500;
	cmd.startIFParam.segmentInterval = 26;
	cmd.startIFParam.segmentNumber = 1;

	MsgHeader msgHeader;
	paramheader header;
	header.sweepNumber = 25;
	header.blocksNumber = 4;
	header.tranferSampleNumber = 1024;
	header.timerTiggerType = 0x04;
	header.timeStamp = 20;
	header.sweepInterval = 1500;
	header.segmentInterval = 26;
	header.segmentNumber = 1;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &msgHeader, sizeof(MsgHeader));
	pushBackToBuffer(dataBuf, &header, sizeof(paramheader));
	IFParam param;
	param.centerFreq = 20;     
	param.sampleRate = 28000;      
	param.attenuationGain = 1;   
	param.antenna = 1;          
	param.tiggerType = 0; 
	param.tiggerValue = -20;  
	pushBackToBuffer(dataBuf, &param, sizeof(IFParam));
	unsigned short dataReportMethod=2; 
	pushBackToBuffer(dataBuf, &dataReportMethod, sizeof(unsigned short));
	unsigned short fileNameLength=3;
	pushBackToBuffer(dataBuf, &fileNameLength, sizeof(unsigned short));
	std::vector<char> fileName = { 'w','k','d' };
	pushBackToBuffer(dataBuf, fileName.data(), sizeof(fileName));
	setSourceAdress(36);
	return makeSendCmdFrame(START_IF_CMD, dataBuf);
}

DataBlock FrameMaker::makeDevcieInfoCmdFrame()
{
	m_sequenceNum--;
	DeviceInfoCmd cmd;
	SystemDeviceCode code;
	code.systemCodeLength = 4;
	code.systemCode[0] = 'A';
	code.systemCode[1] = '1';
	code.systemCode[2] = '1';
	code.systemCode[3] = '2';
	code.deviceCodeLength = 4;
	code.deviceCodeLength = 9;
	code.deviceCode[0] = '4';
	code.deviceCode[1] = '1';
	code.deviceCode[2] = '0';
	code.deviceCode[3] = '0';
	code.deviceCode[4] = '_';
	code.deviceCode[5] = '3';
	code.deviceCode[6] = '9';
	code.deviceCode[7] = '0';
	code.deviceCode[8] = '0';
	cmd.code = code;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &cmd, sizeof(DeviceInfoCmd));
	setSourceAdress(36);
	return makeSendCmdFrame(DEVICE_INFO_CMD, dataBuf);
}

DataBlock FrameMaker::makeDevcieIpCmdFrame()
{
	m_sequenceNum--;
	IpAddressCmd cmd;
	SystemDeviceCode code;
	code.systemCodeLength = 4;
	code.systemCode[0] = 'A';
	code.systemCode[1] = '1';
	code.systemCode[2] = '1';
	code.systemCode[3] = '2';
	code.deviceCodeLength = 9;
	code.deviceCode[0] = '4';
	code.deviceCode[1] = '1';
	code.deviceCode[2] = '0';
	code.deviceCode[3] = '0';
	code.deviceCode[4] = '_';
	code.deviceCode[5] = '3';
	code.deviceCode[6] = '9';
	code.deviceCode[7] = '0';
	code.deviceCode[8] = '0';
	cmd.code = code;
	std::vector<BYTE> dataBuf;
	pushBackToBuffer(dataBuf, &cmd, sizeof(IpAddressCmd));
	setSourceAdress(36);
	return makeSendCmdFrame(IP_CMD, dataBuf);
}

DataBlock FrameMaker::makeSendCmdFrame(const uint32_t cmdId, std::vector<unsigned char>& dataBuf)
{
	fillMsgHeader(dataBuf);
	m_sendmessage.dataLength = dataBuf.size();
	m_sendmessage.infoType = cmdId;
	m_sendmessage.sequenceNum = m_sequenceNum;
	m_sequenceNum++;
	auto offsetBlock = offsetof(Message, dataBlock);
	auto tailSize = sizeof(Message) - offsetof(Message, checkSum);
	m_sendmessage.length = offsetBlock + m_sendmessage.dataLength + tailSize;
	m_sendmessage.sendTime = current64Time();
	m_busBuf.resize(m_sendmessage.length);   
	memcpy(m_busBuf.data(), &m_sendmessage, offsetBlock); //报文头-保留字段   
	memcpy(m_busBuf.data() + offsetBlock, dataBuf.data(), dataBuf.size());//报文内容
	m_sendmessage.checkSum = calculateChecksum();
	memcpy(m_busBuf.data() + offsetBlock + m_sendmessage.dataLength, &m_sendmessage.checkSum, tailSize);//校验和-报文尾	
	m_busBlock.len = m_busBuf.size();
	m_busBlock.addr = m_busBuf.data();
	return m_busBlock;
}

void FrameMaker::fillMsgHeader(std::vector<BYTE>& dataBuf)
{
	MsgHeader msgHeader;
	msgHeader.infoLength = dataBuf.size();
	msgHeader.taskSeriaNumber = getCurrentUtcInt64();
	memcpy(dataBuf.data(), &msgHeader, sizeof(MsgHeader));
}

uint32_t FrameMaker::getsequenceNum()
{
	return m_sequenceNum - 1;
}

void FrameMaker::setSourceAdress(unsigned short source)
{
	m_sendmessage.sourceAdress = source;
	m_sendmessage.destAdress = (source == 41) ? 36 : 41;
}

transType FrameMaker::calculateChecksum()
{	
	if (m_sendmessage.length - 2 * sizeof(transType) < 0)
		return 0;
	std::vector<char> buffer(m_sendmessage.length - 2*sizeof(transType));
	memcpy(buffer.data(), m_busBuf.data(), buffer.size());
	return std::accumulate(buffer.begin(), buffer.end(), 0);
}