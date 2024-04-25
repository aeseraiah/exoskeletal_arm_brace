// This function 

void calculateRMS(double& biRMS, double& triRMS, unsigned int biThreshold, unsigned int triThreshold) {
  int samples;
  unsigned long start, end, initial;
  unsigned long time = 0;
  unsigned int biBuffer[125];
  unsigned int triBuffer[125];
  double bisumOfSquares;
  double trisumOfSquares;

  bisumOfSquares = 0;
  trisumOfSquares = 0;
  initial = micros();
  for (samples = 0; samples<125; samples ++){
    start = micros();
    biBuffer[samples] = readBi();
    biBuffer[samples] = (biBuffer[samples] > biThreshold) ? biBuffer[samples] : 0;
    triBuffer[samples] = readTri();
    triBuffer[samples] = (triBuffer[samples] > triThreshold) ? triBuffer[samples] : 0;
    bisumOfSquares += sq(biBuffer[samples]);
    trisumOfSquares += sq(triBuffer[samples]);
    end = micros();
    delayMicroseconds(timeBudget - (end-start));
  }
  time = micros() - initial;
  
  biRMS = sqrt(bisumOfSquares / samples);
  triRMS = sqrt(trisumOfSquares / samples);

}