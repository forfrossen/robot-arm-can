#ifndef SET_AUTO_SCREEN_OFF_COMMAND_H
#define SET_AUTO_SCREEN_OFF_COMMAND_H

#include "../Command.hpp"
#include "..\..\CANServo.hpp"
#include "..\..\Debug.hpp"

class SetAutoScreenOffCommand : public Command
{
private:
  CANServo *servo;
  uint8_t enable;

public:
  SetAutoScreenOffCommand(CANServo *servo, uint8_t enable)
      : servo(servo), enable(enable) {}

  void execute() override
  {
    Debug debug("SetAutoScreenOffCommand", __func__);
    uint8_t data[2];
    data[0] = 0x87; // Set Auto Screen Off command code
    data[1] = enable;

    debug.info();
    debug.add(F("Setting Auto Screen Off: "));
    debug.print(enable);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_AUTO_SCREEN_OFF_COMMAND_H
