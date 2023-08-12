/*
    Written by William Sutherland for 
    COMP20007 Assignment 1 2023 Semester 1
    Modified by Grady Fitzpatrick
    
    Implementation for module which contains map-related 
        data structures and functions.

    Functions in the task description to implement can
        be found here.
    
    Code implemented by <YOU>
*/
#include "map.h"
#include "stack.h"
#include "pq.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

#define MAX_HEXAGON_VALS 7
#define LAND_TIME 5

struct map *newMap(int height, int width) {
    struct map *m = (struct map *) malloc(sizeof(struct map));
    assert(m);
    m->height = height;
    m->width = width;

    // Note this means all values of map are 0
    int *points = (int *) calloc(height * width, sizeof(int));
    assert(points);
    m->points = (int **) malloc(width * sizeof(int *));
    assert(m->points);

    for (int i = 0; i < width; i++){
        /* Re-use sections of the memory we 
            allocated. */
        m->points[i] = points + i * height;
    }

    return m;
}

struct point *newPoint(int x, int y) {
    struct point *p = (struct point *) malloc(sizeof(struct point));
    assert(p);
    p->x = x;
    p->y = y;

    return p;
}

void freeMap(struct map *m) {
    /* We only allocate one pointer with the rest
    of the pointers in m->points pointing inside the
    allocated memory. */
    free(m->points[0]);
    free(m->points);
    free(m);
}

void printMap(struct map *m) {
    /* Print half of each row each time so we mirror the hexagonal layout. */
    int printRows = 2 * m->height;
    if(m->width < 2){
        /* If the width is less than 2, simply a vertical column so no need to print 
            last row as it will be empty. */
        printRows -= 1;
    }
    for (int i = 0; i < printRows; i++) {
        for (int j = i % 2; j < m->width; j += 2) {
            if (j == 1){
                /* For odd row, add a spacer in to place the first value after the 0th hex
                    in the printout. */
                printf("       ");
            }
            /* Default to even. Select every second column. */
            int yPos = i / 2;
            if(j % 2 != 0){
                /* If odd, numbering along height is reversed. */
                yPos = m->height - 1 - yPos;
            }
            int val = m->points[j][yPos];

            /* Print value appropriately. */
            if (val < 0){
                printf("S%.3d", val % 1000);
            } else if (val == 0){
                printf("  L  ");
            } else if (val == 100){
                printf("  A  ");
            } else {
                printf("T+%.3d ", val % 1000);
            }
            printf("          ");
        }

        printf("\n");
    }
}

int validPoint(struct map *m, int x, int y) {
    // Negative points are not valid
    if (x < 0 || y < 0) return 0;
    // Make sure the coords are within the boundary
    return x < m->width && y < m->height;
}

int validSPoint(struct map *m, struct point* pt) {
    return validPoint(m, pt->x, pt->y);
}

int val(struct map *m, struct point *a) {
    return m->points[a->x][a->y];
}

int length(struct map *m, struct point *a, struct point *b) {
    int val_a = val(m, a), val_b = val(m, b);
    // Moving from Ocean
    if (val_a < 0) return 2 + ceil(pow(val_a, 2) / 1000);
    // Airport
    if (val_a == 100 && val_b == 100) {
        return fmax(15, pow(abs(a->x - b->x), 2) - 85);
    }
    // Land
    return 5; 
}

/* Returns if a pq contains a point. */
int hasPoint(struct pq *q, struct point *item) {
    for(int i = 0; i < q->size; i++) {
        if (!q->nodes[i].item) break;
        // Cast may be dangerous
        struct point* pt = (struct point*) q->nodes[i].item;
        if (pt->x == item->x && pt->y == item->y) return 1;
    }
    return 0;
}

struct point *getAjacentPointsAndAirports(struct map *m, struct point *p) {
    struct point *adjPts = getAdjacentPoints(m, p);
    if (val(m, p) != 100) return adjPts; // Current point not an airport.

    // Count the amount of adjacent points
    int adj_n = 0, adj_size = MAX_HEXAGON_VALS;
    for(;adj_n < MAX_HEXAGON_VALS && (adjPts[adj_n].x != -1); adj_n++);

    // Look for airports within the entire map
    for (int r = 0; r < m->width; r++) {
        for (int c = 0; c < m->height; c++) {
            // Skip the current point, and any thats not an airport.
            if (m->points[r][c] != 100 || (r == p->x && c == p->y)) continue;

            // Skip points which have already been included
            for (int i = 0; i < adj_n; i++) {
                if (adjPts[i].x == r && adjPts[i].y == c) continue;
            }

            // Allocating more memory to the list if needed
            if (adj_n >= adj_size) {
                adj_size *= 2;
                adjPts = realloc(adjPts, adj_size * sizeof(*adjPts));
                assert(adjPts);
            }
            
            // Inserting the point
            adjPts[adj_n++] = *newPoint(r, c);
        }
    }

    // -1 -1 to indicate the end of the array
    adjPts[adj_n] = *newPoint(-1, -1);
    return adjPts;
}

