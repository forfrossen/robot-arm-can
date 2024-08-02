#ifndef RESPONSE_HANDLERS_H
#define RESPONSE_HANDLERS_H

#include <Arduino.h>

void handleSetWorkModeResponse(uint8_t *data, uint8_t length)
{
  if (data[1] == 1)
  {
    Serial.println(F("Set Work Mode: Success"));
  }
  else
  {
    Serial.println(F("Set Work Mode: Failed"));
  }
}

void handleSetCurrentResponse(uint8_t *data, uint8_t length)
{
  if (data[1] == 1)
  {
    Serial.println(F("Set Current: Success"));
  }
  else
  {
    Serial.println(F("Set Current: Failed"));
  }
}

#endif // RESPONSE_HANDLERS_H