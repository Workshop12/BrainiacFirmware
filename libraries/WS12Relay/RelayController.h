#ifndef _RELAY_CONTROLLER_H
#define _RELAY_CONTROLLER_H

#include <Blinky.h>
#include <LineReader.h>
#include <StatementParser.h>
#include <Strings.h>

class RelayController {
public:
    RelayController(const PStr & productId, RelayChannel * switchChannels, uint8_t switchChannelCount, PolarisedChannel * polarisedChannels, uint8_t polarisedChannelCount);
    void setup();
    void loop();
private:
    LineReader mLineReader;
    Blinky mBlinky;
    StatementParser mStatementParser;
    const PStr & mProductId;

    class RelayChannel * mSwitchChannels;
    uint8_t mSwitchChannelCount;

    class PolarisedChannel * mPolarisedChannels;
    uint8_t mPolarisedChannelCount;

    void doId();
    void doEnum();
    void doSet(char * command, int length);
    void doSer(char * command, int length);
    uint8_t safePin(uint8_t & nextPin);

    void processCommand(char * buffer, uint8_t length);
};

#endif // _RELAY_CONTROLLER_H