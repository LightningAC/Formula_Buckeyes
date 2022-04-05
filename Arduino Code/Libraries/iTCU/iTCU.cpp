/*
     iTCU.cpp 
     Formula Buckeyes iTCU (Transmission Control Unit) Motor Controller Library
     Author: Aaron Chen - Electronics Lead 2021 - 2024
     Website: https://github.com/LightningAC
*/

#include "Arduino.h"
#include "iTCU.h"

typedef void (*CallBackFunction) ();

iTCU::iTCU(uint8_t enPin, uint8_t pwnPin1, uint8_t pwnPin2) {
        pinMode(en, OUTPUT);
        pinMode(pwnPin1, OUTPUT);
        pinMode(pwnPin2, OUTPUT);

        _enPin = enPin;
        _pwnPin1 = pwnPin1;
        _pwnPin2 = pwnPin2;
        _pwmVal = 100;
        _isMoving = false;
        _canMove = true;
        _lastMs = 0;
        _activePin = _pwnPin1

        pinMode(_enPin, OUTPUT);
        pinMode(_pwnPin1, OUTPUT);
        pinMode(_pwnPin2, OUTPUT);
        pinMode(_activePin, OUTPUT);
}

void iTCU::forward() {
        _activePin = _pwnPin1;
        analogWrite(_pwnPin2, 0);
        digitalWrite(_enPin, HIGH);
        
        _isMoving = true;
}

void iTCU::forwardFor(unsigned long delay, CallBackFunction callback){
  if ((_lastMs == 0) && _canMove) {
    _lastMs = millis();
    this->forward();
  }

  if (((millis() - _lastMs) > delay) && _canMove) {
    this->stop();
    _lastMs = 0;
    _canMove = false;

    callback();

  }
}

void iTCU::forwardFor(unsigned long delay){
  this->forwardFor(delay, fakeCallback);
}

void iTCU::backward() {
        _activePin = _pwnPin2;
        analogWrite(_pwnPin1, 0);
        digitalWrite(_enPin, HIGH);
        
        _isMoving = true;
}

void iTCU::backwardFor(unsigned long delay, CallBackFunction callback){

  if ((_lastMs == 0) && _canMove) {
    _lastMs = millis();
    this->backward();
  }

  if (((millis() - _lastMs) > delay) && _canMove) {
    this->stop();
    _lastMs = 0;
    _canMove = false;

    callback();

  }
}

void iTCU::backwardFor(unsigned long delay){
  this->backwardFor(delay, fakeCallback);
}

void iTCU::stop() {
        digitalWrite(_enPin, LOW);

        _isMoving = false;
}

void iTCU::reset(){
  _canMove = true;
}

boolean iTCU::isMoving() {
        return _isMoving;
}

void iTCU::fakeCallback(){

}
