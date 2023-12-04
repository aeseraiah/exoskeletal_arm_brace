from model import svm_model
from collection_data.interpolation import create_duration_amplitude_file, interpolate
from collection_data.labeling import label_data

# Main script does the following (assumes you only have the 3 minute coolterm file):
# 1. Uses coolterm file to generate a csv file that includes the duration (in seconds) from the time the arduino script started (from start of data collection). Also includes recorded emg values
# 2. Uses this csv file to interpolate the data (this ensures data is at a fixed sampling rate) and creates a new csv file containing emg values interpolated at 1000 Hz 
# 3. Uses interpolated csv file to label data as resting, flexion, or extension
# 4. Uses labeled data file to split up data, convert data into features and labels, and feed into model

### 1: 
# replace coolterm_filename with name of 3 minute coolterm file
# replace new_filename with the prefered name of the amplitude and durations file
create_duration_amplitude_file.create_duration_file(data_path='./data/', coolterm_filename='coolterm_samplingrate_900Hz.txt', new_filename='amplitude_and_durations_900Hz')
# 'new_filename'' will be located in 'local_testing/collection_data/interpolation'

### 2: 
# replace amplitude_and_durations_filename with name of the 'new_filename' used in calling 'create_duration_file' function
# replace interp_filename with the prefered name of the interpolated data file
interpolate.interp(data_path='./collection_data/interpolation/', amplitude_and_durations_filename='amplitude_and_durations_900Hz.csv', interp_filename='fake_interpolation_data')
# 'interp_filename' file will be located in 'local_testing/collection_data/interpolation'

### 3: 
label_data.data_labeling(data_path='./collection_data/interpolation/', interpolated_filename='fake_interpolation_data.csv', labeled_filename='amplitude_and_durations_900Hz_labeled')
# 'labeled_filename' will be located in 'local_testing/collection_data/labeling'

### 4: 
# svm_model.svm(data_path='./collection_data/labeling/', labeled_filename='emg_data_labeled.csv')


