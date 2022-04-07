/*
     FB22_TCU_rev2.ino
     Formula Buckeyes iTCU (Transmission Control Unit) Program
     Author: Aaron Chen - Electronics Lead 2021 - 2024
     Website: https://github.com/LightningAC
*/
#include <iTCU.h>
#include <SPI.h>

// Motor controller pin definitions
#define ENA 9 
#define PWNA1 10
#define PWNA2 11 
#define ENB 4
#define PWNB1 5
#define PWNB2 6

iTCU gearMotor(ENA, PWNA1, PWNA2);
iTCU clutchMotor(ENB, PWNB1, PWNB2);

// Gear motor position potentiometer
#define gearPotPin A0
int gearPotVal = 0;

// Sensing pins and inital values
int CSRA = A0;
int CSRB = A1;
int thermistor = A4;

int currRawA = 0;
int currRawB = 0;
int currCalcA = 0;
int currCalcB = 0;
int tempRaw = 0;
int tempCalc = 0;
int resist = 0.1;           // Current sensing resistor value


// Direct wired paddle inputs
#define upRequest A4
#define downRequest A5
int upReqVal = 0;
int downReqVal = 0;
int upShift = 0;
int downShift = 0;


// Input variables for paddle shifters
int isUpshift = 0;
int isDownshift = 0;

// Buffer
int shiftBuffer[32];


// Motor timing & speed values
int gearSpeedCW = 255;
int gearSpeedCCW = 255;
int gearUpTime = 1000;
int gearDownTime = 1000;
int clutchSpeed = 255;
int clutchTime = 500;
int shiftInput = 0;

// Misc variables
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;


enum : byte {
  UPSHIFT,
  DOWNSHIFT,
  DO_NOTHING,
  TAKE_A_SCREENSHOT
} shiftState;
int input[2] = {0,0};

bool completeShift = false;
bool completeUpA = false;
bool completeUpB = false;
bool completeDownGearA = false;
bool completeDownGearB = false;
bool completeDownClutchA = false;
bool completeDownClutchB = false;


void setup() {
  Serial.begin(9600);

  pinMode(upRequest, INPUT);
  pinMode(downRequest, INPUT);
  
  pinMode(gearPotPin, INPUT);
  
  gearMotor.setSpeed(gearSpeedCW);
  clutchMotor.setSpeed(clutchSpeed);

  shiftState = DO_NOTHING;
  Serial.println("Begin program");
  
}

