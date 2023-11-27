# THIS FILE PROVIDES A DESCRIPTION OF EACH PYTHON SCRIPT

Input files needed to run generateIndexes script:
- # MARKER_FILE:
   # Ex filename: './data/14_57_50_04_19_23.csv' 
   # Description: 
    # col1 = labels, col2 = time stamps

- # data_file:
    # Ex filename: './data/CoolTerm Capture 2023-04-19 14-57-55.txt' 
    # Description: 
     # col1 = time stamps, col2 = amplitudes


Output file from generateIndexes script:
- # excel_file
    # Ex filename: './data/filenames-indexes.xlsx'
    # Description: 
     # contains several sheets, each sheet representing a single coolterm file in '/data' folder. col1 = coolterm file name, col2 = amplitude vals for flexion, col3 = ampltitude vals for extension, ect. (may not need it, as long as you put in 'w' mode instead of 'a'?)

Input files needed to run realTimeMarkers script:
- # None

Output files:
- # MARKER_FILE:
    # Ex filename: './data/14_57_50_04_19_23.csv' 
    # Description:
     # produces the csv file that labels the data. This file is used by generateIndexes script --> col 1 = label, col2 = time stamps (H:M:S)


    