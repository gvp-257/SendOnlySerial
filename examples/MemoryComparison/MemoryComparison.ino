
#include <Arduino.h>
// Memory comparison between SendOnlySerial and Arduino Serial.
// Compile this example in the Arduino IDE (Ctrl-R) and note the program size
// and SRAM usage.
//
// Then comment out the USE_SENDONLY 1 line, and uncomment the one below it
// to use Serial instead. Compile again and note the program memory and
// SRAM usage.


#define USE_SENDONLY 1
//#define USE_SENDONLY 0   // For serial

#if USE_SENDONLY

#include "SendOnlySerial.h"
#define SENDER SendOnlySerial

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

#else

#define SENDER Serial

#ifndef PRINTFLASHSTRING
#define PRINTFLASHSTRING(name, value) \
do { \
Serial.println(F(value)); \
} while (0)
#endif

#endif // USE_SENDONLY.


void setup() {
    SENDER.begin(9600);


    PRINTFLASHSTRING(aTestFlashString,"This is a string stored in Flash.");

    SENDER.println();


    PRINTFLASHSTRING(floatString,"Printing a floating point number -1.2345678 with 4 decimal places, and with 7:");

    float floatNumber = -1.2345678;
    SENDER.println(floatNumber);      // printing floats defaults to 4 decimal places
    SENDER.println(floatNumber, 7);   // second parameter is number of decimal places
    SENDER.println();

    // Printing integer type numbers:
    PRINTFLASHSTRING(integerString, "Printing an integer in decimal, hexadecimal and binary:");

    int integer = 21400;
    SENDER.println(integer);   // default format is decimal
    SENDER.println(integer, HEX);
    SENDER.println(integer, BIN);  // binary
    SENDER.println();


    // printing a boolean
    PRINTFLASHSTRING(boolString, "Printing a boolean variable, set to true:");
    bool b = true;
    SENDER.println(b);   // "true"

    PRINTFLASHSTRING(endoftest,"That concludes the memory comparison example for SendOnlySerial.");

    SENDER.flush();
}

void loop() {}
