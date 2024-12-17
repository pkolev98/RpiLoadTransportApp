#pragma once

#include <string>
#include <memory>

#define RPI_SET_LOG_LEVEL(LVL) ( RpiLogger::GetInstance() )->setLogLevel(LVL)

#define RPI_LOG(group, level, ...) ( RpiLogger::GetInstance() )->Log(__FILE__, __LINE__, group, level, __VA_ARGS__)

enum LogLevel {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class RpiLogger {
public:
    static RpiLogger *GetInstance();

    ~RpiLogger() {}

    void Log(std::string file, int line, std::string group, LogLevel level, ...);

    void setLogLevel(LogLevel level);
private:
    RpiLogger() {}

    static RpiLogger *RpiLogger_;
    LogLevel globalLevel_{DEBUG};
};