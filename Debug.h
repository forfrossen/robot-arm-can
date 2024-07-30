#ifndef DEBUGGER_H
#define DEBUGGER_H
#include "Print.h"

enum LogLevel
{
  INFO,
  WARNING,
  ERROR
};

const char *LogLevelNames[3] = {"INFO", "WARNING", "ERROR"};

class Debug
{
private:
  const char *className;
  const char *functionName;

public:
  Debug(const char *className, const char *functionName) : className(className), functionName(functionName) {}

  static const LogLevel GlobalLogLevel = LogLevel::INFO;

  void log(const LogLevel logLevel)
  {
    // String logLevelName = LogLevelNames[logLevel];
    // String debugMessage = String("[") + logLevelName + String("]");
    //  sprintf(debugMessage, "[%s]%s::%s\t", logLevelName, className, methodName);
    //  sprintf(debugMessage, "[%s]", logLevelName);
    const char *logLevelName = LogLevelNames[logLevel];
    Serial.print(F("["));
    Serial.print(logLevelName);
    Serial.print(F("\t] "));

    Serial.print(className);
    Serial.print(F("::"));
    Serial.print(functionName);
    for (int i = strlen(className) + strlen(functionName); i <= 45; i++)
    {
      Serial.print(F(" "));
    }
    Serial.print(F("\t"));
  }

  void info()
  {
    if (GlobalLogLevel == LogLevel::INFO)
    {
      log(LogLevel::INFO);
    }
  }

  void warning()
  {
    if (GlobalLogLevel == LogLevel::INFO || GlobalLogLevel == LogLevel::WARNING)
    {
      log(LogLevel::WARNING);
    }
  }

  void error()
  {
    log(LogLevel::ERROR);
  }
};

#endif