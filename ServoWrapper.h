#ifndef SERVO42D_CAN_H
#define SERVO42D_CAN_H

#include "CAN.h" // Include CANBus.h with include guards
#include "utils/commandMapper.h"
#include "Debug.h"
#include <Arduino_CAN.h>

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
  uint32_t canId;

  Servo42D_CAN(uint32_t id, CANBus *bus, CommandMapper *commandMapper) : canId(id), canBus(bus), commandMapper(commandMapper)
  {
    Debug debug("Servo42D_CAN", __func__);
    debug.info();
    Serial.print(F("New Servo42D_CAN object created with CAN ID: "));
    Serial.println(canId, HEX);
  }

  // void handleReceivedMessage(uint32_t id, uint8_t length, const uint8_t *data)
  void handleReceivedMessage(CanMsg frame)
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
    Serial.print(F("ID: "));
    Serial.print(canId);
    Serial.print(F("\t length: "));
    Serial.print(frame.data_length, HEX);
    Serial.print(F("\tcode: "));
    Serial.print(frame.data[0], HEX);
    Serial.print(F("\tcommandName: "));
    Serial.println(commandName);

    if (frame.getStandardId() == canId)
    {
      decodeMessage(frame.data, frame.data_length);
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
    Serial.print(F("ID: "));
    Serial.print(canId, HEX);
    Serial.print(F("\t length: "));
    Serial.print(length);
    Serial.print(F("\t code: "));
    Serial.print(data[0]);
    Serial.print(F("\t commandName: "));
    Serial.println(commandName);

    if (canBus->sendCANMessage(canId, length, data))
    {
      debug.info();
      Serial.println(F("    ==> Message sent successfully!"));
    }
    else
    {
      debug.error();
      Serial.println(F("    ==> Error sending message!"));
    }
  }

  void handleQueryStatusResponse(const __u8 *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    debug.info();
    // Empfange Daten verarbeiten
    uint8_t status = data[1];

    switch (status)
    {
    case 0:
      Serial.println(F("Abfrage fehlgeschlagen"));
      break;
    case 1:
      Serial.println(F("Motor gestoppt"));
      break;
    case 2:
      Serial.println(F("Motor beschleunigt"));
      break;
    case 3:
      Serial.println(F("Motor verlangsamt"));
      break;
    case 4:
      Serial.println(F("Motor volle Geschwindigkeit"));
      break;
    case 5:
      Serial.println(F("Motor wird geparkt"));
      break;
    case 6:
      Serial.println(F("Motor wird kalibriert"));
      break;
    default:
      Serial.println(F("Unbekannter Status"));
      break;
    }
  }

  void handleQueryMotorPositionResponse(const __u8 *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    if (length != 8 || data[0] != 0x30)
    {
      debug.error();
      Serial.println(F("Invalid response length or code."));
      return;
    }

    CarryValue = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
    EncoderValue = (data[5] << 8) | data[6];
    uint8_t crc = data[7];

    debug.info();
    Serial.print(F("Carry value: "));
    Serial.println(CarryValue);

    debug.info();
    Serial.print(F("Encoder value: "));
    Serial.println(EncoderValue);
  }

  void handleSetPositionResponse(const __u8 *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    if (length != 3)
    {

      debug.error();
      Serial.println(F("Invalid response length."));
      return;
    }
    if (data[0] != 0xF5)
    {
      debug.error();
      Serial.print(F("Unexpected command code: "));
      Serial.println(data[0], HEX);
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

  void handeSetHomeResponse(const __u8 *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);

    if (length != 3)
    {
      debug.error();
      Serial.println(F("Invalid response length."));
      return;
    }

    // Überprüfen, ob das erste Byte (Befehlscode) 0x90 ist
    if (data[0] != 0x90)
    {
      debug.error();
      Serial.print(F("Unexpected command code: "));
      Serial.println(data[0], HEX);
      return;
    }

    // Status und CRC extrahieren
    uint8_t status = data[1];
    uint8_t crc = data[2];

    String statusMessage;
    switch (status)
    {
    case 0:
      statusMessage = "Set home failed.";
      break;
    case 1:
      statusMessage = "Set home in progress...";
      break;
    case 2:
      statusMessage = "Set home completed.";
      break;
    default:
      statusMessage = "Unknown status.";
      break;
    }

    debug.info();
    Serial.print(F("Set Home Response: "));
    Serial.print(F("Status: "));
    Serial.print(statusMessage);
    Serial.print(F("CRC: "));
    Serial.println(crc, HEX);

    // CRC überprüfen, falls notwendig
    // uint8_t expected_crc = calculateChecksum(can_id, data, length - 1);
    // if (crc == expected_crc) {
    //     Serial.println(F("CRC check passed."));
    // } else {
    //     Serial.println(F("CRC check failed."));
    // }
  }

  void decodeMessage(const __u8 *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    debug.info();
    Serial.print(F("Received message with code: "));
    Serial.println(data[0], HEX);

    char msgString[128]; // Array to store serial string

    if (data[0] == 0x30)
    {
      handleQueryMotorPositionResponse(data, length);
    }
    else if (data[0] == 0xF5)
    {
      handleSetPositionResponse(data, length);
    }
    else if (data[0] == 0x31)
    {
      handleQueryStatusResponse(data, length);
    }
    else if (data[0] == 0x90)
    {
      handeSetHomeResponse(data, length);
    }
    else
    {
      debug.error();
      Serial.print(F("unimplemented code: "));
      Serial.println(data[0], HEX);

      debug.info();
      Serial.print(F("Raw code byte: "));
      Serial.println(data[0]);

      for (byte i = 0; i < length; i++)
      {
        sprintf_P(msgString, PSTR(" 0x%.2X"), data[i]);
        debug.info();
        Serial.println(msgString);
      }
    }
  }
};
#endif