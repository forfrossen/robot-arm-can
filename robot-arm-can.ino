#include "ServoWrapper.h"
#include "CANBusController.h"

CANBusController CAN;
Servo42D_CAN servo1(CAN, 0x01); // Initialize another servo with CAN ID 0x02
Servo42D_CAN servo2(CAN, 0x02); // Initialize servo with CAN ID 0x01

bool doSpeedTest = false;
unsigned long prevTx = 0;
unsigned long prevRnd = 0;
unsigned int invlTx = 3000;
unsigned int invlRnd = 3000;
unsigned int randomValue = 0;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  SPI.begin();

  CANBusController::begin();
  delay(1000);
  servo1.enableMotor();

  // For random number generation
  randomSeed(analogRead(0));
  randomValue = random(-40000, +40000);
}

void loop()
{
  // Serial.println("================================");
  // servo1.stopMotor();
  // servo2.stopMotor();

  // int status = servo1.queryMotorStatus();

  // int randomDirection = random(0, 1);

  if ((millis() - prevRnd) >= invlTx)
  {
    randomValue = random(-40000, +40000);
    prevRnd = millis();
  }

  if ((millis() - prevTx) >= invlRnd)
  {
    servo1.setTargetPosition(randomValue, 10, 1, true);
    prevTx = millis();
  }

  /*
      servo1.setTargetPosition(0, 10, 1, true);
      int position = servo1.queryMotorPosition();
      delay(3000);

      servo1.setTargetPosition(randomValue, 10, 1, false);
      position = servo1.queryMotorPosition();
      delay(3000);

      servo1.setTargetPosition(randomValue*-1, 10, 1, false);
      position = servo1.queryMotorPosition();
      delay(3000);
  */
  servo1.readCanMessages();
}