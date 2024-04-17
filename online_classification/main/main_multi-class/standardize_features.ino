  
// float standardize(double bicep_rms, double tricep_rms) {
//     ////////// STANDARDIZE RMS ////////////////
//     // Calculate biMean and triMean
//     double biMean, triMean;
//     biMean = bisumOfSquares / samples;
//     triMean = trisumOfSquares / samples;

//     // Calculate biStdDev and triStdDev
//     double biSum = 0;
//     double triSum = 0;
//     for (samples = 0; samples < 125; samples++) {
//       biSum += sq(biBuffer[samples] - biMean);
//       triSum += sq(triBuffer[samples] - triMean);
//     }

//     double biStdDev;
//     double triStdDev;
//     biStdDev = sqrt(biSum / samples);
//     triStdDev = sqrt(triSum / samples);

//     double biRMS_standardized;
//     double triRMS_standardized;
//     biRMS_standardized = (biRMS - biMean) / biStdDev;    
//     triRMS_standardized = (triRMS - triMean) / triStdDev;   
//     ////////////////////////////////////////////
