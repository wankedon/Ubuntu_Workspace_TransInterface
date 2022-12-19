#include"stdafx.h"
#include "CommandSender.h"
#include "FrameMaker.h"
#include "tools.h"
#include "Logger.h"

CommandSender::CommandSender(const SendByUdpHandler& sendByUdp, const BusSendCallBack& bscb)
	:m_sendByUdpHandler(sendByUdp),
	m_bscb(bscb),
	m_frameMaker(std::make_unique<FrameMaker>())
{

}

CommandSender::~CommandSender()
{
	
}

void CommandSender::updateDevcie(const DevList& devList)
{
	m_devList.resize(devList.deviceSize);
	memcpy(m_devList.data(), devList.devicePtr, devList.deviceSize * sizeof(TrDevice));
	if (m_devcies.size() == 0)
	{
		for (auto dev : m_devList)
		{
			m_devcies[dev.name] = dev;
		}
	}
	for (auto dev : m_devList)
	{
		if (m_devcies.find(dev.name) != m_devcies.end())
		{
			m_devcies[dev.name] = dev;
		}
		else
		{
			m_devcies.erase(dev.name);
		}
	}
}

void CommandSender::sendTaskCmdReply(const uint32_t infoType, const uint32_t erroType)
{
	auto db = m_frameMaker->makeTaskCmdReplyFrame(infoType, erroType);
	sendDataByBus(db, RESPONSE_REPLY);
}

void CommandSender::sendPanoData(const PanoData& panoData)
{
	auto db = m_frameMaker->makePanoDataFrame(panoData);
	sendDataByBus(db, PANORAMA_DATA_REPLY);
}

void CommandSender::sendIQData(const IQData& dataPack)
{
	auto db = m_frameMaker->makeIQDataFrame(dataPack);
	std::vector<char> buffer(db.len);
	memcpy(buffer.data(), db.addr, db.len);
	if (m_sendByUdpHandler)
	{
		m_sendByUdpHandler(buffer);
		LOG("sendIQData dataBytes {}", dataPack.dataBytes);
	}
}

void CommandSender::sendAutoReply(uint32_t infoType, const SystemDeviceCode& devCode)
{
	switch (infoType)
	{
	case BEAT_REPLY:
		//startBeat();
		break;
	case DEVICE_TIME_REPLY:
		sendDeviceTime();
		break;
	case DEVICE_INFO_REPLY:
		sendDeviceInfo(devCode);
		break;
	case IP_REPLY:
		sendDeviceIP(devCode);
		break;
	default:
		break;
	}
}

void CommandSender::sendBeat()
{
	if (m_devcies.size() == 0)
		return;
	auto db = m_frameMaker->makeBeatReplyFrame();
	sendDataByBus(db, BEAT_REPLY);
}

void CommandSender::sendDevcieStatus()
{
	for (auto iter = m_devcies.begin(); iter != m_devcies.end(); iter++)
	{
		auto db = m_frameMaker->makeDevcieStatusReplyFrame(iter->second);
		auto infoType = (iter->first == "4100_3900") ? IF_DEVICE_STATUS_REPLY : PA_DEVICE_STATUS_REPLY;
		sendDataByBus(db, infoType);
	}
}

void CommandSender::sendDeviceTime()
{
	auto db = m_frameMaker->makeDeviceTimeReplyFrame();
	sendDataByBus(db, DEVICE_TIME_REPLY);
}

void CommandSender::sendDeviceInfo(const SystemDeviceCode& devCode)
{
	auto db = m_frameMaker->makeDeviceInfoReplyFrame(devCode);
	sendDataByBus(db, DEVICE_INFO_REPLY);
}

void CommandSender::sendDeviceIP(const SystemDeviceCode& devCode)
{
	std::string devName = extractDevName(devCode);
	if (m_devcies.find(devName) == m_devcies.end())
		return;
	auto db = m_frameMaker->makeDeviceIPReplyFrame(m_devcies[devName].ip, devCode);
	sendDataByBus(db, IP_REPLY);
}

void CommandSender::sendDataByBus(const DataBlock& db, const int infoType)
{
	if (m_bscb.sendByBus)
		m_bscb.sendByBus((char*)db.addr, db.len, infoType, m_bscb.owner);
}