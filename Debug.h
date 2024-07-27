#ifndef DEBUGGER_H
#define DEBUGGER_H

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

public:
  Debug(const char *name) : className(name) {}

  static const LogLevel GlobalLogLevel = LogLevel::INFO;

  void log(const char *methodName, const LogLevel logLevel, const char *message)
  {
    // String logLevelName = LogLevelNames[logLevel];
    // String debugMessage = String("[") + logLevelName + String("]");
    //  sprintf(debugMessage, "[%s]%s::%s\t", logLevelName, className, methodName);
    //  sprintf(debugMessage, "[%s]", logLevelName);
    const char *logLevelName = LogLevelNames[logLevel];
    Serial.print(F("["));
    Serial.print(logLevelName);
    Serial.print(F("] "));
    Serial.print(className);
    Serial.print("::");
    Serial.print(methodName);
    Serial.print("\t");
    Serial.print(message);
    Serial.println();
    // Serial.print(debugMessage);
  }

  void info(const char *methodName, const char *message)
  {
    if (GlobalLogLevel == LogLevel::INFO)
    {
      log(methodName, LogLevel::INFO, message);
    }
  }

  void warning(const char *methodName, const char *message)
  {
    if (GlobalLogLevel == LogLevel::INFO || GlobalLogLevel == LogLevel::WARNING)
    {
      log(methodName, LogLevel::WARNING, message);
    }
  }

  void error(const char *methodName, const char *message)
  {
    log(methodName, LogLevel::ERROR, message);
  }

  String getLogLevelName(LogLevel logLevel)
  {
    switch (logLevel)
    {
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARNING:
      return "WARNING";
    case LogLevel::ERROR:
      return "ERROR";
    }
  }
};

#endif