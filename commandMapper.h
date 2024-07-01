#ifndef COMMANDMAPPER_H
#define COMMANDMAPPER_H

#include <stdint.h>

// Declaration of the CommandMap struct
struct CommandMap
{
  uint8_t code;
  const char *name;
};

// Declaration of the getCommandNameFromCode function
const char *getCommandNameFromCode(uint8_t code);

#endif // COMMANDMAPPER_H