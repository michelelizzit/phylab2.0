/*
 *
 *	PhyLab 2.0 - https://lizzit.it/phylab
 *	Author: Michele Lizzit - lizzit.it
 *
 *	Updates: 	19/9/2016
 *				26/7/2016
 *				25/8/2015
 *  			15/4/2015
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

#include <stdio.h>
#include <stdint.h>
#include "bcm2835.h"
#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <thread>
#include <execinfo.h>

#include "nRF24L01.cpp"

using namespace std;

char radiationValue[] = { '0', '0', '0', '0', '0', '\0' };
char temperatureValue[] = { '0', '0', '0', '0', '0', '\0' };
char accelerationValue[] = { '0', '0', '0', '0', '0', '\0' };
char timeValue[] = { '0', '0', '0', '0', '0', '\0' };
char distanceValue[] = { '0', '0', '0', '0', '0', '\0' };
char humidityValue[] = { '0', '0', '0', '0', '0', '\0' };
char voltageValue[] = { '0', '0', '0', '0', '0', '\0' };
char lampVoltageValue[] = { '0', '0', '0', '0', '0', '\0' };
char lampCurrentValue[] = { '0', '0', '0', '0', '0', '\0' };
bool saveRadiation = true;
bool saveTemperature = true;
bool saveAcceleration = true;
bool saveTime = true;
bool saveDistance = true;
bool saveHumidity = true;
bool saveVoltage = true;
bool databaseAccessible = true;
const int numSensors = 7;
const int START_DATABASE_SIZE = 5;
FILE* dataFile[numSensors];
char dataFileName[numSensors][25];
uint64_t* databaseTime[numSensors];
int* databaseValues[numSensors];
int currentDatabaseSize[numSensors];
int currentDatabasePos[numSensors];

const int numUSB = 3;
int serialModule[numUSB];
bool serialActive[numUSB];
char serNames[numUSB][13];

unsigned char serialBuffer[numUSB][256];
int bufferPos[numUSB];

const bool DEBUG = true;

void printDebugLineStart() {
	if (DEBUG)
		printf("[phylab] ");
}

void printDebugLineEnd() {
	if (DEBUG) {
		printf("\n");
		fflush(stdout);
	}
}

void segFaultHandler(int sigNum) {
	printDebugLineStart();
	if (DEBUG)
		printf("Segmentation fault detected");
	printDebugLineEnd();

	void *buf[10];
	size_t size = backtrace(buf, 10);
	fprintf(stderr, "Error: signal %d:\n", sigNum);
	backtrace_symbols_fd(buf, size, STDERR_FILENO);

	exit(1);
}

void createSerialList() {
	strcpy(serNames[0], "/dev/ttyACM0\0");
	strcpy(serNames[1], "/dev/ttyAMA0\0");
	strcpy(serNames[2], "/dev/ttyUSB0\0");
}

void createFileNamesList() { //Gli array hanno lunghezza 25!!
	strcpy(dataFileName[0], "/mnt/plab_data/time.plab\0");
	strcpy(dataFileName[1], "/mnt/plab_data/temp.plab\0");
	strcpy(dataFileName[2], "/mnt/plab_data/humd.plab\0");
	strcpy(dataFileName[3], "/mnt/plab_data/dist.plab\0");
	strcpy(dataFileName[4], "/mnt/plab_data/accl.plab\0");
	strcpy(dataFileName[5], "/mnt/plab_data/volt.plab\0");
	strcpy(dataFileName[6], "/mnt/plab_data/radt.plab\0");
}

void createDatabase() {
	for (int cnt = 0; cnt < numSensors; cnt++) {
		databaseTime[cnt] = new uint64_t[START_DATABASE_SIZE];
		databaseValues[cnt] = new int[START_DATABASE_SIZE];
		currentDatabasePos[cnt] = 0;
		currentDatabaseSize[cnt] = START_DATABASE_SIZE;
	}
}

void expandDatabase(int numDatabase) {
	int newDatabaseSize = currentDatabaseSize[numDatabase] * 2;
	uint64_t* tmpT;
	int* tmpD;

	//alloca spazio per il nuovo database
	tmpT = new uint64_t[newDatabaseSize];
	tmpD = new int[newDatabaseSize];
	
	//copia il vecchio database nel nuovo
	for (int cnt = 0; cnt < currentDatabaseSize[numDatabase]; cnt++) {
		tmpT[cnt] = databaseTime[numDatabase][cnt];
		tmpD[cnt] = databaseValues[numDatabase][cnt];
	}
	
	//libera lo spazio dei vecchi array
	delete databaseTime[numDatabase];
	delete databaseValues[numDatabase];
	
	//copia i puntatori
	databaseTime[numDatabase] = tmpT;
	databaseValues[numDatabase] = tmpD;

	currentDatabaseSize[numDatabase] = newDatabaseSize;
}

void clearDatabase(int numDatabase) { //!!!!funzione eseguita mentre il thread scrive
	int newDatabaseSize = START_DATABASE_SIZE;
	uint64_t* tmpT;
	int* tmpD;
	uint64_t* tmpT2;
	int* tmpD2;

	//chiede al thread di non usare i file del db
	databaseAccessible = false;

	//alloca spazio per il nuovo database
	tmpT = new uint64_t[newDatabaseSize];
	tmpD = new int[newDatabaseSize];

	//resetta la variabile posizione
	currentDatabasePos[numDatabase] = 0;
	
	//copia i vecchi puntatori
	tmpT2 = databaseTime[numDatabase];
	tmpD2 = databaseValues[numDatabase];

	//copia i nuovi puntatori
	databaseTime[numDatabase] = tmpT;
	databaseValues[numDatabase] = tmpD;

	//aggiorna la dimensione del database
	currentDatabaseSize[numDatabase] = newDatabaseSize;

	//libera lo spazio dei vecchi array
	delete tmpT2;
	delete tmpD2;
	
	//cancella il file relativo al database
	fclose(dataFile[numDatabase]);
	dataFile[numDatabase] = fopen(dataFileName[numDatabase], "w+");

	//ora i file sono di nuovo accessibili
	databaseAccessible = true;
}

void loadDatabaseFromFiles() {
	int valuesCounter = 0;
	for (int cnt = 0; cnt < numSensors; cnt++) {
		int tmpScanRes = 1;
		rewind(dataFile[cnt]);
		while ((tmpScanRes != EOF) && (tmpScanRes != 0)) { 
			uint64_t tmpTimeVal;
			int tmpDataVal;
			//tmpScanRes = fscanf(dataFile[cnt], "%llu%d", &tmpTimeVal, &tmpDataVal);

    		char tmpLine[16];
    		if (!fgets(tmpLine, 23, dataFile[cnt])) break;
    		tmpScanRes = sscanf(tmpLine, "%llu\t%d", &tmpTimeVal, &tmpDataVal);

			
			//printf("D %llu %d", tmpTimeVal, tmpDataVal);
			if (tmpScanRes == EOF) {
				//printf("T %s;", dataFileName[cnt]);
				break;
			}
			else if (tmpScanRes == 2) {
				//printf("R%d", tmpScanRes);
				if (currentDatabasePos[cnt] + 2 >= currentDatabaseSize[cnt])
					expandDatabase(cnt);
				//printf("E%d", tmpScanRes);

				databaseTime[cnt][currentDatabasePos[cnt]] = tmpTimeVal;
				databaseValues[cnt][currentDatabasePos[cnt]] = tmpDataVal;
				currentDatabasePos[cnt]++;

				valuesCounter++;
			}
			else {
				printf("Corrupted database\n");
				fflush(stdout);
			}
		}
	}

	printf("Successfully loaded %d values from database\n", valuesCounter);
}

void openFiles() {
	createFileNamesList();
	for (int cnt = 0; cnt < numSensors; cnt++)
		dataFile[cnt] = fopen(dataFileName[cnt], "a+");

	for (int cnt = 0; cnt < numSensors; cnt++) {
		if(dataFile[cnt] == NULL) {
    		printf("Error opening database file: %s\n", strerror(errno));
    		exit(1);
		}
	}
}

void closeFiles() {
	for (int cnt = 0; cnt < numSensors; cnt++)
		fclose(dataFile[cnt]);
}

int initSerial(char* serialName) { 
	createSerialList();

	int ser = open(serialName, O_RDWR | O_NOCTTY | O_NDELAY);

	if (ser == -1 ) return -1;

	struct termios options;
	tcgetattr(ser, &options);
	options.c_cflag = B57600 | CS8 | CLOCAL | CREAD; //baud rate
	options.c_iflag = IGNBRK | IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(ser, TCIFLUSH);
	tcsetattr(ser, TCSANOW, &options);

	usleep(20000);
	return ser;
}

void serialWrite(int ser, char* data, int len) {
	write(ser, data, len);
}

int serialRead(int ser, unsigned char* data, int len) {
	return read(ser, (void*)data, len);
}

void resetBufferPos() {
	for (int cnt = 0; cnt < numUSB; cnt++)
		bufferPos[numUSB] = 0;
}

void initAllSerial() {
	resetBufferPos();

	for (int cnt = 0; cnt < numUSB; cnt++)
		serialModule[cnt] = initSerial(serNames[cnt]);

	for (int cnt = 0; cnt < numUSB; cnt++) {
		if (serialModule[cnt] < 0) {
			serialActive[cnt] = false;
		}
		else {
			serialActive[cnt] = true;
			printf("Successfully opened serial %s\n", serNames[cnt]);
		}
	}
}

bool devExists(char* devPath) {
	if( access( devPath, F_OK ) != -1 )
		return true;

	return false;
}

void refreshActiveSerialList() {
	for (int cnt = 0; cnt < numUSB; cnt++) {
		if (!serialActive[cnt]) {
			if (devExists(serNames[cnt])) {
				printf("New serial device found\n");
				int tmpSerID = initSerial(serNames[cnt]);
				if (tmpSerID >= 0) {
					serialModule[cnt] = tmpSerID;
					serialActive[cnt] = true;
				}
			}
		}/*
		else { //if serial is active check if it still exists
			if (!devExists(serNames[cnt])) {
				printf("A%s\n", serNames[cnt]);
				serialActive[cnt] = false;
				close(serialModule[cnt]);
			}

		}*/
	}
}

