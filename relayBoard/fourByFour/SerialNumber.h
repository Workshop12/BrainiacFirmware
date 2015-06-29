#ifndef _SERIAL_NUMBER_H
#define _SERIAL_NUMBER_H

#include <Arduino.h>

namespace SerialNumber {
  void print();
  void store(char * c, uint8_t length);
}

#endif
