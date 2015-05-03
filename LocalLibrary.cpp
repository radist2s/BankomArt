//
// LocalLibrary.cpp 
// Library C++ code
// ----------------------------------
// Developed with embedXcode 
// http://embedXcode.weebly.com
//
// Project 		Arduino embedXcode Test
//
// Created by 	Александр Баталов, 20.12.14 0:17
// 				Александр Баталов
//	
// Copyright 	© Александр Баталов, 2014
// Licence 		<#license#>
//
// See 			LocalLibrary.cpp.h and ReadMe.txt for references
//


#include "LocalLibrary.h"

void blink(uint8_t pin, uint8_t times, uint16_t ms) {
  for (uint8_t i=0; i<times; i++) {
    digitalWrite(pin, HIGH);
    delay(ms >> 1);               
    digitalWrite(pin, LOW);  
    delay(ms >> 1);              
  }
}

int readNoteAcceptor() {
    if (NO_NOTE_ACCEPTOR) {
        return NULL;
    }
    
    if (NoteAcceptorSerial.available() <= 0) {
        return NULL;
    }
    
    int sData = NoteAcceptorSerial.read();
    
    return sData;
}

void writeNoteAcceptor(int data) {
    NoteAcceptorSerial.write(data);
}

void setNoteAcceptorOption(int optionCode) {
    if (NO_NOTE_ACCEPTOR) {
        return;
    }
    
    unsigned long time, timeTx = NULL;

    while (true) {
        time = millis();
        
        if (timeTx == NULL or time - timeTx > 500) {
            timeTx = time;
            NoteAcceptorSerial.flush();
            NoteAcceptorSerial.write(optionCode);
        }
        
        if (NoteAcceptorSerial.available() > 0) {
            if (optionCode = (int)NoteAcceptorSerial.read()) {
                break;
            }
        }
    }
}

void enableNoteAcceptor() {
    setNoteAcceptorOption(184);
}

void disableNoteAcceptor() {
    setNoteAcceptorOption(185);
}

void enableNoteAcceptorESCROW() {
    setNoteAcceptorOption(170);
}

void setupNoteAcceptor() {
    if (NO_NOTE_ACCEPTOR) {
        return;
    }
    
    NoteAcceptorSerial.begin(9600);
    
    unsigned long time, timeTx = NULL;
    bool infoReceivingStart = false, infoReceivingComplete = false;
    
    delay(1000);
    
    while (infoReceivingComplete == false) {
        time = millis();
        
        if (infoReceivingStart == false and (timeTx == NULL or time - timeTx > 500)) {
            timeTx = time;
            NoteAcceptorSerial.flush();
            NoteAcceptorSerial.write(182);
            
            DEBUG_PRINT("Info Req");
        }
        
        int serialDataLength = NoteAcceptorSerial.available();
        
        if (serialDataLength > 0) {
            if (infoReceivingStart == false) {
                int data = NoteAcceptorSerial.read();
                
                if (data == 182) {
                    infoReceivingStart = true;
                    
                    DEBUG_PRINT("DataIn_S: " + (String)data);
                    
                    delay(500);
                }
            }
            else {
                DEBUG_PRINT("Note info received");
                
                infoReceivingComplete = true;
            }
        }
    }
    
    delay(100);
    
    DEBUG_PRINT("Note Info Complete");

    delay(100);
    
    // Enable all channels
    enableNoteAcceptor();
    
    delay(100);

    DEBUG_PRINT("Enable all");
    
    delay(100);
    
    // Enable ESCROW
    enableNoteAcceptorESCROW();
    DEBUG_PRINT("ESCROW enabled");
}
