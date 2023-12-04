import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.svm import SVC
from sklearn.model_selection import GridSearchCV, train_test_split
from sklearn.metrics import classification_report, confusion_matrix 
import pickle


def svm(data_path, filename):

    # load features and labels:
    file = data_path + filename
    labeled_data_df = pd.read_csv(file)

    resting_data_df = labeled_data_df[labeled_data_df['labels']==0]
    flexion_data_df = labeled_data_df[labeled_data_df['labels']==1]
    extension_data_df = labeled_data_df[labeled_data_df['labels']==2]

    resting_labels = resting_data_df['labels'].values
    flexion_labels = flexion_data_df['labels'].values
    extension_labels = extension_data_df['labels'].values

    resting_amplitudes = resting_data_df['emg amplitude'].values
    flexion_amplitudes = flexion_data_df['emg amplitude'].values
    extension_amplitudes = extension_data_df['emg amplitude'].values

    features = np.concatenate((resting_amplitudes, flexion_amplitudes, extension_amplitudes))
    labels = np.concatenate((resting_labels, flexion_labels, extension_labels))

    features = features.reshape(-1, 1)

    X_train, X_test, y_train, y_test = train_test_split(features, labels, test_size = .2, random_state=42)

    # DO PARAM SEARCH
    param_grid = {'C': [0.001], 'gamma': [0.001],'kernel': ['poly'], 'degree': [1]}
    grid = GridSearchCV(SVC(),param_grid,refit=True)
    grid.fit(X_train,y_train)
    print(grid.best_estimator_)
    print("\nscore\n",grid.best_estimator_.score(X_test,y_test))
    grid_predictions = grid.predict(X_test)
    print(confusion_matrix(y_test,grid_predictions))
    print(classification_report(y_test,grid_predictions))

    # SAVE TO PKL FILE
    pikl_fn = 'new_trained_model.sav'
    pickle.dump(grid.best_estimator_, open(pikl_fn, 'wb'))
    print('MODEL SAVED TO PICKLE FILE')

# data_path = '../collection_data/labeling/'
# svm(data_path, filename='emg_data_labeled.csv')