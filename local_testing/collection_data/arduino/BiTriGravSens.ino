#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "EMGFilters.h"
//#include "algorithm.h"
#include <Servo.h>
#define TIMING_DEBUG 1

#define BiSensorInputPin A0   // bicep input pin number
#define TriSensorInputPin A1  // tricep input pin number

EMGFilters myFilter;
// discrete filters must works with fixed sample frequence
// our emg filter only support "SAMPLE_FREQ_500HZ" or "SAMPLE_FREQ_1000HZ"
// other sampleRate inputs will bypass all the EMG_FILTER
int sampleRate = SAMPLE_FREQ_1000HZ;
// For countries where power transmission is at 50 Hz
// For countries where power transmission is at 60 Hz, need to change to
// "NOTCH_FREQ_60HZ"
// our emg filter only support 50Hz and 60Hz input
// other inputs will bypass all the EMG_FILTER
int humFreq = NOTCH_FREQ_60HZ;

// Calibration:
// put on the sensors, and release your muscles;
// wait a few seconds, and select the max value as the threshold;
// any value under threshold will be set to zero
//static int Threshold = 0;
Servo myservo;

void setup() {
  /* add setup code here */
  myFilter.init(sampleRate, humFreq, true, true, true);

  // open serial
  Serial.begin(115200);

  myservo.attach(9);
  // micros will overflow and auto return to zero every 70 minutes
}

unsigned long readPins() {
  unsigned long biValue, biDataAfterFilter, bienvlope, triValue, triDataAfterFilter, trienvlope;
  biValue = analogRead(BiSensorInputPin);
  biDataAfterFilter = myFilter.update(biValue);
  bienvlope = sq(biDataAfterFilter);
  triValue = analogRead(TriSensorInputPin);
  triDataAfterFilter = myFilter.update(triValue);
  trienvlope = sq(triDataAfterFilter);
  return(bienvlope, trienvlope);
}
void confirmSensors(){
  while (1){
    Serial.println("Confirming good sensor contact.");
    Serial.println("Keep your bicep and tricep as relaxed as possible.");
    Serial.println("Values printed to the screen should be as close to 0 as possible");
    Serial.println("A message will be displayed in 10 seconds with more instructions.");

    unsigned long bienvlope, trienvlope;
    unsigned long flushTime = millis();
    while (millis() - flushTime < 5000) {
        bienvlope, trienvlope = readPins();
      }
    unsigned long biThreshold = 0;
    unsigned long triThreshold = 0;
    long startTime = millis();
    //loop for 5 seconds
    while (millis() - startTime < 5000) {
      bienvlope, trienvlope = readPins();
      //Serial.println(bienvlope);
      if (bienvlope > biThreshold) {
        biThreshold = bienvlope;
        Serial.print("Bicep Value: ");
        Serial.println(biThreshold);
      }
      //Serial.println(trienvlope);
      if (trienvlope > triThreshold) {
        triThreshold = trienvlope;
        Serial.print("Tricep Value: ");
        Serial.println(triThreshold);
      }
    }
    Serial.println("");
    Serial.println("10 seconds has passed. ");
    if (biThreshold <= 1000 and triThreshold <= 1000) {
        Serial.println("");
        Serial.println("Sensor placement confirmed.");
        Serial.print("Highest Bicep Value = ");
        Serial.println(biThreshold);
        Serial.print("Highest Tricep Value = ");
        Serial.println(triThreshold);
        return;
      } else if (biThreshold <= 1000 and triThreshold >= 1000){
        Serial.println("");
        Serial.println("Bicep Sensor placement confirmed. Tricep Sensor placement failed");
        Serial.println("Adjust Tricep Sensor placement and try again");
        Serial.print("Highest Bicep Value = ");
        Serial.println(biThreshold);
        Serial.print("Highest Tricep Value = ");
        Serial.println(triThreshold);
        delay(5000);
      } else if (biThreshold >= 1000 and triThreshold <= 1000){
        Serial.println("");
        Serial.print("Highest Bicep Value = ");
        Serial.println(biThreshold);
        Serial.print("Highest Tricep Value = ");
        Serial.println(triThreshold);
        Serial.println("Bicep Sensor placement failed. Tricep Sensor placement confirmed");
        Serial.println("Adjust Bicep Sensor placement and try again in 5 seconds.");
        delay(5000);
        
      } else {
        Serial.println("");
        Serial.print("Highest Bicep Value = ");
        Serial.println(biThreshold);
        Serial.print("Highest Tricep Value = ");
        Serial.println(triThreshold);
        Serial.println("Both sensor placements failed.");
        Serial.println("Adjust Sensor placements and try again in 5 seconds.");
        delay(5000);
    }
  }
}
int calcExtend() {
  //loop the function until good enough data has been collected to continue
  while (1) {
    Serial.println("");
    Serial.println("Keep your arm in activated extension orientation with the sensor in place for 10 seconds.");
    Serial.println("Attempt maximum extension intensity while maintaining comfort.");
    Serial.println("This should be the same muscle activty you want to use to put the brace in extension orientation.");
    //Serial.println("The values printed to the screen should be as low as possible.");
    Serial.println("A message will appear when 10 seconds has passed.");
    long flushTime = millis();
    unsigned long bienvlope, trienvlope;
    //delay data collection for 4 seconds to allow for reading of the message, flush 4 seconds of values
    while (millis() - flushTime < 5000) {
      bienvlope, trienvlope = readPins();
    }
    unsigned long biThreshold = 0;
    unsigned long triThreshold = 0;
    long startTime = millis();
    //loop for 5 seconds
    while (millis() - startTime < 10000) {
      bienvlope, trienvlope = readPins();
      //Serial.println(bienvlope);
      if (bienvlope > biThreshold) {
        biThreshold = bienvlope;
      }
      //Serial.println(trienvlope);
      if (trienvlope > triThreshold) {
        triThreshold = trienvlope;
      }
    }
    Serial.println("");
    Serial.println("10 seconds has passed. ");
    Serial.print("Extension Bicep Threshold: ");
    Serial.println(biThreshold);
    Serial.print("Extension Tricep Threshold: ");
    Serial.println(triThreshold);
    return(biThreshold, triThreshold);
  }
}

