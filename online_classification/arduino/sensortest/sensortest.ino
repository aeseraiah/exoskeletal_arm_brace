#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "EMGFilters.h"

EMGFilters myFilter1;
EMGFilters myFilter2;
//Servo myservo;
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
#define BiSensorInputPin A8   // bicep input pin number
#define TriSensorInputPin A9 // tricep input pin number

void setup() {
  // put your setup code here, to run once:
  myFilter1.init(sampleRate, humFreq, true, true, true);
  myFilter2.init(sampleRate, humFreq, true, true, true);
  timeBudget = 1e6/sampleRate;
  //myservo.attach(9);
  //input pulldown is not declared? Might not exist for this processor
  //pinMode (BiSensorInputPin, INPUT_PULLDOWN);
  //pinMode (TriSensorInputPin, INPUT_PULLDOWN);
  // open serial
  Serial.begin(230400);
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
}
void loop() {
    unsigned int bicep, tricep;
    unsigned long start, end;

    start = micros();
    bicep = readBi();
    tricep = readTri();
    
    Serial.print(bicep);
    Serial.print(",");
    Serial.println(tricep);

    end = micros();
    delayMicroseconds(timeBudget - (end-start));
}