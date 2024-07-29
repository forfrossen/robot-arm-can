#ifndef SERVO42D_CAN_H
#define SERVO42D_CAN_H

#include "CANBus.h" // Include CANBus.h with include guards
#include "commandMapper.h"
#include "Debug.h"
#include <mcp2515.h>

#define MAX_PROCESSED_MESSAGES 10 // Define the maximum number of processed messages to track

class Servo42D_CAN
{
private:
  CANBus *canBus;
  CommandMapper *commandMapper;
  int32_t CarryValue = 0;
  uint16_t EncoderValue = 0;
  String F5Status = "";

public:
  canid_t canId;

  Servo42D_CAN(canid_t id, CANBus *bus, CommandMapper *commandMapper) : canId(id), canBus(bus), commandMapper(commandMapper)
  {
    Debug debug("Servo42D_CAN", __func__);
    debug.info();
    Serial.print(F("New Servo42D_CAN object created with CAN ID: "));
    Serial.println(canId, HEX);
  }

  // void handleReceivedMessage(uint32_t id, uint8_t length, const uint8_t *data)
  void handleReceivedMessage(struct can_frame frame)
  {
    Debug debug("Servo42D_CAN", __func__);
    __u8 code = frame.data[0];

    if (!code)
    {
      debug.error();
      Serial.println(F("Error: code is empty!"));
      return;
    }

    char commandName[50];
    commandMapper->getCommandNameFromCode(code, commandName);

    debug.info();
    Serial.print("ID: ");
    Serial.print(canId);
    Serial.print("\t length: ");
    Serial.print(frame.can_dlc, HEX);
    Serial.print("\tcode: ");
    Serial.print(frame.data[0], HEX);
    Serial.print("\tcommandName: ");
    Serial.println(commandName);

    if (frame.can_id == canId)
    {
      decodeMessage(frame.data, frame.can_dlc);
    }
  }

  // Method to send CAN message
  void sendCommand(uint8_t *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    __u8 code = data[0];

    if (!code)
    {
      debug.error();
      Serial.println(F("Error: code is empty!"));
      return;
    }

    char commandName[50];
    commandMapper->getCommandNameFromCode(code, commandName);

    debug.info();
    Serial.print("ID: ");
    Serial.print(canId, HEX);
    Serial.print("\t length: ");
    Serial.print(length);
    Serial.print("\t code: ");
    Serial.print(data[0]);
    Serial.print("\t commandName: ");
    Serial.println(commandName);

    if (canBus->sendCANMessage(canId, length, data))
    {
      debug.info();
      Serial.println("    ==> Message sent successfully!");
    }
    else
    {
      debug.error();
      Serial.println("    ==> Error sending message!");
    }
  }

  // Set Speed and Acceleration together
  void setSpeedAndAcceleration(uint16_t speed, uint8_t acceleration, bool direction = true)
  {
    Debug debug("Servo42D_CAN", __func__);
    if (speed > 3000)
    {
      speed = 3000; // Clamp speed to maximum value
    }

    uint8_t data[5];
    data[0] = 0xF6;                                              // Command code for setting speed and acceleration
    data[1] = (direction ? 0x80 : 0x00) | ((speed >> 8) & 0x0F); // Direction bit in the highest bit and upper 4 bits of speed
    data[2] = speed & 0xFF;                                      // Lower 8 bits of speed
    data[3] = acceleration;                                      // Acceleration
                                                                 // data[4] = calculateCRC(data, 5);   // CRC including CAN ID (0x01)
    debug.info();
    Serial.print("ID: ");
    Serial.print(canId, HEX);
    Serial.print(", Speed: ");
    Serial.print(speed);
    Serial.print(", Acceleration: ");
    Serial.print(acceleration);
    Serial.print(", Direction: ");
    Serial.println(direction ? "CW" : "CCW");
    sendCommand(data, 4);
  }

