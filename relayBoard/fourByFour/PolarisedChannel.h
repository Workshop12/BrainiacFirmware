#ifndef _POLARISED_CHANNEL
#define _POLARISED_CHANNEL

#include "RelayChannel.h"

class PolarisedChannel : public RelayInterface {
  public:
    PolarisedChannel();
    virtual ~PolarisedChannel();

    void setPins(uint8_t powerPin, uint8_t directionPin);

    virtual uint8_t setState(uint8_t state);
    virtual void poll(uint32_t now);

  protected:
    virtual char getType();

    RelayChannel mDirectionChannel;
    RelayChannel mPowerChannel;

    uint32_t mNextStateChange;

  private:
    void progressStateMachine(uint32_t now);
};

#endif
