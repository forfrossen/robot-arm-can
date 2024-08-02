#ifndef SET_CAN_BITRATE_COMMAND_H
#define SET_CAN_BITRATE_COMMAND_H

#include "../Command.h"
#include "../../ServoWrapper.h"
#include "../../Debug.h"

class SetCANBitRateCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint8_t bitRate;

public:
  SetCANBitRateCommand(Servo42D_CAN *servo, uint8_t bitRate)
      : servo(servo), bitRate(bitRate) {}

  void execute() override
  {
    Debug debug("SetCANBitRateCommand", __func__);
    uint8_t data[2];
    data[0] = 0x8A; // Set CAN Bit Rate command code
    data[1] = bitRate;

    debug.info();
    Serial.print(F("Setting CAN Bit Rate: "));
    Serial.println(bitRate);

    servo->sendCommand(data, 2);
  }
};

#endif // SET_CAN_BITRATE_COMMAND_H
