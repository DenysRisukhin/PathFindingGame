#include "LevelScene.h"
#include "Player.hpp"
#include "PathFinder.hpp"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

USING_NS_CC;

Scene* LevelScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LevelScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LevelScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    winSize = Director::getInstance()->getWinSize();
    titleLabel = Label::createWithSystemFont("A* PathfindingDemo", "Arial", 45);
    titleLabel->setColor(Color3B::RED);
    titleLabel->setPosition(winSize.width/2, winSize.height/1.5);
    addChild(titleLabel, 10);
    
    background = Sprite::create("images/background.jpg");
    background->setScale(0.5);
    background->setPosition(winSize.width * .5, winSize.height * .5);
    addChild(background);
    
    auto startLabel = Label::createWithSystemFont("Press Start", "Arial", 45);
    startLabel->setColor(Color3B::BLACK);
    
    auto startItem = MenuItemLabel::create(startLabel, CC_CALLBACK_1(LevelScene::startTapped, this));
    startItem->setPosition(winSize.width/2, winSize.height/2);
    startItem->setScale(.1);
    
    startMenu = Menu::createWithItem(startItem);
    startMenu->setPosition(Vec2::ZERO);
    addChild(startMenu, 10);
    
    startItem->runAction(ScaleTo::create(.5, 1.0));
    
    return true;
}

void LevelScene::startTapped(cocos2d::Ref *sender)
{
    background->setVisible(false);
    startMenu->setVisible(false);
    titleLabel->setVisible(false);
    
    map = TMXTiledMap::create("tileMap/Level_01.tmx");
    map->retain();
    addChild(map);
        
    Vec2 spawnTileCoord = Vec2 {9, 5};
    Vec2 spawnPos = positionForTileCoord(spawnTileCoord);
    setViewPointCenter(spawnPos);
    
    audio = SimpleAudioEngine::getInstance();
    audio->playBackgroundMusic("audio/background.mp3", true);
    audio->preloadEffect("audio/pickup.wav");
    audio->preloadEffect("audio/playerAttack.wav");
    audio->preloadEffect("audio/hitWall.wav");
    audio->preloadEffect("audio/lose.wav");
    audio->preloadEffect("audio/step.wav");
    audio->preloadEffect("audio/win.wav");
    
    bgLayer = map->getLayer("Background");
    objectLayer = map->getLayer("Objects");
    
    auto cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile("images/player1.plist", "images/player1.png");
    
    player = Player::createWithScene(this);
    player->retain();
    player->setPosition(spawnPos);
    map->addChild(player, 5);
    
    winSize = Director::getInstance()->getWinSize();
    keysCount = Label::createWithSystemFont("     0", "Arial", 24);
    keysCount->setPosition(winSize.width * .1, winSize.height * .95);
    addChild(keysCount);
    
    auto key = Sprite::create("images/key.png");
    key->setScale(0.05);
    key->setPosition(winSize.width * .07, winSize.height * .95);
    addChild(key);
    
    // create a graph representation
    _graph = Graph::createWithScene(this);
    
    auto eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(LevelScene::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(LevelScene::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(LevelScene::onTouchEnded, this);
    eventListener->onTouchCancelled = CC_CALLBACK_2(LevelScene::onTouchCancelled, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
    
    scheduleUpdate();
    
    won = false;
    gameOver = false;
}

void LevelScene::setViewPointCenter(Vec2 position)
{
    auto winSize = Director::getInstance()->getWinSize();
    
    float x = MAX(position.x, winSize.width/2);
    float y = MAX(position.y, winSize.height/2);
    x = MIN(x, (map->getMapSize().width * map->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (map->getMapSize().height * map->getTileSize().height) - winSize.height/2);
    
    auto actualPosition = Vec2 {x, y};
    auto centerOfView = Vec2(winSize.width/2, winSize.height/2);
    Vec2 viewPoint = centerOfView - actualPosition;
    
    // move the entire layer
    map->setPosition(viewPoint);
}

Vec2 LevelScene::tileCoordForPosition(cocos2d::Vec2 position)
{
    // Getting the x coordinate – divide it by the width of a tile.
    int x = position.x / map->getTileSize().width;
    
    // Getting the y coordinate - flip things around because in Cocos2D-X (0,0) is at the bottom left, not the top left.
    int y = ((map->getMapSize().height * map->getTileSize().height) - position.y) / map->getTileSize().height;
    return Vec2(x, y);
}


Vec2 LevelScene::positionForTileCoord(cocos2d::Vec2 tileCoord)
{
    int x = (tileCoord.x * map->getTileSize().width) + map->getTileSize().width/2;
    int y = (map->getMapSize().height * map->getTileSize().height) - (tileCoord.y * map->getTileSize().height) - map->getTileSize().height/2;
    return Vec2(x, y);
}

bool LevelScene::isValidTileCoord(cocos2d::Vec2 tileCoord)
{
    if (tileCoord.x < 0 || tileCoord.y < 0 ||
        tileCoord.x >= map->getMapSize().width ||
        tileCoord.y >= map->getMapSize().height) {
        return false;
    } else {
        return true ;
    }
}

bool LevelScene::isWallAtTileCoord(cocos2d::Vec2 tileCoord)
{
    return hasProperty("Wall", tileCoord, bgLayer);
}

bool LevelScene::isKeyAtTilecoord(cocos2d::Vec2 tileCoord)
{
    return hasProperty("Key", tileCoord, objectLayer);
}

bool LevelScene::isObstaclAtTilecoord(cocos2d::Vec2 tileCoord)
{
    return hasProperty("Door", tileCoord, objectLayer);
}

bool LevelScene::isExitAtTilecoord(cocos2d::Vec2 tileCoord)
{
    return hasProperty("Exit", tileCoord, objectLayer);
}

bool LevelScene::hasProperty(std::string name, cocos2d::Vec2 tileCoord, cocos2d::TMXLayer *layer)
{
    if (!isValidTileCoord(tileCoord))
        return false;
    
    // The TileGIDAt function in the meta layer to get the GID (globally unique identifier) at the specified tile coordinate.
    // Then we use the GID to look up properties for that tile.
    // It returns a dictionary of properties, so we look through to see if current propertie is set to true.
    int gid = layer->getTileGIDAt(tileCoord);
    Value properties = map->getPropertiesForGID(gid);
    if (properties.getType() != Value::Type::NONE) {
        auto valueMap = properties.asValueMap();
        auto v = valueMap.find(name);
        return v != valueMap.end() ? true : false;
    }
    return false;
}

void LevelScene::showNumKeys(int numKeys)
{
    CCLOG("showNumKeys(%d)", numKeys);
    keysCount->setString("      " + std::to_string(numKeys));
}

void LevelScene::winGame()
{
    CCLOG("winGame()");
    
    gameOver = true;
    won = true;
    
    SimpleAudioEngine::getInstance()->playEffect("audio/win.wav");
    endScene();
}

void LevelScene::loseGame()
{
    CCLOG("loseGame()");
    
    gameOver = true;
    won = false;
    
    SimpleAudioEngine::getInstance()->playEffect("audio/lose.wav");
    endScene();
}

void LevelScene::findPath(cocos2d::Vec2 startTileCoord, cocos2d::Vec2 toTileCoord, std::function<void (Graph::PathStepData &conn)> block)
{
    PathFinder finder;
    finder.find(_graph.get(), startTileCoord, toTileCoord, block);
}

void LevelScene::removeObjectAtTileCoord(cocos2d::Vec2 tileCoord)
{
    objectLayer->removeTileAt(tileCoord);
}

bool LevelScene::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
    if (gameOver)
        return false;
    
    Vec2 touchLocation = map->convertTouchToNodeSpace(touch);
    
    // calling a method on the player sprite to move the player toward a touch on the tile map
    player->moveToward(touchLocation);
    return true;
}

void LevelScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
}

void LevelScene::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
}

void LevelScene::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
    
}

