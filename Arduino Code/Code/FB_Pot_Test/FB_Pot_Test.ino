/*
     FB22_TCU_rev2.ino
     Formula Buckeyes iTCU (Transmission Control Unit) Program
     Author: Aaron Chen - Electronics Lead 2021 - 2024
     Website: https://github.com/LightningAC
*/
#include <iTCU.h>
#include <SPI.h>


// Gear motor position potentiometer
#define potPin A3   // Change as needed
int potVal = 0;

// Motor controller pin definitions
#define ENA 8 
#define PWNA1 10
#define PWNA2 9 


iTCU gearMotor(ENA, PWNA1, PWNA2);

int speed = 255;


void setup() {
  Serial.begin(9600);

  gearMotor.setSpeed(speed);   
}

void loop() {
  gearMotor.forward();
  delay(500);            // Run motor for 500 ms

  gearMotor.stop();
  delay(5000);           // Read potentiometer position 
  Serial.print("Pot Position");
  Serial.print(" ");
  Serial.println(potVal);

  gearMotor.backward();
  delay(500);           // Run motor in other direction for 500 ms

  gearMotor.stop();
  delay(5000);          // Read potentiometer position 
  Serial.print("Pot Position");
  Serial.print(" ");
  Serial.println(potVal);


}