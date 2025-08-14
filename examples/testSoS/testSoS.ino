
#include <Arduino.h>

#include "SendOnlySerial.h"

// Replaces uses of Serial.print() for debugging,
// saves Serial's 175 bytes of RAM.

// This example uses 61 bytes of RAM. 
// Most of that is in the printFloat functions.
// Try to avoid printing floating point variables.


// The printReg and printVar macros below are defined in SendOnlySerial.h.
// They are included here so you can see what they do.

#ifndef printReg
#define printReg(r) \
SendOnlySerial.print(#r); SendOnlySerial.print('\t'); SendOnlySerial.printBinary(r); \
SendOnlySerial.print("\t0x"); SendOnlySerial.print(r, HEX); \
SendOnlySerial.print('\t'); SendOnlySerial.print(r, DEC); \
SendOnlySerial.println()
#endif

#ifndef printVar
#define printVar(x) \
SendOnlySerial.print(#x); SendOnlySerial.print('\t'); \
SendOnlySerial.print(x, DEC); SendOnlySerial.print("\thex "); \
SendOnlySerial.print(x, HEX); SendOnlySerial.println()
#endif

#ifndef printFloatVar
#define printFloatVar(x) \
SendOnlySerial.print(#x); SendOnlySerial.print('\t'); \
SendOnlySerial.print(x, 6); SendOnlySerial.println()
#endif


void setup() {
  SendOnlySerial.begin(9600); // explicit; begin() defaults to 9600.

  //print a string.
  char teststring[8] = "test!";  // 8 bytes global SRAM
  SendOnlySerial.println(teststring);

  // and a string in Flash memory.

  static const char aTestFlashString[] PROGMEM = "The Quick Brown Fox Jumps Over";

  SendOnlySerial.printlnP(aTestFlashString);


  // Print the USART's control registers

  printReg(UCSR0A);        // control and status register A
  printReg(UCSR0B);        // control and status register B
  printReg(UBRR0H);        // baud rate register high byte
  printReg(UBRR0L);        // baud rate register low byte
  SendOnlySerial.println();

  // Some integers using the printVar macro

  for (char i = -2; i < 11; i ++) {
    printVar(i);

    // SendOnlySerial.println(i);  // <-- Prints strange characters!
    
    // There's a fish-hook with printing variables defined as "char" in C.
    // char is really a number, but the receiving hardware tries to interpret
    // it as an ASCII code, and print that.
    // (Because no one prints small numbers, do they?)
    // To print 'char's readably, use this magic formula:-

    SendOnlySerial.println((char)(i + '0'));
    
    // Adding the ASCII code for 0 to the character makes an "int",
    // which we then have to convert back to "char" format with (char).
    // (That's called a "cast". I think from casting metal into molds.)
    // Anyway:-
    // -2 + '0' (48) gives 46, which is ASCII code for "." dot.
    // -1 + '0' gives 47, ASCII for "/".
    // C, eh? *shakes head*
  }

  // printing a floating-point constant (shouldn't need debugging??)

  printFloatVar(-1.23456);

  // and just print some values using the regular print function.

  for (float f2 = PI; f2 < 5; f2 +=  0.4) {
    SendOnlySerial.println(f2);
  }

}

void loop() {}
