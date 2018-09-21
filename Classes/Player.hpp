//
//  Player.hpp
//  PathFindingDemo01
//
//  Created by Denys Risukhin on 18/10/2017.
//
//

#ifndef Player_hpp
#define Player_hpp

#include "Graph.hpp"
#include "LevelScene.h"

/*!
 * Represents main character.
 */
class Player : public cocos2d::Sprite
{
    
public:
    static Player* createWithScene(LevelScene *scene);
    
    void moveToward(cocos2d::Vec2 target);
    
private:
    using PathStepData = Graph::PathStepData;
    
    bool initWithScene(LevelScene *scene);
    
    // The Animation object contains AnimationFrame objects, and a possible delay between the frames.
    // You can animate a Animation object by using the Animate action.
    static cocos2d::Animation *createCatAnimation(std::string name);
    
    void runAnimation(cocos2d::Animation *animation);
    
    LevelScene *scene;
    
    cocos2d::Animation *_facingForwardAnimation,
    *_facingBackAnimation,
    *_facingLeftAnimation,
    *_facingRightAnimation,
    *_curAnimation;
    
    cocos2d::ActionInterval *_curAnimateAction;

    int numKeys;

    struct WalkingAnimator
    {
        Player *_sprite;
        std::list<PathStepData> _conns;
        bool _stop;
                
        void runAnimation(std::function<bool (PathStepData &conn)> block);
        void pushBack(PathStepData &conn) { _conns.push_back(conn); }
        void clear() {
            _conns.clear();
            _stop = false;
        }
        bool isBusy() { return _conns.size() > 0 && !_stop; }
        void stop() { _stop = true; }
    };
    
    WalkingAnimator _walkingAnimator;

};

#endif /* Player_hpp */
