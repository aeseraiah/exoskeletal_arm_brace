// This function creates real labels for the training data
// Data is labeled for 8 seconds, with flexion and extension switching every 2 seconds 
// User is provided countdown for more accurate labeling. Countdown should be moved to OLED display.

int labelData() {
  double biRMS, triRMS;
  // unsigned int biThresh, triThresh;
  // confirmSensors(biThresh, triThresh);
  unsigned long start, end, initial;
  unsigned long time = 0;
  int samples;
  unsigned int biBuffer[125];
  unsigned int triBuffer[125];
  double bisumOfSquares;
  double trisumOfSquares;
  unsigned long labelStartTime = millis(); // initializes the time that labels are defined 
  unsigned long StartTime = millis(); // initializes the time that labels are defined, but it kept separate for model training
  unsigned long switchDuration = 2000; // 2 seconds
  unsigned long length_data_collection = 8; // 8 seconds
  unsigned long num_data_points = length_data_collection * 4; // 8 (seconds) * 4 (RMS values per second) = 32 data points
  int emgIndex = 0; // Index to keep track of the number of RMS values stored

  String currentLabel = "flexion"; 
  String newLabel = "extension";

  while (emgIndex < num_data_points) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - labelStartTime;
    
    if (currentLabel == "flexion") {
      newLabel = "extension";
    }
    if (currentLabel == "extension") {
      newLabel = "flexion";
    }

    if (elapsedTime >= switchDuration) {
      Serial.print("Start ");
      Serial.print(newLabel);
      Serial.println(" in: ");
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
      // Switch label every 2 seconds
      currentTime = millis();
      labelStartTime = currentTime;
      if (currentLabel == "flexion") {
        currentLabel = "extension";
      } 
      else {
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
    
    biRMS = sqrt(bisumOfSquares / samples);
    triRMS = sqrt(trisumOfSquares / samples);

    // only store the first number_data_points RMS values (8 seconds of data)
    emg_Data[emgIndex].biRMS = biRMS;
    emg_Data[emgIndex].triRMS = triRMS;
    emgIndex++; // Increment the index

    Serial.print(biRMS);
    Serial.print(",");
    Serial.print(triRMS);
    Serial.print(",");
    Serial.println(currentLabel);

  }

  return;

}