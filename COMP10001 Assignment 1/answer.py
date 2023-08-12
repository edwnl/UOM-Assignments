def first_lock(word_list):
    '''
    Solves the first question of Project 1.
    :param word_list: List of strings containing the words.
    :returns: Sorted list of strings, containing the keys
    which appear the most frequenty.
    '''

    gems = {}  # Create empty dictionary to store frequencies.

    for word in word_list:  # For each item in the list provided:

        # Find the item's frequency from the dictionary, 0 if not found.
        # Once found, increment the frequency by 1.

        gems[word] = gems.get(word, 0) + 1

    # Get the maximum frequency from the dictionary

    max_freq = max(gems.values())

    # Return keys with the most frequent value in dictionary.

    return sorted([key for (key, value) in gems.items() if value == max_freq])

def second_lock(source):
    '''
    Solves the second question in Project 1.
    :param source: List of ints containing the gems.
    :returns: Tuple (If the question is possible, max number of gems stored)
    '''

    store = []  # Array to store gems.
    most_stored = 0  # Variable for largest amount of gems stored.

    # Attempt to find every number in order, from 1 to the len of source.

    for target in range(1, len(source) + 1):
        found = False  # Temp variable for the while loop.

        while not found:

            # Get the last element of the 'source' and 'store' arrays.

            (source_e, store_e) = (last_e(source), last_e(store))

            # If elements from 'source' and 'store' are both not the target:

            if source_e != target and store_e != target:

                if len(source) < 2:

                    # 'Source' has less than 2 elements, no solution.

                    return (False, -1)
                else:

                    # Store the 'source' element in 'store'.

                    store.append(source.pop())

                    # Update the most_stored variable.

                    most_stored = max(most_stored, len(store))
            else:

                # Else, if the target is found in either source or store...

                found = True  # Escape the while loop.

                # Remove the target from either source or store array.

                (source.pop() if source_e == target else store.pop())

    return (True, most_stored)


def last_e(arr):
    '''
    Returns the last element of a list.
    :param arr: List
    :returns: Last element of list, -1 if empty.
    '''

    return (arr[-1] if arr else 0)

def third_lock(key, stone):
    '''
    Solves the third question in Project 1.
    :param key: 2D String array containing the key.
    :param stone: 2D String array containing the key.
    :returns: Tuple (Y-Displacement, X-Displacement, Rotation),
    or None if it will not fit.
    '''

    # Determine the maximum horizontal and vertical offset using
    # the difference in size between the stone and the key.
    max_x_offset = cols(stone) - cols(key) 
    max_y_offset = rows(stone) - rows(key)

    # Loop through each possible placement of the key.
    for x_offset in range(0, max_x_offset + 1):
        for y_offset in range(0, max_y_offset + 1):
            # Check of the key will fit at a possible placement.
            result = will_fit(key, stone, x_offset, y_offset)

            # If the key does fit, return the result
            if result is not None:
                return result

    # If the key does not fit in any placements, return None.
    return None

def will_fit(key, stone, x_offset, y_offset):
    '''
    Given a key, stone, horizontal and vertical offset of the key,
    determines if it will fit in the stone in one of the 4 rotations.

    :param key: 2D String array containing the key.
    :param stone: 2D String array containing the key.
    :param x_offset: Horizontal displacement of the key.
    :param y_offset: Vertical displacement of the key.
    '''

    # Loop thorugh all 4 possible rotations
    for rotation in range(4):
        keep = True  # Variable used to escape an outer loop.
        if rotation != 0:
            key = rotate(key)
            (x_offset, y_offset) = (y_offset, x_offset)

        for r in range(rows(key)):
            if not keep:
                break
            for c in range(cols(key)):
                key_char = key[r][c]
                stone_char = stone[r + y_offset][c + x_offset]
                if key_char == '*' and stone_char != '.':
                    keep = False
                    break

        if keep:
            return (y_offset, x_offset, direction(rotation))

    return None

def direction(int):
    '''
    Returns a human readable string from an int,
    which represents a rotation. (0-N, 1-E, 2-S, 3-W)

    :param int: Int (0-3 Inclusive) representing rotation.
    :returns: String of direction (N, E, S, W). None if int out of bounds.
    '''
    if int == 0:
        return 'N'
    if int == 1:
        return 'E'
    if int == 2:
        return 'S'
    if int == 3:
        return 'W'

def rows(arr):
    '''
    Returns the rows in a list.

    :param arr: List.
    :returns: The amount of rows it has.
    '''
    return len(arr)

def cols(arr):
    '''
    Returns the columns in a 2D array.

    :param arr: Array.
    :returns: The amount of columns it has.
    '''
    return len(arr[0])

def rotate(arr):
    '''
    Rotates a 2D array clockwise.

    :param arr: Array.
    :returns: The rotated array.
    '''
    return list(zip(*arr[::-1]))

# Below is code for the Fourth Lock
import math
 
def valid_place(maze, r, c):
    if r >= len(maze) or r < 0: return False
    if c >= len(maze[0]) or c < 0: return False
    return True

def min_path(maze, r, c, reverse, score, visited):
    if not valid_place(maze, r, c): return math.inf

    if [r,c] in visited: return math.inf
    else: visited.append([r, c])

    points = ['d', 'w'] if reverse else ['w', 'd']

    location = maze[r][c]

    if location == points[0]: return score
    if location == '#': return math.inf
    if location == '.' or location == points[1]:
        return min(
            min_path(maze, r + 1, c, reverse, 1+score, visited),  
            min_path(maze, r, c + 1, reverse, 1+score, visited),
            min_path(maze, r - 1, c, reverse, 1+score, visited),
            min_path(maze, r, c - 1, reverse, 1+score, visited)
        )

def update_cave(maze, locations):
    new_maze = maze.copy()
    for location in locations:
        center_x, center_y = location
        for x_offset in range(-1, 2):
            r, c = center_x + x_offset, center_y
            if not valid_place(new_maze, r, c): continue
            new_maze[r][c] = "#"
        for y_offset in range(-1, 2):
            r, c = center_x, center_y + y_offset
            if not valid_place(new_maze, r, c): continue
            new_maze[r][c] = "#"
    return new_maze

def find(maze, char):
    for r in range(len(maze)):
        for c in range(len(maze[0])):
            if maze[r][c] == char: return [r, c]
    return None

def lins_escape(maze, locations):
    copy_maze = maze.copy()
    door = find(copy_maze, 'd')
    well_steps = min_path(copy_maze, door[0], door[1], False, 0, [])
    if well_steps == math.inf: return (None, None)

    new_maze = update_cave(copy_maze, locations)

    well = find(new_maze, 'w')
    if well == None: return (well_steps, None)
    door_steps = min_path(new_maze, well[0], well[1], True, 0, [])
    if door_steps == math.inf: return (well_steps, None)

    return (well_steps, door_steps)