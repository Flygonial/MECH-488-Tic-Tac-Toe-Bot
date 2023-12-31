#include "math.h"
#include "LiquidCrystal_I2C.h"
#include <wire.h>
#include <Servo.h>
#include <string.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // For TI chip, for NXP chip, use 0x3F
int startUp = 0;
int state = 0; // 0 = game not started/reset
int firstComp = 0; // Flags the first turn for the computer's turn
int board[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
int won[2] = {0, 0};
static int decisArray[3] = {0, 0, 0};
static int checkArray[3] = {0, 0, 0};
static int pickPlace[2] = {0, 0};

Servo myservo1;  // create servo object to control a servo
Servo myservo2;
int servoPin1 = 10;
int servoPin2 = 9;
int compPieces;
int magPin = 22;
int limitSwitch = 24;
int stepPin = 11;
int hall1 = A0;
int hall2 = A1;
int hall3 = A2;
int hall4 = A3;
int hall5 = A4;
int hall6 = A5;
int hall7 = A6;
int hall8 = A7;
int hall9 = A8;
int rightButton = 36;
int leftButton = 34;
int dirPin = 32;
int X1 = 90; // Placeholder
int X2 = 90; //Placeholder
int pos = -1;
int col1 = 500;
int col2 = 5500;
int col3 = 15500;
int col4 = 20500;
int col5 = 25500;
int compPieceNum = 0;
float steps = 200;
int idx1 = 1;
int angles_input[2] = {0, 0};

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  while (!Serial); // Leonardo: wait for serial monitor
  Serial.println("\nScanning for I2C devices:");
  lcd.init();
  lcd.clear();
  lcd.backlight();
  myservo1.write(0);
  myservo2.write(60);
  myservo1.attach(10);
  myservo2.attach(12);  // attaches the servo on pin 9 to the servo object
  pinMode(magPin, OUTPUT);
  pinMode(limitSwitch, INPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(hall1, INPUT);
  pinMode(hall2, INPUT);
  pinMode(hall3, INPUT);
  pinMode(hall4, INPUT);
  pinMode(hall5, INPUT);
  pinMode(hall6, INPUT);
  pinMode(hall7, INPUT);
  pinMode(hall8, INPUT);
  pinMode(hall9, INPUT);
  armHome();

  randomSeed(analogRead(13));
}


void findPos(int pos1, int pos2) {
  if (pos1 == 1 && pos2 == 1) {
    angles_input[0] = 0;
    angles_input[1] = 18;
  }
  if (pos1 == 2 && pos2 == 1) {
    angles_input[0] = 12;
    angles_input[1] = 34;
  }
  if (pos1 == 3 && pos2 == 1) {
    angles_input[0] = 22;
    angles_input[1] = 54;
  }
  if (pos1 == 1 && pos2 == 0) {
    angles_input[0] = 7;
    angles_input[1] = 18;
  }
  if (pos1 == 2 && pos2 == 0) {
    angles_input[0] = 15;
    angles_input[1] = 33;
  }
  if (pos1 == 3 && pos2 == 0) {
    angles_input[0] = 26;
    angles_input[1] = 54;
  }
}

void moveServoTo1(int deg, Servo myservo, int idx1) {
  if (idx1 == 1) {

    float stepDiff = (deg - 0) / steps;
    for (float angle1 = 0; angle1 < deg; angle1 = angle1 + stepDiff) {
      Serial.println(angle1);
      myservo.write(angle1);
    }
  }
}

void moveServoBack1(int deg, Servo myservo, int idx1) {
  if (idx1 == 1) {
    float stepDiff = (deg - 0) / steps;
    for (float angle = deg; angle > 0; angle = angle - stepDiff) {
      Serial.println(angle);
      myservo.write(angle);
    }
  }
}


void moveServoTo2(int deg, Servo myservo, int idx1) {
  if (idx1 == 1) {

    float stepDiff = (60 - deg) / steps;
    for (float angle1 = 60; angle1 > deg; angle1 = angle1 - stepDiff) {
      Serial.println(angle1);
      myservo.write(angle1);
    }
  }
}

void moveServoBack2(int deg, Servo myservo, int idx1) {
  if (idx1 == 1) {
    float stepDiff = (60 - deg) / steps;
    for (float angle = deg; angle < 60; angle = angle + stepDiff) {
      Serial.println(angle);
      myservo.write(angle);
    }
  }
}



int moveToPos(int col) { // col in put is # 1 - 5
  int newPos = 0;
  int newDir = 0;
  if (col == 1) {
    newPos = col1;
  }
  if (col == 2) {
    newPos = col2;
  }
  if (col == 3) {
    newPos = col3;
  }
  if (col == 4) {
    newPos = col4;
  }
  if (col == 5) {
    newPos = col5;
  }

  if (newPos > pos) {
    newDir = 0;
  }
  else {
    newDir = 1;
  }
  int newSteps = abs(newPos - pos);

  moveRev(newDir, 1800, 1200, newSteps);
  pos = newPos;
}

void moveRev(int dir, float init, float fin, int num_step)
{
  // This function is marked as 'Attempt' due to the unsuccessful attempt to add higher or lower order polynomial
  // modelling to the uroflow curves
  // dir: the direction to spin: 0 for CW, 1 for CCW
  // init: initial period of square wave output in microseconds
  // fin: final period of square wave output in microseconds
  // num_step: the number of steps to run the acceleration curve through.
  // order: the polynomial order with which to fill the acceleration curve with
  // float scaled = pow(float(abs(init - fin)/num_step), order);

  if (dir == 0)
  {
    digitalWrite(dirPin, HIGH);
    Serial.println(digitalRead(dirPin));
    float addedSpeed = 0.0;
    for (int i = 0; i < num_step; i++)
    {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds((init - addedSpeed) / 2);
      digitalWrite(stepPin, LOW);
      delayMicroseconds((init - addedSpeed) / 2);
      addedSpeed = addedSpeed + float(abs(init - fin) / num_step);
      // Serial.println(addedSpeed);
    }
  }

  if (dir == 1)
  {
    digitalWrite(dirPin, LOW);
    Serial.println(digitalRead(dirPin));
    float addedSpeed = 0.0;
    for (int i = 0; i < num_step; i++)
    {
      digitalWrite(stepPin, HIGH);
      //Serial.println("high");
      delayMicroseconds((init - addedSpeed) / 2);
      digitalWrite(stepPin, LOW);
      //Serial.println("low");
      delayMicroseconds((init - addedSpeed) / 2);
      addedSpeed = addedSpeed + float(abs(init - fin) / num_step);
      //Serial.println(addedSpeed);
    }
  }
  //return(0);
}

void armHome() {
  digitalWrite(dirPin, LOW);
  while (digitalRead(limitSwitch) == 1) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  digitalWrite(dirPin, HIGH);
  while (digitalRead(limitSwitch) == 0) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1200);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1200);
  }
  pos = 0;
}