void loop() {
  //Serial.println("Main loop");


  // Comment out these lines when using CAN. This is for direct inputs
  upReqVal = analogRead(A4);
  downReqVal = analogRead(A5);

  if (upReqVal < 300) {
    upShift = 1;
  }
  else {
    upShift = 0;
  }
  if (downReqVal < 300) {
    downShift = 1;
  }
  else {
    downShift = 0;
  }
  Serial.print("Up Shift = ");
  Serial.print(upShift);
  Serial.print(" | Down Shift = ");
  Serial.println(downShift);
  input[0] = upShift;
  input[1] = downShift;

  
  if (input[0] == 1 && input[1] == 1){
    shiftState = TAKE_A_SCREENSHOT;
  }
  else if (input[0] == 1) {
    shiftState = UPSHIFT;
  }
  else if (input[1] == 1) {
    shiftState = DOWNSHIFT;
  }
  else {
    DO_NOTHING;
  }


completeShift = false;
completeUpA = false;
completeUpB = false;
completeDownGearA = false;
completeDownGearB = false;
completeDownClutchA = false;
completeDownClutchB = false;


    
previousMillis = millis();
// Shifting controls
switch(shiftState) {
  case UPSHIFT:
    Serial.println("UPSHIFT");
    gearMotor.forward();
    
    while(!completeShift) {
    
    if (millis() - previousMillis > gearUpTime && completeUpA == false && completeUpB == false) {
      Serial.println("Upshift sequence A");
      gearMotor.stop();
      previousMillis = millis();
      completeUpA = true;
      gearMotor.backward();
    }
    else if (millis() - previousMillis > gearUpTime/2 && completeUpA == true && completeUpB == false) {
      Serial.println("Upshift sequence B");
      gearMotor.stop();
      previousMillis = millis();
      completeUpB = true;
    }

    if (completeUpA == true && completeUpB == true) {
      Serial.println("Upshift complete");
      completeShift = true;
    }
    }
   
    shiftState = DO_NOTHING;

  break;

  case DOWNSHIFT:
    Serial.println("DOWNSHIFT");
    gearMotor.backward();
    clutchMotor.forward();

    while(!completeShift) {
    if ((millis() - previousMillis > gearDownTime || millis() - previousMillis > clutchTime) && (completeDownGearA == false || completeDownClutchA == false) && completeDownGearB == false && completeDownClutchB == false) {
      if (millis() - previousMillis > gearDownTime && completeDownGearA == false) {
        Serial.println("Downshift gear sequence A");
        gearMotor.stop();
        completeDownGearA = true;
      }

      else if (millis() - previousMillis > clutchTime && completeDownClutchA == false) {
        Serial.println("Downshift clutch sequence A");
        clutchMotor.stop();
        completeDownClutchA = true;
      }
      if(completeDownGearA == true && completeDownClutchA == true && completeDownGearB == false && completeDownClutchB == false) {
        Serial.println("Downshift gear and clutch sequence complete");
        previousMillis = millis();  
        gearMotor.forward();
        clutchMotor.backward();    
      }
    }

    else if ((millis() - previousMillis > gearDownTime || millis() - previousMillis > clutchTime) && completeDownGearA == true && completeDownClutchA == true && (completeDownGearB == false || completeDownClutchB == false)) {
       if (millis() - previousMillis > gearDownTime/2 && completeDownGearB == false) {
        Serial.println("Downshift gear sequence B");
        gearMotor.stop();
        completeDownGearB = true;
      }

      else if (millis() - previousMillis > clutchTime && completeDownClutchB == false) {
        Serial.println("Downshift clutch sequence B");
        clutchMotor.stop();
        completeDownClutchB = true;
      }
      if(completeDownGearB == true && completeDownClutchB == true) {
        Serial.println("Downshift gear and clutch sequence B");
        previousMillis = millis();  
        gearMotor.stop();
        clutchMotor.stop();    
      }
    }

    if (completeDownGearB == true && completeDownClutchB == true && completeDownGearB == true && completeDownClutchB == true) {
      Serial.println("Downshift sequence complete");
      completeShift = true;
    }
    }
    shiftState = DO_NOTHING;
  break;

  case TAKE_A_SCREENSHOT:
    Serial.println("Screenshot Taken");
  break;

  case DO_NOTHING:
    
  break;

  default:
    
  break;
}
}














  

// Returns true if current sensor detects an overcurrent condition
bool overcurrent(long current) {
  long current_max = 2;        // maximum current threshold; change accordingly
  
  if (current >= current_max){
    return true;          // current threshold is met or exceeded
  }
  else {
    return false;
  }
}


// Returns true if temp sensor detects an overheating condition
bool overheat(long temp) {
  long temp_max = 2;         // maximum temperature threshold; change accordingly

  if (temp >= temp_max) {
    return true;        // temperature threshold is met or exceeded
  }
  else {
    return false;
  }
}


void estop(){
  Serial.println();
  Serial.println("***Error: Overcurrent/Overheat detected ***");
  Serial.println();
  gearMotor.setSpeed(0);
  clutchMotor.setSpeed(0);
  
  gearMotor.stop();
  clutchMotor.stop();

  //while(!errorAcknowledged(digitalRead(ack_button)));      // Block the program until error is acknowledged
}


// returns true if error acknowledged button is pressed
bool errorAcknowledged(int input) { 
   if (input == 1) {
      return true;
   }
   else{
      return false;
   }
}
