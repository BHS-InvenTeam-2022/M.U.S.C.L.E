#define TCAADDR 0x70

//SD Card
#include <SD.h>
#include <SPI.h>
const byte chipSelect = 10;

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <EEPROM.h>
Adafruit_MPU6050 mpu1 = Adafruit_MPU6050();
Adafruit_MPU6050 mpu2 = Adafruit_MPU6050();

float deltax1 = 0;
float deltax2 = 0;
float previous1 = NULL;
float previous2 = NULL;
float t_delay = 250;
float t_step = t_delay / 1000;
float newval;

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

float getintegral(float x, float xx, float p_step) {
  return .5 * (x + xx) * p_step;
}

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

void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void scanPorts() {
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
}


void setup(void)
{
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Wire.begin();
  Serial.begin(9600);
  
  randomSeed(analogRead(0));
  setSerialNum();
  //scanPorts();

  //Serial.println("MPU6050 Test"); Serial.println("");

  /* Initialise the 1st sensor */
  tcaselect(0);
  if (!mpu1.begin()) {
    Serial.println(F("Failed to find 1st MPU6050 chip 1"));
    while (1) {
      delay(10);
    }
  }
  mpu1.setAccelerometerRange(MPU6050_RANGE_8_G);
  /*Serial.print("Accelerometer range set to: ");
    switch (mpu1.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
    }*/
  mpu1.setGyroRange(MPU6050_RANGE_500_DEG);
  /*Serial.print("Gyro range set to: ");
    switch (mpu1.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
    }*/
  mpu1.setFilterBandwidth(MPU6050_BAND_21_HZ);
  /*Serial.print("Filter bandwidth set to: ");
    switch (mpu1.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
    }*/

  /* Initialise the 2 sensor */
  tcaselect(1);
  if (!mpu2.begin()) {
    Serial.println(F("Failed to find 2nd MPU6050 chip 2"));
    while (1) {
      delay(10);
    }
  }
  mpu2.setAccelerometerRange(MPU6050_RANGE_8_G);
  /*Serial.print("Accelerometer range set to: ");
    switch (mpu1.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
    }*/
  mpu2.setGyroRange(MPU6050_RANGE_500_DEG);
  /*Serial.print("Gyro range set to: ");
    switch (mpu1.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
    }*/
  mpu2.setFilterBandwidth(MPU6050_BAND_21_HZ);
  /*Serial.print("Filter bandwidth set to: ");
    switch (mpu1.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
    }*/


  /* Display some basic information on this sensor */
  /*tcaselect(0);
    displaySensorDetails(&mpu1);
    tcaselect(1);
    displaySensorDetails(&mpu2);*/

  //Serial.println("Frequency of reading: " + String(t_step));

  Serial.print("Initializing SD card...");
    pinMode(10, OUTPUT); // change this to 53 on a mega // don't follow this!!
    digitalWrite(10, HIGH);
    if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);


    Serial.println("SD Card initialization done.\n");
    Serial.println(SD.exists("datalog.txt"));
    if (!SD.exists("datalog.txt")) {
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
    dataFile.print(Fahrenheit);
    dataFile.print(Celcius);
    dataFile.print(phValue);


    dataFile.close();
    Serial.println("successfully printed");
    }
    // if the file isn't open, pop up an error:
    else {
    Serial.println("error opening datalog.txt");
    }
    } else {
    Serial.println("file exists\n");
    }
    }
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
    char t_buffer[10]; // Enough room for the digits you want and more to be safe
    dtostrf(deltax1, 5, 2, t_buffer);
    char buffer[40];
    sprintf(buffer, "{\"sensor-1\": %s}", t_buffer); //fix later as string library can cause memory fragmentation
    Serial.println(buffer);    
    previous1 = g.gyro.y;
  }


  tcaselect(1);
  mpu2.getEvent(&a, &g, &temp); 

  if (previous2 == NULL) {
    previous2 = g.gyro.y;
  }
  else {
    newval = getintegral(previous2, g.gyro.y, t_step);
    previous2 = g.gyro.y;
    deltax2 += newval;
    char t_buffer[10]; // Enough room for the digits you want and more to be safe
    dtostrf(deltax2, 5, 2, t_buffer);
    char buffer[40];
    sprintf(buffer, "{\"sensor-2\": %s}", t_buffer); //fix later as string library can cause memory fragmentation
    Serial.println(buffer);
    previous2 = g.gyro.y;
  }

  float angleopen = deltax2 - deltax1;
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(angleopen);
    dataFile.close();
  }
  else {
    Serial.println("error opening datalog.txt");
  }
  delay(t_delay);
}