/*
RF 434 Library for Energia with MSP430 Launchpad
Ivan Rene Morales Argueta (2014)
ivan[at]fisica[dot]usac[dot]edu[dot]gt
Universidad de San Carlos de Guatemala
EE School
Electronics Lab
*/

/* READ THIS FIRST */

/*This library is intended to be used in a single- or multiple-link radio
environment, so you may use as much radios as you need in your project
without interfering with each other. Just be sure to setup the addresses
correctly, and you will have no problems.

I made this because (at least, by now) the VirtualWire library port to Energia
didn't work at all, and I really needed to create reliable radio links with these
cheap ASK Radios. The data radio formatting was done based on the knowledge
acquired from this paper: http://www.rfm.com/products/apnotes/tr_swg05.pdf. Despite
this is not a recent document, it highlights clue parameters for you to learn
how to design the firmware to drive your radios.
*/

/*
HOW TO USE THIS LIBRARY:

Data formatting for RF transmission and redundance check is done automatically
and you don't have to take care about it. Just send the data like if it were
a UART channel.

You can even have multiple Tx and Rx radios in a single board: just create
as many instances of RF434 objects as you need. Be careful with the addresses.

As this library was enterely written in Wiring/C++ and no MSP430 special features
were used, it may work well with other microcontrollers in Energia, like
Fraunchpad, Stellaris, Tiva C, etc. I haven't tested it yet, but if you do,
please make me know about it.

Also, if you have the chance/need to use 315 MHz ASK or 433MHz FSK (or any other 
radios which send the raw data as you send it from the microcontroller) try it out,
because it should work. Please, notify me if you do so. However, if it doesn't work,
please correct it, and send me the updated code :P

---------------------------------------------------------------------------------

ADVICE: If you are in a multiple radio environmnet, be sure to setup
the "Number of retries" parameter to 8 or more (check the "rf.rxTx"
method at the end of this example to know what am I referring to).
The more radios you have, the more retries you may use to ensure reliability.

ALWAYS attatch an antenna to the radios. You'll notice this significantly
increases the coverage range. A 17 cm wire is ok for 433 Mhz. Also, notice that
the higher the voltage (VCC) you apply to the Tx radio, the higher the Tx Power.
Read the datasheets for further details.
*/

#include <RF434.h>

const byte txPIN = 10; //Physical pin on which radio DATA pin is connected.
const byte address1 = 0xAB; //Destination addresses
const byte address2 = 0xAC;

RF434 tx(txPIN, 0, 0x01); //(TX PIN, RX PIN, RX Address)
//In this case, we aren't receiving data, so RX address doesn't matter


void setup(){
  tx.rfTxSetup(500); //Setup TX Radio to work with 500 bps.
  /*I've found that 500 bps is the most efficient transmission rate
  for these cheap modules if you want your data to travel the farthest.*/
  pinMode(PUSH2, INPUT_PULLUP); //Press the button to send data
  pinMode(GREEN_LED, OUTPUT); //BUSY indicator
 
  digitalWrite(GREEN_LED, 0);
}

void loop(){
  byte dt = 55; //Sample data to be sent. You may send data splitted in bytes
  while(digitalRead(PUSH2)) //Polling PUSH BUTTON.
    ;
  digitalWrite(GREEN_LED, 1); // I'm sending DATA now: BUSY.
  tx.rfTx(address1, dt, 10); //(Destination Address, DATA, number of retries)
  tx.rfTx(address2, dt, 10); //You may even multicast to multiple addresses
  digitalWrite(GREEN_LED, 0); //I'm done!
}
