class QueryMotorStatusCommand : public Command
{
private:
  Servo42D_CAN *servo;

public:
  QueryMotorStatusCommand(Servo42D_CAN *servo) : servo(servo) {}

  void execute() override
  {
    Debug debug("QueryMotorStatusCommand", __func__);
    uint8_t data[1] = {0xF1};

    debug.info();
    Serial.println(F("Querying motor status"));

    servo->sendCommand(data, 1);
  }
};