void start() {
	bcm2835_gpio_fsel(CE_PIN, BCM2835_GPIO_FSEL_OUTP);

	bcm2835_spi_begin();
	
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
	
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128); //nRF24L01+ maximum is 10Mhz
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);

	bcm2835_gpio_write(CE_PIN, LOW);
	initializeNRF24L01();
	bcm2835_gpio_write(CE_PIN, HIGH);

	initAllSerial();
}

void parseTemperature(char data1, char data2, char data3, char data4, char data5) {
	temperatureValue[0] = data5;
	temperatureValue[1] = data4;
	temperatureValue[2] = data3;
	temperatureValue[3] = data2;
	temperatureValue[4] = data1;

	char* tmp;
	int tmpTempVal = (int)strtol(temperatureValue, &tmp, 10);
	tmpTempVal -= 323;
	tmpTempVal *= 24;
	char tmpStrVal[10] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};
	sprintf(tmpStrVal, "%d", tmpTempVal);
	temperatureValue[0] = tmpStrVal[0];
	temperatureValue[1] = tmpStrVal[1];
	temperatureValue[2] = tmpStrVal[2];
	temperatureValue[3] = tmpStrVal[3];
	temperatureValue[4] = tmpStrVal[4];

	struct timeval tempOra;
	gettimeofday(&tempOra, NULL);
	uint64_t tempOraus = (uint64_t)(tempOra.tv_sec) * 1000 + (uint64_t)(tempOra.tv_usec / 1000);

	if (saveAcceleration) {
		int currentSensorID = 1;
		//printf("A");
		if (databaseAccessible)
			fprintf(dataFile[currentSensorID], "%llu\t%d\n", tempOraus, tmpTempVal);
		if (currentDatabasePos[currentSensorID] >= currentDatabaseSize[currentSensorID] - 2)
			expandDatabase(currentSensorID);
		char* tmp;
		databaseValues[currentSensorID][currentDatabasePos[currentSensorID]] = tmpTempVal;
		databaseTime[currentSensorID][currentDatabasePos[currentSensorID]] = tempOraus;
		currentDatabasePos[currentSensorID]++;
	}

	//printf("Temperature: %f\n", data5);
}

