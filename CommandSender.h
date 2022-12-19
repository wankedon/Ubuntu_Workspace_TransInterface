#pragma once
#include "CmdAndReplyDef.h"
#include <mutex>

using UdpPackageToSend = std::vector<char>;
using BusPackageToSend = std::pair<std::vector<char>, int>;
using SendByUdpHandler = std::function<void(const std::vector<char>& buffer)>;

class FrameMaker;
class CommandSender
{
public:
	CommandSender(const SendByUdpHandler& sendByUdp, const BusSendCallBack& bscb);
	~CommandSender();

public:
	void sendPanoData(const PanoData& panoData);
	void sendIQData(const IQData& dataPack);
	void sendTaskCmdReply(const uint32_t infoType, const uint32_t erroType);
	void sendAutoReply(uint32_t infoType, const SystemDeviceCode& devCode);
	void updateDevcie(const DevList& devList);
	void sendBeat();
	void sendDevcieStatus();

private:
	void sendDeviceTime();
	void sendDeviceInfo(const SystemDeviceCode& devCode);
	void sendDeviceIP(const SystemDeviceCode& devCode);
	void sendDataByBus(const DataBlock& db, const int infoType);
private:
	SendByUdpHandler m_sendByUdpHandler;
	BusSendCallBack m_bscb;
	std::unique_ptr<FrameMaker> m_frameMaker;
	std::map<std::string, TrDevice> m_devcies;
	std::vector<TrDevice> m_devList;
};