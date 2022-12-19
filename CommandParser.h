#pragma once
#include "TransComDef.h"
#include "CmdAndReplyDef.h"

using RLOGHandler = std::function<void(const LogExecMode& model)>;
using RStartPanaHandler = std::function<void(const StartPanoParam& param)>;
using RStopPanaHandler = std::function<void(const StopTaskParam& param)>;
using RStartIFHandler = std::function<void(const StartIFParam& param)>;
using RStopIFHandler = std::function<void(const StopTaskParam& param)>;
using AutoReplyHandler = std::function<void(const uint32_t infoType, const SystemDeviceCode& devCode)>;

class MessageFrame;
struct ReceiveCallBacks
{
	RStartPanaHandler startPana;
	RStopPanaHandler stopPana;
	RStartIFHandler startIF;
	RStopIFHandler stopIF;
	RLOGHandler changeLog;
};

class BytesBuffer;
class CommandParser
{
public:
	CommandParser(AutoReplyHandler autoReplyHandler, const ReceiveCallBacks& cpcbs);
	~CommandParser();

public:
	void inputRawData(const RawData& recBuf);

private:
	static bool isCmdFrame(uint32_t id) { return (CMD_NAME.find(id) != CMD_NAME.end()); }
	bool extractDataBlock(std::shared_ptr<MessageFrame> m_currentFrame);
	bool detectReply(std::shared_ptr<MessageFrame> frame, uint32_t sequenceNum) const;
	void parserStartPanacmd(const std::vector<char>& data);
	void parserStopPanacmd(const std::vector<char>& data);
	void parserStartIFcmd(const std::vector<char>& data);
	void parserStopIFcmd(const std::vector<char>& data);
	void parserBeatcmd(const std::vector<char>& data);
	void parserSystemTimecmd(const std::vector<char>& data);
	void parserDevcieInfocmd(const std::vector<char>& data);
	void parserDevcieIPcmd(const std::vector<char>& data);
	void parserLogCmd(const std::vector<char>& data);

private:
	ReceiveCallBacks m_cpcbs;
	AutoReplyHandler m_autoReplyHandler;
	std::unique_ptr<BytesBuffer> m_buffer;

private:
	static const std::set<uint32_t> CMD_NAME;
};