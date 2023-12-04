import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


def data_labeling(interpolated_filename, labeled_filename):

    data_df = pd.read_csv(interpolated_filename, index_col=False)

    total_num_samples = len(data_df)

    ###############################
    # if sampling rate is known:
    sampling_rate = 1000
    time_of_file_sec = total_num_samples / sampling_rate
    time_of_file_min = time_of_file_sec / 60
    ###############################

    # plt.plot(data_df.index, data_df['emg amplitude'])
    # plt.show()

    # Append labels as the third column to emg file:

    num_samples_for_30sec_rest = sampling_rate * 30
    num_samples_for_5sec_intervals = sampling_rate * 5

    rest_labels = np.full(num_samples_for_30sec_rest, 0)
    flexion_labels = np.full(num_samples_for_5sec_intervals, 1)
    extension_labels = np.full(num_samples_for_5sec_intervals, 2)

    # Create an empty column for labels
    data_df['labels'] = np.nan

    # Handle the first 30 seconds as rest
    data_df.loc[:num_samples_for_30sec_rest-1, 'labels'] = rest_labels[:num_samples_for_30sec_rest]

    for i in range(num_samples_for_30sec_rest, total_num_samples - num_samples_for_30sec_rest, num_samples_for_5sec_intervals * 2):
        data_df.loc[i:i + num_samples_for_5sec_intervals-1, 'labels'] = flexion_labels
        data_df.loc[i + num_samples_for_5sec_intervals:i + 2 * num_samples_for_5sec_intervals-1, 'labels'] = extension_labels

    # Handle the last 30 seconds as rest
    data_df.loc[total_num_samples - num_samples_for_30sec_rest:, 'labels'] = rest_labels[:num_samples_for_30sec_rest]

    # Fill NaN values in the labels column with the last label
    data_df['labels'].fillna(method='ffill', inplace=True)

    # Convert labels column to integer type
    data_df['labels'] = data_df['labels'].astype(int)

    data_df.to_csv(f'./collection_data/labeling/{labeled_filename}.csv', index=False)

# data_labeling(interpolated_filename='../interpolation/fake_interpolation_data.csv', labeled_filename='test1')
