//
//  Enemies.h
//  LegendDaryTheTower
//
//  Created by SUNG PILL PACK on 14. 6. 13..
//
//

#ifndef __LegendDaryTheTower__Enemies__
#define __LegendDaryTheTower__Enemies__


#include "cocos2d.h"
#include "Droppable.h"
#include "ui/UIHelper.h"
#include "editor-support/cocostudio/CCArmature.h"
#include "editor-support/cocostudio/CCArmatureDataManager.h"
#include "editor-support/cocostudio/CCArmatureDefine.h"
#include "editor-support/cocostudio/CCDataReaderHelper.h"
#include "editor-support/cocostudio/CCDatas.h"
#include "editor-support/cocostudio/CCSkin.h"
#include "editor-support/cocostudio/CocoStudio.h"


using namespace cocos2d;
#define ENEMY_GUN_TYPE_NONE 0
#define ENEMY_GUN_TYPE_HANDGUN 1
#define ENEMY_GUN_TYPE_RIFLE 2
#define ENEMY_GUN_TYPE_WAND 3

#define SELLING_ITEM_WEAPON 0
#define SELLING_ITEM_HEART -1
#define SELLING_ITEM_AMMO -2
#define SELLING_ITEM_KEY -3
#define SELLING_ITEM_DRAGON_BABY -4
#define SELLING_ITEM_DRAGON_ADOLESCENT -5
#define SELLING_ITEM_DRAGON_ADULT -6
#define SELLING_ITEM_DRAGON_SENIOR -7
#define SELLING_ITEM_ -9
///////////////////////////
// EnemyBase
///////////////////////////
class EnemyBase : public Droppable
{
private:
    
public:
    virtual void resumeWalking(float dt);
    virtual void movingSchedule(float dt);
    EnemyBase();
    ~EnemyBase();
    void stopSchedule();
    int orderAction;
    int currentAction;
    
    int currentMoveType;
    void setCurrentMoveType(int move);
    int moveTypeBeforeFindHero;
    int moveTypeAfterFindHero;
    int actionTypeBeforeFindHero;
    int actionTypeAfterFindHero;
    
    std::string fireAniName="";
    cocos2d::Point wanderStart;
    cocos2d::Point wanderEnd;
    Sprite* energyBar = nullptr;
    Sprite* energyBarContent = nullptr;
    
    void showEnergy();
    void hideEnergy();
    
    int rewards = 1;
//    int effectType;
    void initEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int actionBeforeFindHero, int actionAfterFindHero, int monitorDistance);
    virtual void runSchedule();

    
    static EnemyBase* createEnemy(int unit, int eng, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity);
    static EnemyBase* createEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    static EnemyBase* createEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int actionBeforeFindHero, int actionAfterFindHero, int monitorDistance);
};

class Missile : public EnemyBase
{
public:
    Vector<Droppable*> victimArray;
    bool isPiercing = false;
    int parentWeaponIndex;
    float criticalPossibility;
    static Missile* createMissile(int unit, int eng, int angle, float extraSpd, const char* sptName);
    static Missile* createMissile(int unit, int eng, int angle, float extraSpd, const char* sptName, bool destructable);
};
///////////////////////////
// FireableBase
///////////////////////////
class FireableBase : public EnemyBase
{
private:
    
public:
    Sprite* sptGun=nullptr;
    FireableBase();
    ~FireableBase();
    static FireableBase* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    
    float fireCoolTime;
    float fireCoolTimeMax;
    Vector<Missile*> missileArray;
    float missileAngle;
};
///////////////////////////
// WalkableOnPlatformBase
///////////////////////////
class WalkableOnPlatformBase : public EnemyBase
{
private:
    
public:
    WalkableOnPlatformBase();
    ~WalkableOnPlatformBase();
    static WalkableOnPlatformBase* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    virtual void encounteredWall();
    virtual void encounteredCliff(int direction);
};

