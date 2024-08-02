class StopMotorCommand : public Command
{
private:
  Servo42D_CAN *servo;

public:
  StopMotorCommand(Servo42D_CAN *servo) : servo(servo) {}

  void execute() override
  {
    Debug debug("StopMotorCommand", __func__);
    uint8_t data[4] = {0xF7};

    debug.info();
    Serial.println(F("Stopping motor"));

    servo->sendCommand(data, 1);
  }
};
