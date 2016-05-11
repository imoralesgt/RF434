#ifndef RF434_H
#define RF434_H

#include <Energia.h>

class RF434{
	public:
		RF434(byte pinTx, byte pinRx, byte rxAddress);
		~RF434();
		void rfTxSetup(unsigned int baudrate);
		void rfRxSetup(unsigned int baudrate);
		boolean rfTx(byte addr, byte data, byte repeats);
		byte rfRx(unsigned int timeOut);
		
	private:
		byte conv4B6B(byte nibble4B);
		byte conv6B4B(byte nibble6B);
		unsigned int encodeByteTo12Bits(byte dt);
		unsigned int decode12BitsToByte(unsigned int dt);
		byte computeRedundancy(byte addr, byte data);
		boolean checkRedundancy(byte addr, byte data, byte red);
		boolean readSample();
		boolean readAndIntegrateBit();
		boolean lookUpAddress(unsigned int timeOut);
		unsigned long readDataAndRed6B();
	
		byte txPin;
		byte rxPin;
		byte address;

		unsigned int delayus;
		unsigned int addr6B;
};

#endif