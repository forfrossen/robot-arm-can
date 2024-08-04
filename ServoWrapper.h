#ifndef SERVO42D_CAN_H
#define SERVO42D_CAN_H

#include <Arduino.h>
#include <SPI.h>
#include <Arduino_CAN.h>
#include <map>
#include <functional>
#include "Debug.h"
#include "CAN.h"
#include "utils/CommandMapper.h"
#include "ResponseHandlerRegistry.h"

#define MAX_PROCESSED_MESSAGES 10

class Servo42D_CAN
{
private:
  CANBus *canBus;
  CommandMapper *commandMapper;
  int32_t CarryValue = 0;
  uint16_t EncoderValue = 0;
  String F5Status = "";
  ResponseHandlerRegistry responseHandlerRegistry;

public:
  uint32_t canId;

  Servo42D_CAN(uint32_t id, CANBus *bus, CommandMapper *commandMapper) : canId(id), canBus(bus), commandMapper(commandMapper)
  {
    Debug debug("Servo42D_CAN", __func__);
    debug.info();
    debug.add(F("New Servo42D_CAN object created with CAN ID: "));
    debug.print(canId, HEX);

    // registerResponseHandler(0x01, myResponseHandler); // Register handler for command code 0x01

    // Initialize the response handlers
  }

  static void initializeResponseHandlers();

  void registerResponseHandler(uint8_t commandCode, std::function<void(uint8_t *, uint8_t)> handler)
  {
    responseHandlerRegistry.registerHandler(commandCode, handler);
  }

  void handleResponse(uint8_t *data, uint8_t length)
  {
    responseHandlerRegistry.handleResponse(data, length);
  }

  // void handleReceivedMessage(uint32_t id, uint8_t length, const uint8_t *data)
  void handleReceivedMessage(CanMsg frame)
  {
    Debug debug("Servo42D_CAN", __func__);
    uint8_t code = frame.data[0];

    if (!code)
    {
      debug.error();
      debug.print(F("Error: code is empty!"));
      return;
    }

    char commandName[50];
    commandMapper->getCommandNameFromCode(code, commandName);

    debug.info();
    debug.add(F("ID: "));
    debug.add(canId);
    debug.add(F("\t length: "));
    debug.add(frame.data_length, HEX);
    debug.add(F("\tcode: "));
    debug.add(frame.data[0], HEX);
    debug.add(F("\tcommandName: "));
    debug.print(commandName);

    if (frame.getStandardId() == canId)
    {
      decodeMessage(frame.data, frame.data_length);
    }
  }

  // Method to send CAN message
  void sendCommand(uint8_t *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    uint8_t code = data[0];

    if (!code)
    {
      debug.error();
      debug.print(F("Error: code is empty!"));
      return;
    }

    char commandName[50];
    commandMapper->getCommandNameFromCode(code, commandName);

    debug.info();
    debug.add(F("ID: "));
    debug.add(canId, HEX);
    debug.add(F("\t length: "));
    debug.add(length);
    debug.add(F("\t code: "));
    debug.add(data[0]);
    debug.add(F("\t commandName: "));
    debug.print(commandName);

    if (canBus->sendCANMessage(canId, length, data))
    {
      debug.info();
      debug.print(F("    ==> Message sent successfully!"));
    }
    else
    {
      debug.error();
      debug.print(F("    ==> Error sending message!"));
    }
  }

  void handleQueryStatusResponse(const uint8_t *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    debug.info();
    // Empfange Daten verarbeiten
    uint8_t status = data[1];

    switch (status)
    {
    case 0:
      debug.print(F("Abfrage fehlgeschlagen"));
      break;
    case 1:
      debug.print(F("Motor gestoppt"));
      break;
    case 2:
      debug.print(F("Motor beschleunigt"));
      break;
    case 3:
      debug.print(F("Motor verlangsamt"));
      break;
    case 4:
      debug.print(F("Motor volle Geschwindigkeit"));
      break;
    case 5:
      debug.print(F("Motor wird geparkt"));
      break;
    case 6:
      debug.print(F("Motor wird kalibriert"));
      break;
    default:
      debug.print(F("Unbekannter Status"));
      break;
    }
  }

  void handleQueryMotorPositionResponse(const uint8_t *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    if (length != 8 || data[0] != 0x30)
    {
      debug.error();
      debug.print(F("Invalid response length or code."));
      return;
    }

    CarryValue = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
    EncoderValue = (data[5] << 8) | data[6];
    uint8_t crc = data[7];

    debug.info();
    debug.add(F("Carry value: "));
    debug.print(CarryValue);

    debug.info();
    debug.add(F("Encoder value: "));
    debug.print(EncoderValue);
  }

  void handleSetPositionResponse(const uint8_t *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    if (length != 3)
    {

      debug.error();
      debug.print(F("Invalid response length."));
      return;
    }
    if (data[0] != 0xF5)
    {
      debug.error();
      debug.add(F("Unexpected command code: "));
      debug.print(data[0], HEX);
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
    debug.add(F("Motor response to setTargetPosition: "));
    debug.print(F5Status);
  }

  void handeSetHomeResponse(const uint8_t *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);

    if (length != 3)
    {
      debug.error();
      debug.print(F("Invalid response length."));
      return;
    }

    // Überprüfen, ob das erste Byte (Befehlscode) 0x90 ist
    if (data[0] != 0x90)
    {
      debug.error();
      debug.add(F("Unexpected command code: "));
      debug.print(data[0], HEX);
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
    debug.add(F("Set Home Response: "));
    debug.add(F("Status: "));
    debug.add(statusMessage);
    debug.add(F("CRC: "));
    debug.print(crc, HEX);

    // CRC überprüfen, falls notwendig
    // uint8_t expected_crc = calculateChecksum(can_id, data, length - 1);
    // if (crc == expected_crc) {
    //     debug.print(F("CRC check passed."));
    // } else {
    //     debug.print(F("CRC check failed."));
    // }
  }

  void handleSetWorkModeResponse(uint8_t *data, uint8_t length)
  {
    Debug debug("ResponseHandlers", __func__);
    if (data[1] == 1)
    {
      debug.print(F("Set Work Mode: Success"));
    }
    else
    {
      debug.print(F("Set Work Mode: Failed"));
    }
  }

  void handleSetCurrentResponse(uint8_t *data, uint8_t length)
  {
    Debug debug("ResponseHandlers", __func__);
    if (data[1] == 1)
    {
      debug.print(F("Set Current: Success"));
    }
    else
    {
      debug.print(F("Set Current: Failed"));
    }
  }

  void decodeMessage(const uint8_t *data, uint8_t length)
  {
    Debug debug("Servo42D_CAN", __func__);
    debug.info();
    debug.add(F("Received message with code: "));
    debug.print(data[0], HEX);

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
      debug.add(F("unimplemented code: "));
      debug.print(data[0], HEX);

      debug.info();
      debug.add(F("Raw code byte: "));
      debug.print(data[0]);

      for (byte i = 0; i < length; i++)
      {
        sprintf_P(msgString, PSTR(" 0x%.2X"), data[i]);
        debug.info();
        debug.print(msgString);
      }
    }
  }
};
#endif