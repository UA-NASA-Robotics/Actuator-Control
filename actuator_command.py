import keyboard

# Define velocity and position steps
VELOCITY_STEPS = [0, 25, 50, 75, 100]
NEGATIVE_VELOCITY_STEPS = [-x for x in VELOCITY_STEPS]
POSITION_STEPS = [0, 25, 50, 75, 100]

def change_command():
    key_to_velocity = {
        'q': VELOCITY_STEPS[0],
        'w': VELOCITY_STEPS[1],
        'e': VELOCITY_STEPS[2],
        'r': VELOCITY_STEPS[3],
        't': VELOCITY_STEPS[4],
        'a': NEGATIVE_VELOCITY_STEPS[0],
        's': NEGATIVE_VELOCITY_STEPS[1],
        'd': NEGATIVE_VELOCITY_STEPS[2],
        'f': NEGATIVE_VELOCITY_STEPS[3],
        'g': NEGATIVE_VELOCITY_STEPS[4],
    }

    key_to_position = {
        'z': POSITION_STEPS[0],
        'x': POSITION_STEPS[1],
        'c': POSITION_STEPS[2],
        'v': POSITION_STEPS[3],
        'b': POSITION_STEPS[4],
    }

    velocity = 0  # Default velocity
    position = 0  # Default position

    while True:
        # Check for velocity key presses
        for key, vel in key_to_velocity.items():
            if keyboard.is_pressed(key):
                velocity = vel
                position = 0  # Reset position to 0 when changing velocity
                return velocity, position

        # Check for position key presses
        for key, pos in key_to_position.items():
            if keyboard.is_pressed(key):
                position = pos
                velocity = 0  # Reset velocity to 0 when changing position
                return velocity, position
