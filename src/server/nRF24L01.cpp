/*	
 *
 *	nRF24L01+ C++ RaspberryPI library
 *  This software is part of PhyLab 2.0 - lizzit.it/phylab
 *	Author: Michele Lizzit - lizzit.it
 *  v1.0 - 26/7/2016
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

//PIN
//#define CE_PIN RPI_GPIO_P1_18
#define CE_PIN RPI_GPIO_P1_22


//NRF ADDRESS
char NRF_ADDR[3] = { 0x87, 0xC1, 0xB9 };

//COMMANDS
const char R_REG = (char)0x00;
const char W_REG = (char)0x20;
const char RX_PAYLOAD = (char)0x61;
const char TX_PAYLOAD = (char)0xA0;
const char FLUSH_TX = (char)0xE1;
const char FLUSH_RX = (char)0xE2;
const char ACTIVATE = (char)0x50;

//REG NAMES
const char CONFIG = (char)0x00;
const char EN_AA = (char)0x01;
const char EN_RXADDR = (char)0x02;
const char SETUP_AW = (char)0x03;
const char SETUP_RETR = (char)0x04;
const char RF_CH = (char)0x05;
const char RF_SETUP = (char)0x06;
const char STATUS = (char)0x07;
const char OBSERVE_TX = (char)0x08;
const char CD = (char)0x09;
const char RX_ADDR_P0 = (char)0x0A;
const char RX_ADDR_P1 = (char)0x0B;
const char RX_ADDR_P2 = (char)0x0C;
const char RX_ADDR_P3 = (char)0x0D;
const char RX_ADDR_P4 = (char)0x0E;
const char RX_ADDR_P5 = (char)0x0F;
const char TX_ADDR = (char)0x10;
const char RX_PW_P0 = (char)0x11;
const char RX_PW_P1 = (char)0x12;
const char RX_PW_P2 = (char)0x13;
const char RX_PW_P3 = (char)0x14;
const char RX_PW_P4 = (char)0x15;
const char RX_PW_P5 = (char)0x16;
const char FIFO_STATUS = (char)0x17;
const char DYNPD = (char)0x1C;
const char FEATURE = (char)0x1D;

void regWrite(char reg, char val) {
	char buf[2];
	buf[0] = (char)(W_REG | reg);
	buf[1] = val;
	bcm2835_spi_transfern(buf, 2);
}

void addrWrite(char reg, char num, char addr[]) {
	char buf[num + 1];
	buf[0] = (char)(W_REG | reg);

	for (short cnt = 1; cnt <= num; cnt++)
		buf[cnt] = addr[cnt - 1];
	
	bcm2835_spi_transfern(buf, num + 1);
}

char regRead(char reg) {
	char buf[2];
	buf[0] = (char)(R_REG | reg);
	buf[1] = (char)0x00;

	bcm2835_spi_transfern(buf, 2);

	return buf[1];
}

void setPayloadContent(char num, char data[]) {
	char buf[num + 1];
	buf[0] = TX_PAYLOAD;

	for (short cnt = 1; cnt <= num; cnt++)
		buf[cnt] = data[cnt];

	bcm2835_spi_transfern(buf, num + 1);

	bcm2835_gpio_write(CE_PIN, HIGH);
	usleep(1000);
	bcm2835_gpio_write(CE_PIN, LOW);
}

void getPayload(char num, char data[]) {
	char buf[num + 1];
	buf[0] = RX_PAYLOAD;
	
	for (short cnt = 1; cnt <= num; cnt++)
		buf[cnt] = (char)0x00;

	bcm2835_spi_transfern(buf, num + 1);

	for (short cnt = 0; cnt < num; cnt++)
		data[cnt] = buf[cnt + 1];
}

void flushBuffers() {
	regWrite(STATUS, (char)0x70);
	bcm2835_spi_transfer(FLUSH_TX);
	bcm2835_spi_transfer(FLUSH_RX);
}

char getStatusRegister() { //evita la lettura del registro vedi datasheet
	return bcm2835_spi_transfer((char)0xFF);
}

void getNRFpacket(char* packet) {
	getPayload((char)10, packet);
	
	//clear data available bit
	regWrite(STATUS, (char)0x40);
}

bool packetAvail() {
	return (getStatusRegister() & 0x40) != 0;
}

void setRXMode() {
	regWrite(CONFIG, (char)0x0B);
	bcm2835_gpio_write(CE_PIN, HIGH);
}

void setTXMode() {
	bcm2835_gpio_write(CE_PIN, LOW);
	regWrite(CONFIG, (char)0x0A);
}

void powerDownNRF() {
	bcm2835_gpio_write(CE_PIN, LOW);
	regWrite(CONFIG, (char)0);
}

void initializeNRF24L01() {
	bcm2835_gpio_write(CE_PIN, LOW);
	usleep(1600); //see datasheet power down to RX time
	regWrite(CONFIG, (char)0x0B); //RXMode
	regWrite(EN_AA, (char)0x00);
	regWrite(EN_RXADDR, (char)0x01);
	regWrite(SETUP_AW, (char)0x01);
	regWrite(SETUP_RETR, (char)0x00);
	regWrite(RF_CH, (char)0x01); //RADIO CHANNEL
	regWrite(RF_SETUP, (char)0x26); //250kbps, 0dBm = 0x26 250kbps, 0dBm (1mW) = 0x06
	regWrite(RX_PW_P0, (char)0x0A); //Payload size = 10 byte

	//use the same address for TX and RX
	addrWrite(RX_ADDR_P0, (char)3, NRF_ADDR);
	addrWrite(TX_ADDR, (char)3, NRF_ADDR);

	flushBuffers();

	setRXMode();
}