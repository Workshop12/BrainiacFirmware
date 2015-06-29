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

  directedChannels[0].setPins(7, 8);

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

// uint8_t doStatement2(char * buffer, uint8_t length) {
//   char service = 0;
//   uint8_t instance = 0;
//   char subService = 0;

//   uint32_t value = 0;
//   bool valueValid = false;

//   RelayInterface * toUse = NULL;

//   uint8_t state = 0;
//   for (uint8_t i=0; i<length; ++i) {
//     char next = buffer[i];
//     if (state == 0) {
//       service = next;
//       if (service != 's' && service != 'd') {
//         return 1;
//       }
//       state = 1;
//     } else if (state == 1) {
//       if (next != ':') {
//         return 2;
//       } else {
//         state == 2;
//       }
//     } else if (state == 2) {
//       if (next <'0' || next >'9') {
//         return 3;
//       }
//       instance = next - '0';

//       if (service == 's' && instance >= SWITCH_CHANNEL_COUNT) {
//         return 4;
//       }
//       if (service == 'd' && instance >= DIRECTED_CHANNEL_COUNT) {
//         return 5;
//       }

//       if (service == 's') {
//         toUse = &switchChannels[instance];
//       }
//       if (service == 'd') {
//         toUse = &directedChannels[instance];
//       }

//       if (toUse == NULL) {
//         return 6;
//       }
//       state = 3;
//     } else if (state == 3) {
//       if (next == ' ') {
//         state = 4;
//       } else if (next == ':') {
//         state = 50;
//       } else {
//         return 7;
//       }
//     } else if (state == 4) {
//       valueValid = parseInt(buffer+i,length-i,value);
//       if (valueValid) {
//         toUse->setState(value);
//         Serial.println("OK");
//       } else {
//         return 8;
//       }
//     } else if (state==50) {
//       subService = next;
//       if (subService != 't') {
//         return 9;
//       }
//       state = 51;
//     } else if (state==51) {
//       if (next == ' ') {
//         state = 52;
//       } else {
//         return 10;
//       }
//     } else if (state==52) {
//       valueValid = parseInt(vuffer+i,length-i,value);
//       if (valueValid) {
//         switch(subService) {
//           case 't':
//             toUse->setTimeout(value);
//             break;
//           default:
//             return 11;
//         }
//       } else {
//         return 12;
//       }
//     }
//   }
//   return 13;
// }

// uint8_t doStatement(char * buffer, uint8_t length) {
//   uint8_t offset = 0;
//   if (offset == length) {
//     return 1;
//   }

//   char service = buffer[offset++];
//   if (offset == length) {
//     return 2;
//   }
//   if (service != 's' && service != 'd') {
//     return 3;
//   }
//   char colon = buffer[offset++];
//   if (colon != ':') {
//     return 4;
//   }
//   if (offset == length) {
//     return 5;
//   }
//   char instanceChar = buffer[offset++];
//   int8_t instance = instanceChar - '0';

//   if (instance < 0) {
//     return 6;
//   }
//   if (service == 's' && instance >= SWITCH_CHANNEL_COUNT) {
//     return 7;
//   }
//   if (service == 'd' && instance >= DIRECTED_CHANNEL_COUNT) {
//     return 8;
//   }

//   RelayInterface * toUse = NULL;
//   switch(service) {
//     case 's':
//       toUse = switchChannels[instance];
//       break;
//     case 'd':
//       toUse = directedChannels[instance];
//       break;
//     default:
//       return 17;
//   }

//   if (offset == length) {
//     return 9;
//   }
//   colon = buffer[offset++];
//   if (offset == length) {
//     return 10;
//   }
//   if (colon == ' ') {
//     uint32_t value;
//     if (!parseInt(buffer + offset, length - offset, value)) {
//       return 11;
//     }
//     uint8_t stateToPrint = toUse->setState(value);
//     Serial.print(service);
//     Serial.print(':');
//     Serial.print(instance);
//     Serial.print(' ');
//     Serial.println(stateToPrint);
//   } else if (colon == ':') {
//     char subservice = buffer[offset++];
//     if (subservice != 't') {
//       return 12;
//     }
//     if (offset == length) {
//       return 13;
//     }
//     char space = buffer[offset++];
//     if (space != ' ') {
//       return 14;
//     }
//     if (offset == length) {
//       return 15;
//     }

//     uint32_t value;
//     if (!parseInt(buffer+offset,length-offset,value)) {
//       return 16;
//     }

//     switch(subservice) {
//       case 't':
//         toUse->setTimeout(value);
//         break;
//       deafult:
//         return 18;
//     }

//     Serial.print(service);
//     Serial.print(':');
//     Serial.print(instance);
//     Serial.print(':');
//     Serial.print(subservice);
//     Serial.print(' ');
//     Serial.println(value);
//   }

//   return 0;
// }

// uint8_t nextMode = 0;
// uint32_t nextTime = 0;

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

  // if (nextTime == 0 || now >= nextTime) {
  //   nextMode += 5;
  //   nextMode %= 3;
  //   directedChannels[0].setState(nextMode);
  //   nextTime = now + 10000;
  // }
}