///////////////////////////
// WalkableCrossPlatformBase
///////////////////////////
class WalkableCrossPlatformBase : public EnemyBase
{
private:
    
public:
    WalkableCrossPlatformBase();
    ~WalkableCrossPlatformBase();
    static WalkableCrossPlatformBase* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    virtual void encounteredWall();
};
///////////////////////////
// FireableOnPlatformBase
///////////////////////////
class FireableOnPlatformBase : public FireableBase
{
private:
    
public:
    FireableOnPlatformBase();
    ~FireableOnPlatformBase();
    static FireableOnPlatformBase* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    virtual void encounteredWall();
    virtual void encounteredCliff(int direction);
};

///////////////////////////
// FireableCrossPlatformBase
///////////////////////////
class FireableCrossPlatformBase : public FireableBase
{
private:
    
public:
    FireableCrossPlatformBase();
    ~FireableCrossPlatformBase();
    static FireableCrossPlatformBase* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    virtual void encounteredWall();
};

///////////////////////////
// ThornDrop
///////////////////////////
class ThornDrop : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static ThornDrop* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    virtual void onLanded();
};


///////////////////////////
// BlueWorm
///////////////////////////
class BlueWorm : public WalkableOnPlatformBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static BlueWorm* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};
///////////////////////////
// HotDog
///////////////////////////
class HotDog : public EnemyBase
{
private:
    virtual void movingSchedule(float dt);
public:
    float jumpCoolTime;
    float jumpCoolTimeMax;
    virtual void runSchedule();
    static HotDog* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    
    virtual void onLanded();
};
///////////////////////////
// HotDogGun/Octopus
///////////////////////////
class HotDogGun : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
    void missileSchedule(float dt);
public:
    const char* aniName;
    float jumpCoolTime;
    float jumpCoolTimeMax;
    virtual void runSchedule();
    static HotDogGun* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    
    virtual void onLanded();
};

///////////////////////////
// AppleMon
///////////////////////////
class AppleMon : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static AppleMon* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    void setPositionForWander(cocos2d::Point pos);
    cocos2d::Point leftPos;
    cocos2d::Point rightPos;
    void resumeAction(float dt);
    void turnaround(Ref* ref);
};
///////////////////////////
// FlyingPenguin
///////////////////////////
class FlyingPenguin : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static FlyingPenguin* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    void setPositionForWander(cocos2d::Point pos);
    cocos2d::Point leftPos;
    cocos2d::Point rightPos;
    void resumeAction(float dt);
    void turnaround(Ref* ref);
};
///////////////////////////
// Ghost
///////////////////////////
class Ghost : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static Ghost* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    void setPositionForWander(cocos2d::Point pos);
    void resumeAction(float dt);
    void turnaround(Ref* ref);
};
///////////////////////////
// Bat
///////////////////////////
class Bat : public EnemyBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static Bat* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    void setPositionForWander(cocos2d::Point pos);
    void resumeAction(float dt);
};
///////////////////////////
// OddEye/JellyFish
///////////////////////////
class OddEye : public EnemyBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static OddEye* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    void setPositionForWander(cocos2d::Point pos);
    void resumeAction(float dt);
    void turnaround(Ref* ref);
};

///////////////////////////
// OddEyeRide
///////////////////////////
class OddEyeRide : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    void animateAgain();
    static OddEyeRide* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    void setPositionForWander(cocos2d::Point pos);
    void resumeAction(float dt);
    void turnaround(Ref* ref);
};

///////////////////////////
// Wood
///////////////////////////
class Wood : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static Wood* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};
///////////////////////////
// BeeHive
///////////////////////////
class BeeHive : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static BeeHive* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};

///////////////////////////
// Pumpkin
///////////////////////////
class Pumpkin : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static Pumpkin* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};

///////////////////////////
// DungBeetle
///////////////////////////
class DungBeetle : public FireableOnPlatformBase
{
private:
    virtual void movingSchedule(float dt);
    Droppable* dung;
public:
    virtual void runSchedule();
    static DungBeetle* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    virtual void encounteredWall();
    virtual void encounteredCliff(int direction);
};

