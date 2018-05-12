
				Используемые абстракции:

Граф: структура данных, которая может сообщить о соседях каждой из точки

The class Graph uses for path representation as a graph
	- The class Node - inner class, which keep tile coordinates
	- The class PathStepData - inner class, which keep:
		 1. start and final tile coordinates
     		 2. Tile base cost
    		 3. Compute the cost of moving from a step to an adjacent one

Точки(локации): простое значение(int, string, tuple и т.д.), помечающее точки в графе

Поиск: алгоритм, получающий граф, начальную точку и (необязательно) конечную точку, вычисляющий полезную информацию (посещенный, указатель на родительский элемент, расстояние) для некоторых или всех точек.

class PathFinder
	- The class ManhattanDistance - inner class, which calculates the total number of 	        step moved horizontally and vertically to reach the final desired step         		from the current step, ignoring any obstacles that may be in the way.
		Compute the H score from a position to another (from the current pos to 		the final desired pos).
		H is the estimated movement cost from the current tile to the destination 		point
	- The class ShortestPathStep - inner class, which represents a step on a path.
		1. gCost - The movement cost from the start point to the current tile
		2. fCost - The final cost (F = G + H)
	- The class PriorityQueue 
		1. openSteps - PriorityQueue to write down all the tiles that are being 			considered to 	find the shortest path
    		2. closedSteps - PriorityQueue to write down the tiles that does not have 			to consider it again

Очередь: структура данных, используемая алгоритмом поиска для выбора порядка обработки точек

				     Псевдокод A*:
var CLOSED = EMPTY()
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
  