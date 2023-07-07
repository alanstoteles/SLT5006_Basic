/*
  Name: SLT5006_Basic
  Author: Alan Tejeda Ramirez
  email: atejeda-ramirez@murata.com
  Description: This a basic script to read the Murata SLT5006 Soil Sensor vairables, for Debug/Manual please uncoment ManualMode() function.
               For debug_lvl:
               0 -  Only print measurements from the sensor
               1 -  Print results of each command sent and measurements from the sensor
               2 -  Manual Mode 
   
*/

#include <SoftwareSerial.h>
const byte rxPin = 3;
const byte txPin = 2;
SoftwareSerial mySerial (rxPin, txPin);

byte response[6];
byte meas_data[21]; 
int i;
byte meas_start[]       = {0x02, 0x07, 0x01, 0x01, 0x0D, 0x70};
byte read_meas_status[] = {0x01, 0x08, 0x01, 0x00, 0xE6};
byte read_meas_data[]   = {0x01, 0x13, 0x10, 0xFC, 0x2C};

double      _temp;          /// [℃]
double      _BlukEC;        /// Bulk_EC[dS/m]
double      _VWC;           /// [%]
double      _PoreEC;        /// PoreEC[dS/m] 

int debug_lvl=0; //Default, no debug

void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

 // Set the baud rate for the SoftwareSerial object
 mySerial.begin(9600); // opens SW serial port, sets data rate to 9600 bps
 
}

void p(byte x) {
   if (x < 10) {Serial.print("0");}
   Serial.println(x, HEX);
}


void ManualMode(){
  Serial.println("Debug Mode Enabled, please write manually your commands, use HTERM instead of Serial Monitor:");
  while(1){
    if (mySerial.available()) {
      Serial.write(mySerial.read());
    }
    if (Serial.available()) {
      mySerial.write(Serial.read());
    }  
  }
}


bool start_meas(int debug_lvl){
  Serial.println("Measurement Starting...");
  int state = 0;
  while (state == 0) {
      delay(11); //EN "start" ～ FW "start" time
      mySerial.flush();
      mySerial.write(meas_start, sizeof(meas_start));
      delay(50); //response wait time + response time + additional 12ms as safe time
      
      if(mySerial.available()>=6) {
        for (i=0; i<6; i++) {
          response[i] = mySerial.read();
        }
        if (debug_lvl == 1){
          Serial.println("Response: ");
          p(response[0]);
          p(response[1]);
          p(response[2]);
          p(response[3]);
          p(response[4]);
          p(response[5]);
          //Serial.println("");
        }
        memset(response, 0, sizeof(response));
      }
      //if all works
      state=1;
  }
  Serial.println("-------------");
  mySerial.flush();
  return 1;
}

bool read_stat(int debug_lvl){
  Serial.println("Reading Measurement Status...");
  int state = 1;
  while (state == 1) {
    //delay(3000);
    mySerial.write(read_meas_status, sizeof(read_meas_status));
    delay(50); //response wait time + response time + additional 12ms as safe time
    if(mySerial.available()>=6) {
      for (i=0; i<6; i++) {
        response[i] = mySerial.read();
      }
      if (debug_lvl == 1){
        Serial.println("Response: ");
        p(response[0]);
        p(response[1]);
        p(response[2]);
        p(response[3]);
        p(response[4]);
        p(response[5]);
        //Serial.println("");      
      }
    }
    //Validate if measurement is incomplete
    if (response[3]!=0){
      memset(response, 0, sizeof(response));
      state=2;
    }
    else{
      state=1;
    }
  }
  Serial.println("-------------");
  mySerial.flush();
  return 1;
}

bool read_data(int debug_lvl){
      //delay(3000);
    Serial.println("Reading Measurement Data...");
    mySerial.write(read_meas_data, sizeof(read_meas_data));
    //delay(3000);
    delay(50); //response wait time + response time + additional 12ms as safe time
    if(mySerial.available()>=21) {
      for (i=0; i<21; i++) {
        meas_data[i] = mySerial.read();
      }
      if (debug_lvl == 1){
        Serial.println("Response: ");
        p(meas_data[0]);
        p(meas_data[1]);
        p(meas_data[2]);
        p(meas_data[3]);
        p(meas_data[4]);
        p(meas_data[5]);
        p(meas_data[6]);
        p(meas_data[7]);
        p(meas_data[8]);
        p(meas_data[9]);
        p(meas_data[10]);
        p(meas_data[11]);
        p(meas_data[12]);
        p(meas_data[13]);
        p(meas_data[14]);
        p(meas_data[15]);
        p(meas_data[16]);
        p(meas_data[17]);
        p(meas_data[18]);
        p(meas_data[19]);
        p(meas_data[20]);
        Serial.println("");  
      }  
    }
    
    //if all works
   _temp   = (meas_data[4] << 8) + meas_data[3];
   _BlukEC = (meas_data[6] << 8) + meas_data[5];
   _VWC    = (meas_data[10] << 8) + meas_data[9];
   _PoreEC = (meas_data[16] << 8) + meas_data[15];
   
   _temp    = _temp * 0.0625;
   _BlukEC  = _BlukEC * 0.001;
   _VWC     = _VWC * 0.1;
   _PoreEC  = _PoreEC * 0.001;
   
   Serial.print("Temp: ");Serial.print(_temp); Serial.println(" °C");
   Serial.print("BlukEC: ");Serial.print(_BlukEC); Serial.println(" dS/m");
   Serial.print("VWC: ");Serial.print( _VWC); Serial.println(" %");
   Serial.print("PoreEC: ");Serial.print(_PoreEC); Serial.println(" dS/m");
   //delay(3000);
   memset(meas_data, 0, sizeof(meas_data));

  Serial.println("-------------");
  mySerial.flush();
  return 1;
  }

void loop() {

  if (debug_lvl==2){
    ManualMode();
  }
  else{
    if(start_meas(debug_lvl));
    if(read_stat(debug_lvl));
    if(read_data(debug_lvl));
  }
}
