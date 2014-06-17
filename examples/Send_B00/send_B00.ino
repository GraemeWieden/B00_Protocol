/*
	B00 Protocol Sample Data Sender Sketch
	======================================

	For the latest version, visit the project home at :
	https://github.com/graemewieden/B00_Protocol

	Released under the MIT license.
	See license.md for more information.

	This sample data sender utility continually sends data using the B00 protocol
	All supported content types are used. These are as follows:

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

#include <B00Sender.h>

byte ledPin = 13; // digital pin for LED
byte txPin = 9; // default digital pin for transmitter
byte houseCode = 0;  // default house code
byte channel = 0;  // default channel

// construct the B00Sender object with default settings
B00Sender sender;

void setup()
{
  // set up the B00Sender object in case non-default values are desired
  sender.setup(txPin, houseCode, channel);
  
  pinMode(ledPin, OUTPUT);  
}

void loop()
{
  // double values can range from -3.4028235E+38 to 3.4028235E+38
  // but only have a maximum of 6 to 7 decimal digits of precision

  double testDouble = 12345.67;
  sender.sendB00(testDouble);
  blinkAndPause();

  testDouble = -9876.543;
  sender.sendB00(testDouble);
  blinkAndPause();

  // long values can range from -2,147,483,648 to 2,147,483,647

  long testLong = 123456789;
  sender.sendB01(testLong);
  blinkAndPause();

  testLong = -987654321;
  sender.sendB01(testLong);
  blinkAndPause();

  // unsigned long values can range from 0 to 4,294,967,295

  unsigned long testUnsignedLong = 1234567890;
  sender.sendB02(testUnsignedLong);
  blinkAndPause();

  // int values can range from -32,768 to 32,767

  int testA = -32123;
  int testB = 32123;
  sender.sendB03(testA, testB);
  blinkAndPause();

  // unsigned int values can range from 0 to 65,535

  unsigned int testC = 0;
  unsigned int testD = 65432;  
  sender.sendB04(testC, testD);
  blinkAndPause();

  // byte values can range from 0 to 255

  sender.sendB05(B10101010, B11001100, B11110000, B11111111);
  blinkAndPause();  

  delay(5000);
}

void blinkAndPause()
{
  // blink LED
  digitalWrite(ledPin, HIGH);
  delay(50);   
  digitalWrite(ledPin, LOW);
  delay(1000);
}





