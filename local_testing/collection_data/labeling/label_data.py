import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

data_dir = '../data/'


def data_labeling(emg_filename):
    columns = ['emg amplitudes']
    data_df = pd.read_csv(data_dir + emg_filename, names=columns, index_col=False)

    num_samples = len(data_df)

    ###############################
    # if sampling rate is unknown but tduration of file is known:
    time_of_file = 195 # 3 minutes
    sampling_rate = num_samples/time_of_file
    
    ###############################

    ###############################
    # if sampling rate is known:
    # sampling_rate = 1000
    # time_of_file_sec = num_samples/sampling_rate
    # time_of_file_min = time_of_file_sec/60
    ###############################

    print(data_df['emg amplitudes'])
    plt.plot(data_df.index, data_df['emg amplitudes'])
    plt.show()

    data_df['labels'] = np.arange(0,len(data_df))

    ################################################

    # Append labels as the second column to emg file:
    # label_arr = []

    # num_samples_for_30sec_rest = sampling_rate * 30
    # num_samples_for_5sec_intervals = sampling_rate * 5

    # rest_labels = np.full(num_samples_for_30sec_rest, 0)
    # flexion_labels = np.full(num_samples_for_5sec_intervals, 1)
    # extension_labels = np.full(num_samples_for_5sec_intervals, 2)

    # data_df.iloc[:num_samples_for_30sec_rest, data_df.columns.get_loc('labels')] = rest_labels
    
    # data_df.iloc[num_samples_for_30sec_rest+2:num_samples_for_30sec_rest+num_samples_for_5sec_intervals] = flexion_labels
    print(data_df.head)


    # label_arr.append()

    # for index, row in data_df.iterrows():
    #     data_df.at[index, 'labels'] = index

    data_df.to_csv('emg_data_labeled.csv', index=False)




time_of_file_min = data_labeling(emg_filename='Grant_Bicep_Take3')
# Isaac Bicep 3min protocol
