/*
     iTCU.h
     Formula Buckeyes iTCU (Transmission Control Unit) Motor Controller Library
     Author: Aaron Chen - Electronics Lead 2021 - 2024
     Website: https://github.com/LightningAC
*/

#ifndef iTCU_h
#define iTCU_h
#include "Arduino.h"

typedef void (*CallBackFunction) ();

class iTCU {
public:
        iTCU(uint8_t enPin, uint8_t pwnPin1, uint8_t pwnPin2);
        void forward();
        void forwardFor(unsigned long delay, CallBackFunction callback);
        void forwardFor(unsigned long delay);
        void backward();
        void backwardFor(unsigned long delay, CallBackFunction callback);
        void backwardFor(unsigned long delay);
        void stop();
        void setSpeed(unsigned short pwmVal);
        boolean isMoving();
private:
        byte _enPin;
        byte _pwnPin1;
        byte _pwnPin2;
        byte _activePin;
        byte _pwmVal;
        unsigned long _lastMs;
        boolean _isMoving;
        boolean _canMove;
        static void fakeCallback();
};
#endif
