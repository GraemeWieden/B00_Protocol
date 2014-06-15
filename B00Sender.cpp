/*
  B00 Protocol Sender Library for Arduino
  =======================================

  For the latest version, visit the project home at :
  https ://github.com/graemewieden/B00_Protocol

  For full technical details see readme.md

  Released under the MIT license.
  See license.md for more information.

  The B00 RF protocol is called B00 as it says Boo! to announce itself

  The code word is a total of 50 bits long and is constructed like this:

  12 bits: the content descriptor - default is B00
  5 bits: 2 bit house code - 3 bit channel code
  32 bits: content - see below
  1 bit: even parity bit

  Content descriptor currently defines these content types:
  B00:
  32 bits: double precision floating point value
  B01:
  32 bits: signed long integer value
  B02:
  32 bits: unsigned long integer value
  B03:
  16 bits: signed integer value 1
  16 bits: signed integer value 2
  B04:
  16 bits: unsigned integer value 1
  16 bits: unsigned integer value 2
  B05:
  8 bits: byte value 1
  8 bits: byte value 2
  8 bits: byte value 3
  8 bits: byte value 4
*/

#include "B00Sender.h"

B00Sender::B00Sender()
{
  // default setup txPin set to 9, House and Channel codes both set to 0
  setup(9, 0, 0);
}

B00Sender::B00Sender(byte txPin, byte houseCode, byte channel)
{
  setup(txPin, houseCode, channel);
}

void B00Sender::setup(byte txPin, byte houseCode, byte channel)
{
  // by default repeat the codeword 4 times
  setRepeats(4);
  setTxPin(txPin);
  setHouseAndChannel(houseCode, channel);

#ifdef DEBUG
  Serial.begin(9600);
#endif
}

void B00Sender::setTxPin(byte txPin)
{
  this->txPin = txPin;
  pinMode(txPin, OUTPUT);
}

void B00Sender::setHouseAndChannel(byte houseCode, byte channel)
{
  this->houseCode = houseCode;
  this->channel = channel;
}

void B00Sender::setRepeats(byte repeats)
{
#ifdef DEBUG
  this->repeats = 1;
#else
  this->repeats = repeats;
#endif

}

//----------------------------------------------------------------
// double values can range from -3.4028235E+38 to 3.4028235E+38
// but only have a maximum of 6 to 7 decimal digits of precision
//----------------------------------------------------------------

void B00Sender::sendB00(double value)
{
#ifdef DEBUG
  Serial.println("B00 : double");
#endif
  // we have a special case here to convert the double to an unsigned it without casting.
  // to do this, we create a union of the double and an unsigned long. this means they share
  // the same memory and so we can treat them as the same bit pattern
  union
  {
    double dblValue;
    unsigned long ulValue;
  };
  dblValue = value;
  sendCodeWord(0, ulValue);
}

//----------------------------------------------------------------
// long values can range from -2,147,483,648 to 2,147,483,647
//----------------------------------------------------------------

void B00Sender::sendB01(long value)
{
#ifdef DEBUG
  Serial.println("B01 : long");
#endif

  sendCodeWord(1, value);
}

//----------------------------------------------------------------
// unsigned long values can range from 0 to 4,294,967,295
//----------------------------------------------------------------

void B00Sender::sendB02(unsigned long value)
{
#ifdef DEBUG
  Serial.println("B02 : unsigned long");
#endif

  sendCodeWord(2, value);
}

//----------------------------------------------------------------
// int values can range from -32,768 to 32,767
//----------------------------------------------------------------

void B00Sender::sendB03(int valueA, int valueB)
{
#ifdef DEBUG
  Serial.println("B03 : int int");
#endif

  unsigned long value = valueA;
  value = value << 16 | valueB;
  sendCodeWord(3, value);
}

//----------------------------------------------------------------
// unsigned int values can range from 0 to 65,535
//----------------------------------------------------------------

void B00Sender::sendB04(unsigned int valueA, unsigned int valueB)
{
#ifdef DEBUG
  Serial.println("B04 : unsigned int unsigned int");
#endif

  unsigned long value = valueA;
  value = value << 16 | valueB;
  sendCodeWord(4, value);
}

//----------------------------------------------------------------
// byte values can range from 0 to 255
//----------------------------------------------------------------

void B00Sender::sendB05(byte b1, byte b2, byte b3, byte b4)
{
#ifdef DEBUG
  Serial.println("B05 : byte byte byte byte");
#endif

  unsigned long value = b1;
  value = value << 8 | b2;
  value = value << 8 | b3;
  value = value << 8 | b4;
  sendCodeWord(5, value);
}

void B00Sender::sendCodeWord(byte contentType, unsigned long value)
{
  sendSyncSignal();

  for (int i = 0; i < repeats; i++)
  {
    // parityBit is reset to zero by writing the parity bit
    sendBits(0xB, 4); // announce!
    sendBits(contentType, 8); // default 00
    sendBits(houseCode, 2); // 0-3
    sendBits(channel, 3); // 0-7
    sendBits(value, 32); // content dependent
    sendBits(parityBit, 1); // even parity
    sendSyncSignal();
  }
#ifdef DEBUG
  Serial.println();
#endif
}

void B00Sender::sendSyncSignal()
{
  digitalWrite(txPin, HIGH);
  delayMicroseconds(SHORTPULSE);
  digitalWrite(txPin, LOW);
  delayMicroseconds(SYNCPULSE);
}

void B00Sender::sendBits(unsigned long data, unsigned int bits)
{
  // bits must be no more than 32
  unsigned long bitMask = 1;
  bitMask = bitMask << (bits - 1);
  for (int i = 0; i < bits; i++)
  {
#ifdef DEBUG
    debugBit((data&bitMask) == 0 ? 0 : 1);
#else
    sendBit((data&bitMask) == 0 ? 0 : 1);
#endif

    bitMask = bitMask >> 1;
  }
}

void B00Sender::sendBit(byte b)
{
  if (b == 0)
  {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(SHORTPULSE);
    digitalWrite(txPin, LOW);
    delayMicroseconds(LONGPULSE);
  }
  else // any bit in the byte is on
  {
    parityBit = !parityBit;
    digitalWrite(txPin, HIGH);
    delayMicroseconds(LONGPULSE);
    digitalWrite(txPin, LOW);
    delayMicroseconds(SHORTPULSE);
  }
}

#ifdef DEBUG
void B00Sender::debugBit(byte b)
{
  if (b == 0)
    Serial.print("0");
  else
  {
    Serial.print("1");
    parityBit = !parityBit;
  }
}
#endif





