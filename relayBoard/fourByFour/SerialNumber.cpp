#include "SerialNumber.h"

#include <Arduino.h>
#include <avr/eeprom.h>

namespace SerialNumber {

void nibble(uint8_t val) {
  val &= 0xf;
  if (val < 10) {
    Serial.print((char)('0' + val));
  } else {
    Serial.print((char)('a' + val - 10));
  }
}

uint8_t denibble(char c) {
  if (c >= '0' && c <= '9') {
    return c - 48;
  }
  if (c >= 'A' && c <= 'F') {
    return 10 + c - 'A';
  }
  if (c >= 'a' && c <= 'f') {
    return 10 + c - 'a';
  }
  return 0xf;
}

uint8_t twoCharsFromHexToInt(char * c) {
  uint8_t toReturn = denibble(c[0]);
  toReturn <<= 4;
  toReturn |= denibble(c[1]);
  return toReturn;
}

void print() {
  for (uint8_t i = 0; i < 8; ++i) {
    uint8_t val = eeprom_read_byte((uint8_t*)i);
    eeprom_busy_wait();
    nibble(val >> 4);
    nibble(val);
  }
}

void store(char * c, uint8_t length) {
  if (length != 16) {
    return;
  }
  for (uint8_t i = 0; i < length; i += 2) {
    uint8_t val = twoCharsFromHexToInt(c + i);
    eeprom_write_byte((uint8_t*)(i / 2), val);
    eeprom_busy_wait();
  }
}

}

