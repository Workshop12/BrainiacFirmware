#include <WS12Constants.h>
#include <SerialNumber.h>

#include "RelayController.h"
#include "RelayChannel.h"
#include "PolarisedChannel.h"

RelayController::RelayController(const PStr & productId, RelayChannel * relayChannels, uint8_t relayChannelCount, PolarisedChannel * polarisedChannels, uint8_t polarisedChannelCount) :
  mProductId(productId),
  mSwitchChannels(relayChannels),
  mSwitchChannelCount(relayChannelCount),
  mPolarisedChannels(polarisedChannels),
  mPolarisedChannelCount(polarisedChannelCount),
  mStatementParser(relayChannels,relayChannelCount,polarisedChannels,polarisedChannelCount) {

}

void RelayController::doId() {
  WS12::commandId.print();
  Serial.print(' ');
  mProductId.print();
  SerialNumber::print();
  Serial.print('\n');
}

void RelayController::doEnum() {
  WS12::commandEnum.print();
  Serial.print(' ');
  Serial.print('s');
  Serial.print(':');
  Serial.print(mSwitchChannelCount);
  Serial.print(' ');
  Serial.print('d');
  Serial.print(':');
  Serial.print(mPolarisedChannelCount);
  Serial.print('\n');
}

void RelayController::doSet(char * command, int length) {
  bool any = false;
  Serial.print("set ");
  for (uint8_t i = 0; i < mSwitchChannelCount; ++i) {
    if (any) {
      Serial.print(' ');
    }
    mSwitchChannels[i].showSet(i);
    any = true;
  }
  for (uint8_t i = 0; i < mPolarisedChannelCount; ++i) {
    if (any) {
      Serial.print(' ');
    }
    mPolarisedChannels[i].showSet(i);
    any = true;
  }
  Serial.println();
}

void RelayController::doSer(char * command, int length) {
  if (length != 20) {
    WS12::errorSerUsage.print();
  } else {
    SerialNumber::store(command + 4, length - 4);
    doId();
  }
}

uint8_t RelayController::safePin(uint8_t & nextPin) {
  uint8_t toReturn = nextPin;
  ++nextPin;
  if (toReturn == 13) { // blinky is on 13
    return safePin(nextPin);
  }
  return toReturn;
}

void RelayController::setup() {
  Serial.begin(115200);

  uint8_t pinIndex = 0;

  for (uint8_t i=0; i < mSwitchChannelCount; ++i) {
    mSwitchChannels[i].setPin(pinMap(pinIndex++));
  }

  for (uint8_t i=0; i < mPolarisedChannelCount; ++i) {
    mPolarisedChannels[i].setPins(pinMap(pinIndex++),pinMap(pinIndex++));
  }

  for (uint8_t i = 0; i < 2; ++i) {
    doId();
  }
}

void RelayController::processCommand(char * buffer, uint8_t length) {
  bool newLine = true;
  if (WS12::commandId.equals(buffer, length)) {
    doId();
  } else if (WS12::commandEnum.equals(buffer, length)) {
    doEnum();
  } else if (WS12::commandSer.begins(buffer, length)) {
    doSer(buffer, length);
    Serial.print('\n');
  } else if (WS12::commandSet.begins(buffer, length)) {
    doSet(buffer, length);
  } else if (length > 2 && buffer[1] == ':' && (buffer[0] == 's' || buffer[0] == 'd' || buffer[0] == 'p')) {
    Serial.print(buffer[0]);
    Serial.print(' ');
    if (mStatementParser.doStatement(buffer, length)) {
      Serial.print("OK\n");
    } else {
      Serial.print("FAIL\n");
    }
  } else if (length == 0) {
    // do nothing.
    newLine = false;
  } else {
    for (uint8_t i = 0; i < length && (i==0 || (buffer[i] != ' ' && buffer[i] != ':')); ++i) {
      Serial.print(buffer[i]);
    }
    Serial.print(' ');
    Serial.print('?');
    Serial.print('\n');;
  }
}

void RelayController::loop() {
  Pollable::pollAll();

  if (mLineReader.isLineReady()) {
    char * buffer;
    uint8_t length;
    mLineReader.getLine(buffer, length);
    processCommand(buffer, length);
  }
}






