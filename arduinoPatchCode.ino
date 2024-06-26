#include "ADCTouch.h"
#include "Adafruit_NeoPixel.h"

int touchPin = A0; // This pin is what you connect your touch sensor to, It's literally just a wire! so easy!

int analogPinOut = 11; // This pin is mapped to the touch sensitive pin

// WE ARE CURRENTLY USING SERIAL OUTPUT AND NOT PHYSICAL OUTPUT

// connect your LED, small motor, vibration motor, piezo buzzer or whatever you want to activate when something is touching the touch pin

//These two values map the touch sensor's value. Tweak these if you aren't getting the response you want. Look at the Serial Plotter to get a feel for the min and max values you are getting!
int val0high = 350;
int val0low = 87;

int val0constrained = 0; // This is the value that will be mapped to our output

int ref0; //This is the value we use for calibrating, we store this value and remove it from the raw data as an offset

//I have included an additional Output pin if you want something triggered just on and off when the touch goes past a certain threshold
int digitalPinOut = 12;
int digitalPinThresh = 30; //you can tweak this threshold to change when the pinout is triggered

// if you want to be able to recalibrate the Capactive Touch sensor, you can plug  a wire (or button) from pin 2 to GND
int recalibratePin = 2;

// led and neopixel pin assignments

int neoPixLed = 9;
int neoPixCount = 18;

Adafruit_NeoPixel ring(neoPixCount, neoPixLed, NEO_RGBW + NEO_KHZ800);

int ledPin = 4;

void setup()
{
  Serial.begin(9600);

  // Setup the ring
  ring.begin();
  ring.show();
  ring.setBrightness(50);

  // Setup the pins
  pinMode(ledPin, OUTPUT);

  pinMode(analogPinOut, OUTPUT);
  pinMode(digitalPinOut, OUTPUT);
  pinMode(recalibratePin, INPUT_PULLUP);

  ref0 = ADCTouch.read(touchPin, 500);    //Calibrate the capactive touch sensor to whatever the thing was touching when it first starts

  digitalWrite(analogPinOut, HIGH); // give us a twinkle when we start to let us know we are on and done calibrating
  delay(50);
}

void loop()
{

  /************************* TOUCH SENSOR *************************/

  int value0 = ADCTouch.read(touchPin);   //no second parameter

  //If the recalibrate pin is triggered, recalibrate the touch sensor
  if (0 == digitalRead(recalibratePin)) {
    ref0 = ADCTouch.read(touchPin, 500);   // Calibrate the capactive touch sensor to whatever the thing is now touching
  }

  value0 -= ref0;       //remove offset from the calibration

  Serial.println(value0);  //send Raw calibrated reading for debugging
  delay(5); // tiny delay to smooth out things

  if (value0 > 200) {digitalWrite(ledPin, HIGH);} else {digitalWrite(ledPin, LOW);}

  // Map the values to output values
  val0constrained = map(value0, val0low, val0high, 0, 255); // map from 0 to 255 for output for an LED or Buzzer
  val0constrained = constrain(val0constrained, 0, 255); // extra make sure it doesn't go past

  //Trigger the analog out
  analogWrite(analogPinOut, val0constrained);

  //Trigger the digital Output if past threshold (optional)
  if (val0constrained > digitalPinThresh) {
    digitalWrite(digitalPinOut, HIGH);
  }
  else {
    digitalWrite(digitalPinOut, LOW);

  }
  /************************* END TOUCH SENSOR *************************/

  /************************* NEOPIXEL CONTROLS *************************/

  // turns lights on in a pattern
  for (int i = 0; i < ring.numPixels(); i++) {
    ring.setPixelColor(i, random(255), random(255), random(255), 0);
    ring.show();
    delay(50);
  }
  // turns lights back off
  for (int i = ring.numPixels - 1; i >= 0; i++) {
    ring.setPixelColor(i, 0, 0, 0, 0);
    ring.show();
    delay(50);
  }

  // ADD CONTROLS FOR ALCOHOL SENSOR AND TO TRIGGER TOUCH BUTTON

  /************************* END NEOPIXEL CONTROLS *************************/


}