import keyboard  #Import the keyboard module (*pip install keyboard* in console)
import time

#Initial values for the variables
a1_vel_cmd = 0
a1_pos_cmd = 55

#Define the minimum and maximum limits
MIN_VEL = -95
MAX_VEL = 95
MIN_POS = 0
MAX_POS = 95

#Custom print function to update the same line in the console
def custom_print():
    print(f"A1 Vel Cmd: {int(a1_vel_cmd)} | A1 Pos Cmd: {int(a1_pos_cmd)}")

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

# Main loop
print("Press 'q/a' to adjust velocity, 'e/d' to adjust position, or 'p' to exit.")
while True:
    if keyboard.is_pressed('p'):  #Exit the program when 'p' is pressed
        print("\nExiting the program.")
        break

    change_command()
    custom_print()
    time.sleep(0.1) #Done so CPU doesn't fry