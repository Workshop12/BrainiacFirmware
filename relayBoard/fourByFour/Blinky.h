#ifndef _BLINKY_H
#define _BLINKY_H

#include "Pollable.h"

class Blinky : public Pollable {
  public:
    Blinky(uint8_t pin, uint16_t period);
    virtual ~Blinky();

    void poll(uint32_t now);

  private:
    uint8_t mState;
    uint8_t mPin;
    uint16_t mPeriod;
    uint32_t mNextTransition;
};

#endif _BLINKY_H

