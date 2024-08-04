
/*
#define configSUPPORT_DYNAMIC_ALLOCATION (1)
#define configUSE_MUTEXES (1)
*/

#include <map>
#include <Arduino_CAN.h>

#include "LogQueue.h"
#include "Debug.h"
#include "CAN.h"
#include "ServoWrapper.h"

#define DONT_USE_WIFI
#ifdef USE_WIFI
#include "utils/wifi.h"
#include "utils/webserver.h"
#endif

#include "utils/commandMapper.h"

#include "Commands/Command.h"
#include "Commands/SetParameters/SetHomeCommand.h"
#include "Commands/RunMotorInSpeedModeCommand.h"
#include "Commands/StopMotorCommand.h"
#include "Commands/Query/QueryMotorStatusCommand.h"
#include "Commands/Query/QueryMotorPositionCommand.h"
#include "Commands/SetTargetPositionCommand.h"

const char compile_date[] = __DATE__ " " __TIME__;

#ifdef USE_WIFI
WiFiHelper wifiHelper;
WiFiServer server(80);
#endif
LogQueue logQueue;
CANBus *canBus;
CommandMapper *commandMapper;
Servo42D_CAN *servo1;
Servo42D_CAN *servo2;
Servo42D_CAN *servo3;

std::map<uint8_t, Servo42D_CAN *> Servos; // Replace LittleVector with std::map

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
SemaphoreHandle_t xMutex;

void setup()
{
  Serial.begin(115200);

  while (!Serial)
    ;


  Debug debug("MAIN", __func__);
  Debug::setLogQueue(&logQueue);
  xTaskCreate(processLogQueue, "LogProcessor", 256, NULL, 1, NULL);

  SPI.begin();
  delay(100);

  debug.info();
  debug.print(F("Hallo, Test from Arm !"));

  debug.info();
  debug.add(F("Build date: "));
  debug.print(compile_date);

#ifdef USE_WIFI
  wifiHelper.connectWifi();
  server.begin();
  delay(1000);
#endif

  canBus = new CANBus();
  delay(1000);
  canBus->begin();
  commandMapper = new CommandMapper();

  // Initialisierung der Servo42D_CAN Instanzen
  servo1 = new Servo42D_CAN(0x01, canBus, commandMapper);
  servo2 = new Servo42D_CAN(0x02, canBus, commandMapper);
  servo3 = new Servo42D_CAN(0x03, canBus, commandMapper);

  Servos[0x01] = servo1;
  Servos[0x02] = servo2;
  Servos[0x03] = servo3;

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
#ifdef USE_WIFI
  WiFiClient client = server.available();
  if (client)
  {
    webserver(client);
  }
#endif

  checkForMessages();

  // debug.print("================================");
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
    for (auto &pair : Servos)
    {
      Command *setTargetPositionCommand = new SetTargetPositionCommand(pair.second, randomValue, randomSpeed, randomAccel, true);
      setTargetPositionCommand->execute();
      delay(50);
      checkForMessages();
      delay(100);

      Command *queryMotorPosition = new QueryMotorPositionCommand(pair.second);
      queryMotorPosition->execute();
      delete queryMotorPosition;
      delay(50);
      checkForMessages();
    }
    /*

    servo1->queryMotorPosition();
    delay(50);
    checkForMessages();

    servo2->queryMotorPosition();
    delay(50);
    checkForMessages();

    servo3->queryMotorPosition();
    delay(50);
    checkForMessages();

    servo1->setTargetPosition(randomValue, randomSpeed, randomAccel, true);
    delay(50);
    checkForMessages();

    servo2->setTargetPosition(randomValue, randomSpeed, randomAccel, true);
    delay(50);
    checkForMessages();

    servo3->setTargetPosition(randomValue, randomSpeed, randomAccel, true);
    delay(50);
    checkForMessages();
*/
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
  delay(10);
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
  if (CAN.available())
  {
    CanMsg const msg = CAN.read();
    debug.info();
    debug.add(F("Message received with ID: "));
    debug.print(msg.getStandardId(), HEX);

    auto it = Servos.find(msg.getStandardId());
    if (it != Servos.end())
    {
      it->second->handleReceivedMessage(msg);
    }
  }
  // prevRx = millis();
}

void processLogQueue(void *pvParameters)
{
  while (true)
  {
    if (!logQueue.isEmpty())
    {
      String message = logQueue.dequeue();
      if (message != "")
      {
        Serial.println(message);
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}