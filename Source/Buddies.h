//
//  Buddies.h
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 14. 7. 15..
//
//

#ifndef __LegendDaryTwo__Buddies__
#define __LegendDaryTwo__Buddies__

#include "cocos2d.h"
#include "Droppable.h"
#include "Enemies.h"
using namespace cocos2d;

class Buddy : public Droppable
{
    
public:
    virtual void movingSchedule(float dt);
    virtual void runSchedule();
    virtual void fireSchedule(float dt);
    Buddy();
    ~Buddy();
    
    bool isEncounteredCliff;
    bool isEncounteredWall;
    Missile* missile;
    float missileAngle;
    
    int moveType;
    int petNumber;
    PointArray* positionArray;
    Buddy* behinder;
    Droppable* master;
    float fireCoolTime;
    float fireCoolTimeMax;
    float isReadyToFire;
    virtual void fire();
//    void initBuddy(int unit, int eng, int buddyType, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int monitorDistance);

    virtual void encounteredWall();
    virtual void encounteredCliff();
    
    void initBuddy(int unit, int eng, int buddyType, const char* sptName, int monitorDistance);
    void setPositionForBoth(cocos2d::Point pos);
    void walkToTarget(Droppable* tar, float distance);
};


///////////////////////////////////////////////
// Fox
///////////////////////////////////////////////
class GroundPet : public Buddy
{
    
public:
    virtual void movingSchedule(float dt);
    virtual void runSchedule();
    virtual void fireSchedule(float dt);
    static GroundPet* create(int unit, int eng, int buddyType, const char* sptName, int monitorDistance);
    virtual void fire();
};

///////////////////////////////////////////////
// SkyPet
///////////////////////////////////////////////
class SkyPet : public Buddy
{
    
public:
    virtual void setFlippedX(bool flip);
    virtual void movingSchedule(float dt);
    virtual void runSchedule();
    virtual void fireSchedule(float dt);
    static SkyPet* create(int unit, int eng, int buddyType, const char* sptName, int monitorDistance);
    virtual void fire();
};


#endif /* defined(__LegendDaryTwo__Buddies__) */
