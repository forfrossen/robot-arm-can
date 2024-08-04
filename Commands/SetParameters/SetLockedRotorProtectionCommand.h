#ifndef SET_LOCKED_ROTOR_PROTECTION_COMMAND_H
#define SET_LOCKED_ROTOR_PROTECTION_COMMAND_H

#include "../Command.h"
#include "..\..\Servo.h"
#include "../../Debug.h"

class SetLockedRotorProtectionCommand : public Command
{
private:
  Servo *servo;
  uint8_t enable;

public:
  SetLockedRotorProtectionCommand(Servo *servo, uint8_t enable)
      : servo(servo), enable(enable) {}

  void execute() override
  {
    Debug debug("SetLockedRotorProtectionCommand", __func__);
    uint8_t data[2];
    data[0] = 0x88; // Set Locked Rotor Protection command code
    data[1] = enable;

    debug.info();
    debug.add(F("Setting Locked Rotor Protection: "));
    debug.print(enable);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_LOCKED_ROTOR_PROTECTION_COMMAND_H
