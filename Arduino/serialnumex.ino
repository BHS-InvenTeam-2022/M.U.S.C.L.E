#include <EEPROM.h>

int address = 0;
String letters = "0123456789";
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
  data[newStrLen] = '\0';
  return String(data);
}

void randSerialNum() {
  serialnum = "ss";
  for (int i = 0; i < 7; i++)
  {
    serialnum = serialnum + letters[random(0, 9)];
  }
}

void setSerialNum() {
  String retrievedString = readStringFromEEPROM(0);
  Serial.println("retrieved number is: " + retrievedString);
  Serial.println("length is: " + String(retrievedString.length()));

  if (retrievedString.length() > 1) {
    for (int i = 0; i < retrievedString.length(); i++)
    {
      serialnum  = serialnum + retrievedString[i];
    }
  } else {
    randSerialNum();
    writeStringToEEPROM(0, serialnum);
  }

}

void clear() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  //clear();
  randomSeed(analogRead(0));
  setSerialNum();
  Serial.println("serial number is: " + serialnum);
}

void loop() {
  delay(500);
}