import numpy as np
import matplotlib.pyplot as plt
import scipy
import pandas as pd
import pathlib
# from collection_data.interpolation import create_duration_amplitude_file, interpolate
import mne.filter as filter


# Josiah / Natalie

def Filtering(file_path, graphtitle, sample_rate, filtered_plot = False):
    filter_order = 2
    low_freq = 20
    high_freq = 150

    ############################################################################
    # an anti-hum notch filter to filter out 50HZ or 60HZ power line noise.
    # a lowpass filter to filter out noises above 150HZ.
    # a highpass filter to filter out noises below 20HZ.
    ############################################################################
    
    emg_signal = pd.read_csv(file_path)

    num_samples = len(emg_signal['emg amplitude'])
    times = emg_signal['duration (seconds)']
    amplitudes = emg_signal['emg amplitude']



    sos = scipy.signal.butter(filter_order, high_freq, btype='low', fs=sample_rate, output='sos')
    filtered_amplitudes = scipy.signal.sosfilt(sos, amplitudes)
    sos = scipy.signal.butter(filter_order, low_freq, btype='high', fs=sample_rate, output='sos')
    filtered_amplitudes = scipy.signal.sosfilt(sos, filtered_amplitudes)



    if(filtered_plot == True):
        plt.clf()
        plt.subplot(2,1,1) # 2 rows, 1 column, 1st subplot
        plt.title(f"{graphtitle} Unfiltered")
        plt.plot(times, amplitudes, label = "Unfiltered") #, marker='o')
        plt.xlabel("Time (Seconds)")
        plt.ylabel("Amplitude (mV^2)")
        plt.legend()

        plt.subplot(2,1,2)
        plt.title(f"{graphtitle} Filtered")
        plt.plot(times, filtered_amplitudes, label = "Filtered") #, marker='o')
        plt.xlabel("Time (Seconds)")
        plt.ylabel("Amplitude (mV^2)")
        plt.legend()

        plt.savefig("Unfiltered_Vs_Filtered.png")

        plt.show()


    return(0)
Graph_Title = "Anna's Janky Data"
path = 'amplitude_and_durations_900Hz.csv'

# path = The tile path to the data that you want to filter (I was thinking the data that has already been interpolated/downsampled)
# graphtitle = The name that you want to title the graph
# sample_rate = the sampling rate of the original file
# filtered_plot = True -> plot the before and after the filtering 
#                 False -> plot nothing

Filtering(path, graphtitle = Graph_Title, sample_rate=1000, filtered_plot = True)
