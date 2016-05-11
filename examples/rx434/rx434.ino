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

const byte rxPIN = 11; //Physical pin on which radio DATA pin is connected.
const byte address = 0xAB; //Address on which radio will listen to.

RF434 rx(0, rxPIN, address); //TX pin, RX pin, RX address
//As we are not sendig data via TX, this pin was set to 0.

void setup(){ 
  pinMode(RED_LED, OUTPUT); //Just a LED that flashes when valid data is arriving
  digitalWrite(RED_LED, 0);
  Serial.begin(9600);
  Serial.println("||||||||||||");
  Serial.println("| Setup RX |");
  Serial.println("||||||||||||");
  rx.rfTxSetup(500); //500 bps. This is the optimal baudrate for these ASK radios
}

void loop(){
  byte dt=0;  //Radio input buffer
  dt = rx.rfRx(0); //Receive data with 0 seconds of timeout (blocking action)
  //You may try using rx.rfRx(n) if you need non-blocking effect (n is an integer in seconds)
  if(dt>0){
    digitalWrite(RED_LED, 1); //Hey, I found a packet that was sent to my address!
    Serial.println(dt);  //Print the received data
    digitalWrite(RED_LED, 0);

  }
}
