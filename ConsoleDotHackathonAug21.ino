/**
   Arduino as external Joystick-based mouse
   
   Needs an Arduino that is able to use the keyboard and mouse libraries.
   See https://www.arduino.cc/reference/en/language/functions/usb/keyboard/
   and https://www.arduino.cc/reference/en/language/functions/usb/mouse/
*/

#include <Keyboard.h>
#include <Mouse.h>

#define STOP_PIN 0        // pull to gnd to end keyboard functionality
#define BUTTON_PIN1 1
#define BUTTON_PIN2 2

// #define DEBUG   // when uncommented enables serial output instead of keyboard.
#define MOVEMENT_THRESHOLD 15
#define MAX_JUMP 15 // Max value for a mouse move

boolean shouldBlink = true;

// Array to map pin to emitted character
int pinToChar[] = { 'X', 'Y'} ;

volatile char val = -1;
int a0, a1, a2, a3;
int xDir, yDir;

void setup() {

  pinMode(13, OUTPUT);
  for (int i = 0; i <= 2; i++) {
    pinMode(i, INPUT);        // Pin is for input
    digitalWrite(i, HIGH);    // Enable the internal pull-up resistor for it.
  }
  // Enable Analog pins as input with pull-up resistor
  //  A0/A1 are one potentiometer of the joystick
  //  A2/A3 the other
  pinMode(A0, INPUT_PULLUP);  
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);

#ifdef DEBUG
  Serial.begin(115200);
#endif  

  // Start acting as a HID
  Keyboard.begin();
  Mouse.begin();
  
  // Attach the interrupt handlers for the buttons
  attachInterrupt(digitalPinToInterrupt(0), shouldEnd, FALLING);
  attachInterrupt(digitalPinToInterrupt(1), sendKey, FALLING);
  attachInterrupt(digitalPinToInterrupt(2), sendKey, FALLING);
}

void loop() {
  // We blink the LED and wait a bit.
  //   if the stop bin was engaged, we stop blinking
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(50);               // wait for 0.1 seconds
  if (shouldBlink) {
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  }
  delay(50);

  // Read the potentiometers of the Joystick
  a0 = analogRead(A0);
  a1 = analogRead(A1);
  a2 = analogRead(A2);
  a3 = analogRead(A3);

  xDir = a0-a1;
  if (abs(xDir) < MOVEMENT_THRESHOLD) { // Threshold for movement
    xDir = 0;
  }
  else {
    xDir = map(xDir, -255,255,-MAX_JUMP,MAX_JUMP);
  }
  
  yDir = a2-a3;
  if (abs(yDir) < MOVEMENT_THRESHOLD) { // Threshold for movement
    yDir = 0;
  }
  else {
    yDir = map(yDir, -255,255,-MAX_JUMP,MAX_JUMP);
  }
  Mouse.move(xDir,yDir,0);

#ifdef DEBUG
  Serial.print(a0);
  Serial.print(" - ");
  Serial.print(a1);
  Serial.print(" -> ");
  Serial.print(a1-a0);
  Serial.print("  ->  ");
  if (a1-a0 > MOVEMENT_THRESHOLD ) {
    Serial.print("L");
  }
  else if (a1-a0 < -MOVEMENT_THRESHOLD) {
    Serial.print("R");
  }
  Serial.print("     |   ");
  Serial.print(a2);
  Serial.print(" - ");
  Serial.print(a3);
  Serial.print(" -> ");
  Serial.print(a3-a2);
  Serial.print("  ->  ");
  if (a3-a2 > MOVEMENT_THRESHOLD ) {
    Serial.print("D");
  }
  else if (a3-a2 < -MOVEMENT_THRESHOLD) {
    Serial.print("U");
  }

  Serial.println();
  Serial.flush();
#endif  

  // If the interrups routine set a value we emit it.
  if (val != -1) {

#ifdef DEBUG
    Serial.println(val);
    Serial.flush();
#endif    

    // The next is a bit doubled as it writes a char and does 
    //   a mouse click. Needs a bit of revisit
    Keyboard.write(val);
    if(val == 'X') {
      Mouse.click(MOUSE_LEFT);
    } else {
      Mouse.click(MOUSE_RIGHT);
    }
    val = -1;
  }

}

void shouldEnd() {
  if (digitalRead(STOP_PIN) == LOW) {
    Keyboard.end();  // end sending stuff via Keyboard HID
    Mouse.end();     // end sending mouse events
    shouldBlink = false;
  }
}

void sendKey() {
  // figure out which pin was pressed
  for (int i = 1; i <= 2 ; i++) {
    if (digitalRead(i) == LOW) {
      val = pinToChar[i - 1];
      break;
    }
  }
}