  void setTargetPosition(uint32_t position, uint8_t speed = 100, uint8_t acceleration = 5, bool absolute = true)
  {
    Debug debug("Servo42D_CAN", __func__);
    uint8_t data[8];
    data[0] = absolute ? 0xF5 : 0xF4;  // Befehlscode für Position mode4: absolute motion by axis
    data[1] = (speed >> 8) & 0x7F;     // Combine direction bit with the upper 7 bits of speed
    data[2] = speed & 0xFF;            // Lower 8 bits of speed
    data[3] = acceleration;            // Beschleunigung
    data[4] = (position >> 16) & 0xFF; // Obere 8 Bits der Position
    data[5] = (position >> 8) & 0xFF;  // Mittlere 8 Bits der Position
    data[6] = position & 0xFF;         // Untere 8 Bits der Position
                                       // data[7] = calculateCRC(data, 8);   // Checksumme oder reserved (kann auf 0 gesetzt werden)
    debug.info();
    Serial.print(canId, HEX);
    Serial.print(", Position: ");
    Serial.print(position);
    Serial.print(", Geschwindigkeit: ");
    Serial.print(speed);
    Serial.print(", Beschleunigung: ");
    Serial.print(acceleration);
    Serial.print(", Modus: ");
    Serial.println(absolute ? "Absolut" : "Relativ");

    sendCommand(data, 7);
  }

  void stopMotor()
  {
    uint8_t data[4] = {0xF7, 0x00, 0x00};
    sendCommand(data, 4);
  }

  void enableMotor()
  {
    // F3 = Enable Command
    // 01 = Enable / O0 = Disable
    uint8_t data[3] = {0xF3, 0x01, 0x00};
    sendCommand(data, 3);

    // 85 = set enable pin
    // enable = 00 active low (L)
    // enable = 01 active high (H)
    // enable = 02 active always (Hold)
    uint8_t data2[3] = {0x85, 0x02, 0x00};
    sendCommand(data, 3);
  }

  void queryMotorStatus()
  {
    uint8_t data[2] = {0xF1, 0x00};

    sendCommand(data, 2);
  }

  void queryMotorPosition()
  {
    uint8_t data[1] = {0x30};

    sendCommand(data, 2);
  }

  void interpreteStatusResponse(uint8_t *data)
  {
    Debug debug("Servo42D_CAN", __func__);
    debug.info();
    // Empfange Daten verarbeiten
    uint8_t status = data[1];

    switch (status)
    {
    case 0:
      Serial.println("Abfrage fehlgeschlagen");
      break;
    case 1:
      Serial.println("Motor gestoppt");
      break;
    case 2:
      Serial.println("Motor beschleunigt");
      break;
    case 3:
      Serial.println("Motor verlangsamt");
      break;
    case 4:
      Serial.println("Motor volle Geschwindigkeit");
      break;
    case 5:
      Serial.println("Motor wird geparkt");
      break;
    case 6:
      Serial.println("Motor wird kalibriert");
      break;
    default:
      Serial.println("Unbekannter Status");
      break;
    }
  }

  void decodeEncoderValue(const __u8 *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    if (length != 8 || data[0] != 0x30)
    {
      debug.error();
      Serial.println("Invalid response length or code.");
      return;
    }

    CarryValue = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
    EncoderValue = (data[5] << 8) | data[6];
    uint8_t crc = data[7];

    debug.info();
    Serial.print("Carry value: ");
    Serial.println(CarryValue);

    debug.info();
    Serial.print("Encoder value: ");
    Serial.println(EncoderValue);
  }

  void handleSetPositionResponse(const __u8 *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    if (length != 3)
    {

      debug.error();
      Serial.println("Invalid response length.");
      return;
    }
    if (data[0] != 0xF5)
    {
      debug.error();
      Serial.println("Unexpected command code: " + String(data[0], HEX));
      return;
    }

    // Extract status and CRC
    uint8_t status = data[1];
    uint8_t crc = data[2];

    String statusMessage;
    switch (status)
    {
    case 0:
      F5Status = "Run failed";
      break;
    case 1:
      F5Status = "Run starting";
      break;
    case 2:
      F5Status = "Run complete";
      break;
    case 3:
      F5Status = "End limit stopped";
      break;
    default:
      F5Status = "Unknown status";
      break;
    }

    debug.info();
    Serial.print(F("Motor response to setTargetPosition: "));
    Serial.println(F5Status);
  }

