#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <Arduino.h>

#define SWITCH_CHANNEL_COUNT (4)
#define DIRECTED_CHANNEL_COUNT (4)
#define PWM_CHANNEL_COUNT (0)

#include "RelayChannel.h"
#include "PolarisedChannel.h"

extern RelayChannel switchChannels[SWITCH_CHANNEL_COUNT];
extern PolarisedChannel directedChannels[DIRECTED_CHANNEL_COUNT];

#endif