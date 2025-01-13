import keyboard  #*pip install keyboard* in console
import time
import serial #pip install pyserial in console

#Initial values for the variables
a1_vel_cmd = 0
a1_pos_cmd = 55

#Define the minimum and maximum limits
MIN_VEL = -95
MAX_VEL = 95
MIN_POS = 0
MAX_POS = 95


SERIAL_PORT = 'COM4'  # Adjust this to your specific port
BAUD_RATE = 9600  # Might want to experiment with higher baud rates, just make sure Arduino matches

#Custom print function to update the same line in the console
def custom_print(encoder_value, bool1, bool2):
    print(f"A1 Vel Cmd: {int(a1_vel_cmd)} | A1 Pos Cmd: {int(a1_pos_cmd)} Encoder Value: {int(encoder_value)} | Butt1: {bool1} | Butt2: {bool2}")

#Function to change the values based on key presses
def change_command():
    global a1_vel_cmd, a1_pos_cmd

    if keyboard.is_pressed('q'):  #Increment velocity
        a1_pos_cmd = 0
        a1_vel_cmd += 5
        if a1_vel_cmd > MAX_VEL:
            a1_vel_cmd = MAX_VEL
            time.sleep(.5)

    elif keyboard.is_pressed('a'):  #Decrement velocity
        a1_pos_cmd = 0
        a1_vel_cmd -= 5
        if a1_vel_cmd < MIN_VEL:
            a1_vel_cmd = MIN_VEL
            time.sleep(.5)

    elif keyboard.is_pressed('e'):  #Increment position
        a1_vel_cmd = 0
        a1_pos_cmd += 5
        if a1_pos_cmd > MAX_POS:
            a1_pos_cmd = MAX_POS
            time.sleep(.5)

    elif keyboard.is_pressed('d'):  #Decrement position
        a1_vel_cmd = 0
        a1_pos_cmd -= 5
        if a1_pos_cmd < MIN_POS:
            a1_pos_cmd = MIN_POS
            time.sleep(.5)

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

# Main loop
def main():
    print("Press 'q/a' to adjust velocity, 'e/d' to adjust position, or 'p' to exit.")

    # Initialize the serial connection
    try:
        ser = initialize_serial_connection()  # Ensure this function is correctly implemented
        if not ser:
            print("Unable to start due to serial connection failure.")
            return
    except Exception as e:
        print(f"Error initializing serial connection: {e}")
        return

    try:
        while True:
            # Exit the program when 'p' is pressed
            if keyboard.is_pressed('p'):
                print("\nExiting the program.")
                break

            # Adjust commands based on keyboard input
            change_command()
            

            # Read and parse data from the Arduino
            if ser.in_waiting > 0:
                # Read the data as a string
                data = ser.readline().decode('utf-8').strip()
                
                # Parse the received data
                encoder_value, bool1, bool2 = parse_arduino_data(data)
                if encoder_value is not None:
                    custom_print(encoder_value, bool1, bool2)
                else:
                    print("Skipping invalid data.")
    except KeyboardInterrupt:
        print("\nProgram interrupted manually.")
    finally:
        if ser:
            ser.close()
            print("Serial connection closed.")

if __name__ == "__main__":
    main()