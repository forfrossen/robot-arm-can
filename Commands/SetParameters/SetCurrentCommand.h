#ifndef SET_CURRENT_COMMAND_H
#define SET_CURRENT_COMMAND_H

#include "../Command.h"
#include "../../ServoWrapper.h"
#include "../../Debug.h"

class SetCurrentCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint16_t current;

public:
  SetCurrentCommand(Servo42D_CAN *servo, uint16_t current)
      : servo(servo), current(current) {}

  void execute() override
  {
    Debug debug("SetCurrentCommand", __func__);
    uint8_t data[3];
    data[0] = 0x83; // Set Current command code
    data[1] = (current >> 8) & 0xFF;
    data[2] = current & 0xFF;

    debug.info();
    Serial.print(F("Setting Current: "));
    Serial.println(current);

    servo->sendCommand(data, 3);
  }
};

#endif // SET_CURRENT_COMMAND_H
