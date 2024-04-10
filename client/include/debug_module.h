#pragma once

#include <string>

namespace Debug {
    enum class LogLevel {
        Info,
        Warning,
        Error,
    };

    void Log(const std::string &message, LogLevel level = LogLevel::Info);
}