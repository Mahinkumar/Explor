
#include <Servo.h>
#include "pitches.h"
#define SPEAKER 4
#include <Drive.h>

Servo headservo;
const int EchoPin = 2;
const int TrigPin = 3;

const int IN1 = 8;
const int IN2 = 9;
const int IN3 = 6;
const int IN4 = 7;

const int HeadServopin = 5;
const int maxStart = 800;
unsigned long time;
unsigned long time1;
int add = 0;
int add1 = 0;
int roam = 0;
int dur = 1000;
int i;
int pos = 0;
int x = 0;
int y = 0;
int ix = 0;
int iy = 0;
int currDist = 5000;
boolean running = false;

Drive drive(IN1, IN2, IN3, IN4);


void setup() {

  tone(SPEAKER, NOTE_C7, 100);
  delay(500);
  tone(SPEAKER, NOTE_C6, 100);
  tone(SPEAKER, NOTE_C7, 100);
  delay(500);
  tone(SPEAKER, NOTE_C6, 100);
  tone(SPEAKER, NOTE_C7, 100);
  delay(500);
  tone(SPEAKER, NOTE_C6, 100);


  Serial.begin(9600); // Enables Serial monitor for debugging purposes
  Serial.println("Exploration Rover Initialized");
  Serial.print("O/S Starting .");
  delay(1000);
  Serial.print(" .");
  delay(1000);
  Serial.print(" .");
  delay(1000);
  Serial.println(" .");
  Serial.println("System Check Results : OK ");
  delay(1000);
  Serial.println("Data Link Initialized... "); // Test the Serial communication
  Serial.println("Awaiting Commands from mission control");
  Serial.println("----------------------------------------------------------------");
  pinMode(EchoPin, INPUT);//signal input port
  for (int pinindex = 3; pinindex < 10; pinindex++) {
    pinMode(pinindex, OUTPUT); // set pins 3 to 10 as outputs
  }

  // headservo
  headservo.attach(HeadServopin);
  headservo.write(160);
  delay(1000);
  headservo.write(20);
  delay(1000);
  headservo.write(90);
  delay(1000);
  return;
}

void loop()
{
  if (Serial.available() > 0)
  {
    int data = Serial.read();
    switch (data)
    {
      case 'F' :
        Serial.println("Rolling!");
        moveForward1();
        break;
      case 'B' :
        back1();
        //buzz();
        break;
      case 'L' :
        lturn();
        //buzz();
        break;
      case 'v' :
        radar();
        break;
      case 'R' :
        rturn();
        // buzz();
        break;
      case 'S' :
        totalhalt();
        break;
      case 'W' :
        toggleRoam();
        buzz();
        break;
    }
  }


  if (roam == 0) { //listen
  }

  else if (roam == 1) {//If roam active
    //time = millis();
    currDist = MeasuringDistance();
    Serial.print("Current Forward Distance: ");
    //Serial.println(currDist);
    if (currDist > 35) {
      add = (add1++);
      nodanger();
      Serial.println("Nodanger: ");
    }
    else if (currDist < 35) {
      //add=0;
      Serial.println("Forward Blocked- Decide Which Way");
      backup();
      whichway();
    }
  }




  // serial control only
  // forward



  for (int i = 0; i <= (dur / 25); i++) {
    currDist = MeasuringDistance();
    if (currDist > 35) {
      Serial.println(i);
      drive.moveForward(500);
    }
    else {
      Serial.println("Forward now blocked! - Stopping");
      Serial.print("Steps Moved Before Being Blocked: ");
      Serial.println(i);
      totalhalt();
      break;
    }
  }
  Serial.print("Finished Forward Travel Duration: ");
  Serial.println(dur);
  totalhalt();
  Serial.println("Awaiting Next Command");
  Serial.println("----------------------------------------------------------------");
}

void moveForward1() {
  for (int i = 0; i <= (dur * 10); i++); {
    drive.moveForward(500);
  }
  if (currDist > 35) {
    Serial.println("Forward Path is Clear - Proceeding Forward ");
  }
  else if (currDist < 35) {
    Serial.println("*******FORWARD BLOCKED****** Cannot Execute Forward Movement");
    Serial.println("Awaiting Next Command");
    Serial.println("----------------------------------------------------------------");
    return;
  }
}
void back1() {
  drive.moveBackward(500);
  /* analogWrite(leftmotorpin1, 120);
    analogWrite(leftmotorpin2, 0);
    analogWrite(rightmotorpin1, 120);
    analogWrite(rightmotorpin2, 0); */
}

void lturn() {
  for (int i = 0; i <= (dur * 5); i++) {
    drive.turnLeft(500);
    /* analogWrite(leftmotorpin1, 0);
      analogWrite(leftmotorpin2, 190);
      analogWrite(rightmotorpin1, 190);
      analogWrite(rightmotorpin2, 0);
      //Serial.println("Turning Left");*/
  }
}

