#define INFO(fmt, ...) logging::log(logging::LogLevel::INFO, fmt, __VA_ARGS__)
#define WARN(fmt, ...) logging::log(logging::LogLevel::WARN, fmt, __VA_ARGS__)
#define DEBUG(fmt, ...) logging::log(logging::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define FATAL(fmt, ...) logging::log(logging::LogLevel::FATAL, fmt, __VA_ARGS__)

namespace logging {
    enum LogLevel {
        INFO,
        WARN,
        DEBUG,
        FATAL
    };

    void log(const LogLevel level, const char* fmt, ...);
}