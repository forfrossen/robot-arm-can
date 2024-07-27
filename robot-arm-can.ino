#include <Arduino.h>
#include <SPI.h>
#include "commandMapper.h"
#include "CANBus.h"
#include "ServoWrapper.h"
#include <LittleVector.h>

CANBus canBus(CAN_CS_PIN);
Servo42D_CAN servo1(0x01, canBus);
Servo42D_CAN servo2(0x02, canBus);
Servo42D_CAN servo3(0x03, canBus);
LittleVector<Servo42D_CAN *> Servos;

struct can_frame frame;
bool doSpeedTest = false;
unsigned long prevTx = 0;
unsigned long prevRx = 0;
unsigned long prevRnd = 0;
uint16_t invlTx = 4000;
uint16_t invlRx = 50;
uint16_t invlRnd = 4000;
int16_t randomValue = 0;
uint8_t randomDirection = 0;
uint16_t randomSpeed = 0;
uint8_t randomAccel = 0;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  SPI.begin();
  Serial.println(F("Hallo, Test from Arm !"));
  canBus.begin();
  delay(1000);

  Servos.push_back(&servo1);
  Servos.push_back(&servo2);
  Servos.push_back(&servo3);

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

  checkForMessages();

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
    // servo1.queryMotorPosition();
    // servo2.setTargetPosition(randomValue, randomSpeed, randomAccel, true);
    servo2.queryMotorPosition();
    // servo3.queryMotorPosition();
    /*
    servo2.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
    servo1.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
    servo2.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
    servo3.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
    */

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
}

void checkForMessages()
{
  /*
  if ((millis() - prevRx) <= invlRx)
  {
    return;
  }
  */

  if (!canBus.listenForMessages(frame))
  {
    return;
  }

  for (int i = 0; i <= Servos.size(); i++)
  {
    if (Servos[i]->canId == frame.can_id)
    {
      Servos[i]->processCANMessage(frame);
    }
  }
  // prevRx = millis();
}