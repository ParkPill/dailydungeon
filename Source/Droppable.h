//
//  Droppable.h
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 26..
//
//

#ifndef AlmostLegendary_Droppable_h
#define AlmostLegendary_Droppable_h

#include "cocos2d.h"
#include "MovingPlatform.h"
#include "ui/UIImageView.h"
#define CLIFF_NONE 0
#define CLIFF_LEFT 1
#define CLIFF_RIGHT 2
#define TILE_SIZE 16
#define SECOND_TAG_NULL -99372
using namespace cocos2d;

class Droppable : public cocos2d::Sprite
{
private:
    
public:
    int unitType;
    float waterAccelX ;
    float waterMaxX ;
    float waterAccelYN;
    float waterAccelYP;
    float waterMinY;
    bool doubleJump;
    Droppable();
    ~Droppable(){}

    cocos2d::Point centerPosition;
    cocos2d::Point velocity;
    cocos2d::Point desiredPosition;
    cocos2d::Rect demageRect;
    Droppable* target;
    MovingPlatform* currentPlatform;
    
    virtual void setFlippedX(bool flip);
    Vector<Droppable*> victimArray;
    bool isPiercing = false;
    float DP = 0;
    float oneWayCancelY;
    float energy;
    float maxEnergy = 0;
    int ySpeed;
    int xSpeed;
    int state;
    bool freezed;
    bool fixAngleToTargetWhenReleaseFreezing=false;
    void freezeForAWhile(float dur);
    void releaseFreezing();
    int extraSpeed;
    int extraJump;
    int effectType;
    int movingAngle;
    float weight;
    

    int goldPossiblity;
    float fireGuage;
    float blinkingTime;
    bool isMissile;
    bool isTowardLeft;
    bool exitingVehicle;
    bool inWater;
    bool untouchable;
    bool immortal;
    bool wantsToJump;
    bool wantsToGoLeft;
    bool wantsToGoRight;
    bool wantsToStop;
    void goLeft();
    void goRight();
    void stop();
    int monitoringDistance;
    bool isFregile;
    bool ignoreGravity;
    
    bool onGround;
//    bool encounteredWall;
//    bool encounteredCliff;
    bool jumped;
    bool tumbled;
    virtual void tumbleAnimation();
    bool iWasInWater;
    bool isDropping = false;
//    void initialize();
    void turnIntoEnemy();
    virtual void setInWater(bool inOrOut);
    virtual void updatePosition(float dt);
    void updatePositionForStraight(float dt);
    void updatePositionForBouncing(float dt);
    void updatePositionToFlyToFollowTarget(float dt);
    virtual float getCurrentY();
    void setTarget(Droppable* t);
    bool isTargetFound = false;
    float findTarget(Droppable* tar, float dist);
    const char* animationName;
    bool isAnimationRepeat;
    void runAnimationLater(float dt);
    void runAnimation(const char* name, bool repeat);
    void runAnimation(const char* name, bool repeat, float delay);
    void runFireAnimation();
    void fireAnimationDone(Node* nd);
    virtual void onLanded();
    virtual void onJumped();
    virtual void onDropping();
    bool isHurting = true;
    void dontHurt();
    void jumpAnimation();
    bool jumpAllowed;
    void banJump(float dt);
    void setOnGround(bool ground);
    bool buttonReleased;
    int secondTag=SECOND_TAG_NULL;
    std::string talk;
    void showTalk(std::string text, float time, float y, float delay);
    void updateTalkBalloon(float dt);
    float talkBalloonY;
    cocos2d::ui::ImageView* imgTalkBalloon;
    Label* lblTalk;
    float talkVisibleTime=2;
    float talkVisibleTimeElapsed=0;
    virtual void onDead();
    virtual void onHit();
    virtual void encounteredWall();
    virtual void encounteredCliff(int direction);
    virtual cocos2d::Rect damageBoundingBox();
    virtual cocos2d::Rect collisionBoundingBox();
    virtual cocos2d::Rect collectBoundingBox();

    cocos2d::Rect RectOffset(cocos2d::Rect rect, float x, float y);
    cocos2d::Rect RectInset(cocos2d::Rect rect, float x, float y);
    
    void init(int unit, int eng, float extraSpd, const char* sptName);
    static Droppable* createDroppable(int unit, int eng, float extraSpd, const char* sptName);
    static Droppable* createDroppable(int unit, int eng, float extraSpd, const char* sptName, const char* animationName);
    static Droppable* createDroppable(int unit, int eng, float extraSpd, const char* sptName, const char* animationName, bool repeat);

    virtual cocos2d::Point getGravityPosition();
    void blinkForAWhile();
    void blinking(float dt);
    virtual void runSchedule();
    bool energyBarExist = true;
};

#endif