void toggleMag(int on) {
  if (on == 1) {
    digitalWrite(magPin, HIGH);    // Should just toggle a digital pin linked to a relay linked to the electromagnet
  }
  if (on == 0) {
    digitalWrite(magPin, LOW);
  }
}

//void moveArm(int row, int col) {
//  // Basically a bunch of ifs and the necessary arm movements will suffice
//  // Include drawing a piece and placing a piece
//  if (row == 3 && col == 0) { // Just count up in row inputs to denote retrieving a playing piece
//    Servo2.write(X1);
//    Servo1.write(X2);
//    toggleMag("ON");
//    Servo1.write(180 - X2);
//    Servo2.write(180 - X1);
//  }
//  if (row == 4 && col == 0) { // Just count up in row inputs to denote retrieving a playing piece
//    Servo2.write(X1);
//    Servo1.write(X2);
//    toggleMag("ON");
//    Servo1.write(180 - X2);
//    Servo2.write(180 - X1);
//  }
//  if (row == 5 && col == 0) { // Just count up in row inputs to denote retrieving a playing piece
//    Servo2.write(X1);
//    Servo1.write(X2);
//    toggleMag("ON");
//    Servo1.write(180 - X2);
//    Servo2.write(180 - X1);
//  }
//  if (row == 6 && col == 0) { // Just count up in row inputs to denote retrieving a playing piece
//    Servo2.write(X1);
//    Servo1.write(X2);
//    toggleMag("ON");
//    Servo1.write(180 - X2);
//    Servo2.write(180 - X1);
//  }
//  if (row == 7 && col == 0) { // Just count up in row inputs to denote retrieving a playing piece
//    Servo2.write(X1);
//    Servo1.write(X2);
//    toggleMag("ON");
//    Servo1.write(180 - X2);
//    Servo2.write(180 - X1);
//  }
//
//  // Moveing to board positions
//  if (row == 0 && col == 0) {
//    Servo2.write(X1); // This is all a placeholder for now
//    Servo1.write(X2); // We want to move the farther motor out first to avoid any possible board clipping
//    toggleMag("OFF");
//    Servo1.write(180 - X2);
//    Servo2.write(180 - X1); // We want to just maintain a given position
//  }
//  if (row == 0 && col == 0) {
//    Servo2.write(X1); // This is all a placeholder for now
//    Servo1.write(X2); // We want to move the farther motor out first to avoid any possible board clipping
//    toggleMag("OFF");
//    Servo1.write(180 - X2);
//    Servo2.write(180 - X1); // We want to just maintain a given position
//  }
//}


