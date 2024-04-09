#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "EMGFilters.h"


#define BiSensorInputPin A0   // bicep input pin number
#define TriSensorInputPin A1  // tricep input pin number

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
  //input pulldown is not declared? Might not exist for this processor
  //pinMode (BiSensorInputPin, INPUT_PULLDOWN);
  //pinMode (TriSensorInputPin, INPUT_PULLDOWN);
  // open serial
  Serial.begin(230400);
  // Serial.begin(250000);
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
void actuateServo(){
  
}
//1. get thresholds for both tricep and bicep
//LOOP:
//2. read in 1/4 seconds worth of samples into tribuffer and bibuffer
//3. get RMS on these buffers
//4. make activation decision based on RMS (TINY ML)
//5. activate servo in based on activation response (keep track of current location and slow the movement as it is close to the endpoints)
//6. enter loop again

double calculateRMS(unsigned int buffer[], int size) {
  double sumOfSquares = 0.0;
  for (int i = 0; i < size; i++) {
    sumOfSquares += sq(buffer[i]);
  }
  return sqrt(sumOfSquares / size);
}

void loop() {
  // put your main code here, to run repeatedly:
  double biRMS, triRMS;
  unsigned int biThresh, triThresh;
  confirmSensors(biThresh, triThresh);
  unsigned long start, end, initial;
  unsigned long time = 0;
  int samples;
  unsigned int biBuffer[125];
  unsigned int triBuffer[125];
  double bisumOfSquares;
  double trisumOfSquares;
  unsigned long labelStartTime = millis(); // initializes the time that labels are defined 
  unsigned long labelDuration = 125; // 1/4 second 
  unsigned long switchDuration = 5000; // 5 seconds
  String currentLabel = "unknown"; 

  while (1){
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - labelStartTime;

    if (elapsedTime >= switchDuration) {
      // Switch label every 5 seconds
      labelStartTime = currentTime;
      if (currentLabel == "flexion") {
        currentLabel = "extension";
      } else {
        labelStartTime = millis();
        currentLabel = "flexion";
      }
    }

    bisumOfSquares = 0;
    trisumOfSquares = 0;
    initial = micros();
    for (samples = 0; samples<125; samples ++){
      start = micros();
      biBuffer[samples] = readBi();
      triBuffer[samples] = readTri();
      bisumOfSquares += sq(biBuffer[samples]);
      trisumOfSquares += sq(triBuffer[samples]);
      end = micros();
      delayMicroseconds(timeBudget - (end-start));
    }
    time = micros() - initial;

    // TESTING SAMPLING RATE:
    Serial.println(time);
    Serial.print(F("current sampling rate in Hz: "));
    Serial.println(double(samples*1000000)/double(time));
    
    biRMS = sqrt(bisumOfSquares / samples);
    triRMS = sqrt(trisumOfSquares / samples);
    Serial.print(biRMS);
    Serial.print(",");
    Serial.println(triRMS);
    // Serial.print(triRMS);
    // Serial.print(",");
    // Serial.println(currentLabel);
  }
  
}