int calcFlex(){
  //sample at 20/second
  //instantiate a list to hold 30 seconds of thresholding data

  //most likely problem is improper initialization of list
  //unsigned long data[100];

  //start a timer for 30 seconds
  Serial.println("");
  Serial.println("Flex your arm with the sensor in place for 10 seconds.");
  Serial.println("Attempt maximum flex intensity while maintaining comfort.");
  Serial.println("This should be the same muscle activty you want to use to put the brace in flexion orientation.");
  Serial.println("A message will appear when 10 seconds has passed.");
  long flushTime = millis();
  unsigned long bienvlope, trienvlope;
  //delay data collection for 4 seconds to allow for reading of the message, flush 4 seconds of values
  while (millis() - flushTime < 4000) {
    bienvlope, trienvlope = readPins();
  }
  unsigned long biThreshold = 0;
  unsigned long triThreshold = 0;
  long startTime = millis();
  //loop for 5 seconds
  while (millis() - startTime < 10000) {
    bienvlope, trienvlope = readPins();
    //Serial.println(bienvlope);
    if (bienvlope > biThreshold) {
      biThreshold = bienvlope;
    }
    //Serial.println(trienvlope);
    if (trienvlope > triThreshold) {
      triThreshold = trienvlope;
    }
  }
  Serial.println("");
  Serial.println("10 seconds has passed. ");
  Serial.println("");
  Serial.println("Automated Flexion Thresholding Complete.");
  Serial.print("Calculated bicep flexion max = ");
  Serial.println(biThreshold);
  Serial.print("Calculated tricep flexion max = ");
  Serial.println(triThreshold);
  return (biThreshold, triThreshold);
}
void actuate(unsigned long biThresh, unsigned long triThresh, unsigned long biFlex, unsigned long triFlex) {
  while (1) {
    //gather data from .5 seconds, if any values are above thresh (demonstrating activated muscle), the servomotor will "flex" the brace
    unsigned long bienvlope, trienvlope;
    unsigned long biExt = biThresh;
    unsigned long triExt = triThresh;
    unsigned long biFlx = biFlex;
    unsigned long triFlx = triFlex;
    long startTime = millis();
    int bf = 0;
    int be = 0;
    int tf = 0;
    int te = 0;
    int bcount = 0;
    int tcount = 0;
    //gather .5 seconds of sensor inputs
    while (millis() - startTime < 500) {
      //read bicep sensor
      bienvlope, trienvlope = readPins();
      
      //keep track of instances greater or less than thresholds
      if (bienvlope > biExt) {   
        bf++;    
      }
      if (bienvlope < biFlx) {
        be++;
      }
      if (trienvlope < triExt) {
        tf++;
      }
      if (trienvlope > triFlx) {
        te++;
      }
    }
    //if max is greater than threshold (if any values were greater than threshold), set max = max. Else, max = 0
    //Use machine learning to set "weight"
    //biMax = (biMax > biThresh * 1.5) ? biMax : 0;
    //triMax = (triMax > triThresh * 1.5) ? triMax : 0;
    //timeStamp = micros() - timeStamp;
    if (TIMING_DEBUG) {
      // Serial.print("Read Data: "); Serial.println(Value);
      // Serial.print("Filtered Data: ");Serial.println(DataAfterFilter);
      //Serial.print("Squared Data: ");
      //Serial.println(envlope);
      bcount = bf + be;
      tcount = tf + te;
      int val = 0;
      int pos = 0;
      if (bf>=10 and tf>=10) {
        //potentially use this later to map servo actuation to amplitude of max flexion value
        //val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
        Serial.print("number of bicep samples indicating flexion: ");
        Serial.println(bf);
        Serial.print("number of tricep samples indicating flexion: ");
        Serial.println(tf);
        Serial.print("number of bicep samples indicating extension: ");
        Serial.println(be);
        Serial.print("number of tricep samples indicating extension: ");
        Serial.println(te);
        Serial.println("flexing servo");  
        //create a variable speed activation based on the amount of bicep samples indicating flexion
        //slowest speed takes 2.5 seconds to flex
        //val = map(bf, 0, bcount , 0, 180);
        Serial.println("flexing servo"); 
        Serial.println(180*10*bcount/bf); 
        for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          //mimimum activation time is just under 1 second
          delay(5* bcount/bf);                       
        }
        //Serial.println(myservo.read());
      } else if (be>=10 and te>=10) {
        Serial.print("number of bicep samples indicating flexion: ");
        Serial.println(bf);
        Serial.print("number of tricep samples indicating flexion: ");
        Serial.println(tf);
        Serial.print("number of bicep samples indicating extension: ");
        Serial.println(be);
        Serial.print("number of tricep samples indicating extension: ");
        Serial.println(te);
        Serial.println("extending servo");
        Serial.println(180*10*bcount/be); 
        for (pos = 180; pos >= 0; pos -= 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          //mimimum activation time is just under 1 second
          delay(5* bcount/be);                       
        }
        //Serial.println(myservo.read());
      } else if (bf>=10 and te>=10) {
        Serial.print("number of bicep samples indicating flexion: ");
        Serial.println(bf);
        Serial.print("number of tricep samples indicating flexion: ");
        Serial.println(tf);
        Serial.print("number of bicep samples indicating extension: ");
        Serial.println(be);
        Serial.print("number of tricep samples indicating extension: ");
        Serial.println(te);
        Serial.println("neutral servo");
        int loc = myservo.read();
        if (loc > 90){
          Serial.println(180*10*tcount/te); 
          for (pos = loc; pos >= 90; pos -= 1) { // goes from 0 degrees to 180 degrees
            // in steps of 1 degree
            myservo.write(pos);              // tell servo to go to position in variable 'pos'
            //mimimum activation time is just under .5 second
            delay(5* tcount/te);                       
          }
        }
        else if (loc < 90){
          Serial.println(180*10*bcount/bf); 
          for (pos = loc; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
            // in steps of 1 degree
            myservo.write(pos);              // tell servo to go to position in variable 'pos'
            //mimimum activation time is just under .5 second
            delay(5* bcount/bf);                       
          }
        }
      } else {
        Serial.print("number of bicep samples indicating flexion: ");
        Serial.println(bf);
        Serial.print("number of tricep samples indicating flexion: ");
        Serial.println(tf);
        Serial.print("number of bicep samples indicating extension: ");
        Serial.println(be);
        Serial.print("number of tricep samples indicating extension: ");
        Serial.println(te);
        Serial.println("Inconclusive data, do nothing.");
      }
    }
  }
}
void loop() {
  unsigned long biExtendThresh, triExtendThresh, biFlexThresh, triFlexThresh;
  //get threshold values for extension
  confirmSensors();
  biExtendThresh, triExtendThresh = calcExtend();
  //get threshold values for flexion
  biFlexThresh, triFlexThresh = calcFlex();
  actuate(biExtendThresh, triExtendThresh, biFlexThresh, triFlexThresh);
}