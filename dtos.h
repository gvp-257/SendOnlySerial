#ifndef DTOS_H
#define DTOS_H
/*

  DTOS: "Debug TO Serial", or "Dangerous Transmit-Only Serial"
  ------------------------------------------------------------

  Low RAM Arduino library for sending data and text to Arduino's Serial
  Monitor. ATmega168P/328P only (2009, Uno, Nano (original), Pro Mini, Micro)

  Has no error checking and no timeout support.
  Only uses 8N1 data frames (bytes).
  No support for Arduino String objects.
  Printing C strings stored in PROGMEM (flash memory) is slightly awkward.

  BUT:

  DTOS uses very little RAM--unlike Serial, which grabs 175 precious,
  precious bytes. If you do Serial.Begin() even just once.

  So instead of Serial, use DebugSerial, like so:-


  #define BAUD 9600    // 100000, 50000, 9600. Possibly 200000, 500000

  #include "dtos.h"

  .... your defines, globals and functions ....

  void setup() {
      DebugSerial.Begin();
      DebugSerial.Print("Started.");
      .... your setup code ....
  }

  Void loop() {
    .... your code ....
  }


  NOTE the "#define BAUD" line before the include line. If you omit it, the
  serial port will use 9600 baud, and the compiler output will have an
  information line suggesting that you #define BAUD.

  NOTES
  =====
  1. Uses 6 bytes of RAM, somewhere in the avr-libc functions for formatting
  numbers.  Still, 6 is better than 175.
  2. Printing any floating-point number will increase the flash usage of this
  library by about 1900 bytes. Try to avoid using floating-point numbers.

  References
  -----------
  ATmega88/168/328/P/PA datasheet: section 20, USART0.
  AVR-libc documentation for util/setbaud.h and stdlib.h.
  "MAKE: AVR Programming" by Elliot Williams. Makermedia, Sebastopol, CA, USA;
  2014. Chapter 9 on serial communications using the USART.

*/


#include <avr/pgmspace.h>    // For PrintStringP, TXDataP

//Number printing formats. These are defined in Arduino's "Print.h" header file.
#ifndef DEC
#define DEC 10
#endif
#ifndef HEX
#define HEX 16
#endif
#ifndef BIN
#define BIN 2
#endif

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

/* These are defined for convenience */
#define   USART_HAS_DATA   bit_is_set(UCSR0A, RXC0)
#define   USART_READY      bit_is_set(UCSR0A, UDRE0)


// In C++ structs are just like classes except they have no constructor or
// destructor.

struct M328P_USART0
{

    void begin(void)
    {
        // turn on the peripheral and configure it for 8N1 and selected BAUD.
        PRR    &= ~(1<<PRUSART0);

        //Enable transmit and receive; no interrupts enabled.
        UCSR0B &= ~((1<<RXCIE0) | (1<<TXCIE0) | (1<<UDRIE0) | (1<<UCSZ02));
        UCSR0B |= (1<<RXEN0) | (1<<TXEN0);

        #ifndef BAUD
        #define BAUD  9600
        #pragma message "BAUD set to 9600 in smallserial_basic.h. #define BAUD xxxxx for other speeds."
        #endif
        #include <util/setbaud.h>
        // AVR-libc helper library for serial baud rates.
        // uses BAUD to calculate UBRRH_VALUE, UBRRL_VALUE and USE_2X.

        UBRR0H = UBRRH_VALUE;
        UBRR0L = UBRRL_VALUE;
        #if USE_2X
        UCSR0A |= (1<<U2X0);
        #else
        UCSR0A &= ~(1<<U2X0);
        #endif

        // Data size 8 bits, Async mode, no parity and 1 stop bit.
        UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
        // UMSEL01, UMSEL00 = 0 -> Asynchronous UART mode
        // UPM01, UPM00 = 0 -> No parity; USBS = 0 -> 1 stop bit.
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

    void printChar(const char c)  {TX((uint8_t)c);}
    void printDigit(uint8_t d) {TX((d < 10)? (d + '0'): (d - 10 + 'a'));}


    // Send a byte, or a block of bytes: an array or C-struct.
    void write(const uint8_t b)              {TX(b);}
    void write(const uint8_t* data, int num) {TXData(data, num);}
    // PROGMEM version
    void writeP(const uint8_t* data, int num) {TXDataP(data, num);}


    // Print a text string - C null-delimited arrays only.
    void print(const char c)       {TX(c);}
    void print(const char* string) {TXString(string);}
    void print(const double df, const int decimals = 4)
    {
        char buf[30] = {0};
        dtostrf(df, 5, decimals, buf);
        TXString(buf);
    }
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
    void println(void)               {print('\r'); print('\n');}
    void println(const char c)       {print(c); println();}
    void println(const char* string) {print(string); println();}

    void println(const double d, const int decimals = 4)
        {print(d, decimals); println();}

    void println(const float f, const int decimals = 4)
        {print(f, decimals); println();}

    void println(const int i, const int base = 10) {print(i, base); println();}
    void println(const long l, const int base = 10){print(l, base); println();}



    // For printing strings stored in PROGMEM:
    // defined: static const char infostring[] PROGMEM = "InfoInfoInfo!";

    void printP(const char* string) {TXStringP(string);}
    void printlnP(const char* string) {TXStringP(string); println();}

};

// The object:
struct M328P_USART0 DebugSerial;

#endif