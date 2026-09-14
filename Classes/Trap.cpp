//
//  Trap.cpp
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 7. 15..
//
//

#include "Trap.h"

using namespace cocos2d;


// on "init" you need to initialize your instance
bool Trap::init()
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(! Sprite::init());
		
		
        bRet = true;
    } while (0);
	
    
    return bRet;
}
Trap* Trap::createTrap(const char* normalSprite, const char* runAnimation, float delay, float duration, int type)
{
    Trap* trap = Trap::create();
    trap->initWithSpriteFrameName(normalSprite);
    trap->_stopFrameName = normalSprite;
    trap->_animationName = runAnimation;
    trap->schedule(schedule_selector(Trap::running), duration, kRepeatForever, delay);
    trap->trapType = type;
    return trap;
}

void Trap::startAnimation()
{
    _isHurting = true;
    Animation* animation = AnimationCache::getInstance()->getAnimation(_animationName);
    Animate* animate = CCAnimate::create(animation);
    this->runAction(RepeatForever::create(animate));
}
void Trap::stopAnimation()
{
    _isHurting = false;
    this->setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(_stopFrameName));
    this->stopAllActions();
}
void Trap::running(float dt)
{
    _isHurting = !_isHurting;
    if (_isHurting) {
        startAnimation();
    }else{
        stopAnimation();
    }
}

Mace* Mace::createMace(const char* stringName, const char* headName, int theExtraAngle, Point anchor, Point headPoint)
{
    Mace* mace = new Mace();
    mace->initWithSpriteFrameName(stringName);
//    mace->autorelease();
    mace->head = Sprite::createWithSpriteFrameName(headName);
    mace->addChild(mace->head);
    mace->setAnchorPoint(anchor);
    mace->head->setPosition(headPoint);
    float duration = 0.07f;

    mace->runAction(RepeatForever::create(Sequence::create(RotateTo::create(duration, -7 + theExtraAngle),
                                                           RotateTo::create(duration, -14 + theExtraAngle),
                                                           RotateTo::create(duration, -21 + theExtraAngle),
                                                           RotateTo::create(duration, -27 + theExtraAngle),
                                                           RotateTo::create(duration, -32 + theExtraAngle),
                                                           RotateTo::create(duration, -36 + theExtraAngle),
                                                           RotateTo::create(duration, -39 + theExtraAngle),
                                                           RotateTo::create(duration, -41 + theExtraAngle),
                                                           RotateTo::create(duration, -42.5 + theExtraAngle),
                                                           RotateTo::create(duration, -44 + theExtraAngle),
                                                           RotateTo::create(duration, -45 + theExtraAngle),
                                                           RotateTo::create(0.1, -45 + theExtraAngle),
                                                           RotateTo::create(duration, -44 + theExtraAngle),
                                                           RotateTo::create(duration, -42.5 + theExtraAngle),
                                                           RotateTo::create(duration, -41 + theExtraAngle),
                                                           RotateTo::create(duration, -39 + theExtraAngle),
                                                           RotateTo::create(duration, -36 + theExtraAngle),
                                                           RotateTo::create(duration, -32 + theExtraAngle),
                                                           RotateTo::create(duration, -27 + theExtraAngle),
                                                           RotateTo::create(duration, -21 + theExtraAngle),
                                                           RotateTo::create(duration, -14 + theExtraAngle),
                                                           RotateTo::create(duration, -7 + theExtraAngle),
                                                           RotateTo::create(duration, -0 + theExtraAngle),
                                                           
                                                           RotateTo::create(duration, 7 + theExtraAngle),
                                                           RotateTo::create(duration, 14 + theExtraAngle),
                                                           RotateTo::create(duration, 21 + theExtraAngle),
                                                           RotateTo::create(duration, 27 + theExtraAngle),
                                                           RotateTo::create(duration, 32 + theExtraAngle),
                                                           RotateTo::create(duration, 36 + theExtraAngle),
                                                           RotateTo::create(duration, 39 + theExtraAngle),
                                                           RotateTo::create(duration, 41 + theExtraAngle),
                                                           RotateTo::create(duration, 42.5 + theExtraAngle),
                                                           RotateTo::create(duration, 44 + theExtraAngle),
                                                           RotateTo::create(duration, 45 + theExtraAngle),
                                                           RotateTo::create(0.1, 45 + theExtraAngle),
                                                           RotateTo::create(duration, 44 + theExtraAngle),
                                                           RotateTo::create(duration, 42.5 + theExtraAngle),
                                                           RotateTo::create(duration, 41 + theExtraAngle),
                                                           RotateTo::create(duration, 39 + theExtraAngle),
                                                           RotateTo::create(duration, 36 + theExtraAngle),
                                                           RotateTo::create(duration, 32 + theExtraAngle),
                                                           RotateTo::create(duration, 27 + theExtraAngle),
                                                           RotateTo::create(duration, 21 + theExtraAngle),
                                                           RotateTo::create(duration, 14 + theExtraAngle),
                                                           RotateTo::create(duration, 7 + theExtraAngle),
                                                           RotateTo::create(duration, 0 + theExtraAngle),
                                                           NULL)));
                                                                          
    return mace;
}

FireStick* FireStick::createFireStick(const char* name, bool clockWise, int fireCount)
{
    FireStick* fireStick = new FireStick();
    fireStick->initWithSpriteFrameName(name);
    fireStick->runAction(RepeatForever::create(RotateBy::create(0.8, clockWise?360:-360)));
    for (int i = 0; i < fireCount - 1 ; i++) {
        Sprite* fire = Sprite::createWithSpriteFrameName(name);
        fire->runAction(RepeatForever::create(RotateBy::create(0.8, clockWise?360:-360)));
        fireStick->fireArray.pushBack(fire);
    }
    fireStick->isClockWise = clockWise;
    return fireStick;
}

DestructablePlatform* DestructablePlatform::createDestructablePlatform(const char* name, float fallTime, Point pos, TMXLayer* parentLayer)
{
    DestructablePlatform* platform = new DestructablePlatform();
    platform->initWithSpriteFrameName(name);
    platform->layer = parentLayer;
    platform->fallingStartTime = fallTime;
    platform->isFallingStarted = false;
    platform->tilePos = pos;
    return platform;
}
