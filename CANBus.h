#ifndef CANBUS_H
#define CANBUS_H

#include <mcp2515.h>
#include <SPI.h>

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
    uint8_t errorFlag = mcp2515.getErrorFlags();

    Serial.print("Error Flag: 0x");
    Serial.println(errorFlag, HEX);

    if (errorFlag & MCP2515::EFLG_RX0OVR)
      Serial.println("Receive Buffer 0 Overflow");
    if (errorFlag & MCP2515::EFLG_TXEP)
      Serial.println("Transmit Error Passive");
    if (errorFlag & MCP2515::EFLG_TXBO)
      Serial.println("Transmit Bus-Off");
    if (errorFlag & MCP2515::EFLG_RXEP)
      Serial.println("Receive Error Passive");
    if (errorFlag & MCP2515::EFLG_TXWAR)
      Serial.println("Transmit Warning");
    if (errorFlag & MCP2515::EFLG_RXWAR)
      Serial.println("Receive Warning");
    if (errorFlag & MCP2515::EFLG_EWARN)
      Serial.println("Error Warning");
  }

public:
  CANBus(int csPin) : mcp2515(csPin) {}

  void begin()
  {
    mcp2515.reset();
    if (mcp2515.setBitrate(CAN_SPEED, CLOCK_SPEED) == MCP2515::ERROR_OK)
    {
      Serial.println("CAN BUS Shield init ok!");
      pinMode(CAN_INT_PIN, INPUT);
      mcp2515.setNormalMode();
    }
    else
    {
      Serial.println("CAN BUS Shield init fail");
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
      Serial.println("Error sending message");
      printCANStatus(); // Print status and error flags

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

  bool listenForMessages(struct can_frame &frame)
  {
    if (mcp2515.readMessage(&frame) == MCP2515::ERROR_OK)
    {
      return true;
    }
    return false;
  }
};

#endif // CANBUS_H
