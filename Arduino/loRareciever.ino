unsigned long lastTransmission;
const int interval = 1000;
 
bool receiving = false;
int counter = 0;
int previous = -1;
int totalsize = 0;
int currentsize = 0;
 
String incomingString; // added LED blinking till it recieves lora recieve signal, and SD Card button to begin logging --> have to test
 
//SD Card
#include <SD.h>
#include <SPI.h>
const byte chipSelect = 10;
 
#define LED1 3 //red
 
void setup() {
  Serial.begin(9600);
  delay(10000);
  pinMode(LED1, LOW);
 
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
    Serial.println("SD Card initialization done.\n");
    Serial.println(SD.exists("datalog.txt"));
    if (!SD.exists("datalog.txt")) {
      File dataFile = SD.open("datalog.txt", FILE_WRITE);
      // if the file is available, write to it:
      if (dataFile)
      {
        dataFile.print(incomingString.substring(18, 51));
 
        dataFile.close();
        Serial.println("successfully printed");
      }
      // if the file isn't open, pop up an error:
      else {
        Serial.println("error opening datalog.txt");
      }
    }
    else {
      Serial.println("file exists\n");
    }
 
  }
 

}


void loop() { 
  if (!recieving) {
    String data = "ALL";
    if (millis() > lastTransmission + interval) {
      Serial.println("AT+SEND=2,35," + data);
      lastTransmission = millis();
    }
 
    if (Serial.available()) {
      incomingString = Serial.readString();
      if (incomingString.substring(13, 18) == "size:") {
        recieving = true;
        String temp = incomingString.substring(19);
        totalsize = temp.toInt();
        currentsize = 0;
      } else {
        digitalWrite(LED1, HIGH);
        delay(250);
        digitalWrite(LED1, LOW);
        delay(250);
        digitalWrite(LED1, HIGH);
        delay(250);
        digitalWrite(LED1, LOW);
        delay(250);
        continue;
      }
    }
 
  }
  else {
    if (Serial.available()) {
      incomingString = Serial.readString();
      String temp = incomingString.substring(incomingString.indexOf("?"));
      counter = temp.toInt();
      if(counter - previous == 1){
        String t_load = incomingString.substring(13, incomingString.indexOf("?"));
        currentsize = currentsize + sizeof(t_load);
        File dataFile = SD.open("datalog.txt", FILE_WRITE);      
        if (dataFile) {
          dataFile.print(incomingString.substring(13));
          dataFile.close();
        }
        else {
          Serial.println("error opening datalog.txt");
        }
        Serial.println("AT+SEND=2,35," + "OK");
        previous = counter;
      }
    }
  }
 
  if(currentsize == totalsize){
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
  } 
 
 
  delay(100);
 
}