#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <sstream>
#include <string>

#ifndef NDEBUG
#define LOGGING_ENABLED
#endif

class Logger
{
public:
    Logger(const std::string &filename);
    ~Logger();

    void log(const std::string &message, const std::string &functionName);

    template <typename... Args>
    void log(const std::string &functionName, const char *format, Args... args)
    {
#ifdef LOGGING_ENABLED
        if (logFile.is_open())
        {
            std::ostringstream oss;
            format_string(oss, format, args...);
            logFile << "[" << functionName << "] " << oss.str() << std::endl;
        }
#endif
    }

private:
    std::ofstream logFile;

    void format_string(std::ostringstream &oss, const char *format)
    {
        oss << format;
    }

    template <typename T, typename... Args>
    void format_string(std::ostringstream &oss, const char *format, T value, Args... args)
    {
        while (*format != '\0')
        {
            if (*format == '%' && *(format + 1) == 'd')
            {
                oss << value;
                format += 2;
                format_string(oss, format, args...);
                return;
            }
            else
            {
                oss << *format++;
            }
        }
    }
};

extern Logger logger;

#endif // LOGGER_H
