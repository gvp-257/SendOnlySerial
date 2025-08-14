#ifndef SENDONLYSERIAL_H
#define SENDONLYSERIAL_H
/*
  Send Only Serial for debugging or logging using ATmega328's hardware serial.

  Low RAM usage Arduino library for sending data and text to Arduino's Serial
  Monitor, or to another Arduino for logging.
  ATmega168P/328P only (2009, Uno, Nano (original), Pro Mini, Micro).

  Has no error checking and no timeout support.
  Only uses 8N1 data frames (bytes).
  No support for Arduino String objects. Does not support Arduino's "F()" macro.
  Don't use it at the same time as Arduino's Serial.

  Printing C strings stored in PROGMEM (flash memory) is possible but slightly
  awkward.  Use:-

    static const char aFlashString[] PROGMEM = "The string you want to print";
    // Have to give the string a name, different for each one.

    SendOnlySerial.printlnP(aFlashString);
    // Note the 'P': -----^ and no []s: ^


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
  ATmega88/168/328/P/PA datasheet: section 20, USART0.
  AVR-libc documentation for util/setbaud.h and stdlib.h.
  "MAKE: AVR Programming" by Elliot Williams. Makermedia, Sebastopol, CA, USA;
  2014. Chapter 9 on serial communications using the USART.
*/
#include <avr/pgmspace.h>    // For TXDataP, TXStringP

// Possibly useful debugging macros:-
#ifndef NDEBUG

