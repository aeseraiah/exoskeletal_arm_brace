import pandas as pd

file_name = 'Josiah_CoolTerm Capture 2024-02-06 10-43-06.txt'
# file_name = 'ISAACCoolTerm Capture 2024-02-06 10-31-42.txt'
df = pd.read_csv(file_name, sep='\t',header=None)
subject_name = 'Josiah'

found_state = False
counter = 0

# Define a dictionary to store values for each state
values_dict = {}

# Define the states to check
states_to_check = ['flexion', 'extension', 'half flexion']

# Initialize lists for each state
for state in states_to_check:
    values_dict[state] = []

# Iterate through rows
for index, row in df.iterrows():
    # Check if the value is one of the states we're interested in
    if row[0] in states_to_check:
        # Set the flag to True
        found_state = True
        # Define a counter to collect the next 150 rows
        counter = 0
        state = row[0]  # Get the current state
        continue  # Skip this row since we don't want to include the state in the list
    
    # If the state has been found and we're within the next 150 rows
    if found_state and counter < 1000 and state in states_to_check:
        # Collect values from the next 150 rows
        values_dict[state].append(row.tolist())  # Append the entire row to the corresponding state's list
        counter += 1  # Increment the counter
    
    # If we've collected 150 rows after the state, reset the flag and counter
    if counter == 1000 and state in states_to_check:
        found_state = False
        counter = 0

# Convert each list to a DataFrame if needed
result_dfs = {}
for state, values_list in values_dict.items():
    result_dfs[state] = pd.DataFrame(values_list)

# Display the resulting DataFrames
for state, df in result_dfs.items():
    print(f"State: {state}")
    if df.empty:
        # if dataframe empty, skip and move on
        continue
    else:
        # if dataframe not empty, split single column into two columns (col1 = bicep data, col2 = tricep data)
        split_values = df[0].str.split(',', 1, expand=True)
        df[[0, 1]] = split_values
        df.to_csv(f'{state}_{subject_name}.csv', header=None, index=None)
        print(state)

