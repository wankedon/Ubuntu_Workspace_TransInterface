#pragma once
#include "CommandParser.h"
#include "CommandSender.h"

class CommandSender;
class CommandParser;
class Interactor
{
public:
	Interactor(const SendByUdpHandler& sendByUdp, const ReceiveCallBacks& rcbs, const BusSendCallBack& bscb);
	~Interactor();

public:
	void sendBeat();
	void inputRawData(const RawData& buffer);
	void sendPanoData(const PanoData& panoData);
	void sendIQData(const IQData& dataPack);
	void sendTaskCmdReply(const uint32_t infoType, const uint32_t erroType);
	void updateDevcie(const DevList& devcies);

private:
	std::unique_ptr<CommandSender> m_sender;
	std::unique_ptr<CommandParser> m_parser;
};