#pragma once
/**
* @brief ¥¥Ω®∑¢ÀÕ√¸¡Ó÷°
* @details
*/
#include "CmdAndReplyDef.h"

class FrameMaker
{
public:
	FrameMaker();
	~FrameMaker();

public:
	DataBlock makeIQDataFrame(const IQData& dataPack);
	DataBlock makeSelfCheckFrame();
	DataBlock makePanoDataFrame(const PanoData& panoData);
	DataBlock makeTaskCmdReplyFrame(const uint32_t infoType, const uint32_t erroType);
	DataBlock makeBeatReplyFrame();
	DataBlock makeDeviceTimeReplyFrame();
	DataBlock makeDevcieStatusReplyFrame(const TrDevice& dev);
	DataBlock makeDeviceInfoReplyFrame(const SystemDeviceCode& code);
	DataBlock makeDeviceIPReplyFrame(const std::string& ip, const SystemDeviceCode& code);
	uint32_t getsequenceNum();
	DataBlock makeLogCmdFrame();
	DataBlock makeStartPanoCmdFrame();
	DataBlock makeStopPanoCmdFrame();
	DataBlock makeStartIFCmdFrame();
	DataBlock makeStopIFCmdFrame();
	DataBlock makeDevcieInfoCmdFrame();
	DataBlock makeDevcieIpCmdFrame();

private:
	void initMessage();
	void setSourceAdress(unsigned short source);
	DataBlock makeSendCmdFrame(const uint32_t cmdId, std::vector<unsigned char>& dataBuf);
	PanoThrdReply makePanoThrdReply(const PanoData& panoData);
	transType calculateChecksum();
	void fillMsgHeader(std::vector<BYTE>& dataBuf);

private:
	uint32_t m_sequenceNum;
	int64_t m_iqFrameIndex;
	DataBlock m_udpBlock;
	DataBlock m_busBlock;
	Message m_sendmessage;
	std::vector<char> m_udpBuf;
	std::vector<char> m_busBuf;
};