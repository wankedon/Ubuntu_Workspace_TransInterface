#include "stdafx.h"
#include "TransInterface.h"
#include "TransportPool.h"
#include "Transport.h"

using namespace std;

TransportPool::TransportPool()
{
	for (auto i = 0; i < MAX_HANDLE_COUNT; i++)
	{
		m_Handles.push_back(MAGIC_NUM + i);   ///构造函数中构建MAX_HANDLE_COUNT个备用句柄
	}
}


TransportPool::~TransportPool()
{
}

/**
* @brief 创建新的连接
* @param[in] peerAddress 远端主机地址
* @param[in] handler 回调函数集合
* @return pair<int, PS_Error>
*/
std::pair<int, TR_Error> TransportPool::newHandle(const char*  udpPoint, const ControlCallBacks& ccbs, const BusSendCallBack& bscb)
{
	if (m_Handles.empty())
		return std::pair<int, TR_Error>{-1, TR_ERR_NO_HANDLE_RESOURCE};
	auto transport = make_shared<Transport>(udpPoint, ccbs, bscb);
	int handle = m_Handles.front();
	assert(m_socketsInUse.find(handle) == m_socketsInUse.end());
	m_socketsInUse[handle] = transport;
	m_Handles.pop_front();
	return std::pair<int, TR_Error>{handle, TR_ERR_NONE};
}

/**
* @brief 获得DeviceSession的一个实例
* @param[in] handle PositionSocket句柄
* @return shared_ptr<DeviceSession>
* @retval DeviceSession实例的指针
*/
shared_ptr<Transport> TransportPool::getTransport(int handle)
{
	if (m_socketsInUse.find(handle) == m_socketsInUse.end()) 
		return nullptr;
	return m_socketsInUse[handle];
}

/**
* @brief 关闭句柄
* @details
* @param[int] handle PositionSocket句柄
* @return PS_Error
*/
TR_Error TransportPool::closeHandle(int handle)
{
	if (m_socketsInUse.find(handle) == m_socketsInUse.end())
		return TR_ERR_INVALID_HANDLE;
	m_socketsInUse[handle]->release();
	m_socketsInUse.erase(handle);			///< 排除socket，自动清理
	m_Handles.push_back(handle);		    ///< 归还句柄号
	return TR_ERR_NONE;
}