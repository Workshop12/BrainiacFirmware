#include "LineReader.h"
#include "RelayChannel.h"
#include "PolarisedChannel.h"
#include "Blinky.h"

#include "Strings.h"
#include "SerialNumber.h"

Blinky blinky(13, 500);
LineReader lineReader;

Pollable * pollables[10];
uint8_t pollablesOffset = 0;

#define SWITCH_CHANNEL_COUNT (4)
#define DIRECTED_CHANNEL_COUNT (4)
#define PWM_CHANNEL_COUNT (0)

RelayChannel switchChannels[SWITCH_CHANNEL_COUNT];
PolarisedChannel directedChannels[DIRECTED_CHANNEL_COUNT];

bool parseInt(char * buffer, uint8_t length, uint32_t & toReturn) {
  toReturn = 0;
  if (length == 0) {
    return false;
  }
  for (uint8_t i = 0; i<length; ++i) {
    char next = buffer[i];
    if (next >= '0' && next <= '9') {
      toReturn *= 10;
      toReturn += next -'0';
    } else {
      return false;
    }
  }

  return true;
}

void doId() {
  product_details.print();
  SerialNumber::print();
}

void doEnum() {
  Serial.print('s');
  Serial.print(':');
  Serial.print(SWITCH_CHANNEL_COUNT);
  Serial.print(',');
  Serial.print(' ');
  Serial.print('d');
  Serial.print(':');
  Serial.print(DIRECTED_CHANNEL_COUNT);
}

void doSet(char * command, int length) {
  for (uint8_t i=0; i<SWITCH_CHANNEL_COUNT; ++i) {
    switchChannels[i].showSet(i);
  }
  for (uint8_t i=0; i<DIRECTED_CHANNEL_COUNT; ++i) {
    directedChannels[i].showSet(i);
  }
}

void doSer(char * command, int length) {
  if (length != 20) {
    error_serUsage.print();
  } else {
    SerialNumber::store(command + 4, length - 4);
    doId();
  }
}


void addPollable(Pollable * pollable) {
  if (pollablesOffset < sizeof(pollables) / sizeof(pollable)) {
    pollables[pollablesOffset++] = pollable;
  } else {
    error_pollables.println();
  }
}

void setup() {
  for (uint8_t i=0; i<sizeof(pollables)/sizeof(Pollable*); ++i) {
    pollables[i] = NULL;
  }

  Serial.begin(9600);
  addPollable(&blinky);
  addPollable(&lineReader);

  directedChannels[0].setPins(4,5);
  directedChannels[1].setPins(6,7);
  directedChannels[2].setPins(8,9);
  directedChannels[3].setPins(10,11);

  switchChannels[0].setPin(12);
  switchChannels[1].setPin(13);
  switchChannels[2].setPin(14);
  switchChannels[3].setPin(15);

  for (uint8_t i = 0; i < 4; ++i) {
    addPollable(&directedChannels[i]);
    addPollable(&switchChannels[i]);
  }

  for (uint8_t i = 0; i < 5; ++i) {
    string_prompt.println();
  }

  directedChannels[0].setState(2);
}

class StatementParser {
public:
  StatementParser() {

  }

  bool doStatement(char * buffer, uint8_t length) {
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

private:

  bool readService() {
    if (mOffset<mLength) {
      mService = mBuffer[mOffset++];
      if (mService == 's' || mService == 'd') {
        return true;
      }
    }
    return false;
  }

  bool readInstance() {
    if (mOffset < mLength) {
      mInstance = mBuffer[mOffset++] - '0';

      if (mService == 's' && mInstance >= SWITCH_CHANNEL_COUNT) {
        return false;
      }
      if (mService == 'd' && mInstance >= DIRECTED_CHANNEL_COUNT) {
        return false;
      }

      switch(mService) {
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

  bool readSubStatement() {
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

  bool readSubService() {
    if (mOffset < mLength) {
      mSubService = mBuffer[mOffset++];
      if (mSubService == 't') {
        return true;
      }
    }
    return false;
  }

  bool readChar(char expected) {
    if (mOffset < mLength) {
      char next = mBuffer[mOffset++];
      if (next == expected) {
        return true;
      }
    }
    return false;
  }

  bool readValue() {
    return parseInt(mBuffer+mOffset,mLength-mOffset,mValue);
  }

  bool setState() {
    if (mToUse != NULL) {
      mToUse->setState(mValue);
      return true;
    }
    return false;
  }

  bool setSubservice() {
    if (mToUse != NULL) {
      switch(mSubService) {
        case 't':
          mToUse->setTimeout(mValue);
          return true;
      }
    }
    return false;
  }

  char * mBuffer;
  uint8_t mLength;
  uint8_t mOffset;

  char mService;
  char mSubService;
  uint8_t mInstance;
  uint32_t mValue;

  RelayInterface * mToUse;
};

StatementParser statementParser;

void processCommand(char * buffer, uint8_t length) {
  bool newLine = true;
  if (command_id.equals(buffer, length)) {
    doId();
  } else if (command_enum.equals(buffer, length)) {
    doEnum();
  } else if (command_ser.begins(buffer, length)) {
    doSer(buffer, length);
  } else if (command_set.begins(buffer, length)) {
    doSet(buffer, length);
    newLine = false;
  } else if (length > 2 && buffer[1] == ':' && (buffer[0] == 's' || buffer[0] == 'd' || buffer[0] == 'p')) {
    if (statementParser.doStatement(buffer,length)) {
      Serial.print("OK");
    } else {
      Serial.print("FAIL");
    }
  } else if (length == 0) {
    // do nothing.
    newLine = false;
  } else {
    Serial.print("???");
  }
  if (newLine) {
    Serial.println();
  }
  string_prompt.print();
}

void loop() {
  uint32_t now = millis();

  for (uint8_t i = 0; i < pollablesOffset; ++i) {
    pollables[i]->poll(now);
  }

  if (lineReader.isLineReady()) {
    char * buffer;
    uint8_t length;
    lineReader.getLine(buffer, length);
    processCommand(buffer, length);
  }
}






