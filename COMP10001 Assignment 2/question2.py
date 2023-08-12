def path_cost(path, intersections, road_times):
    '''
    Solves the second question in Project 2.
    :param path: List of paths taken by the vehicle.
    :param intersections: Dictionary of intersections.
    :param road_times: Dictionary containing travel times.

    :returns: Total Travel Time as int, or None if vehicle will be stopped.
    '''
    total_time = 0  # Used to record the total travel time.

    for time_step in range(len(path) - 1):
        # Set the current and next locations
        current_loc, next_loc = path[time_step], path[time_step + 1]

        # Find the travel time using the road times dictionary
        travel_time = road_times[tuple([current_loc, next_loc])]

        # Check if vehicle is blocked by light at current loc.
        if current_loc in intersections.keys():  
            # Get the intersection if current loc is one.
            intersection = intersections[current_loc] 
            
            # Get the stage of the light
            light = intersection[total_time % len(intersection)]  

            # If the intersection's move is not permitted, return none
            if tuple([path[time_step - 1], next_loc]) not in light:
                return None
        
        # If the intersection is passed, add to travel time
        total_time += travel_time

    return total_time