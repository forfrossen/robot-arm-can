#ifndef RUN_MOTOR_IN_SPEED_MODE_COMMAND_H
#define RUN_MOTOR_IN_SPEED_MODE_COMMAND_H

#include "Command.h"
#include "../ServoWrapper.h"
#include "../Debug.h"

class RunMotorInSpeedModeCommand : public Command
{
private:
  Servo42D_CAN *servo;
  bool direction;
  uint16_t speed;
  uint8_t acceleration;

public:
  RunMotorInSpeedModeCommand(Servo42D_CAN *servo, bool direction, uint16_t speed, uint8_t acceleration)
      : servo(servo), direction(direction), speed(speed), acceleration(acceleration) {}

  void execute() override
  {
    Debug debug("RunMotorInSpeedModeCommand", __func__);
    if (speed > 3000)
    {
      speed = 3000;
    }

    uint8_t data[3];
    data[0] = 0xF6;
    data[1] = (direction ? 0x80 : 0x00) | ((speed >> 8) & 0x0F);
    data[2] = speed & 0xFF;
    data[3] = acceleration;

    debug.info();
    debug.add(F("Running motor in speed mode: "));
    debug.add(speed);
    debug.add(F(" RPM, acceleration: "));
    debug.add(acceleration);
    debug.add(F(", direction: "));
    debug.print(direction ? F("CW") : F("CCW"));

    servo->sendCommand(data, 3);
  }
};
#endif // RUN_MOTOR_IN_SPEED_MODE_COMMAND_H