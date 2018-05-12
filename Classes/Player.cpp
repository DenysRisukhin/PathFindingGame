//
//  CatSprite.cpp
//  PathFindingDemo01
//
//  Created by Denys Risukhin on 18/10/2017.
//
//

#include "Player.hpp"
#include "LevelScene.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

Player* Player::createWithScene(LevelScene *scene)
{
    Player *ret = new (std::nothrow) Player();
    if (ret && ret->initWithScene(scene))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool Player::initWithScene(LevelScene *scene)
{
    if (initWithSpriteFrameName("player1_0_0.png")) {
        this->scene = scene;
        _facingForwardAnimation = createCatAnimation("player1_0");
        _facingBackAnimation = createCatAnimation("player1_3");
        _facingLeftAnimation = createCatAnimation("player1_1");
        _facingRightAnimation = createCatAnimation("player1_2");
        numKeys = 0;
        
        // init walking animator
        _walkingAnimator._sprite = this;
        _walkingAnimator.clear();
        
        return true;
    }
    return false;
}

Animation *Player::createCatAnimation(std::string name)
{
    auto animation = Animation::create();
    for (int i = 0; i <= 3; i++) {
        std::stringstream ss;
        ss << name << "_" << i << ".png";
        animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(ss.str()));
    }
    animation->setDelayPerUnit(.2);
    animation->retain();
    return animation;
}

void Player::runAnimation(cocos2d::Animation *animation)
{
    if (_curAnimation == animation)
        return;
    
    _curAnimation = animation;
    
    if (_curAnimation != nullptr) {
        stopAction(_curAnimateAction);
    }
    
    _curAnimateAction = RepeatForever::create(Animate::create(animation));
    runAction(_curAnimateAction);
}


void Player::moveToward(cocos2d::Vec2 target)
{
    if (_walkingAnimator.isBusy()) {
        log("Walking animator is stoped");
        _walkingAnimator.stop();
        return;
    }
    
    //get current tile coordinate and desired tile coord
    auto fromTileCoord = scene->tileCoordForPosition(getPosition());
    auto toTileCoord = scene->tileCoordForPosition(target);
    
    //check that there is a path compute
    if (fromTileCoord == toTileCoord) {
        log("you're already there!");
        return;
    }
    
    // Must check that the desired location is walkable
    if (scene->isWallAtTileCoord(toTileCoord)) {
        SimpleAudioEngine::getInstance()->playEffect("audio/hitWall.wav");
        return;
    }
    
    _walkingAnimator.clear();
    scene->findPath(fromTileCoord, toTileCoord, [this](Graph::PathStepData &conn) {
        _walkingAnimator.pushBack(conn);
    });
    
    _walkingAnimator.runAnimation([this] (PathStepData &conn) {
        
        auto toTileCoord = conn._to._loc;
        if (scene->isWallAtTileCoord(toTileCoord)) {
            SimpleAudioEngine::getInstance()->playEffect("audio/hitWall.wav");
        }
        
        if (scene->isKeyAtTilecoord(toTileCoord)) {
            SimpleAudioEngine::getInstance()->playEffect("audio/pickup.wav");
            numKeys++;
            scene->showNumKeys(numKeys);
            scene->removeObjectAtTileCoord(toTileCoord);
        }
        else if (scene->isObstaclAtTilecoord(toTileCoord)) {
            if (numKeys == 0) {
                scene->loseGame();
                return false;
            } else {
                numKeys--;
                scene->showNumKeys(numKeys);
                scene->removeObjectAtTileCoord(toTileCoord);
                SimpleAudioEngine::getInstance()->playEffect("audio/playerAttack.wav");
            }
        }
        else if (scene->isExitAtTilecoord(toTileCoord)) {
            scene->winGame();
        } else {
            SimpleAudioEngine::getInstance()->playEffect("audio/step.wav");
        }
        
        return true;
    });
}

void Player::WalkingAnimator::runAnimation(std::function<bool (PathStepData &conn)> block)
{
    if (_stop) {
        CCLOG("Animator has been interrupted. Stopping...");
        return;
    }
    
    // This instruction is used for correct animation run after player finish path and stop animation activated
    _sprite->resume();
    
    auto conn = _conns.front();
    _conns.pop_front();
    
    // Update game logics for the current PathStepData.
    if (!block(conn)) {
        return;
    }
    
    auto delta = conn._to._loc - conn._from._loc;
    if (delta.x > 0) {
        _sprite->runAnimation(_sprite->_facingRightAnimation);
    }
    else if (delta.x < 0) {
        _sprite->runAnimation(_sprite->_facingLeftAnimation);
    }
    
    if (delta.y > 0) {
        _sprite->runAnimation(_sprite->_facingForwardAnimation);
    }
    else if (delta.y < 0) {
        _sprite->runAnimation(_sprite->_facingBackAnimation);
    }
    
    Vec2 toLoc = conn._to._loc;
    auto move = MoveTo::create(.2, _sprite->scene->positionForTileCoord(conn._to._loc));
    
    Vector<FiniteTimeAction *> seq;
    seq.pushBack(move);
    
    if (_conns.size() > 0) {
        // CallFunc - creates the action with the callback of type std::function<void()>.
        // This is the preferred way to create the callback.
        seq.pushBack(CallFunc::create([this, block]() {
            this->runAnimation(block);
        }));
    }

    log("_conns.size() = %lu", _conns.size());
    log("seq.size() = %zi", seq.size());
    
    // 1. Sprite will done his move
    // 2. Recursion call for current method
    _sprite->runAction(Sequence::create(seq));

    // The player animation stoped, after he finish his path
    if(_conns.size() == 0){
        
        auto stopAnimation = DelayTime::create(0.2);
        CallFunc* stopPlayerAnimation = CallFunc::create([&] (){
            _sprite->pause();
        });
    _sprite->runAction(Sequence::create(stopAnimation,stopPlayerAnimation,nullptr));
    }
}
