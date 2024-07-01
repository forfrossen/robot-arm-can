#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <mcp_can.h>
#include <SPI.h>

class ServoController {
public:
    ServoController(int csPin, int intPin, long canSpeed = CAN_500KBPS, int clockSpeed = MCP_8MHZ);
    bool begin();
    bool setSpeed(int16_t speed, byte direction, byte acceleration);
    bool setSteps(uint32_t steps, byte direction);
    int32_t queryPosition();
    bool queryStatus(byte &status);
    bool setZeroPosition();
    void checkIncomingMessages();

private:
    MCP_CAN CAN;
    int csPin;
    int intPin;
    long canSpeed;
    int clockSpeed;

    byte calculateCRC(byte canId, const byte* data, int length);
};
#endif // SERVO_CONTROLLER_H
