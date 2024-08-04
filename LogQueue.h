#ifndef LOG_QUEUE_H
#define LOG_QUEUE_H

#include <Arduino_FreeRTOS.h>
#include "lib/FreeRTOS-Kernel-v10.5.1/semphr.h"
#include <queue>

class LogQueue
{
private:
  std::queue<String> logQueue;
  SemaphoreHandle_t queueMutex;

public:
  LogQueue()
  {
    queueMutex = xSemaphoreCreateMutex();
  }

  void enqueue(const String &message)
  {
    xSemaphoreTake(queueMutex, portMAX_DELAY);
    logQueue.push(message);
    xSemaphoreGive(queueMutex);
  }

  String dequeue()
  {
    xSemaphoreTake(queueMutex, portMAX_DELAY);
    if (logQueue.empty())
    {
      xSemaphoreGive(queueMutex);
      return "";
    }
    String message = logQueue.front();
    logQueue.pop();
    xSemaphoreGive(queueMutex);
    return message;
  }

  bool isEmpty()
  {
    xSemaphoreTake(queueMutex, portMAX_DELAY);
    bool empty = logQueue.empty();
    xSemaphoreGive(queueMutex);
    return empty;
  }
};

#endif // LOG_QUEUE_H
