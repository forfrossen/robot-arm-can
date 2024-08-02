#ifndef SET_WORK_MODE_COMMAND_H
#define SET_WORK_MODE_COMMAND_H

#include "Command.h"
#include "Servo42D_CAN.h"

class SetWorkModeCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint8_t mode;

public:
  SetWorkModeCommand(Servo42D_CAN *servo, uint8_t mode)
      : servo(servo), mode(mode) {}

  void execute() override
  {
    Debug debug("SetWorkModeCommand", __func__);
    uint8_t data[2];
    data[0] = 0x82; // Set Work Mode command code
    data[1] = mode;

    debug.info();
    Serial.print(F("Setting Work Mode: "));
    Serial.println(mode);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_WORK_MODE_COMMAND_H
