#pragma once

class MessageFrame;
class BytesBuffer
{
public:
	BytesBuffer();
	~BytesBuffer();
public:
	std::list<std::shared_ptr<MessageFrame>> inputBuffer(const std::vector<char>& recBuf);
private:
	std::list<std::shared_ptr<std::vector<char>>> extractFullFrame(std::vector<char>& buffer);
	std::vector<int> extractFrameHead(std::vector<char>& allFrame, std::vector<char>& byteBuffer);
	void multiFrameExtract(std::vector<char>& allFrame, std::list<std::shared_ptr<std::vector<char>>>& frameList);
	void singleFrameExtract(std::vector<char>& allFrame, std::list<std::shared_ptr<std::vector<char>>>& frameList);
private:
	std::vector<char> m_bytebuffer;
	std::vector<int> m_headIndex;
};