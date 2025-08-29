#include <Arduino.h>
// Test the convenience functions for printing common characters

#include "SendOnlySerial.h"

#ifndef PRINTFLASHSTRING
#define PRINTFLASHSTRING(name, value) \
do { \
static const char name[] PROGMEM = value; \
SendOnlySerial.printlnP(name); \
} while (0)
#endif


#define ss SendOnlySerial
void setup() {
    ss.begin(9600);

    PRINTFLASHSTRING(TestTitle,"Test of convenient single-character print functions in SendOnlySerial.");
    ss.println();

    ss.print("slash: "); ss.slash();
    ss.print("comma: "); ss.comma();
    ss.print("colon: "); ss.colon();
    ss.print("dash:  "); ss.dash();
    ss.print("minus: "); ss.minus();
    ss.print("dot:   "); ss.dot();
    ss.print("star:  "); ss.star();
    ss.print("dquote:"); ss.dquote();
    ss.print("tab:   "); ss.tab();
    ss.print("dollar:"); ss.dollar();
    ss.print("apos:  "); ss.apos();
    ss.print("lparen:"); ss.lparen();
    ss.print("rparen:"); ss.rparen();
    ss.print("crlf:  "); ss.crlf();
    ss.print("langle:"); ss.langle();
    ss.print("rangle:"); ss.rangle();
    ss.print("at:    "); ss.at();
    ss.print("vbar:  "); ss.vbar();
    ss.print("qmark:");  ss.qmark();

    ss.flush();
}

void loop() {}
