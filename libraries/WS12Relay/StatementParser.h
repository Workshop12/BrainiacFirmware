#ifndef _STATEMENTPARSER_H
#define _STATEMENTPARSER_H

#include <Arduino.h>

class StatementParser {
  public:
    StatementParser(class RelayChannel * switchChannels, uint8_t switchChannelCount, class PolarisedChannel * polarisedChannels, uint8_t polarisedChannelCount);
    bool doStatement(char * buffer, uint8_t length);

  private:

    bool readService();
    bool readInstance();
    bool readSubStatement();
    bool readSubService();
    bool readChar(char expected);
    bool readValue();
    bool setState();
    bool setSubservice();

    bool parseInt(char * buffer, uint8_t length, uint32_t & toReturn);

    char * mBuffer;
    uint8_t mLength;
    uint8_t mOffset;

    char mService;
    char mSubService;
    uint8_t mInstance;
    uint32_t mValue;

    class RelayInterface * mToUse;

    class RelayChannel * mSwitchChannels;
    uint8_t mSwitchChannelCount;
    class PolarisedChannel * mPolarisedChannels;
    uint8_t mPolarisedChannelCount;
};

#endif
