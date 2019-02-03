#include <Blinky.h>
#include <LineReader.h>
#include <Pollable.h>
#include <SerialNumber.h>
#include <Strings.h>
#include <WS12Constants.h>

#include "global.h"

#include "StatementParser.h"

Blinky blinky(13, 500);
LineReader lineReader;

RelayChannel switchChannels[SWITCH_CHANNEL_COUNT];
PolarisedChannel directedChannels[DIRECTED_CHANNEL_COUNT];

void doId() {
  WS12::commandId.print();
  Serial.print(' ');
  product_details.print();
  SerialNumber::print();
  Serial.print('\n');
}

void doEnum() {
  command_enum.print();
  Serial.print(' ');
  Serial.print('s');
  Serial.print(':');
  Serial.print(SWITCH_CHANNEL_COUNT);
  Serial.print(' ');
  Serial.print('d');
  Serial.print(':');
  Serial.print(DIRECTED_CHANNEL_COUNT);
  Serial.print('\n');
}

void doSet(char * command, int length) {
  bool any = false;
  Serial.print("set ");
  for (uint8_t i = 0; i < SWITCH_CHANNEL_COUNT; ++i) {
    if (any) {
      Serial.print(' ');
    }
    switchChannels[i].showSet(i);
    any = true;
  }
  for (uint8_t i = 0; i < DIRECTED_CHANNEL_COUNT; ++i) {
    if (any) {
      Serial.print(' ');
    }
    directedChannels[i].showSet(i);
    any = true;
  }
  Serial.println();
}

void doSer(char * command, int length) {
  if (length != 20) {
    WS12::errorSerUsage.print();
  } else {
    SerialNumber::store(command + 4, length - 4);
    doId();
  }
}

uint8_t safePin(uint8_t & nextPin) {
  uint8_t toReturn = nextPin;
  ++nextPin;
  if (toReturn == 13) { // blinky is on 13
    return safePin(nextPin);
  }
  return toReturn;
}

void setup() {
  Serial.begin(115200);

  uint8_t nextPin = 2;

  for (uint8_t i=0; i < SWITCH_CHANNEL_COUNT; ++i) {
    switchChannels[i].setPin(safePin(nextPin));
  }

  for (uint8_t i=0; i < DIRECTED_CHANNEL_COUNT; ++i) {
    directedChannels[i].setPins(safePin(nextPin),safePin(nextPin));
  }

  for (uint8_t i = 0; i < 2; ++i) {
    doId();
  }
}

StatementParser statementParser;

void processCommand(char * buffer, uint8_t length) {
  bool newLine = true;
  if (WS12::commandId.equals(buffer, length)) {
    doId();
  } else if (command_enum.equals(buffer, length)) {
    doEnum();
  } else if (WS12::commandSer.begins(buffer, length)) {
    doSer(buffer, length);
    Serial.print('\n');
  } else if (command_set.begins(buffer, length)) {
    doSet(buffer, length);
  } else if (length > 2 && buffer[1] == ':' && (buffer[0] == 's' || buffer[0] == 'd' || buffer[0] == 'p')) {
    Serial.print(buffer[0]);
    Serial.print(' ');
    if (statementParser.doStatement(buffer, length)) {
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

void loop() {
  Pollable::pollAll();

  if (lineReader.isLineReady()) {
    char * buffer;
    uint8_t length;
    lineReader.getLine(buffer, length);
    processCommand(buffer, length);
  }
}






