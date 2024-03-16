/*
  www.aifes.ai
  Copyright (C) 2020-2021 Fraunhofer Institute for Microelectronic Circuits and Systems. All rights reserved.

  AIfES is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// This file covers the prediction of the current object

void color_detection() {
  // This function is responsible for getting a RGB sample and use the trained model to detect the corresponding object
  // It displays the probability for each object/class

  int r, g, b, c, p;
  float sum;

  // ----------------------------------------- Definition of tensors --------------------------
  // Tensor for the input RGB
  // Those values are the input of the ANN
  float input_data[3];                                                  // Array for storage of the RGB input data
  uint16_t input_shape[] = {1, 3};                                      // Definition of the shape of the tensor, here: {1 (i.e. 1 sample), 3 (i.e. the sample contains 3 RGB values)}
  aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data);   // Creation of the input AIfES tensor

  // Tensor for the output with 3 classes
  // Output values of the ANN are saved here
  float output_data[3];                                                   // Array for storage of the output data, for each object/class one output is created
  uint16_t output_shape[] = {1, 3};                                       // Definition of the shape of the tensor, here: {1 (i.e. 1 sample), 3 (i.e. the sample contains predictions for 3 classes/objects)}
  aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);  // Creation of the output AIfES tensor

  // Calculating the proportions of the individual colours in the total signal. They are normalized into a range from 0 to 1.
  input_data[0] = 0;
  input_data[1] = 0;
  input_data[2] = 0;

  // ----------------------------------------- Run the AIfES model to detect the object --------------------------
  // Run the inference with the trained AIfES model
  // The function needs the trained model, the input_tensor with the input data and the output_tensor where the results are saved in the corresponding array
  aialgo_inference_model(&model, &input_tensor, &output_tensor);

  // ----------------------------------------- Output of results --------------------------
  Serial.print (F("Object 1: "));
  Serial.print (output_data[0] * 100); // Probability in percent for class 0
  Serial.print (F("%"));
  Serial.println(F(""));
  Serial.print (F("Object 2: "));
  Serial.print (output_data[1] * 100); // Probability in percent for class 1
  Serial.print (F("%"));
  Serial.println(F(""));
  Serial.print (F("Object 3: "));
  Serial.print (output_data[2] * 100); // Probability in percent for class 2
  Serial.print (F("%"));
  Serial.println(F(""));
  Serial.println(F(""));

  // Reduce the output speed to be able to read the result of the detection comfortably
  //delay(1000);

}
