# COMP10001 Assignment 2

In this project, you will build a simple model of a road network. These types of models are commonly used to understand traffic behavior, identify points of congestion, and plan traffic light sequences that minimize congestion and average travel time. Real-world traffic simulation models are very complex, and factor in many aspects of traffic behavior.

A road network can be modelled as a graph. Nodes in the graph represent intersections and locations. Each node is assigned an ID number for ease of identification. Edges between nodes represent roads. The graph is directed, as it is possible for roads to allow travel in one direction but not the other (e.g. one-way streets). One sample road network is found in the directory: road_network.png

Note in this sample network the yellow squares represent intersections while the blue circles represent locations. A vehicle will leave one location node, potentially travel through a number of intersections, then arrive at another location node.

Note also that not every route through an intersection can be traversed simultaneously. For example, intersection 0 in the diagram above would not likely allow a car to travel from 2 to 3 at the same time as another car is travelling from 1 to 4 as this would result in a collision.

To describe this behavior, we can consider a particular traffic signal which identifies which paths through the intersection can be traversed at any point in time. This can be modelled as a list of (source, destination) pairs. For example, [(5,6), (6,5)] is a traffic signal for intersection 4 that indicates that cars can travel from node 5 to node 6 or from node 6 to node 5. We can then describe the intersection as a list of traffic signals, each of which is active for one timestep and cycle back to the first traffic signal after the final signal has completed. For example, [[(5,6), (6,5)], [(0, 6), (6, 0), (5, 0), (0, 5)]] indicates:

At timestep 0 cars can travel from 5 to 6 or from 6 to 5
At timestep 1 cars can travel from 0 to 6, 6 to 0, 5 to 0 or 0 to 5
At timestep 2 cars can travel from 5 to 6 or from 6 to 5 etc...
To simplify our model, we will assume that an intersection allows at most one vehicle through in each permitted direction at each timestep. For example, at timestep 0 one vehicle may travel from 5 to 6 and one vehicle may travel from 6 to 5. All other vehicles must wait at the intersection.

Once a vehicle has exited the intersection it will be placed on the road taking it towards the next intersection or location node. For simplicity, we assume it will take a fixed number of timesteps (one or more) to traverse the road.

Each car will begin its journey at one of the location nodes and travel through zero or more intersections until it reaches its final destination node.

## Question 1
Write a function load_road_network(filename) that parses a file with the structure described below and returns a tuple containing two dictionaries.

The first dictionary should represent the intersections. The keys of this dictionary should be the ID numbers of each intersection and the values should be the corresponding list of traffic signals.

The second dictionary should represent the roads. The keys of this dictionary should be tuples containing the source and destination nodes of the road, and the values should be the number of timesteps required to traverse the corresponding road.

The structure of the file is as follows:

The text '#Intersection:' followed by the ID number (a positive integer or zero) of an intersection
An arbitrary number of lines each corresponding to a traffic signal for that intersection. The traffic signals are expressed as a sequence of source, destination pairs. Each source, destination pair is encased by brackets and separated from the next pair by a semicolon (;). The source and destination are both integers >= 0, and there must be at least one pair per traffic signal in the text file.
The points above are repeated for each intersection in the road network
After the intersections have been specified, the text '#Roads' is used to indicate that the remainder of the file will specify road traversal times
Each subsequent line describes a road, using the format (source, destination):time
There may be an arbitrary number of blank lines following the definition of each intersection.

Example Calls:
```
>>> intersections_list, roads_cost = load_road_network('road_sample.txt')
>>> print(intersections_list, '\n', roads_cost)
{0: [[(4, 1), (1, 4)], [(2, 3), (3, 2)], [(4, 2), (2, 4), (1, 3), (3, 1)], [(1, 2), (2, 1), (4, 3), (3, 4)]], 4: [[(5, 6), (6, 5)], [(0, 6), (6, 0), (5, 0), (0, 5)]]} 
 {(0, 1): 1, (1, 0): 1, (0, 2): 1, (2, 0): 1, (0, 3): 1, (3, 0): 1, (0, 4): 3, (4, 0): 3, (4, 5): 2, (5, 4): 2, (4, 6): 2, (6, 4): 2}
```

