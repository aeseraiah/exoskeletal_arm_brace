import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import tensorflow as tf
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVC
from sklearn.cluster import KMeans
from sklearn.model_selection import GridSearchCV, train_test_split
from sklearn.metrics import classification_report, confusion_matrix 
import pickle
import os
import pandas as pd
from sklearn.cluster import KMeans
import matplotlib.pyplot as plt

# data_path = '../data/'
data_path = './small_data_files/'
# Read flexion data from CSV
subject_name = 'josiah_isaac'
# flex_data_csv = pd.read_csv(data_path + f'flexion_{subject_name}.csv', header=None)
flex_data_csv = pd.read_csv(data_path + f'{subject_name}_flex_combined.csv', header=None)

# Read extension data from CSV
# ext_data_csv = pd.read_csv(data_path + f'extension_{subject_name}.csv', header=None)
ext_data_csv = pd.read_csv(data_path + f'{subject_name}_ext_combined.csv', header=None)

def plot_data(flex_data, ext_data):
    flex_data.loc[:,2] = 'flexion'
    ext_data.loc[:,2] = 'extension'
    flex_data['Color'] = 'red'  
    ext_data['Color'] = 'blue'

    data = pd.concat([flex_data, ext_data], axis=0)
    print(data)

    # Scatter plot for flexion data
    plt.scatter(flex_data.iloc[:, 0], flex_data.iloc[:, 1], c=flex_data['Color'], label='Flexion', marker='o', edgecolors='k')

    # Scatter plot for extension data
    plt.scatter(ext_data.iloc[:, 0], ext_data.iloc[:, 1], c=ext_data['Color'], label='Extension', marker='o', edgecolors='k')

    plt.xlabel('RMS - Bicep')
    plt.ylabel('RMS - Tricep')
    plt.legend()
    plt.savefig(f"{subject_name}_rms_data.png")
    plt.show()

plot_data(flex_data_csv, ext_data_csv)

# def simple_Kmeans(flex_data, ext_data):
    
#     combined_data = pd.concat([flex_data, ext_data], axis=0)
#     combined_data = combined_data[:,:-1]

#         # Hyperparameters
#     num_clusters = 2  # You can adjust this based on your understanding of the data
#     kmeans = KMeans(n_clusters=num_clusters, init='random', n_init=1, random_state=42)

#     # Fit the model
#     kmeans.fit(combined_data)

#     # Get the cluster assignments for each data point
#     assignments = kmeans.labels_

#     # Plot the data points with different colors for each cluster
#     colors = ['b', 'r', 'g', 'y', 'c', 'm', 'k']
#     for i in range(len(combined_data)):
#         plt.scatter(combined_data.iloc[i, 0], combined_data.iloc[i, 1], c=colors[assignments[i]])

#     # Plot the cluster centers
#     cluster_centers = kmeans.cluster_centers_
#     for i, center in enumerate(cluster_centers):
#         plt.scatter(center[0], center[1], marker='x', s=200, c='k', label=f'Cluster {i + 1}')

# def simple_NN(flex_data, ext_data):

#     combined_data = pd.concat([flex_data, ext_data], axis=0)
#     # combined_data = combined_data.iloc[:, 0].values.reshape(-1, 1)
#     combined_data = combined_data.iloc[:, :-2]
#     print(combined_data)
#     all_labels = np.concatenate([np.zeros(flex_data.shape[0]), np.ones(ext_data.shape[0])])

#     X_train, X_test, y_train, y_test = train_test_split(combined_data, all_labels, test_size=0.2, random_state=42)

#     # Standardize the data (optional but often recommended)
#     scaler = StandardScaler()
#     X_train_scaled = scaler.fit_transform(X_train)
#     X_test_scaled = scaler.transform(X_test)

#     model = tf.keras.Sequential()
#     model.add(tf.keras.layers.Dense(64, activation='relu')) # relu is used for performance
#     model.add(tf.keras.layers.Dense(32, activation='relu')) # relu is used for performance
#     model.add(tf.keras.layers.Dense(16, activation='relu')) # relu is used for performance
#     model.add(tf.keras.layers.Dense(1, activation='sigmoid')) # softmax is used, because we only expect one gesture to occur per input
#     model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])
#     history = model.fit(X_train_scaled, y_train, epochs=50, batch_size=1, validation_split=0.2)

#     # Evaluate the model on the test set
#     accuracy = model.evaluate(X_test_scaled, y_test)[1]
#     print(f"Test Accuracy: {accuracy}")

#     return model

# NN_model = simple_NN(flex_data_csv, ext_data_csv)

# # Convert the model to the TensorFlow Lite format without quantization
# converter = tf.lite.TFLiteConverter.from_keras_model(NN_model)
# tflite_model = converter.convert()

# # Save the model to disk
# open("josiah_isaac_NN_model.tflite", "wb").write(tflite_model)
  
##############################################
## The purpose of creating the header file is to embed the TensorFlow Lite model into an Arduino sketch for deployment
## The header file encapsulates the model's binary data in a format that can be easily included in an Arduino sketch.
##############################################

# # Generate the header file for Arduino
# header_content = """
# const unsigned char model[] = {
# """
# header_content += ", ".join([str(byte) for byte in tflite_model])
# header_content += "\n};"

# with open(f"./{subject_name}_model.h", "w") as header_file:
#     header_file.write(header_content)

# model_h_size = os.path.getsize(f"./{subject_name}_model.h")
# print(f"Header file, model.h, is {model_h_size:,} bytes.")

# def svm(labeled_data_df):

#     # resting_data_df = labeled_data_df[labeled_data_df['labels']==0]
#     flexion_data_df = labeled_data_df[labeled_data_df['labels']==1]
#     extension_data_df = labeled_data_df[labeled_data_df['labels']==2]

#     # resting_labels = resting_data_df['labels'].values
#     flexion_labels = flexion_data_df['labels'].values
#     extension_labels = extension_data_df['labels'].values

#     # resting_amplitudes = resting_data_df['emg amplitude'].values
#     flexion_amplitudes = flexion_data_df['emg amplitude'].values
#     extension_amplitudes = extension_data_df['emg amplitude'].values

#     # features = np.concatenate((resting_amplitudes, flexion_amplitudes, extension_amplitudes))
#     # labels = np.concatenate((resting_labels, flexion_labels, extension_labels))

#     features = np.concatenate((flexion_amplitudes, extension_amplitudes))
#     labels = np.concatenate((flexion_labels, extension_labels))

#     features = features.reshape(-1, 1)

#     X_train, X_test, y_train, y_test = train_test_split(features, labels, test_size = .2, random_state=42)

#     # DO PARAM SEARCH
#     param_grid = {'C': [10,0.1,0.001,1], 'gamma': [0.001,0.01,0.1,1],'kernel': ['linear','poly'], 'degree': [1,2,3]}
#     grid = GridSearchCV(SVC(),param_grid,refit=True)
#     history = grid.fit(X_train,y_train)
#     print(grid.best_estimator_)
#     print("\nscore\n",grid.best_estimator_.score(X_test,y_test))
#     grid_predictions = grid.predict(X_test)
#     print(confusion_matrix(y_test,grid_predictions))
#     print(classification_report(y_test,grid_predictions))

#     # SAVE TO PKL FILE
#     pikl_fn = 'new_trained_model.sav'
#     pickle.dump(grid.best_estimator_, open(pikl_fn, 'wb'))
#     print('MODEL SAVED TO PICKLE FILE')

# data_path = '../collection_data/labeling/'
# svm(data_path, labeled_filename='labeled_test_data.csv')