void parseRadiation(char data1, char data2, char data3, char data4, char data5) {
	radiationValue[0] = data1;
	radiationValue[1] = data2;
	radiationValue[2] = data3;
	radiationValue[3] = data4;
	radiationValue[4] = data5;

	printf("Radiation: %f\n", radiationValue);
}

void parseLampCurrent(char data1, char data2, char data3, char data4, char data5) {
	lampCurrentValue[0] = data1;
	lampCurrentValue[1] = data2;
	lampCurrentValue[2] = data3;
	lampCurrentValue[3] = data4;
	lampCurrentValue[4] = data5;

	printDebugLineStart();
	if (DEBUG)
		printf("[data received] Lamp current: %s", lampCurrentValue);
	printDebugLineEnd();
}

void parseLampVoltage(char data1, char data2, char data3, char data4, char data5) {
	lampVoltageValue[0] = data1;
	lampVoltageValue[1] = data2;
	lampVoltageValue[2] = data3;
	lampVoltageValue[3] = data4;
	lampVoltageValue[4] = data5;

	printDebugLineStart();
	if (DEBUG)
		printf("[data received] Lamp voltage: %s", lampVoltageValue);
	printDebugLineEnd();
}

void parseAcceleration(char data1, char data2, char data3, char data4, char data5) {
	accelerationValue[0] = data1;
	accelerationValue[1] = data2;
	accelerationValue[2] = data3;
	accelerationValue[3] = data4;
	accelerationValue[4] = data5;

	char* tmp;
	int tmpAccVal = (int)strtol(accelerationValue, &tmp, 10);
	tmpAccVal -= 323;
	tmpAccVal *= 24;
	char tmpStrVal[10] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};
	sprintf(tmpStrVal, "%d", tmpAccVal);
	accelerationValue[0] = tmpStrVal[0];
	accelerationValue[1] = tmpStrVal[1];
	accelerationValue[2] = tmpStrVal[2];
	accelerationValue[3] = tmpStrVal[3];
	accelerationValue[4] = tmpStrVal[4];

	struct timeval tempOra;
	gettimeofday(&tempOra, NULL);
	uint64_t tempOraus = (uint64_t)(tempOra.tv_sec) * 1000 + (uint64_t)(tempOra.tv_usec / 1000);

	if (saveAcceleration) {
		int currentSensorID = 4;
		//printf("A");
		if (databaseAccessible)
			fprintf(dataFile[currentSensorID], "%llu\t%d\n", tempOraus, tmpAccVal);
		if (currentDatabasePos[currentSensorID] >= currentDatabaseSize[currentSensorID] - 2)
			expandDatabase(currentSensorID);
		char* tmp;
		databaseValues[currentSensorID][currentDatabasePos[currentSensorID]] = tmpAccVal;
		databaseTime[currentSensorID][currentDatabasePos[currentSensorID]] = tempOraus;
		currentDatabasePos[currentSensorID]++;
	}

	//char* tmp;
	//printf("A%d", strtol(accelerationValue, &tmp, 10));
}

