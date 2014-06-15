/*
  B00 Protocol Sender Library for Arduino
  =======================================

  For the latest version, visit the project home at :
  https ://github.com/graemewieden/B00_Protocol

  For full technical details see readme.md

  Released under the MIT license.
  See license.md for more information.
*/

#ifndef B00Sender_h
#define B00Sender_h

#include "Arduino.h"

// define DEBUG to send output to the serial monitor rather than transmitting
// #define DEBUG

#define SHORTPULSE 300 // time in microseconds for a short pulse
#define LONGPULSE 900 // time in microseconds for a long pulse (ShortPulse * 3)
#define SYNCPULSE 9300 // trigger time in microseconds (ShortPulse * 31)

// note that the trigger pulse duration must be less than 16383 which is the
// maximum allowed by the Arduino delayMicroseconds function

class B00Sender
{
public:
  B00Sender(); // constructor for creating with default settings
  B00Sender(byte txPin, byte houseCode, byte channel); // constructor for custom settings
  void setup(byte txPin, byte houseCode, byte channel); // for changing settings after construction
  void setTxPin(byte txPin);
  void setHouseAndChannel(byte houseCode, byte channel);
  void setRepeats(byte repeats);
  void sendB00(double value);
  void sendB01(long value);
  void sendB02(unsigned long value);
  void sendB03(int valueA, int valueB);
  void sendB04(unsigned int valueA, unsigned int valueB);
  void sendB05(byte b1, byte b2, byte b3, byte b4);

protected:
  byte txPin; // digital pin for transmitter
  byte houseCode; // valid house codes are 0 to 3
  byte channel; // valid channels are 0 to 7
  boolean parityBit; // used to maintain the parity state as we send bits in the code word
  byte repeats; // number of times code word is repeated

  void sendCodeWord(byte, unsigned long value);
  void sendSyncSignal();
  void sendBits(unsigned long data, unsigned int bits);
  void sendBit(byte b); // sends output to the tx pin
#ifdef DEBUG
  void debugBit(byte b); // sends output to the serial monitor
#endif
};

#endif