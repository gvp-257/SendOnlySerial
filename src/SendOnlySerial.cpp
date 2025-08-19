
// SendOnlySerial:  low-RAM lib for debugging or logging using ATmega328P's
// hardware USART (USART0).

// GvP, 2025.   MIT licence.
// https://github.com/gvp-257/SendOnlySerial


#include  <stdlib.h>         // For itoa, ltoa, dtostrf.
#include <avr/pgmspace.h>    // For TXDataP, TXStringP
#include "SendOnlySerial.h"

void AVR_USART::begin(void) {begin(9600);}

void AVR_USART::begin(const unsigned long baudRequested)
{
    unsigned long baud = baudRequested;

    if (baud < 300 ) {baud = 300;}  // 16 MHz system clock can't go below 300.

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

void AVR_USART::end(void)
{
    UCSR0B = 0;             // turn off RXEN and TXEN.
    PRR   |= 1<<PRUSART0;   // turn off the USART's clock.
}

void AVR_USART::flush(void)
{
    loop_until_bit_is_set(UCSR0A, TXC0);
}

//==============================================================================
//
// Transmit.
//

bool AVR_USART::isReady(void)              {return USART_READY;}

bool AVR_USART::ready(void)                {return USART_READY;}

void AVR_USART::TX(const uint8_t b)
{
    loop_until_bit_is_set(UCSR0A, UDRE0); // wait till prev tx complete.
    UDR0 = b;
}

void AVR_USART::TXRaw(const uint8_t b)     {UDR0 = b;}

// Transmitting multiple bytes:-
void AVR_USART::TXData(const uint8_t* data, size_t num)
// Transmit num bytes from the supplied data array.
{
    int i = 0;
    while (num--) {TX(data[i++]);}
}

// Bytes are in program memory (flash):-
void AVR_USART::TXDataP(const uint8_t* data, size_t num)
{
    for (int i = 0; i < num; i++) {TX(pgm_read_byte(&data[i]));}
}


// Transmitting text strings.

void AVR_USART::TXString(const char* string)
// warning: Assumes string is properly terminated with a null 0 byte.
{
    if (!string) return;
    register int i = 0;
    while (string[i]) {TX((uint8_t)string[i++]);}
}

// warning: Assumes string is properly terminated with a null 0 byte
// and has been declared with PROGMEM.
void AVR_USART::TXStringP(const char * _s)
{
    if (!string) return;
    size_t  i = 0;
    uint8_t c;
    while ((c = pgm_read_byte(&_s[i++])) != '\0') {TX(c);}
}


// Send a byte, or a block of bytes: an array or C-struct.
void AVR_USART::write(const uint8_t b)              {TX(b);}

void AVR_USART::write(const uint8_t* data, size_t num) {TXData(data, num);}

// PROGMEM version
void AVR_USART::writeP(const uint8_t* data, size_t num) {TXDataP(data, num);}

//Numerical types.

// Print a byte as binary, fixed length, format: 0b0011 1011
// (Using itoa() with base 2 gives variable length results.)
void AVR_USART::printBinary(const uint8_t b)
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

// Four bits 0 - F (hex)
void AVR_USART::printDigit(uint8_t d) {d &= 0x0f; TX((d < 10)? (d + '0'): (d - 10 + 'a'));}


// 8-bit special types
void AVR_USART::print(const bool b)    {if (b) print("true"); else print("false");}
void AVR_USART::print(const char c)    {TX(c);}
// Print strings - C null-delimited arrays only.
void AVR_USART::print(const char* string)
{
    if (!string) return;
    TXString(string);
}


// Floating-point types
void AVR_USART::print(const double df)                  {print(df, 4);}
void AVR_USART::print(const double df, const int decimals = 4)
{
    char buf[30] = {0};
    dtostrf(df, 5, decimals, buf);
    TXString(buf);
}

void AVR_USART::print(const float f)                    {print(f, 4);}
void AVR_USART::print(const float f, const int decimals)
{
    print((double)f, decimals);
}

// integer types
void AVR_USART::print(const long integer)               {print(integer, 10);}
void AVR_USART::print(const long integer, const int base)
{
    char buf[20] = {0};
    ltoa(integer, buf, base);
    TXString(buf);
}
void AVR_USART::print(const int integer)                {print(integer, 10);}
void AVR_USART::print(const int integer, const int base)
{
    print((long)integer, base);
}
void AVR_USART::print(const uint8_t b) {TX(b);}  // byte, unsigned char, unsigned short
void AVR_USART::print(const uint8_t b, const int base)
{
    char buf[20] = {0};
    utoa((unsigned int)b, buf, base);
    TXString(buf);
}

void AVR_USART::print(const unsigned long integer)      {print(integer, 10);}
void AVR_USART::print(const unsigned long integer, const int base)
{
    char buf[20] = {0};
    ultoa(integer, buf, base);
    TXString(buf);
}
void AVR_USART::print(const unsigned int integer)       {print(integer, 10);}
void AVR_USART::print(const unsigned int integer, const int base)
{
    print((unsigned long)integer, base);
}


// println()

void AVR_USART::println(void)                                 {print('\r');print('\n');}

void AVR_USART::println(const bool b)                         {print((bool)b); println();}
void AVR_USART::println(const char c)                         {print(c);       println();}
void AVR_USART::println(const uint8_t b)                      {print((short)b);println();}

void AVR_USART::println(const char* string)                   {print(string);  println();}

void AVR_USART::println(const double d)                       {print(d, 4);    println();}
void AVR_USART::println(const double d, const int decimals)
{print(d, decimals); println();}
void AVR_USART::println(const float f)                        {print(f, 4);    println();}
void AVR_USART::println(const float f, const int decimals)
    {print(f, decimals); println();}

void AVR_USART::println(const int i)                          {print(i,   10); println();}
void AVR_USART::println(const int i, const int base)          {print(i, base); println();}
void AVR_USART::println(const long l)                         {print(l,   10); println();}
void AVR_USART::println(const long l, const int base)         {print(l, base); println();}
void AVR_USART::println(const unsigned int i)                 {print(i,   10); println();}
void AVR_USART::println(const unsigned int i, const int base) {print(i, base); println();}
void AVR_USART::println(const unsigned long l)                {print(l,   10); println();}
void AVR_USART::println(const unsigned long l, const int base){print(l, base); println();}


// Flash strings, defined:-
// static const char infostring[] PROGMEM = "InfoInfoInfo!";

void AVR_USART::printP(const char* string)   {TXStringP(string);}
void AVR_USART::printlnP(const char* string) {TXStringP(string); println();}

// TODO: Support Arduino's 'F()' macro.
// compile error: 'str' has incomplete type
// WString.h:37:7: note: forward declaration of 'class __FlashStringHelper'
// #if defined(F)   // F macro in Arduino's Print.h
//     void AVR_USART::print(const __FlashStringHelper str)
//     {
//         const char * s = reinterpret_cast<const char *>(str);
//         TXStringP(s);
//     }
//     void AVR_USART::println(const __FlashStringHelper str) {print(str); println();}
// #endif

// The object:
struct AVR_USART SendOnlySerial;
