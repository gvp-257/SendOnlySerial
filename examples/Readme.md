# Send Only Serial Examples

### SimpleUsage

Demonstrates using a higher baud rate (250000), `print`ing strings and numbers, and writing a block of bytes - the `write` function, usually used for non-text data.

### MemoryComparison

Report memory usage of SendOnlySerial versus Arduino's Serial for printing strings and numbers.

With Arduino IDE 2.3.6, the numbers I obtained are:

|        bytes:|    FLASH|    SRAM|
|--------------|---------|--------|
|Serial        |     3484|     188|
|SendOnlySerial|     2904|      15|



### DebuggingExample

Shows the debugging macros `printVar`, `printFloatVar`, and `printReg` (print register).