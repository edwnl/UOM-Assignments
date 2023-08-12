def intersection_step(intersections, road_times, intersection_id,
                      cars_at_intersection, timestep):
    '''
    Solves the thrid question in Project 2.
    :param intersections: Dictionary of intersections.
    :param road_times: Dictionary containing travel times.
    :param intersection_id: Current intersection ID.
    :param cars_at_intersection: List of cars at the current intersection.
    :param timestep: Current timestep.

    :returns: List of car ids that are allowed to pass.
    '''

    # Determine the stage of the lights
    stages = intersections[intersection_id]
    lights = stages[timestep % len(stages)]

    # Init temp variables used in the question.
    # moves_and_cars - Dictionary of moves and cars wanting to make that move.
    # cars_allowed - List of car ID's that are allowed to pass.
    moves_and_cars, cars_allowed = {}, []

    for car in cars_at_intersection:
        # Init temp variables for the path of the car and the move its making.
        path, move = car[1], None

        # Find the move before and after this intersection.
        for x in range(len(path)):
            if path[x] == intersection_id:
                move = (path[x - 1], path[x + 1])
        
        # If the move is not permitted, the car cannot move.
        if move not in lights: 
            continue
        
        # Else, update moves_and_cars dict with the move and car data.
        if move not in moves_and_cars.keys():
            moves_and_cars[move] = [car]
        else:
            moves_and_cars[move].append(car)

    for target_move in moves_and_cars.keys():
        # Cars stores the data of a car that is making the target move.
        cars = moves_and_cars[target_move]

        # Sort the data by arrival time (data[2]), then car id (data[0]).
        cars = sorted(cars, key=lambda data: (data[2], data[0]))

        # Once sorted, allow the first car to move.
        cars_allowed.append(cars[0][0])
    
    return cars_allowed