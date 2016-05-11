#include "RF434.h"

const byte symbols[16] = 
{0x0d, 0x0e, 0x13, 0x15, 0x16, 0x19, 0x1a, 0x1c, 
0x23, 0x25, 0x26, 0x29, 0x2a, 0x2c, 0x32, 0x34};

const byte MAX_SAMPLES_PER_BIT = 8;
const byte DELAYS_BETWEEN_RESENT = 50;

/*byte txPin;
byte rxPin;
byte Address;

unsigned int delayus;
unsigned int addr6B;*/

RF434::RF434(byte pinTx, byte pinRx, byte rxAddress){
	txPin    = pinTx;
	rxPin    = pinRx;
	address  = rxAddress;
	pinMode(pinTx, OUTPUT);
	pinMode(pinRx, INPUT);
}

RF434::~RF434(){
	;
}


byte RF434::conv4B6B(byte nibble4B){
  return symbols[nibble4B];
}

byte RF434::conv6B4B(byte nibble6B){
  for(int i=0; i<16; i++){
    if(nibble6B == symbols[i]){
      return i;
    }
  }
  return 0;
}

unsigned int RF434::encodeByteTo12Bits(byte dt){
  unsigned int dt6B = 0;
  byte dtNibbles[2] = {0, 0};
  for(int x=0; x<4; x++){
    dtNibbles[0] += bitRead(dt, x) * bit(x);
    dtNibbles[1] += bitRead(dt, x+4) * bit(x);
  }
  dt6B = ((conv4B6B(dtNibbles[0]) + conv4B6B(dtNibbles[1])*64)&0xFFF);
  return dt6B;  
}

unsigned int RF434::decode12BitsToByte(unsigned int dt){
  byte dtNibbles6[2]  = {0, 0};
  for(int x=0; x<6; x++){
    dtNibbles6[0] += bitRead(dt, x)*bit(x);
    dtNibbles6[1] += bitRead(dt, x+6)*bit(x);
  }
  return conv6B4B(dtNibbles6[0]) + conv6B4B(dtNibbles6[1])*16;
}

void RF434::rfTxSetup(unsigned int baudrate){
  delayus = 1000000/baudrate;
  pinMode(txPin, OUTPUT);
  digitalWrite(txPin, 0);
  addr6B = encodeByteTo12Bits(address);
}

void RF434::rfRxSetup(unsigned int baudrate){
  delayus = 1000000/baudrate;
  pinMode(rxPin, INPUT);
  addr6B = encodeByteTo12Bits(address);
}

byte RF434::computeRedundancy(byte addr, byte data){
  return addr^data;
}

boolean RF434::checkRedundancy(byte addr, byte data, byte red){
  return (((addr^data)==red)&&(addr==address)) ? true : false;
}

boolean RF434::rfTx(byte addr, byte data, byte repeats){
  byte redundancy;
  
  unsigned int data6B = 0;
  unsigned int  red6B = 0;
  
  unsigned int dataPacket[3];

  redundancy = computeRedundancy(addr, data);
     
  data6B = encodeByteTo12Bits(data);
  red6B  = encodeByteTo12Bits(redundancy);
  
  dataPacket[0] = addr6B;
  dataPacket[1] = data6B;
  dataPacket[2] = red6B;
  
  if(!repeats)
    repeats = 1;
  

  for(int k=0; k<repeats; k++){
    for(int i=0; i<3; i++){
      //delayMicroseconds(delayus/2);
      for(int j=0; j<12; j++){
        digitalWrite(txPin, bitRead(dataPacket[i],j));
        delayMicroseconds(delayus);
      }
    }
    digitalWrite(txPin, 0);
    delayMicroseconds(DELAYS_BETWEEN_RESENT*delayus);
  }
  
  return true; //Will return a TRUE at the end of transmission
}

boolean RF434::readSample(){
  return digitalRead(rxPin);
}

boolean RF434::readAndIntegrateBit(){
  byte sum = 0;
  byte delaybit = delayus/MAX_SAMPLES_PER_BIT;
  for(int i = 0; i < MAX_SAMPLES_PER_BIT; i++){
    sum += readSample();
    delayMicroseconds(delaybit);
  }
  return (sum >= (MAX_SAMPLES_PER_BIT/2)) ? true : false;
}

boolean RF434::lookUpAddress(unsigned int timeOut){
  unsigned int recvData = 0;
  timeOut *= 1000; //User will input seconds, but we need millis.
  
  if(!timeOut){
    while(addr6B != recvData){
      recvData >>= 1;
      bitWrite(recvData, 11, readAndIntegrateBit());
    }
    return true;
  }else{
    unsigned long startTime = millis();
    unsigned long elapsedTime = startTime;
    while((addr6B != recvData) && (elapsedTime-startTime <= timeOut)){
      recvData >>= 1;
      bitWrite(recvData, 11, readAndIntegrateBit());
      elapsedTime = millis();
    }
    if(recvData == addr6B){
      return true;
    }else{
      return false;
    }
  }
}

unsigned long RF434::readDataAndRed6B(){
  unsigned long recvData = 0;
  for(int x=0; x<24; x++){
    bitWrite(recvData, x, readAndIntegrateBit());    
  }
  return recvData;
}

byte RF434::rfRx(unsigned int timeOut){
  unsigned long dataAndRed = 0;
  unsigned int data6B = 0;
  unsigned int red6B  = 0;
  byte data = 0;
  byte red  = 0;
  
  if(lookUpAddress(timeOut)){  
    
    dataAndRed = readDataAndRed6B();
    for(int x=0; x<12; x++){
      data6B += bitRead(dataAndRed,x)*bit(x);
      red6B += bitRead(dataAndRed,x+12)*bit(x);
    }
    
    data = decode12BitsToByte(data6B);
    red  = decode12BitsToByte(red6B);
      
    return checkRedundancy(address, data, red)?data:0;  
    
  }else{
    
    return NULL;

  }  
}