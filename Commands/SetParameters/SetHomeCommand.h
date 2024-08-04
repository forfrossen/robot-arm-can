#ifndef SET_HOME_COMMAND_H
#define SET_HOME_COMMAND_H

#include "../Command.h"
#include "..\..\Servo.h"
#include "../../Debug.h"
class SetHomeCommand : public Command
{
private:
  Servo *servo;
  uint8_t homeTrig;
  uint8_t homeDir;
  uint16_t homeSpeed;
  uint8_t endLimit;

public:
  SetHomeCommand(Servo *servo, uint8_t homeTrig, uint8_t homeDir, uint16_t homeSpeed, uint8_t endLimit)
      : servo(servo), homeTrig(homeTrig), homeDir(homeDir), homeSpeed(homeSpeed), endLimit(endLimit) {}

  void execute() override
  {
    Debug debug("SetHomeCommand", __func__);

    uint8_t data[6];                   // Command code + parameters + CRC
    data[0] = 0x90;                    // Set Home command code
    data[1] = homeTrig;                // Home trigger level: 0 = Low, 1 = High
    data[2] = homeDir;                 // Home direction: 0 = CW, 1 = CCW
    data[3] = (homeSpeed >> 8) & 0xFF; // High byte of home speed
    data[4] = homeSpeed & 0xFF;        // Low byte of home speed
    data[5] = endLimit;                // End limit: 0 = disable, 1 = enable

    debug.info();
    debug.add(F("ID: "));
    debug.add(servo->canId, HEX);
    debug.add(F(", Home Trigger: "));
    debug.add(homeTrig ? F("High") : F("Low"));
    debug.add(F(", Home Direction: "));
    debug.add(homeDir ? F("CCW") : F("CW"));
    debug.add(F(", Home Speed: "));
    debug.add(homeSpeed);
    debug.add(F(", End Limit: "));
    debug.print(endLimit ? F("Enabled") : F("Disabled"));

    servo->sendCommand(data, 6);
  }
};
#endif // SET_HOME_COMMAND_H