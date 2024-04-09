// #include <Servo.h>

// Servo myservo;  // create servo object to control a servo
// // twelve servo objects can be created on most boards

// int pos = 0;    // variable to store the servo position

// void setup() {
//   myservo.attach(6);  // attaches the servo on pin 9 to the servo object
// }

// void loop() {
//   while (1){
//     myservo.write(90);
//     delay(500);
//     myservo.write(0);
//     delay(500);
//   }
  
// }

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(A9);  // attaches the servo on pin 9 to the servo object
  Serial.begin(115200);
}

void loop() {
  int init = myservo.read();
  // endpoint can be input to the function in actual implementation
  int endpoint = 100;
  int mid = (endpoint - init)/2;
  int mid_sq = sq((endpoint - init)/2);
  int x = 0;
  int d = 0;
  myservo.write(0);
  if (myservo.read() != endpoint){
    for (pos = init; pos <= endpoint; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      
      x = sq(pos - mid);
      d = map(x, 0, mid_sq, 5, 20);
      Serial.print(pos);
      Serial.print(",");
      Serial.println(d);
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(d);                       // waits 15ms for the servo to reach the position
      Serial.println(F("servo moving"));
    }
  }
  myservo.write(0);
  // else{
  //   // int dist = sq(init);
  //   for (pos = init; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
  //     // do not subtract because endpoint is always 0
  //     x = sq(pos - mid);
  //     d = map(x, 0, mid_sq, 1, 10);
  //     myservo.write(pos);              // tell servo to go to position in variable 'pos'
  //     delay(10);                       // waits 15ms for the servo to reach the position
  //     Serial.println(F("servo moving backwards"));
  //   }
  // }
}