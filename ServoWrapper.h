#ifndef SERVO42D_CAN_H
#define SERVO42D_CAN_H

#include "CANBus.h" // Include CANBus.h with include guards
#include "SimpleVector.h"
#include "commandMapper.h"

class Servo42D_CAN
{
private:
  CANBus &canBus;
  uint32_t canId;

  static SimpleVector<Servo42D_CAN *> instances;

  static void callback(unsigned long id, uint8_t length, uint8_t *data)
  {
    for (size_t i = 0; i < instances.getSize(); ++i)
    {
      Servo42D_CAN *instance = instances[i];
      if (instance->canId == id)
      {
        instance->handleReceivedMessage(id, length, data);
        break;
      }
    }
  }

public:
  Servo42D_CAN(uint32_t id, CANBus &bus) : canId(id), canBus(bus)
  {
    instances.push_back(this);
    canBus.registerCallback(callback);
  }

  void handleReceivedMessage(uint32_t id, uint8_t len, uint8_t *data)
  {
    Serial.println();
    Serial.println();
    Serial.println("************************************************");
    Serial.println();
    Serial.println("class [Servo42D_CAN] - fn: [handleReceivedMessage]");
    Serial.print("ID: ");
    Serial.println(canId, HEX);

    if (id == canId)
    {
      Serial.println("Received message: ");

      char msgString[128]; // Array to store serial string
      /*
      if (id & CAN_IS_EXTENDED) // Determine if ID is standard (11 bits) or extended (29 bits)
        sprintf_P(msgString, PSTR("Extended ID: 0x%.8lX  DLC: %1d  Data:"), (id & CAN_EXTENDED_ID), len);
      else
        sprintf_P(msgString, PSTR("Standard ID: 0x%.3lX       DLC: %1d  Data:"), id, len);

      */
      Serial.print(msgString);

      for (byte i = 0; i < len; i++)
      {
        sprintf_P(msgString, PSTR(" 0x%.2X"), data[i]);
        Serial.print(msgString);
      }
    }
    else
    {
      Serial.print("Received message from unknown ID: ");
      Serial.print(id);
    }
    Serial.println();
    Serial.println("************************************************");
    Serial.println();
    Serial.println();
  }

  // Method to send CAN message
  void sendCommand(uint8_t *data, uint8_t length)
  {
    Serial.println();
    Serial.println();
    Serial.println("************************************************");
    Serial.println();
    Serial.println("class [Servo42D_CAN] - fn: [sendCommand]");
    Serial.print("ID: ");
    Serial.print(canId);
    Serial.println();
    /*Serial.println("Sending command [");
    Serial.print(getCommandNameFromCode(data[0]));
    Serial.print("]");*/

    if (canBus.sendCANMessage(canId, length, data))
    {
      Serial.println("    ==> Message sent successfully!");
    }
    else
    {
      Serial.println("    ==> Error sending message!");
    }
    Serial.println();
    Serial.println("************************************************");
    Serial.println();
    Serial.println();
  }

  // Set Speed and Acceleration together
  void setSpeedAndAcceleration(uint16_t speed, uint8_t acceleration, bool direction = true)
  {
    uint8_t data[5];
    data[0] = 0xF6;                                              // Command code for setting speed and acceleration
    data[1] = (direction ? 0x80 : 0x00) | ((speed >> 8) & 0x0F); // Direction bit in the highest bit and upper 4 bits of speed
    data[2] = speed & 0xFF;                                      // Lower 8 bits of speed
    data[3] = acceleration;                                      // Acceleration
    // data[4] = calculateCRC(data, 5);   // CRC including CAN ID (0x01)
    Serial.println();
    Serial.println();
    Serial.println("************************************************");
    Serial.println("class [Servo42D_CAN] - fn: [setSpeedAndAcceleration]");
    Serial.print("ID: ");
    Serial.print(canId, HEX);
    Serial.print(", Speed: ");
    Serial.print(speed);
    Serial.print(", Acceleration: ");
    Serial.print(acceleration);
    Serial.print(", Direction: ");
    Serial.print(direction ? "CW" : "CCW");
    Serial.println();
    Serial.println("************************************************");
    Serial.println();
    Serial.println();
    sendCommand(data, 5);
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
    Serial.println();
    Serial.println();
    Serial.println("************************************************");
    Serial.println();
    Serial.println("class [Servo42D_CAN] - fn: [setTargetPosition]");
    Serial.print("ID: ");
    Serial.print(canId, HEX);
    Serial.print(", Position: ");
    Serial.print(position);
    Serial.print(", Geschwindigkeit: ");
    Serial.print(speed);
    Serial.print(", Beschleunigung: ");
    Serial.print(acceleration);
    Serial.print(", Modus: ");
    Serial.println(absolute ? "Absolut" : "Relativ");
    Serial.println();
    Serial.println("************************************************");
    Serial.println();
    Serial.println();
    sendCommand(data, 8);
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

// Define the static member
SimpleVector<Servo42D_CAN *> Servo42D_CAN::instances;

#endif