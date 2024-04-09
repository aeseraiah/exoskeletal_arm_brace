import datetime
import time
import csv
from getkey import getkey, keys
from msvcrt import getch

def generate_emg_data(file_name, downsample_rate):

    with open(file_name, 'wb') as fn:
        ...

    with open(file_name, 'a') as file:
        # try:
        while True:
            emg_str_value = 'digital value = 350 analog value = 1.80'
            space_sep_strs = emg_str_value.split()
            digital_emg_value = space_sep_strs[3]
            analog_emg_value = space_sep_strs[7]
            current_time = time.strftime("%H:%M:%S", time.localtime())
            line = f"{current_time}\t{digital_emg_value}\t{analog_emg_value}\t{emg_str_value}\t\n"
            file.write(line)


            key = getkey()
            if key == 'x':
                print('done')
                # fn.close()
                exit()
            else:
                checkKey(key, 'temp_filename.txt')

            # Sleep for the specified downsample_rate
            time.sleep(1 / downsample_rate)
        

        # except KeyboardInterrupt:
        #     pass  # Handle Ctrl+C to stop the loop


def checkKey(key, file_name):
    with open(file_name, 'wb') as fn:
        ...

    with open(file_name, 'a') as file:
        csv_write = csv.writer(file)
    
    if key == 'f':
        print(f'{time.strftime("%H:%M:%S", time.localtime())} FLEXION RECORDED')
        csv_write.writerow([0, time.strftime("%H:%M:%S", time.localtime())])

# Example usage
# NOTE: Although down sample rate is 39 Hz, the data is actually being sampeld and stored at 30 Hz (find out why later)
generate_emg_data("emg_data.txt", 39)