void winCheck(int playerID) {
  // index 1: 0 if not won, 1 if player has won, 2 if computer has won
  // index 2: 0 if no pattern, 1 if two-line possible, 2 if win possible, 3 if won
  // index 3: 0-8 position of winning position if exists, else 9
  if (playerID == 0) {
    // Loop over horizontals
    // Horizontal 1:
    int markCount = 0;
    int winPos = 9; // Initialize to some invalid value at first
    int untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[0][i] == 1) { // Player pieces
        markCount += 1;
      }
      if (board[0][i] == 0) {
        winPos == i;
        untaken += 1;
      }
    }

    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 1;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Now reset to check the other horizontals
    // Horizontal 2:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[1][i] == 1) { // Robot pieces
        markCount += 1;
      }
      if (board[1][i] == 0) {
        winPos == i + 3;
      }
    }

    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 1;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Horizontal 3:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[2][i] == 1) { // Robot pieces
        markCount += 1;
      }
      if (board[2][i] == 0) {
        winPos == i + 6;
      }
    }

    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 1;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Vertical 1:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[i][0] == 1) { // Robot pieces
        markCount += 1;
      }
      if (board[i][0] == 0) {
        winPos == 3 * i; // Can be 0, 3, or 6
      }
    }

    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 1;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Vertical 2:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[i][1] == 1) { // Robot pieces
        markCount += 1;
      }
      if (board[i][1] == 0) {
        winPos == 3i + 1;
      }
    }
    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 1;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Vertical 3:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[i][2] == 1) { // Robot pieces
        markCount += 1;
      }
      if (board[i][2] == 0) {
        winPos == 3i + 2;
      }
    }
    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 1;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Horizontal LR:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[i][i] == 1) { // Robot pieces
        markCount += 1;
      }
      if (board[i][i] == 0) {
        winPos == 4i; // positions are 0, 4, 8
      }
    }
    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 1;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Horizontal RL:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[i][2 - i] == 1) { // Robot pieces
        markCount += 1;
      }
      if (board[i][2 - i] == 0) {
        winPos == 2 + 2i; // positions are 2, 4, 6
      }
    }
    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 1;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }
  }
  else if (playerID == 1) {
    // Loop over horizontals
    // Horizontal 1:
    int markCount = 0;
    int winPos = 9; // Initialize to some invalid value at first
    int untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[0][i] == 2) { // Robot pieces
        markCount += 1;
      }
      if (board[0][i] == 0) {
        winPos == i;
        untaken += 1;
      }
    }

    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 2;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Now reset to check the other horizontals
    // Horizontal 2:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[1][i] == 2) { // Robot pieces
        markCount += 1;
      }
      if (board[1][i] == 0) {
        winPos == i + 3;
      }
    }

    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 2;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Horizontal 3:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[2][i] == 2) { // Robot pieces
        markCount += 1;
      }
      if (board[2][i] == 0) {
        winPos == i + 6;
      }
    }

    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 2;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Vertical 1:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[i][0] == 2) { // Robot pieces
        markCount += 1;
      }
      if (board[i][0] == 0) {
        winPos == 3 * i; // Can be 0, 3, or 6
      }
    }

    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 2;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Vertical 2:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[i][1] == 2) { // Robot pieces
        markCount += 1;
      }
      if (board[i][1] == 0) {
        winPos == 3i + 1;
      }
    }
    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 2;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Vertical 3:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[i][2] == 2) { // Robot pieces
        markCount += 1;
      }
      if (board[i][2] == 0) {
        winPos == 3i + 2;
      }
    }
    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 2;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Horizontal LR:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[i][i] == 2) { // Robot pieces
        markCount += 1;
      }
      if (board[i][i] == 0) {
        winPos == 4i; // positions are 0, 4, 8
      }
    }
    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 2;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }

    // Horizontal RL:
    markCount = 0;
    winPos = 9;
    untaken = 0;
    for (int i = 0; i < 3; i++) {
      if (board[i][2 - i] == 2) { // Robot pieces
        markCount += 1;
      }
      if (board[i][2 - i] == 0) {
        winPos == 2 + 2i; // positions are 2, 4, 6
      }
    }
    if (markCount == 3) {
      // won = {1, 2};
      decisArray[0] = 2;
      decisArray[1] = 3;
      decisArray[2] = 9;
      //decisArray = {1, 3, 9};
      // return {1, 3, 9}; // Player 2 has won
    }
    if (markCount == 2 && untaken == 1) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 2, winPos};
    }
    if (markCount == 1 && untaken == 2) {
      decisArray[0] = 0;
      decisArray[1] = 1;
      decisArray[2] = winPos;
      // decisArray = {0, 1, winPos};
    }
  }
  // All else fails:
  // return {0, 0, 9}; // No winning position exists
}

