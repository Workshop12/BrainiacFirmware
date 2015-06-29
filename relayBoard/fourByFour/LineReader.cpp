#include "LineReader.h"

LineReader::LineReader() : mBufferOffset(0), mLineReady(0) {

}

LineReader::~LineReader() {

}

void LineReader::poll(uint32_t now) {
  // now is ignored.
  if (mLineReady) {
    return;
  }
  while (Serial.available()) {
    uint8_t newChar = Serial.read();
    Serial.print((char)newChar);
    if (newChar == 10) {
      mLineReady = 1;
      break;
    } else if (newChar >= 32) {
      mBufferOffset %= sizeof(mBuffer);
      mBuffer[mBufferOffset++] = newChar;
    }
  }
}

void LineReader::getLine(char * & buffer, uint8_t & length) {
  if (mLineReady) {
    buffer = mBuffer;
    length = mBufferOffset;
  } else {
    buffer = NULL;
    length = 0;
  }
  mBufferOffset = 0;
  mLineReady = 0;
}

uint8_t LineReader::isLineReady() {
  return mLineReady;
}

