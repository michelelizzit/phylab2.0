//Uncomment for physical serial
#define centralUnitSerial Serial

//Uncomment for software serial
#define centralUnitSerial softwareSerial
#define SER_RX_PIN 9
#define SER_TX_PIN 10

#define MEASURE_PIN A0


//Default CE is 8, CSN is 7
//#define PIN_NRF_OVERRIDE_DEFAULT
//#define CE_PIN A0
//#define CSN_PIN A1

//Uncomment if you are using a PhyLab 2.0 module
//#define PCB_REV1_ATMEGA328

//Preset for PhyLab 2.0 modules
#ifdef PCB_REV1_ATMEGA328
  //Default CE is 8, CSN is 7
  #define PIN_NRF_OVERRIDE_DEFAULT
  #define CE_PIN A0
  #define CSN_PIN A1
  #define SER_RX_PIN 9//3
  #define SER_TX_PIN 10//4
  
  #define centralUnitSerial softwareSerial               

  #define MEASURE_PIN A5
#endif
