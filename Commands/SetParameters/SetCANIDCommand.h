#ifndef SET_CAN_ID_COMMAND_H
#define SET_CAN_ID_COMMAND_H

#include "../Command.h"
#include "../../ServoWrapper.h"
#include "../../Debug.h"

class SetCANIDCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint16_t canId;

public:
  SetCANIDCommand(Servo42D_CAN *servo, uint16_t canId)
      : servo(servo), canId(canId) {}

  void execute() override
  {
    Debug debug("SetCANIDCommand", __func__);
    uint8_t data[3];
    data[0] = 0x8B; // Set CAN ID command code
    data[1] = (canId >> 8) & 0xFF;
    data[2] = canId & 0xFF;

    debug.info();
    Serial.print(F("Setting CAN ID: "));
    Serial.println(canId, HEX);

    servo->sendCommand(data, 3);
  }
};

#endif // SET_CAN_ID_COMMAND_H
