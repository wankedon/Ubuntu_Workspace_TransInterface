#include "stdafx.h"
#include "TransInterface.h"
#include "Transport.h"
#include "TransportPool.h"

std::unique_ptr<TransportPool> transportPool;
TRANSINTERFACE_API TR_Error InitLib()
{
	if (transportPool == nullptr)
	{
		transportPool = std::make_unique<TransportPool>();
	}
	return TR_ERR_NONE;
}

TRANSINTERFACE_API TR_Error CreateTransport(int* fd, const char*  udpPoint, const ControlCallBacks& trcbs, const BusSendCallBack& bscb)
{
	if (transportPool == nullptr)
		return TR_ERR_INIT;
	auto pair = transportPool->newHandle(udpPoint, trcbs,bscb);
	*fd = pair.first;
	return pair.second;
}

TRANSINTERFACE_API TR_Error InputRawData(int fd, const RawData& buffer)
{
	if (transportPool == nullptr)
		return TR_ERR_INIT;
	auto transport = transportPool->getTransport(fd);
	if (transport == nullptr)
		return TR_ERR_INVALID_HANDLE;
	transport->inputRawData(buffer);
	return TR_ERR_NONE;
}

TRANSINTERFACE_API TR_Error GetReportCallBacks(int fd, ReportCallBacks& rcbs)
{
	if (transportPool == nullptr)
		return TR_ERR_INIT;
	auto transport = transportPool->getTransport(fd);
	if (transport == nullptr)
		return TR_ERR_INVALID_HANDLE;
	transport->reportCallBacks(rcbs);
	return TR_ERR_NONE;
}

TRANSINTERFACE_API TR_Error CloseTransport(int fd)
{
	if (transportPool == nullptr)
		return TR_ERR_INIT;
	if (fd == -1)
		return TR_ERR_INVALID_HANDLE;
	return transportPool->closeHandle(fd);
}

TRANSINTERFACE_API TR_Error DeInitLib()
{
	transportPool.reset();
	return TR_ERR_NONE;
}