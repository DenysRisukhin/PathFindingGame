//
//  PathFinder.hpp
//  PathFindingDemo01
//
//  Created by Denys Risukhin on 18/10/2017.
//
//

#ifndef PathFinder_hpp
#define PathFinder_hpp

#include "Graph.hpp"
using namespace std;

class PathFinder
{
    using Graph = Graph;
    using Node = Graph::Node;
    using PathStepData = Graph::PathStepData;
 
public:
    int find(Graph *g, Node start, Node end, std::function<void (PathStepData &conn)> block);
    int find(Graph *g, Node start, Node end, std::function<int(Node &)> heuristic, std::function<void (PathStepData &conn)> block);
    
private:
    
    /*!
    * Represents ManhattanDistance - calculates the total number of step moved
    * horizontally and vertically to reach the final desired step from the current step,
    * ignoring any obstacles that may be in the way
    */
    class ManhattanDistance {
    public:
        ManhattanDistance(Node &target) : _target(target){}
        
        int operator()(Node &from) {
            
            /*!
             * Compute the H score from a position to another (from the current pos to the final desired pos)
             * H is the estimated movement cost from the current tile to the destination point.
             */
            return std::abs(_target._loc.x - from._loc.x) + std::abs(_target._loc.y - from._loc.y);
        }
    private:
        Node &_target;
    };
    
    /*!
     * Represents a step on a path.
     */
    struct ShortestPathStep {
        
        ShortestPathStep() : gCost(-1), fCost(-1) {}
        
        ShortestPathStep(Node &node, const PathStepData &pathStepData, int gScore, int _fCost)
        : _node(node), _pathStepData(pathStepData), gCost(gScore), fCost(_fCost) {}
        
        ShortestPathStep(const ShortestPathStep &rhs) : _node(rhs._node), _pathStepData(rhs._pathStepData), gCost {rhs.gCost}, fCost {rhs.fCost} {}
        
        int compare(ShortestPathStep &rhs) const{
            return fCost - rhs.fCost;
        }
        
        std::string getDesc() const{
            std::stringstream ss;
            ss << "ShortestPathStep {"
            << "node " << _node.getDesc()
            << " pathStepData " << _pathStepData.getDesc()
            << " gCost " << gCost
            << " fCost " << fCost << "}";
            return ss.str();
        }
        
        /*!
         * Represents a tile coordinates.
         */
        Node _node;
        
        PathStepData _pathStepData;
        
        /*!
         * Represents the movement cost from the start point to the current tile.
         */
        int gCost;
        
        /*!
         * Represents the final cost (F = G + H).
         */
        int fCost;

    };
    
    struct Compare {
        bool operator() (ShortestPathStep &a, ShortestPathStep &b){
            return a.compare(b) > 0;
        }
    };
    
    /*!
     * Represents data structure used by the search algorithm to select the order of processing points.
     */
    class PriorityQueue : public priority_queue<ShortestPathStep, deque<ShortestPathStep>, Compare>
    {
    public:
        using Iterator = std::deque<ShortestPathStep>::iterator;

        Iterator find(Node &node){
            auto first = c.begin();
            while (first!= c.end()) {
                if (first->_node == node) {
                    return first;
                }
                ++first;
            }
            return c.end();
        }
        
        bool contains(Node &node){
            auto first = c.begin();
            auto last = c.end();
            while (first!=last) {
                if (first->_node == node)
                    return true;
                ++first;
            }
            
            return false;
        }
        
        void remove(Node &node){
            auto iter = find(node);
            CC_ASSERT(iter != c.end());
            
            iter->_node = Node::INVALID;
        }
        
        const ShortestPathStep &lowestTrace(){
            while (!empty() && top()._node == Node::INVALID) {
                /**
                 * Pop to discard.
                 */
                pop();
            }
            return top();
        }
    };
    
private:
    
    /**
     * PriorityQueue to write down all the tiles that are being considered to find the shortest path.
     */
    PriorityQueue openSteps;
    
    /**
     * PriorityQueue to write down the tiles that does not have to consider it again.
     */
    PriorityQueue closedSteps;
};

#endif /* PathFinder_hpp */
