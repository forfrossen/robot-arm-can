#ifndef SET_ENABLE_PIN_COMMAND_H
#define SET_ENABLE_PIN_COMMAND_H

#include "../Command.h"
#include "../../ServoWrapper.h"
#include "../../Debug.h"

class SetEnablePinCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint8_t enable;

public:
  SetEnablePinCommand(Servo42D_CAN *servo, uint8_t enable)
      : servo(servo), enable(enable) {}

  void execute() override
  {
    Debug debug("SetEnablePinCommand", __func__);
    uint8_t data[2];
    data[0] = 0x85; // Set Enable Pin command code
    data[1] = enable;

    debug.info();
    Serial.print(F("Setting Enable Pin: "));
    Serial.println(enable);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_ENABLE_PIN_COMMAND_H
