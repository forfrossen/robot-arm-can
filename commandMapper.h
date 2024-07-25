#ifndef COMMANDMAPPER_H
#define COMMANDMAPPER_H

#include <stdint.h>

// Declaration of the CommandMap struct
struct CommandMap
{
  uint8_t code;
  const char *name;
};

const CommandMap commandMap[] = {
    {0x30, "Read encoder value (carry)"},
    {0x31, "Read encoder value (addition)"},
    {0x32, "Read motor speed"},
    {0x33, "Read number of pulses"},
    {0x34, "Read IO Ports status"},
    {0x39, "Read motor shaft angle error"},
    {0x3A, "Read En pin status"},
    {0x3B, "Read go back to zero status"},
    {0x3D, "Release motor shaft locked-rotor protection state"},
    {0x3E, "Read motor shaft protection state"},
    {0x80, "Calibrate encoder"},
    {0x82, "Set work mode"},
    {0x83, "Set working current"},
    {0x84, "Set subdivisions"},
    {0x85, "Set active level of En pin"},
    {0x86, "Set motor rotation direction"},
    {0x87, "Set auto turn off screen"},
    {0x88, "Set motor shaft locked-rotor protection function"},
    {0x89, "Set subdivision interpolation function"},
    {0x8A, "Set CAN bitRate"},
    {0x8B, "Set CAN ID"},
    {0x8C, "Set slave respond and active"},
    {0x8D, "Set group ID"},
    {0x8F, "Set key lock/unlock"},
    {0x90, "Set parameters of home"},
    {0x91, "Go home"},
    {0x92, "Set current axis to zero"},
    {0x94, "Set parameter of 'noLimit' go home"},
    {0xF1, "Query motor status"},
    {0xF2, "Query motor position"},
    {0xF3, "Enable the motor"},
    {0xF4, "Relative position control mode"},
    {0xF5, "Absolute position control mode"},
    {0xF6, "Speed mode command"},
    {0xF7, "Stop motor"},
};

// Declaration of the getCommandNameFromCode function
const char *getCommandNameFromCode(uint8_t code)
{
  for (unsigned int i = 0; i < sizeof(commandMap) / sizeof(commandMap[0]); ++i)
  {
    if (commandMap[i].code == code)
    {
      return commandMap[i].name;
    }
  }
  return "Unknown command";
}

#endif // COMMANDMAPPER_H