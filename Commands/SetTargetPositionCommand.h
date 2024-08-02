class SetTargetPositionCommand : public Command
{
private:
  Servo42D_CAN *servo;
  uint32_t position;
  uint8_t speed;
  uint8_t acceleration;
  bool absolute;

public:
  SetTargetPositionCommand(Servo42D_CAN *servo, uint32_t position, uint8_t speed, uint8_t acceleration, bool absolute)
      : servo(servo), position(position), speed(speed), acceleration(acceleration), absolute(absolute) {}

  void execute() override
  {
    Debug debug("SetTargetPositionCommand", __func__);
    uint8_t data[6];
    data[0] = absolute ? 0xF5 : 0xF4;
    data[1] = speed;
    data[2] = acceleration;
    data[3] = (position >> 16) & 0xFF;
    data[4] = (position >> 8) & 0xFF;
    data[5] = position & 0xFF;

    debug.info();
    Serial.print(F("Setting target position: "));
    Serial.print(position);
    Serial.print(F(", Speed: "));
    Serial.print(speed);
    Serial.print(F(", Acceleration: "));
    Serial.print(acceleration);
    Serial.print(F(", Mode: "));
    Serial.println(absolute ? F("Absolute") : F("Relative"));

    servo->sendCommand(data, 6);
  }
};
