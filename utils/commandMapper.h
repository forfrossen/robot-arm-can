#ifndef COMMANDMAPPER_H
#define COMMANDMAPPER_H
#include <avr/pgmspace.h>
#include <map>
#include <string>
#include "../Debug.h"

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

class CommandMapper
{

private:
  std::map<uint8_t, const char *>
      commandMap;

public:
  CommandMapper()
  {
    commandMap = {
        {0x30, cmd0},
        {0x31, cmd1},
        {0x32, cmd2},
        {0x33, cmd3},
        {0x34, cmd4},
        {0x39, cmd5},
        {0x3A, cmd6},
        {0x3B, cmd7},
        {0x3D, cmd8},
        {0x3E, cmd9},
        {0x80, cmd10},
        {0x82, cmd11},
        {0x83, cmd12},
        {0x84, cmd13},
        {0x85, cmd14},
        {0x86, cmd15},
        {0x87, cmd16},
        {0x88, cmd17},
        {0x89, cmd18},
        {0x8A, cmd19},
        {0x8B, cmd20},
        {0x8C, cmd21},
        {0x8D, cmd22},
        {0x8F, cmd23},
        {0x90, cmd24},
        {0x91, cmd25},
        {0x92, cmd26},
        {0x94, cmd27},
        {0xF1, cmd28},
        {0xF2, cmd29},
        {0xF3, cmd30},
        {0xF4, cmd31},
        {0xF5, cmd32},
        {0xF6, cmd33},
        {0xF7, cmd34},
        {0xFF, unknownCommand}};
  }

  // Declaration of the getCommandNameFromCode function
  void getCommandNameFromCode(uint8_t code, char *commandName)
  {
    Debug debug("CommandMapper", __func__);

    debug.info();
    Serial.print(F("Looking for code: "));
    Serial.println(code, HEX);

    auto it = commandMap.find(code);
    if (it != commandMap.end())
    {
      strcpy_P(commandName, it->second);
    }
    else
    {
      strcpy_P(commandName, unknownCommand);
    }

    debug.info();
    Serial.print(F("Found command name: "));
    Serial.println(commandName);
  };
};
#endif