#ifndef __TRAP_H__
#define __TRAP_H__

#include "cocos2d.h"

using namespace cocos2d;
class Trap : public Sprite
{
private:
    
public:
	bool _isHurting;
    int trapType;
    const char* _stopFrameName;
    const char* _animationName;
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
	
    // implement the "static node()" method manually
    CREATE_FUNC(Trap);
    static Trap* createTrap(const char* normalSprite, const char* runAnimation, float delay, float duration, int type);
    
	void startAnimation();
	void stopAnimation();
    void running(float dt);
};


class Mace : public Sprite
{
private:
    
public:
    static Mace* createMace(const char* stringName, const char* headName, int theExtraAngle, cocos2d::Point anchor, cocos2d::Point headPoint);
    int extraAngle;
    Sprite* head;
};


class FireStick : public Sprite
{
private:
    
public:
    bool isClockWise;
    float currentAngle;
    int stepAngle;
    static FireStick* createFireStick(const char* name, bool clockWise, int fireCount);
    Vector<Sprite*> fireArray;
};


class DestructablePlatform : public Sprite
{
private:
    
public:
    TMXLayer* layer;
    cocos2d::Point tilePos;
    cocos2d::Point originalPos;
    bool isFallingStarted;
    float fallingStartTime;
    static DestructablePlatform* createDestructablePlatform(const char* name, float fallTime, cocos2d::Point pos, TMXLayer* parentLayer);
};


#endif  // __TRAP_H__