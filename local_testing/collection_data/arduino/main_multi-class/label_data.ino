int labelData() {
  double biRMS, triRMS;
  unsigned long labelStartTime = millis(); // Initializes the time that labels are defined 
  String currentLabel = "flexion"; 
  unsigned long start, end, initial;
  unsigned long time = 0;
  int samples;
  unsigned int biBuffer[125];
  unsigned int triBuffer[125];
  double bisumOfSquares;
  double trisumOfSquares;
  unsigned long switchDuration = 2000; // 2 seconds
  unsigned long length_data_collection = 12; // 8 seconds
  unsigned long num_data_points = length_data_collection * 4;
  int emgIndex = 0;

  while (emgIndex < num_data_points) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - labelStartTime;

    if (elapsedTime >= switchDuration) {
      if (currentLabel == "flexion") {
        currentLabel = "extension";
      } 
      else if (currentLabel == "extension") {
        currentLabel = "rest";
      }
      else {
        currentLabel = "flexion";
      }

      Serial.print("Start ");
      Serial.print(currentLabel);
      Serial.println(" in: ");
      for (int i = 5; i > 0; i--) {
        Serial.println(i);
        delay(1000);
      }

      // Reset the label start time
      labelStartTime = millis();
    }

    // Collect EMG data and calculate RMS values
    bisumOfSquares = 0;
    trisumOfSquares = 0;
    initial = micros();
    for (samples = 0; samples < 125; samples++) {
      start = micros();
      biBuffer[samples] = readBi();
      triBuffer[samples] = readTri();
      bisumOfSquares += sq(biBuffer[samples]);
      trisumOfSquares += sq(triBuffer[samples]);
      end = micros();
      delayMicroseconds(timeBudget - (end - start));
    }
    time = micros() - initial;

    biRMS = sqrt(bisumOfSquares / samples);
    triRMS = sqrt(trisumOfSquares / samples);

    // Store the EMG data and label
    emg_Data[emgIndex].biRMS = biRMS;
    emg_Data[emgIndex].triRMS = triRMS;
    emg_Data[emgIndex].label = currentLabel;
    emgIndex++;

    // Print the RMS values and label
    Serial.print(biRMS);
    Serial.print(",");
    Serial.print(triRMS);
    Serial.print(",");
    Serial.println(currentLabel);
  }

  return;
}
