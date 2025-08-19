#ifndef SENDONLYSERIAL_LIB_H
#define SENDONLYSERIAL_LIB_H

// SendOnlySerial:  low-RAM lib for debugging or logging using ATmega328P's
// hardware USART (USART0).

// GvP, 2025.   MIT licence.
// https://github.com/gvp-257/SendOnlySerial

/*
  Low RAM usage library for sending data and text to Arduino's Serial
  Monitor, or to another Arduino for logging (via the TX0 pin).

  ATmega168PA/328P only (2009, Uno, Nano (original), Pro Mini, Micro).
  Possible TODO: extend to ATtiny84, ATmega1284P, and/or ATmega2560 (Mega).

  Has no error checking and no timeout support.
  Only uses 8N1 data frames (bytes).
  No support for Arduino String objects. Does not support Arduino's "F()" macro.
  Don't use it at the same time as Arduino's Serial.

  Printing C strings stored in PROGMEM (flash memory) is possible but slightly
  awkward.  Use:-

    static const char aFlashString[] PROGMEM = "The string you want to print";

    // Have to give the string a name, different for each one.

    SendOnlySerial.printlnP(aFlashString);
    //                    ^             ^
    //                    |             |
    // Note the 'P': -----+ and no []:  +


  WHY THOUGH?
  ===========
  SendOnlySerial uses very little RAM--unlike Serial, which grabs 175 precious,
  precious bytes. Even if you do one tiny little Serial.begin() just once.


  NOTES
  =====
  1. Printing any floating-point number will increase the flash usage of this
  library by about 1900 bytes, and allocate some RAM.
  Try to avoid using floating-point numbers.

  2. SendOnlySerial.begin() defaults to 9600 baud.
  For other rates use SendOnlySerial.begin(baudrate).

  References
  ==========
  ATmega88A/PA/168A/PA/328/P datasheet: section 20, USART0.
  https://www.microchip.com/en-us/product/ATMEGA328P

  Arduino Reference for Serial:-
  https://docs.arduino.cc/language-reference/en/functions/communication/serial/

  "MAKE: AVR Programming" by Elliot Williams. Makermedia, Sebastopol, CA, USA;
  2014. Chapter 9 on serial communications using the USART.
*/

#if defined (__AVR_ATmega328P__)  || defined (__AVR_ATmega168PA__) \
 || defined (__AVR_ATmega328PB__) || defined (__AVR_ATmega328__)   \
 || defined (__AVR_ATmega88PA__)  || defined (__AVR_ATmega168A__)  \
 || defined (__AVR_ATmega48PA__)
#else
#error “SendOnlySerial only supports boards with AVR ATmega168A/PA/328/P/PB processor.”
#endif


// Possibly useful debugging macros:-
#ifndef NDEBUG

#ifndef printReg
#define printReg(r) do { \
SendOnlySerial.print(#r); SendOnlySerial.print('\t'); SendOnlySerial.printBinary(r); \
SendOnlySerial.print("\t0x"); SendOnlySerial.print(r, HEX); \
SendOnlySerial.print('\t'); SendOnlySerial.print(r, DEC); \
SendOnlySerial.println(); \
} while(0)

// printReg(ADCSRA) gives a line:  ADCSRA  B1000 0111    0x87     135

#endif

#ifndef printVar
#define printVar(x) do { \
SendOnlySerial.print(#x); \
SendOnlySerial.print('\t'); \
SendOnlySerial.print(x, DEC); \
SendOnlySerial.print("\t0x"); \
SendOnlySerial.print(x, HEX); \
SendOnlySerial.println(); \
} while(0)
#endif

// int arrowcount = 22; printVar(arrowcount) gives: arrowcount      22     0x16

// printVar works for floating-points format as well as integers.
// Just twice in decimal with 10 and 16 decimals.
// So, printFloatVar (decimal only):

#ifndef printFloatVar
#define printFloatVar(x) do {\
SendOnlySerial.print(#x); \
SendOnlySerial.print('\t'); \
SendOnlySerial.print(x, 6); \
SendOnlySerial.println(); \
} while(0)
#endif

#else
#ifndef printReg
#define printReg
#endif
#ifndef printVar
#define printVar
#endif
#ifndef printFloatVar
#define printFloatVar
#endif
#endif //NDEBUG.

//Number printing formats. These are defined in Arduino's "Print.h" header file.
// If that didn't get included we need them here.
#ifndef DEC
#define DEC 10
#endif
#ifndef HEX
#define HEX 16
#endif
#ifndef BIN
#define BIN 2
#endif


/* Defined for convenience */
#define   USART_READY      bit_is_set(UCSR0A, UDRE0)


struct AVR_USART
{
    void begin(void);                 // power on, default baud rate 9600
    void begin(const unsigned long);  // baud rate.
    void end(void);                   // power off USART hardware module.
    void flush(void);                 // Wait for last byte to be sent.

    bool isReady(void);               // USART has room for a byte to send.
    bool ready(void);

    // basic sending functions.
    void TX(const uint8_t);           // Give the USART a byte to send.
    void TXRaw(const uint8_t);        // Does not check if the USART has room.
    void TXData(const uint8_t*, size_t); // Transmit bytes.
    void TXString(const char* );      // Null-delimited ASCII.

    // Bytes that are in program memory (flash):-
    void TXDataP(const uint8_t*, size_t);
    void TXStringP(const char *);

    // Arduino spec-compliant(ish) functions. Write, print verbs.

    // Send a byte, or a block of bytes: an array or C-struct.
    void write(const uint8_t);        // Arduino spec. version of TX
    void write(const uint8_t*, size_t);  // ditto, TXData
    // PROGMEM version
    void writeP(const uint8_t*, size_t); // similar, data in flash

    // 8-bit special types
    void printBinary(const uint8_t);  // B0010 1100 format: constant length
                                      // better than print(b, 2)
    void printDigit(uint8_t);         // numerical 4 bits -> char 0-9,a-f
    void print(const bool);           // "true", "false"
    void print(const char);           // receiver thinks ASCII

    // Strings - C null-delimited arrays only.
    void print(const char*);

    // Numerical types
    void print(const double);            // value, decimals=4
    void print(const double, const int); // value, decimals
    void print(const float);
    void print(const float, const int);

    void print(const int);               // value, base = 10 (decimal)
    void print(const int, const int);    // value, base 2 10 16 bin dec hex
    void print(const long);
    void print(const long, const int);

    void print(const uint8_t);
    void print(const uint8_t, const int);
    void print(const unsigned int);
    void print(const unsigned int, const int);
    void print(const unsigned long);
    void print(const unsigned long, const int);

    // println() variants of the above.
    void println(void);

    void println(const bool);
    void println(const char);
    void println(const uint8_t);

    void println(const char*);

    void println(const double);
    void println(const double, const int);
    void println(const float);
    void println(const float, const int);

    void println(const int);
    void println(const int, const int);
    void println(const long);
    void println(const long, const int);
    void println(const unsigned int);
    void println(const unsigned int, const int);
    void println(const unsigned long);
    void println(const unsigned long, const int);


    // Strings in flash memory, defined:-
    // static const char infostring[] PROGMEM = "InfoInfoInfo!";
    void printP(const char*);
    void printlnP(const char*);

// TODO: Support Arduino's 'F()' macro (Print.h).
// TODO: Support Arduino Strings (maybe? - memory hungry.)

};

// The object:
extern struct AVR_USART SendOnlySerial;

#endif