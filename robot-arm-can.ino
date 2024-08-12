
/*
#define configSUPPORT_DYNAMIC_ALLOCATION (1)
#define configUSE_MUTEXES (1)
*/

#include <map>
#include <Arduino_CAN.h>
#include <SPI.h>

#include <Arduino_FreeRTOS.h>

#include "LogQueue.hpp"
#include "Debug.hpp"
#include "CAN.hpp"
#include "CANServo.hpp"

#define DONT_USE_WIFI
#ifdef USE_WIFI
#include "utils/wifi.hpp"
#include "utils/webserver.hpp"
#endif

#include "utils/commandMapper.hpp"

#include "Commands/Command.hpp"
#include "Commands/SetParameters/SetHomeCommand.hpp"
#include "Commands/RunMotorInSpeedModeCommand.hpp"
#include "Commands/StopMotorCommand.hpp"
#include "Commands/Query/QueryMotorStatusCommand.hpp"
#include "Commands/Query/QueryMotorPositionCommand.hpp"
#include "Commands/SetTargetPositionCommand.hpp"

const char compile_date[] = __DATE__ " " __TIME__;

#ifdef USE_WIFI
WiFiHelper wifiHelper;
WiFiServer server(80);
#endif
LogQueue logQueue;
CANBus *canBus;
CommandMapper *commandMapper;

std::map<uint8_t, CANServo *> Servos; // Replace LittleVector with std::map

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

  Debug debug("MAIN", __func__);

  Debug::setLogQueue(&logQueue);
  xTaskCreate(task_logProcessor, "LogProcessor", 128, NULL, 4, NULL);

  debug.info();
  debug.print(F("Hallo, Test from Arm !!!"));

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
  Servos[0x01] = new CANServo(0x01, canBus, commandMapper);
  Servos[0x02] = new CANServo(0x02, canBus, commandMapper);
  Servos[0x03] = new CANServo(0x03, canBus, commandMapper);

  /*
  servo3.enableMotor();
  delay(100);
  canBus.listenForMessages();

  servo1.enableMotor();
  servo2.enableMotor();
  */

  // For random number generation
  // randomSeed(analogRead(0));
  // randomValue = random(-40000, +40000);
  // randomSpeed = random(0, 2000);
  // randomDirection = random(0, 1);
  // randomAccel = random(0, 255);
  delay(100);

  xTaskCreate(
      task_checkMessages,
      "checkMessages",
      1000,
      NULL,
      1,
      NULL);

  xTaskCreate(
      task_sendPositon,
      "sendPositons",
      1000,
      NULL,
      1,
      NULL);

  xTaskCreate(
      task_QueryPosition,
      "queryPosition",
      256,
      NULL,
      1,
      NULL);

  // xTaskCreate(MyIdleTask, "IdleTask", 100, NULL, 0, NULL);

  vTaskStartScheduler();
  for (;;)
    ;
}

void loop() {}
// void loop()
// {
// #ifdef USE_WIFI
//   WiFiClient client = server.available();
//   if (client)
//   {
//     webserver(client);
//   }
// #endif

//   // debug.print("================================");
//   // servo1.stopMotor();
//   // servo2.stopMotor();

//   // int status = servo1.queryMotorStatus();

//   // int randomDirection = random(0, 1);
//   /*
//    */

//   if ((millis() - prevRnd) >= invlTx)
//   {
//     randomValue = random(-40000, +40000);
//     prevRnd = millis();
//   }

//   if ((millis() - prevTx) >= invlRnd)
//   {
//     for (auto &pair : Servos)
//     {
//       Command *setTargetPositionCommand = new SetTargetPositionCommand(pair.second, randomValue, randomSpeed, randomAccel, true);
//       setTargetPositionCommand->execute();
//       delay(50);

//       Command *queryMotorPosition = new QueryMotorPositionCommand(pair.second);
//       queryMotorPosition->execute();
//       delete queryMotorPosition;
//       delay(50);
//     }
//     /*

//     servo1->queryMotorPosition();
//     delay(50);
//     checkForMessages();

//     servo2->queryMotorPosition();
//     delay(50);
//     checkForMessages();

