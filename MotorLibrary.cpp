//
//  MotorLibrary.cpp
//  BankomArt
//
//  Created by Александр Баталов on 25.12.14.
//  Copyright (c) 2014 Александр Баталов. All rights reserved.
//

#include "MotorLibrary.h"
#include "LocalLibrary.h"

// Set pins in setSpeedBegin()
int directionPinForward = NULL,
    directionPinBackward = NULL;

int directionPrev = NULL;
float speedPrev = NULL;

void setSpeedBegin(int _directionPinForward, int _directionPinBackward) {
    directionPinForward = _directionPinForward;
    directionPinBackward = _directionPinBackward;
    
    pinMode(directionPinForward, OUTPUT);
    pinMode(directionPinBackward, OUTPUT);
}

void setSpeed(float speed = 1, int direction = 1) {
    speed = max(0, speed);
    speed = min(1, speed);
    
    speed = (int) 255 * speed;
    
    if (direction != directionPrev || speed != speedPrev) {
        digitalWrite(directionPinForward, 0);
        digitalWrite(directionPinBackward, 0);
        
        if (direction > 0) {
            analogWrite(directionPinForward, speed);
        }
        else {
            analogWrite(directionPinBackward, speed);
        }
        
        directionPrev = direction;
        speedPrev = speed;
        
        DEBUG_PRINT("Dir: " + (String)directionPrev + " " + "Speed: " + (String)speedPrev);
    }
}