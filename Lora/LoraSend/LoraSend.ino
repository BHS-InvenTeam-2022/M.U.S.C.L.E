/* 
  Basic test program, send date at the BAND you seted.
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/
#include "heltec.h"
#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6
#define SEND_BYTE_COUNT 255U


struct __attribute__((packed)) DataLog{
  uint8_t battery;
  uint8_t ph;
  uint16_t temp;
  uint16_t salinity;
  uint32_t timestamp;
};

DataLog dataLog[] = {
  {109, 7, 1, 95, 1640742167},  
  {104, 6, 2, 95, 1640742168},
  {106, 7, 1, 95, 1640742169},  
  {102, 6, 2, 95, 1640742170},
  {99, 7, 1, 95, 1640742171},  
  {94, 7, 1, 95, 1640742172},
  {92, 7, 1, 95, 1640742173},  
  {96, 6, 2, 95, 1640742174},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
    {109, 7, 1, 95, 1640742167},  
  {104, 6, 2, 95, 1640742168},
  {106, 7, 1, 95, 1640742169},  
  {102, 6, 2, 95, 1640742170},
  {99, 7, 1, 95, 1640742171},  
  {94, 7, 1, 95, 1640742172},
  {92, 7, 1, 95, 1640742173},  
  {96, 6, 2, 95, 1640742174},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
  {90, 7, 1, 95, 1640742175},  
  {100, 7, 1, 95, 1640742176},
  {102, 7, 1, 95, 1640742177},  
  {100, 7, 1, 95, 1640742178},  
  {100, 7, 1, 95, 1640742178},
};

void setup() {
  
  //WIFI Kit series V1 not support Vext control
  Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  LoRa.setSignalBandwidth(500E3);
  LoRa.setSpreadingFactor(7);
  LoRa.enableCrc();
  LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);

  uint16_t totalBytes = sizeof(dataLog);
  uint16_t bytesSent = 0;
  uint16_t bytesLeft = 0;

  LoRa.beginPacket();
  LoRa.write((uint8_t *)(&totalBytes), sizeof(totalBytes));
  LoRa.endPacket();
  delay(200);
  while(bytesSent != totalBytes){
    bytesLeft = totalBytes - bytesSent;
    LoRa.beginPacket();
    LoRa.write(&(((uint8_t *)(&dataLog))[bytesSent]), min((uint16_t)SEND_BYTE_COUNT, bytesLeft));
    LoRa.endPacket();
    bytesSent += min((uint16_t)SEND_BYTE_COUNT, bytesLeft);
    Serial.print(bytesSent);
    Serial.print("/");
    Serial.println(totalBytes);
    for(int i = 0; i < 255; i++){
      int packetSize = LoRa.parsePacket();
        if(packetSize != 0) {
          break;
        }
        delay(10);
    }

  }

  digitalWrite(25, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(2000);                       // wait for a second
  digitalWrite(25, LOW);    // turn the LED off by making the voltage LOW
}

void loop() {
    if(Serial.available()){
      Serial.readString();
      esp_sleep_enable_timer_wakeup(1000000ULL);
      esp_deep_sleep_start();
  }
}
