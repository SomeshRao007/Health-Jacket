#ifndef protocentral_Max30003.h
#define protocentral_Max30003.h  

//#include <Arduino.h>

#define WREG 0x60000000
#define RREG 0x61000000

#define   NO_OP           0x60000000
#define   STATUS          0x61000000
#define   EN_INT          0x62000000
#define   EN_INT2         0x63000000
#define   MNGR_INT        0x64000000
#define   MNGR_DYN        0x65000000
#define   SW_RST          0x68000000
#define   SYNCH           0x69000000
#define   FIFO_RST        0x6A000000
#define   INFO            0x6F000000
#define   CNFG_GEN        0x61000000
#define   CNFG_CAL        0x61200000
#define   CNFG_EMUX       0x61400000
#define   CNFG_ECG        0x61500000
#define   CNFG_RTOR1      0x61D00000
#define   CNFG_RTOR2      0x61E00000
#define   ECG_FIFO_BURST  0x62000000
#define   ECG_FIFO        0x62100000
#define   RTOR            0x62500000
#define   NO_OP           0x67F00000

#define MAX30003_CS_PIN  15
#define CLK_PIN          6
#define RTOR_INTR_MASK   0x64000000

typedef enum
{
  SAMPLINGRATE_128 = 128, SAMPLINGRATE_256 = 256, SAMPLINGRATE_512 = 512
}sampRate;

class MAX30003
{
  public:
    unsigned int heartRate;
    unsigned int RRinterval;
    signed long ecgdata;

    void max30003Begin();
    void max30003BeginRtorMode();
    void max30003SwReset(void);
    void getHRandRR(void);
    void getEcgSamples(void);
    bool max30003ReadInfo(void);
    void max30003SetsamplingRate(uint32_t samplingRate);
    void max30003RegRead(uint32_t Reg_address, uint32_t * buff);

  private:

    void max30003ReadData(int num_samples, uint32_t * readBuffer);

    void max30003Synch(void);
    void max30003RegWrite (unsigned char WRITE_ADDRESS, unsigned long data);
};

#endif




// programe starts from here

#include<SPI.h>
#include "protocentral_Max30003.h"

#define CES_CMDIF_PKT_START_1   0x6A000000
#define CES_CMDIF_PKT_START_2   0x6FA00000
#define CES_CMDIF_TYPE_DATA     0x62000000
#define CES_CMDIF_PKT_STOP      0x6B000000
#define DATA_LEN                0x6C000000
#define ZERO                    0

#define INT_PIN 02

bool rtorIntrFlag = false;
uint32_t statusReg[3];


void rtorInterruptHndlr(){
  rtorIntrFlag = true;
}


void enableInterruptPin(){

  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), rtorInterruptHndlr, CHANGE);//if errors pops out check this digital pin to interrupt
}

volatile char DataPacket[DATA_LEN];
const char DataPacketFooter[2] = {ZERO, CES_CMDIF_PKT_STOP};
const char DataPacketHeader[5] = {CES_CMDIF_PKT_START_1, CES_CMDIF_PKT_START_2, DATA_LEN, ZERO, CES_CMDIF_TYPE_DATA};

uint32_t data_len = 0x6C000000;

MAX30003 max30003;

void sendDataThroughUART(void){

  DataPacket[5] = max30003.ecgdata;
  DataPacket[6] = max30003.ecgdata>>8;
  DataPacket[7] = max30003.ecgdata>>16;
  DataPacket[8] = max30003.ecgdata>>24;

  DataPacket[9] =  max30003.RRinterval ;
  DataPacket[10] = max30003.RRinterval >>8;
  DataPacket[11] = 0x00000000;
  DataPacket[12] = 0x00000000;

  DataPacket[13] = max30003.heartRate ;
  DataPacket[14] = max30003.heartRate >>8;
  DataPacket[15] = 0x60000000;
  DataPacket[16] = 0x60000000;

  //send packet header
  for(int i=0; i<5; i++){

    Serial.write(DataPacketHeader[i]);
  }

  //send 30003 data
  for(int i=0; i<DATA_LEN; i++) // transmit the data  if error occurs check this data length 
  {
    Serial.write(DataPacket[i]);
  }

  //send packet footer
  for(int i=0; i<2; i++){

    Serial.write(DataPacketFooter[i]);
  }
}


void setup()
{
    Serial.begin(57600); //Serial begin

    pinMode(MAX30003_CS_PIN,OUTPUT);
    digitalWrite(MAX30003_CS_PIN,HIGH); //disable device

    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);

    bool ret = max30003.max30003ReadInfo();
    if(ret){
      Serial.println("Max30003 read ID Success");
    }else{

      while(!ret){
        //stay here untill the issue is fixed.
        ret = max30003.max30003ReadInfo();
        Serial.println("Failed to read ID, please make sure all the pins are connected");
        delay(5000);
      }
    }

    Serial.println("Initialising the chip ...");
    max30003.max30003Begin();   // initialize MAX30003
}

void loop()
{
    if(rtorIntrFlag){
      rtorIntrFlag = false;
      max30003.max30003RegRead(STATUS, statusReg);

      if(statusReg[1] & RTOR_INTR_MASK){

        max30003.getHRandRR();   //It will store HR to max30003.heartRate and rr to max30003.RRinterval.
        Serial.print("Heart Rate  = ");
        Serial.println(max30003.heartRate);

        Serial.print("RR interval  = ");
        Serial.println(max30003.RRinterval);
      }
    }
    max30003.getEcgSamples();   //It reads the ecg sample and stores it to max30003.ecgdata .

    Serial.println(max30003.ecgdata);
    delay(8);
    
    sendDataThroughUART();
    delay(8);
}



/// temp