void parseDistance(char data1, char data2, char data3, char data4, char data5) {
	distanceValue[0] = data1;
	distanceValue[1] = data2;
	distanceValue[2] = data3;
	distanceValue[3] = data4;
	distanceValue[4] = data5;
}

void parseHumidity(char data1, char data2, char data3, char data4, char data5) {
	humidityValue[0] = data1;
	humidityValue[1] = data2;
	humidityValue[2] = data3;
	humidityValue[3] = data4;
	humidityValue[4] = data5;
	
	struct timeval tempOra;
	gettimeofday(&tempOra, NULL);
	uint64_t tempOraus = (uint64_t)(tempOra.tv_sec) * 1000 + (uint64_t)(tempOra.tv_usec / 1000);

	if (saveHumidity) {
		int currentSensorID = 2;
		if (databaseAccessible)
			fprintf(dataFile[currentSensorID], "%llu\t%c%c%c%c%c\n", tempOraus, data1, data2, data3, data4, data5);
		if (currentDatabasePos[currentSensorID] >= currentDatabaseSize[currentSensorID] - 2)
			expandDatabase(currentSensorID);
		char* tmp;
		databaseValues[currentSensorID][currentDatabasePos[currentSensorID]] = (int)strtol(humidityValue, &tmp, 10);
		databaseTime[currentSensorID][currentDatabasePos[currentSensorID]] = tempOraus;
		currentDatabasePos[currentSensorID]++;
	}

	//printf("Humidity: %s\n", humidityValue);
}
void parseVoltage(char data1, char data2, char data3, char data4, char data5) {
	voltageValue[0] = data1;
	voltageValue[1] = data2;
	voltageValue[2] = data3;
	voltageValue[3] = data4;
	voltageValue[4] = data5;

	char* tmp;
	int tmpVolVal = (int)strtol(voltageValue, &tmp, 10);
	tmpVolVal -= 323;
	tmpVolVal *= 24;
	char tmpStrVal[10] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};
	sprintf(tmpStrVal, "%d", tmpVolVal);
	voltageValue[0] = tmpStrVal[0];
	voltageValue[1] = tmpStrVal[1];
	voltageValue[2] = tmpStrVal[2];
	voltageValue[3] = tmpStrVal[3];
	voltageValue[4] = tmpStrVal[4];

	struct timeval tempOra;
	gettimeofday(&tempOra, NULL);
	uint64_t tempOraus = (uint64_t)(tempOra.tv_sec) * 1000 + (uint64_t)(tempOra.tv_usec / 1000);

	if (saveAcceleration) {
		int currentSensorID = 5;
		//printf("A");
		if (databaseAccessible)
			fprintf(dataFile[currentSensorID], "%llu\t%d\n", tempOraus, tmpVolVal);
		if (currentDatabasePos[currentSensorID] >= currentDatabaseSize[currentSensorID] - 2)
			expandDatabase(currentSensorID);
		char* tmp;
		databaseValues[currentSensorID][currentDatabasePos[currentSensorID]] = tmpVolVal;
		databaseTime[currentSensorID][currentDatabasePos[currentSensorID]] = tempOraus;
		currentDatabasePos[currentSensorID]++;
	}

	//printf("Voltage: %s\n", voltageValue);
}

