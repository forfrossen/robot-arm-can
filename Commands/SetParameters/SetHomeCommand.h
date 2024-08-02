class SetHomeCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint8_t homeTrig;
  uint8_t homeDir;
  uint16_t homeSpeed;
  uint8_t endLimit;

public:
  SetHomeCommand(Servo42D_CAN *servo, uint8_t homeTrig, uint8_t homeDir, uint16_t homeSpeed, uint8_t endLimit)
      : servo(servo), homeTrig(homeTrig), homeDir(homeDir), homeSpeed(homeSpeed), endLimit(endLimit) {}

  void execute() override
  {
    Debug debug("SetHomeCommand", __func__);

    uint8_t data[6];                   // Command code + parameters + CRC
    data[0] = 0x90;                    // Set Home command code
    data[1] = homeTrig;                // Home trigger level: 0 = Low, 1 = High
    data[2] = homeDir;                 // Home direction: 0 = CW, 1 = CCW
    data[3] = (homeSpeed >> 8) & 0xFF; // High byte of home speed
    data[4] = homeSpeed & 0xFF;        // Low byte of home speed
    data[5] = endLimit;                // End limit: 0 = disable, 1 = enable

    debug.info();
    Serial.print(F("ID: "));
    Serial.print(servo->canId, HEX);
    Serial.print(F(", Home Trigger: "));
    Serial.print(homeTrig ? F("High") : F("Low"));
    Serial.print(F(", Home Direction: "));
    Serial.print(homeDir ? F("CCW") : F("CW"));
    Serial.print(F(", Home Speed: "));
    Serial.print(homeSpeed);
    Serial.print(F(", End Limit: "));
    Serial.println(endLimit ? F("Enabled") : F("Disabled"));

    servo->sendCommand(data, 6);
  }
};