//     servo3->queryMotorPosition();
//     delay(50);
//     checkForMessages();

//     servo1->setTargetPosition(randomValue, randomSpeed, randomAccel, true);
//     delay(50);
//     checkForMessages();

//     servo2->setTargetPosition(randomValue, randomSpeed, randomAccel, true);
//     delay(50);
//     checkForMessages();

//     servo3->setTargetPosition(randomValue, randomSpeed, randomAccel, true);
//     delay(50);
//     checkForMessages();
// */
//     /*
//     servo2.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
//     servo1.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
//     servo2.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
//     servo3.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
//     */

//     /*
//     servo1.setTargetPosition(randomValue, randomSpeed, randomAccel, true);
//     servo2.setTargetPosition(randomValue, randomSpeed, randomAccel, true);
//     servo3.setTargetPosition(randomValue, randomSpeed, randomAccel, true);
//     servo3.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
//     servo3.setSpeedAndAcceleration(randomSpeed, randomDirection, randomAccel);
//     servo3.setTargetPosition(randomValue, 100, 10, true);
//     servo3.setSpeedAndAcceleration(1000, true, 10);
//     servo1.setSpeedAndAcceleration(1000, true, 10);
//     servo2.setSpeedAndAcceleration(1000, true, 10);
//     servo2.setTargetPosition(randomValue * 10, 100, 10, true);

//     servo1.setSpeedAndAcceleration(1000, 1, true);
//     servo2.setSpeedAndAcceleration(1000, 1, true);
//     */

//     prevTx = millis();
//   }
//   delay(10);
// }

void task_logProcessor(void *pvParameters)
{
  for (;;)
  {
    if (!logQueue.isEmpty())
    {
      String message = logQueue.dequeue();
      if (message != "")
      {
        Serial.println(message);
      }
    }
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

void task_checkMessages(void *pvParameters)
{
  Debug debug("MAIN", __func__);
  for (;;)
  {
    debug.info();
    debug.print(F("New iteration of taskCheckCanMessages"));
    checkForMessages();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void task_sendPositon(void *pvParameters)
{
  vTaskDelay(4000 / portTICK_PERIOD_MS);
  Debug debug("MAIN", __func__);
  for (;;)
  {
    debug.info();
    debug.print(F("New iteration of taskSendRandomTargetPositionCommands"));
    sendRandomTargetPositionCommands();
    vTaskDelay(4000 / portTICK_PERIOD_MS);
  }
}

void task_QueryPosition(void *pvParameters)
{
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  Debug debug("MAIN", __func__);
  for (;;)
  {
    debug.info();
    debug.print(F("New iteration of taskQueryMotorPosition"));
    queryMotorPositions();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void sendRandomTargetPositionCommands()
{
  Debug debug("MAIN", __func__);
  uint16_t min = -40000;
  uint16_t max = +40000;
  randomValue = random(min, max);
  randomSpeed = random(0, 2000);
  randomDirection = random(0, 1);
  randomAccel = random(0, 255);

  debug.info();
  debug.print(F("called"));

  for (auto &pair : Servos)
  {
    Command *setTargetPositionCommand = new SetTargetPositionCommand(pair.second, randomValue, randomSpeed, randomAccel, true);
    setTargetPositionCommand->execute();
    delete setTargetPositionCommand;
  }
}

void queryMotorPositions()
{
  Debug debug("MAIN", __func__);
  debug.info();
  debug.print(F("called"));

  for (auto &pair : Servos)
  {
    Command *queryMotorPosition = new QueryMotorPositionCommand(pair.second);
    queryMotorPosition->execute();
    delete queryMotorPosition;
  }
}

void checkForMessages()
{
  while (canBus->msgAvailable())
  {
    CanMsg msg;
    canBus->checkForMessages(msg);
    uint32_t id = msg.getStandardId();

    if (Servos[id] == nullptr)
    {
      Serial.println("Servo not found for ID: " + id);
      continue;
    }

    Servos[id]->handleReceivedMessage(msg);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

/* Idle Task with priority Zero */
static void MyIdleTask(void *pvParameters)
{
  while (1)
  {
    Serial.println(F("Idle state"));
    delay(10);
  }
}