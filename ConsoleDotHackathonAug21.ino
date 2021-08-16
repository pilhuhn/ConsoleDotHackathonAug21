/**
   Arduino as external Keyboard.
   Needs an Arduino that is able to use the keyboard libraries.
   See https://www.arduino.cc/reference/en/language/functions/usb/keyboard/
   
*/

#include <Keyboard.h>

#define STOP_PIN 9        // pull to gnd to end keyboard functionality
#define BUTTON_PIN1 10
#define BUTTON_PIN2 11

boolean shouldBlink = true;

void setup() {
  
  pinMode(13, OUTPUT);
  pinMode(STOP_PIN, INPUT);
  digitalWrite(STOP_PIN, HIGH);    // Pull up

  pinMode(BUTTON_PIN1, INPUT);
  digitalWrite(BUTTON_PIN1, HIGH); // Pull up
  pinMode(BUTTON_PIN2, INPUT);
  digitalWrite(BUTTON_PIN2, HIGH); // Pull up
  
  Keyboard.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for 0.1 seconds
  if (shouldBlink) {
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    delay(100);
  }

  if (digitalRead(BUTTON_PIN1) == 0) {  // if the button goes low
    Keyboard.write('x');  // send a 'x' to the computer via Keyboard HID
  }
  if (digitalRead(BUTTON_PIN2) == 0) { // if the button goes low
    Keyboard.write('X');  // send 'X' to the computer via Keyboard HID
  }
  if (digitalRead(STOP_PIN) == 0) { // if the button goes low
    Keyboard.end();  // end sending stuff via Keyboard HID
    shouldBlink = false;
  }

  delay(100);  // delay so there aren't a ton of x's
}
