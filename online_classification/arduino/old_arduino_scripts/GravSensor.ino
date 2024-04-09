#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "EMGFilters.h"
//#include "algorithm.h"
#include <Servo.h>
#define TIMING_DEBUG 1

#define SensorInputPin A0 // input pin number

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
    Serial.begin(9600);

    // setup for time cost measure
    // using micros()
    timeBudget = 1e6 / sampleRate;
    myservo.attach(9);
    // micros will overflow and auto return to zero every 70 minutes
}

bool cmp(int x, int y) {
    return (y < x);
}

int calc_Thresh(){
  //loop the function until good enough data has been collected to continue
    while(1){
      //sample at 20/second
      //instantiate a list to hold 30 seconds of thresholding data

      //most likely problem is improper initialization of list
      unsigned long data[100];
      
      //start a timer for 30 seconds
      Serial.println("");
      Serial.println("Relax your arm with the sensor in place for 5 seconds."); 
      Serial.println("Try to keep bicep activity at a minimum.");
      Serial.println("The values printed to the screen should be as low as possible.");
      Serial.println("A message will appear when 5 seconds has passed.");
      //delay data collection for 4 seconds to allow for reading of the message
      delay(4000);
      long startTime = millis();
      //loop for 10 seconds
      while (millis() - startTime < 5000){
        //Serial.print(end-start);
        int i = 0;
        int Value = analogRead(SensorInputPin); 
        int DataAfterFilter = myFilter.update(Value);
        unsigned long envlope = sq(DataAfterFilter);

        Serial.println(envlope);

        //append the new value to the list
        //data.push_back(DataAfterFilter);
        data[i] = envlope;
        //delay 100 ms to not overwrite data array; limits to 100 samples in 10 seconds
        delay(50);
        ++i;
        //end = micros();
      }
      int Threshold = data[0];
      for (int j = 1; j < 100; j++) {
          Serial.println(data[j]);
          if (cmp(data[j], Threshold)) {
              Threshold = data[j];
              //Serial.println(data[j]);
          }
      }
      Serial.println("");
      Serial.println("5 seconds has passed. ");
      if (Threshold <= 500){
        Serial.println("");
        Serial.println("Automated Thresholding Complete.");
        Serial.print("Calculated Threshold = ");
        Serial.println(Threshold);
        return(Threshold);
      }
      else {
        Serial.println("");
        Serial.println("Bad data, adjust the sensor. Automated Thresholding will be attempted again in 5 seconds.");
        //clear the array
        for (int i = 0; i<100; i++){
          data[i] = 0;
          Serial.println(i);
        }
        delay(5000);
      }
    }
}

void actuate(unsigned int Threshold) {
    while (1) {
      int Value = analogRead(SensorInputPin);

      // filter processing
      int DataAfterFilter = myFilter.update(Value);

      int envlope = sq(DataAfterFilter);
      //int envlope = sq(Value);
      // any value under threshold will be set to zero
      envlope = (envlope > Threshold) ? envlope : 0;

      //timeStamp = micros() - timeStamp;
      if (TIMING_DEBUG) {
          // Serial.print("Read Data: "); Serial.println(Value);
          // Serial.print("Filtered Data: ");Serial.println(DataAfterFilter);
          //Serial.print("Squared Data: ");
          Serial.println(envlope);
          if(envlope){
            //potentially use this later to map servo actuation to amplitude of max flexion value
            //val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
            myservo.write(180); 
            delay(500);
          }
          else if (!envlope){
            myservo.write(0); 
            delay(500);
          }
          // Serial.print("Filters cost time: "); Serial.println(timeStamp);
          // the filter cost average around 520 us
      }

      /*------------end here---------------------*/
      // if less than timeBudget, then you still have (timeBudget - timeStamp) to
      // do your work
      //delayMicroseconds(500);
      // if more than timeBudget, the sample rate need to reduce to
      // SAMPLE_FREQ_500HZ
    }
}
void loop() {
    /* add main program code here */
    // In order to make sure the ADC sample frequence on arduino,
    // the time cost should be measured each loop
    /*------------start here-------------------*/
    //timeStamp = micros();
    Serial.println("test");
    unsigned int Threshold;
    Threshold = calc_Thresh();
    actuate(Threshold);
}