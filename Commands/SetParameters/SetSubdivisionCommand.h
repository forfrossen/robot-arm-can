#ifndef SET_SUBDIVISION_COMMAND_H
#define SET_SUBDIVISION_COMMAND_H

#include "Command.h"
#include "Servo42D_CAN.h"

class SetSubdivisionCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint8_t subdivision;

public:
  SetSubdivisionCommand(Servo42D_CAN *servo, uint8_t subdivision)
      : servo(servo), subdivision(subdivision) {}

  void execute() override
  {
    Debug debug("SetSubdivisionCommand", __func__);
    uint8_t data[2];
    data[0] = 0x84; // Set Subdivision command code
    data[1] = subdivision;

    debug.info();
    Serial.print(F("Setting Subdivision: "));
    Serial.println(subdivision);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_SUBDIVISION_COMMAND_H
