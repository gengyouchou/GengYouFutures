#include <iostream>
#include <fstream>
#include <string>

class Logger
{
public:
    Logger(const std::string &filename) : logFile(filename, std::ios::out | std::ios::app)
    {
        if (!logFile.is_open())
        {
            std::cerr << "Failed to open log file: " << filename << std::endl;
        }
    }

    ~Logger()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    void log(const std::string &message, const std::string &functionName)
    {
        if (logFile.is_open())
        {
            logFile << "[" << functionName << "] " << message << std::endl;
        }
    }

private:
    std::ofstream logFile;
};
