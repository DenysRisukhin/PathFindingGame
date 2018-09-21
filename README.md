Watch gameplay video here:

[![viberimage](https://user-images.githubusercontent.com/38033580/44616426-0b998900-a858-11e8-8c2b-05b664429490.jpg)
](http://www.youtube.com/embed/kY81mSSV4P8)


                                            Description

Tools:

    ● IDE: Xcode 8.3.3

    ● Language: C++

    ● Game engine: Cocos2dx 3.15.1

Game Flow:

![0-02-04-f4a4d546b8fa311a666c05113ad88c7812f92b00ce32b4b305811cb38820e560_1c6d92eca32be0](https://user-images.githubusercontent.com/38033580/44617141-53bfa800-a866-11e8-9217-e92aa5ba5db4.jpg)

Gameplay Completion:

    Displaying basic elements:                  background, simple menu, tilemap, player

    Displaying additional information:     	    key counter

Game Controls:

    move:	          mouse left button click
    move directions:  diagonal/left/right/up/down

Audio:

    Music support

    Support for sound effects according to the events of the game

    Multiple channels

Used abstractions:

    Graph: A data structure that can report the neighbors of each of the points.

    The class Graph uses for path representation as a graph
    - The class Node - inner class, which keep tile coordinates
    - The class PathStepData - inner class, which keep:
        1. start and final tile coordinates
        2. Tile base cost
        3. Compute the cost of moving from a step to an adjacent one

    Points (locations): a simple value (int, string, tuple, etc.) that marks the points in the graph.

    Searching: an algorithm that receives a graph, a start point, and (optionally) an endpoint that computes useful information (visited, pointer to the parent element, distance) for some or all points.

    class PathFinder
        - The class ManhattanDistance - inner class, which calculates the total number of step moved horizontally and vertically to reach the final desired step from the current step, ignoring any obstacles that may be in the way.
    Compute the H score from a position to another (from the current pos to the final desired pos).
    H is the estimated movement cost from the current tile to the destination point
        - The class ShortestPathStep - inner class, which represents a step on a path.
        1. gCost - The movement cost from the start point to the current tile
        2. fCost - The final cost (F = G + H)

    - The class PriorityQueue 
        1. openSteps - PriorityQueue to write down all the tiles that are being considered to find the shortest path
        2. closedSteps - PriorityQueue to write down the tiles that does not have to consider it again

    Queue: The data structure used by the search algorithm to select the order of processing points.

Pseudocode A*:

    VAR CLOSED = EMPTY()

    VAR OPEN = LIST(START)

    VAR FROM = MAP(NULL)

    G[START] = 0

    F[START] = G[START] + H(START,END)

    WHILE(OPEN)

        CURR = MIN_F(OPEN)

        IF(CURR == END)

            RETURN SUCCESS

        REMOVE(CURR, OPEN)

        ADD(CURR, CLOSED)

    FOREACH NEIGHBOUR IN UNCLOSED_NEIGHTBOUR(CURR)

        TEMP_G = G[CURR] + DIST(CURR, NEIGHBOUR)

        IF(NEIGHBOUR NOT IN OPEN OR TEMP_G < G[NEIGHBOUR])

            FROM[NEIGHBOUR] + CURR;

            G[NEIGHBOUR] = TEMP_G;

            F[NEIGHBOUR] = G[NEIGHBOUR] + H(NEIGHBOUR, END);

        IF(NEIGHBOUR NOT IN OPEN)

            ADD(NEIGHBOUR, OPEN)

    RETURN FAILURE

