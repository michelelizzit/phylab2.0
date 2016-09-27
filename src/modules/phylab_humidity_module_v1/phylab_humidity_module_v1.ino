/*
 *  Arduino code for a PhyLab 2.0 module connected with a DHT22 humidity sensor
 *  PhyLab 2.0 - https://lizzit.it/phylab
 *  Author: Michele Lizzit - lizzit.it
 *
 *  Updates:  27/9/2016
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

#include <DHT.h>
#include <DHT_U.h>
#include <SoftwareSerial.h>
#include "Config.h"

#define SERIAL_SPEED 57600

//temperature on channel 9
//humidity on channel 6

DHT_Unified dht(DHT22_PIN, DHTTYPE);

SoftwareSerial softwareSerial(SER_RX_PIN, SER_TX_PIN); // RX, TX

bool nrfPresent;

void setup() {
  pinMode(VCC_PIN, OUTPUT);
  pinMode(GND_PIN, OUTPUT);
  digitalWrite(VCC_PIN, HIGH);
  digitalWrite(GND_PIN, LOW);
  
  centralUnitSerial.begin(SERIAL_SPEED);
  
  dht.begin();
  
  initializeNRF24L01();

  //attende 2s prima di inviare istruzioni al sensore, vedi datasheet
  //waits 2 seconds before sending commands to the sensor, see datasheet
  delay(2000);

  nrfPresent = checkPresence();
}

void loop() {
  sensors_event_t event;
  
  #ifdef ENABLE_HUM
  //SEND HUMIDITY VALUE
  {
  dht.humidity().getEvent(&event);
  int tmp = event.relative_humidity * (double)100;
  String tmp2 = String(tmp); // <= 5 chars
  
  if (tmp2.length() < 2) tmp2 = "0000" + tmp2;
  else if (tmp2.length() < 3) tmp2 = "000" + tmp2;
  else if (tmp2.length() < 4) tmp2 = "00" + tmp2;
  else if (tmp2.length() < 5) tmp2 = "0" + tmp2;
  if (tmp != 31172) {
    byte a[] = {'a', 'b', 'a', '6', 'x', 'x', 'x', 'x', 'x', '!'};
    
    a[4] = tmp2.charAt(0);
    a[5] = tmp2.charAt(1);
    a[6] = tmp2.charAt(2);
    a[7] = tmp2.charAt(3);
    a[8] = tmp2.charAt(4);
    sendDataToCentralUnit(a, 10);
  }
  else {
    byte a[] = {'a', 'b', 'a', '6', 'E', 'E', 'E', 'E', 'E', '!'};
    sendDataToCentralUnit(a, 10);
  }

  }
  #endif

  #ifdef ENABLE_TEMP
  //SEND TEMPERATURE VALUE

  {
  dht.temperature().getEvent(&event);
  int tmp = event.temperature * (double)100;
  String tmp2 = String(tmp); // <= 5 chars
  
  if (tmp2.length() < 2) tmp2 = "0000" + tmp2;
  else if (tmp2.length() < 3) tmp2 = "000" + tmp2;
  else if (tmp2.length() < 4) tmp2 = "00" + tmp2;
  else if (tmp2.length() < 5) tmp2 = "0" + tmp2;
  if (tmp != 31172) {
    byte a[] = {'a', 'b', 'a', '9', 'x', 'x', 'x', 'x', 'x', '!'};
    
    a[4] = tmp2.charAt(0);
    a[5] = tmp2.charAt(1);
    a[6] = tmp2.charAt(2);
    a[7] = tmp2.charAt(3);
    a[8] = tmp2.charAt(4);
    sendDataToCentralUnit(a, 10);
  }
  else {
    byte a[] = {'a', 'b', 'a', '9', 'E', 'E', 'E', 'E', 'E', '!'};
    sendDataToCentralUnit(a, 10);
  }

  }
  #endif
  delay(50);
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
