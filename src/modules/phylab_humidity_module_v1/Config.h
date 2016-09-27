#define DHTTYPE DHT22

//Default CE is 8, CSN is 7
#define PIN_NRF_OVERRIDE_DEFAULT

//Uncomment if you are using a PhyLab 2.0 module
#define PCB_REV1_ATMEGA328

//PER DEBUG
  //#define centralUnitSerial Serial

#define ENABLE_HUM
#define ENABLE_TEMP

//Preset for PhyLab 2.0 modules
#ifdef PCB_REV1_ATMEGA328
  #define CE_PIN A0
  #define CSN_PIN A1
  #define SER_RX_PIN 9//3
  #define SER_TX_PIN 10//4

  #define centralUnitSerial softwareSerial

  #define DHT22_PIN 1
  #define VCC_PIN 2
  #define GND_PIN A5                                                                                                                                          
#endif

//Pins for other boards
#ifndef PCB_REV1_ATMEGA328
  //Use physical serial
  #define centralUnitSerial Serial
  #define SER_RX_PIN 3
  #define SER_TX_PIN 4

  #define DHT22_PIN 3
  #define VCC_PIN 2
  #define GND_PIN 5
#endif
