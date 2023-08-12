from question3 import intersection_step
from collections import defaultdict


class Vehicle:
    """
    A class to represent a car on a network.
    :attribute id: Car's ID
    :attribute path: List containing the Car's Path
    :attribute loc_index: Index of the car's current loc on path.
    :attribute loc, next_loc: Car's current and next node.
    :attribute driving_until: Time when the car will reach next_loc.
    :attribute waiting_since: Time when the car started waiting at
    an intersection.
    :attribute travel_time: Time taken to travel from loc to next_loc.
    :attribute intersection_move: Tuple containing the previous and next node.
    """

    # Temporary vars used to hold data about a car's loc, id & path.
    id, path = 0, []
    loc_index, loc, next_loc = 0, -1, -1

    # Temporary vars used to handle waiting, driving and intersections.
    driving_until, waiting_since, travel_time = -1, -1, -1
    intersection_move = None

    def __init__(self, id, path):
        """
        Initiates the Vehicle object.
        :param id: Int ID of the Car
        :param path: List of Int, representing the car's path.
        """

        self.id, self.path = id, path

    def move_and_get_state(self, intersections, road_times, global_timestep):
        """
        Function to move and track the car through the network.
        :param intersections: Dictionary of intersection stages.
        :param road_times: Dictionary of road travel times.
        :param global_timestep: Current time step of the network.

        :returns: String representing the state of the car.
        (WAITING, DRIVE, ARRIVE, INTERSECTION)
        """

        just_arrived = False
        # Car is still waiting at an intersection
        if self.waiting_since != -1:
            return 'WAITING'

        # Car was driving
        if self.driving_until != -1:
            # Car still has not arrived yet
            if global_timestep < self.driving_until:
                return 'DRIVE'

            # Car has just arrived
            if global_timestep == self.driving_until:
                self.driving_until, just_arrived = -1, True

                # Move Forward by increasing the location index
                self.loc_index += 1

                # Check if the car has arrived at its final destination
                if self.loc_index == len(self.path) - 1:
                    return 'ARRIVE'

        # Update loc, next loc, then calculate the time to travel between.
        self.loc = self.path[self.loc_index]
        self.next_loc = self.path[self.loc_index + 1]
        self.travel_time = road_times[tuple([self.loc, self.next_loc])]

        # If the car just arrived at an intersection
        if just_arrived and self.loc in intersections:
            # Calculate the intersection move (node before, node after)
            self.intersection_move = (self.path[self.loc_index - 1],
                                      self.next_loc)
            return 'INTERSECTION'

        # If the car is driving to a new node
        self.driving_until = global_timestep + self.travel_time
        return 'DRIVE'


def simulate(intersections, road_times, cars):
    """
    Solves Question Four of Project Two.
    :param intersections: Dictionary of intersection stages.
    :param road_times: Dictionary of road travel times.
    :param cars: List of cars (id, path, entry_time)
    """

    global_time = 0

    # car_list - List containing Vehicles (Objects)
    # car_moves - List containing moves made by a car
    # cars_at_intersections - Dict containing Vehicles at an intersection
    car_list, car_moves, cars_at_intersections, = {}, [], defaultdict(list)

    # Use the original list to keep track of how many cars are left
    while cars:
        # Current current_moves - List of car moves at current time
        # arrived_cars - List of car ids that have arrived at destination
        current_moves, arrived_cars = [], []

        # This loop adds new cars to the loop as Vehicle objects
        for car_state in cars:
            _id, path, enter_time = car_state[0], car_state[1], car_state[2]

            # If its time for a car to enter, add new Vehicle to list
            if global_time == enter_time:
                car_list[_id] = Vehicle(_id, path)

        # Process Vehicles, recording drive and arrive actions.
        for car in car_list.values():
            car_state = car.move_and_get_state(intersections,
                                               road_times, global_time)

            if car_state == 'DRIVE':
                current_moves.append([car.id,
                                      f'drive({global_time}, {car.id}, '
                                      + f'{car.loc}, {car.next_loc})'])

            elif car_state == 'ARRIVE':
                # Car arrived at it's destination, (later) remove from network.
                arrived_cars.append(car.id)
                current_moves.append([car.id,
                                      f'arrive({global_time}, {car.id}, '
                                      + f'{car.path[-1]})'])

            elif car_state == 'INTERSECTION':
                # Car arrived at intersection, add to list to process later.
                cars_at_intersections[car.loc] += [car]
                # Record when the car started waiting
                car.waiting_since = global_time

        # Process Vehicles at intersections, recording drive and wait actions.
        for intersection_id in cars_at_intersections.keys():
            # Cars here is a list of all Vehicles at this intersection
            cars_here = cars_at_intersections[intersection_id]

            # Continue to next intersection if no cars are here
            if not cars_here:
                continue

            # Find car IDs that are and aren't allowed to move
            can_move = intersection_step(
                intersections, road_times, intersection_id,
                [(c.id, c.path, c.waiting_since) for c in cars_here],
                global_time)

            cant_move = [c.id for c in cars_here if c.id not in can_move]

            # For each car id that can move
            for _id in can_move:
                car = car_list[_id]

                # Set the Vehicle Driving
                car.driving_until = global_time + car.travel_time
                car.waiting_since = -1  # Stop the Vehicle from waiting

                # Append Driving action
                current_moves.append([_id, f'drive({global_time}, {car.id}, '
                                      + f'{car.loc}, {car.next_loc})'])

            # For each car ID that can't move
            for _id in cant_move:
                # The Vehicle is still waiting, append waiting action
                current_moves.append([_id, f'wait({global_time}, {_id}, '
                                      + f'{intersection_id})'])

            # Remove cars that left the intersection from dictionary
            cars_at_intersections[intersection_id] = \
                [c for c in cars_here if c.id in cant_move]

        # Remove cars that left the network (arrived at destination)
        cars = [c for c in cars if c[0] not in arrived_cars]
        for arrived_id in arrived_cars:
            del car_list[arrived_id]

        # Once all Vehicles are processed, sort current moves by Car ID
        current_moves = sorted(current_moves, key=lambda d: (d[0]))
        # Append each sorted action in order of Car IDs
        car_moves += [move[1] for move in current_moves]
        # Increment Global Time
        global_time += 1

    return car_moves
