#include "buffer.h"

Buffer::Buffer() : mNextWrite(0), mSize(0) {
}

Buffer::~Buffer() {
}

bool Buffer::read(uint8_t & output) {
    if (mSize==0) {
        return false;
    }
    uint8_t where = (W12_BUFFER_SIZE + mNextWrite - mSize) % W12_BUFFER_SIZE;
    output = mBuffer[where];
    --mSize;
    return true;
}

uint8_t Buffer::size() {
    return mSize;
}

void Buffer::write(uint8_t toWrite) {
    mBuffer[mNextWrite++] = toWrite;
    mNextWrite %= W12_BUFFER_SIZE;

    if (mSize != W12_BUFFER_SIZE) {
        ++mSize;
    }
}