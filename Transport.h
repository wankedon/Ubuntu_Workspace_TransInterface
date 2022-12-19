#pragma once
#include "TransComDef.h"

class Interactor;
class UDPTransport;
class Transport
{
public:
	Transport(const char*  udpPoint, const ControlCallBacks& trcbs, const BusSendCallBack& bscb);
	~Transport();

public:
	void inputRawData(const RawData& buffer);
	void release();
	void reportCallBacks(ReportCallBacks& rcbs);

private:
	static void sendPanoData(const PanoData& panodata, void* ptr);
	static void sendIQData(const IQData& iqdata, void* ptr);
	static void sendDevInfo(const DevList& devcies, void* ptr);
	void onReceiveChangeLog(const LogExecMode& mode);
	void onReceiveStartPano(const StartPanoParam& param);
	void onReceiveStopPano(const StopTaskParam& param);
	void onReceiveStartIF(const StartIFParam& param);
	void onReceiveStopIF(const StopTaskParam& param);
	void sendTaskCmdReply(const uint32_t infoType, const uint32_t erroType);
	void startBeat();

private:
	ControlCallBacks m_ccbs;
	std::unique_ptr<Interactor> m_inter;
	std::unique_ptr<UDPTransport> m_udp;

private:
	static const uint64_t BEAT_INTERVAL_MS = 5000;
};