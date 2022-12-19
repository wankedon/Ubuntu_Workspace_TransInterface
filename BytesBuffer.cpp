/**
* @file BytesBuffer.cpp
* @brief �ֽ���������
* @author װ����ҵ�������� ���˶�
* @date 2022:06:03
* @Copyright (c) 2022 �й����ӿƼ����Ź�˾����ʮһ�о���
*/ 
#include "stdafx.h"
//#include <string.h>
#include "BytesBuffer.h"
#include "MessageFrame.h"
#include "CmdAndReplyDef.h"
using namespace std;

BytesBuffer::BytesBuffer()
{
}

BytesBuffer::~BytesBuffer()
{
}

/**
* @brief ���ݿ���뻺�壬��ȡ��������֡
* @param[in] db �������ݿ�
*/
list<std::shared_ptr<MessageFrame>> BytesBuffer::inputBuffer(const std::vector<char>& recBuf)
{
	list<std::shared_ptr<MessageFrame>> result;
	vector<char> temp = recBuf;
	m_bytebuffer.insert(m_bytebuffer.end(), temp.begin(), temp.end());
	while (true)
	{
		if (m_bytebuffer.size() == 0)
			break;
		auto frameList = extractFullFrame(m_bytebuffer);
		if (frameList.size() == 0)
			break;
		for (auto fr : frameList)
		{
			auto ptr = *fr.get();
			vector<char> fb(ptr.size());
			memcpy(fb.data(), ptr.data(), ptr.size());
			auto frame = make_shared<MessageFrame>(fb);
			if (frame->size() == 0)
				break;
			result.push_back(frame);
			m_bytebuffer.erase(m_bytebuffer.begin(), m_bytebuffer.begin() + frame->size());
		}
	}
	return result;
}

/**
* @brief ��ȡ����֡
* @param[in] buffer �ֽ�����
*/
list<std::shared_ptr<vector<char>>> BytesBuffer::extractFullFrame(vector<char>& byteBuffer)
{
	list<std::shared_ptr<vector<char>>> frameList;
	if (byteBuffer.size() < 2 * sizeof(transType))
		return frameList;
	vector<char> allFrame;
	m_headIndex = extractFrameHead(allFrame, byteBuffer);
	if (m_headIndex.size() == 0)
		return frameList;
	if (m_headIndex.size() > 1)//�������֡ͷ
	{
		multiFrameExtract(allFrame, frameList);
	}
	else//����һ��֡ͷ
	{
		allFrame = { allFrame.begin() + m_headIndex[0],allFrame.end() };
	}
	//��֡ʣ�����ݻ������֡ͷ
	singleFrameExtract(allFrame, frameList);
	return frameList;
}

/**
* @brief ��ȡ֡ͷ
* @param[out] allFrame char����
* @param[in]  byteBuffer �ֽ�����
* @return vector<int> ֡ͷλ��
*/
vector<int> BytesBuffer::extractFrameHead(vector<char>& allFrame, vector<char>& byteBuffer)
{
	allFrame = byteBuffer;
	vector<int> headIndex(0);//֡ͷλ��
	for (int index = 0; index < byteBuffer.size(); index++)
	{
		transType header;
		memcpy(&header, &byteBuffer[index], sizeof(transType));
		if (header == MSG_HEADER)
		{
			headIndex.emplace_back(index);
		}
	}
	return headIndex;
}

/**
* @brief ��֡��ȡ
* @param[out] allFrame ��ȡ֡��ʣ��char����
* @param[out]  frameList ��ȡ������֡
*/
void BytesBuffer::multiFrameExtract(vector<char>& allFrame, list<std::shared_ptr<vector<char>>>& frameList)
{
	if (m_headIndex.size() == 0)
		return;
	int eraseSize = 0;//�������ֽ���
	for (int i = 0; i < m_headIndex.size() - 1; i++)
	{
		vector<char> oneFrame = { allFrame.begin() + m_headIndex[i],allFrame.begin() + m_headIndex[i + 1] };
		frameList.push_back(make_shared<vector<char>>(oneFrame));
		transType frameLength;
		memcpy(&frameLength, &allFrame[m_headIndex[i] + sizeof(transType)], sizeof(transType));
		eraseSize += frameLength;
	}
	//��������֡
	allFrame.erase(allFrame.begin() + m_headIndex[0], allFrame.begin() + eraseSize);
}

/**
* @brief ��֡��ȡ
* @param[out] allFrame ��ȡ֡��ʣ��uint32_t����
* @param[out]  frameList ��ȡ������֡
*/
void BytesBuffer::singleFrameExtract(vector<char>& allFrame, list<std::shared_ptr<vector<char>>>& frameList)
{
	if (allFrame.size() < 2 * sizeof(transType))
		return;
	transType frameLength;
	memcpy(&frameLength, &allFrame[4], sizeof(transType));
	if (allFrame.size() < frameLength)
		return;
	vector<char> oneFrame = { allFrame.begin(),allFrame.begin() + frameLength };
	frameList.push_back(make_shared<vector<char>>(oneFrame));
	//����
	allFrame.erase(allFrame.begin(), allFrame.begin() + frameLength);
}