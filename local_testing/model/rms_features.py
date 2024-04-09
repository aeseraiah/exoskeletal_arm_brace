import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score

# Function to calculate RMS over a moving window
def calculate_rms(signal, window_size):
    rms_values = []
    for i in range(len(signal) - window_size + 1):
        window_data = signal[i:i + window_size]
        rms = np.sqrt(np.mean(np.square(window_data)))
        rms_values.append(rms)
    return rms_values

# Generate synthetic data for demonstration
np.random.seed(42)
emg_signals = np.random.randn(1000)  # Replace with your actual EMG data
labels = np.random.choice([0, 1], size=len(emg_signals))

# Choose a window size for RMS calculation
window_size = 10

# Calculate RMS values for each window
rms_features = calculate_rms(emg_signals, window_size)

# Reshape RMS features for compatibility with scikit-learn
X = np.array(rms_features).reshape(-1, 1)
y = labels

# Split the data into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Create and train a logistic regression classifier
classifier = LogisticRegression()
classifier.fit(X_train, y_train)

# Make predictions on the test set
y_pred = classifier.predict(X_test)

# Evaluate the accuracy of the classifier
accuracy = accuracy_score(y_test, y_pred)
print("Accuracy:", accuracy)
