class QueryMotorPositionCommand : public Command
{
private:
  CANServo *servo;

public:
  QueryMotorPositionCommand(CANServo *servo) : servo(servo) {}

  void execute() override
  {
    Debug debug("QueryMotorPositionCommand", __func__);
    uint8_t data[1] = {0x30};

    debug.info();
    debug.print(F("Querying motor position"));

    servo->sendCommand(data, 1);
  }
};
