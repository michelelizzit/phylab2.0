/*
 *  Arduino code for the PhyLab 2.0 acceleration experiment
 *  PhyLab 2.0 - https://lizzit.it/phylab
 *  Author: Michele Lizzit - lizzit.it
 *
 *  Updates:  27/9/2016
 *  Version: 1.6
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

boolean negAcc;
unsigned long tempPacchetto;
int numPacket;

void setup() {
  tempPacchetto = 0;
  numPacket = 0;
  Serial.begin(115200); //Serial is used only for debugging
  Serial.print(",");
  initializeNRF24L01();
}

void loop() {  
  //Serial.print(".");
  //int accData = readAcc(35);
  int accData = readAcc(30); //prima era 200
  
  negAcc = false;
  if (accData < 0) negAcc = true;
  
  if (negAcc) accData = abs(accData);
  
  String accDataY = String(accData); // < 5 chars
  
  if (accDataY.length() < 2) accDataY = "000" + accDataY;
  else if (accDataY.length() < 3) accDataY = "00" + accDataY;
  else if (accDataY.length() < 4) accDataY = "0" + accDataY;
  //Serial.println(accDataY);
  if (accDataY != "ERR") {
    byte a[] = {'a', 'b', 'a', '2', 'x', 'x', 'x', 'x', 'x', '!'};
    
    //a[4] = '0';
    //if (negAcc) a[4] = '-';

    a[4] = accDataY.charAt(0);
    a[5] = accDataY.charAt(1);
    a[6] = accDataY.charAt(2);
    a[7] = accDataY.charAt(3);
    a[8] = accDataY.charAt(4);
    
    sendStringNRF(a, 10);
  }
  else {
    byte a[] = {'a', 'b', 'a', '2', 'E', 'E', 'E', 'E', 'E', '!'};
    sendStringNRF(a, 10);
  }
  /*numPacket++;
  if (numPacket >= 1000) {
    initializeNRF24L01();
    numPacket = 0;
    Serial.println(readAcc(5));
    Serial.print("Average speed x100: ");
    Serial.println(millis() - tempPacchetto);
    tempPacchetto = millis();
  }*/
}
