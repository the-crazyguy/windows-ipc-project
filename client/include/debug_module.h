#pragma once

#include <string>

namespace Debug {
    enum class LogLevel {
        Info,
        Warning,
        Error,
    };

    void LogA(const std::string &message, const LogLevel level = LogLevel::Info);
    //Linker can't handle the basic_string<TCHAR> for some reason
    //void TLog(const std::basic_string<TCHAR> &message, const LogLevel level = LogLevel::Info);
    void LogW(const std::wstring &message, const LogLevel level = LogLevel::Info);
}