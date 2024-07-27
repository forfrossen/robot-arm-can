#ifndef COMMANDMAPPER_H
#define COMMANDMAPPER_H

// Store the command names in regular RAM
const char *commandNames[] = {
    "Read encoder value (carry)",
    "Read encoder value (addition)",
    "Read motor speed",
    "Read number of pulses",
    "Read IO Ports status",
    "Read motor shaft angle error",
    "Read En pin status",
    "Read go back to zero status",
    "Release motor shaft locked-rotor protection state",
    "Read motor shaft protection state",
    "Calibrate encoder",
    "Set work mode",
    "Set working current",
    "Set subdivisions",
    "Set active level of En pin",
    "Set motor rotation direction",
    "Set auto turn off screen",
    "Set motor shaft locked-rotor protection function",
    "Set subdivision interpolation function",
    "Set CAN bitRate",
    "Set CAN ID",
    "Set slave respond and active",
    "Set group ID",
    "Set key lock/unlock",
    "Set parameters of home",
    "Go home",
    "Set current axis to zero",
    "Set parameter of 'noLimit' go home",
    "Query motor status",
    "Query motor position",
    "Enable the motor",
    "Relative position control mode",
    "Absolute position control mode",
    "Speed mode command",
    "Stop motor",
    "Unknown command"};

// Map command codes to indices of the command names array
const uint8_t commandIndices[] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x39, 0x3A, 0x3B, 0x3D, 0x3E,
    0x80, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A,
    0x8B, 0x8C, 0x8D, 0x8F, 0x90, 0x91, 0x92, 0x94, 0xF1, 0xF2,
    0xF3, 0xF4, 0xF5, 0xF6, 0xF7};

// Declaration of the getCommandNameFromCode function
const char *getCommandNameFromCode(const uint8_t *code)
{
  Serial.print(F("Looking for code: "));
  Serial.println(*code);

  unsigned int numCommands = sizeof(commandIndices) / sizeof(commandIndices[0]);
  for (unsigned int i = 0; i < numCommands; ++i)
  {
    if (commandIndices[i] == *code)
    {
      Serial.print(F("Found command name: "));
      Serial.println(commandNames[i]);
      return commandNames[i];
    }
  }
  Serial.println(F("Unknown command"));
  return commandNames[numCommands];
}

#endif // COMMANDMAPPER_H