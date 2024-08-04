class QueryMotorPositionCommand : public Command
{
private:
  Servo *servo;

public:
  QueryMotorPositionCommand(Servo *servo) : servo(servo) {}

  void execute() override
  {
    Debug debug("QueryMotorPositionCommand", __func__);
    uint8_t data[1] = {0x30};

    debug.info();
    debug.print(F("Querying motor position"));

    servo->sendCommand(data, 1);
  }
};
