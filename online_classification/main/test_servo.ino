// https://github.com/Fraunhofer-IMS/AIfES_for_Arduino
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "EMGFilters.h"
#include <aifes.h> 
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Servo myservo;

void setup() {
  Serial.begin(230400);
  srand(analogRead(A5));  
  myservo.attach(A7);
}

void actuateServo(int pred){
  // if (pred == 1) {
  //   Serial.println("Writing 5");
  //   myservo.write(10);
  // }
  // else if (pred == 0) {
  //   Serial.println("Writing 180");
  //   myservo.write(130);
  // }

  String flexion;
  String extension;
  int start = myservo.read();
  int x = 0;
  int pos = 0;
  int d = 0;
  
  // if prediction is flexion
  if (pred == 1) {
    int target = 180;
    int mid = sq((target - start)/2);
    if (target > start){
      for (pos = start; pos <= target; pos ++){
        myservo.write(pos);
        x = sq(pos) - mid;
        d = map(x,0,mid,5000,20000);
        delayMicroseconds(d);
      }
    }
  }

  //if prediciton is extension
  else if (pred == 0) {
    int target = 10;
    int mid = sq((target - start)/2);
    for (pos = start; pos >= target; pos --){
        myservo.write(pos);
        x = sq(pos) - mid;
        d = map(x,0,mid,5000,20000);
        delayMicroseconds(d);
      }
  }

}


void loop() {
  Serial.println("Writing for 180");
  actuateServo(1);
  delay(500);
  Serial.println("Writing for 10");
  actuateServo(0);
  delay(500);
}
