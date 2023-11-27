import datetime
import random

def generate_emg_data(file_name, downsample_rate):
    with open(file_name, 'w') as file:
        for _ in range(downsample_rate):
            current_time = datetime.datetime.now().strftime("%H:%M:%S")
            emg_str_value = 'digital value = 350 analog value = 1.80'
            space_sep_strs = emg_str_value.split()
            digital_emg_value = space_sep_strs[3]
            analog_emg_value = space_sep_strs[7]
            line = f"{current_time}\t{digital_emg_value}\t{analog_emg_value}\t{emg_str_value}\t\n"
            file.write(line)
# Example usage
generate_emg_data("emg_data.txt", 10)
