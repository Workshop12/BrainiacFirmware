#include <Blinky.h>
#include <RelayChannel.h>
#include <RelayController.h>
#include <Strings.h>

#include "StatementParser.h"

const uint8_t BLINKY_PIN = 13;
const uint16_t BLINKY_PERIOD = 500;
const uint8_t LC202_RELAY_COUNT = 8;

const char _productDetails[] PROGMEM = "lc202 relay 1.0.0.0 ";
const PStr productDetails(_productDetails);

uint8_t RelayChannel::invertedCheck() {
  return 0;
}

RelayChannel switchChannels[LC202_RELAY_COUNT];

uint8_t RelayController::pinMap(uint8_t index) {
    uint8_t pin = index + 2;
    if (pin >= BLINKY_PIN) {
        pin += 1;
    }
    return pin;
}

RelayController relayController(productDetails, switchChannels, LC202_RELAY_COUNT, NULL, 0);

Blinky blinky(BLINKY_PIN,BLINKY_PERIOD);

void setup() {
  relayController.setup();
}

void loop() {
  relayController.loop();
}