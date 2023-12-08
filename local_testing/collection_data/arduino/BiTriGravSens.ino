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

unsigned long timeStamp;
unsigned long timeBudget;
Servo myservo;

void setup() {
  /* add setup code here */
  myFilter.init(sampleRate, humFreq, true, true, true);

  // open serial
  Serial.begin(115200);

  // setup for time cost measure
  // using micros()
  timeBudget = 1e6 / sampleRate;
  myservo.attach(9);
  // micros will overflow and auto return to zero every 70 minutes
}

int bi_calc_Thresh() {
  //loop the function until good enough data has been collected to continue
  while (1) {
    //sample at 20/second
    //instantiate a list to hold 30 seconds of thresholding data

    //most likely problem is improper initialization of list
    //unsigned long data[100];

    //start a timer for 30 seconds
    Serial.println("");
    Serial.println("Relax your arm with the sensor in place for 5 seconds.");
    Serial.println("Try to keep bicep activity at a minimum.");
    Serial.println("The values printed to the screen should be as low as possible.");
    Serial.println("A message will appear when 10 seconds has passed.");
    long flushTime = millis();
    unsigned long Value, DataAfterFilter, envlope;
    //delay data collection for 4 seconds to allow for reading of the message, flush 4 seconds of values
    while (millis() - flushTime < 4000) {
      Value = analogRead(BiSensorInputPin);
      DataAfterFilter = myFilter.update(Value);
      envlope = sq(DataAfterFilter);
    }
    unsigned long Threshold = 0;
    long startTime = millis();
    //loop for 5 seconds
    while (millis() - startTime < 10000) {
      Value = analogRead(BiSensorInputPin);
      DataAfterFilter = myFilter.update(Value);
      envlope = sq(DataAfterFilter);
      Serial.println(envlope);
      if (envlope > Threshold) {
        Threshold = envlope;
      }
    }
    Serial.println("");
    Serial.println("10 seconds has passed. ");
    //if highest value from past 5 seconds is below a certain signal quality threshold, set threshold for resting muscle activity to the highest value
    if (Threshold <= 1000) {
      Serial.println("");
      Serial.println("Automated Thresholding Complete.");
      Serial.print("Calculated Threshold = ");
      Serial.println(Threshold);
      return (Threshold);
    } else {
      Serial.println("");
      Serial.print("Calculated Threshold = ");
      Serial.println(Threshold);
      Serial.println("Bad data, adjust the sensor. Automated Thresholding will be attempted again in 5 seconds.");
      delay(5000);
    }
  }
}
int tri_calc_Thresh() {
  //loop the function until good enough data has been collected to continue
  while (1) {
    //sample at 20/second
    //instantiate a list to hold 30 seconds of thresholding data

    //most likely problem is improper initialization of list
    //unsigned long data[100];

    //start a timer for 30 seconds
    Serial.println("");
    Serial.println("Relax your arm with the sensor in place for 10 seconds.");
    Serial.println("Try to keep tricep activity at a minimum.");
    Serial.println("The values printed to the screen should be as low as possible.");
    Serial.println("A message will appear when 5 seconds has passed.");
    long flushTime = millis();
    unsigned long Value, DataAfterFilter, envlope;
    //delay data collection for 4 seconds to allow for reading of the message, flush 4 seconds of values
    while (millis() - flushTime < 4000) {
      Value = analogRead(TriSensorInputPin);
      DataAfterFilter = myFilter.update(Value);
      envlope = sq(DataAfterFilter);
    }
    unsigned long Threshold = 0;
    long startTime = millis();
    //loop for 5 seconds
    while (millis() - startTime < 10000) {
      Value = analogRead(TriSensorInputPin);
      DataAfterFilter = myFilter.update(Value);
      envlope = sq(DataAfterFilter);
      Serial.println(envlope);
      if (envlope > Threshold) {
        Threshold = envlope;
      }
    }
    Serial.println("");
    Serial.println("10 seconds has passed. ");
    //if highest value from past 5 seconds is below a certain signal quality threshold, set threshold for resting muscle activity to the highest value
    if (Threshold <= 1000) {
      Serial.println("");
      Serial.println("Automated Thresholding Complete.");
      Serial.print("Calculated Threshold = ");
      Serial.println(Threshold);
      return (Threshold);
    } else {
      Serial.println("");
      Serial.print("Calculated Threshold = ");
      Serial.println(Threshold);
      Serial.println("Bad data, adjust the sensor. Automated Thresholding will be attempted again in 5 seconds.");
      delay(5000);
    }
  }
}
void actuate(unsigned long biThresh, unsigned long triThresh) {
  while (1) {
    //gather data from .5 seconds, if any values are above thresh (demonstrating activated muscle), the servomotor will "flex" the brace
    unsigned long biValue, biDataAfterFilter, bienvlope, triValue, triDataAfterFilter, trienvlope;
    unsigned long biMax = biThresh;
    unsigned long triMax = triThresh;
    long startTime = millis();
    int i = 0;
    int j = 0;
    while (millis() - startTime < 1000) {
      //read bicep sensor
      biValue = analogRead(BiSensorInputPin);
      biDataAfterFilter = myFilter.update(biValue);
      bienvlope = sq(biDataAfterFilter);
      //read tricep sensor
      triValue = analogRead(TriSensorInputPin);
      triDataAfterFilter = myFilter.update(triValue);
      trienvlope = sq(triDataAfterFilter);
      //Serial.println(envlope);
      //if read data is greater than threshold, set max = data
      if (bienvlope > biMax) {
        //biMax = bienvlope;
        i++;
        //Serial.println(bienvlope);
      }
      if (trienvlope > triMax) {
        //triMax = trienvlope;
        j++;
        //Serial.println(trienvlope);
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
      if (i>=10 and j<4) {
        //potentially use this later to map servo actuation to amplitude of max flexion value
        //val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
        Serial.print("number of bicep samples above threshold: ");
        Serial.println(i);
        Serial.print("number of tricep samples above threshold: ");
        Serial.println(j);
        Serial.println("flexing servo");
        myservo.write(100);
        //Serial.println(myservo.read());
      } else if (i<4 and j>=10) {
        Serial.print("number of bicep samples above threshold: ");
        Serial.println(i);
        Serial.print("number of tricep samples above threshold: ");
        Serial.println(j);
        Serial.println("extending servo");
        myservo.write(0);
        //Serial.println(myservo.read());
      } else if (i>=10 and j>=10) {
        Serial.print("number of bicep samples above threshold: ");
        Serial.println(i);
        Serial.print("number of tricep samples above threshold: ");
        Serial.println(j);
        Serial.println("neutral servo");
        myservo.write(90);
      } else {
        Serial.print("number of bicep samples above threshold: ");
        Serial.println(i);
        Serial.print("number of tricep samples above threshold: ");
        Serial.println(j);
        Serial.println("Inconclusive data, do nothing.");
      }
    }
  }
}
void loop() {
  unsigned long biThreshold, triThreshold;
  biThreshold = bi_calc_Thresh();
  triThreshold = tri_calc_Thresh();
  actuate(biThreshold, triThreshold);
}