///////////////////////////
// PinkRakoon
///////////////////////////
class PinkRakoon : public WalkableCrossPlatformBase
{
private:
    bool targetFound;
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static PinkRakoon* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};
///////////////////////////
// FishMon
///////////////////////////
class FishMon : public FireableBase
{
private:
    float shootCoolTime;
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static FishMon* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};


///////////////////////////
// SquirrelStand
///////////////////////////
class SquirrelStand : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static SquirrelStand* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};

///////////////////////////
// SkeletonWizard
///////////////////////////
class SkeletonWizard : public FireableOnPlatformBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static SkeletonWizard* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    int attackIndex;
};

///////////////////////////
// BluePot
///////////////////////////
class BluePot : public FireableOnPlatformBase
{
private:
    int attackIndex;
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static BluePot* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};

///////////////////////////
// GreenWorm
///////////////////////////
class GreenWorm : public FireableOnPlatformBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static GreenWorm* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};

///////////////////////////
// DrumBomb
///////////////////////////
class DrumBomb : public FireableOnPlatformBase
{
private:
    virtual void movingSchedule(float dt);
public:
    void explosionDone(Ref* obj);
    virtual void runSchedule();
    static DrumBomb* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};

///////////////////////////
// BoardOwl
///////////////////////////
class BoardOwl : public FireableOnPlatformBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static BoardOwl* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    int attackIndex;
};



///////////////////////////
// LeapWorm
///////////////////////////
class LeapWorm : public BlueWorm
{
private:
public:
    static LeapWorm* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};

///////////////////////////
// TreeMan
///////////////////////////
class TreeMan : public FireableOnPlatformBase
{
private:
    virtual void movingSchedule(float dt);
    float originalSpeed;
    void turnIntoNormal(float dt);
public:
    virtual void runSchedule();
    static TreeMan* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};
///////////////////////////
// WalkingPenguin
///////////////////////////
class WalkingPenguin : public BlueWorm
{
private:
public:
    static WalkingPenguin* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};

///////////////////////////
// ForkPorkStand
///////////////////////////
class ForkPorkStand : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static ForkPorkStand* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};

///////////////////////////
// ForkPork
///////////////////////////
class ForkPork : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    int attackIndex;
    virtual void runSchedule();
    static ForkPork* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    virtual void onLanded();
    virtual void encounteredWall();
};
///////////////////////////
// Squirrel
///////////////////////////
class Squirrel : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    int attackIndex;
    virtual void runSchedule();
    static Squirrel* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    virtual void onLanded();
    virtual void encounteredWall();
};
///////////////////////////
// ThreeEyes
///////////////////////////
class ThreeEyes : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    int attackIndex;
    void fireLater(float dt);
    virtual void runSchedule();
    static ThreeEyes* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    virtual void onLanded();
    virtual void encounteredWall();
};

///////////////////////////
// DJMonkey
///////////////////////////
class DJMonkey : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static DJMonkey* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};


///////////////////////////
// Frog
///////////////////////////
class Frog : public FireableBase
{
private:
    virtual void movingSchedule(float dt);
public:
    virtual void runSchedule();
    static Frog* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    
    virtual cocos2d::Rect damageBoundingBox();
    bool isAttacking;
    Sprite* tung;
    void tungFireDone(Ref* ref);
    void tungTakeDownDone(Ref* ref);
    void forceLanding(float dt);
    virtual void onLanded();
    virtual void encounteredWall();
    virtual void encounteredCliff(int direction);
};

///////////////////////////
// RandomBox
///////////////////////////
class RandomBox : public EnemyBase
{
private:
    
public:
    static RandomBox* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
};