## Question 2
A path through the road network can be described by listing each node the vehicle will travel through. For example, the path [2,0,4,6] would indicate that the vehicle starts at node 2, travels through nodes 0 and 4 then arrives at node 6. Recall that the first and last nodes must be location nodes, while all other nodes must be intersections. In this task we wish to understand:

Whether a vehicle starting its journey at timestep 0 is able to traverse the path without being stopped at any intersection, assuming there are no other vehicles on the road network
How long that journey would take
Write a function path_cost(path, intersections, road_times). If it is possible to traverse the path starting at timestep 0 without being stopped at any intersections then the function should return the total number of timesteps taken. If not, it should return None. You may assume that the path is a valid, traversable path.

Your function should take the following arguments:

path is a list representing the nodes through which the vehicle will traverse
intersections and road_times are the dictionaries that represent the road network, as described in Question 1. A working implementation of the load_road_network function has been made available, you may make calls to this function if you wish.
You may assume that a vehicle moves in the timestep during which it enters the road network. For example, if a vehicle enters the network at node 1 at timestep 0 travelling towards node 2 and the distance between nodes 1 and 2 is 1 timestep, then the vehicle would arrive at node 2 at timestep 1.

You may further assume that it takes 0 time to cross an intersection with a favourable traffic signal. A vehicle may cross an intersection and drive along a road during the same timestep only if the vehicle crosses the intersection before driving along the road.

For example, consider the following function call:

```
>> simple_intersections = {0: [[],[(1,2), (2,1)]]}
>> simple_roads = {(0,1):1, (1,0):1, (0,2):1, (2,0):1}
>> path_cost([1,0,2], simple_intersections, simple_roads)
The vehicle's journey will be as follows:

Timestep 0: Depart node 1 heading towards intersection 0
Timestep 1: Arrive at node 0 and pass through the intersection unimpeded
Timestep 2: Arrive at final location node 2
path_cost([1,0,2], simple_intersections, simple_roads) should therefore return a value of 2.

Below are some sample function calls:

>> simple_intersections = {0: [[],[(1,2), (2,1)]]}
>> simple_roads = {(0,1):1, (1,0):1, (0,2):1, (2,0):1}
>> print(path_cost([1,0,2], simple_intersections, simple_roads))
2
​
>> simple_intersections = {0: [[(1,2), (2,1)], []]}
>> simple_roads = {(0,1):1, (1,0):1, (0,2):1, (2,0):1}
>> print(path_cost([1,0,2], simple_intersections, simple_roads))
None
​
>> intersections, road_times = load_road_network('road_sample.txt')
>> print(path_cost([2,0,4,6], intersections, road_times))
None
​
>> intersections, road_times = load_road_network('road_sample.txt')
>> road_times[(2,0)] = road_times[(0,2)] = 2
>> print(path_cost([2,0,4,6], intersections, road_times))
7
```

## Question 3
We now wish to consider a road network with multiple cars traversing it. In order to do that, we need to be able to identify which vehicles will be able to pass through a crowded intersection at any given time. We can represent each car currently waiting at an intersection by a tuple (car_id, path, arrival_time):

car_id is an integer that uniquely identifies a particular car.
path is a list representing the path that the car will take through the network.
arrival_time is an integer representing the timestep in which the car arrived at the intersection.
In this task, we will write a function to process a single step through time at a given intersection. Write a function intersection_step(intersections, road_times, intersection_id, cars_at_intersection, timestep). This function should return a list containing the car_id of each car that will be allowed to proceed through intersection intersection_id at the current timestep, sorted by car_id.

Recall that in our representation, only one vehicle can travel through in each permitted direction at any given timestep. If multiple vehicles are waiting to move through in the same direction, the vehicle that arrived at the intersection first should be given priority. Note that it is not necessary to track a vehicle's path through the network to determine when it arrived at the intersection, you may rely on the specified arrival_time. If two vehicles arrive at the same timestep, you may pass the one with the lower car_id. For this question, as well as Question 4, you may assume that each car will only visit an intersection once as part of its path, and that a car's final destination will never be an intersection. The function will take the following arguments:

