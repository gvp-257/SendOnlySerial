# SendOnlySerial.h

Lightweight alternative to Arduino's Serial for debugging or logging. Down to 0 bytes vs. Serial's 175 bytes RAM. Uno, Nano, Pro Mini, and breadboard Arduinos using the Atmega328P chip only.

SendOnlySerial uses the TX0 "hardware serial" line which is also connected to the USB interface on Unos and Nanos, just like Serial. Don't use Serial together with SendOnlySerial.  But if you want those 175 bytes, read on.


    #include "SendOnlySerial.h"


## What's Been Removed?

SendOnlySerial is limited to 8N1 data frames. (8 data bits, no parity, 1 stop bit. 8N1 is by far the most common data frame format.) `SendOnlySerial.begin()` defaults to 9600 baud. Uses "CR and LF" line endings. No error checking or timeout.

SendOnlySerial is only capable of *sending* data to the PC. There are no `read()` or `available()` or `parseXxx()` or `findXxx()` or `setTimeout()` functions. Arduino's `String` objects are not supported. (If you're considering using this library, Arduino `String` objects use too much RAM for you anyway.)

## So What's Left, Then?

`print()` and `println()`, mainly. And even those are a bit limited: Besides `String` objects, there is no support for Arduino's `F()` macro for strings in flash (program memory).

Printing strings stored in flash is possible, but slightly awkward. You have to give each one a name, and use a "printP" function instead of "print". Like so:-


    static const char aFlashString[] PROGMEM = "The string you want to print";
    //  different name for each string. Note the square brackets (array).

    SendOnlySerial.printlnP(aFlashString);
    // Note the 'P': -----^ and no []s: ^


## Anything Added?

`SendOnlySerial` does have a few macros of its own, usable unless `NDEBUG` is defined:

**printVar(integer-variable)**:  prints a line with the name of the variable and its contents, in decimal and hexadecimal.

**printFloatVar(float-variable)**: the same for floating-point type variables, in decimal only.

**printReg(REGMACRO)**:  prints a line with the name and contents of the given ATmega register, in binary, hexadecimal, and decimal.

Use these macros "bare", i.e. without putting `SendOnlySerial.` in front:

    int count = 74;
    printVar(count);  // prints: count    74    0x4a

With `NDEBUG` #defined, these macros do nothing. (You may need to `#undef NDEBUG` to use them.)

## SendOnlySerial Functions


|Function              |Remarks                                                                                 |
|----------------------|----------------------------------------------------------------------------------------|
|`begin(BAUDRATE)`     |default is  9600.                                                                |
|`end()`               |disables the hardware and turns it off, saving a few microamps                          |
|`flush()`             |flush waits for the last byte to be transmitted by the USART hardware.                  |
|`print()`, `println()`|print most types of data in readable format.                                            |
|`printP()`, `printlnP()`|print strings stored in program memory (flash). |
|`write()`             |send individual characters(`write(c)`) or blocks of bytes (`write(array, sizeOfArray)`) without making them readable.|


### For Strings in program memory (flash)

    static const char infostring[] PROGMEM = "InfoInfoInfoInfo";

    SendOnlySerial.printlnP(infostring);  // no square brackets on the name


All the above functions are members of the `SendOnlySerial` object.  `SendOnlySerial.begin();`, and so on.

You might like to define shorthand macros to save on typing and clutter in your code:

      #define SOS SendOnlySerial   // now can use SOS.begin(), SOS.println()

      #define flashString(stringName, stringValue) static const char stringName[] PROGMEM = stringValue
      // now can use: flashString(info, "InfoInfoInfo");


### Note on Floating-Point

If you print floating-point numbers in readable format, `SendOnlySerial`  uses an extra 2kb-ish of flash memory and 28 bytes of RAM, in the AVR-libC standard library function `dtostrf()` for formatting floating-point numbers.  Try to avoid doing that.


## Limitations

Besides those listed above? :-)

Because Arduino claims the "USART data register empty" interrupt for the Serial object, all of SendOnlySerial's functions are blocking. `print()` and `printP()`, `println` and `printlnP()` mostly wait for the hardware to trundle the bits and bytes out over the wire, and they will return to your code only when the last byte has been handed off to the ATmega's internal hardware serial module for transmission. `flush()` waits for the hardware to tell us that that last byte has been sent.


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
