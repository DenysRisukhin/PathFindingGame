//
//  Graph.cpp
//  PathFindingDemo01
//
//  Created by Denys Risukhin on 18/10/2017.
//
//

#include "Graph.hpp"
#include "LevelScene.h"

using PathStepData = Graph::PathStepData;

const PathStepData PathStepData::NONE {Vec2{-1,-1}, Vec2{-1,-1}, 0};
const Graph::Node Graph::Node::INVALID {Vec2{-1,-1}};

std::shared_ptr<Graph>
Graph::createWithScene(LevelScene *scene)
{
    // Use make_shared function when possible.
    auto result = std::make_shared<Graph>();
    if (result && result->init(scene))
    {
        return result;
    }
    CC_ASSERT(false);
}

bool Graph::init(LevelScene *scene)
{
    _scene = scene;
    return true;
}

std::vector<PathStepData>
Graph::walkableAdjacentTilesCoordForTileCoord(Node &node)
{
    std::vector<PathStepData> result;
    std::vector<Vec2> toLocs;
    
    // Right
    toLocs.push_back(Vec2 {node._loc.x + 1, node._loc.y} );
    
    // Bottom
    toLocs.push_back(Vec2 {node._loc.x, node._loc.y + 1} );
    
    // Left
    toLocs.push_back(Vec2 {node._loc.x - 1, node._loc.y} );
    
    // Top
    toLocs.push_back(Vec2 {node._loc.x, node._loc.y - 1} );
    
    // diagonal movement
    
    // Bottom Right
    toLocs.push_back(Vec2 {node._loc.x + 1, node._loc.y + 1} );
    
    // Top Right
    toLocs.push_back(Vec2 {node._loc.x + 1, node._loc.y - 1} );
    
    // Bottom Left
    toLocs.push_back(Vec2 {node._loc.x - 1, node._loc.y + 1} );
    
    // Top Left
    toLocs.push_back(Vec2 {node._loc.x - 1, node._loc.y - 1} );
    
    for (Vec2 toLoc : toLocs) {
        // Checking neighbors on the border of the map and the wall
        if (_scene->isValidTileCoord(toLoc) && !_scene->isWallAtTileCoord(toLoc)) {
            
            // Avoid the obstacles at all cost by biasing the base cost.
            // Избегать препятствий путем смещения базовой стоимости
            int baseCost = _scene->isObstaclAtTilecoord(toLoc) ? 100 : 0;
            result.push_back(PathStepData(node._loc, toLoc, baseCost));
        }
    }
    
    return result;
}
