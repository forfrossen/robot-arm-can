#ifndef CANBUS_H
#define CANBUS_H

// #include <mcp_can.h>
#include <mcp2515.h>
#include <SPI.h>

#define CAN_CS 10
#define CAN_INT 2 // Set INT to pin 2
#define CAN_SPEED CAN_500KBPS
#define CLOCK_SPEED MCP_8MHZ
#define MAX_CALLBACKS 10

class CANBus
{
private:
  // MCP_CAN mcp2515;
  MCP2515 mcp2515;
  typedef void (*CallbackType)(uint32_t, uint8_t, uint8_t *);
  CallbackType callbacks[MAX_CALLBACKS];
  int callbackCount;

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
  CANBus(int csPin) : mcp2515(csPin), callbackCount(0) {}

  void begin()
  {
    mcp2515.reset();
    Serial.println("================================");
    if (mcp2515.setBitrate(CAN_SPEED, CLOCK_SPEED) == MCP2515::ERROR_OK)
    {
      Serial.println("CAN BUS Shield init ok!");
      pinMode(CAN_INT, INPUT);
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
    Serial.println("================================");
  }

  byte calculateCRC(uint32_t id, const byte *data, uint8_t length)
  {
    byte crc = 0xFF;
    crc ^= (id >> 24) & 0xFF; // Process each byte of the ID
    crc ^= (id >> 16) & 0xFF;
    crc ^= (id >> 8) & 0xFF;
    crc ^= id & 0xFF;
    for (int i = 0; i < length; ++i)
    {
      crc ^= data[i];
    }
    return crc;
  }

  bool sendCANMessage(uint32_t id, uint8_t length, uint8_t *data)
  {
    uint8_t crc = calculateCRC(id, data, length); // Calculate CRC

    if (length < 8)
    {                     // Ensure there is space for the CRC
      data[length] = crc; // Append CRC to the end of data
    }

    // data[length] = calculateCRC(id, data, length); // Append the CRC to the end of the data

    struct can_frame frame;
    frame.can_id = id;
    frame.can_dlc = length;

    // Ensure not to exceed CAN frame data length limit
    if (frame.can_dlc > 8)
    {
      frame.can_dlc = 8;
    }

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
      // Serial.print(id);
      // Serial.print(" ");
      for (int i = 0; i < length; i++)
      {
        Serial.print(data[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      return false;
    }
  }

  void registerCallback(CallbackType callback)
  {
    if (callbackCount < MAX_CALLBACKS)
    {
      callbacks[callbackCount++] = callback;
    }
  }

  void listenForMessages()
  {
    struct can_frame frame;
    if (mcp2515.readMessage(&frame) == MCP2515::ERROR_OK)
    {

      for (int i = 0; i < callbackCount; i++)
      {
        callbacks[i](frame.can_id, frame.can_dlc, frame.data);
      }
      Serial.println();
      Serial.println();
      Serial.println("************************************************");
      Serial.println();
      Serial.println("class [Servo42D_CAN] - fn: [handleReceivedMessage]");
      Serial.print("ID: ");
      Serial.println(frame.can_id, HEX);
      Serial.print("Length: ");
      Serial.println(frame.can_dlc);
      Serial.print("Data: ");
      for (int i = 0; i < frame.can_dlc; i++)
      {
        Serial.print(frame.data[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      Serial.println("************************************************");
      Serial.println();
      Serial.println();
    }
  }
};

#endif // CANBUS_H