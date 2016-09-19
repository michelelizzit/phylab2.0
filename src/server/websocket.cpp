/*
 *
 *	This file is part of PhyLab 2.0 - https://lizzit.it/phylab
 *	Author: Michele Lizzit - lizzit.it
 *
 *	Updates: 	19/9/2016
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <string>

#include <syslog.h>

#include <signal.h>

#include <libwebsockets.h>
#include <algorithm>

bool compareCharArr(char* arr1, char* arr2, unsigned short len) {
	for (unsigned short cnt = 0; cnt < len; cnt++)
		if (arr1[cnt] != arr2[cnt]) return false;
	return true;
}

uint64_t getTimeFromString(char* input, short startChar) {
	char tmpTime[14];
	for (int cnt = startChar; cnt < 13 + startChar; cnt++)
		tmpTime[cnt - startChar] = input[cnt];

	tmpTime[13] = '\0';

	char* tmp;
	return (uint64_t)strtoull(tmpTime, &tmp, 10);
}

int searchDatabase(int sensorID, uint64_t searchTime) {
	if (searchTime == 0) return 0;

	bool found = false;
	int maxPos = currentDatabasePos[sensorID] - 1;
	if (maxPos == 0) return -1;

	int currentPos = maxPos / 2;
	int leftNum = 0;
	int rightNum = maxPos;


	if (maxPos <= 0) return -1;
	
	if (databaseTime[sensorID][0] >= searchTime) return 0;
	if (databaseTime[sensorID][maxPos] < searchTime) return -1;

	while (!found) {
		if (databaseTime[sensorID][currentPos] == searchTime) {
			found = true;
			break;
		}
		else if (searchTime > databaseTime[sensorID][currentPos])
			leftNum = currentPos + 1;
		else
			rightNum = currentPos - 1;
		currentPos = (leftNum + rightNum) / 2;
		if (leftNum > rightNum) return -1;
	}
	return currentPos;
}

static int phylab_callback(
	struct lws *wsi,
	enum lws_callback_reasons reason,
	void *user, void *in, size_t len) {
	int n, m;
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 +
		LWS_SEND_BUFFER_POST_PADDING];
	unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];


	switch (reason) {

		case LWS_CALLBACK_ESTABLISHED:
		{
			printf("New connection\n");
			fflush(stdout);
		}
		break;

		case LWS_CALLBACK_SERVER_WRITEABLE:
		break;

		case LWS_CALLBACK_RECEIVE:
		{

			//printf("%s\n",in);

			const char* const tmpString = static_cast<const char* const>(in);
			char* tmpString1 = static_cast<char*>(in);

			if (!strcmp(tmpString,"rad")) { //if strings are equal
				//printf("RAD %d\n", radiationValue);

				int replyLen = 6;
				unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
				
				tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING] = 'D';
				for (int i = 1; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = radiationValue[i - 1];
				
				lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
				free(tmpReplyBuf);
			}
			else if (!strcmp(tmpString,"temp")) {
				printf("TEMP %d\n", temperatureValue);

				int replyLen = 6;
				unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
		
				tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING] = 'D';
				for (int i = 1; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = temperatureValue[i - 1];
				
				lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
				free(tmpReplyBuf);
			}
			else if (!strcmp(tmpString,"lampG")) {
				printDebugLineStart();
				if (DEBUG)
					printf("[websocket server] Lamp request");
				printDebugLineEnd();
				const int replyLen = 6;

				//prevents data to change before having finished sending it
				char data1[replyLen];
				char data2[replyLen];
				strcpy(data1, lampCurrentValue);
				strcpy(data2, lampVoltageValue);

				unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);

				tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING] = 'E';
				for (int i = 1; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = data1[i - 1];
				lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);

				tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING] = 'D';
				for (int i = 1; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = data2[i - 1];
				lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);

				free(tmpReplyBuf);
			}
			else if (!strcmp(tmpString,"accG")) {
				//printf("/");

				int replyLen = 6;
				unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
				tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING] = 'D';
				
				for (int i = 1; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = accelerationValue[i - 1];
				
				lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
				free(tmpReplyBuf);
			}
			else if (compareCharArr(tmpString1, "accT", 4)) {
				//conta il numero di valori disponibili a partire dall'ora richiesta, se ce ne sono piu di 100
				//risponde con N, altrimenti con T, seguito dai valori separati da una virgola
				const int currentSensorID = 4; //4 is the ID for acceleration sensor
				const int numCharTime = 13;
				uint64_t requestTime = getTimeFromString(tmpString1, (short)4);
				int tmpNumData = -1;

				//POCO EFFICIENTE
				/*for (int cnt = 0; cnt < currentDatabasePos[currentSensorID]; cnt++) {
					if (databaseTime[currentSensorID][cnt] > requestTime) {
						tmpNumData = cnt;
						break;
					}
				}
				printf("RESULT1: %d", tmpNumData);*/

				//PIU EFFICIENTE
				tmpNumData = searchDatabase(currentSensorID, requestTime);
				tmpNumData++;
				//printf("RESULT2: %d", tmpNumData);

				if (tmpNumData == 0) { //No data
					int replyLen = 6;
					char tmpReply[] = {'N', 'O', 'D', 'A', 'T', 'A', '\0'};

					unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
					for (int i = 0; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = tmpReply[i];

					lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
					free(tmpReplyBuf);
				}
				else {
					int numData = currentDatabasePos[currentSensorID] - tmpNumData;


					char headerBuf[] = {'F', '2', '0', '0', '\n', '\0'};
					
					if (numData > 200) //Se ha più di 200 valori da inviare
						numData = 200;
					else
						headerBuf[1] = '0';

					int headerLen = 5;
					int singleDataLen = (13 + 1 + 5 + 1);

					int dataToSend = tmpNumData;

					int replyLen = headerLen + (singleDataLen * numData); //TIME\tVALUE\n

					unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
					for (int i = 0; i < headerLen; i++) 
						tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = headerBuf[i];

					int bufPos = headerLen;
					for (int i = 0; i < numData; i++) {
						char tmpReply[singleDataLen];
						sprintf(tmpReply, "%llu\t%5d\n", databaseTime[currentSensorID][dataToSend], databaseValues[currentSensorID][dataToSend]);
						dataToSend++;
						for (int j = 0; j < singleDataLen; j++) {
							tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + bufPos] = tmpReply[j];
							bufPos++;
						}
					}

					//printf("D: %s;", tmpReplyBuf);

					lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
					free(tmpReplyBuf);
				}
			}
			else if (!strcmp(tmpString,"accD")) {
				const int currentSensorID = 4; //4 is the ID for acceleration sensor
				printf("DELETE ACC DATABASE");
				clearDatabase(currentSensorID);
				int replyLen = 6;
				unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
				char tmpReply[] = {'R', 'D', 'E', 'L', 'O', 'K', '\0'};
				for (int i = 0; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = tmpReply[i];
				lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
				free(tmpReplyBuf);
			}
			else if (!strcmp(tmpString,"time")) {
				printf("TIME %d\n", timeValue);

				int replyLen = 6;
				unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
				tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING] = 'D';
				
				for (int i = 1; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = timeValue[i - 1];
				
				lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
				free(tmpReplyBuf);
			}
			else if (!strcmp(tmpString,"dist")) {
				printf("DIST %d\n", distanceValue);

				int replyLen = 6;
				unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
				tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING] = 'D';
				
				for (int i = 1; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = distanceValue[i - 1];
				
				lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
				free(tmpReplyBuf);
			}
			else if (!strcmp(tmpString,"humG")) {
				//printf("HUM %s\n", humidityValue);

				int replyLen = 6;
				unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
				tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING] = 'D';
				
				for (int i = 1; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = humidityValue[i - 1];
				
				lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
				free(tmpReplyBuf);
			}
			else if (compareCharArr(tmpString1, "humT", 4)) {
				//conta il numero di valori disponibili a partire dall'ora richiesta, se ce ne sono piu di 100
				//risponde con N, altrimenti con T, seguito dai valori separati da una virgola
				const int currentSensorID = 2;
				const int numCharTime = 13;
				uint64_t requestTime = getTimeFromString(tmpString1, (short)4);
				int tmpNumData = -1;

				tmpNumData = searchDatabase(currentSensorID, requestTime);
				tmpNumData++;

				//printf("RESULT2: %d", tmpNumData);

				if (tmpNumData == 0) { //No data
					printf("NODATA\n");
					fflush(stdout);
					int replyLen = 6;
					char tmpReply[] = {'N', 'O', 'D', 'A', 'T', 'A', '\0'};

					unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
					for (int i = 0; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = tmpReply[i];

					lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
					free(tmpReplyBuf);
				}
				else {
					int numData = currentDatabasePos[currentSensorID] - tmpNumData;


					char headerBuf[] = {'F', '2', '0', '0', '\n', '\0'};
					
					if (numData > 200) //Se ha più di 200 valori da inviare
						numData = 200;
					else
						headerBuf[1] = '0';

					int headerLen = 5;
					int singleDataLen = (13 + 1 + 5 + 1);

					int dataToSend = tmpNumData;

					int replyLen = headerLen + (singleDataLen * numData); //TIME\tVALUE\n
					if (replyLen < headerLen) {
						printf("EE %d;%d;%d", numData, tmpNumData, currentDatabasePos[currentSensorID]);
						fflush(stdout);
						numData = 0;
						replyLen = headerLen;
					}

					unsigned int prePadding = LWS_SEND_BUFFER_PRE_PADDING;
					unsigned int postPadding = LWS_SEND_BUFFER_POST_PADDING;
					unsigned char *tmpReplyBuf = (unsigned char*) malloc(prePadding + replyLen + postPadding);

					for (int i = 0; i < headerLen; i++) 
						tmpReplyBuf[prePadding + i] = headerBuf[i];

					int bufPos = headerLen;
					for (int i = 0; i < numData; i++) {
						char tmpReply[singleDataLen];
						sprintf(tmpReply, "%llu\t%5d\n", databaseTime[currentSensorID][dataToSend], databaseValues[currentSensorID][dataToSend]);
						dataToSend++;
						for (int j = 0; j < singleDataLen; j++) {
							tmpReplyBuf[prePadding + bufPos] = tmpReply[j];
							bufPos++;
						}
					}
					lws_write(wsi, &tmpReplyBuf[prePadding], replyLen, LWS_WRITE_TEXT);
					free(tmpReplyBuf);
				}
			}
			else if (!strcmp(tmpString,"humD")) {
				const int currentSensorID = 2; //2 is the ID for humidity sensor
				printf("DELETE HUM DATABASE");
				clearDatabase(currentSensorID);
				int replyLen = 6;
				unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
				char tmpReply[] = {'R', 'D', 'E', 'L', 'O', 'K', '\0'};
				for (int i = 0; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = tmpReply[i];
				lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
				free(tmpReplyBuf);
			}
			else if (!strcmp(tmpString,"volt")) {
				printf("VOLT %s\n", voltageValue);

				int replyLen = 6;
				unsigned char *tmpReplyBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + replyLen + LWS_SEND_BUFFER_POST_PADDING);
				tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING] = 'D';
				
				for (int i = 1; i < replyLen; i++) tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING + i ] = voltageValue[i - 1];
				
				lws_write(wsi, &tmpReplyBuf[LWS_SEND_BUFFER_PRE_PADDING], replyLen, LWS_WRITE_TEXT);
				free(tmpReplyBuf);
			}
			else {
				printf("INVALID REQUEST: %s\n", tmpString);
				break;
			}
			fflush(stdout);
		}
		break;

		case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
			break;

		default:
			break;
	}
	return 0;
}

static struct lws_protocols protocols[] = {
	{
		"phylab",
		phylab_callback,
		30,
		17,
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};

struct lws_context *context;
void start_websocket_server() {
	printf("Starting websocket server...\n");
	fflush(stdout);
	int opts = 0;
	const char *iface = NULL;
	int syslog_options = LOG_PID | LOG_PERROR;
	unsigned int oldus = 0;
	struct lws_context_creation_info info;

	memset(&info, 0, sizeof info);
	info.port = 9999;

	setlogmask(LOG_UPTO (LOG_DEBUG));
	lws_set_log_level(0, lwsl_emit_syslog);

	info.iface = iface;
	info.protocols = protocols;
	info.extensions = lws_get_internal_extensions();
	info.ssl_cert_filepath = NULL;
	info.ssl_private_key_filepath = NULL;

	info.gid = -1;
	info.uid = -1;
	info.options = opts;

	context = lws_create_context(&info);
	if (context == NULL) {
		lwsl_err("libwebsocket init failed\n");
		return;
	}
}