//turn right
void rturn() {
  for (int i = 0; i <= (dur * 5); i++) {
    drive.turnRight(500);
    /*analogWrite(leftmotorpin1, 190);
      analogWrite(leftmotorpin2, 0);
      analogWrite(rightmotorpin1, 0);
      analogWrite(rightmotorpin2, 190);
      //Serial.println("Turning Right");*/
  }
}
// serial control only

void toggleRoam() {
  if (roam == 0) {
    roam = 1;
    Serial.println("Activated Roam Mode");
  }
  else {
    roam = 0;
    totalhalt();
    Serial.println("De-activated Roam Mode");
  }
}

void nodanger() {
  /*analogWrite(leftmotorpin1, 0);
    analogWrite(leftmotorpin2, 120);
    analogWrite(rightmotorpin1, 0);
    analogWrite(rightmotorpin2, 120);*/
  if (add1 > 38 ) whichway();

  return;
}

//backward
void backup() {
  add1 = 0; // resets the counter for the nodanger loops
  /*digitalWrite(leftmotorpin1, HIGH);
    digitalWrite(leftmotorpin2, LOW);
    digitalWrite(rightmotorpin1, HIGH);
    digitalWrite(rightmotorpin2, LOW);*/
  Serial.println("Backing Up");
  fart();
}

void whichway()
{
  totalhalt();
  headservo.write(160);
  delay(900);
  int lDist = MeasuringDistance();
  Serial.println("checking left");
  Serial.println(lDist);
  headservo.write(20);
  delay(900);
  int rDist = MeasuringDistance();
  Serial.println("checking right");
  Serial.println(rDist);
  if (lDist < rDist) {
    Serial.println("Decided Right Is Best");
    buzz();// Make him talk
    body_rturn();
    totalhalt();
    currDist = MeasuringDistance();
    if (currDist < 45) body_rturn();
    else {
      Serial.println("Decided Left Is Best");
      buzz();// Make him talk
      body_lturn();
      totalhalt();
      currDist = MeasuringDistance();
      if (currDist < 45) body_lturn();
    }
    return;
  }
}
float MeasuringDistance() {
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  // Distance Calculation
  float distance = pulseIn(EchoPin, HIGH);
  distance = distance / 58;
  return (distance);
}

void body_lturn() {
  for (int i = 0; i <= (dur * 5); i++) {
    drive.turnLeft(500);
    /*analogWrite(leftmotorpin1, 0);
        analogWrite(leftmotorpin2, 220);
        analogWrite(rightmotorpin1, 220);
        analogWrite(rightmotorpin2, 0);
        Serial.println("Turning Left");
        delay(400);*/
  }
}


void body_rturn() {
  for (int i = 0; i <= (dur * 5); i++) {
    drive.turnRight(500);
    /*analogWrite(leftmotorpin1, 220);
      analogWrite(leftmotorpin2, 0);
      analogWrite(rightmotorpin1, 0);
      analogWrite(rightmotorpin2, 220);
      Serial.println("Turning Right");
      delay(400);*/
  }
}

void buzz() {
  tone(SPEAKER, NOTE_C7, 100);
  delay(50);
  tone(SPEAKER, NOTE_C6, 100);
}

void fart() {
  tone(SPEAKER, NOTE_C2, 200);
  delay(50);
  tone(SPEAKER, NOTE_C1, 400);
  delay(100);
  noTone(SPEAKER);
}

void radar() {
  Serial.println("SONAR Sweep Command Received");
  Serial.println("SONAR Sweep Initiated...");
  headservo.write(20);
  delay(2000);
  for (pos = 20; pos <= 160; pos += 1)
  {
    headservo.write(pos);
    Print(MeasuringDistance() , pos);
    delay(10);
  }
  delay(1000);
  Serial.println("---------Sweep Back--------------");
  for (pos = 160; pos >= 20; pos -= 1)
  {
    headservo.write(pos);
    Print(MeasuringDistance() , pos);
    delay(10);
  }
  delay(500);
  headservo.write(90);
  Serial.println("SONAR Sweep Complete- Awaiting next command...");
  Serial.println("----------------------------------------------------------------");
}

void Print (int R , int T)
{
  Serial.print(R); Serial.print(", ");
  Serial.print(T); Serial.println(".");
  delay(100);
}

/*digitalWrite(leftmotorpin1, HIGH);
  digitalWrite(leftmotorpin2, HIGH);
  digitalWrite(rightmotorpin1, HIGH);
  digitalWrite(rightmotorpin2, HIGH);
  //Serial.println("Totalhalt!");
  //headservo.write(90); // set servo to face forward
  //delay(250);
  return;*/

void totalhalt() {  // This applies the brakes so rover cannot move
  drive.moveForward(100);
  drive.moveBackward(100);
  delay(50);
  //Serial.println("Totalhalt!");
  //headservo.write(90); // set servo to face forward
  //delay(250);
  return;
}
