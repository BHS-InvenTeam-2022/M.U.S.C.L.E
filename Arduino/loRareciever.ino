unsigned long lastTransmission;
const int interval = 3000;

bool receiving = false;
int counter = 0;
int previous = -1;
int totalsize = 0;
int currentsize = 0;
int address = 1;
int retries = 15;

//SD Card
#include <SD.h>
#include <SPI.h>
const byte chipSelect = 4;

#define LED1 3 //red

void setup() {
  Serial.begin(9600);
  pinMode(LED1, LOW);

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

void loop() {
  if (!receiving) {    

    String data = "ALL";
    if (millis() > lastTransmission + interval) {
      Serial.println("AT+SEND=" + String(address) + ",3," + data);
      lastTransmission = millis();
    }
    delay(500);
    if (Serial.available()) {
      String incomingString = Serial.readString();
      if (incomingString == "") {
        return;
      }
      int delimiter, delimiter_1, delimiter_2;
      delimiter = incomingString.indexOf(",");
      delimiter_1 = incomingString.indexOf(",", delimiter + 1);
      delimiter_2 = incomingString.indexOf(",", delimiter_1 + 1);

      String message = incomingString.substring(delimiter_1 + 1, delimiter_2);
      //Serial.println(message);


      if (message.substring(0, 4) == "size") {
        receiving = true;
        delay(500);
        Serial.println("AT+SEND=" + String(address) + ",2,OK");
        delay(1500);
      }

    }

    

    digitalWrite(LED1, HIGH);
    delay(250);
    digitalWrite(LED1, LOW);
    delay(250);
    digitalWrite(LED1, HIGH);
    delay(250);
    digitalWrite(LED1, LOW);
    delay(250);

  }
  else {
    if (Serial.available()) {
      String readString = Serial.readString();

      int delimiter, delimiter_1, delimiter_2;
      delimiter = readString.indexOf(",");
      delimiter_1 = readString.indexOf(",", delimiter + 1);
      delimiter_2 = readString.indexOf(",", delimiter_1 + 1);

      String message = readString.substring(delimiter_1 + 1, delimiter_2);

      if (message.substring(0, 1) == "+" || message == "") {
        delay(100);
        return;
      }
      if(message.substring(0, 4) == "size"){
        receiving = false;
      }

      int c_delimiter = message.indexOf("?");
      int counter_sent = message.substring(c_delimiter + 1).toInt();

      Serial.println(message);

      if (counter_sent > previous) {
        previous = counter_sent;
      } else {
        counter++;
        if (counter >= retries) {
          receiving = false;
          currentsize = 0;
          totalsize = 0;
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
          digitalWrite(LED1, HIGH);
          delay(100);
          digitalWrite(LED1, LOW);
          delay(100);
          digitalWrite(LED1, HIGH);
          delay(100);
          digitalWrite(LED1, LOW);
          delay(100);
        }
        delay(200);
        return;
      }


      File dataFile = SD.open("datalog.txt", FILE_WRITE);
      if (dataFile) {
        dataFile.println(message);
        dataFile.close();
      }
      else {
        Serial.println("error opening datalog.txt");
      }

      digitalWrite(LED1, HIGH);
      delay(125);
      digitalWrite(LED1, LOW);
      delay(125);
      currentsize = currentsize + sizeof(message);
      counter = 0;
    }

    if (currentsize == totalsize) {
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

      receiving = false;
      currentsize = 0;
      totalsize = 0;
    }

  }

  delay(1500);
}