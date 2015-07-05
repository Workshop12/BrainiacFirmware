#ifndef _RELAY_CHANNEL_H
#define _RELAY_CHANNEL_H

#include "Pollable.h"

class RelayInterface : public Pollable {
  public:
    RelayInterface();

    void setTimeout(uint32_t timeout);
    uint32_t getTimeout();


    virtual uint8_t setState(uint8_t state) = 0;
    uint8_t getState();

    virtual void poll(uint32_t now);

    void showSet(uint8_t index);

  protected:
    virtual char getType() = 0;
    void setOffTime();

    uint32_t mTimeout;
    uint32_t mOffTime;
    uint8_t mState;
};

class RelayChannel : public RelayInterface {
  public:
    RelayChannel();
    virtual ~RelayChannel();

    void setPin(int8_t powerPin);

    virtual uint8_t setState(uint8_t state);

  protected:
    virtual char getType();

    int8_t mPowerPin;

  private:
    void releasePin();
};

#endif

