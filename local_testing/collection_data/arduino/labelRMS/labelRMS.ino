// https://github.com/Fraunhofer-IMS/AIfES_for_Arduino
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "EMGFilters.h"
#include <aifes.h> 

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

struct EMGData {
  double biRMS;
  double triRMS;
  String label;
};

struct EMGData_for_predictions {
  double biRMS;
  double triRMS;
};

// struct used to store accuracy and the trained model from model_training function:
struct ModelResult {
    float accuracy;
    aimodel_t trained_model;
};

// Define a global array to store the first 32 values of biRMS, triRMS, and label
const int num_data_points = 32;
EMGData emg_trainingData[num_data_points]; // stores real-time emg data used to train model
EMGData_for_predictions emg_predictingData[num_data_points]; // stores real-time emg data used to make predictions
int emgIndex = 0; // Index to keep track of the number of values stored


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
//4. make activation decision based on RMS (ML)
//5. activate servo in based on activation response (keep track of current location and slow the movement as it is close to the endpoints)
//6. enter loop again

double calculateRMS(unsigned int buffer[], int size) {
  double sumOfSquares = 0.0;
  for (int i = 0; i < size; i++) {
    sumOfSquares += sq(buffer[i]);
  }
  return sqrt(sumOfSquares / size);
}

//////////////////////////////////////////////////////////////     