  void decodeMessage(const __u8 *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    debug.info();
    Serial.print("Received message ");
    Serial.print("with code: ");
    Serial.println(data[0], HEX);

    char msgString[128]; // Array to store serial string

    if (data[0] == 0x30)
    {
      decodeEncoderValue(data, length);
    }
    else if (data[0] == 0xF5)
    {
      handleSetPositionResponse(data, length);
    }
    else
    {
      debug.error();
      Serial.print("unimplemented code: ");
      Serial.println(data[0], HEX);

      debug.info();
      Serial.print("Raw code byte: ");
      Serial.println(data[0]);

      for (byte i = 0; i < length; i++)
      {
        sprintf_P(msgString, PSTR(" 0x%.2X"), data[i]);
        debug.info();
        Serial.println(msgString);
      }
    }
  }

  /*

  int queryMotorPosition()
  {
    uint8_t data[2] = {0x30, 0x00}; // Command code for querying position (0x30)
    sendCANMessage(data, 2);
    delay(10);
    uint8_t recvData[8];
    uint8_t recvLen;
    unsigned long recvId;

    if (readCANMessage(&recvId, &recvLen, recvData))
    {
      uint32_t position = (recvData[3] << 16) | (recvData[4] << 8) | recvData[5];
      Serial.print("Motor position: ");
      Serial.println(position);
      return position;
    }
    Serial.println("Error reading position");
    return -1; // Error reading message
  }

  void setTargetPosition(uint32_t position, uint8_t speed = 100, uint8_t acceleration = 5, bool absolute = true)
  {
    uint8_t data[8];
    data[0] = absolute ? 0xF5 : 0xF4;  // Befehlscode für Position mode4: absolute motion by axis
    data[1] = (speed >> 8) & 0x7F;     // Combine direction bit with the upper 7 bits of speed
    data[2] = speed & 0xFF;            // Lower 8 bits of speed
    data[3] = acceleration;            // Beschleunigung
    data[4] = (position >> 16) & 0xFF; // Obere 8 Bits der Position
    data[5] = (position >> 8) & 0xFF;  // Mittlere 8 Bits der Position
    data[6] = position & 0xFF;         // Untere 8 Bits der Position
    // data[7] = calculateCRC(data, 8);   // Checksumme oder reserved (kann auf 0 gesetzt werden)

    if (sendCANMessage(data, 8))
    {
      Serial.print("Zielposition gesetzt: ");
      Serial.print(position);
      Serial.print(", Geschwindigkeit: ");
      Serial.print(speed);
      Serial.print(", Beschleunigung: ");
      Serial.print(acceleration);
      Serial.print(", Modus: ");
      Serial.println(absolute ? "Absolut" : "Relativ");
            delay(10);

            if (sendCANMessage(data, 5))
            {

              //      readCanMessages();

              uint8_t recvData[6];
              uint8_t recvLen;
              unsigned long recvId;

              if (readCANMessage(&recvId, &recvLen, recvData))
              {
                // Empfange Daten verarbeiten
                uint8_t status = recvData[1];
                if (status == 1)
                {
                  Serial.println("setTargetPosition Command: Sent Successfully sent and processed!");
                }
                else
                {
                  Serial.println("setTargetPosition Command: Sent Successfully but FAILED to be processed!");
                }
              }
              else
              {
                Serial.println("Could not read response for setTargetPosition!");
              }
    }

    }
    else
    {
      Serial.println("setTargetPosition Command: could not be sent!");
    }
  }

  void
  readCanMessages()
  {
    if (!digitalRead(2)) // If CAN0_INT pin is low, read receive buffer
    {
      uint32_t long rxId;
      byte len;
      byte rxBuf[8];

      if (CAN.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK) // Read data: len = data length, buf = data byte(s)
      {
        char msgString[128];        // Array to store serial string
        if (rxId & CAN_IS_EXTENDED) // Determine if ID is standard (11 bits) or extended (29 bits)
          sprintf_P(msgString, PSTR("Extended ID: 0x%.8lX  DLC: %1d  Data:"), (rxId & CAN_EXTENDED_ID), len);
        else
          sprintf_P(msgString, PSTR("Standard ID: 0x%.3lX       DLC: %1d  Data:"), rxId, len);

        Serial.print(msgString);

        for (byte i = 0; i < len; i++)
        {
          sprintf_P(msgString, PSTR(" 0x%.2X"), rxBuf[i]);
          Serial.print(msgString);
        }

        Serial.println();
      }
      else
      {
        Serial.println(F("Interrupt is asserted, but there were no messages to process..."));
      }
    }
  }
  */
};
#endif