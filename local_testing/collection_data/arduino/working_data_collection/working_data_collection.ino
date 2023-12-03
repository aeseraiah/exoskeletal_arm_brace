#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "EMGFilters.h"
#include <Servo.h>
#define TIMING_DEBUG 1
#define SensorInputPin A0 // input pin number

EMGFilters myFilter;
int sampleRate = SAMPLE_FREQ_1000HZ;
int humFreq = NOTCH_FREQ_60HZ;

const int emgPin = A0;  // Analog pin to which the EMG sensor is connected
int emgValue = 0;      // Variable to store the EMG sensor reading
unsigned long previousMicros = 0;
const int samplingInterval = 1000;  // Set your desired sampling interval in microseconds (1 millisecond for 1000 Hz)
static int Threshold = 0;

unsigned long timeStamp;
unsigned long timeBudget;

void setup() {
  myFilter.init(sampleRate, humFreq, true, true, true);
  Serial.begin(115200);   // Adjusted baud rate

  timeBudget = 1e6 / sampleRate;
}

void loop() {
  unsigned long currentMicros = micros();
  
  if (currentMicros - previousMicros >= samplingInterval) {
    emgValue = analogRead(emgPin);  // Read the analog value from the EMG sensor

    int DataAfterFilter = myFilter.update(emgValue);

    int envlope = sq(DataAfterFilter);
    envlope = (envlope > Threshold) ? envlope : 0;

    // Serial.println(emgValue);       // Print the EMG value to the serial monitor
    Serial.printf("Time in microseconds%d\n:", micros());
    // Serial.println(micros());
    Serial.println(envlope);       // Print the EMG value to the serial monitor
    previousMicros = currentMicros;
  }
}


