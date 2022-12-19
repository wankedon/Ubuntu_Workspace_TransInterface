#include"stdafx.h"
#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

std::string Logger::PATH{ "./TransLog.txt" };

const std::string Logger::CONSOLE_LOGGER_NAME = "ConsoleLogger";
const std::string Logger::FILE_LOGGER_NAME = "FileLogger";

void LOGFlush()
{
	Logger::instance().flush();
}


