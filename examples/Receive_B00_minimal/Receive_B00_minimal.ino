/*
  B00 Protocol Minimal Receiver Sketch
  ====================================
 
  For the latest version, visit the project home at :
  https://github.com/graemewieden/B00_Protocol
 
  Released under the MIT license.
  See license.md for more information.
 
  This version of the example B00 protocol receiver does not rely on the RCSwitch library.
  Instead, it includes a compact, interrupt based decoder for the protocol.
*/
 
#define SHORTPULSE 300 // time in microseconds for a short pulse
#define LONGPULSE 900 // time in microseconds for a long pulse (ShortPulse * 3)
#define SYNCPULSE 9300 // trigger time in microseconds (ShortPulse * 31)
#define PULSETOLERANCE 100 // variability allowance

// the following are shared between the ISR and main loop so declared as volatile
volatile unsigned long long B00CodeWord; // accumulates the B00 code word
volatile byte B00Bit; // tracks number of received bits of B00 code word
  
// we pre-calculate the sample ranges during setup so they don't slow us down later
int syncPulseMin;
int syncPulseMax;
int shortPulseMin;
int shortPulseMax;
int longPulseMin;
int longPulseMax;
int pulseTimeout;

byte sensorInterrupt = 0;  // 0 = pin 2; 1 = pin 3

void setup() 
{
  syncPulseMin = SYNCPULSE - (PULSETOLERANCE*4);
  syncPulseMax = SYNCPULSE + (PULSETOLERANCE*4);  
  shortPulseMin = SHORTPULSE - PULSETOLERANCE;
  shortPulseMax = SHORTPULSE + PULSETOLERANCE;
  longPulseMin = LONGPULSE - PULSETOLERANCE;
  longPulseMax = LONGPULSE + PULSETOLERANCE;
  pulseTimeout = longPulseMax + 1;

  Serial.begin(9600);
  attachInterrupt(sensorInterrupt, rxPin, CHANGE);
}

void loop() 
{
  static unsigned long long lastCodeWord;
      
  if (B00Bit > 50 && lastCodeWord != B00CodeWord)
  {
    // Disable the interrupt while displaying info
    detachInterrupt(sensorInterrupt);

    lastCodeWord = B00CodeWord;
    B00Bit = 0;
        
    byte announce = lastCodeWord >> 42;
    if(announce == 0xB0)
    {
      //Serial.println("got B00 announcement");
      if(parityOk(lastCodeWord))
      {
        //Serial.println("parity OK");
        decodeValue(lastCodeWord);
      }
    }
    
    // Enable the interrupt again
    attachInterrupt(sensorInterrupt, rxPin, CHANGE);
  }
}

// Receive pin Interrupt Service Routive
void rxPin()
{
  unsigned long currentTimer = micros();
  // static variables to retain between calls to the ISR
  static unsigned long lastTimer = 0; // tracks tast time the ISR was called in microseconds
  static unsigned long long B00Bytes = 0; // 64 bits for accumulating the 50 bit B00 code word

 
  unsigned long pulseDuration = currentTimer - lastTimer;
  lastTimer = currentTimer;
         
  //  throw away any noise
  if( pulseDuration < 100)
    return;
      
  boolean rxPinState = PIND&B00000100;
  if( rxPinState == B00000100 ) // rising
  {
    // looking for sync signal
    if(pulseDuration >= syncPulseMin && pulseDuration <= syncPulseMax)
    {
      B00Bit = 1;
      B00Bytes = 0;
    }
    // accumulating the 50 bits of the B00 code word
    else if( B00Bit > 0 && B00Bit <= 50)
    {
      if(pulseDuration >= shortPulseMin && pulseDuration <= shortPulseMax)
      {
        B00Bytes = B00Bytes<<1;
        B00Bytes|=1;
        B00Bit++;
      }

      else if(pulseDuration >= longPulseMin && pulseDuration <= longPulseMax)
      {
        B00Bytes = B00Bytes<<1;
        B00Bit++;
      }
      else
        B00Bit = 0; // error so try again
    }
    if(B00Bit > 50) // completed reading all the bits
    { 
      // put the accumulated data into a variable shared with the main loop
      B00CodeWord = B00Bytes;
    }
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
