import numpy as np
import pandas as pd

def create_duration_file(filename):
    df = pd.read_csv(filename, names=['timestamps', 'amplitudes and duration'], sep='\t')

    duration_array = []
    amplitude_array = []
    next_value_flag = False

    next_value_flag = 0

    for val in df['amplitudes and duration']:
        if next_value_flag == 1:
            duration_array.append(val)
            next_value_flag = 2
        elif next_value_flag == 2:
            amplitude_array.append(val)
            next_value_flag = 0
        elif val == 'Time in microseconds:':
            next_value_flag = 1

    # CHECK TO MAKE SURE LIST OF DURATIONS IN 'duration_array' MAKES SENSE:
    # print(duration_array)

    # converting list of durations from strings to integers:
    int_list = [int(x) for x in duration_array]

    # converting durations from microseconds to seconds:
    int_list_secs = [(x/1000000) for x in int_list]
    # print(int_list_secs)

    # column 1 = the duration at which the emg value was recorded, column 2 = emg value
    new_df = pd.DataFrame({'duration (seconds)': int_list_secs, 'emg amplitude': amplitude_array})

    new_df.to_csv('amplitude_and_durations.csv', index=False)


create_duration_file('../data/coolterm_samplingrate_01.txt')