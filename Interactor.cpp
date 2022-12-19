#include"stdafx.h"
#include "Interactor.h"
#include "CommandParser.h"
#include "CommandSender.h"

Interactor::Interactor(const SendByUdpHandler& sendByUdp, const ReceiveCallBacks& rcbs, const BusSendCallBack& bscb)
	: m_sender(std::make_unique<CommandSender>(sendByUdp, bscb))
{
	auto autoReplyHandler = [this](const uint32_t infoType, const SystemDeviceCode& devCode)
	{
		if (m_sender)
			m_sender->sendAutoReply(infoType, devCode);
	};
	m_parser = std::make_unique<CommandParser>(autoReplyHandler, rcbs);
}

Interactor::~Interactor()
{
	if (m_sender)
		m_sender.reset();
	if (m_parser)
		m_parser.reset();
}

void Interactor::sendBeat()
{
	if (m_sender)
	{
		m_sender->sendBeat();
		//m_sender->sendDevcieStatus();
	}
}

void Interactor::inputRawData(const RawData& buffer)
{
	m_parser->inputRawData(buffer);
}

void Interactor::sendPanoData(const PanoData& panoData)
{
	if (m_sender)
		m_sender->sendPanoData(panoData);
}

void Interactor::sendIQData(const IQData& dataPack)
{
	if (m_sender)
		m_sender->sendIQData(dataPack);
}

void Interactor::sendTaskCmdReply(const uint32_t infoType, const uint32_t erroType)
{
	if (m_sender)
		m_sender->sendTaskCmdReply(infoType, erroType);
}

void Interactor::updateDevcie(const DevList& devcies)
{
	if (m_sender)
		m_sender->updateDevcie(devcies);
}