#ifndef printReg
#define printReg(r) \
SendOnlySerial.print(#r); SendOnlySerial.print('\t'); SendOnlySerial.printBinary(r); \
SendOnlySerial.print("\t0x"); SendOnlySerial.print(r, HEX); \
SendOnlySerial.print('\t'); SendOnlySerial.print(r, DEC); \
SendOnlySerial.println()

// printReg(ADCSRA) gives a line:  ADCSRA  B1000 0111    0x87     135

#endif

#ifndef printVar
#define printVar(x) \
SendOnlySerial.print(#x); \
SendOnlySerial.print('\t'); \
SendOnlySerial.print(x, DEC); \
SendOnlySerial.print("\t0x"); \
SendOnlySerial.print(x, HEX); \
SendOnlySerial.println()
#endif

// int arrowcount = 22; printVar(arrowcount) gives: arrowcount      22     0x16


// printVar works for floating-points format as well as integers.
// Just twice in decimal with 10 and 16 decimals.
// So, printFloatVar (decimal only):

#ifndef printFloatVar
#define printFloatVar(x) \
SendOnlySerial.print(#x); \
SendOnlySerial.print('\t'); \
SendOnlySerial.print(x, 6); \
SendOnlySerial.println()
#endif

#else

#define printReg

#define printVar

#define printFloatVar

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
    void begin(void) {begin(9600);}
    void begin(const unsigned long baud)
    {
        // turn on the peripheral and configure it for 8N1 and selected BAUD.
        PRR    &= ~(1<<PRUSART0);

        // Data size 8 bits, Async mode, no parity and 1 stop bit.
        UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
        // UMSEL01, UMSEL00 = 0 -> Asynchronous UART mode
        // UPM01, UPM00 = 0 -> No parity; USBS = 0 -> 1 stop bit.
        // UCPOL0 = 0 -> normal polarity.

        uint16_t baudreg = ((F_CPU / 4 / baud) - 1) / 2;

        if ((baudreg > 4095) || (baud == 57600 && F_CPU == 16000000UL))
        {
            UCSR0A &= ~(1<<U2X0);   // not USE_2X
            baudreg = ((F_CPU / 8 / baud) -1) / 2; // re-calculate registers.

        }
        else {UCSR0A |= (1<<U2X0);} // USE_2X except in the case above.

        UBRR0H = (uint8_t)(baudreg >> 8);
        UBRR0L = (uint8_t)baudreg;

        //Enable transmit and receive; no interrupts enabled.
        UCSR0B &= ~((1<<RXCIE0) | (1<<TXCIE0) | (1<<UDRIE0) | (1<<UCSZ02));
        UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    }

    void end(void)
    {
        UCSR0B = 0;             // turn off RXEN and TXEN.
        PRR   |= 1<<PRUSART0;   // turn off the USART's clock.
    }

    void flush(void)
    {
        loop_until_bit_is_set(UCSR0A, TXC0);
    }

    //==============================================================================
    //
    // Transmit.
    //

    bool isReady(void)              {return USART_READY;}
    bool ready(void)                {return USART_READY;}

    void TXRaw(const uint8_t b)     {UDR0 = b;}

    void TX(const uint8_t b)
    {
        loop_until_bit_is_set(UCSR0A, UDRE0); // wait till prev tx complete.
        UDR0 = b;
    }

    // Transmitting multiple bytes:-
    void TXData(const uint8_t* data, int num)
    // Transmit num bytes from the supplied data array.
    {
        int i = 0;
        while (num--) {TX(data[i++]);}
    }

    // Bytes are in program memory (flash):-
    void TXDataP(const uint8_t* data, const int num)
    {
        for (int i = 0; i < num; i++) {TX(pgm_read_byte(&data[i]));}
    }


    // Transmitting text strings.

    void TXString(const char* string)
    // warning: Assumes string is properly terminated with a null 0 byte.
    {
        register int i = 0;
        while (string[i]) {TX((uint8_t)string[i++]);}
    }

    // warning: Assumes string is properly terminated with a null 0 byte
    // and has been declared with PROGMEM.
    void TXStringP(const char * _s)
    {
        size_t  i = 0;
        uint8_t c;
        while ((c = pgm_read_byte(&_s[i++])) != '\0') {TX(c);}
    }
    void printBinary(const uint8_t b)
    // transmit a binary representation of the byte.
    {
        TX('0'); TX('b');
        TX(((b & 0x80))? '1':'0');
        TX(((b & 0x40))? '1':'0');
        TX(((b & 0x20))? '1':'0');
        TX(((b & 0x10))? '1':'0');
        TX(' ');
        TX(((b & 0x08))? '1':'0');
        TX(((b & 0x04))? '1':'0');
        TX(((b & 0x02))? '1':'0');
        TX(((b & 0x01))? '1':'0');
    }

    void printDigit(uint8_t d) {TX((d < 10)? (d + '0'): (d - 10 + 'a'));}


    // Send a byte, or a block of bytes: an array or C-struct.
    void write(const uint8_t b)              {TX(b);}
    void write(const uint8_t* data, int num) {TXData(data, num);}
    // PROGMEM version
    void writeP(const uint8_t* data, int num) {TXDataP(data, num);}

    // 8-bit types
    void print(const bool b)    {if (b) print("true"); else print("false");}
    void print(const char c)    {TX(c);} // also short
    void print(const uint8_t b) {TX(b);}  // byte, unsigned char, unsigned short

    // Print strings - C null-delimited arrays only.
    void print(const char* string) {TXString(string);}
    void print(const double df, const int decimals = 4)
    {
        char buf[30] = {0};
        dtostrf(df, 5, decimals, buf);
        TXString(buf);
    }

    // Numerical types
    void print(const float f, const int decimals = 4)
    {
        print((double)f, decimals);
    }
    void print(const long  integer, const int base=10)
    {
        char buf[20] = {0};
        ltoa(integer, buf, base);
        TXString(buf);
    }
    void print(const int integer, const int base=10)
    {
        print((long)integer, base);
    }
    void print(const uint8_t b, const int base=10)
    {
        char buf[20] = {0};
        utoa((unsigned int)b, buf, base);
        TXString(buf);
    }
    void print(const unsigned long integer, const int base=10)
    {
        char buf[20] = {0};
        ultoa(integer, buf, base);
        TXString(buf);
    }
    void print(const unsigned int integer, const int base=10)
    {
        print((unsigned long)integer, base);
    }


    // println variants
    void println(void)               {print('\r'); print('\n');}

    void println(const bool b)       {print((bool)b);      println();}
    void println(const char c)       {print(c);      println();}
    void println(const uint8_t b)    {print((short)b);      println();}

    void println(const char* string) {print(string); println();}

    void println(const double d, const int decimals = 4)
        {print(d, decimals); println();}
    void println(const float f, const int decimals = 4)
        {print(f, decimals); println();}

    void println(const int i, const int base = 10) {print(i, base); println();}
    void println(const long l, const int base = 10){print(l, base); println();}
    void println(const unsigned int i, const int base = 10) {print(i, base); println();}
    void println(const unsigned long l, const int base = 10){print(l, base); println();}


    // Strings stored in PROGMEM.
// F macro.
// compile error: 'str' has incomplete type
// WString.h:37:7: note: forward declaration of 'class __FlashStringHelper'
// #if defined(F)   // F macro in Arduino's Print.h
//     void print(const __FlashStringHelper str)
//     {
//         const char * s = reinterpret_cast<const char *>(str);
//         TXStringP(s);
//     }
//     void println(const __FlashStringHelper str) {print(str); println();}
// #endif

    // Flash strings defined:
    // static const char infostring[] PROGMEM = "InfoInfoInfo!";

    void printP(const char* string) {TXStringP(string);}
    void printlnP(const char* string) {TXStringP(string); println();}
};

// The object:
struct AVR_USART SendOnlySerial;

#endif