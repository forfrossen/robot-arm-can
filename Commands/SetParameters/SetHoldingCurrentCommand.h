#ifndef SET_HOLDING_CURRENT_COMMAND_H
#define SET_HOLDING_CURRENT_COMMAND_H

#include "../Command.h"
#include "..\..\CANServo.h"
#include "../../Debug.h"

class SetHoldingCurrentCommand : public Command
{
private:
  CANServo *servo;
  uint8_t holdCurrent;

public:
  SetHoldingCurrentCommand(CANServo *servo, uint8_t holdCurrent)
      : servo(servo), holdCurrent(holdCurrent) {}

  void execute() override
  {
    Debug debug("SetHoldingCurrentCommand", __func__);
    uint8_t data[2];
    data[0] = 0x9B; // Set Holding Current command code
    data[1] = holdCurrent;

    debug.info();
    debug.add(F("Setting Holding Current: "));
    debug.print(holdCurrent);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_HOLDING_CURRENT_COMMAND_H