///////////////////////////
// EnemyWithGun
///////////////////////////
#define MISSILE_SINGLE 0
#define MISSILE_ARC 1
#define MISSILE_CIRCLE 2
#define MISSILE_COLUMN 3
#define MISSILE_RANDOM 4
#define MISSILE_FOUNTAIN 5
#define MISSILE_SPIRAL 6
#define MISSILE_NONE 7
class EnemyWithGun : public FireableOnPlatformBase
{
private:
    virtual void movingSchedule(float dt);
    virtual void encounteredWall();
    virtual void encounteredCliff(int direction);
    int loadedBulletCount=1;
    int maxLoadedBulletCount=1;
    float maxReloadingTime=2;
    float elapsedReloadingTime=2;
    int gunType;
    int cliffStatus=0;
public:
    int bulletSpeed;
    int missileType = MISSILE_SINGLE;
    virtual void runSchedule();
    virtual void setFlippedX(bool flip);
    static EnemyWithGun* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    void setGunInfo(int bulletCount, float reloadingTime, int gun, int speed);
    void startJumping();
    float jumpTimeElapse=1;
    void jumping(float dt);
};

class NPCBase : public FireableBase
{
private:
    
public:
    int talkIndex=0;
    NPCBase();
    ~NPCBase();
    static NPCBase* create(std::string sptName);
    virtual std::string getTalk();
};

// TheKnight
#define KNIGHT_SWING_LEFT_BOTTOM 0
#define KNIGHT_SWING_BACK_FROM_LEFT_BOTTOM 1
class TheKnight : public NPCBase
{
private:
//    void movingSchedule(float dt);
    cocostudio::timeline::ActionTimeline* animation;
public:
//    TheKnight();
//    ~TheKnight();
    int state=-1;
    static TheKnight* create();
    void initKnight();
    void doKnightAction(int action);
    std::string getTalk();
};

// HoodGhost
class HoodGhost : public NPCBase
{
private:
public:
    Sprite* sptTalkBalloon=nullptr;
    static HoodGhost* create();
    virtual std::string getTalk();
    std::string getKeyToTalk();
};

// TutorialGhost
class TutorialGhost : public EnemyBase
{
private:
    virtual void movingSchedule(float dt);
    float nextTalkTimeLeft = 0;
public:
    virtual void runSchedule();
    cocos2d::ui::ImageView* sptTalkBalloon=nullptr;
    Label* lblTalk;
    static TutorialGhost* create();
    
    void talk();
};
// Seller
class Seller : public NPCBase
{
private:
public:
    static Seller* create();
    virtual std::string getTalk();
};
// OnSale
class OnSale : public NPCBase
{
private:
public:
    static OnSale* create(int theItem);
    int item;
    bool isExplained=false;
    int price;
    bool isReadyToSale;
    Node* priceTag;
    void setItem(int theItem);
    void buyItem();
    virtual std::string getTalk();
};



///////////////////////////
// Bazooka
///////////////////////////
#define BAZOOKA_ACTION_SHOOT 0
#define BAZOOKA_ACTION_MINI 1
#define BAZOOKA_ACTION_BIG 2
#define BAZOOKA_ACTION_CIRCLE 3
class Bazooka : public EnemyWithGun
{
private:
    virtual void movingSchedule(float dt);
    float timeLeftToNextAction;
    int currentAction;
    bool finalActionTaken=false;
    float stopTime=0;
    int shootCountMax;
    int shootCount=0;
public:
    virtual void runSchedule();
    static Bazooka* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    void shoot(float dt);
    void shootMiniReady(float dt);
    void shootMini(float dt);
    void shootBigReady(float dt);
    void shootBig(float dt);
    void shootCircleReady(float dt);
    void shootCircle(float dt);
    virtual cocos2d::Rect damageBoundingBox();
    virtual void onLanded();
    virtual void encounteredWall();
    virtual void encounteredCliff(int direction);
};



///////////////////////////
// Machinegun
///////////////////////////
#define MACHINEGUN_ACTION_SHOOT 0
#define MACHINEGUN_ACTION_MINI 1
#define MACHINEGUN_ACTION_BIG 2
#define MACHINEGUN_ACTION_CIRCLE 3
class Machinegun : public EnemyWithGun
{
private:
    virtual void movingSchedule(float dt);
    float timeLeftToNextAction;
    int currentAction;
    bool finalActionTaken=false;
    float stopTime=0;
    int shootCountMax;
    int shootCount=0;
public:
    virtual void runSchedule();
    static Machinegun* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    void shoot(float dt);
    void shootMiniReady(float dt);
    void shootMini(float dt);
    void shootBigReady(float dt);
    void shootBig(float dt);
    void shootCircleReady(float dt);
    void shootCircle(float dt);
    void turnAround(float dt);
    virtual cocos2d::Rect damageBoundingBox();
    virtual void onLanded();
    virtual void encounteredWall();
    virtual void encounteredCliff(int direction);
};


