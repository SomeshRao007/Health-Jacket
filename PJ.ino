//////////////////////////////////////////////////////////////////////////////////////////
//
//    Demo code for the MAX30003 breakout board
//
//    This example plots the ECG through serial UART on openview processing GUI.
//    GUI URL: https://github.com/Protocentral/protocentral_openview.git
//
//    Arduino connections:
//
//  |MAX30003 pin label| Pin Function         |Arduino Connection|
//  |----------------- |:--------------------:|-----------------:|
//  | MISO             | Slave Out            |  D12             |
//  | MOSI             | Slave In             |  D11             |
//  | SCLK             | Serial Clock         |  D13             |
//  | CS               | Chip Select          |  D7              |
//  | VCC              | Digital VDD          |  +5V             |
//  | GND              | Digital Gnd          |  Gnd             |
//  | FCLK             | 32K CLOCK            |  -               |
//  | INT1             | Interrupt1           |  02              |
//  | INT2             | Interrupt2           |  -               |
//
//    This software is licensed under the MIT License(http://opensource.org/licenses/MIT).
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//   NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//   For information on how to use, visit https://github.com/Protocentral/protocentral_max30003

/*
connection for temperature on terminal

Hardware Connections (Breakoutboard to Arduino):
Vin  - 5V (3.3V is allowed)
GND - GND
SDA - A4 (or SDA)
SCL - A5 (or SCL)

*/

/////////////////////////////////////////////////////////////////////////////////////////

#include<SPI.h>
#include <Wire.h>
#include "protocentral_Max30003.h"
#include "Protocentral_MAX30205.h"


#define CES_CMDIF_PKT_START_1   0x0A
#define CES_CMDIF_PKT_START_2   0xFA
#define CES_CMDIF_TYPE_DATA     0x02
#define CES_CMDIF_PKT_STOP      0x0B
#define DATA_LEN                0x0C
#define ZERO                    0
#define INT_PIN 02
MAX30205 tempSensor;
MAX30003 max30003;
volatile char DataPacket[DATA_LEN];
const char DataPacketFooter[2] = {ZERO, CES_CMDIF_PKT_STOP};
const char DataPacketHeader[5] = {CES_CMDIF_PKT_START_1, CES_CMDIF_PKT_START_2, DATA_LEN, ZERO, CES_CMDIF_TYPE_DATA};

uint8_t data_len = 0x0C;

bool rtorIntrFlag = false;
uint8_t statusReg[3];

void rtorInterruptHndlr(){
  rtorIntrFlag = true;
}

void enableInterruptPin(){

  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), rtorInterruptHndlr, CHANGE);
}

void sendDataThroughUART(void){

  DataPacket[5] = max30003.ecgdata;
  DataPacket[6] = max30003.ecgdata>>8;
  DataPacket[7] = max30003.ecgdata>>16;
  DataPacket[8] = max30003.ecgdata>>24;

  DataPacket[9] =  max30003.RRinterval ;
  DataPacket[10] = max30003.RRinterval >>8;
  DataPacket[11] = 0x00;
  DataPacket[12] = 0x00;

  DataPacket[13] = max30003.heartRate ;
  DataPacket[14] = max30003.heartRate >>8;
  DataPacket[15] = 0x00;
  DataPacket[16] = 0x00;

  //send packet header
  for(int i=0; i<5; i++){

    Serial.write(DataPacketHeader[i]);
  }

  //send 30003 data
  for(int i=0; i<DATA_LEN; i++) // transmit the data
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
   // Serial.begin(57600); 
    Serial.begin(115200);

    pinMode(15,OUTPUT);
    digitalWrite(15,HIGH); //disable device

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
    enableInterruptPin();
    max30003.max30003RegRead(STATUS, statusReg);


   Serial.begin(115200);
    Wire.begin();

       //scan for temperature in every 30 sec untill a sensor is found. Scan for both addresses 0x48 and 0x49
     while(!tempSensor.scanAvailableSensors()){
     Serial.println("Couldn't find the temperature sensor, please connect the sensor." );
     delay(30000);
  }

  tempSensor.begin();   // set continuos mode, active mode
}

void loop()
{
    max30003.getEcgSamples();   //It reads the ecg sample and stores it to max30003.ecgdata .
    Serial.println(max30003.ecgdata);
    delay(8);
    //max30003.getHRandRR();   //It will store HR to max30003.heartRate and rr to max30003.RRinterval.

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

    sendDataThroughUART();
    delay(8);

    
  float temp = tempSensor.getTemperature(); // read temperature for every 100ms
  Serial.print(temp ,2);
  Serial.println("'c" );
  delay(100);
}
