#include "RelayChannel.h"

RelayInterface::RelayInterface(bool addPollable) : Pollable(addPollable), mTimeout(0), mOffTime(0), mState(0) {
}

RelayInterface::~RelayInterface() {
}

void RelayInterface::setOffTime() {
  if (mTimeout != 0 && mState != 0) {
    uint32_t now = millis();
    mOffTime = now + mTimeout;
  } else {
    mOffTime = 0;
  }
}

uint8_t RelayInterface::getState() {
  return mState;
}

void RelayInterface::poll(uint32_t now) {
  if (mOffTime != 0 && now >= mOffTime) {
    setState(0);
  }
}

void RelayInterface::setTimeout(uint32_t timeout) {
  mTimeout = timeout;

  if (timeout == 0) {
    mOffTime = 0;
  } else {
    mOffTime = millis() + timeout;
  }
}

uint32_t RelayInterface::getTimeout() {
  return mTimeout;
}

void RelayInterface::showSet(uint8_t index) {
  Serial.print(getType());
  Serial.print(':');
  Serial.print(index);
  Serial.print('=');
  Serial.print(mState);
  Serial.print(",t:");
  Serial.print(mTimeout);
}

RelayChannel::RelayChannel(bool addPollable) : RelayInterface(addPollable), mPowerPin(-1) {
}

void RelayChannel::setPin(int8_t powerPin) {
  releasePin();
  mPowerPin = powerPin;
  if (mPowerPin >= 0) {
    setState(false);
    pinMode(mPowerPin, OUTPUT);
  }
}

RelayChannel::~RelayChannel() {
}

void RelayChannel::releasePin() {
  if (mPowerPin >= 0) {
    setState(0);
    pinMode(mPowerPin, INPUT);
  }
}

uint8_t RelayChannel::setState(uint8_t state) {
  digitalWrite(mPowerPin, state == 0); // TODO: This is inverted - for the "8 Relay Module" - LC-202.
  mState = state != 0;
  setOffTime();
  return mState;
}

char RelayChannel::getType() {
  return 's';
}
