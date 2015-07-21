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

extern const PStr string_prompt;

extern const PStr error_pollables;
extern const PStr error_serUsage;

extern const PStr command_enum;
extern const PStr command_id;
extern const PStr command_ser;
extern const PStr command_set;

extern const PStr product_details;

#endif
