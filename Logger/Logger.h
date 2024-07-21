#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

class Logger
{
public:
    Logger(const std::string &filename);
    ~Logger();
    void log(const std::string &message, const std::string &functionName);

private:
    std::ofstream logFile;
};

Logger logger("debug.log");

#endif // LOGGER_H
