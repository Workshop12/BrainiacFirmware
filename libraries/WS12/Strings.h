#ifndef _STRINGS_H
#define _STRINGS_H

#include <Arduino.h>

class PStr {
  public:
    PStr(const char * buffer);

    char operator[](uint8_t) const;
    char operator[](uint8_t);
    char charAt(uint8_t) const;

    bool equals(char * buffer, uint8_t length) const;
    bool begins(char * buffer, uint8_t length) const;

    void print() const;
    void println() const;

  private:
    const char * mBuffer;
};

#endif
