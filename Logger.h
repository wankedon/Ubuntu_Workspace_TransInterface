/**
 * @file Logger.h
 * @brief 日志功能
 * @author 装备事业部软件组 杨青 
 * @version 0.1
 * @date 2020-12-24
 * 
 * @copyright Copyright (c) 2020  中国电子科技集团公司第四十一研究所
 * 
 */
#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

class Logger
{
public:
    Logger(const std::string& path)
    {
        consoleLogger = spdlog::stdout_color_mt(CONSOLE_LOGGER_NAME);
        fileLogger = spdlog::basic_logger_mt(FILE_LOGGER_NAME, path);
        consoleLogger->set_level(spdlog::level::debug);
        fileLogger->set_level(spdlog::level::info);
    }
    ~Logger()
    {
		fileLogger->flush();
        consoleLogger.reset();
        fileLogger.reset();
        spdlog::drop_all();
    }

    static std::string PATH;

    static Logger& instance()
    {
        static Logger lg(PATH);
        return lg;
    }

public:
    template<typename T>
    void consoleLog(const T& msg)
    {
        consoleLogger->debug(msg);
    }

    template<typename T>
    void fileLog(const T& msg)
    {
        fileLogger->info(msg);
        fileLogger->flush();
    }

    template<typename FormatString, typename... Args>
    void consoleLog(const FormatString& fmt, const Args &... args)
    {
        consoleLogger->debug(fmt, args...);
    }

    template<typename FormatString, typename... Args>
    void fileLog(const FormatString& fmt, const Args &... args)
    {
        fileLogger->info(fmt, args...);
        fileLogger->flush();
    }

	void flush()
	{
		consoleLogger->flush();
		fileLogger->flush();
	}
private:
	static const std::string CONSOLE_LOGGER_NAME;
	static const std::string FILE_LOGGER_NAME;
private:
    std::shared_ptr<spdlog::logger> consoleLogger;
	std::shared_ptr<spdlog::logger> fileLogger;
};

/**
 * @brief 日志的控制台输出
 * 
 * @tparam T 日志信息模板类型
 * @param msg 要输出的日志信息
 */
template<typename T>
void CLOG(const T& msg)
{
    Logger::instance().consoleLog(msg);
}

template<typename FormatString, typename... Args>
void CLOG(const FormatString& fmt, const Args &... args)
{
    Logger::instance().consoleLog(fmt, args...);
}

/**
 * @brief 日志的文件输出
 * 
 * @tparam T 
 * @param msg 
 */
template<typename T>
void FLOG(const T& msg)
{
    Logger::instance().fileLog(msg);
}

template<typename FormatString, typename... Args>
void FLOG(const FormatString& fmt, const Args &...args)
{
    Logger::instance().fileLog(fmt, args...);
}

/**
 * @brief 日志同时输出到控制台和文件
 * 
 * @tparam T 
 * @param msg 
 */
template<typename T>
void LOG(const T& msg)
{
    CLOG(msg);
    FLOG(msg);
}

/**
 * @brief 日志同时输出
 * 
 * @tparam FormatString 
 * @tparam Args 
 * @param fmt 
 * @param args 
 */
template<typename FormatString, typename... Args>
void LOG(const FormatString& fmt, const Args &... args)
{
    CLOG(fmt, args ...);
    FLOG(fmt, args ...);
}

/**
 * @brief 立即处理日志缓冲，通常在程序退出前调用
 * 
 */
void LOGFlush();

