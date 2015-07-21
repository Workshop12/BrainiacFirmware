#ifndef _POLLABLE_H
#define _POLLABLE_H

#include <Arduino.h>

#ifndef _POLLABLE_COUNT
    #define _POLLABLE_COUNT 10
#endif

class Pollable {
  public:
    Pollable(bool add = true);
    virtual ~Pollable();

    virtual void poll(uint32_t now) = 0;
    
    static void pollAll();

  private:
    static void addPollable(Pollable * pollable);
    static void removePollable(Pollable * pollable);
};

#endif
