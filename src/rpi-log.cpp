#include <memory>
#include <string>
#include <sstream>
#include <cstdarg>
#include <iostream>

#include "rpi-log.h"

RpiLogger *RpiLogger::RpiLogger_;

RpiLogger *RpiLogger::GetInstance() {
    if (!RpiLogger_) {
        RpiLogger_ = new RpiLogger;
    }
}

void RpiLogger::Log(std::string file, int line, std::string group, LogLevel level, ...) {
    if (level < globalLevel_) return;

    std::stringstream log;

    log << "[" << group << "][" << file << "][Line: " << line << "] ";
    std::cout << log.str().c_str();

    va_list argptr;
    va_start(argptr, level);
    std::string str = va_arg(argptr, char *);
    vfprintf(stdout, str.c_str(), argptr);
    va_end(argptr);

    std::cout << std::endl;
}