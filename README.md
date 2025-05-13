# Machine Learning-Controlled Exoskeletal Arm Brace

## Project Overview

This project implements a machine learning pipeline on a Teensy 4.0 microcontroller to control an exoskeletal arm brace. It achieves this by detecting muscle activity (Electromyography - EMG) on the user's opposite, control arm. The workflow involves real-time EMG data collection, preprocessing, RMS feature extraction, classification and actuation. 

The user-specific trained Feedforward Neural Network classifies the intended movement, prompting a servo motor to actuate the exoskeletal arm brace. The system prioritizes minimal delay by **achieving a total system latency of 250 milliseconds and a 100% classification accuracy with multiple demo users**. An OLED display provides user feedback and instructions throughout the process.

## Features

* **Contralateral Control:** Controls the exoskeletal arm brace on one arm based on muscle activity detected on the opposite arm
* **Low Latency:** Designed for minimal delay between intended movement detection and brace actuation for real-time control
* **EMG Signal Acquisition:** Reads raw analog EMG data from bicep and tricep sensors worn on the control arm
* **Signal Preprocessing:** Utilizes the [EMGFilters](https://github.com/oymotion/EMGFilters) library for filtering raw EMG signals
* **Feature Extraction:** Calculates RMS using the filtered EMG values over short windows as the primary feature forclassification
* **On-Device Machine Learning:**
    * Implements a data collection and labeling process for training a machine learning model
    * Utilizes the [AIfES for Arduino](https://github.com/Fraunhofer-IMS/AIfES_for_Arduino) library for implementing a Feedforward Neural Network (FNN) on the MCU
    * Includes functions for building (`build_AIfES_model`) and training (`train_AIfES_model`) the model using user-specific data
    * Makes real-time predictions (`model_predictions`) on new EMG data to classify intended movements (flexion/extension)
* **Servo Actuation:** Controls a servo motor to move the exoskeletal arm brace based on the classified movement. The `actuateServo` function implements a basic speed control profile, slowing down near the target angle
* **OLED Display Output:** Provides instructions, sensor values, training progress, and prediction results to the user via an OLED display
* **Sensor Confirmation:** Includes a calibration phase (`confirmSensors`) to ensure good sensor contact and establish baseline EMG activity during relaxation, providing feedback to the user via the OLED display
* **Adaptive Training:** The system checks the accuracy of the trained model and triggers retraining with new data collection if the accuracy is below an 85% threshold