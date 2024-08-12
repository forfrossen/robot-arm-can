#ifndef SERVO42D_CAN_H
#define SERVO42D_CAN_H

#include <Arduino_FreeRTOS.h>
#include <Arduino_CAN.h>
#include <map>
#include <functional>
#include "Debug.hpp"
#include "CAN.hpp"
#include "utils\CommandMapper.hpp"
#include "ResponseHandlerRegistry.hpp"

#define MAX_PROCESSED_MESSAGES 10

class CANServo
{
public:
  CANServo(uint32_t id, CANBus *bus, CommandMapper *commandMapper);

  void registerResponseHandler(uint8_t commandCode, std::function<void(uint8_t *, uint8_t)> handler);
  void handleResponse(uint8_t *data, uint8_t length);
  void handleReceivedMessage(CanMsg frame);
  void sendCommand(uint8_t *data, uint8_t length);
  void handleQueryStatusResponse(const uint8_t *data, uint8_t length);
  void handleQueryMotorPositionResponse(const uint8_t *data, uint8_t length);
  void handleSetPositionResponse(const uint8_t *data, uint8_t length);
  void handeSetHomeResponse(const uint8_t *data, uint8_t length);
  void handleSetWorkModeResponse(uint8_t *data, uint8_t length);
  void handleSetCurrentResponse(uint8_t *data, uint8_t length);
  void decodeMessage(const uint8_t *data, uint8_t length);
  // static void taskFunctionWrapper(void *pvParameters);
  void taskCheckForMessages();

  uint32_t getCanId() const { return canId; }
  uint32_t getCarryValue() const { return CarryValue; }
  uint16_t getEncoderValue() const { return EncoderValue; }

private:
  uint32_t canId;
  CANBus *canBus;
  CommandMapper *commandMapper;
  ResponseHandlerRegistry responseHandlerRegistry;
  uint32_t CarryValue;
  uint16_t EncoderValue;
  String F5Status;
};

#endif // CANSERVO_H