int getIslandVal(struct map *m, int x, int y, int *amt, struct map *c_map) {
    // Base Case to reject any invalid points (not on map, already explored, or ocean)
    if (!validPoint(m, x, y)) return 1;
    if (c_map->points[x][y] != 0 || m->points[x][y] < 0) return 1;
    int loc = m->points[x][y], result = loc;

    // Handle Land without Treasure
    if (loc == 100 || loc == 0) result = 1;
    else *amt += 1; // land with treasure

    // Set point to already visited
    c_map->points[x][y] = -1;

    // Recursive DFS to all adjacent points
    struct point *curr_p = newPoint(x, y);
    struct point *adjPoints = getAdjacentPoints(m, curr_p), *adjPoint = adjPoints;
    while (adjPoint->x != -1) {
        result *= getIslandVal(m, adjPoint->x, adjPoint->y, amt, c_map);
        adjPoint = adjPoint + 1;
    }

    free(curr_p);
    free(adjPoints);
    return result;
}

int dist(struct map *m, struct point *a, struct point *b) {
    // Inverted y indices for odd z indices
    int ay = a->x % 2 == 0 ? a->y : m->height - a->y;
    int by = b->x % 2 == 0 ? b->y : m->height - b->y;
    // This only works... 95% of the time.
    // Something weird is happening (e.g. 0,0 to 6,3)
    return round(sqrt(pow(a->x - b->x, 2) + pow(ay-by, 2)));
}

/* IMPLEMENT PART A HERE */
struct point *getAdjacentPoints(struct map *m, struct point *p) {
    struct point *ans = malloc(MAX_HEXAGON_VALS * sizeof(*ans));
    if (!validSPoint(m, p)) { // Make sure the point is on the map
        ans->x = ans->y = -1; return ans;
    }

    struct point **tmp = malloc(MAX_HEXAGON_VALS * sizeof(**tmp));
    int tmp_i = 0, ans_i = 0;
    assert(ans); assert(tmp);

    // Manually inserting all 6 adj points
    tmp[tmp_i++] = newPoint(p->x-1, m->height-p->y-1); // Bottom left
    tmp[tmp_i++] = newPoint(p->x-1, m->height-p->y); // Top Left
    tmp[tmp_i++] = newPoint(p->x, p->y-1); // Top
    tmp[tmp_i++] = newPoint(p->x, p->y+1); // Bottom
    tmp[tmp_i++] = newPoint(p->x+1, m->height-p->y-1); // Bottom Right
    tmp[tmp_i++] = newPoint(p->x+1, m->height-p->y); // Top Right

    // Adding all valid points to the answer
    for (int i = 0; i < tmp_i; i++) {
        if (validSPoint(m, tmp[i])) ans[ans_i++] = *tmp[i];
        else free(tmp[i]);
    }

    ans[ans_i] = *newPoint(-1, -1);
    free(tmp);
    return ans;
}

/* IMPLEMENT PART B HERE */
int mapValue(struct map *m) {
    int total_val = 0, amt_treasure = 0;
    struct map *c_map = newMap(m->height, m->width); // to track visited nodes

    // Loop over all map nodes, and conduct DFS on unvisited land.
    for (int r = 0; r < m->width; r++) {
        for (int c = 0; c < m->height; c++) {
            if (m->points[r][c] >= 0 && c_map->points[r][c] == 0) {
                // Calculate total treasure value per island.
                total_val += amt_treasure * getIslandVal(m, r, c, &amt_treasure, c_map);
                amt_treasure = 0;
            }
        }
    }

    freeMap(c_map);
    return total_val;
}

/* IMPLEMENT PART D HERE */
int minTime(struct map *m, struct point *start, struct point *end) {
    struct map *dist = newMap(m->height, m->width);
    struct pq *q = createPQ();

    // Inserting all points
    for(int r = 0; r < m->width; r++) {
        for(int c  = 0; c < m-> height; c++) {
            int init_dist = (r == start->x && c == start->y) ? 0 : INT_MAX;
            struct point *pt = newPoint(r, c);
            dist->points[r][c] = init_dist;
            insert(q, pt, INT_MAX - init_dist);
        }
    }

    while (!isEmpty(q)) {
        struct point *a = pull(q);
        struct point *adjPoints = getAjacentPointsAndAirports(m, a);
        struct point *b = adjPoints;

        while (b->x != -1) {
            int alt = val(dist, a) + length(m, a, b);
            if (hasPoint(q, b) && alt < val(dist, b)) {
                dist->points[b->x][b->y] = alt;
                insert(q, b, INT_MAX - alt);
            }
            b = b + 1;
        }
    }

    return val(dist, end);
}

