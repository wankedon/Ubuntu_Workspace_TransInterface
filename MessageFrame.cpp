#include"stdafx.h"
#include "MessageFrame.h"

MessageFrame::MessageFrame(const std::vector<char>& buffer)
{
	if (buffer.size() < sizeof(transType))
		return;
	uint32_t length;
	memcpy(&length, &buffer[4], sizeof(uint32_t));
	if (buffer.size() < length)
		return;
	m_frame = std::make_unique<std::vector<char>>(length);
	memcpy(m_frame->data(), buffer.data(), length);
}

MessageFrame::~MessageFrame()
{
}

/**
* @brief 获取数据帧中命令ID
* @return 数据帧命令ID
*/
uint32_t MessageFrame::infoType() const
{
	if (m_frame == nullptr)
	{
		return 0;
	}
	else
	{
		unsigned short infoType = 0;
		memcpy(&infoType, m_frame->data() + offsetof(Message, infoType), sizeof(unsigned short));
		return infoType;
	}
}

/**
* @brief 获取数据帧中响应次数
* @return 响应次数
*/
uint32_t MessageFrame::index() const
{
	if (m_frame == nullptr)
		return 0;
	else
	{
		transType sequence = 0;
		memcpy(&sequence, m_frame->data() + offsetof(Message, sequenceNum), sizeof(transType));
		return sequence;
	}
}

/**
* @brief 获取数据帧大小
* @return 响应次数
*/
size_t MessageFrame::size() const
{
	if (m_frame == nullptr)
		return 0;
	else
		return m_frame->size();
}

size_t MessageFrame::blockSize() const
{
	if (m_frame == nullptr)
		return 0;
	else
	{
		transType dataLength;
		memcpy(&dataLength, m_frame->data() + offsetof(Message, dataLength), sizeof(transType));
		return dataLength;
	}
}

bool MessageFrame::extractBlock(std::vector<char>& data)
{
	if (m_frame == nullptr)
		return false;
	data.resize(blockSize());
	memcpy(data.data(), m_frame->data() + offsetof(Message, dataBlock), blockSize());
	return true;
}