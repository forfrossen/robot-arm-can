// CANBusController.h
#include <SPI.h>
#include "mcp_can.h"

#define CAN_CS 10
#define CAN_INT 2 // Set INT to pin 2
#define CAN_SPEED CAN_500KBPS
#define CLOCK_SPEED MCP_8MHZ

MCP_CAN CAN(CAN_CS);

class CANBusController
{
private:
  uint8_t cs_pin;

public:
  CANBusController();

  void static begin()
  {
    if (CAN.begin(CAN_SPEED, CLOCK_SPEED, CAN_INT) == CAN_OK)
    {
      Serial.println("CAN BUS Shield init ok!");
      CAN.setMode(MCP_NORMAL);
    }
    else
    {
      Serial.println("CAN BUS Shield init fail");
      Serial.println("Init CAN BUS Shield again");
      delay(100);
      begin();
    }
  }

  bool sendCANMessage(uint8_t id, uint8_t *data, uint8_t length)
  {
    if (CAN.sendMsgBuf(id, 0, length, data) == CAN_OK)
    {
      return true;
    }
    return false;
  }

  bool readCANMessage(uint8_t *data, uint8_t &length, uint32_t &id)
  {
    if (CAN.checkReceive() == CAN_MSGAVAIL)
    {
      CAN.readMsgBuf(&id, &length, data);
      return true;
    }
    return false;
  }
};