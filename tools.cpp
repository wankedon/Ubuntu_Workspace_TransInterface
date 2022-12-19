#include"stdafx.h"
#include "tools.h"
#ifdef PLATFROM_WINDOWS
#include <time.h>
#else
#include <sys/time.h>
#endif
/*
#if _MSC_VER > 1900
#include <fmt_/format.h>
#else
#include <fmt/format.h>
#endif*/

uint64_t uint32To64(const uint32_t left, const uint32_t right)
{
	uint64_t int64Value = left;
	int64Value = (int64Value << 32);
	int64Value |= right;
	return int64Value;
}

std::pair<uint32_t, uint32_t> uint64To32(const uint64_t int64Value)
{
	return std::make_pair(int64Value >> 32, int64Value);
}

unsigned short charToshort(const char left, const char right)
{
	unsigned short shortValue = left;
	shortValue = (shortValue << 8);
	shortValue |= right;
	return shortValue;
}

std::pair<char, char> shortTochar(const unsigned short shortValue)
{
	return std::make_pair(shortValue >> 8, shortValue);
}

uint64_t current64Time()
{
	struct timespec ts;
	auto ret = timespec_get(&ts, TIME_UTC);
	return uint32To64((uint32_t)ts.tv_sec, (uint32_t)ts.tv_nsec);
}

void pushBackToBuffer(std::vector<BYTE>& buffer, void const* src, size_t size)
{
	std::vector<unsigned char> data(size);
	memcpy(data.data(), src, size);
	buffer.insert(buffer.end(), data.begin(), data.end());
}

std::vector<unsigned char> strIpToChars(const std::string& ip)
{
	std::string ipStr(ip);
	std::vector<unsigned char> ipChar;
	while (ipStr.length())
	{
		auto size = ipStr.find(".");
		auto tt = ipStr.substr(0, size);
		ipChar.emplace_back(atoi(tt.c_str()));
		ipStr = ipStr.substr(size + 1, ipStr.length() - size - 1);
		if (size > ipStr.length())
			break;
	}
	return ipChar;
}

uint64_t utcToMsec(const UTCTime& utc)
{
	struct tm tm_time;
	memset(&tm_time, 0, sizeof(tm_time));
	tm_time.tm_year = utc.year - 1900;
	tm_time.tm_mon = utc.mon - 1;
	tm_time.tm_mday = utc.day;
	tm_time.tm_hour = utc.hour;
	tm_time.tm_min = utc.min;
	tm_time.tm_sec = utc.second;
	return (uint64_t)mktime(&tm_time) * 1000 + (uint64_t)utc.millisecond;
}

UTCTime msecToUtc(const uint64_t& milliSeconds)
{
	UTCTime utc{0,0,0,0,0,0,0};
	time_t seconds = milliSeconds / 1000;
	struct tm local;
#ifdef PLATFROM_WINDOWS
	localtime_s(&local,(time_t*)&seconds);
#else
	local = *(localtime((time_t*)&seconds));
#endif
	utc.year = local.tm_year + 1900;
	utc.mon = local.tm_mon + 1;
	utc.day = local.tm_mday;
	utc.hour = local.tm_hour;
	utc.min = local.tm_min;
	utc.second = local.tm_sec;
	utc.millisecond = milliSeconds % 1000;
	return utc;
}

struct timeval getTimeVal()
{
	struct timeval tp;
#ifdef PLATFROM_WINDOWS
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp.tv_sec = clock;
	tp.tv_usec = wtm.wMilliseconds * 1000;
#else
	struct timezone tz;
	gettimeofday(&tp, &tz);
#endif
	return tp;
}

#ifdef PLATFROM_WINDOWS
#else
void GetLocalTime(SYSTEMTIME* wtm)
{
	time_t t;
	time(&t);
	struct tm* tm = localtime(&t);
	timeval tv = getTimeVal();
	wtm->wYear = tm->tm_year + 1900;
	wtm->wMonth = tm->tm_mon + 1;
	wtm->wDayOfWeek = tm->tm_wday;
	wtm->wDay = tm->tm_mday;
	wtm->wHour = tm->tm_hour;
	wtm->wMinute = tm->tm_min;
	wtm->wSecond = tm->tm_sec;
	wtm->wMilliseconds = tv.tv_usec / 1000;
}
#endif

uint64_t getCurrentTimeMsec()
{
	struct timeval timeval = getTimeVal();
	return (uint64_t)timeval.tv_sec * 1000 + (uint64_t)timeval.tv_usec / 1000;
}

UTCTime getCurrentUtc()
{
	return msecToUtc(getCurrentTimeMsec());
}
/*
std::string getCurrentUtcStr()
{
	auto utc = getCurrentUtc();
	return fmt::format
	(
		"{}{}{}{}{}{}{}",
		utc.year,
		utc.mon,
		utc.day,
		utc.hour,
		utc.min,
		utc.second,
		utc.millisecond
	);
}*/

int64_t getCurrentUtcInt64()
{
	auto utc = getCurrentUtc();
	int64_t recoderNumber = utc.year*10e11;
	recoderNumber += utc.mon*10e10;
	recoderNumber += utc.day*10e8;
	recoderNumber += utc.hour*10e6;
	recoderNumber += utc.min*10e4;
	recoderNumber += utc.second*10e2;
	recoderNumber += utc.millisecond;
	int64_t kk = utc.year*10e11;
	return recoderNumber;
}

SYSTEMTIME getSystemTime()
{
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	return wtm;
}

std::string extractDevName(const SystemDeviceCode& devCode)
{
	std::vector<unsigned char> devNameChar(devCode.deviceCodeLength);
	memcpy(devNameChar.data(), &devCode.deviceCode[0], devCode.deviceCodeLength);
	std::string devName;
	devName.insert(devName.begin(), devNameChar.begin(), devNameChar.end());
	return devName;
}

SYSTEMTIME msecToSystemTime(const uint64_t& milliSeconds)
{
	SYSTEMTIME time;
	auto utc = msecToUtc(milliSeconds);
	time.wYear = utc.year;
	time.wMonth = utc.mon;
	time.wDayOfWeek = 0;
	time.wDay = utc.day;
	time.wHour = utc.hour;
	time.wMinute = utc.min;
	time.wSecond = utc.second;
	time.wMilliseconds = utc.millisecond;
	return time;
}

IQData initDataPack()
{
	IQData iqDataPack;
	iqDataPack.packheader = IQ_packHeader;
	iqDataPack.packindex = 1;
	iqDataPack.dataheader = IQ_dataHeader;
	iqDataPack.ddcId = 1;
	iqDataPack.channelType = 2;
	return iqDataPack;
}