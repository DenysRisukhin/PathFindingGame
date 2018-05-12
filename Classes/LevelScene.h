#ifndef __LevelScene_H__
#define __LevelScene_H__

#include "cocos2d.h"
#include "Graph.hpp"
#include "SimpleAudioEngine.h"


using namespace cocos2d;

class Player;

class LevelScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init() override;
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(LevelScene);
    
    // The method check, if the center of the camera is less than winSize.width/2 or winSize.height/2, part of the view would be off the screen.
    // Important to check the upper bounds as well.
    void setViewPointCenter(Vec2 position);
    
    // The method converts x,y coordinates to “tile coordinates”
    Vec2 tileCoordForPosition(Vec2 position);
    
    // The method converts “tile coordinates” to x,y coordinates
    Vec2 positionForTileCoord(Vec2 tileCoord);
    
    bool isValidTileCoord(Vec2 tileCoord);
    bool isWallAtTileCoord(Vec2 tileCoord);
    bool isKeyAtTilecoord(Vec2 tileCoord);
    bool isObstaclAtTilecoord(Vec2 tileCoord);
    bool isExitAtTilecoord(Vec2 tileCoord);
    
    void showNumKeys(int numKeys);
    
    void removeObjectAtTileCoord(Vec2 tileCoord);
    
    void winGame();
    void loseGame();
    
    void findPath(Vec2 startTileCoord, Vec2 toTileCoord, std::function<void (Graph::PathStepData &conn)> block);
    
    virtual void update(float delta) override;
    
    CocosDenshion::SimpleAudioEngine* audio;
    
private:
    
    // The method determines is tile valid or not valid for move.
    bool hasProperty(std::string name, Vec2 tileCoord, cocos2d::TMXLayer *layer);
    
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    
    void showRestartMenu();
    void restartTapped(cocos2d::Ref *sender);
    void startTapped(cocos2d::Ref *sender);
    void endScene();
    

    TMXTiledMap *map;
    TMXLayer *bgLayer, *objectLayer;
    Player *player;
    
    std::shared_ptr<Graph> _graph;
    
    Label *keysCount;
    
    bool won, gameOver;
    
    Sprite* background;
    Menu* startMenu;
    Label* titleLabel;
    Size winSize;
};


#endif // __LevelScene_H__
