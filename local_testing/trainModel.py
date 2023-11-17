from sklearn_rvm.em_rvm import EMRVC
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from micromlgen import port
from sklearn.svm import SVC
import warnings
from sklearn.model_selection import GridSearchCV
from sklearn.metrics import classification_report, confusion_matrix 
import pickle

#
#  SPECIFY NUMBER OF TRAINING DATA FILES #################################
#

TRAINING_DATA_COUNT = 6
window_size = 100

excel_file = "./data/filenames-indexes.xlsx"
filenames = []

# GET EACH FILENAME AND CORRESPONDING INDEXES
flexion_startindexes = []
sustain_startindexes = []
extension_startindexes = []
rest_startindexes = []
for sheet in range(0, TRAINING_DATA_COUNT):
    curr_sheet = pd.read_excel(excel_file, sheet)
    filenames.append(curr_sheet['FILENAMES'][0])
    flexion_startindexes.append(((curr_sheet['FLEXION'].dropna()).astype(int)).tolist())
    sustain_startindexes.append(((curr_sheet['SUSTAIN'].dropna()).astype(int)).tolist())
    extension_startindexes.append(((curr_sheet['EXTENSION'].dropna()).astype(int)).tolist())
    rest_startindexes.append(((curr_sheet['REST'].dropna()).astype(int)).tolist())

print(filenames)
# READ IN DATA FROM EACH FILE
data = []
for i,filename in enumerate(filenames):
    data.append(pd.read_csv(filename, delimiter='\t', usecols=[1])) # COL 1 is NON TIME COL
# 
#           DATA PREPROCESSING #################################
# 

# TURN DATA INTO NUMPY ARRAY AND SMOOTH WITH EMA
np_data = []
alpha = 0.1
expected = 0
actual = 0
for curr_array in data:
    d = curr_array.iloc[:,0].to_numpy()
    emg_data_ema = []
    for pt in d:
        actual = alpha * pt + (1 - alpha) * actual
        emg_data_ema += [actual]
    np_data.append(emg_data_ema)


# ENSURE DATA SEPARATED
for npd in np_data:
    plt.plot(npd)
    plt.show()
plt.title("ALL TRAINING SIGNALS")
# plt.show()

# 
# SHOW THE SEPARATION BETWEEN FEATURES #################################
# IS NOT SHOWING THE EMA FILTERED WAVES
#

for index_index in range(0, TRAINING_DATA_COUNT):
    for i in extension_startindexes[index_index]:
        plt.plot(data[index_index].iloc[:,0][ int(i) : int(i) + window_size], color = 'green')
    for i in flexion_startindexes[index_index]:
        plt.plot(data[index_index].iloc[:,0][ int(i) : int(i) + window_size], color = 'red')
    for i in sustain_startindexes[index_index]:
        plt.plot(data[index_index].iloc[:,0][ int(i) : int(i) + window_size], color = 'purple')
    for i in rest_startindexes[index_index]:
        plt.plot(data[index_index].iloc[:,0][ int(i) : int(i) + window_size], color = 'orange')
    plt.title("FLEXION - RED , EXTENSION - GREEN, SUSTAIN - PURPLE, REST - ORANGE")
    plt.show()


# GENERATE FEATURE ARRAYS
FLEXION_DATA = []
EXTENSION_DATA = []
SUSTAIN_DATA = []
REST_DATA = []
for index_index in range(0, TRAINING_DATA_COUNT):
    for i in flexion_startindexes[index_index]:
        FLEXION_DATA.append(np_data[index_index][ i : i + window_size])

    for i in extension_startindexes[index_index]:
        EXTENSION_DATA.append(np_data[index_index][ i : i + window_size])

    for i in sustain_startindexes[index_index]:
        SUSTAIN_DATA.append(np_data[index_index][ i : i + window_size])

    for i in rest_startindexes[index_index]:
        REST_DATA.append(np_data[index_index][ i : i + window_size])

# classifiers
flexion = 0
extension = 1
sustain = 2
rest = 3

# SPLIT DATA INTO TRAINING/TESTING
FLEXION_STOP_TRAIN = (8*len(FLEXION_DATA))//10
EXTENSION_STOP_TRAIN = (8*len(EXTENSION_DATA))//10
SUSTAIN_STOP_TRAIN = (8*len(SUSTAIN_DATA))//10
REST_STOP_TRAIN = (8*len(REST_DATA))//10

# print(STOP_TRAIN)
# features = FLEXION_DATA[0:FLEXION_STOP_TRAIN] + EXTENSION_DATA[0:EXTENSION_STOP_TRAIN] + SUSTAIN_DATA[0:SUSTAIN_STOP_TRAIN]
features = FLEXION_DATA[0:FLEXION_STOP_TRAIN] + EXTENSION_DATA[0:EXTENSION_STOP_TRAIN] + SUSTAIN_DATA[0:SUSTAIN_STOP_TRAIN] + REST_DATA[0:REST_STOP_TRAIN]

# FILL LABEL ARRAY
LABEL = []
for i in range(0, FLEXION_STOP_TRAIN):
    LABEL.append(flexion)

for i in range(0, EXTENSION_STOP_TRAIN):
    LABEL.append(extension)

for i in range(0, SUSTAIN_STOP_TRAIN):
    LABEL.append(sustain)

for i in range(0, REST_STOP_TRAIN):
    LABEL.append(sustain)

#
#
# INIT MODEL AND FIT FEATURES/LABELS #################################
#
#


# FILL PREDICITON DATA ARRAY AND ANSWER ARRAY
# X = FLEXION_DATA[FLEXION_STOP_TRAIN:] + EXTENSION_DATA[EXTENSION_STOP_TRAIN:] + SUSTAIN_DATA[SUSTAIN_STOP_TRAIN:]
X = FLEXION_DATA[FLEXION_STOP_TRAIN:] + EXTENSION_DATA[EXTENSION_STOP_TRAIN:] + SUSTAIN_DATA[SUSTAIN_STOP_TRAIN:] + REST_DATA[REST_STOP_TRAIN:] 

answ = []
for i in range(0, len(FLEXION_DATA[FLEXION_STOP_TRAIN:])):
    answ.append(flexion)

for i in range(0, len(EXTENSION_DATA[EXTENSION_STOP_TRAIN:])):
    answ.append(extension)

for i in range(0, len(SUSTAIN_DATA[SUSTAIN_STOP_TRAIN:])):
    answ.append(sustain)

for i in range(0, len(REST_DATA[REST_STOP_TRAIN:])):
    answ.append(sustain)

#
#
# # MAKE PREDICTION #################################
# prediction = model.predict(X)

warnings.filterwarnings("ignore")

# DO PARAM SEARCH
param_grid = {'C': [10,0.1,0.001,1], 'gamma': [0.001,0.01,0.1,1],'kernel': ['linear','poly'], 'degree': [1,2,3]}
grid = GridSearchCV(SVC(),param_grid,refit=True)
grid.fit(features,LABEL)
print(grid.best_estimator_)
print("\nscore\n",grid.best_estimator_.score(X,answ))
grid_predictions = grid.predict(X)
print(confusion_matrix(answ,grid_predictions))
print(classification_report(answ,grid_predictions))

# c_code = port(grid.best_estimator_)
# with open("/Users/nikol/Desktop/main/MotionClassifier.h",'w') as f:
#     f.write(c_code)
# print(c_code)

# SAVE TO PKL FILE
pikl_fn = 'trained_model.sav'
pickle.dump(grid.best_estimator_, open(pikl_fn, 'wb'))
print('MODEL SAVED TO PICKLE FILE')
