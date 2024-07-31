#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <iomanip> // for std::hex, std::setprecision
#include <sstream>
#include <stdexcept> // for std::runtime_error
#include <string>
#include <type_traits> // for std::is_same_v

#ifndef NDEBUG
#define LOGGING_ENABLED
#define ENABLE_DEBUG
#endif

// Define different debug levels
#define DEBUG_LEVEL_INFO 1
#define DEBUG_LEVEL_ERROR 2
#define DEBUG_LEVEL_DEBUG 3

// Set the current debug level
#define DEBUG_LEVEL DEBUG_LEVEL_INFO

class Logger
{
public:
    Logger(const std::string &filename);
    ~Logger();

    void log(const std::string &message, const std::string &functionName);

    template <typename... Args>
    void log(int level, const std::string &functionName, const char *format, Args... args)
    {
#ifdef LOGGING_ENABLED
        if (logFile.is_open() && level <= DEBUG_LEVEL)
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
        while (*format != '\0')
        {
            if (*format == '%' && (*(format + 1) == 's' || *(format + 1) == 'd' || *(format + 1) == 'x' || *(format + 1) == 'l' || *(format + 1) == 'f'))
            {
                // handle case where format specifier is not followed by any argument
                throw std::runtime_error("Too few arguments for format string");
            }
            oss << *format++;
        }
    }

    template <typename T, typename... Args>
    void format_string(std::ostringstream &oss, const char *format, T value, Args... args)
    {
        while (*format != '\0')
        {
            if (*format == '%' && *(format + 1) == 's')
            {
                oss << value;
                format += 2;
                format_string(oss, format, args...);
                return;
            }
            else if (*format == '%' && *(format + 1) == 'd')
            {
                oss << std::dec << value;
                format += 2;
                format_string(oss, format, args...);
                return;
            }
            else if (*format == '%' && *(format + 1) == 'x')
            {
                oss << std::hex << value;
                format += 2;
                format_string(oss, format, args...);
                return;
            }
            else if (*format == '%' && *(format + 1) == 'l' && *(format + 2) == 'd')
            {
                if constexpr (std::is_same_v<T, long>)
                {
                    oss << std::dec << value;
                }
                else
                {
                    throw std::runtime_error("Invalid type for %ld format specifier");
                }
                format += 3;
                format_string(oss, format, args...);
                return;
            }
            else if (*format == '%' && *(format + 1) == 'f')
            {
                if constexpr (std::is_same_v<T, double>)
                {
                    oss << std::fixed << std::setprecision(6) << value;
                }
                else
                {
                    throw std::runtime_error("Invalid type for %f format specifier");
                }
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

// Macro to simplify logging calls
#define DEBUG(level, ...)                             \
    do                                                \
    {                                                 \
        if (level <= DEBUG_LEVEL)                     \
        {                                             \
            logger.log(level, __func__, __VA_ARGS__); \
        }                                             \
    } while (0)

extern Logger logger;

#endif // LOGGER_H


