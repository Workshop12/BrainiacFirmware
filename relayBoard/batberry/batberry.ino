#include <Serial>
#include <SoftwareSerial.h> 

#define RxD 2
#define TxD 3

const uint8_t DEVICES = 4;

int8_t ledPin = 13;

/**
 * Commands
 * 
 * m - move
 * ========
 * Move a device in the given direction.
 *
 *
 * m<device><direction>\n  eg: mA0\n
 *  device    - single character for devices, a~z
 *  direction - 0 - stop NOW
 *              1 - move with direction relays off
 *              2 - move with direction relays on
 *
 * s - set
 * =======
 * 
 * st - set time
 * -------------
 * Set the time limit for a movement in a given direction.
 *
 * st<device><direction>=<time>\n
 *  device    - single character for devices, a~z
 *  direction - 1 - move with direction relays off
 *              2 - move with direction relays on
 *  time      - time in milliseconds to keep relays on
 *
 * k - kill
 * ========
 * Kill all relay outputs. All power relays will be disabled followed by direction relays.
 * 
 * k\n
 * 
 */
 
uint32_t moveTime[DEVICES][2] = {
  {10000,10000 },
  {10000,10000 },
  {10000,10000 },
  {10000,10000 }
};

int8_t   moving[DEVICES];
  // 0 - not moving
  // 1 - moving with direction relays off
  // 2 - moving with direction relays on
  
uint32_t stopTime[DEVICES];
  // time in millis that the device should stop moving.
  

uint8_t devicePins[DEVICES][2] = {
  { 4,5 },
  { 6,7 },
  { 8,9 },
  { 10,11 }
};

SoftwareSerial blueToothSerial(RxD,TxD);

void setup() {
  Serial.begin(9600);
  
  Serial.println("BatBerry 0.0");
  
  pinMode(ledPin,OUTPUT);
  
  pinMode(RxD,INPUT);
  pinMode(TxD,OUTPUT);
  
  for (int i=0; i<DEVICES; ++i) {
    pinMode(devicePins[i][0],OUTPUT);
    pinMode(devicePins[i][1],OUTPUT);
  }
  
  setupBluetoothConnection();
}

const uint8_t commandBufferLength = 30;
uint8_t commandBuffer[commandBufferLength];
uint8_t commandBufferOffset = 0;

void moveDevice(uint8_t device, uint8_t move) {
  if (device>=DEVICES) {
    return;
  }
  if (move>2) {
    return;
  }
  
  if (move==0) {
    // Turn power relays off
    digitalWrite(devicePins[device][0],0);
    delay(100);
    
    // Turn direction relays off
    digitalWrite(devicePins[device][1],0);
    
    moving[device] = 0;
  } else {
    // Turn power relays off
    digitalWrite(devicePins[device][0],0);
    delay(100);
    
    // Possibly turn direction relays on.
    digitalWrite(devicePins[device][1],move==2 ? 1 : 0);
    delay(100);
    
    // Turn power relays on
    digitalWrite(devicePins[device][0],1);
    moving[device] = move;
    stopTime[device] = millis() + moveTime[device][move-1];
  }
  
  delay(100);
}

void kill() {
  for (uint8_t i=0; i<DEVICES; ++i) {
    digitalWrite(devicePins[i][0],0);
    moving[i] = 0;
  }
  delay(100);
  for (uint8_t i=0; i<DEVICES; ++i) {
    digitalWrite(devicePins[i][1],0);
  }
  delay(100);
}

uint8_t getDevice(uint8_t device,uint8_t & valid) {
  valid = 0;
  if (device>='a' && device<='z') {
    device-='a';
  } else if (device>='A' && device<='Z') {
    device-='A';
  } else {
    Serial.println("Device must be from a letter.");
    return 0;
  }
  if (device>=DEVICES) {
    Serial.print("Device must be between A and ");
    Serial.write('A'+DEVICES-1);
    Serial.println();
    return 0;
  }
  
  valid = 1;
  return device;
}

