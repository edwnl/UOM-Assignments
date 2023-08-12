/*
    Written by William Sutherland for 
    COMP20007 Assignment 1 2023 Semester 1
    Modified by Grady Fitzpatrick
    
    Header for module which contains map-related 
        data structures and functions.
*/
#ifndef MAP_H
#define MAP_H

/* Data structure representing a point */
struct point {
    int x;
    int y;
};

/* Data structure representing a map */
struct map {
    int **points;
    int height;
    int width;
};

/* Create a map of size length * width with values of 0 */
struct map *newMap(int height, int width);

/* Creates a new point with coordinates */
struct point *newPoint(int x, int y);

/* Frees the memory of the map */
void freeMap(struct map *m);

/* Prints the map */
void printMap(struct map *m);

/* Returns if a point is on the map. */
int validPoint(struct map *m, int x, int y); 

/* Returns if a point is on the map. (accepts a point struct) */
int validSPoint(struct map *m, struct point* pt);

/* Returns the value on a map pointed to by a point */
int val(struct map *m, struct point *a);

/* Returns the time taken to travel between two points. */
int length(struct map *m, struct point *a, struct point *b);

/* Returns the adjacent points with airports added to the list */
struct point *getAjacentPointsAndAirports(struct map *m, struct point *p);

/* Recursive DFS function to get the island value */
int getIslandVal(struct map *m, int x, int y, int *amt, struct map *c_map);

/* Returns the distance between point A and B */
int dist(struct map *m, struct point *a, struct point *b);

/* A Returns an array of points that are adjacent to the inputted point */
struct point *getAdjacentPoints(struct map *m, struct point *p);

/* B Returns the total value of treasure from a map */
int mapValue(struct map *m);

/* D Returns the minimum travel time between two points */
int minTime(struct map *m, struct point *start, struct point *end);

/* E Returns the minimum travel time between two points for dry maps */
int minTimeDry(struct map *m, struct point *start, struct point *end, struct point *airports, int num_airports);


#endif
