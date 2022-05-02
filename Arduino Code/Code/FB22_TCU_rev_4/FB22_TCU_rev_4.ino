/******************************************************************/
/*    FB22_TCU_rev2.ino                                           */
/*    Formula Buckeyes iTCU (Transmission Control Unit) Program   */
/*    Summary: This program only takes downshift paddle input     */
/*      and steering wheel clutch button input. This is only used */
/*      to control the clutch motor.                              */
/*    Author: Aaron Chen - Electronics Lead 2021 - 2024           */
/*    Website: https://github.com/LightningAC                     */
/******************************************************************/

/*************************** Libraries ****************************/
#include <iTCU.h>
#include <SPI.h>

/*************** Motor Controller Pin Definitions *****************/
#define ENB 4
#define PWNB1 5
#define PWNB2 6

iTCU clutchMotor(ENB, PWNB1, PWNB2);


// Steering wheel inputs
#define clutchPin A4
#define downRequest A5
int clutchVal = 0;
int clutch = 0;
int downReqVal = 0;
int downShift = 0;

// Buffer
int shiftBuffer[32];


// Motor timing & speed values
int clutchSpeed = 255;
int clutchTime = 300;       // Change this to go faster (in milliseconds)
int shiftInput = 0;

enum : byte {
  CLUTCH,
  DOWNSHIFT,
  DO_NOTHING,
  TAKE_A_SCREENSHOT
} shiftState;
int input[2] = {0, 0};


void setup() {
  Serial.begin(9600);

  pinMode(clutchPin, INPUT);
  pinMode(downRequest, INPUT);

  clutchMotor.setSpeed(clutchSpeed);

  shiftState = DO_NOTHING;
  Serial.println("Begin program");

}

void loop() {
  //Serial.println("Main loop");
  downReqVal = analogRead(downRequest);
  clutchVal = analogRead(clutchPin);
  if (downReqVal > 15) {
    downShift = 1;
  }
  else {
    downShift = 0;
  }
  if (clutchVal < 300) {
    clutch = 1;
  }
  else {
    clutch = 0;
  }

  Serial.print("Down Shift Val = ");
  Serial.print(downReqVal);
  Serial.print("Down Shift = ");
  Serial.print(downShift);
  Serial.print(" | ClutchVal = ");
  Serial.print(clutchVal );
  Serial.print(" | Clutch = ");
  Serial.println(clutch );


  input[0] = downShift;
  input[1] = clutch;
  

  if (input[0] == 1 && input[1] == 1) {
    shiftState = TAKE_A_SCREENSHOT;
  }
  else if (input[0] == 1) {
    shiftState = DOWNSHIFT;
  }
  else if (input[1] == 1) {
    shiftState = CLUTCH;
  }
  else {
    DO_NOTHING;
  }

  
    
  // Shifting controls
  switch (shiftState) {
    case DOWNSHIFT:
      Serial.println("DOWNSHIFT");
      clutchMotor.forward();
      delay(clutchTime);
      Serial.println("Downshift A complete");

      clutchMotor.backward();
      delay(clutchTime);
      Serial.println("Downshift B complete");
      clutchMotor.stop();
      Serial.println("Downshift sequence complete");

      shiftState = DO_NOTHING;
      break;

    case CLUTCH:
      Serial.println("CLUTCH OPEN");
      clutchMotor.forward();
      delay(clutchTime);
      unsigned long previousMillis = millis();
      while (analogRead(clutchPin) < 300) {
        if (millis() - previousMillis > 10000) {
          break;
        }
      }
      Serial.println("CLUTCH CLOSED");
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
