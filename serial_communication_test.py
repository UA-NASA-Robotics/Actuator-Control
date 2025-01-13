import serial

SERIAL_PORT = 'COM4'  # Adjust this to your specific port
BAUD_RATE = 9600  # Match this with your Arduino's baud rate

def initialize_serial_connection():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Serial connection established on {SERIAL_PORT} at {BAUD_RATE} baud.")
        return ser
    except serial.SerialException as e:
        print(f"Failed to connect to serial port {SERIAL_PORT}: {e}")
        return None

def parse_arduino_data(data):
    try:
        parsed = data.strip('<>\n\r').split(',')
        if len(parsed) == 3:  # Ensure we have the correct number of values
            encoder_value = int(parsed[0])  # First value is the number
            bool1 = bool(int(parsed[1]) == 0)  # Convert '1' to True, '0' to False
            bool2 = bool(int(parsed[2]) == 0)
            return encoder_value, bool1, bool2
        else:
            print("Malformed data:", data)
            return None, None, None
    except ValueError as e:
        print("Error parsing data:", e)
        return None, None, None

def main():
    ser = initialize_serial_connection()
    if not ser:
        return  # If the serial connection fails, stop the program

    # Read data from the Arduino
    while True:
        if ser.in_waiting > 0:
            # Read the data as a string
            data = ser.readline().decode('utf-8').strip()
            
            # Parse the received data
            encoder_value, bool1, bool2 = parse_arduino_data(data)
            
            # If parsing was successful, print the received values
            if encoder_value is not None:
                print(f"Encoder value: {encoder_value}, Boolean 1: {bool1}, Boolean 2: {bool2}")

if __name__ == "__main__":
    main()
