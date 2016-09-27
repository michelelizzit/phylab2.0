/*
 *  Arduino code for the PhyLab 2.0 capacitor experiment
 *  PhyLab 2.0 - https://lizzit.it/phylab
 *  Author: Michele Lizzit - lizzit.it
 *
 *  Updates:  27/9/2016
 *            03/11/2015
 *  Version: 1.0
 *
 *
 *  Copyright (C) 2016  Michele Lizzit
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Config.h"

#include <SoftwareSerial.h>


#define SERIAL_SPEED 57600

SoftwareSerial softwareSerial(SER_RX_PIN, SER_TX_PIN); // RX, TX

int pos = 0;
int tempMisurazione;
int numMisurazione;

bool nrfPresent;

void setup() {
  tempMisurazione = 0;
  numMisurazione = 0;
  centralUnitSerial.begin(SERIAL_SPEED);
  initializeNRF24L01();
  nrfPresent = checkPresence();
}
void loop() {
  delay(100);
  
  String durationString = String(analogRead(MEASURE_PIN)); // < 5 chars
  
  if (durationString.length() < 2) durationString = "0000" + durationString;
  else if (durationString.length() < 3) durationString = "000" + durationString;
  else if (durationString.length() < 4) durationString = "00" + durationString;
  else if (durationString.length() < 5) durationString = "0" + durationString;
 
  byte a[] = {'a', 'b', 'a', '7', 'x', 'x', 'x', 'x', 'x', '!'};
    
  a[4] = durationString.charAt(4);
  a[5] = durationString.charAt(3);
  a[6] = durationString.charAt(2);
  a[7] = durationString.charAt(1);
  a[8] = durationString.charAt(0);
  //Serial.println("Ciao: " + durationString);
  sendDataToCentralUnit(a, 10);
  
  
  numMisurazione++;
  if (numMisurazione >= 100) {
    numMisurazione = 0;
    Serial.print("Average speed x100: ");
    Serial.println(millis() - tempMisurazione);
    tempMisurazione = millis();
    
    Serial.println(durationString);
  } 
}

void sendStringSerial(unsigned char packet[], short len) {
  for (short cnt = 0; cnt < len; cnt++)
    centralUnitSerial.write(packet[cnt]);
}

void sendDataToCentralUnit(unsigned char packet[], short len) {
  bool currentNrfPresent = checkPresence();
  sendStringSerial(packet, len);
  if (nrfPresent && currentNrfPresent)
    for (int cnt = 0; cnt < 3; cnt++) {
      sendStringNRF(packet, len);
      delay(150);
    }
  else if (nrfPresent && !(currentNrfPresent)) {
    nrfPresent = false;
    delay(400);
  }
  else if (!nrfPresent && currentNrfPresent) {
    initializeNRF24L01();
    nrfPresent = true;
    for (int cnt = 0; cnt < 3; cnt++) {
      sendStringNRF(packet, len);
      delay(150);
    }
  }
  else {
    delay(400);
  }
}
