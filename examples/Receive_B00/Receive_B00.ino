// Important note:
// The B00 protocol uses a 50 bit packet so it needs the modified RCSwitch library that
// supports up to 64 bits rather than the standard version which is limited to 32 bits.

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() 
{
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on inerrupt 0 => that is pin #2
  mySwitch.setProtocol(1,300);
}

void loop() 
{
  if (mySwitch.available()) 
  {
    mySwitch.disableReceive();
    // we have a B00 packet if it is protocol 1, 50 bits long, starts with B0 and ends with an even parity bit
    // for additional checking, we could also test getReceivedDelay for about 300 microseconds

    if (mySwitch.getReceivedProtocol() == 1 && mySwitch.getReceivedBitlength() == 50)
    {
      //Serial.println("got 50 bits via protocol 1");
      unsigned long long value = mySwitch.getReceivedValue();
      byte announce  = value >> 42;
      if(announce == 0xB0)
      {
        //Serial.println("got B00 announcement");
        if(parityOk(value))
        {
          //Serial.println("parity OK");
          decodeValue(value);
        }
      }
    }
    mySwitch.enableReceive(0);
    mySwitch.resetAvailable();
  }
}

boolean parityOk(unsigned long long value)
{
  boolean checkParity = value & 1;
  boolean runningParity = 0;

  for (int i = 1; i < 50; i++)
  {
    value = value >> 1;
    if(value & 1)
      runningParity = !runningParity;
  }
  return (runningParity == checkParity);
}

void decodeValue(unsigned long long value)
{
  byte contentType = value >> 38 & 0xF;
  byte houseCode = value >> 36 & 0x3;
  byte channelCode = value >> 33 & 0x7;
  unsigned long content = value >> 1 &0xFFFFFFFF;

  // we create a union of a double and unsigned long so we can use the bit pattern 
  // without modification between data types
  union
  {
    double dblValue;
    unsigned long ulValue;
  };
  ulValue = content;
  

  Serial.print("House Code: ");
  Serial.print(houseCode);
  Serial.print(" Channel: ");
  Serial.println(channelCode);  

  Serial.print("Content Type ");
  Serial.print(contentType);  
  Serial.print(": ");
      
  switch (contentType) 
  {
  case 0: // double precision floating point value
    Serial.println(dblValue);    
    break;
  case 1: // long signed integer value
    Serial.println((long)content);  
    break;
  case 2: // long unsigned integer value
    Serial.println(content);  
    break;
  case 3: // 2 x signed integer values
    Serial.print((int)(content >> 16)); 
    Serial.print(", ");
    Serial.println((int)(content & 0xFFFF));       
    break;
  case 4: // 2 x unsigned integer values
    Serial.print((unsigned int)(content >> 16)); 
    Serial.print(", ");
    Serial.println((unsigned int)(content & 0xFFFF));   
    break;
  case 5: // 4 x byte values
    Serial.print(content >> 24, HEX); 
    Serial.print(" ");
    Serial.print(content >> 16 & 0xFF, HEX); 
    Serial.print(" ");    
    Serial.print(content >> 8 & 0xFF, HEX);     
    Serial.print(" ");    
    Serial.println(content & 0xFF, HEX);  
    break;
  }
  
}
