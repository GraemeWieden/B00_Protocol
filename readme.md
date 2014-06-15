B00 Protocol Sender Library for Arduino
=======================================

For the latest version, visit the project home at :
https://github.com/graemewieden/B00_Protocol

Released under the MIT license.
See license.md for more information.


The B00 protocol is a custom, general purpose protocol for sending sensor data of various data types via RF between Arduinos.

It's called B00 as it says Boo! to announce itself

The code word is a total of 50 bits long and is constructed like this:

- 12 bits: B00 to announce and content descriptor (see below)
- 5 bits: 2 bit house code 3 bit channel code
- 32 bits: content
- 1 bit: even parity

Content Descriptor
------------------
The B00 protocol is designed to be extensible such that additional content payloads can be supported in future. The content descriptor bits identify the type of data contained in the code word.

At present, the content descriptor defines these content types:
###B00:
- 32 bits: double precision floating point value

###B01: 
- 32 bits: signed long integer value

###B02: 
- 32 bits: unsigned long integer value

###B03: 
- 16 bits: signed integer value 1
- 16 bits: signed integer value 2

###B04: 
- 16 bits: unsigned integer value 1
- 16 bits: unsigned integer value 2

###B05: 
- 8 bits: byte value 1
- 8 bits: byte value 2
- 8 bits: byte value 3
- 8 bits: byte value 4

Features
--------
As well as the content descriptor and the content itself, the B00 Protocol code word also includes a 2 bit house code (0 to 3) and a 3 bit channel code (0 to 7).

The final bit of the code word is an even parity bit to assist in ensuring data integrity.

By default, the B00Sender object will repeat the code word 4 times, however this can be changed using the setRepeats function.

Encoding
--------
The RF encoding scheme used by the B00 protocol is a reasonably 'standard' type where zero is represented as short high - long low and one is represented as long high - short low. 

The short pulse is one quarter of the bit duration and the long pulse is three quarters. The actual pulse durations are derived from the short pulse length which is 300 microseconds. The long pulse length is 3 times the length of the short pulse (900 microseconds). The sync pulse length is 31 times the length of the short pulse length (9.3 milliseconds).

This produces pulse durations of

- 9.3 ms sync pulse
- 0.9 ms long pulse
- 0.3 ms short pulse

The B00 RF protocol mirrors the system defined as 'Protocol 1' in the Arduino RCSwitch library, however the base version of the RCSwitch library can't be used as it only supports up to 32 bit packets.

If you intend using RCSwitch to decode the B00 packet, be sure to use the modified version which supports up to 64 bit packets. It can be found here:
https://github.com/GraemeWieden/RCSwitch_64


Installation
------------
Place (or symbolic link) the B00Sender directory into your Arduino `libraries` directory located within your Arduino sketchbook directory.

Constructing the B00Sender object
-----------------------------------
Two constructors are provided:

```
  B00Sender();
```

```
  B00Sender(byte txPin, byte houseCode, byte channel);
```

The first constructor sets up the object with default paramaters of:

- Transmit pin 9
- House Code 0
- Channel 0
- Repeats 4

The second constructor allows you to specify the transmit pin, the house code and the channel as arguments. If you use the first constructor, you can also manually setup these values with the `setup` function: 

```
void setup(byte txPin, byte houseCode, byte channel);
```

The number of code word repeats can also be manually specified using:

```
void setRepeats(byte repeats);
```

Sending the data
----------------

By default, the data packet will be sent 4 times. This can be changed using the `setRepeats` function:

```
void setRepeats(byte repeats);
```

Depending on the type of data you need to transmit, use the following functions:

```
void sendB00(double value);
void sendB01(long value);
void sendB02(unsigned long value);
void sendB03(int valueA, int valueB);
void sendB04(unsigned int valueA, unsigned int valueB);
void sendB05(byte b1, byte b2, byte b3, byte b4);
```

Examples
--------
For a complete example of how to send and receive / decode the B00 packet, see the examples here:
https://github.com/GraemeWieden/B00_Protocol/tree/master/examples
