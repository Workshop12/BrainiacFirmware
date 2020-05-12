#include <RelayChannel.h>
#include <RelayController.h>
#include <Strings.h>

#include "StatementParser.h"

const uint8_t LC202_RELAY_COUNT = 8;

const char _productDetails[] PROGMEM = "lc202-geraldjust relay 1.0.0.0 ";
const PStr productDetails(_productDetails);

uint8_t RelayChannel::invertedCheck() {
  return 0;
}

RelayChannel switchChannels[LC202_RELAY_COUNT];


uint8_t RelayController::pinMap(uint8_t index) {
  static uint8_t map[LC202_RELAY_COUNT] = {
     9, //PB4 or D9 etc...
    10, //PC7
    A6, //PB5
    A7, //PB6
    A8, //PB7
    11, //PD0
    12, //PC0
    13  //PD1
  };
  if (index <= sizeof(map) / sizeof(uint8_t)) {
    return map[index];
  }
  return map[0];
}

RelayController relayController(productDetails, switchChannels, LC202_RELAY_COUNT, NULL, 0);

void setup() {
  relayController.setup();
}

void loop() {
  relayController.loop();
}
