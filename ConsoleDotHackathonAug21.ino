/**
   Arduino as external Keyboard.
   Needs an Arduino that is able to use the keyboard libraries.
   See https://www.arduino.cc/reference/en/language/functions/usb/keyboard/

*/

#include <Keyboard.h>

#define STOP_PIN 0        // pull to gnd to end keyboard functionality
#define BUTTON_PIN1 1
#define BUTTON_PIN2 2

// #define DEBUG   // when uncommented enables serial output instead of keyboard.

boolean shouldBlink = true;

// Array to map pin to emitted character
int pinToChar[] = { 'X', 'Y'} ;

volatile char val = -1;


void setup() {

  pinMode(13, OUTPUT);
  for (int i = 0; i <= 2; i++) {
    pinMode(i, INPUT);        // Pin is for input
    digitalWrite(i, HIGH);    // Pull up. so it
  }

  // Start acting as a HID
#ifdef DEBUG
  Serial.begin(115200);
#else
  Keyboard.begin();
#endif  
  // Attach the interrupt handlers
  attachInterrupt(digitalPinToInterrupt(0), shouldEnd, FALLING);
  attachInterrupt(digitalPinToInterrupt(1), sendKey, FALLING);
  attachInterrupt(digitalPinToInterrupt(2), sendKey, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for 0.1 seconds
  if (shouldBlink) {
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    delay(100);
  }

  // If the interrups routine set a value we emit it.
  if (val != -1) {
#ifdef DEBUG    
    Serial.println(val);
    Serial.flush();
#else    
    Keyboard.write(val);
#endif    
    val = -1;
  }

}

void shouldEnd() {
  if (digitalRead(STOP_PIN) == LOW) {
#ifndef DEBUG    
    Keyboard.end();  // end sending stuff via Keyboard HID
#endif    
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
