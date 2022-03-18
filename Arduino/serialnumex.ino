#include <EEPROM.h>

int address = 0;
byte value;

char *letters = "0123456789";
String serialnum = "";

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}



String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\ $'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
  return String(data);
}

void randSerialNum() {
  for (int i = 0; i < 9; i++)
  {
    serialnum = serialnum + letters[random(0, 9)];
  }
}
void setSerialNum() {
  String retrievedString = readStringFromEEPROM(0);
  if (retrievedString[retrievedString.length() - 1 == "$"]) {
    for (byte i = 0; i < retrievedString.length()-1; i = i++)
    {
      serialnum  = serialnum + retrievedString[i];
    }    
  } else {
    randSerialNum();
    writeStringToEEPROM(0, serialnum);
  }
  Serial.println(serialnum);
}
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  setSerialNum();
}


void loop() {
  delay(500);
}