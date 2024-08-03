#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <Arduino.h>
#include <vector>

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
  std::vector<String> messageBuffer;

public:
  Debug(const char *className, const char *functionName) : className(className), functionName(functionName) {}

  void add(const String &message)
  {
    messageBuffer.push_back(message);
  }

  void add(const char *message)
  {
    messageBuffer.push_back(String(message));
  }

  void add(const __FlashStringHelper *message)
  {
    messageBuffer.push_back(String(message));
  }

  template <typename T>
  void add(T value, int format = DEC)
  {
    messageBuffer.push_back(String(value, format));
  }

  void print(const String &message)
  {
    messageBuffer.push_back(message);
    println();
  }

  void print(const char *message)
  {
    messageBuffer.push_back(String(message));
    println();
  }

  void print(const __FlashStringHelper *message)
  {
    messageBuffer.push_back(String(message));
    println();
  }

  template <typename T>
  void print(T value, int format = DEC)
  {
    messageBuffer.push_back(String(value, format));
    println();
  }

  void println()
  {
    // if (classLoggingEnabled && functionLoggingEnabled && levelLoggingEnabled)
    for (const auto &msg : messageBuffer)
    {
      Serial.print(msg);
    }
    Serial.println();
    messageBuffer.clear();
  }

  static const LogLevel GlobalLogLevel = LogLevel::INFO;

  void log(const LogLevel logLevel)
  {
    // String logLevelName = LogLevelNames[logLevel];
    // String debugMessage = String("[") + logLevelName + String("]");
    //  sprintf(debugMessage, "[%s]%s::%s\t", logLevelName, className, methodName);
    //  sprintf(debugMessage, "[%s]", logLevelName);
    const char *logLevelName = LogLevelNames[logLevel];
    add(F("["));
    add(logLevelName);
    add(F("\t] "));

    add(className);
    add(F("::"));
    add(functionName);
    for (int i = strlen(className) + strlen(functionName); i <= 45; i++)
    {
      add(F(" "));
    }
    add(F("\t"));
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