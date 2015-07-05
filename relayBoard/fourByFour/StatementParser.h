#ifndef _STATEMENTPARSER_H
#define _STATEMENTPARSER_H

#include <Arduino.h>

#include "global.h"

class StatementParser {
  public:
    StatementParser();
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

    RelayInterface * mToUse;
};

#endif
