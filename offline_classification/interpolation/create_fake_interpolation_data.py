import pandas as pd
import numpy as np

### This file creates a fake interpolated file that will be used to test labeling script ###
time_of_file_secs = 180
sampling_rate = 1000
num_samples = time_of_file_secs * sampling_rate
df = pd.DataFrame({'emg amplitude': np.ones(num_samples)})

df.to_csv('fake_interpolation_data.csv', index=False)