// https://github.com/Fraunhofer-IMS/AIfES_for_Arduino
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "EMGFilters.h"
#include <aifes.h> 
#include <Servo.h>
Servo myservo;

#define BiSensorInputPin A8  // bicep input pin number
#define TriSensorInputPin A9  // tricep input pin number

EMGFilters myFilter1;
EMGFilters myFilter2;
// discrete filters must works with fixed sample frequence
// our emg filter only support "SAMPLE_FREQ_500HZ" or "SAMPLE_FREQ_1000HZ"
// other sampleRate inputs will bypass all the EMG_FILTER
int sampleRate = SAMPLE_FREQ_500HZ;
// For countries where power transmission is at 50 Hz
// For countries where power transmission is at 60 Hz, need to change to
// "NOTCH_FREQ_60HZ"
// our emg filter only support 50Hz and 60Hz input
// other inputs will bypass all the EMG_FILTER
int humFreq = NOTCH_FREQ_60HZ;
unsigned long timeBudget;
bool make_predictions;

struct EMGData {
  double biRMS;
  double triRMS;
  String label;
};

struct EMGData_for_predictions {
  double biRMS;
  double triRMS;
};

// Define a global array to store the first 32 values of biRMS, triRMS, and label
// const int number_data_points = 32;
EMGData emg_Data[32]; // stores real-time emg data used to train model
EMGData_for_predictions emg_predictingData[32]; // stores real-time emg data used to make predictions

// Calibration:
// put on the sensors, and release your muscles;
// wait a few seconds, and select the max value as the threshold;
// any value under threshold will be set to zero
//static int Threshold = 0;

void setup() {
  // put your setup code here, to run once:
  myFilter1.init(sampleRate, humFreq, true, true, true);
  myFilter2.init(sampleRate, humFreq, true, true, true);
  timeBudget = 1e6/sampleRate;
  // open serial
  Serial.begin(230400);
  srand(analogRead(A5));  
  myservo.attach(A7);

}

unsigned int readBi() {
    unsigned int biValue, biDataAfterFilter, bienvlope;
    biValue = analogRead(BiSensorInputPin);
    biDataAfterFilter = myFilter1.update(biValue);
    bienvlope = sq(biDataAfterFilter);
    return(bienvlope);
    // return(biValue);
}

unsigned int readTri() {
    unsigned int triValue, triDataAfterFilter, trienvlope;
    triValue = analogRead(TriSensorInputPin);
    triDataAfterFilter = myFilter2.update(triValue);
    trienvlope = sq(triDataAfterFilter);
    return(trienvlope);
    // return(triValue);
}

