#include "Debug.h"

LogQueue *Debug::logQueue = nullptr;

Debug::Debug(const String className, const String &functionName)
    : className(className), functionName(functionName)
{
}

void Debug::setLogQueue(LogQueue *queue)
{
  logQueue = queue;
}

void Debug::add(const __FlashStringHelper *value)
{
  messageBuffer.push_back(value);
}

void Debug::add(const char *value)
{
  messageBuffer.push_back(String(value));
}

void Debug::add(const String &value)
{
  messageBuffer.push_back(value);
}

void Debug::print(const __FlashStringHelper *value)
{
  add(value);
  println();
}

void Debug::print(const char *value)
{
  add(String(value));
  println();
}

void Debug::print(const String &value)
{
  add(value);
  println();
}

void Debug::println()
{
  for (const auto &msg : messageBuffer)
  {
    Serial.print(msg);
  }
  Serial.println();
  messageBuffer.clear();
}

void Debug::log(const LogLevel logLevel)
{
  const char *logLevelName = LogLevelNames[logLevel];
  add(F("["));
  add(logLevelName);
  add(F("\t] "));

  add(className);
  add(F("::"));
  add(functionName);
  for (int i = className.length() + functionName.length(); i <= 45; i++)
  {
    add(F(" "));
  }
  add(F("\t"));
}

void Debug::info()
{
  if (GlobalLogLevel == LogLevel::INFO)
  {
    log(LogLevel::INFO);
  }
}

void Debug::warning()
{
  if (GlobalLogLevel == LogLevel::INFO || GlobalLogLevel == LogLevel::WARNING)
  {
    log(LogLevel::WARNING);
  }
}

void Debug::error()
{
  log(LogLevel::ERROR);
}
