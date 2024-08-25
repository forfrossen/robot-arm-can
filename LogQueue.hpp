#ifndef LOG_QUEUE_H
#define LOG_QUEUE_H

#include <Arduino_FreeRTOS.h>

class LogQueue
{
private:
  QueueHandle_t logQueue; // FreeRTOS Queue

public:
  // Constructor initializes the queue with a specified length and size per item
  LogQueue(size_t queueLength = 15)
  {
    logQueue = xQueueCreate(queueLength, sizeof(String));
    if (logQueue == nullptr)
    {
      Serial.println("Failed to create log queue");
    }
  }

  // Destructor to clean up the queue if needed
  ~LogQueue()
  {
    if (logQueue != nullptr)
    {
      vQueueDelete(logQueue);
    }
  }

  // Enqueue a message into the queue
  void enqueue(const String &message)
  {
    // Since we're passing a String object, we should make sure it's copied properly
    String messageCopy = message; // Create a local copy of the String

    if (xQueueSend(logQueue, &messageCopy, portMAX_DELAY) != pdPASS)
    {
      Serial.println("Failed to enqueue message");
    }
  }

  // Dequeue a message from the queue
  String dequeue()
  {
    String message;
    if (xQueueReceive(logQueue, &message, portMAX_DELAY) == pdPASS)
    {
      return message;
    }
    return ""; // Return an empty string if dequeue fails
  }

  // Check if the queue is empty (non-blocking check)
  bool isEmpty()
  {
    return uxQueueMessagesWaiting(logQueue) == 0;
  }
};

#endif // LOG_QUEUE_H