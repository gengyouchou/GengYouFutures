#include "Logger.h"
#include <iostream>

Logger::Logger(const std::string &filename) : logFile(filename, std::ios::out | std::ios::app)
{
    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

Logger::~Logger()
{
    if (logFile.is_open())
    {
        logFile.close();
    }
}

void Logger::log(const std::string &message, const std::string &functionName)
{
    if (logFile.is_open())
    {
        logFile << "[" << functionName << "] " << message << std::endl;
    }
}
