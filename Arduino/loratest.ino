unsigned long lastTransmission;
const int interval = 3000;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  //Serial.println("AT+SEND=1,3,ALL");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    
    
    String incomingString = Serial.readString();
    if(incomingString == ""){
      return;
    }
    int delimiter, delimiter_1, delimiter_2;
    delimiter = incomingString.indexOf(",");
    delimiter_1 = incomingString.indexOf(",", delimiter + 1);
    delimiter_2 = incomingString.indexOf(",", delimiter_1 + 1);
    
    String message = incomingString.substring(delimiter_1 + 1, delimiter_2);
    Serial.println(message);
    if(message == "ALL"){
      delay(1000);
      Serial.println("AT+SEND=1,2,OK");
      Serial.println("done");
      delay(3000);
    }
    

  }
  delay(1000);
  /*if(millis() > lastTransmission + interval){
    Serial.println("AT+SEND=2,5,HELLO");
    lastTransmission = millis();
    }*/
}