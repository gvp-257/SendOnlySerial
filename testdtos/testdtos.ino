
#include <Arduino.h>

#include "dtos.h"
// "Debug TO Serial" or "De-featured Transmit Only Serial" library.
// Replaces uses of Serial.print() for debugging,
// saves Serial's 175 bytes of RAM.


// The printReg and printVar macros are defined in dtos.h,
// they are included here so you can see what they do.

#ifndef printReg
#define printReg(r) \
DebugSerial.print(#r); DebugSerial.print('\t'); DebugSerial.printBinary(r); \
DebugSerial.print("\t0x"); DebugSerial.print(r, HEX); \
DebugSerial.print('\t'); DebugSerial.print(r, DEC); \
DebugSerial.println()
#endif

#ifndef printVar
#define printVar(x) \
DebugSerial.print(#x); DebugSerial.print('\t'); \
DebugSerial.print(x, DEC); DebugSerial.print("\thex "); \
DebugSerial.print(x, HEX); DebugSerial.println()
#endif

#ifndef printFloatVar
#define printFloatVar(x) \
DebugSerial.print(#x); DebugSerial.print('\t'); \
DebugSerial.print(x, 6); DebugSerial.println()
#endif

// printVar works for floats too, just with lots of decimals, 10 and 16.

#define dspl(x)  DebugSerial.println(x)

#define dsplP(x) DebugSerial.printlnP(x)  // strings in flash memory
#define sccP(str, textconstant) static const char str[] PROGMEM =  textconstant


void setup()
{
  DebugSerial.begin(9600); // explicit; begin() defaults to 9600.

  //print a string.
  char teststring[8] = "test!";  // 8 bytes global SRAM
  dspl(teststring);

  // and a string in Flash memory.
  sccP(testpgmemstr, "The Quick Brown Fox Jumps Over");
  dsplP(testpgmemstr);

  // print the USART's control registers and data register
  printReg(UCSR0A);
  printReg(UCSR0B);
  printReg(UBRR0H);
  printReg(UBRR0L);
  printReg(UDR0);
  DebugSerial.println();

  // test printing some integers using the printVar macro
  for (char i = -2; i < 11; i ++) {printVar(i); DebugSerial.println(i + '0');}

  printFloatVar(-1.23456);
  // and some floats - just the values.
  for (float f2 = PI; f2 < 5; f2 +=  0.4) {DebugSerial.println(f2);}

}

void loop() {}