void parsePacket(char packetContent[]) {
	/*for (int cnt = 2; cnt + 1 < 10; cnt++) dati[cnt] = packetContent.charAt(cnt);
	//char mittente = dati[0];
	//char destinatario = dati[1];*/

	char tipoDati = packetContent[2]; // a = data b = other
	
	/*char tipoDati2 = ' ';
	if (tipoDati == 'b') tipoDati2 = dati[3];
	if ((tipoDati == 'b') && (tipoDati2 == 'a')) sendKeepAliveReply();*/

	char tipoSensore = ' ';
	if (tipoDati == 'a') { 
		tipoSensore = packetContent[3];   //1 = TEMPERATURA 2 = ACCELERAZIONE 3 = TEMPO
		if (tipoSensore == '1') parseTemperature(packetContent[4], packetContent[5], packetContent[6], packetContent[7], packetContent[8]);
		else if (tipoSensore == '2') parseAcceleration(packetContent[4], packetContent[5], packetContent[6], packetContent[7], packetContent[8]);
		//else if (tipoSensore == '3') parseTime(packetContent[4], packetContent[5], packetContent[6], packetContent[7], packetContent[8]);
		else if (tipoSensore == '4') parseDistance(packetContent[4], packetContent[5], packetContent[6], packetContent[7], packetContent[8]);
		else if (tipoSensore == '5') parseRadiation(packetContent[4], packetContent[5], packetContent[6], packetContent[7], packetContent[8]);
		else if (tipoSensore == '6') parseHumidity(packetContent[4], packetContent[5], packetContent[6], packetContent[7], packetContent[8]);
		else if (tipoSensore == '7') parseVoltage(packetContent[4], packetContent[5], packetContent[6], packetContent[7], packetContent[8]);
		else if (tipoSensore == 'A') parseLampVoltage(packetContent[4], packetContent[5], packetContent[6], packetContent[7], packetContent[8]);
		else if (tipoSensore == 'B') parseLampCurrent(packetContent[4], packetContent[5], packetContent[6], packetContent[7], packetContent[8]);
	}
	//System.out.println("tipoDati: " + tipoDati);
	//System.out.println("tipoSensore: " + tipoSensore);
}

bool isValidPacket(char packetContent[]) {
	
	if (packetContent[0] != 'a') {
		printf(",");
		fflush(stdout);
		return false;
	}
	else if (packetContent[9] != '!') {
		printf(";");
		fflush(stdout);
		return false;
	}
	return true;
}

void acquisisciDati(char messaggio[10]) {
	//printf("*");
    fflush(stdout);
	if (isValidPacket(messaggio)) {
    	fflush(stdout);
		parsePacket(messaggio);
		messaggio[9] = '*';
		messaggio[0] = '*';
	}
	else fprintf(stderr, ";");
}

void deleteBufferElem(int bufferIndex, int shiftNum) {
	for (int cnt = shiftNum; cnt < bufferPos[bufferIndex]; cnt++)
		serialBuffer[bufferIndex][cnt - shiftNum] = serialBuffer[bufferIndex][cnt];
	bufferPos[bufferIndex] -= shiftNum;
}

