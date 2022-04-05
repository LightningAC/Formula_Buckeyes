#include <L298N.h>

//pin definition
#define ENA 5
#define IN1 4
#define IN2 3
#define ENB 9
#define IN3 8
#define IN4 7

//create a motor instance
L298N motor1(ENA, IN1, IN2);
L298N motor2(ENB, IN3, IN4);

int spd1 = 255;
int spd2 = 255;

int motor = 0;

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
}

void loop() {
  motor = 1;
  Serial.println(digitalRead(2));
  
  if (motor == 0)
  {
  motor1.setSpeed(spd1);
  motor1.forward();

  Serial.print("Motor 1 speed: ");
  Serial.print(spd1);
  Serial.print(" | Motor direction: Forward");
  Serial.println();

  delay(5000);

  motor1.stop();

  motor1.backward();

  Serial.print("Motor 1 speed: ");
  Serial.print(spd1);
  Serial.print(" | Motor direction: Backward");
  Serial.println();

  delay(5000);

  motor1.stop();
  }
  if(motor == 1)
  {
  motor2.setSpeed(spd2);
  motor2.forward();

  Serial.print("Motor 2 speed: ");
  Serial.print(spd2);
  Serial.print(" | Motor direction: Forward");
  Serial.println();

  delay(500);

  motor2.stop();

  /*motor2.backward();

  Serial.print("Motor 2 speed: ");
  Serial.print(spd2);
  Serial.print(" | Motor direction: Backward");
  Serial.println();

  delay(500);

  motor2.stop();
  */
  }

  else
  {
    motor1.stop();
    motor2.stop();
  }
}
