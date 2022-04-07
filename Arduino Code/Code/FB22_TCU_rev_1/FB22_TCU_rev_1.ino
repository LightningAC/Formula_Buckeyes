#include <L298N.h>
#include <SPI.h>



// L298N pin definitions
#define ENA 9
#define IN1 7
#define IN2 8
#define ENB 3
#define IN3 5
#define IN4 4

// Sensing pins and inital values
int CSRA = A0;
int CSRB = A1;
int thermistor = A4;

int curr_raw_A = 0;
int curr_raw_B = 0;
int curr_calc_A = 0;
int curr_calc_B = 0;
int temp_raw = 0;
int temp_calc = 0;
int resist = 0.1;           // Current sensing resistor value


// Direct wired paddle inputs
#define up_request 2
#define down_request 6
int upReqVal = 0;
int downReqVal = 0;



// Create two motor instances
L298N gear_motor(ENA, IN1, IN2);
L298N clutch_motor(ENB, IN3, IN4);

// Input variables for paddle shifters
int isUpshift = 0;
int isDownshift = 0;

// Buffer
int shift_buffer[32];


// Motor timing & speed values
int gear_speedCW = 255;
int gear_speedCCW = 255;
int gear_up_time = 1000;
int gear_down_time = 1000;
int clutch_speed = 255;
int clutch_time = 500;
int shift_input = 0;

// Misc variables
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
int counter = 0;
bool shift_complete = false;
enum : byte {
  UPSHIFT,
  DOWNSHIFT,
  DO_NOTHING,
  TAKE_A_SCREENSHOT
} shift_state;
int CAN_input[2] = {0,0};

bool completeShift = false;
bool completeUpA = false;
bool completeUpB = false;
bool completeDownGearA = false;
bool completeDownGearB = false;
bool completeDownClutchA = false;
bool completeDownClutchB = false;


void setup() {
  Serial.begin(9600);


  // Initialize sensor inputs
  pinMode(CSRA, INPUT);
  pinMode(CSRB, INPUT);
  pinMode(thermistor, INPUT);

  pinMode(up_request, INPUT);
  pinMode(down_request, INPUT);

  
  gear_motor.setSpeed(gear_speedCW);
  clutch_motor.setSpeed(clutch_speed);

  shift_state = DO_NOTHING;
  Serial.println("Begin program");
  
}

