#include <debug_module.h>
#include <iostream>

std::string LogLevelToString(Debug::LogLevel level) {
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

    result = "[" + result + "]";
    
    return result;
}

void Debug::Log(const std::string &message, Debug::LogLevel level) {
    std::cout << LogLevelToString(level) << " - " << message << std::endl;
}