void lcdShow(int row, int col, String message) {
  lcd.setCursor(row, col);   // Sets the starting point for the message
  lcd.print(message); // Message to be displayed
  //Serial.println(message);
}

int randChoice(double p) {
  double outcome = random(0, 100) / 100.0;
  if (outcome < p) {
    // This means the event should occur, as the chance of it falling outside of p is 1-p
    return 1;
  }
  if (outcome >= p) {
    return 0; // Event does not occur
  }
}

void placePiece(int row, int col) {
  compPieceNum = compPieceNum + 1;
  if (compPieceNum < 4) {
    moveToPos(2);
  }
  else {
    moveToPos(1);
  }

  if ((compPieceNum == 1) || (compPieceNum == 4)) { // ROW 1
    findPos(1, 0);
    moveServoTo2(angles_input[1], myservo2, idx1);
    delay(500);
    moveServoTo1(angles_input[0], myservo1, idx1);
    toggleMag(1);
    delay(1500);
    moveServoBack1(angles_input[0], myservo1, idx1);
    delay(500);
    moveServoBack2(angles_input[1], myservo2, idx1);
  }
  else if ((compPieceNum == 2) || (compPieceNum == 5)) { // ROW 2
    findPos(2, 0);
    moveServoTo2(angles_input[1], myservo2, idx1);
    delay(500);
    moveServoTo1(angles_input[0], myservo1, idx1);
    toggleMag(1);
    delay(1500);
    moveServoBack1(angles_input[0], myservo1, idx1);
    delay(500);
    moveServoBack2(angles_input[1], myservo2, idx1);
  }
  else { // ROW 3
    findPos(3, 0);
    moveServoTo2(angles_input[1], myservo2, idx1);
    delay(500);
    moveServoTo1(angles_input[0], myservo1, idx1);
    toggleMag(1);
    delay(1500);
    moveServoBack1(angles_input[0], myservo1, idx1);
    delay(500);
    moveServoBack2(angles_input[1], myservo2, idx1);
  }

  moveToPos(col + 3);

  // move down to row to drop off piece
  //drop piece off in row row+1

    findPos(row+1, 1);
    moveServoTo2(angles_input[1], myservo2, idx1);
    delay(500);
    moveServoTo1(angles_input[0], myservo1, idx1);
    toggleMag(0);
    delay(1500);
    moveServoBack1(angles_input[0], myservo1, idx1);
    delay(500);
    moveServoBack2(angles_input[1], myservo2, idx1);


  // turn off EM
  // move up

}

