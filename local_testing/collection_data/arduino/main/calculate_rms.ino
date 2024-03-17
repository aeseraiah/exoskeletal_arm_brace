// This function 

void calculateRMS() {
  int samples;
  double biRMS, triRMS;
  unsigned long start, end, initial;
  unsigned long time = 0;
  unsigned int biBuffer[125];
  unsigned int triBuffer[125];
  double bisumOfSquares;
  double trisumOfSquares;
  unsigned long length_data_collection = 8; // 8 seconds
  unsigned long num_data_points = length_data_collection * 4; // 8 (seconds) * 4 (RMS values per second) = 32 data points
  int emgIndex = 0;

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
  
  // return biRMS and triRMS instead of storing them in an array. 

  // only store the first number_data_points RMS values (8 seconds of data)
  if (emgIndex < 32) {
      emg_Data[emgIndex].biRMS = biRMS;
      emg_Data[emgIndex].triRMS = triRMS;
      emgIndex++; // Increment the index
  }

}