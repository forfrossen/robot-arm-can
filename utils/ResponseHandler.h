#ifndef RESPONSE_HANDLERS_H
#define RESPONSE_HANDLERS_H

#include <Arduino.h>
#include "../Debug.h"

void handleSetWorkModeResponse(uint8_t *data, uint8_t length)
{
  Debug debug("ResponseHandlers", __func__);
  if (data[1] == 1)
  {
    debug.print(F("Set Work Mode: Success"));
  }
  else
  {
    debug.print(F("Set Work Mode: Failed"));
  }
}

void handleSetCurrentResponse(uint8_t *data, uint8_t length)
{
  Debug debug("ResponseHandlers", __func__);
  if (data[1] == 1)
  {
    debug.print(F("Set Current: Success"));
  }
  else
  {
    debug.print(F("Set Current: Failed"));
  }
}

#endif // RESPONSE_HANDLERS_H