intersections and road_times are the dictionaries representing the road network, as imported in Question 1
intersection_id represents the ID of the intersection we are considering
cars_at_intersection is a list of cars present at the intersection represented by intersection_id at the current timestep. Each car is described by the 3-tuple above.
timestep is an integer representing the current timestep

Below are some example function calls:
```
>> simple_intersections = {0: [[], [(1,2), (2,1)]] }
>> simple_roads = {(0,1):1, (1,0):1, (0,2):1, (2,0):1}
>> cars_at_intersection = [(0,[1,0,2], 1), (1, [2,0,1], 1)]
>> intersection_step(simple_intersections, simple_roads, 0, cars_at_intersection, 1)
[0, 1]
    
>> simple_intersections = {0: [[], [(1,2), (2,1)]] }
>> simple_roads = {(0,1):1, (1,0):1, (0,2):1, (2,0):1}
>> cars_at_intersection = [(0,[1,0,2], 1), (1, [2,0,1], 1)]
>> intersection_step(simple_intersections, simple_roads, 0, cars_at_intersection, 2)
[]
    
>> intersections, road_times = load_road_network('road_sample.txt')
>> cars_at_intersection = [(0,[2,0,4,6], 1), (1, [3,0,4,6], 1), (2, [1,0,4,6], 1), (3,[2,0,4,6], 2)]
>> intersection_step(intersections, road_times, 0, cars_at_intersection, 2)
[0]
```
Working implementations of the load_road_network and path_cost functions have been made available, you may make calls to these functions if you wish.

## Question 4
Question 4
We now wish to simulate the running of the entire road network. The simulation will involve cars entering the road network, proceeding through the network to their destination, and leaving the road network at different times. Write a function simulate(intersections, road_times, cars_to_add). Your function should track the position of each vehicle in the road network at each timestep, starting from timestep 0. Your function should return a list of actions specifying what each car currently in the road network is doing at each timestep. Valid actions are:

drive(timestep, car_id, source_node, destination_node) - This represents a car identified by car_id that is traversing the road between source_node and destination_node at timestep
wait(timestep, car_id, intersection_id) - This represents a car identified by car_id that is waiting at intersection intersection_id at timestep
arrive(timestep, car_id, destination) - This represents a car identified by car_id having arrived at destination at timestep
Each action should be represented as a string. The list of actions should be sorted first by timestep and then by car_id.

Your function should take the following arguments:

intersections and road_times are the dictionaries representing the road network, as imported in Question 1
cars_to_add is a list that represents the cars that will be added to the road network as part of the simulation. Each car is described as a tuple (car_id, path, timestep). This tuple represents a unique car_id for each car that will be added, the path that car will take through the network, and the timestep in which the car should begin its journey.
A car may be removed from the simulation after it has arrived at its destination. Your simulation should continue until all cars currently in the road network have reached their destination, and there are no additional cars waiting to be added to the simulation.

Below are some sample function calls:
```
>> simple_intersections = {0: [[(1,2), (2,1)]]}
>> simple_roads = {(0,1):1, (0,2):1, (1,0):1, (2,0):2}
>> simple_cars_to_add = [(0, [1,0,2], 0)]
>> simulate(simple_intersections, simple_roads, simple_cars_to_add)
['drive(0, 0, 1, 0)', 'drive(1, 0, 0, 2)', 'arrive(2, 0, 2)']
    
>> intersections, roads_cost = load_road_network('road_sample.txt')
>> cars_to_add = [(0, [2,0,3], 3), (1,[3,0,2],3)]
>> simulate(intersections, roads_cost, cars_to_add)
['drive(3, 0, 2, 0)', 'drive(3, 1, 3, 0)', 'wait(4, 0, 0)', 'wait(4, 1, 0)', 'drive(5, 0, 0, 3)', 'drive(5, 1, 0, 2)', 'arrive(6, 0, 3)', 'arrive(6, 1, 2)']
```
Working implementations of the load_road_network, path_cost and intersection_step functions have been made available, you may make calls to these functions if you wish.