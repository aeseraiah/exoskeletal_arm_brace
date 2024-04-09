import os
import csv

folder = '.'

for log_file in os.listdir(folder):
    if log_file.endswith('.log'):
        log_file_path = os.path.join(folder, log_file)
        output_file = os.path.splitext(log_file)[0] + '.csv'

        with open(log_file_path, 'r') as infile, open(output_file, 'w', newline='') as outfile:
            reader = csv.reader(infile)
            writer = csv.writer(outfile)

            for row in reader:
                # Process and write each row as needed
                writer.writerow(row)
