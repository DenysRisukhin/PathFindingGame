//
//  PathFinder.cpp
//  PathFindingDemo01
//
//  Created by Denys Risukhin on 18/10/2017.
//
//

#include "PathFinder.hpp"

using namespace cocos2d;

int PathFinder::find(Graph *g, Node start, Node end, function<void (PathStepData &conn)> block)
{
    return find(g, start, end, ManhattanDistance(end), block);
}

int PathFinder::find(Graph *g, Node start, Node end, std::function<int(Node &)> heuristic, std::function<void (PathStepData &conn)> block)
{
    log("start %s -> end %s", start.getDesc().c_str(), end.getDesc().c_str());
    
    ShortestPathStep startStep(start, PathStepData::NONE, 0, heuristic(start));
    
    // Start by adding the from position to the open list
    openSteps.push(startStep);
    
    ShortestPathStep currentStep;
    
    while (openSteps.size() > 0)
    {
        // Get the lowest F cost. The list is ordered and first step is always the one with the lowest F coast
        currentStep = openSteps.lowestTrace();
        
        log("currentStep %s", currentStep.getDesc().c_str());
        
        if (currentStep._node == end) {
            log("found goal node: %s", end.getDesc().c_str() );
            break;
        }
        
        auto walkableAdjacents = g->walkableAdjacentTilesCoordForTileCoord(currentStep._node);
        
        // In cycle we work with each neighbors from "connections" list
        for (auto &walkableAdjacent : walkableAdjacents) {
            
            // Get cordinates for current neighbor
            auto toNode = walkableAdjacent._to;
            
            // Compute the cost from the current step to current neighbor
            auto toNodeCost = currentStep.gCost + walkableAdjacent.costToMoveFromStep();
            
            log("toNode %s toNodeCost %d", toNode.getDesc().c_str(), toNodeCost);
            
            int toNodeHeuristic;
            ShortestPathStep toNodeStep;
            
            if (closedSteps.contains(toNode)) {
                
                log("toNode is in CLOSED list");
                
                auto iter = closedSteps.find(toNode);
                toNodeStep = *iter;
                
                log("toNodeTrace %s", toNodeStep.getDesc().c_str());
                
                if (toNodeStep.gCost <= toNodeCost) {
                    log("ignoring toNode because costSoFar %d is lesser than toNodeCost %d", toNodeStep.gCost, toNodeCost);
                    continue;
                }
                
                closedSteps.remove(toNode);
                
                toNodeHeuristic = toNodeStep.fCost - toNodeStep.gCost;
            }
            else if (openSteps.contains(toNode)) {
                
                log("toNode is in OPEN list");
                
                auto iter = openSteps.find(toNode);
                toNodeStep = *iter;
                
              
                CCLOG("toNodeStep %s", toNodeStep.getDesc().c_str());
                
                if (toNodeStep.gCost <= toNodeCost) {
                    log("ignoring toNode because costSoFar %d is lesser than toNodeCost %d", toNodeStep.gCost, toNodeCost);
                    continue;
                }
                
                toNodeHeuristic = toNodeStep.fCost - toNodeStep.gCost;
            }
            else {
                log("toNode is new");
                
                toNodeStep._node = toNode;
                toNodeHeuristic = heuristic(toNode);
            }
            
            
            toNodeStep.gCost = toNodeCost;
            toNodeStep._pathStepData = walkableAdjacent;
            toNodeStep.fCost = toNodeCost + toNodeHeuristic;
            
            log("final toNodeStep %s toNodeHeuristic %d", toNodeStep.getDesc().c_str(), toNodeHeuristic);
            
            if (!openSteps.contains(toNode)) {
                log("inserting step to OPEN list");
                openSteps.push(toNodeStep);
            }
            
        }
        
        // Remove currentStep from the open list
        openSteps.remove(currentStep._node);
        closedSteps.push(currentStep);
    }
    
    if (currentStep._node != end) {
        log("couldn't find goal node on the paths: %s", end.getDesc().c_str() );
        return -1;
    }
    
    std::stack<PathStepData> stack;
    auto currentNode = currentStep._node;
    while (currentNode != start) {
        //log("reverse order %s", currentStep._connection.getDesc().c_str());
        stack.push(currentStep._pathStepData);
        currentNode = currentStep._pathStepData._from;
        auto iter = closedSteps.find(currentNode);
        currentStep = *iter;
    }
    
    while (stack.size()) {
        auto conn = stack.top();
        //log("correct order %s", conn.getDesc().c_str());
        block(conn); // lambda function call for "_walkingAnimator.pushBack(conn);"
        stack.pop();
    }
    
    return 0;
}
