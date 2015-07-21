#include "Pollable.h"

#include "Strings.h"

static const char _error_pollables[] PROGMEM = "ERROR: Too many pollables for the given array size";
static const PStr error_pollables(_error_pollables);

static Pollable * pollables[_POLLABLE_COUNT];

static uint8_t nullPollables() {
    for (uint8_t i=0; i< _POLLABLE_COUNT; ++i) {
        pollables[i] = 0;
    }
    return 0;
}

static uint8_t pollablesOffset = nullPollables();

Pollable::Pollable(bool addPollable) {
    if (addPollable) {
        Pollable::addPollable(this);
    }
}

Pollable::~Pollable() {
    removePollable(this);
}

void Pollable::addPollable(Pollable * pollable) {
  for (uint8_t i=0; i<_POLLABLE_COUNT; ++i) {
    if (pollables[i] == pollable) {
        return;
    }
  }

  for (uint8_t i=0; i<_POLLABLE_COUNT; ++i) {
    if (pollables[i] == NULL) {
        pollables[i] = pollable;
        return;
    }
  }
  error_pollables.println();
}

void Pollable::removePollable(Pollable * pollable) {
    for (uint8_t i=0; i<_POLLABLE_COUNT; ++i) {
        if (pollables[i] == pollable) {
            pollables[i] = NULL;
            return;
        }
    }
}

void Pollable::pollAll() {
    uint32_t now = millis();
    for (uint8_t i=0; i<_POLLABLE_COUNT; ++i) {
        if (pollables[i] != NULL) {
            pollables[i]->poll(now);
        }
    }
}