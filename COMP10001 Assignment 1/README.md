# COMP10001 Assignment 1

In this project you will write functions to solve three increasingly tricky puzzles. While the setting of these problems is a somewhat whimsical story of an intrepid underground adventurer, the problems exemplify several important aspects of computer programming.

There is an optional fourth question for bonus marks. You can obtain full marks for the project without completing this question. However, if you are interested in challenging yourself, this question - determining whether our intrepid adventurer can escape - is markedly more difficult than the first three!

## First Lock
The first lock consists of a panel filled with sparkling gems: diamonds, rubies, emeralds, and so on. Beside the panel is a row of buttons, one corresponding to each type of gem. To unlock the door, Lin must press the button corresponding to the most common type of gem in the panel. Note that, if there is a tie between the most common type of gem (ie, there are equal numbers), then more than one button will need to be pressed.

Your task is to write a function first_lock(gems) that determines which button(s) should be pressed to unlock the first door. Your function should take a list of gems (represented as strings) on the panel as input. Your function should return a list containing the button or buttons (again, represented as strings) that need to be pressed as output. If more than one button should be pressed, they should be returned in alphabetical order.

Assumptions:
You may assume that all gems are spelled consistently using lower case letters only (so you don't have to worry about "topaz" and "Topaz" both appearing in the list of gems).

Example Calls:
```
>>> print(first_lock(['ruby', 'ruby', 'diamond', 'amethyst']))
['ruby']
>>> print(first_lock(['ruby', 'ruby', 'diamond', 'diamond', 'amethyst']))
['diamond', 'ruby']
>>> print(first_lock(['topaz']))
['topaz']
```

## Second Lock
The second lock consists of a "T"-shaped groove in the door. The left branch of the T is labelled source, the right branch is labelled destination, and the bottom branch is labelled store. Several gems are mounted in the left branch (the source) such that they can be slid along the groove toward the right or bottom branches (the destination or store, respectively). Each of the gems is numbered consecutively beginning from one. Lin notes that no numbers are skipped, nor are any numbers duplicated; however, some of the gems are out of order. Gems cannot be slid over the top of other gems.

To unlock the door, Lin must slide the gems onto the destination branch in increasing numerical order. The store branch can be used to help reorder the gems (see below for a step by step example). After some experimentation, Lin notes that any gem which is slid into the destination or store branches can no longer slide back toward the source branch.

Your task is to write a function second_lock(gems) that determines whether, for a given configuration of gems in the source branch, it is possible to unlock the door. Your function should take a list of integer gem numbers as input (in which the gem at position 0 in the list is the last gem that can be moved from the source branch).

Your function should return a tuple consisting of a boolean value specifying whether the door can be unlocked or not, and an integer corresponding to the largest number of gems that are required to be located on the store branch at any one point in time while the puzzle is being solved. If the door cannot be unlocked, this number should be -1.

Assumptions:
You may assume that the N gems are numbered from 1..N inclusive, with no missing or repeated numbers.
You may assume that all branches are sufficiently long to contain all of the gems (ie, you won't run out of space on any branch).

Example Calls:
```
>>> print(second_lock([4, 3, 2, 1]))
(True, 0)
>>> print(second_lock([1, 2, 3, 4, 5]))
(True, 4)
>>> print(second_lock([6, 1, 4, 5, 2, 3]))
(False, -1)
>>> print(second_lock([3, 4, 5, 2, 1]))
(True, 2)
```

## Third Lock
The third lock consists of a stone with a set of irregular holes carved out of it. Lin realises that an oddly-shaped gem found elsewhere in the cave that might fit this lock.

Your task is to write a function third_lock(key, stone) that determines whether the "key" gem will fit the pattern of holes in the "stone" lock. If the key is smaller than the stone, then the key can be moved to any location relative to the stone, so long as it doesn't extend beyond the edge of the stone in any direction. The key can also be rotated to any of four possible orientations: (N)orth (with the top of the key facing up), (E)ast (with the key rotated 90 degree clockwise such that the top faces to the right), (S)outh (with the key rotated 180 degrees clockwise such that the top faces downwards), or (W)est (with the key rotated 270 degrees clockwise such that the top faces to the left). The key can only be moved or rotated, it cannot be flipped over.

Your function should take as input two nested lists describing the shape of the key (which parts are "raised") and the shape of the stone (which parts contain "holes" that raised parts of the key fit within). Your function should return a tuple specifying the location (in terms of row-index and column-index) and orientation of the key if it will fit, or None if the key will not fit for any location or orientation. Note that while the key will only open the lock if every raised part fits within a hole in the stone, it is not necessary for every hole in the stone needs to be matched by a raised part of the key.

If a key will fit a stone in multiple orientations and/or locations, then your function should return the location that is closest to the top of the stone. If there are multiple such locations then your function should return the location that is closest to the left of the stone. If the key at this location will fit in multiple orientations, then your function should return the orientation that requires the smallest clockwise rotation. For example, if the key would fit at a particular location when oriented both (E)ast and (S)outh, then your function should return (E)ast, as that only entails a 90, rather than 180, degree clockwise rotation.

Assumptions:
You may assume that the width and height of both the key and the stone are greater than zero.
You may assume that the key and the stone are both rectangular (including square; ie width == height).
You may assume that the initial orientation of the key (ie, as specified by the input) is (N)orth.
The entirety of the key must fit within the stone, even parts that are not raised.
Example:
In a nested list describing a key, '*' denotes a raised part of the key while '.' denotes blank space. In a nested list describing a stone, '#' denotes a solid part of the stone while '.' denotes a hole in the stone. Thus the key can only be used to unlock the door if all of the raised parts of the key ('*') line up with holes in the stone ('.') (for some location and orientation).

The following shows a simple representation of two keys and three stones, together with the corresponding nested list representation:
```
Key 1
**
*.
​
key1 = [['*', '*'], ['*', '.']]

Key 2
.*
*.
.*
​
key2 = [['.', '*'], ['*', '.'], ['.', '*']]

Stone 1
...
.#.
...
​
stone1 = [['.', '.', '.'], ['.', '#', '.'], ['.', '.', '.']]

Stone 2
#..
##.
..#
​
stone2 = [['#', '.', '.'], ['#', '#', '.'], ['.', '.', '#']]

Stone 3
##.
.#.
#.#
​
stone3 = [['#', '#', '.'], ['.', '#', '.'], ['#', '.', '#']]

Example calls:
>>> print(third_lock(key1, stone1))
(0, 0, 'N')
>>> print(third_lock(key2, stone1))
(0, 0, 'N')
>>> print(third_lock(key1, stone2))
(0, 1, 'E')
>>> print(third_lock(key2, stone2))
(0, 1, 'S')
>>> print(third_lock(key1, stone3))
None
>>> print(third_lock(key2, stone3))
(1, 0, 'W')
```

## Fourth Lock
Note: This question is optional and for bonus marks only! You can obtain full marks for Project 1 without attempting this question. It is more difficult than the previous questions and intended for students who would like to challenge themselves. As such, lecturers and tutors will provide less guidance on how to complete it.

Lin has gained access to the final cavern containing the Well of Wisdom and filled a handy collapsible bucket brought along for just this purpose. As the bucket is filled, the earth rumbles and the ceiling begins to collapse above Lin's head!

Your task is to write a function lins_escape(cavern, locations) that determine whether Lin can get to the Well and then return safely to the door, and if so the shortest route by which this can be achieved.

Your function should take as input a nested list describing the cavern, and a list of locations, each specified as a (row, column) tuple, where the ceiling collapses once the Well is reached. When the ceiling collapses at a particular location, that location and all horizontally or vertically adjacent locations (ie, up to four additional locations) are filled with rubble and become impassable.

Your function should return a tuple consisting of the length of the shortest path from the cavern door to the Well, and from the Well back to the cavern door, or None if no such path is possible (either to or from the Well).

Assumptions:
All areas outside of the edges of the cavern map are stone and impassable.
Lin may only move to horizontally or vertically adjacent locations (ie, not diagonally), and may not enter locations containing stone or rubble.
You may assume that the ceiling collapses simultaneously at all specified locations as soon as Lin reaches the well.
If the collapse of the ceiling affects the cell containing the Well (and Lin) then no escape is possible.

Example
In a nested list descrbining a cavern: '.' denotes empty space (through which Lin can move); '#' denotes stone, which is impassable; 'd' denotes the door through which Lin enters the cavern and (hopefully!) escapes; 'w' denotes the Well of Wisdom.

```
The cavern:

d...
..#.
....
.#w.

is encoded as:

cavern = [['d', '.', '.', '.'], ['.', '.', '#', '.'], ['.', '.', '.', '.'], ['.', '#', 'w', '.']]

Example calls:
>>> print(lins_escape(cavern, [(2, 0)]))
(5, 7)
>>> print(lins_escape(cavern, [(2, 0), (1, 1)]))
(5, None)
```