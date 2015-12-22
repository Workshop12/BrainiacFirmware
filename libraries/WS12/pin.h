#ifndef _PIN_H
#define _PIN_H

class Pin {
public:
    Pin() : mSetup(false) {
    }

    virtual ~Pin() {
    };

    void setup(volatile uint8_t & port, uint8_t portMask, volatile uint8_t & dd, uint8_t ddMask) {
        mPort = &port;
        mPortMask = portMask;
        mDD = &dd;
        mDDMask = ddMask;
    }

    void setDirection(bool isOutput) {
        if (mSetup) {
            if (isOutput) {
                *mDD |= mDDMask;
            } else {
                *mDD &= ~mDDMask;
            }
        }
    }

    void write(bool value) {
        if (mSetup) {
            if (value) {
                *mPort |= mPortMask;
            } else {
                *mPort &= ~mPortMask;
            }
        }
    }

    bool read() {
        if (mSetup) {
            return (*mPort & mPortMask)!=0;
        }
        return false;
    }

private:
    bool mSetup;
    volatile uint8_t * mPort;
    uint8_t mPortMask;
    volatile uint8_t * mDD;
    uint8_t mDDMask;
};

#endif