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
    Serial.print(F("Error: "));

    switch (errorCode * -1)
    {
    case FSP_ERR_CAN_MODE_SWITCH_FAILED:
      Serial.println(F("Switching operation modes failed."));
      break;
    case FSP_ERR_CAN_INIT_FAILED:
      Serial.println(F("Hardware initialization failed."));
      break;
    case FSP_ERR_CAN_TRANSMIT_NOT_READY:
      Serial.println(F("Transmit in progress."));
      break;
    case FSP_ERR_CAN_RECEIVE_MAILBOX:
      Serial.println(F("Mailbox is setup as a receive mailbox."));
      break;
    case FSP_ERR_CAN_TRANSMIT_MAILBOX:
      Serial.println(F("Mailbox is setup as a transmit mailbox."));
      break;
    case FSP_ERR_CAN_MESSAGE_LOST:
      Serial.println(F("Receive message has been overwritten or overrun."));
      break;
    case FSP_ERR_CAN_TRANSMIT_FIFO_FULL:
      Serial.println(F("Transmit FIFO is full."));
      break;
    default:
      Serial.println(F("Unknown error."));
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
      Serial.println(F("CAN BUS could not be initialized!"));
      for (;;)
      {
      }
    }
    else
    {
      debug.info();
      Serial.println(F("CAN BUS initialized!"));
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
    Serial.print("ID: ");
    Serial.print(id, HEX);
    Serial.print("\tLength: ");
    Serial.print(length);
    Serial.print("\tsizeOf data: ");
    Serial.print(sizeof(data));

    uint8_t crc = calculateCRC(id, data, length);
    Serial.print("\tCRC: ");
    Serial.println(crc, HEX);

    uint8_t extendedData[length + 1];                 // +1 for CRC
    memcpy(extendedData, data, length);               // Copy original data
    memcpy(extendedData + length, &crc, sizeof(crc)); // Copy CRC to the end

    debug.info();
    Serial.print("data: ");
    for (int i = 0; i < length + 1; i++)
    {
      Serial.print(extendedData[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    CanMsg const msg(CanStandardId(id), sizeof(extendedData), extendedData);

    if (int const rc = CAN.write(msg); rc < 0)
    {
      debug.error();
      Serial.print(F("Error sending message. code: "));
      Serial.println(rc);
      printCANStatus(rc);
      return false;
    }

    msg_cnt++;
    return true;
  }
};

#endif // CANBUS_H
