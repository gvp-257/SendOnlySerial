# SendOnlySerial.h

A lightweight alternative to Arduino's Serial for debugging or logging. Down to 0 bytes of RAM, *vs.* Serial's 175+ bytes RAM.

Compiling the MemoryComparison example gives the following results with Arduino IDE 2.3.6 on an Uno:


|        bytes:        |FLASH|SRAM|
|----------------------|-----|----|
|Serial (built in)     |3484 | 188|
|SendOnlySerial library|2904 |  15|


## Compatibility

The Send Only Serial library is compatible with the Arduino Uno, the Nano, the Duemilanove, and the Pro Mini (both 5 volt and 3 volt) boards. It will also work with "breadboard Arduinos" using the AVR ATmega328P microcontroller and with a system clock at 16 MHz, 8 MHz, or 1 MHz.

`SendOnlySerial` uses the TX0 "hardware serial" line which is also connected to the USB interface on Unos and Nanos, just like Serial.  You *could* use SendOnlySerial together with Arduino's built-in `Serial`, but I can't think why you might want to.


## Differences from Arduino's Built-in "Serial" Object

SendOnlySerial is only capable of *sending* data to the PC. There are no `read()` or `available()` or `parseXxx()` or `findXxx()` or `setTimeout()` functions. Arduino's `String` objects are not supported.

(If you're considering using this library, because you are running out of RAM, Arduino `String` objects use too much RAM for you anyway.)

SendOnlySerial is limited to 8N1 formatted data frames. (8 data bits, no parity, 1 stop bit. 8N1 is by far the most common data frame format.) Uses "CR and LF" line endings. No error checking or timeout.

### So What's Left, Then?

`print()` and `println()`, mainly. And even those are a bit limited: Besides `String` objects, there is no support for Arduino's `F()` macro for strings in flash (program memory).

Printing strings stored in flash is possible, but slightly awkward. You have to give each one a name, and use a "printP" function instead of "print". Like so:-


    static const char aFlashString[] PROGMEM = "The string you want to print";
    //  different name for each string. Note the square brackets (array).

    SendOnlySerial.printlnP(aFlashString);
    // Note the 'P': -----^ and no []s: ^


### Anything Added?

`SendOnlySerial.printBinary(byte b)`: print a single byte in fixed length binary format, in two groups of four bits:-

    SendOnlySerial.printBinary(0xc6);  // prints "0b1100 0110"  via serial.

Also, `SendOnlySerial.printDigit(byte b)`: prints the low four bits of b in ASCII:-

    SendOnlySerial.printDigit(0xf5);   // prints "5".
    SendOnlySerial.printDigit(0x2e);   // prints "e".

`SendOnlySerial` has a few macros, usable unless `NDEBUG` is defined. Use these macros "bare", i.e. without putting `SendOnlySerial.` in front:-

**printVar(integer-variable)**:  prints a line with the name of the variable and its contents, in decimal and hexadecimal.

    int count = 73;
    printVar(count);    // prints "count    73   0x49"

**printFloatVar(float-variable)**: the same for floating-point type variables, in decimal only.

**printReg(REGMACRO)**:  prints a line with the name and contents of the given ATmega register (or other byte variable), in binary, hexadecimal, and decimal.

    printReg(UBRR0L);  // prints: "UBRR0L  0b1100 1111      0xcf      207"

With `NDEBUG` #defined, these macros do nothing. You may need to `#undef NDEBUG` to use them where required.


## SendOnlySerial Functions

|Function              |Remarks                                                                                 |
|----------------------|----------------------------------------------------------------------------------------|
|`begin(BAUDRATE)`     |default is  9600.                                                                       |
|`end()`               |disables the hardware and turns it off, saving a few microamps                          |
|`flush()`             |flush waits for the last byte to be transmitted by the USART hardware.                  |
|`print()`, `println()`|print most types of data in readable format.                                            |
|`printBinary()`       |print a byte as a fixed length string of form "0b0011 1010".                            |
|`printDigit()`        |print the lower 4 bits of the given byte as a single hexadecimal character 0-9,a-f.     |
|`printP()`, `printlnP()`|print strings stored in program memory (flash).                                       |
|`write()`             |send individual characters(`write(c)`) or blocks of bytes (`write(array, sizeOfArray)`) without making them readable.|


### To print strings from program memory (flash) (so they don't use any RAM)

    static const char infoString[] PROGMEM = "InfoInfoInfoInfo";

    SendOnlySerial.printlnP(infoString);  // no square brackets on the name


All the above functions are members of the `SendOnlySerial` object. Use `SendOnlySerial.begin();`, and so on.

You might like to define shorthand macros to save on typing and clutter in your code:

      #define SOS SendOnlySerial   // now can use SOS.begin(), SOS.println()

      #define flashString(stringName, stringValue) static const char stringName[] PROGMEM = stringValue
      // now can use: flashString(infoString2, "InfoInfoInfo");


### Note on Floating-Point

If you print floating-point numbers in readable format, `SendOnlySerial` uses an extra 2kb-ish of flash memory and 28 bytes of RAM, in the AVR-libC standard library function `dtostrf()` for formatting floating-point numbers.  Try to avoid doing that.


## Limitations

Besides those listed above? :-)   You have to wait.

Because Arduino claims the "USART Data Register Empty" interrupt for its Serial object, all of SendOnlySerial's functions are blocking, meaning that your program waits while they do their thing.

`print()` and `printP()`, `println` and `printlnP()` mostly wait for the hardware to trundle the bits and bytes out over the wire, and they will return to your code only when the last byte has been handed off to the ATmega's internal hardware serial module for transmission. `flush()` waits for the hardware to tell us that that last byte has been sent.

## Installation

Click the green "Code" button, and choose "Download zip".  Unzip the downloaded zip file into your Arduino "libraries" folder inside your sketchbook folder.

## Usage

Include the file at the top of your sketch:

    #include "SendOnlySerial.h"

Then use the functions and macros described above, seasoning to taste.

## TODO

as at 2025-08-08  GvP.

### Maybe

Adapt to support the ATtiny44/84, ATtiny85, ATmega2560, and/or ATmega1284P microcontrollers.

Document functions more thoroughly.

Measure flash and RAM consumption more rigorously.


### Unlikely

Support the `F()` macro in `print()` functions.  This seems tricky.

Support other parities, stop bits, and error checking.

Support timeout. (The obvious way would use 8 bytes of RAM.)

Non-blocking transmit functions, controlled with a "WANT" define, for use outside the Arduino environment.
