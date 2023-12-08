import numpy as np
import pandas as pd

def create_duration_file(data_path, coolterm_filename, new_filename):
    df = pd.read_csv(data_path + coolterm_filename, names=['amplitudes and duration'], sep='\t')

    duration_array = []
    amplitude_array = []

    for i in range(0, len(df), 2):
        duration_array.append(df['amplitudes and duration'][i])
        amplitude_array.append(df['amplitudes and duration'][i+1])

    # CHECK TO MAKE SURE LIST OF DURATIONS IN 'duration_array' MAKES SENSE:
    # print(duration_array)

    # converting durations from microseconds to seconds:
    duration_arr_secs = [(x/1000000) for x in duration_array]

    # column 1 = the duration at which the emg value was recorded, column 2 = emg value
    new_df = pd.DataFrame({'duration (seconds)': duration_arr_secs, 'emg amplitude': amplitude_array})

    # new_df.to_csv(f'./collection_data/interpolation/{new_filename}.csv', index=False)
    new_df.to_csv(f'{new_filename}.csv', index=False)
    print(new_df)


create_duration_file(data_path='../../data/',coolterm_filename='removed_samples_Isaac_3min_protocol.txt', new_filename='test')