//
//  Hero.h
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 14. 7. 9..
//
//

#ifndef __LegendDaryTwo__Hero__
#define __LegendDaryTwo__Hero__

#include "cocos2d.h"
#include "Droppable.h"
#include "Enemies.h"

#define TAG_NODDING 78
#define HERO_DARY 0
#define HERO_AMOR 1

using namespace cocos2d;

class Hero : public EnemyBase
{
public:
    bool immortalByTumble=false;
    cocos2d::Point leftArmFlipPos;
    cocos2d::Point rightArmFlipPos;
    cocos2d::Point faceFlipPos;
    cocos2d::Point gunFlipPos;
    cocos2d::Point leftArmPos;
    cocos2d::Point rightArmPos;
    cocos2d::Point leftLegPos;
    cocos2d::Point rightLegPos;
    cocos2d::Point facePos;
    cocos2d::Point gunPos;
    NPCBase* encounteredNpc=nullptr;
    int heroType = HERO_AMOR;
    Sprite* leftArm;
    Sprite* rightArm;
    Sprite* leftLeg;
    Sprite* rightLeg;
    Sprite* body;
    Sprite* face;
    Sprite* hand;
    Vec2 handPos;
    Vec2 flippedHandPos;
    Sprite* gun;
    Sprite* vehicle;
    Sprite* appleWorm;
    Sprite* sptDropEffect;
    float originalExtraSpeed;
    Sprite* sptTalkBalloon = nullptr;
    void setFeelingVisible(bool visible, int feeling);
    cocos2d::Point bodyPoint;
    cocos2d::Point bodyPointFlip;
    int vehicleType;
    cocos2d::Point center;
    bool tumbled;
    int weapon;
    float weaponPower;
    int weaponCriticalPossibility;
    bool isHatUp;
    void hatCheck(float dt);
    virtual void setFlippedX(bool flip);
    virtual void updatePosition(float dt);
    virtual float getCurrentY();
    virtual cocos2d::Rect collisionBoundingBox();
    virtual cocos2d::Rect damageBoundingBox();
    virtual cocos2d::Point getGravityPosition();
    void spriteMoveDone(Ref* obj);
    void getInTheVehicle(int vType, Sprite* vhc);
    void getOutTheVehicle();
    void updatePositionForSuperBoard(float dt, bool jump, bool left, bool right);
    void setRestOfBodyPart();
    void setGun(int gunType);
    void setGunPos(int angle);
    virtual void tumbleAnimation();
    void runHatCheck();
    
    float lastAngle = 88;
    void setGunPosHandler();
    static Hero* createHero(int unit, int eng, float extraSpd);
    void runFireAnimation();
    void setAppleWorm();
    void appleWormBlink(float dt);
    void appleWormDone(float dt);
    void addDirt();
    void addLandingDirt();
    void addJumpingDirt();
    bool isFootStepStarted;
    void startFootStep();
    void stopFootStep();
    void stepSoundOnce(float dt);
    void stepSound(float dt);
    virtual void onJumped();
    virtual void onLanded();
    virtual void onDropping();
    virtual void setInWater(bool inOrOut);
    void startWalkingAnimation();
    void stopAnimationAndStand();
    void stopAndStand();
    void goLeft();
    void goRight();
    void jump();
    void setWeapon(int weaponType, float power, int criPossibility);
    void setColorTo(Color3B color);
    Vec2 msStartPos;
};
#endif /* defined(__LegendDaryTwo__Hero__) */
