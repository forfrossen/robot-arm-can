#ifndef SET_SUBDIVISION_INTERPOLATION_COMMAND_H
#define SET_SUBDIVISION_INTERPOLATION_COMMAND_H

#include "Command.h"
#include "Servo42D_CAN.h"

class SetSubdivisionInterpolationCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint8_t enable;

public:
  SetSubdivisionInterpolationCommand(Servo42D_CAN *servo, uint8_t enable)
      : servo(servo), enable(enable) {}

  void execute() override
  {
    Debug debug("SetSubdivisionInterpolationCommand", __func__);
    uint8_t data[2];
    data[0] = 0x89; // Set Subdivision Interpolation command code
    data[1] = enable;

    debug.info();
    Serial.print(F("Setting Subdivision Interpolation: "));
    Serial.println(enable);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_SUBDIVISION_INTERPOLATION_COMMAND_H