/* IMPLEMENT PART E HERE */
int minTimeDry(struct map *m, struct point *start, struct point *end, struct point *airports, int numAirports) {
    /* 
        Since all movement on the map costs LAND_TIME, we only need to check direct-line distance. 

        The simplest approach is to perform Dijkstra's algorithm or Uniform Cost Search on the
        graph comprising the start point, end point and airports.
    */
    /* Comes from LAND_TIME and the sea cost always being equal. */
    int stepCost = LAND_TIME;

    int distances[numAirports + 2];
    int visited[numAirports + 2];

    /* Set up priority queue. */
    struct pq *pq = createPQ();

    /* Start point index is assigned to the second last item in the distances array. */
    int startIndex = numAirports;
    /* End point index is assigned to last item in distances array. */
    int endIndex = numAirports + 1;

    distances[startIndex] = 0;
    /* Default end distance to the direct-line distance. */
    distances[endIndex] = hexDistance(m, start, end) * stepCost;

    /* Insert with negative priority to prioritise lower distance values. */
    insert(pq, (int *) (size_t) endIndex, -distances[endIndex]);
    
    /* Process all airports starting from start location to avoid having to handle different
        kinds of source points. */
    for (int i = 0; i < numAirports; i++){
        /* Walk. */
        distances[i] = hexDistance(m, start, &airports[i]) * stepCost;
        /* Insert with negative priority to prioritise lower distance values. */
        if(distances[i] < distances[endIndex]){
            insert(pq, (int *) (size_t) i, -distances[i]);
        }
        visited[i] = 0;
    }
    /* Mark start vertex visited. */
    visited[startIndex] = 1;

    while(! isEmpty(pq)){
        int nextIndex = (int)((size_t) pull(pq));
        if (distances[nextIndex] == distances[endIndex]){
            break;
        }
        /* Stale priority. */
        if (visited[nextIndex]){
            continue;
        }
        /* Mark visited. */
        visited[nextIndex] = 1;
        /* Point must be an airport. */
        for (int i = 0; i < numAirports; i++){
            if (i == nextIndex){
                /* Self-edges uninteresting. Skip! */
                continue;
            }
            int walkTime = hexDistance(m, &airports[nextIndex], &airports[i]) * stepCost;
            int flyTime  = calculateAirportCost(m, getIndex(m, &airports[nextIndex]), getIndex(m, &airports[i]));
            int minTime;
            if (flyTime < walkTime){
                minTime = flyTime;
            } else {
                minTime = walkTime;
            }

            if (distances[nextIndex] + minTime < distances[i]){
                /* Cheaper, update. */
                distances[i] = distances[nextIndex] + minTime;
                if(distances[i] < distances[endIndex]){
                    insert(pq, (int *) (size_t) i, -distances[i]);
                }
            }
        }
        /* See if we can walk to the end point - this also catches cases where end point is an airport. */
        int walkTime = hexDistance(m, &airports[nextIndex], end) * stepCost;
        if (distances[nextIndex] + walkTime < distances[endIndex]){
            distances[endIndex] = distances[nextIndex] + walkTime;
            insert(pq, (int *) (size_t) endIndex, -distances[endIndex]);
        }
    }


    return distances[endIndex];
}

/* Converts the point p from zig-zagging ordering to its place where all cells
begin from the same cartesian coordinate (offset vertically by one cell for every 
odd x coordinate) so that a direct x/y difference can be calculated. */
void convertToZeroBased(struct map *m, struct point *p);

void convertToZeroBased(struct map *m, struct point *p){
    if (p->x % 2 == 1) {
        p->y = (m->height - 1) - p->y; // Odd
    }
}

int hexDistance(struct map *m, struct point *start, struct point *end){
    /* 
        If only one of x or y need to be changed, the difference between the
        y-coordinates or x-coordinates will give the shortest number of hexes
        required. 
        
        If both need to be corrected, the hexagonal shape means we can move
        half a square up or down in the y-axis every step along the x-axis.

        If we are starting on an odd x-axis square, this is .5 further along 
        the y-axis (starting from 0).
    */
    int totalSteps = 0;

    /* 1. Convert both coordinates to 0-based numbering. */
    struct point zeroStart = *start;
    struct point zeroEnd = *end;
    convertToZeroBased(m, &zeroStart);
    convertToZeroBased(m, &zeroEnd);

    /* 2. Find x-difference. */
    int xDiff = zeroEnd.x - zeroStart.x;

    /* Make positive. */
    if (xDiff < 0) xDiff *= (-1);

    /* 3. Work out y-difference required. */
    int yDiff = zeroEnd.y - zeroStart.y;

    /* 4. Convert current y-difference to 2x y-difference. */
    int yDiffCart = 2*yDiff;
    /* If the x-position is odd, the y-coordinate is 1 further 
        down. */
    if (zeroStart.x % 2 == 1) yDiffCart -= 1;
    if (zeroEnd.x % 2 == 1) yDiffCart += 1;
    if (yDiffCart < 0) yDiffCart *= (-1);

    /* 5. Remove y-difference contributed by x-difference movement. */
    yDiffCart -= xDiff;
    totalSteps += xDiff;

    /* 6. If any y-difference remains, switch back to coordinate system
        and add on y-difference as movement. */
    if(yDiffCart > 0){
        /* Total steps increases by one if half-step required to reach
            final y position. */
        totalSteps += (yDiffCart + 1) / 2;
    }

    return totalSteps;
}
