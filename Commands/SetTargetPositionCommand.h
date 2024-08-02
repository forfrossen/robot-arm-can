#ifndef SET_TARGET_POSITION_COMMAND_H
#define SET_TARGET_POSITION_COMMAND_H

#include "Command.h"
#include "../ServoWrapper.h"
#include "../Debug.h"
class SetTargetPositionCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint32_t position;
  uint8_t speed;
  uint8_t acceleration;
  bool absolute;

public:
  SetTargetPositionCommand(Servo42D_CAN *servo, uint32_t position, uint8_t speed = 100, uint8_t acceleration = 5, bool absolute = true)
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
    Serial.print(F("Setting target position: "));
    Serial.print(position);
    Serial.print(F(", Speed: "));
    Serial.print(speed);
    Serial.print(F(", Acceleration: "));
    Serial.print(acceleration);
    Serial.print(F(", Mode: "));
    Serial.println(absolute ? F("Absolute") : F("Relative"));

    servo->sendCommand(data, 7);
  }
};

#endif // SET_TARGET_POSITION_COMMAND_H