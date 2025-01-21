import keyboard

# Define velocity and position steps
VELOCITY_STEPS = [-1, 1]
POSITION_STEPS = [0, 25, 50, 75, 100]

def change_command():
    key_to_velocity = {
        't': VELOCITY_STEPS[1],
        'g': VELOCITY_STEPS[0],
    }

    key_to_position = {
        '1': POSITION_STEPS[0],
        '2': POSITION_STEPS[1],
        '3': POSITION_STEPS[2],
        '4': POSITION_STEPS[3],
        '5': POSITION_STEPS[4],
    }

    velocity = 0  # Default velocity
    position = 0  # Default position

    # Check for velocity key presses
    for key, vel in key_to_velocity.items():
        if keyboard.is_pressed(key):
            velocity = vel
            position = None  # Reset position to 0 when changing velocity
            return velocity, position

        # Check for position key presses
    for key, pos in key_to_position.items():
        if keyboard.is_pressed(key):
            position = pos
            velocity = None  # Reset velocity to 0 when changing position
            return velocity, position
        
    return None, None