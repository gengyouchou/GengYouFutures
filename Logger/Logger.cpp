// Logger.cpp
#include "Logger.h"
#include <iostream>

// #ifndef NDEBUG
#define LOGGING_ENABLED
// #endif

// Constructor: Automatically add date to the log file name
Logger::Logger(const std::string &filename_prefix)
{
    std::string date = get_current_date();
    std::string filename = date + "_" + filename_prefix + ".log";
    logFile.open(filename, std::ios::app);

    if (!logFile)
    {
        throw std::runtime_error("Failed to open log file: " + filename);
    }
}

Logger::~Logger()
{
    if (logFile.is_open())
    {
        logFile.close();
    }
}
