#include <debug_module.h>
#include <stdio.h>
#include <windows.h>
#include <string>

std::string LogLevelToTString(Debug::LogLevel level) {
    std::string result;

    switch (level)
    {
    case Debug::LogLevel::Info:
        result = "i";
        break;
    
    case Debug::LogLevel::Warning:
        result = "!";
        break;
    
    case Debug::LogLevel::Error:
        result = "X";
        break;
    
    default:
        result = "unknown";
        break;
    }
    
    return result;
}

void Debug::LogA(const std::string &message, const Debug::LogLevel level) {
    printf("[%s] - %s\n", LogLevelToTString(level).c_str(), message.c_str());
}

void Debug::LogW(const std::wstring &message, const LogLevel level)
{
    std::string levelString = LogLevelToTString(level);
    std::wstring levelWideString(levelString.begin(), levelString.end());

    wprintf(L"[%s] - %s\n", levelWideString.c_str(), message.c_str());
}
