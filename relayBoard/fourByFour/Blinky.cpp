#include "Blinky.h"

Blinky::Blinky(uint8_t pin, uint16_t period) : mPin(pin), mPeriod(period), mNextTransition(0), mState(0) {
  pinMode(pin,OUTPUT);
  digitalWrite(pin,0);
}

Blinky::~Blinky() {
  digitalWrite(mPin,0);
  pinMode(mPin,INPUT);
}

void Blinky::poll(uint32_t now) {
  if (mNextTransition == 0) {
    mNextTransition = now + mPeriod;
  } else if (now >= mNextTransition) {
    mState += 1;
    mState %= 2;
    digitalWrite(mPin,mState);
    mNextTransition = mNextTransition + mPeriod;
  }
}


