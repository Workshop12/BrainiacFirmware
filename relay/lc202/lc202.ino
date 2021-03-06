#include <Blinky.h>
#include <RelayChannel.h>
#include <RelayController.h>
#include <Strings.h>

#include "StatementParser.h"

const char _productDetails[] PROGMEM = "lc202 relay 1.0.0.0 ";
const PStr productDetails(_productDetails);

uint8_t RelayChannel::invertedCheck() {
  return 0;
}

RelayChannel switchChannels[8];

RelayController relayController(productDetails, switchChannels, 8, NULL, 0);

void setup() {
  relayController.setup();
}

void loop() {
  relayController.loop();
}