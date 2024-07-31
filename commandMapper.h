#ifndef COMMANDMAPPER_H
#define COMMANDMAPPER_H
#include <avr/pgmspace.h>
#include "Debug.h"
#include <mcp2515.h>

struct CommandMap
{
  uint8_t commandNum;
  const char *commandName;
};

static const char cmd0[] PROGMEM = "Read encoder value (carry)";
static const char cmd1[] PROGMEM = "Read encoder value (addition)";
static const char cmd2[] PROGMEM = "Read motor speed";
static const char cmd3[] PROGMEM = "Read number of pulses";
static const char cmd4[] PROGMEM = "Read IO Ports status";
static const char cmd5[] PROGMEM = "Read motor shaft angle error";
static const char cmd6[] PROGMEM = "Read En pin status";
static const char cmd7[] PROGMEM = "Read go back to zero status";
static const char cmd8[] PROGMEM = "Release motor shaft locked-rotor protection state";
static const char cmd9[] PROGMEM = "Read motor shaft protection state";
static const char cmd10[] PROGMEM = "Calibrate encoder";
static const char cmd11[] PROGMEM = "Set work mode";
static const char cmd12[] PROGMEM = "Set working current";
static const char cmd13[] PROGMEM = "Set subdivisions";
static const char cmd14[] PROGMEM = "Set active level of En pin";
static const char cmd15[] PROGMEM = "Set motor rotation direction";
static const char cmd16[] PROGMEM = "Set auto turn off screen";
static const char cmd17[] PROGMEM = "Set motor shaft locked-rotor protection function";
static const char cmd18[] PROGMEM = "Set subdivision interpolation function";
static const char cmd19[] PROGMEM = "Set CAN bitRate";
static const char cmd20[] PROGMEM = "Set CAN ID";
static const char cmd21[] PROGMEM = "Set slave respond and active";
static const char cmd22[] PROGMEM = "Set group ID";
static const char cmd23[] PROGMEM = "Set key lock/unlock";
static const char cmd24[] PROGMEM = "Set parameters of home";
static const char cmd25[] PROGMEM = "Go home";
static const char cmd26[] PROGMEM = "Set current axis to zero";
static const char cmd27[] PROGMEM = "Set parameter of 'noLimit' go home";
static const char cmd28[] PROGMEM = "Query motor status";
static const char cmd29[] PROGMEM = "Query motor position";
static const char cmd30[] PROGMEM = "Enable the motor";
static const char cmd31[] PROGMEM = "Relative position control mode";
static const char cmd32[] PROGMEM = "Absolute position control mode";
static const char cmd33[] PROGMEM = "Speed mode command";
static const char cmd34[] PROGMEM = "Stop motor";
static const char unknownCommand[] PROGMEM = "Unknown command";
static const char *const commandNames[36] PROGMEM = {
    cmd0, cmd1, cmd2, cmd3, cmd4, cmd5, cmd6, cmd7, cmd8, cmd9,
    cmd10, cmd11, cmd12, cmd13, cmd14, cmd15, cmd16, cmd17, cmd18, cmd19,
    cmd20, cmd21, cmd22, cmd23, cmd24, cmd25, cmd26, cmd27, cmd28, cmd29,
    cmd30, cmd31, cmd32, cmd33, cmd34, unknownCommand};

__u8 commandIndices[] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x39, 0x3A, 0x3B, 0x3D, 0x3E,
    0x80, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A,
    0x8B, 0x8C, 0x8D, 0x8F, 0x90, 0x91, 0x92, 0x94, 0xF1, 0xF2,
    0xF3, 0xF4, 0xF5, 0xF6, 0xF7};
class CommandMapper
{
public:
  CommandMapper() {}
  // Declaration of the getCommandNameFromCode function
  void getCommandNameFromCode(__u8 code, char *commandName)
  {
    Debug debug("CommandMapper", __func__);

    debug.info();
    Serial.print(F("Looking for code: "));
    Serial.println(code, HEX);

    unsigned int numCommands = sizeof(commandIndices) / sizeof(commandIndices[0]);
    unsigned int i = 0;

    do
    {
      if (commandIndices[i] == code)
        break;
      i++;
    } while (i < numCommands);

    strcpy_P(commandName, (char *)pgm_read_ptr(&(commandNames[i])));
    debug.info();
    Serial.print(F("Found command name: "));
    Serial.println(commandName);
  }
};

#endif