void confirmSensors(unsigned int& biThresh, unsigned int& triThresh){
  unsigned long start, end;
  while (1){
    Serial.println(F("Confirming good sensor contact."));
    Serial.println(F("Keep your bicep and tricep as relaxed as possible."));
    Serial.println(F("Values printed to the screen should be as close to 0 as possible"));
    Serial.println(F("A message will be displayed in 10 seconds with more instructions."));
    unsigned int bienvlope, trienvlope;
    unsigned int flushTime = millis();
    while (millis() - flushTime < 5000) {
        start = micros();
        bienvlope = readBi();
        trienvlope = readTri();
        end = micros();
        delayMicroseconds(timeBudget - (end - start));
    }
    unsigned int biThreshold = 0;
    unsigned int triThreshold = 0;
    long startTime = millis();
    //loop for 5 seconds
    while (millis() - startTime < 5000) {
      start = micros();
      bienvlope = readBi();
      // trienvlope = readTri();
      
      Serial.print("Bi: ");
      Serial.println(bienvlope);
      // Serial.print("Tri: ");
      // Serial.println(trienvlope);
      if (bienvlope > biThreshold) {
        biThreshold = bienvlope;
        Serial.print(F("High Bicep Value: "));
        Serial.println(biThreshold);
      }
      //Serial.println(trienvlope);
      trienvlope = readTri();
      Serial.print("Tri: ");
      Serial.println(trienvlope);
      if (trienvlope > triThreshold) {
        triThreshold = trienvlope;
        Serial.print(F("High Tricep Value: "));
        Serial.println(triThreshold);
      }
      end = micros();
      delayMicroseconds(timeBudget - (end - start));
    }
    Serial.println(F(""));
    Serial.println("10 seconds has passed. ");
    if (biThreshold <= 1000 and triThreshold <= 1000) {
        Serial.println(F(""));
        Serial.println(F("Sensor placement confirmed."));
        Serial.print(F("Highest Bicep Value = "));
        Serial.println(biThreshold);
        Serial.print(F("Highest Tricep Value = "));
        Serial.println(triThreshold);
        //return the pair of biThreshold and triThreshold values
        biThresh = biThreshold;
        triThresh = triThreshold;
        Serial.println(triThreshold);
        return;
      } else if (biThreshold <= 1000 and triThreshold >= 1000){
        Serial.println(F(""));
        Serial.println(F("Bicep Sensor placement confirmed. Tricep Sensor placement failed"));
        Serial.println(F("Adjust Tricep Sensor placement and try again"));
        Serial.print(F("Highest Bicep Value = "));
        Serial.println(biThreshold);
        Serial.print(F("Highest Tricep Value = "));
        Serial.println(triThreshold);
        delay(5000);
      } else if (biThreshold >= 1000 and triThreshold <= 1000){
        Serial.println(F(""));
        Serial.print(F("Highest Bicep Value = "));
        Serial.println(biThreshold);
        Serial.print(F("Highest Tricep Value = "));
        Serial.println(triThreshold);
        Serial.println(F("Bicep Sensor placement failed. Tricep Sensor placement confirmed"));
        Serial.println(F("Adjust Bicep Sensor placement and try again in 5 seconds."));
        delay(5000);
        
      } else {
        Serial.println(F(""));
        Serial.print(F("Highest Bicep Value = "));
        Serial.println(biThreshold);
        Serial.print(F("Highest Tricep Value = "));
        Serial.println(triThreshold);
        Serial.println(F("Both sensor placements failed."));
        Serial.println(F("Adjust Sensor placements and try again in 5 seconds."));
        delay(5000);
    }
  }
}

// void actuateServo(String& movement){
//   String flexion;
//   String extension;

//   if (movement == flexion) {
//     // myservo.write(180);
//   }

//   else {
//     // myservo.write(0);
//   }
  
// }
//1. get thresholds for both tricep and bicep
//LOOP:
//2. read in 1/4 seconds worth of samples into tribuffer and bibuffer
//3. get RMS on these buffers
//4. make activation decision based on RMS (ML)
//5. activate servo in based on activation response (keep track of current location and slow the movement as it is close to the endpoints)
//6. enter loop again

// put your main code here, to run repeatedly:
void loop() {
  // unsigned int biThresh, triThresh;

  // continue labeling and retraining unless make_predictions is true (make_predictions = true if model is above 85%)
  if (make_predictions == true) {
    double biRMS, triRMS;
    // collect data and calculate RMS just before making predictions:
    while(1) {
      calculateRMS(biRMS, triRMS);
      float array = model_predictions(biRMS, triRMS);
      Serial.println("PROGRAM EXITED");
      // exit(0);
    }
    
  }

  else {
    confirmSensors(biThresh, triThresh);
    Serial.println("Training will begin with flexion. A countdown will be given shortly");
    delay(3000);
    Serial.println("Start flexion in: ");
    Serial.println("5");
    delay(1000);
    Serial.println("4");
    delay(1000);
    Serial.println("3");
    delay(1000);
    Serial.println("2");
    delay(1000);
    Serial.println("1");
    delay(1000);

    labelData(); // labels and collects 8 seconds of data, then calculates RMS
    Serial.println("Relax arm. Model training will now begin");

    build_AIfES_model();
    // float accuracy = train_AIfES_model(emg_Data, number_data_points);
    float accuracy = train_AIfES_model(emg_Data);
    // if model accuracy is above 85%, break out of loop to take in new data that will be used to make predictions. Then continue to actuation of servo:
    if (accuracy > 40) {
      Serial.println("Model accuracy is above 85%. Predictions will now be made on new data.");
      make_predictions = true;
    }

    else {
      // modify this else statement to go back to section of code that collects data and retrain model with new data 
      Serial.println("Model is below 85% accuracy. It should be retrained with new data");
      make_predictions = false;
    }

  }
}