#pragma once
#include "CmdAndReplyDef.h"

#pragma pack(1)
struct UTCTime
{
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int second;
	int millisecond;
};
#pragma pack()

uint64_t uint32To64(const uint32_t left, const uint32_t right);
std::pair<uint32_t, uint32_t> uint64To32(const uint64_t int64Value);
unsigned short charToshort(const char left, const char right);
std::pair<char, char> shortTochar(const unsigned short shortValue);
uint64_t current64Time();
void pushBackToBuffer(std::vector<BYTE>& buffer, void const* src, size_t size);
std::vector<unsigned char> strIpToChars(const std::string& ip);
uint64_t utcToMsec(const UTCTime& utc);
UTCTime msecToUtc(const uint64_t& milliSeconds);
int64_t getCurrentUtcInt64();
struct timeval getTimeVal();
#ifdef PLATFROM_WINDOWS
#else
void GetLocalTime(SYSTEMTIME* wtm);
#endif
uint64_t getCurrentTimeMsec();
UTCTime getCurrentUtc();
SYSTEMTIME getSystemTime();
std::string extractDevName(const SystemDeviceCode& devCode);
SYSTEMTIME msecToSystemTime(const uint64_t& milliSeconds);
IQData initDataPack();