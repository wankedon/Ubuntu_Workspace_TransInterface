#include"stdafx.h"
#include"Transport.h"
#include"Interactor.h"
#include"UDPTransport.h"
#include"tools.h"
#include "Logger.h"

Transport::Transport(const char*  udpPoint, const ControlCallBacks& ccbs, const BusSendCallBack& bscb)
	:m_ccbs(ccbs)
{
	auto receiveByUdp = [this](asio::ip::udp::endpoint point, asio::const_buffer buffer)
	{
		std::vector<char> tempbuffer(buffer.size());
		memcpy(tempbuffer.data(), buffer.data(), buffer.size());	
		RawData rawdata;
		rawdata.addr = tempbuffer.data();
		rawdata.len = buffer.size();
		inputRawData(rawdata);
	};
	m_udp = std::make_unique<UDPTransport>(5555, udpPoint, receiveByUdp);
	auto sendByUdp = [this](const std::vector<char> buffer)
	{
		if (m_udp)
		{
			m_udp->send(asio::buffer(buffer));
			LOG("sendByUdp");
		}
	};
	ReceiveCallBacks rcbs{
	std::bind(&Transport::onReceiveStartPano, this, std::placeholders::_1),
	std::bind(&Transport::onReceiveStopPano, this, std::placeholders::_1),
	std::bind(&Transport::onReceiveStartIF, this, std::placeholders::_1),
	std::bind(&Transport::onReceiveStopIF, this, std::placeholders::_1),
	std::bind(&Transport::onReceiveChangeLog, this, std::placeholders::_1)
	};
	m_inter = std::make_unique<Interactor>(sendByUdp, rcbs, bscb);
	startBeat();
	m_udp->start();
}

Transport::~Transport()
{
	release();
}

void Transport::release()
{
	if (m_udp)
		m_udp->stop();
	if (m_inter)
		m_inter.reset();
}

//开始心跳
void Transport::startBeat()
{
	auto timeoutHandler = [this]()
	{
		m_inter->sendBeat();
	};
	m_udp->startTimer(BEAT_INTERVAL_MS, timeoutHandler);
}

//输入原始数据
void Transport::inputRawData(const RawData& buffer)
{
	if (m_inter)
		m_inter->inputRawData(buffer);
}

void Transport::reportCallBacks(ReportCallBacks& rcbs)
{
	rcbs.owner = this;
	rcbs.reportPanoData = sendPanoData;
	rcbs.reportIQData = sendIQData;
	rcbs.reportDevInfo = sendDevInfo;
}

//日志指令回调函数
void Transport::onReceiveChangeLog(const LogExecMode& mode)
{
	if (m_ccbs.changeLogMode)
		m_ccbs.changeLogMode(mode, m_ccbs.owner);
}

//开始全景任务回调函数
void Transport::onReceiveStartPano(const StartPanoParam& param)
{
	if (!m_ccbs.startPano)
		return;
	auto erroType = m_ccbs.startPano(param, m_ccbs.owner);
	sendTaskCmdReply(START_PANORAMA_CMD, erroType);
}

//停止全景任务回调函数
void Transport::onReceiveStopPano(const StopTaskParam& param)
{
	if (!m_ccbs.startPano)
		return;
	auto erroType = m_ccbs.stopPano(param, m_ccbs.owner);
	sendTaskCmdReply(STOP_PANORAMA_CMD, erroType);
}

//开始IQ任务回调函数
void Transport::onReceiveStartIF(const StartIFParam& param)
{
	if (!m_ccbs.startIF)
		return;
	auto erroType = m_ccbs.startIF(param, m_ccbs.owner);
	sendTaskCmdReply(START_IF_CMD, erroType);
}

//停止IQ任务回调函数
void Transport::onReceiveStopIF(const StopTaskParam& param)
{
	//m_iqDataToSend.clear();
	if (!m_ccbs.stopIF)
		return;
	auto erroType = m_ccbs.stopIF(param, m_ccbs.owner);
	sendTaskCmdReply(STOP_IF_CMD, erroType);
}

//发送全景数据
void Transport::sendPanoData(const PanoData& panodata,void* ptr)
{
	Transport* self = (Transport*)ptr;
	if (self->m_inter)
	{
		self->m_inter->sendPanoData(panodata);
	}
}

//发送IQ数据
void Transport::sendIQData(const IQData& iqdata, void* ptr)
{
	Transport* self = (Transport*)ptr;
	if (self->m_inter)
	{
		self->m_inter->sendIQData(iqdata);
	}
}

//发送设备信息
void Transport::sendDevInfo(const DevList& devcies, void* ptr)
{
	Transport* self = (Transport*)ptr;
	if (self->m_inter)
	{
		self->m_inter->updateDevcie(devcies);
	}
}

//发送任务响应
void Transport::sendTaskCmdReply(const uint32_t infoType, const uint32_t erroType)
{
	if (m_inter)
		m_inter->sendTaskCmdReply(infoType, erroType);
}