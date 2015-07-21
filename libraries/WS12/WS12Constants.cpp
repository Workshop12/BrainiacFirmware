#include <Arduino.h>
#include "WS12Constants.h"

namespace WS12 {

const char _command_id[] PROGMEM = "id";
const PStr commandId(_command_id);

const char _command_ser[] PROGMEM = "ser";
const PStr commandSer(_command_ser);

const char _string_prompt[] PROGMEM = "$ ";
const PStr stringPrompt(_string_prompt);

const char _error_serUsage[] PROGMEM = "Usage: ser <serialNumber> - where serialNumber is 16 hex chars";
const PStr errorSerUsage(_error_serUsage);

const char _question_marks[] PROGMEM = "???";
const PStr questionMarks(_question_marks);

}