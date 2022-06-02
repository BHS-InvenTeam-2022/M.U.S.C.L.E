#define TCAADDR 0x70

//SD Card
#include <SD.h>
#include <SPI.h>
const byte chipSelect = 10;

//LoRa module
unsigned long lastTransmission;
const int interval = 1000;

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <EEPROM.h>
Adafruit_MPU6050 mpu1 = Adafruit_MPU6050();
Adafruit_MPU6050 mpu2 = Adafruit_MPU6050();

float deltax1 = 0;
float deltax2 = 0;
float angleopen = 0.0;
float previous1 = NULL;
float previous2 = NULL;
float t_delay = 1000;
float t_step = t_delay / 1000; //frequency of reading
float newval;

int address = 2;
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

float getintegral(float x, float xx, float p_step) {
  return .5 * (x + xx) * p_step;
}

/*
void displaySensorDetails(Adafruit_MPU6050 *mpu)
{
  sensor_t sensor;
  mpu->getGyroSensor();
  Serial.println("------------------------------------");
  Serial.print ("Sensor: "); Serial.println(sensor.name);
  Serial.print ("Driver Ver: "); Serial.println(sensor.version);
  Serial.print ("Unique ID: "); Serial.println(sensor.sensor_id);
  Serial.print ("Max Value: "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print ("Min Value: "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print ("Resolution: "); Serial.print(sensor.resolution); Serial.println(" uT");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}
*/

void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

/*void scanPorts() {
  Serial.println("\nTCAScanner ready!");
  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    Serial.print("TCA Port #"); Serial.println(t);

    for (uint8_t addr = 0; addr <= 127; addr++) {
      if (addr == TCAADDR) continue;

      Wire.beginTransmission(addr);
      if (!Wire.endTransmission()) {
        Serial.print("Found I2C 0x");  Serial.println(addr, HEX);
      }
    }
  }
  Serial.println("\ndone");
}*/


void setup(void)
{
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Wire.begin();
  Serial.begin(9600);

  randomSeed(analogRead(0));
  setSerialNum();
  //scanPorts();

  /* Initialise the 1st sensor */
  tcaselect(0);
  if (!mpu1.begin()) {
    Serial.println(F("Failed to find 1st MPU6050 chip 1"));
    while (1) {
      delay(10);
    }
  }
  mpu1.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu1.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu1.setFilterBandwidth(MPU6050_BAND_21_HZ);

  /* Initialise the 2 sensor */
  tcaselect(1);
  if (!mpu2.begin()) {
    Serial.println(F("Failed to find 2nd MPU6050 chip 2"));
    while (1) {
      delay(10);
    }
  }
  mpu2.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu2.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu2.setFilterBandwidth(MPU6050_BAND_21_HZ);


  /* Display some basic information on this sensor */
  /*
     tcaselect(0);
    displaySensorDetails(&mpu1);
    tcaselect(1);
    displaySensorDetails(&mpu2);
  */


  Serial.print(F("Initializing SD card..."));
  pinMode(chipSelect, OUTPUT);
  digitalWrite(chipSelect, HIGH);
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  //Serial.println(SD.exists("datalog.txt"));
  if (SD.exists("datalog.txt")) {
    Serial.println("file exists\n");
  } else {
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println("success");
      dataFile.close();
      Serial.println("successfully printed");
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
  }

  Serial.println(F("SD Card initialization done.\n"));
  
}

int counter = 0;
int retries = 20;//number of seconds to retry for basically timeout connection
String mybuffer;
bool connected = false;
void sendpackets() {
  File dataFile = SD.open("datalog.txt", FILE_READ);
  int totalBytes = dataFile.size();
  String data = "size:" + String(totalBytes);
  delay(100);
  Serial.println("AT+SEND=" + String(address) + "," + data.length() + "," + data);
  delay(500);

  int temp_counter = 0;
  while (temp_counter <= retries) {
    temp_counter++;
    delay(300);
    if (Serial.available()) {
      String readString = Serial.readString();
      int delimiter, delimiter_1, delimiter_2;
      delimiter = readString.indexOf(",");
      delimiter_1 = readString.indexOf(",", delimiter + 1);
      delimiter_2 = readString.indexOf(",", delimiter_1 + 1);
      String message = readString.substring(delimiter_1 + 1, delimiter_2);
      //Serial.println(message);
      if (message == "OK") {
        connected = true;
        break;
      }
    }
    if (millis() > lastTransmission + 4000) {
      Serial.println("AT+SEND=" + String(address) + "," + data.length() + "," + data);
      lastTransmission = millis();
    }

  }

  if (temp_counter >= retries) {
    counter = 0;
    connected = false;
    dataFile.close();
    return;
  }


  delay(250);

  dataFile = SD.open("datalog.txt");
  if (!dataFile) {
    Serial.print("The text file cannot be opened");
    while (1);
  }
  while (dataFile.available() && connected) {
    String mybuffer = "";
    mybuffer = dataFile.readStringUntil('\n');
    counter++;
    if (millis() > lastTransmission + interval) {
      String data = mybuffer + "?" + String(counter);
      delay(100);
      Serial.println("AT+SEND=" + String(address) + "," + data.length() + "," + data);
      lastTransmission = millis();
    }
    delay(2200);

    

  }

  dataFile.close();
}


void loop(void)
{
  
  /* Get a new sensor event */
  sensors_event_t a, g, temp;

  tcaselect(0);
  mpu1.getEvent(&a, &g, &temp);
  if (previous1 == NULL) {
    previous1 = g.gyro.y;
  }
  else {
    newval = getintegral(previous1, g.gyro.y, t_step);
    deltax1 += newval;
    //bottom code if for formatting for json through pass through serial
    /*
    char t_buffer[10]; // Enough room for the digits you want and more to be safe
    dtostrf(deltax1, 5, 2, t_buffer);
    char buffer[40];
    sprintf(buffer, "{\"sensor-1\": %s}", t_buffer); //fix later as string library can cause memory fragmentation
    //Serial.println(buffer);
    */
    previous1 = g.gyro.y;
  }

  tcaselect(1);
  mpu2.getEvent(&a, &g, &temp);
  if (previous2 == NULL) {
    previous2 = g.gyro.y;
  }
  else {
    newval = getintegral(previous2, g.gyro.y, t_step);
    deltax2 += newval;
    //bottom code is for formatting angles for json to pass through serial
    /*
    char t_buffer[10]; // Enough room for the digits you want and more to be safe
    dtostrf(deltax2, 5, 2, t_buffer);
    char buffer[40];
    sprintf(buffer, "{\"sensor-2\": %s}", t_buffer); //fix later as string library can cause memory fragmentation
    //Serial.println(buffer);
    */
    previous2 = g.gyro.y;
  }


  angleopen = deltax2 - deltax1;
  Serial.println(angleopen);

  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(angleopen);
    dataFile.print(";");
    dataFile.print(serialnum);
    dataFile.close();
  }
  else {
    Serial.println("error opening datalog.txt");
  }

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

  delay(t_delay);

}