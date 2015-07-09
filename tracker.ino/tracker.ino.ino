#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout =0);

void setup() {
 while (!Serial);
 Serial.begin(115200);
 Serial.println("Initializing device");
 fonaSS.begin(4800);
 if(!fona.begin(fonaSS)){
  Serial.println(F("Couldn't find FONA"));
  while(1);
 }
Serial.println(F("Fona is OK"));\
turnGPSon();
//Print SIM card IMEI number
char imei[15] = {0};
uint8_t imeiLen = fona.getIMEI(imei);
  if(imeiLen>0){
    Serial.print("Sim IMEI:");
    Serial.println(imei);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
 readGPSloc();
 Serial.println("done");
 delay(5000); 
  
}

void PlayTone(){
      // play tone
      flushSerial();
      Serial.print(F("Play tone #"));
      uint8_t kittone = readnumber();
      Serial.println();
      // play for 1 second (1000 ms)
      if (! fona.playToolkitTone(kittone, 1000)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
}

void callPhone(){
  char number[30];
      flushSerial();
      Serial.print(F("Call #"));
      readline(number, 30);
      Serial.println();
      Serial.print(F("Calling ")); Serial.println(number);
      if (!fona.callPhone(number)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("Sent!"));
      }
}

void hangUp() {
       // hang up! 
      if (! fona.hangUp()) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
}   

void pickUp() {
    {
       // pick up! 
      if (! fona.pickUp()) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
    }
}

void turnGPSon(){
    if (!fona.enableGPS(true)) { Serial.println(F("Failed to turn on"));}
}

void turnGPSoff(){
   if (!fona.enableGPS(false)){Serial.println(F("Failed to turn off"));}
}

void readGPSloc(){
  char gpsdata[80];
       fona.getGPS(0, gpsdata, 80);
       Serial.println(F("Reply in format: mode,longitude,latitude,altitude,utctime(yyyymmddHHMMSS),ttff,satellites,speed,course"));
       Serial.println(gpsdata);

}
void flushSerial() {
    while (Serial.available()) 
    Serial.read();
}

char readBlocking() {
  while (!Serial.available());
  return Serial.read();
}
uint16_t readnumber() {
  uint16_t x = 0;
  char c;
  while (! isdigit(c = readBlocking())) {
    //Serial.print(c);
  }
  Serial.print(c);
  x = c - '0';
  while (isdigit(c = readBlocking())) {
    Serial.print(c);
    x *= 10;
    x += c - '0';
  }
  return x;
}
  
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout) {
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;
  
  while (true) {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

    while(Serial.available()) {
      char c =  Serial.read();

      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

      if (c == '\r') continue;
      if (c == 0xA) {
        if (buffidx == 0)   // the first 0x0A is ignored
          continue;
        
        timeout = 0;         // the second 0x0A is the end of the line
        timeoutvalid = true;
        break;
      }
      buff[buffidx] = c;
      buffidx++;
    }
    
    if (timeoutvalid && timeout == 0) {
      //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  buff[buffidx] = 0;  // null term
  return buffidx;
}