ModelResult model_training(EMGData data[32], unsigned int number_data_points) {
  while (!Serial);
  //AIfES requires random weights for training
  //Here the random seed is generated by the noise of an analog pin
  srand(analogRead(A5));
  uint32_t i;

  double biArray[number_data_points];
  double triArray[number_data_points];
  String labelArray[number_data_points];
  float num_labelArray[number_data_points];

  for (int i = 0; i < number_data_points; ++i) {
      // Store the first column values (biRMS) in biArray
      biArray[i] = data[i].biRMS;
      triArray[i] = data[i].triRMS;
      labelArray[i] = data[i].label;
      //Serial.println(labelArray[i]); // check to see if number of labels per second make sense. It should be 4/sec
  }

  for (int i = 0; i < number_data_points; ++i) {
    if (labelArray[i] == "flexion") {
      num_labelArray[i] = 1.0f;
    }
    else { 
      num_labelArray[i] = 0.0f;
    }
    //Serial.println(num_labelArray[i]);
  }
  
  // Tensor for the training data
  uint16_t input_shape[] = {number_data_points, 2};    // Definition of the input shape

  //////////////////////////////////////////////////////////////////////
  // float input_data[number_data_points*2];

  // // Populate input_data array with biArray and triArray values
  // for (int i = 0; i < number_data_points; ++i) {
  //     input_data[i * 2] = biArray[i];
  //     input_data[i * 2 + 1] = triArray[i];
  // }

  float input_data[number_data_points*2] = {
    33.05f,32.89f,
    87.00f,77.51f,
    94.27f,81.90f,
    96.67f,78.30f,
    78.28f,61.53f,
    95.32f,76.87f,
    78.22f,53.66f,
    92.20f,75.80f,
    15.47f,21.14f,
    53.86f,57.77f,
    70.11f,75.33f,
    66.01f,75.66f,
    33.61f,48.81f,
    82.46f,85.92f,
    49.10f,62.33f,
    42.28f,48.70f,
    33.05f,32.89f,
    87.00f,77.51f,
    94.27f,81.90f,
    96.67f,78.30f,
    78.28f,61.53f,
    95.32f,76.87f,
    78.22f,53.66f,
    92.20f,75.80f,
    15.47f,21.14f,
    53.86f,57.77f,
    70.11f,75.33f,
    66.01f,75.66f,
    33.61f,48.81f,
    82.46f,85.92f,
    49.10f,62.33f,
    42.28f,48.70f
  };

  aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data);       // Creation of the input AIfES tensor with two dimensions and data type F32 (float32)
  // Tensor for the target data
  uint16_t target_shape[] = {number_data_points, 1};     // Definition of the input shape

  //////////////////////////////////////////////////////////////////////
  // Populate input_data array with biArray and triArray values
  //float target_data[number_data_points*1];

  // for (int i = 0; i < number_data_points; ++i) {
  //     target_data[i] = num_labelArray[i];
  // }

  float target_data[number_data_points*1] = {
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f
  };

  aitensor_t target_tensor = AITENSOR_2D_F32(target_shape, target_data); // Assign the target_data array to the tensor. It expects a pointer to the array where the data is stored
  // Empty tensor for the output data (result after training). 
  // Same configuration as for the target tensor
  uint16_t output_shape[] = {number_data_points, 1};
  float output_data[number_data_points*1];
  aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);
  
  // ---------------------------------- Layer definition ---------------------------------------
  uint16_t input_layer_shape[] = {1, 2};          // Definition of the input layer shape (Must fit to the input tensor)
  ailayer_input_f32_t   input_layer     = AILAYER_INPUT_F32_A( /*input dimension=*/ 2, /*input shape=*/ input_layer_shape);   // Creation of the AIfES input layer
  ailayer_dense_f32_t   dense_layer_1   = AILAYER_DENSE_F32_A( /*neurons=*/ 40); // Creation of the AIfES hidden dense layer with 3 neurons
  ailayer_sigmoid_f32_t sigmoid_layer_1 = AILAYER_SIGMOID_F32_A(); // Hidden activation function
  ailayer_dense_f32_t   dense_layer_2   = AILAYER_DENSE_F32_A( /*neurons=*/ 1); // Creation of the AIfES output dense layer with 1 neuron
  ailayer_sigmoid_f32_t sigmoid_layer_2 = AILAYER_SIGMOID_F32_A(); // Output activation function
  
  ailoss_crossentropy_t crossentropy_loss;                         //Loss: mean squared error
  
  // --------------------------- Define the structure of the model ----------------------------
  
  aimodel_t model;  // AIfES model
  ailayer_t *x;     // Layer object from AIfES to connect the layers
  
  // Connect the layers to an AIfES model
  model.input_layer = ailayer_input_f32_default(&input_layer);
  x = ailayer_dense_f32_default(&dense_layer_1, model.input_layer);
  x = ailayer_sigmoid_f32_default(&sigmoid_layer_1, x);
  x = ailayer_dense_f32_default(&dense_layer_2, x);
  x = ailayer_sigmoid_f32_default(&sigmoid_layer_2, x);
  model.output_layer = x;
  
  // Add the loss to the AIfES model
  // model.loss = ailoss_mse_f32_default(&mse_loss, model.output_layer);
  model.loss = ailoss_crossentropy_f32_default(&crossentropy_loss, model.output_layer);
  aialgo_compile_model(&model); // Compile the AIfES model
  
  // ------------------------------------- Print the model structure ------------------------------------
  
  Serial.println(F("-------------- Model structure ---------------"));
  Serial.println(F("Layer:"));
  aialgo_print_model_structure(&model);
  Serial.print(F("\nLoss: "));
  aialgo_print_loss_specs(model.loss);
  Serial.println(F("\n----------------------------------------------\n"));
  
  // ------------------------------- Allocate memory for the parameters of the model ------------------------------
  
  uint32_t parameter_memory_size = aialgo_sizeof_parameter_memory(&model);
  Serial.print(F("Required memory for parameter (Weights, Biases): "));
  Serial.print(parameter_memory_size);
  Serial.print(F(" bytes"));
  Serial.println();
  byte *parameter_memory = (byte *) malloc(parameter_memory_size);
  
  // Distribute the memory for the trainable parameters of the model
  aialgo_distribute_parameter_memory(&model, parameter_memory, parameter_memory_size);
  
  // ------------------------------- Initialize the parameters ------------------------------
  
  // Recommended weight initialization
  aimath_f32_default_init_glorot_uniform(&dense_layer_1.weights);
  aimath_f32_default_init_zeros(&dense_layer_1.bias);
  
  aimath_f32_default_init_glorot_uniform(&dense_layer_2.weights);
  aimath_f32_default_init_zeros(&dense_layer_2.bias);
  
  // -------------------------------- Define the optimizer for training ---------------------
  
  // Adam optimizer
  aiopti_adam_f32_t adam_opti = AIOPTI_ADAM_F32(/*learning rate=*/ 0.001f, /*beta_1=*/ 0.9f, /*beta_2=*/ 0.999f, /*eps=*/ 1e-7);
  aiopti_t *optimizer = aiopti_adam_f32_default(&adam_opti); // Initialize the optimizer
    
  // -------------------------------- Allocate and schedule the working memory for training ---------
  
  uint32_t memory_size = aialgo_sizeof_training_memory(&model, optimizer);
  Serial.print(F("Required memory for the training (Intermediate results, gradients, optimization memory): "));
  Serial.print(memory_size);
  Serial.print(F(" bytes"));
  Serial.println();
  Serial.print(parameter_memory_size+memory_size);
  Serial.print(F(" total bytes required"));
  Serial.println();
  byte *memory_ptr = (byte *) malloc(memory_size);

  if(memory_ptr == 0){
    Serial.println(F("ERROR: Not enough memory (RAM) available for training! Try to use another optimizer (e.g. SGD) or make your net smaller."));
    while(1);
  }
  
  // Schedule the memory over the model
  aialgo_schedule_training_memory(&model, optimizer, memory_ptr, memory_size);
  
  // IMPORTANT: Initialize the AIfES model before training
  aialgo_init_model_for_training(&model, optimizer);
  
  uint32_t input_counter = 0;  // Counter to print the inputs/training data

  // ------------------------------------- Training configuration ------------------------------------
  
  uint32_t batch_size = 1; // Configuration tip: ADAM=4   / SGD=1
  uint16_t epochs = 100;   // Configuration tip: ADAM=100 / SGD=550
  uint16_t print_interval = 5;
  
  Serial.println(F("\n------------ Training configuration ----------"));
  Serial.print(F("Epochs: "));
  Serial.print(epochs);
  Serial.print(F(" (Print loss every "));
  Serial.print(print_interval);
  Serial.println(F(" epochs)"));
  Serial.print(F("Batch size: "));
  Serial.println(batch_size);
  Serial.print(F("Optimizer: "));
  aialgo_print_optimizer_specs(optimizer);
  Serial.println(F("\n----------------------------------------------\n"));
  
  // ------------------------------------- Run the training ------------------------------------
  
  Serial.println(F("Start training"));
  float loss;
  for(i = 0; i < epochs; i++)
  {
    // One epoch of training. Iterates through the whole data once
    aialgo_train_model(&model, &input_tensor, &target_tensor, optimizer, batch_size);
  
    // Calculate and print loss every print_interval epochs
    if(i % print_interval == 0)
    {
      aialgo_calc_loss_model_f32(&model, &input_tensor, &target_tensor, &loss);
      Serial.print(F("Epoch: "));
      Serial.print(i);
      Serial.print(F(" Loss: "));
      Serial.println(loss);
      
    }
  }
  Serial.println(F("Finished training"));
  
  // ----------------------------------------- Evaluate the trained model --------------------------
  
  // Do the inference after training
  aialgo_inference_model(&model, &input_tensor, &output_tensor);
  
  Serial.println(F(""));
  Serial.println(F("After training:"));
  Serial.println(F("Results:"));
  Serial.println(F("input 1:\tinput 2:\treal output:\tcalculated output:\tpredicted label:"));
  
  input_counter = 0;
  float predicted_labels[number_data_points*1];
  int correct_predictions = 0;
  int total_predictions = number_data_points;
  
  for (i = 0; i < number_data_points; i++) {
    Serial.print(input_data[input_counter]);
    //Serial.print(((float* ) input_tensor.data)[i]); //Alternative print for the tensor
    input_counter++;
    Serial.print(F("\t\t"));
    Serial.print(input_data[input_counter]);
    input_counter++;
    Serial.print(F("\t\t"));
    Serial.print(target_data[i]);
    Serial.print(F("\t\t"));
    Serial.print(output_data[i]);
    if (output_data[i] > 0.5) {
      // ACTUATE SERVO:
      // myservo.write(180);
      predicted_labels[i] = 1;
    } 
    else {
      // myservo.write(0);
      predicted_labels[i] = 0;
    }
    Serial.print(F("\t\t\t"));
    Serial.println(predicted_labels[i]);

    if (predicted_labels[i] == target_data[i]) {
      correct_predictions++;
    }

    //Serial.println(((float* ) output_tensor.data)[i]); //Alternative print for the tensor
  }

  float accuracy = (float)correct_predictions / total_predictions * 100;
  Serial.print("Accuracy: ");
  Serial.print(accuracy);
  Serial.println("%");
  
  free(parameter_memory);
  free(memory_ptr);

  ModelResult result;
  result.accuracy = accuracy;
  memcpy(&result.trained_model, &model, sizeof(aimodel_t));

  return result;

}
//////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// MODEL INFERENCE (MAKE REAL-TIME PREDICTIONS USING TRAINED MODEL):
float make_predictions(EMGData_for_predictions data[32], unsigned int number_data_points, aimodel_t trained_model) {
  while (!Serial);
  //AIfES requires random weights for training
  //Here the random seed is generated by the noise of an analog pin
  srand(analogRead(A5));
  uint32_t i;

  double biArray[number_data_points];
  double triArray[number_data_points];

  for (int i = 0; i < number_data_points; ++i) {
      // Store the first column values (biRMS) in biArray
      biArray[i] = data[i].biRMS;
      triArray[i] = data[i].triRMS;
  }

  uint16_t input_shape[] = {number_data_points, 2}; 
  uint16_t output_shape[] = {number_data_points, 1};

  float input_data[number_data_points*2];
  float output_data[number_data_points*1];
  // Populate input_data array with biArray and triArray values
  for (int i = 0; i < number_data_points; ++i) {
      input_data[i * 2] = biArray[i];
      input_data[i * 2 + 1] = triArray[i];
  }

  aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data); 
  aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);

  aialgo_inference_model(&trained_model, &input_tensor, &output_tensor);
  
  Serial.println(F(""));
  Serial.println(F("After training:"));
  Serial.println(F("Results:"));
  Serial.println(F("input 1:\tinput 2:\tcalculated output:"));
  
  uint32_t input_counter = 0; 
  
  for (i = 0; i < 4; i++) {
    Serial.print (input_data[input_counter]);
    //Serial.print(((float* ) input_tensor.data)[i]); //Alternative print for the tensor
    input_counter++;
    Serial.print(F("\t\t"));
    Serial.print(input_data[input_counter]);
    input_counter++;
    Serial.print(F("\t\t"));
    Serial.println(output_data[i]);
    //Serial.println(((float* ) output_tensor.data)[i]); //Alternative print for the tensor
  }
}

