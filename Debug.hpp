#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <Arduino.h>
#include <vector>
#include "LogQueue.hpp"

enum LogLevel
{
  INFO,
  WARNING,
  ERROR
};

class Debug
{
private:
  const char *LogLevelNames[3] = {"INFO", "WARNING", "ERROR"};
  String className;
  String functionName;
  std::vector<String> messageBuffer;
  static LogQueue *logQueue;
  std::vector<String> disabledFunctions = {
      // "task_checkMessages",
      // "task_sendPositon",
      // "task_QueryPosition",
  };

public:
  Debug(const String className, const String &functionName);

  static const LogLevel GlobalLogLevel = LogLevel::INFO;

  static void setLogQueue(LogQueue *queue);

  template <typename T>
  void add(T value, int format = -1);
  void add(const __FlashStringHelper *value);
  void add(const char *value);
  void add(const String &value);

  template <typename T>
  void print(T value, int format = -1);
  void print(const __FlashStringHelper *value);
  void print(const char *value);
  void print(const String &value);

  void println();

  void log(const LogLevel logLevel);

  void info();

  void warning();

  void error();
};
#include "Debug.tpp"
#endif
