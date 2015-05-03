#include <Arduino.h>

#ifndef local_lib_h
#define local_lib_h

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
    #define DEBUG false
#endif

#ifndef DEBUG
    #define DEBUG true
    #define NoteAcceptorSerial Serial3
#else
    #define NoteAcceptorSerial Serial
#endif

#ifndef NO_NOTE_ACCEPTOR
    #define NO_NOTE_ACCEPTOR false
#endif

#if DEBUG
    #define DEBUG_PRINT(x) Serial.println(x)
    #define DEBUG_ECHO(x) Serial.print(x)
    #define DEBUG_SETUP() ({ Serial.begin(9600); })
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_ECHO(x)
    #define DEBUG_SETUP()
#endif

void blink(uint8_t pin, uint8_t times, uint16_t ms);

void setupNoteAcceptor();

int readNoteAcceptor();

void writeNoteAcceptor(int data);

void setNoteAcceptorOption(int optionCode);

void enableNoteAcceptor();

void disableNoteAcceptor();

void enableNoteAcceptorESCROW();

#endif