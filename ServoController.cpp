#include "ServoController.h"

ServoController::ServoController(int csPin, int intPin, long canSpeed, int clockSpeed)
    : CAN(csPin), csPin(csPin), intPin(intPin), canSpeed(canSpeed), clockSpeed(clockSpeed) {}

bool ServoController::begin() {
    if (CAN.begin(MCP_ANY, canSpeed, clockSpeed) == CAN_OK) {
        CAN.setMode(MCP_NORMAL);
        pinMode(intPin, INPUT);
        return true;
    }
    return false;
}

bool ServoController::setSpeed(int16_t speed, byte direction, byte acceleration) {
    byte data[6];
    data[0] = 0x20; // Command code for speed mode
    data[1] = 0x00; // Reserved
    data[2] = (byte)speed; // Speed value
    data[3] = direction;
    data[4] = acceleration;
    data[5] = calculateCRC(0x01, data, 5); // CRC including CAN ID (0x01)

    return CAN.sendMsgBuf(0x01, 0, 6, data) == CAN_OK;
}

bool ServoController::setSteps(uint32_t steps, byte direction) {
    byte data[8];
    data[0] = 0x40; // Command code for step mode
    data[1] = (steps >> 24) & 0xFF;
    data[2] = (steps >> 16) & 0xFF;
    data[3] = (steps >> 8) & 0xFF;
    data[4] = steps & 0xFF;
    data[5] = direction;
    data[6] = 0x00; // Reserved
    data[7] = calculateCRC(0x01, data, 7); // CRC including CAN ID (0x01)

    return CAN.sendMsgBuf(0x01, 0, 8, data) == CAN_OK;
}

bool ServoController::queryPosition() {
    byte data[3] = {0x30, 0x00, 0x00}; // Command code for querying position
    data[1] = 0x00; // Reserved
    data[2] = calculateCRC(0x01, data, 2); // CRC including CAN ID (0x01)

    return CAN.sendMsgBuf(0x01, 0, 3, data) == CAN_OK;
}

bool ServoController::queryStatus(byte &status) {
    byte data[3] = {0xF1, 0x00, 0x00}; // Command code for querying status (corrected to 0xF1)
    data[1] = 0x00; // Reserved
    data[2] = calculateCRC(0x01, data, 2); // CRC including CAN ID (0x01)

    if (CAN.sendMsgBuf(0x01, 0, 3, data) != CAN_OK) {
        return false;
    }

    delay(100);

    if (digitalRead(intPin) == LOW) {
        long unsigned int rxId;
        unsigned char len = 0;
        unsigned char rxBuf[8];

        if (CAN.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK && len >= 1) {
            status = rxBuf[0];
            return true;
        }
    }
    return false;
}

bool ServoController::setZeroPosition() {
    byte data[3] = {0x92, 0x00, 0x00}; // Command code for setting position to zero
    data[1] = 0x00; // Reserved
    data[2] = calculateCRC(0x01, data, 2); // CRC including CAN ID (0x01)

    return CAN.sendMsgBuf(0x01, 0, 3, data) == CAN_OK;
}

byte ServoController::calculateCRC(byte canId, const byte* data, int length) {
    byte crc = canId; // Start with the CAN ID
    for (int i = 0; i < length; ++i) {
        crc += data[i];
    }
    return crc & 0xFF;
}