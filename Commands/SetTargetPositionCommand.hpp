#ifndef SET_TARGET_POSITION_COMMAND_H
#define SET_TARGET_POSITION_COMMAND_H

#include "Command.hpp"
#include "..\CANServo.hpp"
#include "..\Debug.hpp"
class SetTargetPositionCommand : public Command
{
private:
  CANServo *servo;
  uint32_t position;
  uint8_t speed;
  uint8_t acceleration;
  bool absolute;

public:
  SetTargetPositionCommand(CANServo *servo, int position, int speed = 100, int acceleration = 5, bool absolute = true)
      : servo(servo), position(position), speed(speed), acceleration(acceleration), absolute(absolute) {}

  void execute() override
  {
    Debug debug("SetTargetPositionCommand", __func__);
    uint8_t data[7];
    data[0] = absolute ? 0xF5 : 0xF4;  // Befehlscode für Position mode4: absolute motion by axis
    data[1] = (speed >> 8) & 0x7F;     // Combine direction bit with the upper 7 bits of speed
    data[2] = speed & 0xFF;            // Lower 8 bits of speed
    data[3] = acceleration;            // Beschleunigung
    data[4] = (position >> 16) & 0xFF; // Obere 8 Bits der Position
    data[5] = (position >> 8) & 0xFF;  // Mittlere 8 Bits der Position
    data[6] = position & 0xFF;         // Untere 8 Bits der Position
                                       // data[7] = calculateCRC(data, 8)

    debug.info();
    debug.add(F("Setting target position: "));
    debug.add(position);
    debug.add(F(", Speed: "));
    debug.add(speed);
    debug.add(F(", Acceleration: "));
    debug.add(acceleration);
    debug.add(F(", Mode: "));
    debug.print(absolute ? F("Absolute") : F("Relative"));

    servo->sendCommand(data, 7);
  }
};

#endif // SET_TARGET_POSITION_COMMAND_H