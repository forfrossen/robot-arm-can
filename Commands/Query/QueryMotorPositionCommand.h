class QueryMotorPositionCommand : public Command
{
private:
  Servo42D_CAN *servo;

public:
  QueryMotorPositionCommand(Servo42D_CAN *servo) : servo(servo) {}

  void execute() override
  {
    Debug debug("QueryMotorPositionCommand", __func__);
    uint8_t data[1] = {0x30};

    debug.info();
    Serial.println(F("Querying motor position"));

    servo->sendCommand(data, 1);
  }
};
