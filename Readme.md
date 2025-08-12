#dtos.h

The ""Debug TO Serial" library, or the "De-featured Transmit-Only Serial" library.


    #define   BAUD    100000  // baud rate for hardware serial transmission
    #include "dtos.h"


A low memory library for the hardware serial port on ATmega328Ps:
an alternative to Arduino's `Serial` object for the common use case of sending
data to the Arduino IDE's Serial Monitor.

For Arduino Uno, Nano, Pro Mini, Duemilanove and "breadboard Arduinos" using the ATmega168/328P only.

Limited to 8N1 data frames. (8 data bits, no parity, 1 stop bit. 8N1 is the most common data frame format.) Uses a macro named BAUD to set the baud rate, as shown above; defaults to 9600 baud. Uses "CR and LF" line endings. No error checking or timeout.

Only capable of *sending* data to the PC. There are no `read()` or `parseXxx()` or `findXxx()` or `timeout` functions. Arduino's `String` objects are not supported.

So what's left? Print and println, mainly.


|Function              |Remarks                                                                                 |
|----------------------|----------------------------------------------------------------------------------------|
|`begin()`             |no baudrate parameter, it's #defined, as above.                                         |
|`end()`               |disables the hardware and turns it off, saving a few microamps                          |
|`flush()`             |flush waits for the last byte to be transmitted by the USART hardware.                  |
|`print()`, `println()`|print most types of data in readable format.                                            |
|`write()`             |send individual characters(`write(c)`) or blocks of bytes (`write(array, sizeOfArray)`).|


### Strings in program memory (flash)

`Print` and `println` don't currently support Arduino's `F()` macro, because the author can't (yet) understand C++'s `reinterpret_cast` feature. Instead there are


| Function               |Remarks                                                          |
|------------------------|-----------------------------------------------------------------|
|`printP()`, `printlnP()`|print a string that has been defined with the PROGMEM attribute:-|
|                        |static const char infostring[] PROGMEM = "InfoInfoInfoInfo"; DebugSerial.printlnP(infostring);  // no square brackets on the name|


All the above functions are members of the `DebugSerial` object.  `DebugSerial.begin();`, and so on.

You might like to define shorthand macros to save on typing and clutter in your code:

      #define ds DebugSerial   // now can use ds.begin(), ds.println()

      #define flashstring(name, value) static const char name[] PROGMEM = value
      // now can use: flashstring(info, "InfoInfoInfo");



##Motivation

Arduino's built-in `Serial` object uses 175 bytes of precious SRAM on the ATmega328P as soon as you do `Serial.begin(BAUDRATE)`. This on top of the strings that you want to print to the serial monitor.

`dtos` uses none for itself, unless you want to print floating-point numbers as text to the serial port.  `dtos` uses about 1200 bytes of flash in simple cases.


###Note on floating-point

If you print floating-point numbers in readable format, `dtos`  uses an extra 2kb-ish of flash memory and 28 bytes of RAM, in the AVR-libC standard library function `dtostrf()` for formatting floating-point numbers.  Try to avoid doing that.



##Limitations

Besides those listed above? :-)

Because Arduino claims the "USART data register empty" interrupt for the Serial object, all `print` and `write` functions are blocking. They will return to your code when the last byte has been handed off to the ATmega's hardware for transmission. `flush()` waits for that last byte to be transmitted.


##TODO

as at 2025-08-08  GvP.

###Maybe

Adapt to support the ATtiny44/84, ATtiny85, ATmega2560, Atmega1284P microcontrollers.

Document functions more thoroughly.

Measure flash and RAM consumption more rigorously.

Support the `F()` macro in `print()` functions.

Change `begin()` to match Arduino's `Serial.begin(baudrate)`, and remove the requirement to define BAUD before including the library.

More convenience macros for printing stuff to the serial monitor.


###Unlikely

Support other parities, stop bits, and error checking.

Support timeout. (The obvious way would use 8 bytes of RAM.)

Non-blocking transmit functions, controlled with a "WANT" define, for use outside the Arduino environment.
