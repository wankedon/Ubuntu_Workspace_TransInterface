#pragma once
#include "CmdAndReplyDef.h"

/**
* @brief 从字节流中提取完整数据帧
* @details
*/
class MessageFrame
{
public:
	MessageFrame(const std::vector<char>& buffer);
	~MessageFrame();

public:
	//bool isCmdReply() const { return size() == REPLY_FRAME_SIZE; }
	size_t size() const;
	size_t blockSize() const;
	uint32_t infoType() const;
	uint32_t index() const;
	bool extractBlock(std::vector<char>& data);

private:
	std::unique_ptr<std::vector<char>> m_frame;
};