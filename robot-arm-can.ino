#include <Arduino.h>
#include <SPI.h>
#include "commandMapper.h"
#include "CANBus.h"
#include "ServoWrapper.h"
#include <LittleVector.h>

CANBus *canBus;
CommandMapper *commandMapper;
Servo42D_CAN *servo1;
Servo42D_CAN *servo2;
Servo42D_CAN *servo3;
LittleVector<Servo42D_CAN> Servos;
Debug *debug;

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
  Servos.reserve(3);
  // Serial.println("\n\n");
  Debug debug("MAIN", __func__);
  debug.info();
  Serial.println(F("Hallo, Test from Arm !"));
  delay(1000);
  canBus = new CANBus(CAN_CS_PIN);
  delay(1000);
  canBus->begin();
  commandMapper = new CommandMapper();

  // Initialisierung der Servo42D_CAN Instanzen
  servo1 = new Servo42D_CAN(0x01, canBus, commandMapper);
  servo2 = new Servo42D_CAN(0x02, canBus, commandMapper);
  servo3 = new Servo42D_CAN(0x03, canBus, commandMapper);

  Servos.push_back(*servo1);
  Servos.push_back(*servo2);
  Servos.push_back(*servo3);

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
    // servo3.queryMotorPosition();
    servo2->queryMotorPosition();
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
  delay(50);
}

void checkForMessages()
{
  Debug debug("MAIN", __func__);
  /*
  if ((millis() - prevRx) <= invlRx)
  {
    return;
  }
  */
  can_frame frame;
  if (!canBus->listenForMessages(&frame))
  {
    return;
  }
  debug.info();
  Serial.print(F("Message received with ID: "));
  Serial.println(frame.can_id, HEX);
  /*
    for (LittleVector<Servo42D_CAN>::iterator itr = Servos.begin(); itr != Servos.end();)
    {
      if ((*itr).canId == frame.can_id)
      {
        Serial.print(F("Processing message for servo with ID: "));
        Serial.println((*itr).canId, HEX);
        //(*itr).handleReceivedMessage(&frame);
      }
      else
        ++itr;
    }
  */
  for (auto servo : Servos)
  {
    if (servo.canId == frame.can_id)
    {
      // servo.handleReceivedMessage(frame);
      servo.greet(frame);
    }
  }
  // prevRx = millis();
}