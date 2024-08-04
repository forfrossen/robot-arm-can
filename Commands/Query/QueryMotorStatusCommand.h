class QueryMotorStatusCommand : public Command
{
private:
  Servo *servo;

public:
  QueryMotorStatusCommand(Servo *servo) : servo(servo) {}

  void execute() override
  {
    Debug debug("QueryMotorStatusCommand", __func__);
    uint8_t data[1] = {0xF1};

    debug.info();
    debug.print(F("Querying motor status"));

    servo->sendCommand(data, 1);
  }
};