void processMoveCommand() {
  if (commandBufferOffset!=3) {
    Serial.println("Usage: m<device><direction>");
  } else {
    uint8_t deviceValid;
    uint8_t device = getDevice(commandBuffer[1],deviceValid);
    if (!deviceValid) {
      return;
    }
    
    uint8_t move = commandBuffer[2];
    if (move<'0' || move>'2') {
      Serial.println("Direction must be between 0 and 2");
      return;
    }
    move -= '0';
    moveDevice(device,move);
  }
}

uint32_t readNumber(uint8_t * buffer, uint8_t count, uint8_t & ok) {
  ok = 0;
  uint32_t toReturn = 0;
  for (uint8_t i=0; i<count; ++i) {
    uint8_t next = buffer[i];
    if (next>='0' && next<='9') {
      toReturn *= 10;
      toReturn += (next-'0');
    } else {
      // bad!
      return 0;
    }
  }
  ok = 1;
  return toReturn;
}

void processSetCommand() {
  if (commandBufferOffset<2) {
    Serial.println("Sub set command missing");
    return;
  }
  switch(commandBuffer[1]) {
    case 't': {
      if (commandBufferOffset<6) {
        Serial.println("st command too short");
        return;
      }
      uint8_t deviceValid;
      uint8_t device = getDevice(commandBuffer[2],deviceValid);
      if (!deviceValid) {
        return;
      }
      
      uint8_t move = commandBuffer[3];
      if (move<'1' || move>'2') {
        Serial.println("Move must be between 1 and 2");
        return;
      }
      move -= '1'; // move will now be between 0 and 1
      
      if (commandBuffer[4]!='=') {
        Serial.println("Missing equals sign");
        return;
      }
      
      uint8_t numberGood;
      uint32_t time;
      time = readNumber(commandBuffer+5,commandBufferOffset - 5,numberGood);
      if (!numberGood) {
        Serial.println("Need valid (positive) integer.");
        return;
      }
      
      moveTime[device][move] = time;
      Serial.print("OK Setting time to: ");
      Serial.print(time);
      Serial.println();
    }
    break;
    
    default:
    Serial.println("Unknown sub set command");
  }
}

void processCommandLine() {
  if (commandBufferOffset==0) {
    return;
  }
  switch(commandBuffer[0]) {
    case 'm':
      processMoveCommand();
      break;
    case 's':
      processSetCommand();
      break;
    case 'k':
      kill();
      break;
    case '+':
      // ignore
      break;
    default:
      Serial.println("Unknown command");
  }
  commandBufferOffset = 0;
}

void checkForInput() {
  while(blueToothSerial.available()) {
    uint8_t nextChar = blueToothSerial.read();
    Serial.write(nextChar);
    if (nextChar=='\n' || nextChar=='\r') {
      processCommandLine();
    } else if (nextChar < ' ') {
      // ignore
    } else {
      commandBuffer[commandBufferOffset++] = nextChar;
      if (commandBufferOffset>= commandBufferLength) {
        commandBufferOffset = 0;
      }
    }
  }
}

void checkForTimeouts() {
  uint32_t now = millis();
  for (uint8_t i=0; i<DEVICES; ++i) {
    if (moving[i]!=0 && stopTime[i] < now) {
      moveDevice(i,0);
    }
  }
}

uint32_t lastDot = 0;

void loop() {
  // Check for input
  checkForInput();
  
  // Check for timeouts
  checkForTimeouts();
  
  uint32_t now = millis();
  if (now-lastDot>2000) {
    lastDot = now;
    Serial.write('.');
  }
}

void setupBluetoothConnection()
{
  blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
  blueToothSerial.print("\r\n+STNA=BatBerry\r\n"); //set the bluetooth name as "SeeedBTSlave"
  blueToothSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  blueToothSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
  delay(2000); // This delay is required.
  blueToothSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000); // This delay is required.
  blueToothSerial.flush();
}


