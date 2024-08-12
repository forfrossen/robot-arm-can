#ifndef SET_WORK_MODE_COMMAND_H
#define SET_WORK_MODE_COMMAND_H

#include "../Command.hpp"
#include "..\..\CANServo.hpp"
#include "..\..\Debug.hpp"

class SetWorkModeCommand : public Command
{
private:
  CANServo *servo;
  uint8_t mode;

public:
  SetWorkModeCommand(CANServo *servo, uint8_t mode)
      : servo(servo), mode(mode) {}

  void execute() override
  {
    Debug debug("SetWorkModeCommand", __func__);
    uint8_t data[2];
    data[0] = 0x82; // Set Work Mode command code
    data[1] = mode;

    debug.info();
    debug.add(F("Setting Work Mode: "));
    debug.print(mode);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_WORK_MODE_COMMAND_H
