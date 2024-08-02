#ifndef SET_HOLDING_CURRENT_COMMAND_H
#define SET_HOLDING_CURRENT_COMMAND_H

#include "Command.h"
#include "Servo42D_CAN.h"

class SetHoldingCurrentCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint8_t holdCurrent;

public:
  SetHoldingCurrentCommand(Servo42D_CAN *servo, uint8_t holdCurrent)
      : servo(servo), holdCurrent(holdCurrent) {}

  void execute() override
  {
    Debug debug("SetHoldingCurrentCommand", __func__);
    uint8_t data[2];
    data[0] = 0x9B; // Set Holding Current command code
    data[1] = holdCurrent;

    debug.info();
    Serial.print(F("Setting Holding Current: "));
    Serial.println(holdCurrent);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_HOLDING_CURRENT_COMMAND_H
