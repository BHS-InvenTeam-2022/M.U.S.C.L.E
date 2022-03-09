#define TCAADDR 0x70

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
Adafruit_MPU6050 mpu1 = Adafruit_MPU6050();
Adafruit_MPU6050 mpu2 = Adafruit_MPU6050();

float deltax1 = 0;
float deltax2 = 0;
float previous1 = NULL;
float previous2 = NULL;
float t_delay = 250;
float t_step = t_delay / 1000;
float newval;

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
}

void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t a, g, temp;
  tcaselect(0);
  mpu1.getEvent(&a, &g, &temp);
  /*Serial.print("Rotation X: ");
    Serial.print(g.gyro.x);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z);
    Serial.println(" rad/s");*/

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


    //Serial.println("Delta Y1: ");
    //Serial.println(deltax1);
    //Serial.print("Rotation Y: ");
    //Serial.println(g.gyro.y);
    previous1 = g.gyro.y;
  }


  tcaselect(1);
  mpu2.getEvent(&a, &g, &temp);
  /*Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");*/

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
    
    //Serial.print("Delta Y2: ");
    //Serial.println(deltax2);
    //Serial.print("Rotation Y: ");
    //Serial.println(g.gyro.y);
    previous2 = g.gyro.y;
  }


  delay(t_delay);
}