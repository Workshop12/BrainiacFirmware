#ifndef _POLLABLE_H
#define _POLLABLE_H

#include <Arduino.h>

class Pollable {
public:
  virtual void poll(uint32_t now) = 0;
};

#endif
