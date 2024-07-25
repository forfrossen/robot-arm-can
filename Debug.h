#ifndef DEBUGGER_H
#define DEBUGGER_H
#include <Arduino.h>

enum LogLevel
{
    "INFO",
    "WARNING",
    "ERROR"
};

class Debug
{
private:
    String className;

public:
    Debug(const String &name) : className(name) {}

    Static LogLevel GlobalLogLevel;

    void log(const String &methodName, enum LogLevel, const String &message) const
    {
        String debugMessage = "[ " + LogLevel.toUpperCase() + " ] " + className + ":" + methodName + " - " + message;
        Serial.println(debugMessage);
    }

    void info(const String &methodName, const String &message) const
    {
        if (GlobalLogLevel == "INFO")
        {
            log(methodName, LogLevel::INFO, message);
        }
    }

    void warning(const String &methodName, const String &message) const
    {
        if (GlobalLogLevel == "INFO" || GlobalLogLevel == "WARNING")
        {
            log(methodName, LogLevel::WARNING, message);
        }
    }

    void error(const String &methodName, const String &message) const
    {
        log(methodName, LogLevel::ERROR, message);
    }
};

#endif