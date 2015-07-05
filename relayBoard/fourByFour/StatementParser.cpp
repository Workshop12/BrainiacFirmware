#include "StatementParser.h"

StatementParser::StatementParser() {

}

bool StatementParser::doStatement(char * buffer, uint8_t length) {
  mBuffer = buffer;
  mLength = length;
  mOffset = 0;

  mService = 0;
  mInstance = 0;
  mSubService = 0;
  mValue = 0;
  mToUse = NULL;

  if (readService() && readChar(':') && readInstance() && readSubStatement()) {
    return true;
  }
  return false;
}

bool StatementParser::readService() {
  if (mOffset < mLength) {
    mService = mBuffer[mOffset++];
    if (mService == 's' || mService == 'd') {
      return true;
    }
  }
  return false;
}

bool StatementParser::readInstance() {
  if (mOffset < mLength) {
    mInstance = mBuffer[mOffset++] - '0';

    if (mService == 's' && mInstance >= SWITCH_CHANNEL_COUNT) {
      return false;
    }
    if (mService == 'd' && mInstance >= DIRECTED_CHANNEL_COUNT) {
      return false;
    }

    switch (mService) {
      case 's':
        mToUse = &switchChannels[mInstance];
        break;
      case 'd':
        mToUse = &directedChannels[mInstance];
        break;
      default:
        return false;
    }

    return true;
  }
  return false;
}

bool StatementParser::readSubStatement() {
  if (mOffset < mLength) {
    char next = mBuffer[mOffset++];
    if (next == ':') {
      return readSubService() && readChar(' ') && readValue() && setSubservice();
    }
    if (next == ' ') {
      return readValue() && setState();
    }
  }
  return false;
}

bool StatementParser::readSubService() {
  if (mOffset < mLength) {
    mSubService = mBuffer[mOffset++];
    if (mSubService == 't') {
      return true;
    }
  }
  return false;
}

bool StatementParser::readChar(char expected) {
  if (mOffset < mLength) {
    char next = mBuffer[mOffset++];
    if (next == expected) {
      return true;
    }
  }
  return false;
}

bool StatementParser::readValue() {
  return parseInt(mBuffer + mOffset, mLength - mOffset, mValue);
}

bool StatementParser::setState() {
  if (mToUse != NULL) {
    mToUse->setState(mValue);
    return true;
  }
  return false;
}

bool StatementParser::setSubservice() {
  if (mToUse != NULL) {
    switch (mSubService) {
      case 't':
        mToUse->setTimeout(mValue);
        return true;
    }
  }
  return false;
}

bool StatementParser::parseInt(char * buffer, uint8_t length, uint32_t & toReturn) {
  toReturn = 0;
  if (length == 0) {
    return false;
  }
  for (uint8_t i = 0; i < length; ++i) {
    char next = buffer[i];
    if (next >= '0' && next <= '9') {
      toReturn *= 10;
      toReturn += next - '0';
    } else {
      return false;
    }
  }

  return true;
}

