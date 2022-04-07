
int PWM_Value = 255;
byte Enable_Pin = 8;
byte PWM1_Pin = 9;
byte PWM2_Pin = 10;
byte PWM_Pin = 9;
 
int upVal = 0;
int downVal = 0;
int upShift = 0;
int downShift = 0;

#define potPin A0
int potVal=100;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  
  pinMode(Enable_Pin, OUTPUT);
  pinMode(PWM1_Pin, OUTPUT);
  pinMode(PWM2_Pin, OUTPUT);
  
  
  analogWrite(PWM1_Pin, PWM_Value);
  analogWrite(PWM2_Pin, PWM_Value);
              }
void loop() 
{
  digitalWrite(Enable_Pin, HIGH);
  potVal = analogRead(A0);
  upVal = analogRead(A4);
  downVal = analogRead(A5);

  if (upVal < 300) {
    upShift = 1;
  }
  else {
    upShift = 0;
  }
  if (downVal < 300) {
    downShift = 1;
  }
  else {
    downShift = 0;
  }
  Serial.print("Up Shift = ");
  Serial.print(upVal);
  Serial.print(" | Down Shift = ");
  Serial.print(downShift);
  Serial.print(" | Potentiometer Value = ");
  Serial.println(potVal);

  
  /*if (upShift == 1) {
    digitalWrite(Enable_Pin, HIGH);
    
   PWM_Pin = PWM1_Pin;
    analogWrite(PWM_Pin, PWM_Value);
   analogWrite(PWM2_Pin, 0);
   delay(1000);
    digitalWrite(Enable_Pin, LOW);
  }
  else {
    digitalWrite(Enable_Pin, LOW);
   
  }

  if (downShift == 1) {
    digitalWrite(Enable_Pin, HIGH);
    PWM_Pin = PWM2_Pin;
    analogWrite(PWM_Pin, PWM_Value);
   analogWrite(PWM1_Pin, 0);
   delay(1000);
    digitalWrite(Enable_Pin, LOW);
  }
  else {
    digitalWrite(Enable_Pin, LOW);
   
  }*/
digitalWrite(Enable_Pin, LOW);
 analogWrite(PWM_Pin, 0);
  
 
}
