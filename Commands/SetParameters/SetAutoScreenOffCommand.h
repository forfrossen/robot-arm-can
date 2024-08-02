#ifndef SET_AUTO_SCREEN_OFF_COMMAND_H
#define SET_AUTO_SCREEN_OFF_COMMAND_H

#include "../Command.h"
#include "../../ServoWrapper.h"
#include "../../Debug.h"

class SetAutoScreenOffCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint8_t enable;

public:
  SetAutoScreenOffCommand(Servo42D_CAN *servo, uint8_t enable)
      : servo(servo), enable(enable) {}

  void execute() override
  {
    Debug debug("SetAutoScreenOffCommand", __func__);
    uint8_t data[2];
    data[0] = 0x87; // Set Auto Screen Off command code
    data[1] = enable;

    debug.info();
    Serial.print(F("Setting Auto Screen Off: "));
    Serial.println(enable);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_AUTO_SCREEN_OFF_COMMAND_H
