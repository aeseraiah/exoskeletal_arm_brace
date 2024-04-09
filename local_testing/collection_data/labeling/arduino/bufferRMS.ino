#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "EMGFilters.h"


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

void setup() {
  // put your setup code here, to run once:
  myFilter.init(sampleRate, humFreq, true, true, true);

  // open serial
  Serial.begin(115200);
}

unsigned long readBi() {
    unsigned long biValue, biDataAfterFilter, bienvlope;
    biValue = analogRead(BiSensorInputPin);
    biDataAfterFilter = myFilter.update(biValue);
    bienvlope = sq(biDataAfterFilter);
    return(biValue);
}

unsigned long readTri() {
    unsigned long triValue, triDataAfterFilter, trienvlope;
    triValue = analogRead(TriSensorInputPin);
    triDataAfterFilter = myFilter.update(triValue);
    trienvlope = sq(triDataAfterFilter);
    return(triValue);
}
void confirmSensors(unsigned long& biThresh, unsigned long& triThresh){
  while (1){
    Serial.println(F("Confirming good sensor contact."));
    Serial.println(F("Keep your bicep and tricep as relaxed as possible."));
    Serial.println(F("Values printed to the screen should be as close to 0 as possible"));
    Serial.println(F("A message will be displayed in 10 seconds with more instructions."));
    unsigned long bienvlope, trienvlope;
    unsigned long flushTime = millis();
    while (millis() - flushTime < 5000) {
        bienvlope = readBi();
        trienvlope = readTri();
      }
    unsigned long biThreshold = 0;
    unsigned long triThreshold = 0;
    long startTime = millis();
    //loop for 5 seconds
    while (millis() - startTime < 5000) {
      bienvlope = readBi();
      trienvlope = readTri();
      Serial.print("Bi: ");
      Serial.println(bienvlope);
      Serial.print("Tri: ");
      Serial.println(trienvlope);
      //Serial.println(bienvlope);
      if (bienvlope > biThreshold) {
        biThreshold = bienvlope;
        Serial.print(F("Bicep Value: "));
        Serial.println(biThreshold);
      }
      //Serial.println(trienvlope);
      if (trienvlope > triThreshold) {
        triThreshold = trienvlope;
        Serial.print(F("Tricep Value: "));
        Serial.println(triThreshold);
      }
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
void actuateServo(){
  
}
//1. get thresholds for both tricep and bicep
//LOOP:
//2. read in 1/4 seconds worth of samples into tribuffer and bibuffer
//3. get RMS on these buffers
//4. make activation decision based on RMS (TINY ML)
//5. activate servo in based on activation response (keep track of current location and slow the movement as it is close to the endpoints)
//6. enter loop again

double calculateRMS(unsigned long buffer[], int size) {
  double sumOfSquares = 0.0;
  for (int i = 0; i < size; i++) {
    sumOfSquares += sq(buffer[i]);
  }
  return sqrt(sumOfSquares / size);
}


void loop() {
  // put your main code here, to run repeatedly:
  unsigned long biThresh, triThresh;
  confirmSensors(biThresh, triThresh);
  Serial.println(F("Testing reception of biThresh and triThresh in main:"));
  Serial.print(F("bi = "));
  Serial.println(biThresh);
  Serial.print(F("tri = "));
  Serial.println(triThresh);
  delay(1000);
  unsigned long start = millis();
  unsigned int bi = 0;
  unsigned int tri = 0;
  unsigned long time = 0;
  int samples = 0;
  unsigned long biBuffer[1000];
  unsigned long triBuffer[1000];
  // while (1){
  //   for (samples = 0; samples<1000; samples ++){
  //     biBuffer[samples] = readBi();
  //     triBuffer[samples] = readTri();
  //   }
  //   Serial.println(F("Time to get 1000 samples (ms):"));
  //   Serial.print(millis() - start);
  // }
}

void loop() {
  // ... (existing code)

  // Variables for labeling
  unsigned long labelStartTime = millis();
  unsigned long labelDuration = 250; // 1/4 second
  unsigned long switchDuration = 5000; // 5 seconds
  String currentLabel = "unknown";

  while (1) {
    // Check the current state based on the labeling timing
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - labelStartTime;
    
    if (elapsedTime >= switchDuration) {
      // Switch label every 5 seconds
      labelStartTime = currentTime;
      if (currentLabel == "flexion") {
        currentLabel = "extension";
      } else {
        currentLabel = "flexion";
      }
    }

    // Repeat each label 4 times within a second
    for (int i = 0; i < 4; i++) {
      // Buffer and RMS calculation
      int samples;
      unsigned long biBuffer[1000];
      unsigned long triBuffer[1000];

      // Read in samples for 1/4 seconds
      unsigned long bufferStartTime = millis();
      while (millis() - bufferStartTime < labelDuration) {
        for (samples = 0; samples < 1000; samples++) {
          biBuffer[samples] = readBi();
          triBuffer[samples] = readTri();
        }
      }

      // Calculate RMS for biBuffer and triBuffer
      double biRMS = calculateRMS(biBuffer, 1000);
      double triRMS = calculateRMS(triBuffer, 1000);

      // Print RMS values and current label
      Serial.print(biRMS);
      Serial.print(",");
      Serial.print(triRMS);
      Serial.print(",");
      Serial.println(currentLabel);
    }
  }
}

// 10 seconds of data where the label switches between "flexion" and "extension" every 5 seconds, and each label is repeated 4 times within a single second
// csv/print output should have this format:
// 11.78,17.92,flexion
// 13.21,19.05,flexion
// 10.98,16.78,flexion
// 14.32,20.14,flexion
// 12.65,18.42,flexion
// 13.87,19.78,flexion
// 11.54,17.21,flexion
// 12.89,18.97,flexion
// 13.45,19.32,flexion
// 12.12,18.15,flexion
// 14.01,20.05,flexion
// 11.76,17.86,flexion
// 13.28,19.12,flexion
// 12.45,18.23,flexion
// 14.02,20.07,flexion
// 11.78,17.92,flexion
// 13.21,19.05,flexion
// 10.98,16.78,flexion
// 14.32,20.14,flexion
// 14.32,20.14,flexion
// 14.32,20.14,flexion
// 14.32,20.14,flexion
// 14.32,20.14,flexion
// 14.32,20.14,flexion
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension
// 14.32,20.14,extension