///////////////////////////
// Giant
///////////////////////////
#define GIANT_ACTION_SHOOT 0
#define GIANT_ACTION_SLAP 1
#define GIANT_ACTION_STAMP 2
#define GIANT_ACTION_SLAP_SHOOT 3
class Giant : public EnemyWithGun
{
private:
    virtual void movingSchedule(float dt);
    float timeLeftToNextAction;
    int currentAction;
    bool finalActionTaken=false;
    float stopTime=0;
    int shootCountMax;
    int shootCount=0;
    float maxSpeed = 10;
    float speed = 0;
public:
    Sprite* leftHand;
    Sprite* rightHand;
    Droppable* head;
    Sprite* nose;
    cocos2d::Point leftHandPos;
    cocos2d::Point rightHandPos;
    void moveHands();
    void playKoongSound();
    void stopHands();
    int handsAngle=0;
    void movingHands(float dt);
    float handsMovingRadius = 20;
    void movingSlap(float dt);
    float slapAngle;
    float slapTimeout;
    Droppable* slapHand;
    void hit();
    int hitCount=0;
    void getBackToNormalNose(float dt);
    void setPositions();
    virtual void runSchedule();
    static Giant* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    bool isShootLeftAttack=false;
    bool isSlapLeftAttack=false;
    void shoot(float dt);
    void shootSlapReady(float dt);
    void shootSlap(float dt);
    void shootStampReady(float dt);
    void shootStamp(float dt);
    virtual cocos2d::Rect damageBoundingBox();
    virtual void onLanded();
    virtual void onHit();
    virtual void onDead();
    virtual void encounteredWall();
    virtual void encounteredCliff(int direction);
    
};

///////////////////////////
// GiantHood
///////////////////////////
#define GIANT_HOOD_ACTION_SHOOT 0
#define GIANT_HOOD_ACTION_SPIRAL 1
#define GIANT_HOOD_ACTION_SPIN 2
#define GIANT_HOOD_ACTION_RANDOM 3
class GiantHood : public EnemyWithGun
{
private:
    virtual void movingSchedule(float dt);
    float timeLeftToNextAction;
    int currentAction;
    bool finalActionTaken=false;
    float stopTime=0;
    int shootCountMax;
    int shootCount=0;
    float maxSpeed = 3;
    float speed = 0;
    float shootAngle;
    cocos2d::Point moveTargetPos;
    cocos2d::Point centerMapPos;
public:
    virtual void runSchedule();
    static GiantHood* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    void shoot(float dt);
    void changeMoveTarget(float dt);
    void shootSpiral(float dt);
    float spiralMissileMoveAngle;
    float spinAngle=0;
    void shootSpin(float dt);
    void shootRandom(float dt);
    virtual cocos2d::Rect damageBoundingBox();
    virtual void onLanded();
    virtual void encounteredWall();
    virtual void encounteredCliff(int direction);
};

///////////////////////////
// SpinMissile
///////////////////////////
class SpinMissile : public Missile
{
private:
    void movingSchedule(float dt);
    cocos2d::Point center;
    float radius;
public:
    float angleVar;
    static SpinMissile* create(int unit, const char* sptName, cocos2d::Point centerPoint, cocos2d::Point initPos);
};


///////////////////////////
// Alphabet
///////////////////////////
class Alphabet : public EnemyBase
{
private:
public:
    float keyDownTime=0;
    void movingSchedule(float dt);
    cocos2d::Label* lblChar=nullptr;
    void initLabel(std::string str, int fontSize, cocos2d::Point pos);
    cocos2d::Point lblPos = cocos2d::Point::ZERO;
    static Alphabet* create(int unit, const char* sptName);
};
#endif /* defined(__LegendDaryTheTower__Enemies__) */