void loop() {
  //Serial.println("Main loop");

  // Read sensor data
  //curr_raw_A = analogRead(csra);
  //Serial.println(curr_raw_A);
  //curr_raw_B = analogRead(CSRB);
  //temp_raw = analogRead(thermistor);
  
  
  // Calculate current from shunt resistors (I=V/R)
  curr_calc_A = curr_raw_A / resist;
  curr_calc_B = curr_raw_B / resist;
  Serial.print("Current: ");
  Serial.print(curr_calc_A);
  Serial.print("/");
  Serial.print(curr_calc_B);
  Serial.print("   ");


  if (overcurrent(curr_calc_A) || overcurrent(curr_calc_B || overheat(temp_raw))){
    //estop();
    
  }
  // Read CAN input data

  // Comment out these lines when using CAN. This is for direct inputs
  upReqVal = digitalRead(up_request);
  downReqVal = digitalRead(down_request);
  Serial.print(upReqVal);
  Serial.print(" ");
  Serial.println(downReqVal);
  CAN_input[0] = upReqVal;
  CAN_input[1] = downReqVal;

  
  if (CAN_input[0] == 1 && CAN_input[1] == 1){
    shift_state = TAKE_A_SCREENSHOT;
  }
  else if (CAN_input[0] == 1) {
    shift_state = UPSHIFT;
  }
  else if (CAN_input[1] == 1) {
    shift_state = DOWNSHIFT;
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
switch(shift_state) {
  case UPSHIFT:
     Serial.println("UPSHIFT");
    gear_motor.forward();
    
    while(!completeShift) {
    
    if (millis() - previousMillis > gear_up_time && completeUpA == false && completeUpB == false) {
      Serial.println("Upshift sequence A");
      gear_motor.stop();
      previousMillis = millis();
      completeUpA = true;
      //gear_motor.backward();
    }
    else if (millis() - previousMillis > gear_up_time/2 && completeUpA == true && completeUpB == false) {
      Serial.println("Upshift sequence B");
      gear_motor.stop();
      previousMillis = millis();
      completeUpB = true;
    }

    if (completeUpA == true && completeUpB == true) {
      Serial.println("Upshift complete");
      completeShift = true;
    }
    }
   
    shift_state = DO_NOTHING;

  break;

  case DOWNSHIFT:
    Serial.println("DOWNSHIFT");
    gear_motor.backward();
    clutch_motor.forward();

    while(!completeShift) {
    if ((millis() - previousMillis > gear_down_time || millis() - previousMillis > clutch_time) && (completeDownGearA == false || completeDownClutchA == false) && completeDownGearB == false && completeDownClutchB == false) {
      if (millis() - previousMillis > gear_down_time && completeDownGearA == false) {
        Serial.println("Downshift gear sequence A");
        gear_motor.stop();
        completeDownGearA = true;
      }

      else if (millis() - previousMillis > clutch_time && completeDownClutchA == false) {
        Serial.println("Downshift clutch sequence A");
        clutch_motor.stop();
        completeDownClutchA = true;
      }
      if(completeDownGearA == true && completeDownClutchA == true && completeDownGearB == false && completeDownClutchB == false) {
        Serial.println("Downshift gear and clutch sequence complete");
        previousMillis = millis();  
        //gear_motor.forward();
        clutch_motor.backward();    
      }
    }

    else if ((millis() - previousMillis > gear_down_time || millis() - previousMillis > clutch_time) && completeDownGearA == true && completeDownClutchA == true && (completeDownGearB == false || completeDownClutchB == false)) {
       if (millis() - previousMillis > gear_down_time/2 && completeDownGearB == false) {
        Serial.println("Downshift gear sequence B");
        gear_motor.stop();
        completeDownGearB = true;
      }

      else if (millis() - previousMillis > clutch_time && completeDownClutchB == false) {
        Serial.println("Downshift clutch sequence B");
        clutch_motor.stop();
        completeDownClutchB = true;
      }
      if(completeDownGearB == true && completeDownClutchB == true) {
        Serial.println("Downshift gear and clutch sequence B");
        previousMillis = millis();  
        gear_motor.stop();
        clutch_motor.stop();    
      }
    }

    if (completeDownGearB == true && completeDownClutchB == true && completeDownGearB == true && completeDownClutchB == true) {
      Serial.println("Downshift sequence complete");
      completeShift = true;
    }
    }
    shift_state = DO_NOTHING;
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


// Upshift if input is 1, downshift if input is -1
/*void shift(shift_input) {
  if (shift_input == 1) {               // upshift
    previousMillis = millis();                   // start timer
    gear_motor.setSpeed(gear_speedCW);           // change speed accordingly
    gear_motor.forward();         // rotate cw for a certain amount of time in milliseconds
    gear_motor.backward();      // return to starting position
    gear_motor.stop();
  }
  else if (shift_input == -1) {         // downshift
     previousMillis = millis();                 // start timer
     gear_motor.setSpeed(gear_speedCCW);        
     clutch_motor.setSpeed(clutch_speed);
     gear_motor.forward();                      // rotate ccw
     clutch_motor.forward();                    // actuate clutch
     delay(gear_down_time);
     
     gear_motor.backward();                     // reset position
     clutch_motor.backward();
     delay(gear_down_time);     
     // In case gear motor and clutch motor move for different times
     currentMillis = millis()
     while (currentMillis - previousMillis != 0){
        currentMillis = millis()
     
        if(currentMillis - previousMillis > gear_down_time) {  
          previousMillis = currentMillis; 
          gear_motor.backward();
        }
        else if(currentMillis - previousMillis > clutch_time) {
          previousMillis = currentMillis;
          clutch_motor.backward();
        }
     }
     
     
  }
  
}
*/

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
  gear_motor.setSpeed(0);
  clutch_motor.setSpeed(0);
  
  gear_motor.stop();
  clutch_motor.stop();

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
