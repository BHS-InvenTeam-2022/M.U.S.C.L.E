/*
  reads input fom accelerometers and logs to sd-card
*/
#include <Adafruit_MPU6050.h>
//#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#define TCAADDR 0x70

Adafruit_MPU6050 mpu1;

const byte chipSelect = 10;

void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void printValues(Adafruit_MPU6050 mpu, int bus) {
  tcaselect(bus);
  Serial.print("Sensor number on bus:");
  Serial.println(bus);

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  /*Serial.print("c");
    Serial.print(a.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(a.acceleration.z);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(g.gyro.x);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z);
    Serial.println(" rad/s");

    Serial.println();*/

  writeToCard(a,g,temp,bus);
}

void writeToCard(sensors_event_t a, sensors_event_t g, sensors_event_t t, int bus) {
  String dataString = "";
  dataString += String(bus);
  dataString += "," + String(a.acceleration.x) + "," + String(a.acceleration.y) + "," + String(a.acceleration.z);
  dataString += "," + String(g.gyro.x) + "," + String(g.gyro.y) + "," + String(g.gyro.z);


  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

}

void setup() {
  Serial.begin(9600);
  while (!Serial)
    delay(10);
  Wire.begin();

  tcaselect(3);
  Serial.println("Adafruit MPU6050");
  if (!mpu1.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu1.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu1.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu1.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT); // change this to 53 on a mega  // don't follow this!!
  digitalWrite(10, HIGH);
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("SD Card initialization done.\n");
  if (!SD.exists("datalog.txt")) {
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println("Bus,AX,AY,AZ,GX,GY,GZ");
      dataFile.close();
    }else {
      Serial.println("error opening datalog.txt");
    }
  }

  delay(1000);
}

void loop() {
  printValues(mpu1, 2);
  delay(5000);
}
