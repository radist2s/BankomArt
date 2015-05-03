//
//  MotorLibrary.h
//  BankomArt
//
//  Created by Александр Баталов on 25.12.14.
//  Copyright (c) 2014 Александр Баталов. All rights reserved.
//

#include <Arduino.h>

void setSpeedBegin(int _directionPinForward, int _directionPinBackward);

void setSpeed(float speed, int direction);
