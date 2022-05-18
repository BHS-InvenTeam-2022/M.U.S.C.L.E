//LoRa module
unsigned long lastTransmission;
const int interval = 1000;
const byte address = 2;

//pH
const int analogInPin = A2;
int sensorValue = 0;
unsigned long int avgValue;
float b;
int buf[10], temp;
float phValue;

// Temperature
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float Celcius = 0;
float Fahrenheit = 0;

//Clock Module
//https://create.arduino.cc/projecthub/MisterBotBreak/how-to-use-a-real-time-clock-module-ds3231-bc90fe
//above for clock library in case it doesn't work
#include <Wire.h>
#include <ds3231.h>
struct ts t;

//Salinity Module
int potPin = 0;
float ppt = 0;
float refvoltage = 1.1;
unsigned int ADCValue;
double Vcc;

//SD Card
#include <SD.h>
#include <SPI.h>
const byte chipSelect = 4;

#include <EEPROM.h>
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
  if (retrievedString.length() > 1) {
    for (int i = 0; i < retrievedString.length(); i++)
    {
      serialnum = serialnum + retrievedString[i];
    }
  } else {
    randSerialNum();
    writeStringToEEPROM(0, serialnum);
  }
}

/*
  void clear() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  }
*/

void setup()
{
  Serial.begin(9600);
  sensors.begin();
  //Clock Module
  Wire.begin();



  //set serial number
  randomSeed(analogRead(0));
  setSerialNum();


  // DS3231_init(DS3231_CONTROL_INTCN);
  DS3231_init(0);
  /*----------------------------------------------------------------------------
    In order to synchronise your clock module, insert timetable values below !
    ----------------------------------------------------------------------------*/
  t.hour = 8;
  t.min = 52;
  t.sec = 20;
  t.mday = 16;
  t.mon = 12;
  t.year = 2021;
  DS3231_set(t);


  //salinity setup
  //Serial.println("Sal sensor ");
  //analogReference(INTERNAL);


  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }

  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.close();
    Serial.println("successfully printed");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  Serial.println("SD Card initialization done.\n");
}


void phReading() {
  for (int i = 0; i < 10; i++)
  {
    buf[i] = analogRead(analogInPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++) {
    avgValue += buf[i];
  }
  float pHVol = (float)avgValue * 5.0 / 1024 / 6;
  phValue = -5.70 * pHVol + 21.34;
}

void tempReading() {
  sensors.requestTemperatures();
  Celcius = sensors.getTempCByIndex(0);
  Fahrenheit = sensors.toFahrenheit(Celcius);
}

void clockReading() {
  DS3231_get(&t);
}

//Calculate the ADC voltage
long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}


void salinityReading() {
  Vcc = readVcc() / 1000.0;
  int samples = 20;
  int aRead = 0;

  for (int i = 0; i < samples ; i++)
  {
    aRead += analogRead(potPin);
  }
  int aSamples = aRead / samples;
  float voltage = 5.0 * aSamples / 1023; // assuming 5V reference
  // question: would it be better to use the following?: voltage = Vcc * aSamples / 1024
  ppt = 16.3 * voltage;
}


void writeToFile() {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  //salinity wasnt added
  if (dataFile) {
    dataFile.print(Fahrenheit);
    dataFile.print(";");
    dataFile.print(phValue);
    dataFile.print(";");
    dataFile.print(ppt);
    dataFile.print(";");
    dataFile.print(t.year);
    dataFile.print("-");
    dataFile.print(t.mon);
    dataFile.print("-");
    dataFile.print(t.mday);
    dataFile.print("T");
    dataFile.print(t.hour);
    dataFile.print(":");
    dataFile.print(t.min);
    dataFile.print(":");
    dataFile.print(t.sec);
    dataFile.print(";");
    dataFile.println(serialnum);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}

int counter = 0;
int retries = 5;//number of seconds to retry for basically timeout connection
String mybuffer;
bool connected = false;
bool sent = false;
void sendpackets() {
  File dataFile = SD.open("datalog.txt", FILE_READ);
  int totalBytes = dataFile.size();
  String data = "size:" + String(totalBytes);
  Serial.println(String(sizeof(data)));
  delay(500);
  Serial.println("AT+SEND=" + String(address) + "," + sizeof(data) + "," + data);

  int temp_counter = 0;
  while (temp_counter <= retries) {
    temp_counter++;
    if (Serial.available()) {
      String readString = Serial.readString();
      int delimiter, delimiter_1, delimiter_2;
      delimiter = readString.indexOf(",");
      delimiter_1 = readString.indexOf(",", delimiter + 1);
      delimiter_2 = readString.indexOf(",", delimiter_1 + 1);
      String message = readString.substring(delimiter_1 + 1, delimiter_2);
      if (message == "OK") {
        connected = true;
        break;
      }
    }
    if (millis() > lastTransmission + 3000) {
      Serial.println("AT+SEND=" + String(address) + "," + sizeof(data) + "," + data);
      lastTransmission = millis();
    }
  }

  if (temp_counter >= retries) {
    counter = 0;
    connected = false;
    dataFile.close();
    return;
  }

  sent = false;
  delay(1000);

  dataFile = SD.open("datalog.txt");
  if (!dataFile) {
    Serial.print("The text file cannot be opened");
    while (1);
  }
  while (dataFile.available() && connected) {
    mybuffer = dataFile.readStringUntil('\n');
    if (millis() > lastTransmission + interval) {
      String data = mybuffer + "?" + String(counter);
      Serial.println("AT+SEND=" + String(address) + "," + sizeof(data) + "," + data);
      lastTransmission = millis();
      sent = true;
    }

    int t_count = 0;
    while (Serial.available() && t_count < retries) {
      String incomingString = Serial.readString();
      int delimiter, delimiter_1, delimiter_2;
      delimiter = incomingString.indexOf(",");
      delimiter_1 = incomingString.indexOf(",", delimiter + 1);
      delimiter_2 = incomingString.indexOf(",", delimiter_1 + 1);

      String message = incomingString.substring(delimiter_1 + 1, delimiter_2);
      if (message == "OK") { //continue sending packets
        delay(400);
        break;
      }
      t_count++;
      delay(1000);
    }
    if (t_count >= retries) {
      connected = false;
    }

  }

  dataFile.close();
}

void loop()
{

  //pH
  phReading();
  //Serial.print("ph sensor = " + String(phValue));

  //Temperature
  tempReading();
  //Serial.print(" F " + String(Fahrenheit));

  //Salinity
  //salinityReading();
  //Serial.println(" salinity " + String(ppt));

  //Clock Module
  clockReading();
  //Serial.print("Date : " + String(t.mday) + "/" + String(t.mon) + "/" + String(t.year) + "\t Hour : " + ":" + String(t.hour) + ":" + String(t.min) + "." + String(t.sec));

  writeToFile();

  if (Serial.available()) {
    String incomingString = Serial.readString();
    int delimiter, delimiter_1, delimiter_2;
    delimiter = incomingString.indexOf(",");
    delimiter_1 = incomingString.indexOf(",", delimiter + 1);
    delimiter_2 = incomingString.indexOf(",", delimiter_1 + 1);

    String message = incomingString.substring(delimiter_1 + 1, delimiter_2);

    if (message == "ALL") {
      sendpackets();
    }
  }

  delay(1000);
}