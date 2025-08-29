
#include <Arduino.h>

#include "SendOnlySerial.h"

// Debugging Example: demonstrates use of the debugging macros,
// printReg, PrintVar, and printFloatVar.

// This example uses 115 bytes of RAM.
// Most of that is in the first and last strings,
// and internally in functions for printing floating point numbers.

// Each use of printReg and/or printVar will consume a few bytes.
// Try to avoid printing floating point variables, use PROGMEM for strings,
// and comment out printVar() and printFloatVar() when not needed.


// The printReg, printVar, and printFloatVar macros below are defined in SendOnlySerial.h.

// They are repeated here so that you can see what they do.

#ifndef printReg
#define printReg(r) do { \
SendOnlySerial.print(#r); SendOnlySerial.print('\t'); SendOnlySerial.printBinary(r); \
SendOnlySerial.print("\t0x"); SendOnlySerial.print(r, HEX); \
SendOnlySerial.print('\t'); SendOnlySerial.print(r, DEC); \
SendOnlySerial.println() \
} while(0)
#endif

#ifndef printVar
#define printVar(x) do {\
SendOnlySerial.print(#x); SendOnlySerial.print('\t'); \
SendOnlySerial.print(x, DEC); SendOnlySerial.print("\thex "); \
SendOnlySerial.print(x, HEX); SendOnlySerial.println() \
} while(0)
#endif

#ifndef printFloatVar
#define printFloatVar(x) do {\
SendOnlySerial.print(#x); SendOnlySerial.print('\t'); \
SendOnlySerial.print(x, 6); SendOnlySerial.println() \
} while (0)
#endif

#ifndef PRINTFLASHSTRING
#define PRINTFLASHSTRING(name, value) \
do { \
  static const char name[] PROGMEM = value; \
  SendOnlySerial.printlnP(name); \
} while (0)

#define REPRINTFLASHSTRING(name) \
do { \
 SendOnlySerial.printlnP(name); \
 } while (0)
#endif


void setup() {
  SendOnlySerial.begin(9600); // explicit; begin() defaults to 9600.

  //print a string stored in RAM.
  char teststring[] = "test!";  // 6 bytes global SRAM
  SendOnlySerial.println(teststring);

  bool b = true;
  SendOnlySerial.println(b);

   // Print a string stored in flash.
   PRINTFLASHSTRING(usartRegistersHeading, "ATmega328P USART0 control registers, using printReg macro:");

  printReg(UCSR0A);        // control and status register A
  printReg(UCSR0B);        // control and status register B
  printReg(UBRR0H);        // baud rate register high byte
  printReg(UBRR0L);        // baud rate register low byte
  SendOnlySerial.println();

  // Some integers using the printVar macro
  static const char integerRangeString[] PROGMEM = "integers from -2 to 11 using printVar macro:";
  SendOnlySerial.printlnP(integerRangeString);

  for (char i = -2; i < 11; i ++) {
    printVar(i);

    // SendOnlySerial.println(i);  // <-- Prints strange characters!

    SendOnlySerial.println((char)(i + '0'));
    // This converts i to a printable number by adding ASCII '0' offset
    // Preventing interpretation as ASCII control characters

    // Adding the ASCII code for 0 to the character makes an "int",
    // which we then have to convert back to "char" format with (char).

    // -2 + '0' (48) gives 46, which is ASCII code for "." dot.
    // -1 + '0' gives 47, ASCII for "/".
  }
  SendOnlySerial.println();

  // printing a floating-point constant (shouldn't need debugging??)
  static const char floatString[] PROGMEM = "printFloatVar() for floating point number -1.23456:";
  SendOnlySerial.printlnP(floatString);

  float floatNumber = -1.23456;
  printFloatVar(floatNumber);
  SendOnlySerial.println();

  // and just print some values using the regular print function.
  static const char floatRangeString[] PROGMEM = "f-p numbers from PI up in steps of 0.4 to under 5:";
  SendOnlySerial.printlnP(floatRangeString);

  for (float f2 = PI; f2 < 5; f2 +=  0.4) {
    SendOnlySerial.println(f2);
  }
  SendOnlySerial.println();

  char endoftest[] = "That concludes the basic test of SendOnlySerial.";
  SendOnlySerial.println(endoftest);

  SendOnlySerial.flush();
}

void loop() {}