void loop() {
  // put your main code here, to run repeatedly:
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
  unsigned long labelDuration = 125; // 1/4 second 
  unsigned long switchDuration = 2000; // 2 seconds
  unsigned long labelDuration_model_training = 4000; // collect and label data for 4 seconds, then start model training
  bool should_break_void_loop = false; 

  String currentLabel = "unknown"; 
  // String currentLabel = "flexion";
  String newLabel = "extension";

  while (1){
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - labelStartTime;

    if (currentLabel == "unknown") {
      newLabel = "flexion";
    }
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

    // TESTING SAMPLING RATE:
    // Serial.println(time);
    // Serial.print(F("current sampling rate in Hz: "));
    // Serial.println(double(samples*1000000)/double(time));
    
    biRMS = sqrt(bisumOfSquares / samples);
    triRMS = sqrt(trisumOfSquares / samples);

    // only store the first number_data_points RMS values (8 seconds of data)
    if (emgIndex < 32) {
        emg_trainingData[emgIndex].biRMS = biRMS;
        emg_trainingData[emgIndex].triRMS = triRMS;
        emg_trainingData[emgIndex].label = currentLabel;
        emgIndex++; // Increment the index
    }

    Serial.print(biRMS);
    Serial.print(",");
    Serial.print(triRMS);
    Serial.print(",");
    Serial.println(currentLabel);

    unsigned long time = millis();
    unsigned long elapsedtime = time - StartTime;

    if (elapsedtime >= labelDuration_model_training) {
      if (emgIndex == num_data_points) {
        Serial.println("Relax arm. Model training will now begin");
        // delay(5000);
        time = millis();
        StartTime = time;
        //float accuracy = model_training(emg_trainingData, num_data_points); 
        ModelResult training_result = model_training(emg_trainingData, num_data_points);
        float accuracy = training_result.accuracy;
        aimodel_t trained_model = training_result.trained_model;
        
        // if model accuracy is above 85%, reak out of loop and continue to actuation of servo:
        if (accuracy > 85) {
          Serial.println("Model accuracy is above 85%. Continue to actuation.");
          make_predictions(emg_predictingData, num_data_points, trained_model);
          should_break_void_loop = true;
          break;
        }
        else {
          // modify this else statement to go back to section of code that collects data and retrain model with new data 
          Serial.println("Model is below 85% accuracy. It should be retrained with new data");
        }
      }

    }
  }

  if (should_break_void_loop == true) {
    // replace below code with actuation of servo
    Serial.println("PROGRAM EXITED");
    exit(0);
  }
}