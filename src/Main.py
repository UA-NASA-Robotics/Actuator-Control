import serial
from actuator_command import *


SERIAL_PORT = 'COM4'  # Adjust this to your specific port
BAUD_RATE = 115200 # Might want to experiment with higher baud rates, just make sure Arduino matches

# Function to initialize the serial connection
def initialize_serial_connection():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Serial connection established on {SERIAL_PORT} at {BAUD_RATE} baud.")
        return ser
    except serial.SerialException as e:
        print(f"Failed to connect to serial port {SERIAL_PORT}: {e}")
        return None

# Parse the data received from the Arduino.
# Expects data in the format: <encoder value, upper button status, lower button status>.
def parse_arduino_data(data):
    try:
        parsed = data.strip('<>\n\r').split(',')
        if len(parsed) == 6:  # Ensure we have the correct number of values
            encoder_value = int(parsed[0])  # First value is the number
            bool1 = bool(int(parsed[1]) == 1)  # Convert '1' to True, '0' to False
            bool2 = bool(int(parsed[2]) == 1)
            bool3 = bool(int(parsed[3]) == 1)
            homed_encoder_value = int(parsed[4])
            actuator_one_length = int(parsed[5])
            return encoder_value, bool1, bool2, bool3, homed_encoder_value, actuator_one_length
        else:
            print("Malformed data:", data)
            return None, None, None, None, None, None
    except ValueError as e:
        print("Error parsing data:", e)
        return None, None, None, None, None, None
    
# Function to send velocity and position commands to Arduino with <>
def send_commands_to_arduino(ser, vel_cmd, pos_cmd):
    command_string = f"<{vel_cmd},{pos_cmd}>\n"
    ser.write(command_string.encode('latin-1'))

# Main loop
def main():

    # Initialize the serial connectione
    try:
        print("Attempting connection")
        ser = initialize_serial_connection()  # Ensure this function is correctly implemented
        if not ser:
            print("Unable to start due to serial connection failure.")
            return
    except Exception as e:
        print(f"Error initializing serial connection: {e}")
        return

    try:
        while True:
            # Adjust commands based on keyboard input
            #print("Begining of Loop")
            a1_vel_cmd, a1_pos_cmd = change_command()

            # Send these commands to Arduino
            #print("sending Commands")
            send_commands_to_arduino(ser, a1_vel_cmd, a1_pos_cmd)
            
            #breaking here

            # Read and parse data from the Arduino
            #print("Reading Adruino")
            if ser.in_waiting > 0:
                # Read the data as a string
                data = ser.readline().decode('latin-1').strip()
                
                #print("parsing")
                # Parse the received data
                encoder_value, bool1, bool2, bool3, homed_encoder_value, actuator_one_length = parse_arduino_data(data)
                if encoder_value is not None:
                    print(f"Vel Cmd: {a1_vel_cmd}, Pstn Cmd: {a1_pos_cmd}, Ecr Val: {encoder_value}, Btn1: {bool1}, Btn2: {bool2}, Homed: {bool3}, HMD Ecr Val: {homed_encoder_value}, Test: {actuator_one_length}")
                else:
                    print("Skipping invalid data.")
    except KeyboardInterrupt:
        print("\nProgram interrupted manually.")
    finally:
        if ser:
            ser.close()
            print("Sererial connection closed.")

if __name__ == "__main__":
    main()