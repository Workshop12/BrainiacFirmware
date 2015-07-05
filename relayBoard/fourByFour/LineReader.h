#ifndef _LINE_READER_H
#define _LINE_READER_H

#include "Pollable.h"

class LineReader : public Pollable {
  public:
    LineReader();
    virtual ~LineReader();

    virtual void poll(uint32_t now);
    uint8_t isLineReady();

    void getLine(char * & buffer, uint8_t & length);
  private:
    char mBuffer[40];
    uint8_t mBufferOffset;
    uint8_t mLineReady;
};

#endif
