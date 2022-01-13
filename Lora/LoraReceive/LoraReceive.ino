#include "heltec.h" 
#include "FS.h"
#include "SPIFFS.h"
#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6
#define FORMAT_SPIFFS_IF_FAILED true
#define SEND_BYTE_COUNT 255

struct __attribute__((packed)) DataLog{
  uint8_t battery;
  uint8_t ph;
  uint16_t temp;
  uint16_t salinity;
  uint32_t timestamp;
};

File file;
uint16_t totalFileSize = 0;
uint8_t buf[SEND_BYTE_COUNT];
uint16_t bytesWritten = 0;

void LoRaData(){
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawStringMaxWidth(64 , 10 , 128, String(bytesWritten, DEC)+"/"+String(totalFileSize, DEC));
  Heltec.display->drawProgressBar(4, 32, 120, 8, uint8_t((100.0*bytesWritten)/totalFileSize));
  Heltec.display->display();
}

void cbk(int packetSize) {
  LoRa.readBytes(buf, packetSize);
  if(packetSize == 2 && totalFileSize == 0){
    memcpy((void *)&totalFileSize, buf, 2);
    file = SPIFFS.open("/data.bin", FILE_WRITE);
    if(!file){
      Serial.println("- failed to open file for writing");
      return;
    }
  }
  else{
    bytesWritten += file.write(buf, packetSize);
    Serial.println(bytesWritten);
  }
  if(bytesWritten == totalFileSize){
    file.close();
  }
  LoRaData();
  LoRa.beginPacket();
  LoRa.write(0);
  LoRa.endPacket();
}

DataLog dataLog;

void setup() {    
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
 
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->clear();
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  Heltec.display->drawString(0, 0, "Wait for data...");
  Heltec.display->display();

  LoRa.setSignalBandwidth(500E3);
  LoRa.setSpreadingFactor(7); 
  LoRa.enableCrc();

}
void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) { cbk(packetSize);  }
}
