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
  Serial.begin(2000000);
}

unsigned long readBi() {
    unsigned long biValue, biDataAfterFilter, bienvlope;
    biValue = analogRead(BiSensorInputPin);
    //biDataAfterFilter = myFilter.update(biValue);
    //bienvlope = sq(biDataAfterFilter);
    return(biValue);
}

unsigned long readTri() {
    unsigned long triValue, triDataAfterFilter, trienvlope;
    triValue = analogRead(TriSensorInputPin);
    //triDataAfterFilter = myFilter.update(triValue);
    //trienvlope = sq(triDataAfterFilter);
    return(triValue);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long start = millis();
  unsigned int bi = 0;
  unsigned int tri = 0;
  unsigned long time = 0;
  int samples = 0;
  while(1){
    bi = readBi();
    tri = readTri();
    time = millis() - start;
    Serial.print(F("Time since beginning in milliseconds: "));
    Serial.println(time);
    Serial.print(F("Bicep Sensor Value = "));
    Serial.println(bi);
    Serial.print(F("Tricep Sensor Value = "));
    Serial.println(tri);
    samples += 1;
    Serial.print(F("current sampling rate in Hz: "));
    Serial.println(float(samples/(time/1000)));
    //delay(100);
  }
}
