#include <SPI.h>
#include <Pixy.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 3

SoftwareSerial sonarSerial(rxPin, txPin, true);

unsigned long previousMillis = 0;

Pixy pixy;

int visionTargetCoord = -1;
int rangedDistance = 0;

void setup() {
  visionTargetCoord = -1;
  rangedDistance = 0;
  
  Wire.begin(4);
  Wire.onRequest(requestEvent);
  
  pixy.init();
  
  sonarSerial.begin(9600);
  
  Serial.begin(115200);
  Serial.println("Starting...");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 10) {
    previousMillis = currentMillis;
    
    if (pixy.getBlocks(2)) {
      visionTargetCoord = (pixy.blocks[0].x + pixy.blocks[1].x) / 2;
    }
    
    boolean msgComplete = false;
    int index = 0;
    char buff[5];
    sonarSerial.flush();
    while (!msgComplete) {
      if (sonarSerial.available()) {
        char headerByte = sonarSerial.read();
        if (headerByte == 'R') {
          while (index < 4) {
            if (sonarSerial.available()) {
              buff[index] = sonarSerial.read();
              index++;
            }
          }
          buff[index] = 0x00;
        }
        headerByte = 0;
        index = 0;
        msgComplete = true;
        rangedDistance = atoi(buff);
      }
    }
    
  }
}

void requestEvent() {
  byte buff[8];
  
  buff[0] = visionTargetCoord >> 24;
  buff[1] = visionTargetCoord >> 16;
  buff[2] = visionTargetCoord >> 8;
  buff[3] = visionTargetCoord;
  
  buff[4] = rangedDistance >> 24;
  buff[5] = rangedDistance >> 16;
  buff[6] = rangedDistance >> 8;
  buff[7] = rangedDistance;
  
  Wire.write(buff, 8);
}


