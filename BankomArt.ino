// 
// BankomArt 
//
// Description of the project
// Developed with [embedXcode](http://embedXcode.weebly.com)
// 
// Author	 	Александр Баталов
// 				Александр Баталов
//
// Date			24.12.14 12:25
// Version		<#version#>
// 
// Copyright	© Александр Баталов, 2014
// Licence		<#license#>
//
// See			ReadMe.txt for references
//

#include <Arduino.h>
#include <arduino.h>

#include "LocalLibrary.h"
#include "MotorLibrary.h"
#include <Bounce2.h>

const int
    PAPER_TAKE_BTN_PIN = 10, // 7
    PAPER_TAKE_BTN_HIGHLIGHT_RED_PIN = 12, // 5
    PAPER_TAKE_BTN_HIGHLIGHT_GRN_PIN = 11, // 6
    PAPER_IR_SWITCH_ANALOG_PIN = 0,
    PAPER_IR_SWITCH_ENABLE_PIN = 7, // 11
    PAPER_TAKE_BTN_FORCE_ENABLE_BTN_PIN = 4, // 8
    MOTOR_FWD_PIN = 5, // 9
    MOTOR_BWD_PIN = 6; // 10

const int PAPER_SWITCH_SIGNAL_THRESHOLD = 700;

unsigned long time, ledBlinkTime = NULL;

const float motorRollingSpeed = 1, motorGrabSpeed = 1;

bool
    paperIn = false,
    canTakePaper = false,
    canAcceptNotes = true,
    escrowInProgress = false;

Bounce takePaperBtn = Bounce();
Bounce forceEnableTakePaperBtn = Bounce();

// Define variables and constants
//
// Brief	Name of the LED
// Details	Each board has a LED but connected to a different pin
//

//
// Brief	Setup
// Details	Define the pin the LED is connected to
//

const int
    ESCROW_ACCEPT_CODE = 172,
    ESCROW_REJECT_CODE = 173;

// Accept ESCROW
void acceptNote(bool validatedMode = false) {
    if (validatedMode) {
        setNoteAcceptorOption(ESCROW_ACCEPT_CODE);
    }
    else {
        writeNoteAcceptor(ESCROW_ACCEPT_CODE);
    }
}

// Reject ESCROW
void rejectNote(bool validatedMode = false) {
    if (validatedMode) {
        setNoteAcceptorOption(ESCROW_REJECT_CODE);
    }
    else {
        writeNoteAcceptor(ESCROW_REJECT_CODE);
    }
}

void disablePaperMotorButton() {
    canTakePaper = false;

    digitalWrite(PAPER_TAKE_BTN_HIGHLIGHT_GRN_PIN, LOW);
    digitalWrite(PAPER_TAKE_BTN_HIGHLIGHT_RED_PIN, HIGH);
    
    DEBUG_PRINT("Paper motor Disabled");
}

int takePaper() {
    int error = 0;
    unsigned long time, paperRollingTime, paperGrabTime = NULL, startTime;
    
    digitalWrite(PAPER_IR_SWITCH_ENABLE_PIN, HIGH);
    
    startTime = time = millis();
    
    while (true) {
        int paperInSwitchSignal = analogRead(PAPER_IR_SWITCH_ANALOG_PIN);
        
        if (paperInSwitchSignal < PAPER_SWITCH_SIGNAL_THRESHOLD) { // Paper grabed and rolling
            setSpeed(motorRollingSpeed, -1);
            
            paperRollingTime = time;
            
            if (paperGrabTime == NULL) { // If paper was grabed early
                paperGrabTime = time;
            }
            
            paperIn = true;
            
            if (time - paperGrabTime > 5000) {
                setSpeed(0, -1);
                
                DEBUG_PRINT("Paper jam");

                error = 2;
                
                break;
            }
        }
        else {
            if (paperIn) { // Paper near the exit
                if (time - paperRollingTime > 3000) { // Waite while paper in printer, then stop
                    paperIn = false;
                    setSpeed(0, -1);
                    
                    break;
                }
            }
            else {
                setSpeed(motorRollingSpeed, 1); // Start paper grabing
                paperGrabTime = time;
                
                if (time - startTime > 5000) {
                    setSpeed(0, 1);
                    
                    DEBUG_PRINT("No paper inside");
                    
                    error = 1;
                    
                    break;
                }
            }
        }
        
        time = millis();
    }
    
    digitalWrite(PAPER_IR_SWITCH_ENABLE_PIN, LOW);
    
    if (error) {
        DEBUG_ECHO("Paper taken error: ");
        DEBUG_PRINT(error);
    }
    else {
        DEBUG_PRINT("Paper taken success");
    }
    
    return error;
}

