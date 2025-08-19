
#include <Arduino.h>

#include "SendOnlySerial.h"

// Simple Usage: Using SendOnlySerial like Arduino Serial.
// Shows "print" and "write" functions, and the "printBinary" function
// that Arduino's Serial does not have.


void setup() {
    SendOnlySerial.begin(250000); // begin() defaults to 9600.

    //print a string stored in RAM.
    char RAMstring[] = "An ordinary string, stored in RAM.";
    SendOnlySerial.println(RAMstring);
    SendOnlySerial.println();

    // How to store a string in Flash memory and print it.
    static const char aTestFlashString[] PROGMEM = "This is a string stored in Flash, printed with printlnP.";
    SendOnlySerial.printlnP(aTestFlashString);
    SendOnlySerial.println();


    static const char floatString[] PROGMEM = "Printing a floating point number -1.2345678 with 4 decimal places, and with 7:";
    SendOnlySerial.printlnP(floatString);

    float floatNumber = -1.2345678;
    SendOnlySerial.println(floatNumber);      // printing floats defaults to 4 decimal places
    SendOnlySerial.println(floatNumber, 7);   // second parameter is number of decimal places
    SendOnlySerial.println();

    // Printing integer type numbers:
    static const char integerString[] PROGMEM = "Printing an integer in decimal, hexadecimal and binary:";
    SendOnlySerial.printlnP(integerString);

    int integer = 21400;
    SendOnlySerial.println(integer);   // default format is decimal
    SendOnlySerial.println(integer, HEX);
    SendOnlySerial.println(integer, BIN);  // binary
    SendOnlySerial.println();

    // Using write(block, blocksize):
    static const char blockString[] PROGMEM = "Writing a block of bytes containing the letter 'A':";
    SendOnlySerial.printlnP(blockString);

    byte block[8];
    for (size_t i = 0; i < 8; i++) block[i] = 'A';
    SendOnlySerial.write(block, sizeof(block));
    SendOnlySerial.println();


    static const char binaryString[] PROGMEM = "Printing a byte in fixed-length binary format (decimal value 118):";
    SendOnlySerial.printlnP(binaryString);

    int8_t singlebyte = 118;
    SendOnlySerial.printBinary(singlebyte);
    SendOnlySerial.println();


    // printing a boolean
    static const char boolString[] PROGMEM = "Printing a boolean variable, set to true:";
    SendOnlySerial.printlnP(boolString);
    bool b = true;
    SendOnlySerial.println(b);   // "true"

    char endoftest[] = "That concludes the simple usage example of SendOnlySerial.";
    SendOnlySerial.println(endoftest);

    SendOnlySerial.flush();
}

void loop() {}
