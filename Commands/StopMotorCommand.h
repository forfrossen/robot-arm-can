#ifndef STOP_MOTOR_COMMAND_H
#define STOP_MOTOR_COMMAND_H

#include "Command.h"
#include "..\Servo.h"
#include "../Debug.h"

class StopMotorCommand : public Command
{
private:
  Servo *servo;

public:
  StopMotorCommand(Servo *servo) : servo(servo) {}

  void execute() override
  {
    Debug debug("StopMotorCommand", __func__);
    uint8_t data[4] = {0xF7};

    debug.info();
    debug.print(F("Stopping motor"));

    servo->sendCommand(data, 1);
  }
};

#endif // STOP_MOTOR_COMMAND_H