bool grabPaper() {
    int error = 0;
    unsigned long time, startTime;

    digitalWrite(PAPER_IR_SWITCH_ENABLE_PIN, HIGH);
    startTime = time = millis();
    
    while (true) {
        int paperInSwitchSignal = analogRead(PAPER_IR_SWITCH_ANALOG_PIN);
        
        if (paperInSwitchSignal < PAPER_SWITCH_SIGNAL_THRESHOLD) { // Paper grabed
            setSpeed(0, -1); // Stop motor
            
            break;
        }
        else {
            setSpeed(motorGrabSpeed, 1); // Start paper grabing
            
            if (time - startTime > 5000) {
                setSpeed(0, 1);
                
                DEBUG_PRINT("No paper inside");
                
                error = 1;
                
                break;
            }
        }
        
        time = millis();
    }
    
    digitalWrite(PAPER_IR_SWITCH_ENABLE_PIN, LOW);
    
    if (error == 0) {
        DEBUG_ECHO("Paper grabing error: ");
        DEBUG_PRINT(error);
        
        return true;
    }
    else {
        DEBUG_PRINT("Paper grabing success");
        
        return false;
    }
}

void enablePaperMotorButton() {
    DEBUG_PRINT("Paper motor Enabled");
    
    digitalWrite(PAPER_TAKE_BTN_HIGHLIGHT_GRN_PIN, HIGH);
    digitalWrite(PAPER_TAKE_BTN_HIGHLIGHT_RED_PIN, LOW);
    
    canTakePaper = true;
}

void serialEventRead() {
    int sData = readNoteAcceptor();
    
    if (sData == NULL) {
        return;
    }

    DEBUG_ECHO("S_EVENT: ");
    DEBUG_ECHO(sData);
    DEBUG_PRINT("; S_DATA__ESCROW: " + (String)escrowInProgress);


    if (ESCROW_ACCEPT_CODE == sData) {
        escrowInProgress = true;
    }
    else if (ESCROW_REJECT_CODE == sData) {
        escrowInProgress = false;
    }
    // Note accepted
    else if (sData > 0 and sData <= 16) {
        if (escrowInProgress != true) {
            if (canAcceptNotes and grabPaper() == true) {
                acceptNote();
            }
            else {
                canAcceptNotes = false;
                
                rejectNote(true);
                disableNoteAcceptor();
            }
        }
        else if (escrowInProgress == true) { // Escrow completed
            escrowInProgress = false;
            
            DEBUG_PRINT("TRANS_COMPLETE");
            
            enablePaperMotorButton();
        }
    }
    // ESCROW aborted by machine
    else if (sData == 70) {
        
    }
    // Request Startup options
    else if (sData == 192) {
        setupNoteAcceptor();
    }
}

void takePaperBtnRead() {
    takePaperBtn.update();
   
    if (canTakePaper != true) {
        return;
    }

    if (takePaperBtn.read() != LOW) {
        return;
    }
    
    DEBUG_PRINT("Take paper Btn On");
    
    canAcceptNotes = false;
    
    disablePaperMotorButton();
    disableNoteAcceptor();
    
    int errorState = takePaper();
    
    if (errorState == 0) {
        canAcceptNotes = true;
        
        enableNoteAcceptor();
        enableNoteAcceptorESCROW();
    }
    
}

// Add setup code
void setup() {
    DEBUG_SETUP();
    
    setSpeedBegin(MOTOR_FWD_PIN, MOTOR_BWD_PIN);
    
    pinMode(13, OUTPUT);
    pinMode(PAPER_IR_SWITCH_ENABLE_PIN, OUTPUT);
    pinMode(PAPER_TAKE_BTN_PIN, INPUT_PULLUP);

    pinMode(PAPER_TAKE_BTN_HIGHLIGHT_RED_PIN, OUTPUT);
    pinMode(PAPER_TAKE_BTN_HIGHLIGHT_GRN_PIN, OUTPUT);
    
    pinMode(PAPER_TAKE_BTN_FORCE_ENABLE_BTN_PIN, INPUT_PULLUP);
   
    takePaperBtn.attach(PAPER_TAKE_BTN_PIN);
    takePaperBtn.interval(5); // interval in ms
    
    forceEnableTakePaperBtn.attach(PAPER_TAKE_BTN_FORCE_ENABLE_BTN_PIN);
    forceEnableTakePaperBtn.interval(5); // interval in ms
    
    disablePaperMotorButton();
    setupNoteAcceptor();
}

//
// Brief	Loop
// Details	Call blink
//
// Add loop code

void loop() {
    serialEventRead();
    
    takePaperBtnRead();
    
    forceEnableTakePaperBtn.update();
    
    if (forceEnableTakePaperBtn.read() == LOW) {
        enablePaperMotorButton();
    }
    
    time = millis();
    
    if (ledBlinkTime == NULL || time - ledBlinkTime > 500) {
        digitalWrite(13, HIGH);
        ledBlinkTime = time;
    }
    else if (time - ledBlinkTime > 250) {
        digitalWrite(13, LOW);
    }
}