void LevelScene::update(float delta)
{
    setViewPointCenter(player->getPosition());
}

void LevelScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void LevelScene::endScene()
{
    player->runAction(Sequence::create(
                                     ScaleTo::create(.2, 0),
                                     CallFunc::create([this]() {
        this->showRestartMenu();
    }), nullptr));
}

void LevelScene::showRestartMenu()
{
    std::string message;
    if (won) {
        message = "Level completed";
    } else {
        message = "Game over";
    }
    
    auto label = Label::createWithSystemFont(message, "Arial", 24);
    label->setScale(.1);
    
    auto winSize = Director::getInstance()->getWinSize();
    label->setPosition(winSize.width/2, winSize.height * 0.6);
    addChild(label);
    
    auto restartLabel = Label::createWithSystemFont("Try Again", "Arial", 24);
    
    auto restartItem = MenuItemLabel::create(restartLabel, CC_CALLBACK_1(LevelScene::restartTapped, this));
    restartItem->setPosition(winSize.width/2, winSize.height * 0.4);
    restartItem->setScale(.1);
    
    auto menu = Menu::createWithItem(restartItem);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 10);
    
    restartItem->runAction(ScaleTo::create(.5, 1.0));
    label->runAction(ScaleTo::create(.5, 1.0));
}

void LevelScene::restartTapped(cocos2d::Ref *sender)
{
    auto newScene = LevelScene::createScene();
    Director::getInstance()->replaceScene(TransitionProgressRadialCCW::create(.5, newScene));
}


