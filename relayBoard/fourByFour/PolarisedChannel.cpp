#include "PolarisedChannel.h"

#include <Arduino.h>

#define STATE_CHANGE_DELAY_MS 30

PolarisedChannel::PolarisedChannel() {
  
}

PolarisedChannel::~PolarisedChannel() {
  mPowerChannel.setState(0);
  delay(10);
  mDirectionChannel.setState(0);
}

void PolarisedChannel::setPins(uint8_t powerPin, uint8_t directionPin) {
  mPowerChannel.setPin(powerPin);
  mDirectionChannel.setPin(directionPin);
}

uint8_t PolarisedChannel::setState(uint8_t state) {
  if (state > 0) {
    // (1+1)%2 = 0 +1 = 1
    // (2+1)%2 = 1 +1 = 2
    // (69+1)%2 = 0 + 1 = 1
    state = (state+1)%2 + 1;
  }
  if (state != mState) {
    mState = state;
    if (mNextStateChange == 0) {
      progressStateMachine(millis());
    }
  }
  setOffTime();
  return mState;
}

void PolarisedChannel::poll(uint32_t now) {
  if (mNextStateChange != 0 && now >= mNextStateChange) {
    progressStateMachine(now);
  }
  RelayInterface::poll(now);
}

void PolarisedChannel::progressStateMachine(uint32_t now) {
  uint8_t desiredPower = mState != 0;
  uint8_t desiredDirection = mState == 2;

  uint8_t currentPower = mPowerChannel.getState();
  uint8_t currentDirection = mDirectionChannel.getState();

//  Serial.print("Desired Power ");
//  Serial.print(desiredPower);
//  Serial.print(" Desired Direction: ");
//  Serial.print(desiredDirection);
//
//  Serial.print(" Current Power: ");
//  Serial.print(currentPower);
//  Serial.print(" Current Direction: ");
//  Serial.print(currentDirection);
//  Serial.println();

  boolean anotherChange = false;

  if (currentDirection != desiredDirection) {
    if (currentPower) {
      mPowerChannel.setState(0);
      anotherChange = true;
    } else {
      mDirectionChannel.setState(desiredDirection);
      anotherChange = desiredPower != currentPower;
    }
  } else if (currentPower != desiredPower) {
    mPowerChannel.setState(desiredPower);
    anotherChange = desiredDirection != currentDirection;
  }

  if (anotherChange) {
    mNextStateChange = now + STATE_CHANGE_DELAY_MS;
  } else {
    mNextStateChange = 0;
  }
}

char PolarisedChannel::getType() {
  return 'd';
}

