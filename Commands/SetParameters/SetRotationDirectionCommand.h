#ifndef SET_ROTATION_DIRECTION_COMMAND_H
#define SET_ROTATION_DIRECTION_COMMAND_H

#include "Command.h"
#include "Servo42D_CAN.h"

class SetRotationDirectionCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint8_t direction;

public:
  SetRotationDirectionCommand(Servo42D_CAN *servo, uint8_t direction)
      : servo(servo), direction(direction) {}

  void execute() override
  {
    Debug debug("SetRotationDirectionCommand", __func__);
    uint8_t data[2];
    data[0] = 0x86; // Set Rotation Direction command code
    data[1] = direction;

    debug.info();
    Serial.print(F("Setting Rotation Direction: "));
    Serial.println(direction);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_ROTATION_DIRECTION_COMMAND_H
