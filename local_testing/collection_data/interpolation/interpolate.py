# Example testing script for interpoaltion:
import pandas as pd
import scipy
import numpy as np
import matplotlib.pyplot as plt

def interp(data_path, amplitude_and_durations_filename, interp_filename):
    # replace w/ Josiah's and Natalie's code
    sample_rate = 1000  #1000Hz: Sampling rate of EMG board

    path = data_path + amplitude_and_durations_filename

    emg_signal = pd.read_csv(path)

    num_samples = len(emg_signal['emg amplitude'])
    times = emg_signal['duration (seconds)']
    amplitudes = emg_signal['emg amplitude']

    # This is assuming every second has the same number of samples
    # og_sampling_rate = 872.3629525939913 Hz
    og_sampling_rate = num_samples / (times[(len(times)-1)] - times[0]) 

    time_1000_Hz = np.arange(times[0], times[(len(times)-1)], 1/sample_rate)
    interpolator = scipy.interpolate.interp1d(times, amplitudes, kind='linear', )
    amplitudes_1000_Hz = interpolator(time_1000_Hz)

    new_df = pd.DataFrame({'duration (seconds)': time_1000_Hz, 'emg amplitude': amplitudes_1000_Hz})


    new_df.to_csv(interp_filename, index=False)

    plt.subplot(2,1,1) # 2 rows, 1 column, 1st subplot
    plt.title("Uninterpolated")
    plt.plot(times, amplitudes, label = "Uninterpolated", marker='o')
    plt.xlabel("Time (Milliseconds)")
    plt.ylabel("Amplitude (mV^2)")
    plt.legend()

    plt.subplot(2,1,2)
    plt.title("Interpolated")
    plt.plot(time_1000_Hz, amplitudes_1000_Hz, label = "Interpolated", marker='o', color='green')
    plt.xlabel("Time (Milliseconds)")
    plt.ylabel("Amplitude (mV^2)")
    plt.legend()

    plt.tight_layout()

    plt.savefig('Uninterpolated_Vs_Interpolated.jpeg')

#interp(data_path='./collection_data/interpolation/', amplitude_and_durations_filename='amplitude_and_durations_900Hz.csv', interp_filename='fake_interpolation_data')