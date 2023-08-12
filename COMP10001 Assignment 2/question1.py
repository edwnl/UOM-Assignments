def load_road_network(filename):
    '''
    Solves the first question in Project 2.
    :param filename: String containing the file's name.

    :returns: Tuple containing the intersections and roads.
    '''

    # Read and store lines of the file in a list.
    contents = open(filename).readlines()
    intersections, roads = {}, {}  # Init the empty dictionaries.

    for line_n in range(len(contents)):
        # Retrieve the line by indexing.
        line = contents[line_n]

        # '#Intersection:' indicates the start of intersection info
        if line.startswith('#Intersection:'):
            stages_list = []  # Temp empty list to store stages.

            # Loop each line proceeding the intersection line
            for next_line in contents[line_n + 1:]:
                # If line is empty, it's the end of intersection stages
                if not next_line or next_line.isspace(): 
                    break

                # Using eval and split, convert string info to stages.
                # E.g (5,6);(6,5) -> [[5,6], [6,5]]
                stages = [eval(data) for data in next_line.split(';')]
                
                # Once converted, add the stages to to the temp list.
                stages_list.append(stages)
                
            # Using string slicing, find the intersection number and insert 
            # the intersection stages into the dictionary.
            intersections[int(line[14:])] = stages_list

        # '#Roads' indicates the start of road info
        if line.startswith('#Roads'):
            # Loop each line proceeding the road line
            for next_line in contents[line_n + 1:]:
                # Using eval and split, convert string info to a list.
                # E.g (0,1):1 -> [(0,1), 1]
                road_list = [eval(data) for data in next_line.split(':')]

                # Insert road data into the roads dictionary.
                # road_list[0] - Tuple Containing the move
                # road_list[1] - Int representing the time taken to travel
                roads[road_list[0]] = road_list[1]
        
    return (intersections, roads)