# Send Only Serial Examples

### SimpleUsage

Demonstrates using the default baud rate (9600), `print`ing strings and numbers, and writing a block of bytes - the `write` function, usually used for non-text data.

### MemoryComparison

Report memory usage of SendOnlySerial versus Arduino's Serial for printing strings and numbers.

With Arduino IDE 2.3.6 and an Uno clone, the numbers I obtained are:

|        bytes:|    FLASH|    SRAM|
|--------------|---------|--------|
|Serial        |     3484|     188|
|SendOnlySerial|     2904|      15|


Note: Arduino uses 9 bytes of SRAM at the bare minimum, for the millis() and micros() functions.

### Shorthand

Demonstrates convenience functions for printing commonly used characters, like comma, dash, dot, colon, and tab.


### DebuggingExample

Shows the debugging macros `printVar`, `printFloatVar`, and `printReg` (print register).