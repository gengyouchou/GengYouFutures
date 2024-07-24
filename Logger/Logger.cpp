#include "Logger.h"
#include <iostream>

#ifndef NDEBUG
#define LOGGING_ENABLED
#endif

Logger::Logger(const std::string &filename) : logFile(filename, std::ios::out | std::ios::app)
{
#ifdef LOGGING_ENABLED
    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
#endif
}

Logger::~Logger()
{
#ifdef LOGGING_ENABLED
    if (logFile.is_open())
    {
        logFile.close();
    }
#endif
}


