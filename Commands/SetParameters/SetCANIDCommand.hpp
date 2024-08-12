#ifndef SET_CAN_ID_COMMAND_H
#define SET_CAN_ID_COMMAND_H

#include "../Command.hpp"
#include "..\..\CANServo.hpp"
#include "..\..\Debug.hpp"

class SetCANIDCommand : public Command
{
private:
  CANServo *servo;
  uint16_t canId;

public:
  SetCANIDCommand(CANServo *servo, uint16_t canId)
      : servo(servo), canId(canId) {}

  void execute() override
  {
    Debug debug("SetCANIDCommand", __func__);
    uint8_t data[3];
    data[0] = 0x8B; // Set CAN ID command code
    data[1] = (canId >> 8) & 0xFF;
    data[2] = canId & 0xFF;

    debug.info();
    debug.add(F("Setting CAN ID: "));
    debug.print(canId, HEX);

    servo->sendCommand(data, 3);
  }
};

#endif // SET_CAN_ID_COMMAND_H
