#ifndef _BUFFER_H
#define _BUFFER_H

#include <Arduino.h>

#define W12_BUFFER_SIZE 160

class Buffer {
public:
    Buffer();
    virtual ~Buffer();

    /**
     * Read a byte from the buffer.
     */
    bool read(uint8_t &);

    /**
     * Write a byte to the buffer.
     */
    void write(uint8_t);

    /**
     * Get the size of data in the buffer.
     */
    uint8_t size();

private:
    /**
     * Raw array backing the buffer.
     */
    uint8_t mBuffer[W12_BUFFER_SIZE];

    /**
     * Position that the next write will write to.
     */
    uint8_t mNextWrite;

    /**
     * Size of data in the buffer.
     */ 
    uint8_t mSize;
};

#endif