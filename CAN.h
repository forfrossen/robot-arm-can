#ifndef CANBUS_H
#define CANBUS_H
#include <Arduino_CAN.h>

#include "Debug.h"

#define MAX_CALLBACKS 10

class CANBus
{
private:
  uint32_t msg_cnt = 0;

  void printCANStatus(int errorCode)
  {
    Debug debug("CANBus", __func__);

    debug.error();
    debug.add(F("Error: "));

    switch (errorCode * -1)
    {
    case FSP_ERR_CAN_MODE_SWITCH_FAILED:
      debug.print(F("Switching operation modes failed."));
      break;
    case FSP_ERR_CAN_INIT_FAILED:
      debug.print(F("Hardware initialization failed."));
      break;
    case FSP_ERR_CAN_TRANSMIT_NOT_READY:
      debug.print(F("Transmit in progress."));
      break;
    case FSP_ERR_CAN_RECEIVE_MAILBOX:
      debug.print(F("Mailbox is setup as a receive mailbox."));
      break;
    case FSP_ERR_CAN_TRANSMIT_MAILBOX:
      debug.print(F("Mailbox is setup as a transmit mailbox."));
      break;
    case FSP_ERR_CAN_MESSAGE_LOST:
      debug.print(F("Receive message has been overwritten or overrun."));
      break;
    case FSP_ERR_CAN_TRANSMIT_FIFO_FULL:
      debug.print(F("Transmit FIFO is full."));
      break;
    default:
      debug.print(F("Unknown error."));
      break;
    }
  }

public:
  CANBus()
  {
  }

  void begin()
  {
    Debug debug("CANBus", "begin\t");
    if (!CAN.begin(CanBitRate::BR_500k))
    {
      debug.error();
      debug.print(F("CAN BUS could not be initialized!"));
      for (;;)
      {
      }
    }
    else
    {
      debug.info();
      debug.print(F("CAN BUS initialized!"));
    }
  }

  uint8_t calculateCRC(uint32_t id, const uint8_t *data, uint8_t length)
  {
    uint8_t crc = id;
    for (uint8_t i = 0; i < length; i++)
    {
      crc += data[i];
    }
    return crc & 0xFF;
  }

  bool sendCANMessage(uint32_t id, uint8_t length, uint8_t *data)
  {

    Debug debug("CANBus", __func__);
    debug.info();
    debug.add("ID: ");
    debug.add(id, HEX);
    debug.add("\tLength: ");
    debug.add(length);
    debug.add("\tsizeOf data: ");
    debug.add(sizeof(data));

    uint8_t crc = calculateCRC(id, data, length);
    debug.add("\tCRC: ");
    debug.print(crc, HEX);

    uint8_t extendedData[length + 1];                 // +1 for CRC
    memcpy(extendedData, data, length);               // Copy original data
    memcpy(extendedData + length, &crc, sizeof(crc)); // Copy CRC to the end

    debug.info();
    debug.add("data: ");
    for (int i = 0; i < length + 1; i++)
    {
      debug.add(extendedData[i], HEX);
      debug.add(" ");
    }
    debug.println();

    CanMsg const msg(CanStandardId(id), sizeof(extendedData), extendedData);

    if (int const rc = CAN.write(msg); rc < 0)
    {
      debug.error();
      debug.add(F("Error sending message. code: "));
      debug.print(rc);
      printCANStatus(rc);
      return false;
    }

    msg_cnt++;
    return true;
  }
};

#endif // CANBUS_H
