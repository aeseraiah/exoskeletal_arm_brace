#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "EMGFilters.h"

#define BiSensorInputPin A0   // bicep input pin number
#define TriSensorInputPin A1  // tricep input pin number

EMGFilters myFilter1, myFilter2;
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
  timeBudget = 1e6 / sampleRate;
  // open serial
  Serial.begin(230400);
}

unsigned long readBi() {
    unsigned long biValue, biDataAfterFilter, bienvlope;
    biValue = analogRead(BiSensorInputPin);
    biDataAfterFilter = myFilter1.update(biValue);
    bienvlope = sq(biDataAfterFilter);
    return(bienvlope);
}

unsigned long readTri() {
    unsigned long triValue, triDataAfterFilter, trienvlope;
    triValue = analogRead(TriSensorInputPin);
    triDataAfterFilter = myFilter2.update(triValue);
    trienvlope = sq(triDataAfterFilter);
    return(trienvlope);
}


void loop() {
  // put your main code here, to run repeatedly:
  
  int bienvlope;
  int trienvlope;

  int samples = 0;
  unsigned long time, initial, start, end;
  unsigned int bi[250];
  unsigned int tri[250];
  initial = micros();
  while(samples < 250){
    //read in and filter bicep sensor data
    start = micros();
    bi[samples] = readBi();
    //delayMicroseconds(50);
    tri[samples] = readTri();
    //Serial.println(F("Bicep Time: Tricep Time:  Total Time:"));
    //Serial.print(bicepTime - start);
    //Serial.print(F("  "));
    //Serial.print(tricepTime - bicepTime);
    //Serial.print(F("  "));
    //Serial.println(total - start);
    //Serial.print(F("Time since beginning in milliseconds: "));
    //Serial.println(time);
    //Serial.print(F("Bicep Sensor Value = "));
    //Serial.println(bi[samples]);
    //Serial.print(F("Tricep Sensor Value = "));
    //Serial.println(tri[samples]);
    samples += 1;
    end = micros();
    delayMicroseconds(timeBudget - (end-start));
    
    //Serial.print(F("current sampling rate in Hz: "));
    //Serial.println(float(samples/(time/1000)));
    //delay(100);
  }
  Serial.println(timeBudget);
  time = micros() - initial;
  Serial.println(time);
  
  Serial.print(F("current sampling rate in Hz: "));
  Serial.println(double(samples*1000000)/double(time));
  delay(2000);
}
