#include "Serial.h"

#define RXEN RXEN0
#define TXEN TXEN0
#define UCSZ0 UCSZ00
#define RXC RXC0
#define TXC TXC0
#define UDRE UDRE0

W12Serial::W12Serial(uint8_t port, bool isSingleWire) : mSerialSkip(0), mSerialMode(0xff), mIsSingleWire(isSingleWire) {
    #ifdef ARDUINO_AVR_UNO
    {
        mUdr = &UDR0;
        mUcsra = &UCSR0A;
        mUcsrb = &UCSR0B;
        mUcsrc = &UCSR0C;
        mUbrrh = &UBRR0H;
        mUbrrl = &UBRR0L;

        mRX.setup(PORTD,PD0,DDRD,DDD0);
        mTX.setup(PORTD,PD1,DDRD,DDD1);
    }
    #endif
    #ifdef ARDUINO_AVR_ADK
    {
        switch(port) {
            case 0:
            mUdr = &UDR0;
            mUcsra = &UCSR0A;
            mUcsrb = &UCSR0B;
            mUcsrc = &UCSR0C;
            mUbrrh = &UBRR0H;
            mUbrrl = &UBRR0L;

            mRX.setup(PORTE,PE0,DDRE,DDE0);
            mTX.setup(PORTE,PE1,DDRE,DDE1);
            break;
            case 1:
            mUdr = &UDR1;
            mUcsra = &UCSR1A;
            mUcsrb = &UCSR1B;
            mUcsrc = &UCSR1C;
            mUbrrh = &UBRR1H;
            mUbrrl = &UBRR1L;

            mRX.setup(PORTD,PD2,DDRD,DDD2);
            mTX.setup(PORTD,PD3,DDRD,DDD3);
            break;
            case 2:
            mUdr = &UDR2;
            mUcsra = &UCSR2A;
            mUcsrb = &UCSR2B;
            mUcsrc = &UCSR2C;
            mUbrrh = &UBRR2H;
            mUbrrl = &UBRR2L;

            mRX.setup(PORTH,PD0,DDRH,DDH0);
            mTX.setup(PORTH,PD1,DDRH,DDH1);
            break;
        }
    }
    #endif
}

W12Serial::~W12Serial() {
}

void W12Serial::begin(uint32_t baudrate) {
    uint16_t multiplier = 103;
    switch(baudrate) {
        case 2400:
            multiplier = 416;
            break;
        case 4800:
            multiplier = 207;
            break;
        case 9600: 
            multiplier = 103;
            break;
        case 14400: 
            multiplier = 68;
            break;
        case 19200: 
            multiplier = 51;
            break;
        case 28800: 
            multiplier = 34;
            break;
        case 38400: 
            multiplier = 25;
            break;
        case 57600: 
            multiplier = 16;
            break;
        case 76800: 
            multiplier = 12;
            break;
        case 115200: 
            multiplier = 8;
            break;
    }

    *mUbrrh = (multiplier>>8)&0xf;
    *mUbrrl = multiplier;

    *mUcsra = 0;
    setSerialMode(mIsSingleWire ? 0 : 2);
    *mUcsrc = (3 << UCSZ0);

}

void W12Serial::setSerialMode(uint8_t serialMode) {
    if (serialMode != mSerialMode) {
        if (serialMode == 0) {
            // Disable the transmitter.
            // Enable the receiver.

            // Make TX an _input_
            mTX.setDirection(false);
            mTX.write(0);

            mRX.setDirection(false);
            mRX.write(0);

            *mUcsrb = (1 << RXEN);

        } else if (serialMode == 1) {
            // Enable the transmitter.
            *mUcsrb = (1 << TXEN);

            // Make TX an output pin.
            mTX.write(1);
            mTX.setDirection(true);
        } else if (serialMode == 2) {
            *mUcsrb = (1 << RXEN) | (1 << TXEN);
            mRX.setDirection(false);
            mRX.write(0);
            mTX.write(0);
            mTX.setDirection(true);
        }
    }
    mSerialMode = serialMode;
}

void W12Serial::write(uint8_t val) {
    if (mIsSingleWire) {
        setSerialMode(1);
    }
    mWriteBuffer.write(val);
}

bool W12Serial::read(uint8_t & val) {
    return mReadBuffer.read(val);
}

uint32_t lastNow = 0;

void W12Serial::poll(uint32_t now) {
    // Check for received data.
    while(*mUcsra & (1<<RXC)) {
        uint8_t temp = *mUdr;
        if (mSerialSkip!=0) {
            --mSerialSkip;
        } else {
            mReadBuffer.write(temp);
        }
    }

    // Check for ability to write data.
    while(mWriteBuffer.size()>0 && (*mUcsra & (1<<UDRE))) {
        uint8_t toWrite;
        mWriteBuffer.read(toWrite);
        *mUdr = toWrite;
        if (mIsSingleWire) {
            mSerialSkip++;
        }
    }

    if (mSerialMode == 1 && mWriteBuffer.size()==0 && (*mUcsra & (1<<UDRE))) {
        setSerialMode(0);
    }
}


