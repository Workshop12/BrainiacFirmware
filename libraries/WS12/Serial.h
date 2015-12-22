#ifndef _W12_SERIAL_H
#define _W12_SERIAL_H

#include "Pollable.h"
#include "Buffer.h"
#include "pin.h"

class W12Serial : public Pollable {
public:
    W12Serial(uint8_t port, bool isSingleWire = false);
    virtual ~W12Serial();


    void write(uint8_t val);
    bool read(uint8_t & val);

    void begin(uint32_t baudrate);

protected:
    virtual void poll(uint32_t now);

    void setSerialMode(uint8_t serialMode);

private:

    Buffer mReadBuffer;
    Buffer mWriteBuffer;

    /**
     * Data Register
     */
    volatile uint8_t * mUdr;

    /**
     * Control and status register
     */
    volatile uint8_t * mUcsra;
    volatile uint8_t * mUcsrb;
    volatile uint8_t * mUcsrc;

    /**
     * Baud rate register
     */
    volatile uint8_t * mUbrrh;
    volatile uint8_t * mUbrrl;

    /**
     * RX Pin
     */
    Pin mRX;

    /**
     * TX Pin
     */
    Pin mTX;

    uint8_t mSerialMode;
    bool mIsSingleWire;
    uint8_t mSerialSkip;
};


#endif