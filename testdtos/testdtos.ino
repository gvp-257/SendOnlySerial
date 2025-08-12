
#include <Arduino.h>

#define BAUD 9600
#include "dtos.h"
// "Debug TO Serial" or "Dangerous Transmit Only Serial" library
// Replaces uses of Serial.print() for debugging.

#define dspl(x)  DebugSerial.println(x)

#define dsplP(x) DebugSerial.printlnP(x)  // strings in flash memory
#define sccP(str, textconstant) static const char str[] PROGMEM =  textconstant

#ifndef printReg
#define printReg(r) \
DebugSerial.print(#r); DebugSerial.print(" = "); DebugSerial.printBinary(r); \
DebugSerial.print("\t0x"); DebugSerial.print(r, HEX); \
DebugSerial.print("\tdec "); DebugSerial.print(r, DEC); \
DebugSerial.println()
#endif

#ifndef printVar
#define printVar(x) \
DebugSerial.print(#x); DebugSerial.print(" = "); \
DebugSerial.print(x, HEX); DebugSerial.print("\tdec "); \
DebugSerial.print(x, DEC); DebugSerial.println()
#endif


void setup()
{
  DebugSerial.begin();
  char teststring[8] = "test!";  // 8 bytes global SRAM
  dspl(teststring);
  dspl(-1.23456);
  printReg(UCSR0A);
  printReg(UCSR0B);
  printReg(UBRR0H);
  printReg(UBRR0L); DebugSerial.println();

  for (char i = -2; i < 11; i ++) {printVar(i); DebugSerial.println(i + '0');}

  for (float f2 = PI; f2 < 5; f2 +=  0.4) {DebugSerial.println(f2);}
  sccP(testpgmemstr, "The Quick Brown Fox Jumps Over");
  dsplP(testpgmemstr);

}

void loop() {}