void acquisisciDatiSeriale() {
	int packetLength = 10;

	//controlla se nel buffer c'è un pacchetto valido
	for (int currentSerial = 0; currentSerial < numUSB; currentSerial++) {
		for (int cnt = 0; cnt < bufferPos[currentSerial] - (packetLength - 3); cnt++) {
			if ((serialBuffer[currentSerial][cnt] == 'a') and
				(serialBuffer[currentSerial][cnt + 1] == 'b') and
				(serialBuffer[currentSerial][cnt + 2] == 'a') and
				(serialBuffer[currentSerial][cnt + 9] == '!')) {

				//se c'è un pacchetto valido lo estrae e lo copia in un array
				char tmpPacket[packetLength];
				for (int cnt2 = 0; cnt2 < packetLength; cnt2++)
					tmpPacket[cnt2] = serialBuffer[currentSerial][cnt + cnt2];

				//il pacchetto viene processato
				acquisisciDati(tmpPacket);

				//una volta processato il pacchetto rimuove
				//dal buffer il pacchetto e i byte precendenti
				deleteBufferElem(currentSerial, cnt + packetLength);
			}
		}

		//elimina i primi elementi del buffer per riportarlo a massimo 9 elementi
		//aggiorna bufferPos
		if (bufferPos[currentSerial] >= packetLength) {
			int shiftNum = bufferPos[currentSerial] - (packetLength - 1);
			deleteBufferElem(currentSerial, shiftNum);
		}
	}
}

int numPacket = 0;

void checkDataAvailable() {
	if (packetAvail()) {
			char data[10];
			getNRFpacket(data);
			//printf("Data: %c%c\n", data[0], data[9]);
			acquisisciDati(data);
			
			//printf(".");
			//fflush(stdout);
			/*numPacket++;
			if (numPacket >= 100) {
				numPacket = 0;

				gettimeofday(&tempOra, NULL);
				int tempOraus = (tempOra.tv_sec) * 1000000 + (tempOra.tv_usec);

				printf("AS100 %d\n", (tempOraus - tempPacchettous));

				gettimeofday(&tempPacchetto, NULL);
				tempPacchettous = (tempPacchetto.tv_sec) * 1000000 + (tempPacchetto.tv_usec) ;
			}*/
		}
		unsigned char tmpBufferR[256];
		for (int cnt = 0; cnt < numUSB; cnt++) {
			if (serialActive[cnt]) {
				short numByteR = serialRead(serialModule[cnt], tmpBufferR , 255);
				if (numByteR > 0) {
					for (int cnt2 = 0; cnt2 < numByteR; cnt2++) {
						//printf("%c", tmpBufferR[cnt2]);
						serialBuffer[cnt][bufferPos[cnt]] = tmpBufferR[cnt2];
						bufferPos[cnt]++;
					}
					acquisisciDatiSeriale();
				}
			}
		}
		numPacket++;
		if (numPacket >= 50000) { //called every 50000 loops
			numPacket = 0;
			refreshActiveSerialList();
			/*for (int cnt = 0; cnt < currentDatabasePos[4]; cnt++)
				printf("%d, ", databaseValues[4][cnt]);
			printf("\n");*/
		}
}


#include "websocket.cpp"


int force_exit = 0;

void sigHandler(int signal) {
	force_exit = 1;
}

void checkDataThread() {
	while (!force_exit) {
		checkDataAvailable();
	}
}

int main() {
	printf("PhyLab 2.0 v1.2\n");
	printf("© Michele Lizzit\n");
	printf("15/4/2015\n\n\n");
	fflush(stdout);

	//exit if signal received
	signal(SIGINT, sigHandler);
	signal(SIGTERM, sigHandler);
	signal(SIGSEGV, segFaultHandler);

	if (!bcm2835_init()) {
		printf("\n\nThis software is written only for bcm2835!\n\n");
		fflush(stdout);
		return 1;
	}

	printf("Starting PhyLab...\n");
	fflush(stdout);
	start();
	fflush(stdout);
	openFiles();
	createDatabase();
	loadDatabaseFromFiles();

	bool end = false;

	printf("Listening for incoming packets...\n\n");
	fflush(stdout);

	thread dataThread(checkDataThread);

	start_websocket_server();

	while (!force_exit) {
		lws_service(context, 50);
	}

	printf("Exiting...\n");
	fflush(stdout);

	dataThread.join();

	lws_context_destroy(context);
	closelog();
	closeFiles();
	printf("PhyLab terminated\n");
	fflush(stdout);
}