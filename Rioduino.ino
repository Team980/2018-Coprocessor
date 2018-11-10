/*
 *  MIT License
 *  
 *  Copyright (c) 2018 FRC Team 980 ThunderBots
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include <SPI.h>

#include <Pixy.h>
//#include "TFMini.h"

#include <Wire.h>
#include <SoftwareSerial.h>

Pixy pixy;

SoftwareSerial sonarSerial(4, 5, true);

//TFMini tfMini;

int visionTargetCoord;
int powerCubeWidth;
int powerCubeHeight;
int powerCubeCoord;
int sonarDistance;
//int lidarDistance;

void setup() {
  visionTargetCoord = -2;
  powerCubeWidth = 0;
  powerCubeHeight = 0;
  powerCubeCoord = -2;
  sonarDistance = 0;
  //lidarDistance = 0;

  Wire.begin(4);
  Wire.onRequest(requestEvent);

  pixy.init();

  sonarSerial.begin(9600);

  //Serial.begin(115200); //lidar
  //tfMini.begin(&Serial);

  //Serial.begin(115200);
  //Serial.println("Starting...");
}

void loop() {
  int numBlocks = pixy.getBlocks();
  //Serial.println((String) "numBlocks: " + numBlocks);
  if (numBlocks > 0) {
    int tmp_visionTargetWidth = 0;
    int tmp_powerCubeWidth = 0;
    for (int i = 0; i < numBlocks; i++) {
      //Serial.println((String) i + ": " + pixy.blocks[i].signature);
      if (pixy.blocks[i].signature == 1) { //Vision target
        if (pixy.blocks[i].width > tmp_visionTargetWidth) { //We want the biggest one
          tmp_visionTargetWidth = pixy.blocks[i].width;
          visionTargetCoord = pixy.blocks[i].x;
        }
      } else if (pixy.blocks[i].signature == 2 || pixy.blocks[i].signature == 3
                 || pixy.blocks[i].signature == 4)  { //Power cubes at different ranges, plus mustard
        if (pixy.blocks[i].width > tmp_powerCubeWidth) { //We want the biggest one
          //Serial.println((String) "Width: " + pixy.blocks[i].width);
          tmp_powerCubeWidth = pixy.blocks[i].width;
          powerCubeWidth = pixy.blocks[i].width;
          powerCubeHeight = pixy.blocks[i].height;
          powerCubeCoord = pixy.blocks[i].x;
        }
      }
    }
  }

  boolean msgComplete = false;
  int index = 0;
  char buff[5];
  sonarSerial.flush();
  while (!msgComplete) {
    if (sonarSerial.available()) {
      char headerByte = sonarSerial.read();
      if (headerByte == 'R') {
        while (index < 3) {
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
      sonarDistance = atoi(buff);
      //Serial.println((String) "sonarDistance: " + sonarDistance);
    }
  }

  //lidarDistance = tfMini.getDistance(); //it's so simple xD
  //Serial.println((String) "lidarDistance: " + lidarDistance);
}

void requestEvent() {
  byte buff[20];

  buff[0] = visionTargetCoord >> 24;
  buff[1] = visionTargetCoord >> 16;
  buff[2] = visionTargetCoord >> 8;
  buff[3] = visionTargetCoord;

  buff[4] = powerCubeWidth >> 24;
  buff[5] = powerCubeWidth >> 16;
  buff[6] = powerCubeWidth >> 8;
  buff[7] = powerCubeWidth;

  buff[8] = powerCubeHeight >> 24;
  buff[9] = powerCubeHeight >> 16;
  buff[10] = powerCubeHeight >> 8;
  buff[11] = powerCubeHeight;

  buff[12] = powerCubeCoord >> 24;
  buff[13] = powerCubeCoord >> 16;
  buff[14] = powerCubeCoord >> 8;
  buff[15] = powerCubeCoord;

  buff[16] = sonarDistance >> 24;
  buff[17] = sonarDistance >> 16;
  buff[18] = sonarDistance >> 8;
  buff[19] = sonarDistance;

  //buff[20] = lidarDistance >> 24;
  //buff[21] = lidarDistance >> 16;
  //buff[22] = lidarDistance >> 8;
  //buff[23] = lidarDistance;

  Wire.write(buff, 20);
}
