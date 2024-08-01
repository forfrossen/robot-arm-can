#ifndef CANBUS_H_old
#define CANBUS_H_old

#include <mcp2515.h>
#include <SPI.h>
#include "Debug.h"

#define CAN_CS_PIN 10
#define CAN_INT_PIN 2
#define CAN_SPEED CAN_500KBPS
#define CLOCK_SPEED MCP_8MHZ
#define MAX_CALLBACKS 10

class CANBus
{
private:
  MCP2515 mcp2515;

  void printCANStatus()
  {
    Debug debug("CANBus", __func__);
    uint8_t errorFlag = mcp2515.getErrorFlags();

    debug.error();
    Serial.print(F("Error Flag: 0x"));
    Serial.println(errorFlag, HEX);
    debug.error();
    Serial.print(F("Errors:"));

    if (errorFlag & MCP2515::EFLG_RX0OVR)
      Serial.print(F(" Receive Buffer 0 Overflow"));
    if (errorFlag & MCP2515::EFLG_TXEP)
      Serial.print(F(", Transmit Error Passive"));
    if (errorFlag & MCP2515::EFLG_TXBO)
      Serial.print(F(", Transmit Bus-Off"));
    if (errorFlag & MCP2515::EFLG_RXEP)
      Serial.print(F(", Receive Error Passive"));
    if (errorFlag & MCP2515::EFLG_TXWAR)
      Serial.print(F(", Transmit Warning"));
    if (errorFlag & MCP2515::EFLG_RXWAR)
      Serial.print(F(", Receive Warning"));
    if (errorFlag & MCP2515::EFLG_EWARN)
      Serial.print(F(", Error Warning"));

    Serial.println();
  }

public:
  CANBus(int csPin) : mcp2515(csPin)
  {
  }

  void begin()
  {
    Debug debug("CANBus", "begin\t");
    mcp2515.reset();

    if (mcp2515.setBitrate(CAN_SPEED, CLOCK_SPEED) == MCP2515::ERROR_OK)
    {
      pinMode(CAN_INT_PIN, INPUT);
      mcp2515.setNormalMode();
      debug.info();
      Serial.println(F("CAN BUS Shield init ok!"));
    }
    else
    {
      debug.error();
      Serial.println(F("CAN BUS Shield could not be initialized!"));
      while (true)
      {
        delay(100);
      }
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
    if (length >= 7)
    {
      length = 7; // Limit length to 7 to make space for CRC
    }

    uint8_t crc = calculateCRC(id, data, length); // Calculate CRC
    data[length] = crc;                           // Append CRC to the end of data

    struct can_frame frame;
    frame.can_id = id;
    frame.can_dlc = length + 1; // Include CRC in the length

    memcpy(frame.data, data, frame.can_dlc);

    if (mcp2515.sendMessage(&frame) == MCP2515::ERROR_OK)
    {
      return true;
    }
    else
    {
      debug.error();
      Serial.println(F("Error sending message"));
      printCANStatus(); // Print status and error flags

      debug.error();
      Serial.print("Data: ");
      for (int i = 0; i < frame.can_dlc; i++)
      {
        Serial.print(frame.data[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      return false;
    }
  }

  bool listenForMessages(struct can_frame *frame)
  {
    if (mcp2515.readMessage(frame) == MCP2515::ERROR_OK)
    {
      return true;
    }
    return false;
  }
};

#endif // CANBUS_H_old_old
