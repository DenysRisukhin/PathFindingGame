//
//  Graph.hpp
//  PathFindingDemo01
//
//  Created by Denys Risukhin on 18/10/2017.
//
//

#ifndef Graph_hpp
#define Graph_hpp

class LevelScene;

// The class Graph uses for path representation as a graph
class Graph
{
public:
    
    
    // The class Node - inner class, which keep tile coordinates
    struct Node
    {
        cocos2d::Vec2 _loc;
        
        Node() : _loc(cocos2d::Vec2 {-1, -1}) {}
        Node(cocos2d::Vec2 loc) : _loc(loc) {}
        Node(const Node &rhs) : _loc(rhs._loc) {}
        
        bool operator==(const Node &rhs) const
        {
            return _loc == rhs._loc;
        }
        bool operator!=(const Node &rhs) const
        {
            return _loc != rhs._loc;
        }
        
        std::string getDesc() const
        {
            std::stringstream ss;
            ss << "{" << _loc.x << "," << _loc.y << "}";
            return ss.str();
        }
        
        static const Node INVALID;
    };
    
    // The class PathStepData - inner class, which keep:
    // 1. start and final tile coordinates
    // 2. Tile base cost
    // 3. Compute the cost of moving from a step to an adjacent one
    
    struct PathStepData
    {
        Node _from;
        Node _to;
        
        int _baseCost;
        
        PathStepData() : _from {}, _to {}, _baseCost {0} {}
        PathStepData(cocos2d::Vec2 p0, cocos2d::Vec2 p1, float baseCost) : _from(p0), _to(p1), _baseCost(baseCost) {}
        PathStepData(const PathStepData &rhs) : _from(rhs._from), _to(rhs._to), _baseCost {rhs._baseCost} {}
        
        // Compute the cost of moving from a step to an adjacent one
        int costToMoveFromStep() const
        {
            // Because we can't move diagonally and because rerrian is just walkable or unwalkable the coast is always the same
            // return 1;
            
            // The function getDistance() calculates the distance between two points
            // auto tt = _from._loc.getDistance(_to._loc) + _baseCost;
            //cocos2d::log("(%f %f) - (%f %f) = %f", _from._loc.x, _from._loc.y, _to._loc.x, _to._loc.y, _from._loc.getDistance(_to._loc));
            //return _from._loc.getDistance(_to._loc) + _baseCost;;
            
            // But it have to be different if we can move diagonally and/or if there is swamps, hills, etc...
            // Computing with integers is far more efficient than floats, so insted of using floats to represent the cost of a diagonal move,
            // we can simply multyple the coast by 10 and round them, so moving horizontally ot vertically will cost 10 and diagonally will cost 14.
            int moveCoast = ((_from._loc.x != _to._loc.x) && (_from._loc.y != _to._loc.y)) ? 14 : 10;
            return moveCoast + _baseCost;
        }
        
        std::string getDesc() const
        {
            std::stringstream ss;
            ss << _from.getDesc() << "->" << _to.getDesc();
            return ss.str();
        }
        
        static const PathStepData NONE;
    };
    
    
public:
    static std::shared_ptr<Graph> createWithScene(LevelScene *scene);
    
    // Get all the walkable tiles adjacent
    std::vector<PathStepData> walkableAdjacentTilesCoordForTileCoord(Node &node);
    
private:
    bool init(LevelScene *scene);
    
private:
    LevelScene *_scene;
};

#endif /* Graph_hpp */
