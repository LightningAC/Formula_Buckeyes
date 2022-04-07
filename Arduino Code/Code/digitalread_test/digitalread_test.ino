int upVal = 0;
int downVal = 0;
int upShift = 0;
int downShift = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
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
  Serial.print(upShift);
  Serial.print(" | Down Shift = ");
  Serial.println(downShift);
}
