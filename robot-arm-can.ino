#include "CANBus.h"
#include "ServoWrapper.h"
#include "SimpleVector.h"

const int CAN_CS_PIN = 10; // Adjust pin number as necessary

CANBus canBus(CAN_CS_PIN);

Servo42D_CAN servo1(0x01, canBus); // Initialize another servo with CAN ID 0x001
Servo42D_CAN servo2(0x02, canBus); // Initialize servo with CAN ID 0x002
Servo42D_CAN servo3(0x03, canBus); // Initialize servo with CAN ID 0x003

bool doSpeedTest = false;
unsigned long prevTx = 0;
unsigned long prevRnd = 0;
unsigned int invlTx = 10000;
unsigned int invlRnd = 10000;
unsigned int randomValue = 0;
unsigned int randomDirection = 0;
unsigned int randomSpeed = 0;
unsigned int randomAccel = 0;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  SPI.begin();
  Serial.println("Hallo, Test from Arm !");
  canBus.begin();
  delay(1000);

  /*
  servo3.enableMotor();
  delay(100);
  canBus.listenForMessages();

  servo1.enableMotor();
  servo2.enableMotor();
  */

  // For random number generation
  randomSeed(analogRead(0));
  randomValue = random(-40000, +40000);
  randomSpeed = random(0, 2000);
  randomDirection = random(0, 1);
  randomAccel = random(0, 255);
  delay(100);
}

void loop()
{

  canBus.listenForMessages();
  // Serial.println("================================");
  // servo1.stopMotor();
  // servo2.stopMotor();

  // int status = servo1.queryMotorStatus();

  // int randomDirection = random(0, 1);
  /*
   */

  if ((millis() - prevRnd) >= invlTx)
  {
    randomValue = random(-40000, +40000);
    prevRnd = millis();
  }

  if ((millis() - prevTx) >= invlRnd)
  {
    servo1.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
    servo2.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
    servo3.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
    /*
    servo1.setTargetPosition(randomValue, randomSpeed, randomAccel, true);
    servo2.setTargetPosition(randomValue, randomSpeed, randomAccel, true);
    servo3.setTargetPosition(randomValue, randomSpeed, randomAccel, true);
    servo3.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
    servo3.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
    servo3.setTargetPosition(randomValue, 100, 10, true);
    servo3.setSpeedAndAcceleration(1000, true, 10);
    servo1.setSpeedAndAcceleration(1000, true, 10);
    servo2.setSpeedAndAcceleration(1000, true, 10);
    servo2.setTargetPosition(randomValue * 10, 100, 10, true);

    servo1.setSpeedAndAcceleration(1000, 1, true);
    servo2.setSpeedAndAcceleration(1000, 1, true);
    */

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
  delay(10);
  */
}