void playDecision() {
  // Board should just be an internal variable so this should operate on it
  int emptyTest = 0;
  if (firstComp == 0) { // Check for first turn condition
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if (board[i][j] == 0) {
          emptyTest += 1;
        }
      }
    }
    if (emptyTest == 9) {
      int choice = randChoice(0.8);
      if (choice == 1) {
        pickPlace[0] = 1;
        pickPlace[1] = 1;
        // return {1, 1}; // Place in center of board
      }
      else {
        int col = random(0, 8); // Pick a random place between 0 and 8
        int row = 0; // Use a modulo-series subtraction counter
        while (col > 2) {
          col -= 3;
          row += 1; // The idea here is that each consecutive decrement corresponds to a new row
        }

        pickPlace[0] = row;
        pickPlace[1] = col;
        //return {row, col};
      }
    }
  }
  winCheck(1);
  // if (winCheck(1, board)[1] == 2) { // A win is possible
  if (decisArray[1] == 2) {
    int col = decisArray[2]; // winCheck(1, board)[2]; // The winPos output
    int row = 0; // Use a modulo-series subtraction counter
    while (col > 2) {
      col -= 3;
      row += 1; // The idea here is that each consecutive decrement corresponds to a new row
    }
    pickPlace[0] = row;
    pickPlace[1] = col;
    // return {row, col};
  }

  // else if (winCheck(1, board)[1] == 1) { // A two-line is possible
  else if (decisArray[1] == 1) {
    int col = decisArray[2]; // winCheck(1, board)[2]; // The winPos output
    int row = 0; // Use a modulo-series subtraction counter
    while (col > 2) {
      col -= 3;
      row += 1; // The idea here is that each consecutive decrement corresponds to a new row
    }
    pickPlace[0] = row;
    pickPlace[1] = col;
    // return {row, col};
  }

  // Finally, if none else applies, pick a random slot

  int rowRand;
  int colRand;
  while (true) {
    rowRand = random(2);
    colRand = random(2);
    if (board[rowRand][colRand] == 0) {
      break;
    }
  }
  pickPlace[0] = rowRand;
  pickPlace[1] = colRand;
  
  /*
  int availSlots[9] = {9, 9, 9, 9, 9, 9, 9, 9, 9}; // Initialize as non 0-8 value
  int availCount = 0;
  // else { // Pick a random slot
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (board[i][j] == 0) {
        // emptyTest += 1;
        availSlots[availCount] = i + 3 * j; // i basically selects row, j column
        availCount += 1;
      }
    }
  }
  // Then we want to do a random pick select:
  int placeSelect = random(0, availCount); // Want to make sure this indexes properly
  int col = availSlots[placeSelect]; // Goal is to place into a corresponding place
  int row = 0;
  while (col > 2) {
    col -= 3;
    row += 1; // The idea here is that each consecutive decrement corresponds to a new row
  }
  pickPlace[0] = row;
  pickPlace[1] = col;
  // return {row, col};
  // }
  */
}

int checkSensors() {
  // First index is on if a change has occured, second index is the row, third index is the col
  // if change in TTT board
  if (board[0][0] == 0 && ((analogRead(hall1) < 480) || (analogRead(hall1) > 560))) {
    checkArray[0] = 1;
    checkArray[1] = 0; // Row 0
    checkArray[2] = 0; // Col 0
    return 1;
  }
  if (board[0][1] == 0 && ((analogRead(hall2) < 480) || (analogRead(hall2) > 560))) {
    checkArray[0] = 1;
    checkArray[1] = 0; // Row 0
    checkArray[2] = 1; // Col 0
    return 1;
  }
  if (board[0][2] == 0 && ((analogRead(hall3) < 480) || (analogRead(hall3) > 560))) {
    checkArray[0] = 1;
    checkArray[1] = 0; // Row 0
    checkArray[2] = 2; // Col 0
    return 1;
  }
  if (board[1][0] == 0 && ((analogRead(hall4) < 480) || (analogRead(hall4) > 560))) {
    checkArray[0] = 1;
    checkArray[1] = 1; // Row 0
    checkArray[2] = 0; // Col 0
    return 1;
  }
  if (board[1][1] == 0 && ((analogRead(hall5) < 480) || (analogRead(hall5) > 560))) {
    checkArray[0] = 1;
    checkArray[1] = 1; // Row 0
    checkArray[2] = 1; // Col 0
    return 1;
  }
  if (board[1][2] == 0 && ((analogRead(hall6) < 480) || (analogRead(hall6) > 560))) {
    checkArray[0] = 1;
    checkArray[1] = 1; // Row 0
    checkArray[2] = 2; // Col 0
    return 1;
  }
  if (board[2][0] == 0 && ((analogRead(hall7) < 480) || (analogRead(hall7) > 560))) {
    checkArray[0] = 1;
    checkArray[1] = 2; // Row 0
    checkArray[2] = 0; // Col 0
    return 1;
  }
  if (board[2][1] == 0 && ((analogRead(hall8) < 480) || (analogRead(hall8) > 560))) {
    checkArray[0] = 1;
    checkArray[1] = 2; // Row 0
    checkArray[2] = 1; // Col 0
    return 1;
  }
  if (board[2][2] == 0 && ((analogRead(hall9) < 480) || (analogRead(hall9) > 560))) {
    checkArray[0] = 1;
    checkArray[1] = 2; // Row 0
    checkArray[2] = 2; // Col 0
    return 1;
  }
  return 0;
}



