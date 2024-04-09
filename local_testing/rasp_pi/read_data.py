import serial

# PORT = '/dev/ttyUSB0'
PORT = 'COM12'
# Replace 'COM10' with your COM port name and adjust other settings as needed
ser = serial.Serial(PORT, baudrate=230400, timeout=1)

while True:
    # Read a line of serial data
    # data = ser.readline().decode().strip()
    data = ser.readline().decode('latin1').strip()
    
    # Print the received data
    print(data)