void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("state ");
  //Serial.println(state);
  if (startUp == 0) {
    lcd.clear();
    lcdShow(0, 0, "Tic-Tac-Toe");
    lcdShow(1, 1, "Tic-Tac Titans");
    compPieces = 5;
    state = 0;
    delay(1000); // I don't think a non-blocking timer is necessary here
    startUp = 1;
  }
  int goesFirst = 1;
  Serial.println("BOARD STATE:");
  Serial.println(board[0][0]);
  Serial.println(board[0][1]);
  Serial.println(board[0][2]);
  Serial.println(board[1][0]);
  Serial.println(board[1][1]);
  Serial.println(board[1][2]);
  Serial.println(board[2][0]);
  Serial.println(board[2][1]);
  Serial.println(board[2][2]);
  switch (state) {
    case 0: // Game start screen
      lcd.clear
      lcdShow(0, 0, "Begin?");
      if (digitalRead(rightButton) == 0) {
        state = 1;
      }
      break;
    case 1: // Choose which player goes first
      lcd.clear();
      goesFirst = 1; //randChoice(0.5);      // MAKE THIS RANDOM ALTER
      if (goesFirst == 1) {
        lcdShow(4, 0, "Player's");
        lcdShow(6, 1, "Turn");
        firstComp = 1;
        delay(1000);
        state = 2; // go to player's turn
      }
      else if (goesFirst == 0) {
        lcdShow(5, 0, "Robot's");
        lcdShow(6, 1, "Turn");
        delay(1000);
        firstComp = 0;
        state = 3; // Computer's turn

      }
      break;          // THIS BREAK WAS TURNED OFF
    case 2: // Player's turn
      lcd.clear();
      lcdShow(4, 0, "Make A");
      lcdShow(6, 1, "Move");
      Serial.println(checkSensors());
      // Serial.println(digitalRead(rightButton));
      if (checkSensors() == 1) {
        lcd.clear();
        lcdShow(3, 0, "Are you");
        lcdShow(6, 1, "Sure?");

        if (digitalRead(rightButton) == 0) { // Wait for player to hit the button
          board[checkArray[1]][checkArray[2]] = 1;
          Serial.println("update board: ");
          Serial.println(board[checkArray[1]][checkArray[2]]);
          lcd.clear();
          winCheck(0); // 0 for player, 1 for computer
          if (decisArray[0] == 0) {
            lcdShow(2, 0, "Computer's");
            lcdShow(6, 1, "Turn");
            delay(1000);
            state = 3;
          }
          else {
            state = 4;
          }
          break;
        }
        else if (digitalRead(leftButton) == 0) { // Wait for player to hit the button
          lcd.clear();
          break;
        }
      }
      else if (digitalRead(leftButton) == 0) {
        break;
      }
      //state = 4; // Checking game state for win or loss
      break;
    case 3: // Computer's turn
      lcd.clear();
      lcdShow(2, 0, "Computer's");
      lcdShow(7, 1, "Turn");
      playDecision();
      if (decisArray[0] == 0) {
        // pickPlace will be the array mutated to read
        placePiece(pickPlace[0], pickPlace[1]);
        board[pickPlace[0]][pickPlace[1]] = 2;
        lcdShow(4, 0, "Player's");
        lcdShow(6, 1, "Turn");
        delay(1000);
        state = 2;
      }
      else {
        state = 4;
      }
      break;
    case 4:
      lcd.clear();
      if (decisArray[0] == 1) {
        lcdShow(5, 0, "Player");
        lcdShow(5, 1, "Victory");
        delay(5000);
      }
      else if (decisArray[0] == 2) {
        lcdShow(3, 0, "Computer");
        lcdShow(4, 1, "Victory");
        delay(5000);
      }
      while (digitalRead(rightButton) == 1) {
        lcdShow(1, 0, "Play");
        lcdShow(9, 1, "Again?");
        if (digitalRead(rightButton) == 1) {
          state = 0;
          startUp = 0;
        }
      }
  }
}
