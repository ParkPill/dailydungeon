//
//  Enemies.cpp
//  LegendDaryTheTower
//
//  Created by SUNG PILL PACK on 14. 6. 13..
//
//

#include "Enemies.h"
#include "GameManager.h"
#include "HelloWorldScene.h"
#include "LanguageManager.h"




///////////////////////////////////////
// EnemyBase
///////////////////////////////////////
EnemyBase::EnemyBase(){
//    effectType = EFFECT_NONE;
    energyBar = NULL;
    energyBarContent = NULL;
}

EnemyBase::~EnemyBase(){
    stopSchedule();
}
void EnemyBase::stopSchedule(){
    this->unschedule(schedule_selector(EnemyBase::movingSchedule));
}
void EnemyBase::resumeWalking(float dt){
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}

void EnemyBase::setCurrentMoveType(int move){
    currentMoveType = move;
    if (currentMoveType == ENEMY_MOVE_STAND) {
        stop();
    }else if (move == ENEMY_MOVE_WANDER_CROSS_PLATFORM ||
              move == ENEMY_MOVE_WANDER_ON_A_PLATFORM ||
              move == ENEMY_MOVE_WANDER_CROSS_PLATFORM_CRASH_BY_WALL){
        isTowardLeft?goLeft():goRight();
    }
}
void EnemyBase::runSchedule(){

    CCLOG("EnemyBase run schedule");
}
void EnemyBase::movingSchedule(float dt){
    
    
}
void EnemyBase::hideEnergy(){
    if(energyBar != NULL){
        energyBar->setVisible(false);
        energyBarContent->setVisible(false);
    }
}
void EnemyBase::showEnergy(){
    if (!energyBarExist) {
        return;
    }
    if(energyBar == NULL){
        energyBar = Sprite::createWithSpriteFrameName("energyBar.png");
        this->addChild(energyBar);
        energyBar->setPosition(Point(this->getContentSize().width/2, this->getContentSize().height + 10));
        energyBarContent = Sprite::createWithSpriteFrameName("energyGreen.png");
        energyBar->addChild(energyBarContent);
        energyBarContent->setPosition(Point(2, energyBar->getContentSize().height/2));
        energyBarContent->setAnchorPoint(Point(0, 0.5));
        energyBar->setScale(1/this->getScaleX());
    }
    int length = 15;
    float rate = (float)energy/maxEnergy;
    energyBarContent->setScaleX(length*rate);
    if(rate < 0.3){
        energyBarContent->setSpriteFrame("energyRed.png");
    }else if(rate < 0.7){
        energyBarContent->setSpriteFrame("energyYellow.png");
    }
}
Missile* Missile::createMissile(int unit, int eng, int angle, float extraSpd, const char* sptName){
    return Missile::createMissile(unit, eng, angle, extraSpd, sptName, false);
}
Missile* Missile::createMissile(int unit, int eng, int angle, float extraSpd, const char* sptName, bool destructable){
    Missile* enemy = new Missile();
    enemy->init(unit, eng, extraSpd, sptName);
    enemy->parentWeaponIndex = unit;
    enemy->movingAngle = angle;
    enemy->setRotation(-angle);
    enemy->immortal = !destructable;
    enemy->rewards = 0;
    enemy->goldPossiblity = 0;
    enemy->victimArray.clear();
    return enemy;
}

void EnemyBase::initEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int actionBeforeFindHero, int actionAfterFindHero, int monitorDistance){
    this->init(unit, eng, extraSpd, sptName);
    this->moveTypeBeforeFindHero = howToMoveBeforeFindHero;
    this->moveTypeAfterFindHero = howToMoveAfterFindHero;
    this->setCurrentMoveType(howToMoveBeforeFindHero);
    this->rewards = rewardsCoinCount;
    this->goldPossiblity = goldCoinPossiblity;
    this->monitoringDistance = monitorDistance;
    this->actionTypeBeforeFindHero = actionBeforeFindHero;
    this->actionTypeAfterFindHero = actionAfterFindHero;
    this->isTowardLeft = false;
    this->wantsToGoLeft = this->isTowardLeft;
    this->wantsToGoRight = !this->isTowardLeft;

    this->runSchedule();
}

EnemyBase* EnemyBase::createEnemy(int unit, int eng, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity)
{
    EnemyBase* enemy = new EnemyBase();
    enemy->initEnemy(unit, eng, 0, sptName, rewardsCoinCount, goldCoinPossiblity, ENEMY_MOVE_HANG, ENEMY_MOVE_HANG, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, 100);
    return enemy;
}
EnemyBase* EnemyBase::createEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    EnemyBase* enemy = new EnemyBase();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}

EnemyBase* EnemyBase::createEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int actionBeforeFindHero, int actionAfterFindHero, int monitorDistance)
{
    EnemyBase* enemy = new EnemyBase();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, actionBeforeFindHero, actionAfterFindHero, monitorDistance);
    return enemy;
}

///////////////////////////////////////
// FireableBase
///////////////////////////////////////
FireableBase::FireableBase(){
    fireCoolTimeMax = 1.5f;
    fireCoolTime = fireCoolTimeMax;
}
FireableBase::~FireableBase(){}

FireableBase* FireableBase::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    FireableBase* enemy = new FireableBase();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->sptGun = Sprite::createWithSpriteFrameName("handGun.png");
    enemy->addChild(enemy->sptGun);
    return enemy;
}
///////////////////////////////////////
// WalkableOnPlatformBase
///////////////////////////////////////
WalkableOnPlatformBase::WalkableOnPlatformBase(){}
WalkableOnPlatformBase::~WalkableOnPlatformBase(){}
void WalkableOnPlatformBase::encounteredCliff(int direction){
    setFlippedX(!isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
void WalkableOnPlatformBase::encounteredWall(){
    
    setFlippedX(!isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
WalkableOnPlatformBase* WalkableOnPlatformBase::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    WalkableOnPlatformBase* enemy = new WalkableOnPlatformBase();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}

///////////////////////////////////////
// WalkableCrossPlatformBase
///////////////////////////////////////
WalkableCrossPlatformBase::WalkableCrossPlatformBase(){}
WalkableCrossPlatformBase::~WalkableCrossPlatformBase(){}
void WalkableCrossPlatformBase::encounteredWall(){
    setFlippedX(!isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
WalkableCrossPlatformBase* WalkableCrossPlatformBase::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    WalkableCrossPlatformBase* enemy = new WalkableCrossPlatformBase();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}

///////////////////////////////////////
// FireableOnPlatformBase
///////////////////////////////////////
FireableOnPlatformBase::FireableOnPlatformBase(){}
FireableOnPlatformBase::~FireableOnPlatformBase(){}
void FireableOnPlatformBase::encounteredCliff(int direction){
    setFlippedX(!isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
void FireableOnPlatformBase::encounteredWall(){
    setFlippedX(!isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
FireableOnPlatformBase* FireableOnPlatformBase::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    FireableOnPlatformBase* enemy = new FireableOnPlatformBase();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}

///////////////////////////////////////
// FireableCrossPlatformBase
///////////////////////////////////////
FireableCrossPlatformBase::FireableCrossPlatformBase(){}
FireableCrossPlatformBase::~FireableCrossPlatformBase(){}
void FireableCrossPlatformBase::encounteredWall(){
    setFlippedX(!isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
FireableCrossPlatformBase* FireableCrossPlatformBase::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    FireableCrossPlatformBase* enemy = new FireableCrossPlatformBase();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}

///////////////////////////////////////
// HotDog
///////////////////////////////////////
void HotDog::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if(isTargetFound){
        this->setFlippedX(this->target->getPosition().x < this->getPosition().x);
    }
    
    jumpCoolTime += dt;
    if (jumpCoolTime < jumpCoolTimeMax) {
        return;
    }
    
    jumpCoolTime = 0;
    
    wantsToStop = false;
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
    wantsToJump = true;
}

void HotDog::runSchedule(){
//    this->EnemyBase::runSchedule();
    this->schedule(schedule_selector(HotDog::movingSchedule), 0.3);
}

HotDog* HotDog::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    HotDog* enemy = new HotDog();
    enemy->jumpCoolTimeMax = 3 + (rand()%10)*0.1f;
    enemy->jumpCoolTime = enemy->jumpCoolTimeMax;
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
//    enemy->runSchedule();
    return enemy;
}

void HotDog::onLanded(){
    Droppable::onLanded();
    this->runAction(Sequence::create(ScaleTo::create(0.1, 1, 0.9), ScaleTo::create(0.1, 1, 1.05), ScaleTo::create(0.05, 1, 1), NULL));
    this->wantsToStop = true;
    wantsToGoLeft = false;
    wantsToGoRight = false;
}

///////////////////////////////////////
// HotDogGun/Octopus
///////////////////////////////////////
void HotDogGun::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if(isTargetFound){
        this->setFlippedX(this->target->getPosition().x < this->getPosition().x);
    }
    jumpCoolTime += dt;
    if (jumpCoolTime < jumpCoolTimeMax) {
        return;
    }
    
    jumpCoolTime = 0;
    
    wantsToStop = false;
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
    wantsToJump = true;
}
void HotDogGun::missileSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if(isTargetFound){
        this->setFlippedX(this->target->getPosition().x < this->getPosition().x);
        if(this->target->getPosition().x < this->getPosition().x && !isTowardLeft)
            this->setFlippedX(true);
        else if(this->target->getPosition().x > this->getPosition().x && isTowardLeft)
            this->setFlippedX(false);
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "msHotDog.png");
        missile->isFregile = true;
        missile->extraSpeed = 100;
        
        if(this->getTag() == UNIT_OCTOPUS) {
            missile->setPosition(this->getPosition() + Point(isTowardLeft?-6:6,12));
            missile->weight = 0.1f;
            double angle = GameManager::getInstance()->getAngle(missile->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2));
            missile->movingAngle = angle + 180;
        }else if(this->getTag() == UNIT_HOTDOG_GUN) {
            missile->setPosition(this->getPosition() + Point(isTowardLeft?-8:8,-4));
            missile->movingAngle = isTowardLeft?180:0;
        }
        missile->setRotation(-missile->movingAngle);
        this->missileArray.pushBack((Missile*)missile);
        this->runAnimation(aniName, false);
        
        this->getParent()->addChild(missile);
    }
}
void HotDogGun::runSchedule(){
    float delay = (rand()%10)*0.05;
    this->schedule(schedule_selector(HotDogGun::missileSchedule), 4, kRepeatForever, 1 + delay);
    this->schedule(schedule_selector(HotDogGun::movingSchedule), 4, kRepeatForever, delay);
}

HotDogGun* HotDogGun::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    HotDogGun* enemy = new HotDogGun();
    enemy->jumpCoolTimeMax = 4;
    enemy->jumpCoolTime = enemy->jumpCoolTimeMax;
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}

void HotDogGun::onLanded(){
    Droppable::onLanded();
    this->runAction(Sequence::create(ScaleTo::create(0.1, 1, 0.9), ScaleTo::create(0.1, 1, 1.05), ScaleTo::create(0.05, 1, 1), NULL));
    this->wantsToStop = true;
    wantsToGoLeft = false;
    wantsToGoRight = false;
}

///////////////////////////////////////
// AppleMon
///////////////////////////////////////
void AppleMon::movingSchedule(float dt){
    this->stopActionByTag(79);
    
    Missile* missile = Missile::createMissile(UNIT_MISSILE_DROPPABLE, 1, 0, 0, "appleWorm.png");
//    missile->velocity = Point(isTowardLeft?-50:50, 50);
    missile->isFregile = true;
    missile->setPosition(this->getPosition());
    missile->weight = 0.1f;
    this->missileArray.pushBack((Missile*)missile);
    missile->secondTag = UNIT_APPLEMON_WORM;
    
    this->getParent()->addChild(missile);
    this->scheduleOnce(schedule_selector(AppleMon::resumeAction), 1);
}

void AppleMon::resumeAction(float dt){
    RepeatForever* repeat;
    float duration = 3;
    if (isTowardLeft) {
        repeat = RepeatForever::create(Sequence::create(MoveTo::create(duration*(this->getPosition().x - leftPos.x)/(rightPos.x - leftPos.x), leftPos), ScaleTo::create(0, 1,1), MoveTo::create(duration, rightPos), ScaleTo::create(0, -1,1), NULL));
    }else{
        repeat = RepeatForever::create(Sequence::create(MoveTo::create(duration*(rightPos.x - this->getPosition().x)/(rightPos.x - leftPos.x), rightPos), ScaleTo::create(1, -1,1), MoveTo::create(duration, leftPos), ScaleTo::create(1, 1,1), NULL));
    }
    repeat->setTag(79);
    this->runAction(repeat);
}

void AppleMon::runSchedule(){
    this->schedule(schedule_selector(AppleMon::movingSchedule), 3 + 0.1f*(rand()%10));
}

AppleMon* AppleMon::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    AppleMon* enemy = new AppleMon();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->runAnimation("appleMon", true);
    return enemy;
}

void AppleMon::setPositionForWander(Point pos){
    this->setPosition(pos);
    this->leftPos = pos + Point(-40, 0);
    this->rightPos = pos + Point(40, 0);
}

///////////////////////////////////////
// FlyingPenguin
///////////////////////////////////////
void FlyingPenguin::movingSchedule(float dt){
    this->stopActionByTag(79);
    
//    findTarget(target, monitoringDistance);
    
//    if (this->isTargetFound) {
    Missile* missile = Missile::createMissile(UNIT_MISSILE_DROPPABLE, 1, 0, 0, "msPenguin.png");
        missile->isFregile = true;
        missile->setPosition(this->getPosition() + Point(getScaleX() < 0?-20:20, -12));
        missile->weight = 0.3f;
        this->missileArray.pushBack(missile);;
        
        this->getParent()->addChild(missile);
//    }
    
    this->scheduleOnce(schedule_selector(FlyingPenguin::resumeAction), 0.1);
}

void FlyingPenguin::resumeAction(float dt){
    RepeatForever* repeat;
    float duration = 1;
    if (isTowardLeft) {
        repeat = RepeatForever::create(Sequence::create(MoveTo::create(duration*(this->getPosition().x - leftPos.x)/(rightPos.x - leftPos.x), leftPos), ScaleTo::create(0, 1,1), MoveTo::create(duration, rightPos), ScaleTo::create(0, -1,1), NULL));
    }else{
        repeat = RepeatForever::create(Sequence::create(MoveTo::create(duration*(rightPos.x - this->getPosition().x)/(rightPos.x - leftPos.x), rightPos), ScaleTo::create(0, -1,1), MoveTo::create(duration, leftPos), ScaleTo::create(0, 1,1), NULL));
    }
    repeat->setTag(79);
    this->runAction(repeat);
}

void FlyingPenguin::runSchedule(){
    this->schedule(schedule_selector(FlyingPenguin::movingSchedule), 1 + 0.1f*(rand()%10));
//    this->schedule(schedule_selector(FlyingPenguin::movingSchedule), 0.8);
}

FlyingPenguin* FlyingPenguin::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    FlyingPenguin* enemy = new FlyingPenguin();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->runAnimation("flyingPenguin", true);
    return enemy;
}

void FlyingPenguin::setPositionForWander(Point pos){
    this->setPosition(pos);
    this->leftPos = pos + Point(-40, 0);
    this->rightPos = pos + Point(40, 0);
    
    float duration = 2.3;
    RepeatForever* repeat = RepeatForever::create(Sequence::create(MoveTo::create(duration, leftPos), ScaleTo::create(0, 1,1), MoveTo::create(duration, rightPos), ScaleTo::create(0, -1,1), NULL));
    this->runAction(repeat);
    this->schedule(schedule_selector(FlyingPenguin::movingSchedule), 1.0);
}
///////////////////////////////////////
// Bat
///////////////////////////////////////
void Bat::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        if (this->currentMoveType != ENEMY_MOVE_FLY_TO_HERO_FOLLOW) {
            this->setCurrentMoveType(ENEMY_MOVE_FLY_TO_HERO_FOLLOW);
        }
    }else{
        if (this->currentMoveType != ENEMY_MOVE_HANG) {
            this->setCurrentMoveType(ENEMY_MOVE_HANG);
        }
    }
}

void Bat::runSchedule(){
    this->schedule(schedule_selector(Bat::movingSchedule), 1.5f);
}

Bat* Bat::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    Bat* enemy = new Bat();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->runAnimation("bat", true);
    return enemy;
}
///////////////////////////////////////
// OddEye/JellyFish
///////////////////////////////////////
void OddEye::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        this->setFlippedX(this->target->getPosition().x < this->getPosition().x);
        if (this->currentMoveType != this->moveTypeAfterFindHero) {
            this->setCurrentMoveType(this->moveTypeAfterFindHero);
        }
    }
//    else{
//        if (this->currentMoveType != ENEMY_MOVE_HANG) {
//            this->setCurrentMoveType(ENEMY_MOVE_HANG);
//        }
//    }
}

void OddEye::runSchedule(){
    this->schedule(schedule_selector(OddEye::movingSchedule), 1.5f);
}

OddEye* OddEye::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    OddEye* enemy = new OddEye();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}

///////////////////////////////////////
// OddEyeRide
///////////////////////////////////////
void OddEyeRide::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        this->stopActionByTag(79);
        this->setFlippedX(this->target->getPosition().x < this->getPosition().x);
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "msGhost.png");
        missile->isFregile = true;
        missile->setPosition(this->getPosition());
        missile->weight = 0.7f;
        missile->movingAngle = this->isTowardLeft?180:0;
        missile->setFlippedX(this->isTowardLeft);
        this->missileArray.pushBack(missile);
        this->runAnimation("oddEyeRideFire", false);
        this->runAction(Sequence::create(DelayTime::create(0.5), CallFunc::create(CC_CALLBACK_0(OddEyeRide::animateAgain, this)), NULL));
        this->getParent()->addChild(missile);
        
        if (this->currentMoveType != this->moveTypeAfterFindHero) {
            this->setCurrentMoveType(this->moveTypeAfterFindHero);
        }
    }
}
void OddEyeRide::animateAgain(){
    this->runAnimation("oddEyeRide", true);
}
void OddEyeRide::runSchedule(){
    this->schedule(schedule_selector(OddEyeRide::movingSchedule), 1.5f);
}

OddEyeRide* OddEyeRide::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    OddEyeRide* enemy = new OddEyeRide();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->runAnimation("oddEyeRide", true);
    return enemy;
}

///////////////////////////////////////
// Ghost
///////////////////////////////////////
void Ghost::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        this->stopActionByTag(79);
        
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "msGhost.png");
        //    missile->velocity = Point(isTowardLeft?-50:50, 50);
        missile->isFregile = true;
        missile->setPosition(this->getPosition());
        missile->weight = 0.6f;
        double angle = GameManager::getInstance()->getAngle(missile->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2));
        missile->movingAngle = angle + 180;
        missile->runAction(RepeatForever::create(RotateBy::create(0.5, 360)));
        this->missileArray.pushBack(missile);
        
        this->getParent()->addChild(missile);
        
        if (this->currentMoveType != ENEMY_MOVE_FLY_TO_HERO_FOLLOW) {
            this->setCurrentMoveType(ENEMY_MOVE_FLY_TO_HERO_FOLLOW);
        }
    }else{
        if (this->currentMoveType != ENEMY_MOVE_HANG) {
            this->setCurrentMoveType(ENEMY_MOVE_HANG);
        }
    }
}

void Ghost::runSchedule(){
    this->schedule(schedule_selector(Ghost::movingSchedule), 1.5f);
}

Ghost* Ghost::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    Ghost* enemy = new Ghost();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->runAnimation("ghost", true);
    return enemy;
}
///////////////////////////////////////
// SquirrelStand
///////////////////////////////////////
void SquirrelStand::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
 
    if (this->isTargetFound) {
        if(this->target->getPosition().x < this->getPosition().x && !isTowardLeft)
            this->setFlippedX(true);
        else if(this->target->getPosition().x > this->getPosition().x && isTowardLeft)
            this->setFlippedX(false);
        Missile* missile = Missile::createMissile(UNIT_MISSILE_DROPPABLE, 1, 0, 0, "dotori.png");
        missile->velocity = Point(isTowardLeft?-50:50, 50);
        missile->isFregile = true;
        missile->setPosition(this->getPosition());
        missile->weight = 0.1f;
        this->missileArray.pushBack(missile);
        this->runAnimation("squirrelFire", false);
        
        this->getParent()->addChild(missile);
    }
}

void SquirrelStand::runSchedule(){
    this->schedule(schedule_selector(SquirrelStand::movingSchedule), 1.3);
}

SquirrelStand* SquirrelStand::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    SquirrelStand* enemy = new SquirrelStand();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToStop = true;
    enemy->wantsToGoLeft = false;
    enemy->wantsToGoRight = false;
    return enemy;
}
///////////////////////////////////////
// SkeletonWizard
///////////////////////////////////////
void SkeletonWizard::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        attackIndex++;
        
        wantsToGoLeft = false;
        wantsToGoRight = false;
        if (attackIndex < 10) {
            return;
        }
        this->stopAllActions();
        attackIndex = 0;
        if(this->target->getPosition().x < this->getPosition().x && !isTowardLeft)
            this->setFlippedX(true);
        else if(this->target->getPosition().x > this->getPosition().x && isTowardLeft)
            this->setFlippedX(false);
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "wizardLaser.png");
        missile->isFregile = true;
        missile->extraSpeed = 100;
        missile->movingAngle = isTowardLeft?180:0;
        missile->setPosition(this->getPosition() + Point(isTowardLeft?-20:20,6));
        this->missileArray.pushBack(missile);
        this->runAnimation("skeletonWizardFire", false);
        
        this->getParent()->addChild(missile);
    }else{
        if (attackIndex != 10) {
            attackIndex = 10;
            this->runAnimation("skeletonWizardWalk", true);
        }
    }
}

void SkeletonWizard::runSchedule(){
    attackIndex = 0;
    this->schedule(schedule_selector(SkeletonWizard::movingSchedule), 0.1);
}

SkeletonWizard* SkeletonWizard::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    SkeletonWizard* enemy = new SkeletonWizard();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}
///////////////////////////////////////
// BluePot
///////////////////////////////////////
void BluePot::movingSchedule(float dt){
//    findTarget(target, monitoringDistance);
    attackIndex++;
    if (attackIndex > 9) {
        attackIndex = 0;
//        wantsToGoLeft = false;
//        wantsToGoRight = false;
//        this->scheduleOnce(schedule_selector(EnemyBase::resumeWalking), 1);
//        this->stopAllActions();

//        if(this->target->getPosition().x < this->getPosition().x && !isTowardLeft)
//            this->setFlippedX(true);
//        else if(this->target->getPosition().x > this->getPosition().x && isTowardLeft)
//            this->setFlippedX(false);
        Missile* missile = Missile::createMissile(UNIT_MISSILE_DROPPABLE, 1, 0, 0, "waterBall.png");
        float power = 300;
        missile->extraSpeed = 80;
        missile->velocity = Point(isTowardLeft?-power:power, power/2);
        missile->secondTag = UNIT_BLUE_POT_WATERBALL;
        missile->isFregile = true;
        missile->setPosition(this->getPosition() + Point(isTowardLeft?-13:13, 5));
        missile->weight = 0.9f;
        this->missileArray.pushBack(missile);
        this->setSpriteFrame("bluePot0.png");
        
        this->getParent()->addChild(missile);
    }else{
        
    }
}

void BluePot::runSchedule(){
    attackIndex = 0;
    this->schedule(schedule_selector(BluePot::movingSchedule), 0.1);
}

BluePot* BluePot::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    BluePot* enemy = new BluePot();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToGoLeft = false;
    enemy->wantsToGoRight = true;
    enemy->runAnimation("bluePot", true);
    return enemy;
}
///////////////////////////////////////
// GreenWorm
///////////////////////////////////////
void GreenWorm::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        fireCoolTime-=dt;
        if(fireCoolTime < 0){
            this->stopAllActions();
            if(this->target->getPosition().x < this->getPosition().x && !isTowardLeft)
                this->setFlippedX(true);
            else if(this->target->getPosition().x > this->getPosition().x && isTowardLeft)
                this->setFlippedX(false);
            Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "msGreenWorm.png");
            missile->isFregile = true;
            missile->extraSpeed = 100;
            missile->movingAngle = isTowardLeft?180:0;
            missile->setPosition(this->getPosition() + Point(isTowardLeft?-20:20,6));
            this->missileArray.pushBack(missile);
            this->runAnimation("greenFire", false);
            missile->setFlippedX(this->target->getPosition().x < this->getPosition().x);
            fireCoolTime = fireCoolTimeMax;
            this->getParent()->addChild(missile);
        }
        this->wantsToGoLeft = false;
        this->wantsToGoRight = false;
        this->wantsToStop = true;
    }else{
        this->wantsToGoLeft = isTowardLeft;
        this->wantsToGoRight = !isTowardLeft;
        this->wantsToStop = false;
        this->runAnimation("greenWorm", true);
    }
}

void GreenWorm::runSchedule(){
    fireCoolTime = -1;
    this->schedule(schedule_selector(GreenWorm::movingSchedule), 0.5);
}

GreenWorm* GreenWorm::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    GreenWorm* enemy = new GreenWorm();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}

///////////////////////////////////////
// ThreeEyes
///////////////////////////////////////
void ThreeEyes::movingSchedule(float dt){
//    findTarget(target, monitoringDistance);
    if(attackIndex < 4){
        attackIndex++;
    }else{
        this->isTargetFound = false;
        findTarget(target, monitoringDistance);
        
        if (this->isTargetFound) {
            this->velocity = Point(0, 200);
            this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("threeEyeJump.png"));
            this->scheduleOnce(schedule_selector(ThreeEyes::fireLater), 0.2);
            
            this->setFlippedX(target->getPosition().x < this->getPosition().x);
            wantsToGoLeft = isTowardLeft;
            wantsToGoRight = !isTowardLeft;
            
            attackIndex = 0;
            wantsToStop = true;
        }else{
            this->velocity = Point(0, 100);
            wantsToStop = false;
            wantsToGoLeft = isTowardLeft;
            wantsToGoRight = !isTowardLeft;
            
            this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("threeEyeJump.png"));
        }
    }
}

void ThreeEyes::fireLater(float dt){
    if(this->target->getPosition().x < this->getPosition().x && !isTowardLeft)
        this->setFlippedX(true);
    else if(this->target->getPosition().x > this->getPosition().x && isTowardLeft)
        this->setFlippedX(false);
    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "msThreeEyes.png");
    
    missile->isFregile = true;
    missile->movingAngle = isTowardLeft?190:-10;
    missile->setPosition(this->getPosition() + Point(isTowardLeft?-5:5,-10));
    missile->setFlippedX(isTowardLeft);
    missile->extraSpeed = 100;
    this->getParent()->addChild(missile);
    this->missileArray.pushBack(missile);
    this->runAnimation("threeEyesFire", false);
}
void ThreeEyes::runSchedule(){
    this->schedule(schedule_selector(ThreeEyes::movingSchedule), 0.5);
}

ThreeEyes* ThreeEyes::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    ThreeEyes* enemy = new ThreeEyes();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}

void ThreeEyes::onLanded(){
    Droppable::onLanded();
    this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("threeEye0.png"));
    wantsToStop = true;
    this->wantsToGoLeft = false;
    this->wantsToGoRight = false;
}
void ThreeEyes::encounteredWall(){
    this->setFlippedX(!this->isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
///////////////////////////////////////
// DrumBomb
///////////////////////////////////////
void DrumBomb::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        Animation* animation = AnimationCache::getInstance()->getAnimation("explosion");
        Animate* animate = Animate::create(animation);
        this->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(DrumBomb::explosionDone, this)), NULL));
        
        ((HelloWorld*)this->getParent()->getParent()->getParent())->hitDary(this->maxEnergy*0.1f);
        this->setScale(2);
        this->unschedule(schedule_selector(DrumBomb::movingSchedule));
    }else{
       
    }
}

void DrumBomb::explosionDone(Ref* obj){
    ((HelloWorld*)this->getParent()->getParent()->getParent())->removeEnemy(this);
}

void DrumBomb::runSchedule(){
    this->schedule(schedule_selector(DrumBomb::movingSchedule), 0.1);
}

DrumBomb* DrumBomb::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    DrumBomb* enemy = new DrumBomb();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->runAnimation("drumBomb", true);
    return enemy;
}
///////////////////////////////////////
// BoardOwl
///////////////////////////////////////
void BoardOwl::movingSchedule(float dt){
    attackIndex++;
    if(attackIndex > 10){
        attackIndex = 0;
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "msOwl.png");
        missile->isFregile = true;
        missile->movingAngle = isTowardLeft?180:0;
        missile->setPosition(this->getPosition() + Point(isTowardLeft?-5:5,-10));
        missile->setFlippedX(isTowardLeft);
        missile->extraSpeed = 100;
        this->missileArray.pushBack(missile);
        this->getParent()->addChild(missile);
    }
}

void BoardOwl::runSchedule(){
    attackIndex = 0;
    this->schedule(schedule_selector(BoardOwl::movingSchedule), 0.1);
}

BoardOwl* BoardOwl::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    BoardOwl* enemy = new BoardOwl();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->runAnimation("boardOwl", true);
    return enemy;
}

///////////////////////////////////////
// ThornDrop
///////////////////////////////////////
void ThornDrop::movingSchedule(float dt){
    if(this->target->getPosition().x - this->getPosition().x < 30 && this->target->getPosition().x - this->getPosition().x > -30){
        this->missileArray.pushBack((Missile*)this);
        this->isFregile = true;
        this->unschedule(schedule_selector(ThornDrop::movingSchedule));
    }
}
void ThornDrop::runSchedule(){
    this->schedule(schedule_selector(ThornDrop::movingSchedule), 0.1);
}
ThornDrop* ThornDrop::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    ThornDrop* enemy = new ThornDrop();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_MOVE, ENEMY_ACTION_MOVE, monitorDistance);
    enemy->wantsToStop = true;
    enemy->wantsToGoLeft = false;
    enemy->wantsToGoRight = false;
    enemy->untouchable = true;
    return enemy;
}
void ThornDrop::onLanded(){
    Droppable::onLanded();
    ((HelloWorld*)this->getParent()->getParent()->getParent())->removeEnemy(this);
}
///////////////////////////////////////
// BlueWorm
///////////////////////////////////////
void BlueWorm::movingSchedule(float dt){
    
}
void BlueWorm::runSchedule(){
    this->schedule(schedule_selector(BlueWorm::movingSchedule), 0.1);
}
BlueWorm* BlueWorm::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    BlueWorm* enemy = new BlueWorm();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_MOVE, ENEMY_ACTION_MOVE, monitorDistance);
    enemy->wantsToStop = false;
    enemy->runAnimation("blueWorm", true);
    return enemy;
}
///////////////////////////////////////
// LeapWorm
///////////////////////////////////////
LeapWorm* LeapWorm::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    LeapWorm* enemy = new LeapWorm();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_MOVE, ENEMY_ACTION_MOVE, monitorDistance);
    enemy->runAnimation("leapWorm", true);
    return enemy;
}
///////////////////////////////////////
// TreeMan
///////////////////////////////////////
void TreeMan::movingSchedule(float dt){
    float duration = 0.5f;
    this->extraSpeed = -originalSpeed*20;
    this->runAction(Sequence::create(ScaleTo::create(duration/2, 3, 1), ScaleTo::create(duration/2, 1, 1), NULL));
    this->scheduleOnce(schedule_selector(TreeMan::turnIntoNormal), duration);
    
}
void TreeMan::turnIntoNormal(float dt){
    this->extraSpeed = originalSpeed;
}
void TreeMan::runSchedule(){
    this->schedule(schedule_selector(TreeMan::movingSchedule), 2);
}

TreeMan* TreeMan::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    TreeMan* enemy = new TreeMan();
    
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_MOVE, ENEMY_ACTION_MOVE, monitorDistance);
    enemy->wantsToStop = false;
    enemy->originalSpeed = extraSpd;
    enemy->runAnimation("treeMan", true);
    return enemy;
}

///////////////////////////////////////
// WalkingPenguin
///////////////////////////////////////
WalkingPenguin* WalkingPenguin::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    WalkingPenguin* enemy = new WalkingPenguin();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_MOVE, ENEMY_ACTION_MOVE, monitorDistance);
    enemy->wantsToStop = false;
    enemy->runAnimation("walkingPenguin", true);
    return enemy;
}

///////////////////////////////////////
// Wood
///////////////////////////////////////
void Wood::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        if(this->target->getPosition().x < this->getPosition().x && !isTowardLeft)
            this->setFlippedX(true);
        else if(this->target->getPosition().x > this->getPosition().x && isTowardLeft)
            this->setFlippedX(false);
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "msCloud.png");
        missile->isFregile = true;
        missile->setPosition(this->getPosition() + Point(isTowardLeft?-10:10,3));
        double angle = GameManager::getInstance()->getAngle(missile->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2));
        missile->movingAngle = angle + 180;
        missile->setFlippedX(isTowardLeft);
        missile->weight = 0.5f;
        this->missileArray.pushBack(missile);
        this->runAnimation("woodFire", false);
        
        this->getParent()->addChild(missile);
    }
}

void Wood::runSchedule(){
    this->schedule(schedule_selector(Wood::movingSchedule), 1.3);
}

Wood* Wood::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    Wood* enemy = new Wood();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToStop = true;
    enemy->wantsToGoLeft = false;
    enemy->wantsToGoRight = false;
    return enemy;
}

///////////////////////////////////////
// BeeHive
///////////////////////////////////////
void BeeHive::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        Missile* missile = Missile::createMissile(UNIT_MISSILE_CHASING, 0, 0, 100, "bee.png");
        missile->moveTypeBeforeFindHero = ENEMY_MOVE_CUSTOM;
        missile->moveTypeAfterFindHero = ENEMY_MOVE_CUSTOM;
        missile->isFregile = true;
        missile->movingAngle = isTowardLeft?180:0;
        missile->setPosition(this->getPosition() + Point(isTowardLeft?-5:5,3));
        missile->setFlippedX(isTowardLeft);
        missile->extraSpeed = -20;
        missile->energyBarExist = false;
        missile->secondTag = UNIT_BEE;
        missile->target = this->target;
        this->missileArray.pushBack(missile);
        
        this->getParent()->addChild(missile);
    }
}

void BeeHive::runSchedule(){
    this->schedule(schedule_selector(BeeHive::movingSchedule), 2.3);
}

BeeHive* BeeHive::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    BeeHive* enemy = new BeeHive();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToStop = true;
    return enemy;
}
///////////////////////////////////////
// Pumpkin
///////////////////////////////////////
void Pumpkin::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        if(this->target->getPosition().x < this->getPosition().x && !isTowardLeft)
            this->setFlippedX(true);
        else if(this->target->getPosition().x > this->getPosition().x && isTowardLeft)
            this->setFlippedX(false);
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "msPumpkin.png");
        missile->isFregile = true;
        missile->energyBarExist = false;
        Point extraPoint = Point(0,0);
        if(this->target->velocity.x > 0) extraPoint = Point(15, 0);
        if(this->target->velocity.x < 0) extraPoint = Point(-15, 0);
        missile->setPosition(this->getPosition() + Point(isTowardLeft?-5:5,3));
        double angle = GameManager::getInstance()->getAngle(missile->getPosition(), this->target->getPosition() + extraPoint + Point(0, TILE_SIZE/2));
        missile->movingAngle = angle + 180;
        missile->extraSpeed = 100;
        missile->setFlippedX(isTowardLeft);
        missile->weight = 0.1f;
        this->missileArray.pushBack(missile);
        this->runAnimation("pumpkinFire", false);
        
        this->getParent()->addChild(missile);
    }
}

void Pumpkin::runSchedule(){
    this->schedule(schedule_selector(Pumpkin::movingSchedule), 1.5);
}

Pumpkin* Pumpkin::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    Pumpkin* enemy = new Pumpkin();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToStop = true;
    enemy->wantsToGoLeft = false;
    enemy->wantsToGoRight = false;
    return enemy;
}
///////////////////////////////////////
// DungBeetle
///////////////////////////////////////
void DungBeetle::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound && dung) {
        if(this->target->getPosition().x < this->getPosition().x && !isTowardLeft)
            this->setFlippedX(true);
        else if(this->target->getPosition().x > this->getPosition().x && isTowardLeft)
            this->setFlippedX(false);
        
        this->removeChild(dung, true);
        dung = NULL;

        Missile* missile = Missile::createMissile(UNIT_MISSILE_DROPPABLE, 1, 0, 0, "dung.png");
        missile->isFregile = false;
        missile->setPosition(this->getPosition() + Point(isTowardLeft?-10:10, missile->getContentSize().height/2));
        missile->extraSpeed = 100;
        missile->weight = 0.5;
        missile->setFlippedX(isTowardLeft);
        missile->wantsToGoLeft = isTowardLeft;
        missile->wantsToGoRight = !isTowardLeft;
//        missile->setPosition(Point(this->getContentSize().width/2 + missile->getContentSize().width/2 - 1, missile->getContentSize().height/2));
        
        this->missileArray.pushBack(missile);
        missile->runAction(RepeatForever::create(RotateBy::create(0.7, isTowardLeft?-360:360)));
        this->getParent()->addChild(missile);
        
        this->runAnimation("dungBeetleWalking", true);
        this->unschedule(schedule_selector(DungBeetle::movingSchedule));
    }
}
void DungBeetle::encounteredCliff(int direction){
    this->setFlippedX(!this->isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
    if (dung == NULL) {
        return;
    }
    this->dung->runAction(RepeatForever::create(RotateBy::create(0.7, isTowardLeft?-360:360)));
    if (isTowardLeft) {
        dung->setPosition(Point(-this->dung->getContentSize().width/2 + 1, this->dung->getContentSize().height/2));
    }else{
        dung->setPosition(Point(this->getContentSize().width + this->dung->getContentSize().width/2 - 1, this->dung->getContentSize().height/2));
    }
}
void DungBeetle::encounteredWall(){
    this->setFlippedX(!this->isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
    if (dung == NULL) {
        return;
    }
    if (isTowardLeft) {
        dung->setPosition(Point(-this->dung->getContentSize().width/2 + 1, this->dung->getContentSize().height/2));
    }else{
        dung->setPosition(Point(this->getContentSize().width + this->dung->getContentSize().width/2 - 1, this->dung->getContentSize().height/2));
    }
}
void DungBeetle::runSchedule(){
    this->schedule(schedule_selector(DungBeetle::movingSchedule), 0.1);
}

DungBeetle* DungBeetle::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    DungBeetle* enemy = new DungBeetle();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToStop = false;
    enemy->wantsToGoLeft = true;
    enemy->wantsToGoRight = false;
    enemy->runAnimation("dungBeetleRolling", true);
    
    enemy->dung = Droppable::createDroppable(UNIT_MISSILE_DROPPABLE, 1, 0, "dung.png");
    enemy->dung->isFregile = false;
    enemy->dung->runAction(RepeatForever::create(RotateBy::create(0.7, -360)));
    enemy->dung->extraSpeed = 100;
    
    enemy->addChild(enemy->dung);
    enemy->dung->setPosition(Point(enemy->getContentSize().width + enemy->dung->getContentSize().width/2 - 1, enemy->dung->getContentSize().height/2));
    return enemy;
}
///////////////////////////////////////
// PinkRakoon
///////////////////////////////////////
void PinkRakoon::movingSchedule(float dt){
    if (targetFound) {

    }else{
        findTarget(target, monitoringDistance);
        
        if (this->isTargetFound) {
            this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("pinkRakoonDash.png"));
            this->setCurrentMoveType(ENEMY_MOVE_WANDER_CROSS_PLATFORM);
            this->extraSpeed = 100;
            this->weight = 0.5f;
            targetFound = true;
            
            this->setFlippedX(this->target->getPosition().x < this->getPosition().x);
            wantsToStop = false;
            wantsToGoLeft = isTowardLeft;
            wantsToGoRight = !isTowardLeft;
            this->unschedule(schedule_selector(PinkRakoon::movingSchedule));
        }
    }
}

void PinkRakoon::runSchedule(){
    targetFound = false;
    this->schedule(schedule_selector(PinkRakoon::movingSchedule), 0.1);
}

PinkRakoon* PinkRakoon::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    PinkRakoon* enemy = new PinkRakoon();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToStop = true;
    enemy->wantsToGoLeft = false;
    enemy->wantsToGoRight = false;
    return enemy;
}
///////////////////////////////////////
// FishMon
///////////////////////////////////////
void FishMon::movingSchedule(float dt){
    shootCoolTime -= dt;
    if (shootCoolTime > 0) {
        return;
    }
    shootCoolTime = 0.8 + (rand()%8 + 1)*0.1f;
    
    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "msFishMon.png");
    missile->isFregile = true;
    missile->movingAngle = 90;
    missile->energyBarExist = false;
    missile->setPosition(this->getPosition() + Point(0,8));
    missile->setFlippedX(isTowardLeft);
    missile->extraSpeed = 50;
    missile->weight = 0.1f;
    this->missileArray.pushBack(missile);
    this->runAnimation("fishMonFire", false);
    
    this->getParent()->addChild(missile);
}

void FishMon::runSchedule(){
    this->schedule(schedule_selector(FishMon::movingSchedule), 0.1);
}

FishMon* FishMon::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    FishMon* enemy = new FishMon();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToStop = true;
    enemy->wantsToGoRight = false;
    enemy->wantsToGoLeft = false;
    return enemy;
}


///////////////////////////////////////
// ForkPorkStand
///////////////////////////////////////
void ForkPorkStand::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        if(this->target->getPosition().x < this->getPosition().x && !isTowardLeft)
            this->setFlippedX(true);
        else if(this->target->getPosition().x > this->getPosition().x && isTowardLeft)
            this->setFlippedX(false);
        this->runAnimation("forkPorkFire", false);
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "fork.png");
        missile->movingAngle = isTowardLeft?225:-45;
        missile->setRotation(-missile->movingAngle);
        missile->setPosition(this->getPosition() + Point(isTowardLeft?-10:10, 0));
        missile->extraSpeed = 80;
        this->missileArray.pushBack(missile);
        this->getParent()->addChild(missile);
    }
}

void ForkPorkStand::runSchedule(){
    this->schedule(schedule_selector(ForkPorkStand::movingSchedule), 1.3);
}

ForkPorkStand* ForkPorkStand::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    ForkPorkStand* enemy = new ForkPorkStand();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToStop = true;
    enemy->wantsToGoLeft = false;
    enemy->wantsToGoRight = false;
    
    return enemy;
}
///////////////////////////////////////
// ForkPork
///////////////////////////////////////
void ForkPork::movingSchedule(float dt){
    if(attackIndex < 4){
        attackIndex++;
    }else{
        findTarget(target, monitoringDistance);
        
        if (this->isTargetFound) {
            
            this->setFlippedX(target->getPosition().x < this->getPosition().x);
            wantsToGoLeft = isTowardLeft;
            wantsToGoRight = !isTowardLeft;
            
            this->runAnimation("forkPorkFire", false);
            Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "fork.png");
            missile->movingAngle = isTowardLeft?180:0;
            missile->setRotation(-missile->movingAngle);
            missile->setPosition(this->getPosition() + Point(isTowardLeft?-10:10, 6));
            missile->extraSpeed = 80;
            this->missileArray.pushBack(missile);
            this->getParent()->addChild(missile);
            
            attackIndex = 0;
            wantsToStop = true;
        }else{
            this->velocity = Point(0, 100);
            wantsToStop = false;
            wantsToGoLeft = isTowardLeft;
            wantsToGoRight = !isTowardLeft;
            this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("forkPork1.png"));
        }
    }
}

void ForkPork::runSchedule(){
    this->schedule(schedule_selector(ForkPork::movingSchedule), 0.3);
}

ForkPork* ForkPork::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    ForkPork* enemy = new ForkPork();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToStop = false;
    return enemy;
}

void ForkPork::onLanded(){
    Droppable::onLanded();
    this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("forkPork0.png"));
    wantsToStop = true;
    
    this->wantsToGoLeft = false;
    this->wantsToGoRight = false;
}

void ForkPork::encounteredWall(){
    this->setFlippedX(!this->isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
///////////////////////////////////////
// Squirrel
///////////////////////////////////////
void Squirrel::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    
    if(attackIndex < 2){
        attackIndex++;
    }else{
        findTarget(target, monitoringDistance);
        
        if (this->isTargetFound) {
            this->setFlippedX(target->getPosition().x < this->getPosition().x);
            wantsToGoLeft = isTowardLeft;
            wantsToGoRight = !isTowardLeft;
            int force = 80;
            Missile* missile = Missile::createMissile(UNIT_MISSILE_DROPPABLE, 1, 0, 0, "dotori.png");
            missile->velocity = Point(isTowardLeft?-force:force, force);
            missile->isFregile = true;
            missile->setPosition(this->getPosition());
            missile->weight = 0.15f;
            this->missileArray.pushBack(missile);
            this->runAnimation("squirrelFire", false);
            
            this->getParent()->addChild(missile);
            isTargetFound = false;
            attackIndex = 0;
            wantsToStop = true;
        }else{
            this->velocity = Point(0, 120);
            jumped = true;
            onGround = false;
            wantsToStop = false;
            wantsToGoLeft = isTowardLeft;
            wantsToGoRight = !isTowardLeft;
            
            this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("squirrelJump.png"));
        }
    }
    
}

void Squirrel::runSchedule(){
    this->schedule(schedule_selector(Squirrel::movingSchedule), 0.5);
}

Squirrel* Squirrel::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    Squirrel* enemy = new Squirrel();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}

void Squirrel::onLanded(){
    Droppable::onLanded();
    this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("squirrel0.png"));
    wantsToStop = true;
    this->wantsToGoLeft = false;
    this->wantsToGoRight = false;
}

void Squirrel::encounteredWall(){
    this->setFlippedX(!this->isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
///////////////////////////////////////
// DJMonkey
///////////////////////////////////////
void DJMonkey::movingSchedule(float dt){
    if(!isTargetFound)
        findTarget(target, monitoringDistance);
    
    if (this->isTargetFound) {
        if(this->target->getPosition().x < this->getPosition().x && !isTowardLeft)
            this->setFlippedX(true);
        else if(this->target->getPosition().x > this->getPosition().x && isTowardLeft)
            this->setFlippedX(false);
        Missile* missile = Missile::createMissile(UNIT_MISSILE_CUSTOM, 1, 0, 0, "musicNote.png");
//        CCLOG("DJ pos: %f, %f", this->getPosition().x, this->getPosition().y);
        int gap = rand()%32;
        missile->setPosition(this->getPosition() + Point(isTowardLeft?-gap:gap, 0));
        missile->extraSpeed = 80;
        float duration = 1;
        int x = this->isFlippedX()?-40:40;
        int y = 30;
//        float ease = 2.0f;
//        missile->runAction(RepeatForever::create(Sequence::create(EaseOut::create(MoveBy::create(duration, Point(0, y)), ease), EaseIn::create(MoveBy::create(duration, Point(0, -y)), ease) , EaseOut::create(MoveBy::create(duration, Point(0, -y)), ease) , EaseIn::create(MoveBy::create(duration, Point(0, y)), ease), NULL)));
//        missile->runAction(RepeatForever::create(Sequence::create(MoveBy::create(duration, Point(x, 0)), NULL)));
        missile->runAction(RepeatForever::create(Sequence::create(JumpBy::create(duration/2, Point(x, 0), y, 1), JumpBy::create(duration/2, Point(x, 0), -y, 1), NULL)));
        this->missileArray.pushBack(missile);
        this->getParent()->addChild(missile);
    }
}

void DJMonkey::runSchedule(){
    this->schedule(schedule_selector(DJMonkey::movingSchedule), 1.3);
}

DJMonkey* DJMonkey::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    DJMonkey* enemy = new DJMonkey();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToStop = true;
    return enemy;
}

///////////////////////////////////////
// Frog
///////////////////////////////////////
void Frog::movingSchedule(float dt){
    findTarget(target, monitoringDistance);
    if(isAttacking){
        
    }else{
        if (this->isTargetFound) {
            isAttacking = true;
            this->tung->setVisible(true);
            this->tung->setFlippedX(target->getPosition().x < this->getPosition().x);
            this->setFlippedX(target->getPosition().x < this->getPosition().x);
            wantsToGoLeft = isTowardLeft;
            wantsToGoRight = !isTowardLeft;
            wantsToStop = true;
            this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("frogFire.png"));
            if(isTowardLeft){
                this->tung->setAnchorPoint(Point(1, 0.5));
                this->tung->setPosition(Point(1, 10));
            }else{
                this->tung->setAnchorPoint(Point(0, 0.5));
                this->tung->setPosition(Point(15, 10));
            }
            
            this->tung->setScale(0.1, 1);
            this->tung->runAction(Sequence::create(ScaleTo::create(0.1, 1, 1), DelayTime::create(0.2), ScaleTo::create(0.1, 0.1, 1), CallFuncN::create(CC_CALLBACK_1(Frog::tungTakeDownDone, this)), NULL));
            isTargetFound = false;
        }else{
            this->velocity = Point(0, 120);
            wantsToStop = false;
            wantsToGoLeft = isTowardLeft;
            wantsToGoRight = !isTowardLeft;
            
            this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("frogJump.png"));
        }
    }
    this->scheduleOnce(schedule_selector(Frog::forceLanding), 0.3);
}

void Frog::tungFireDone(Ref* ref){
    
}
void Frog::tungTakeDownDone(Ref* ref){
    this->tung->setVisible(false);
    isAttacking = false;
    this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("frog0.png"));
}

void Frog::runSchedule(){
    this->scheduleOnce(schedule_selector(Frog::movingSchedule), 1);
}

Frog* Frog::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    Frog* enemy = new Frog();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->tung = Sprite::createWithSpriteFrameName("frogTung.png");
    enemy->addChild(enemy->tung);
    enemy->tung->setVisible(false);
    return enemy;
}

Rect Frog::damageBoundingBox(){
    if (isAttacking) {
        Rect rect = this->tung->getBoundingBox();
        rect = RectInset(rect, 1, 1);
        rect = RectOffset(rect, this->getPosition().x, this->getPosition().y);
        return rect;
    }else{
        return this->Droppable::damageBoundingBox();
    }
}
void Frog::onLanded(){
    Droppable::onLanded();
    this->runAction(Sequence::create(ScaleTo::create(0.1, 1, 0.9), ScaleTo::create(0.1, 1, 1.05), ScaleTo::create(0.05, 1, 1), NULL));
    this->wantsToStop = true;
    this->wantsToGoLeft = false;
    this->wantsToGoRight = false;
    this->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("frog0.png"));
    this->unschedule(schedule_selector(Frog::movingSchedule));
    this->scheduleOnce(schedule_selector(Frog::movingSchedule), 1);
}
void Frog::forceLanding(float dt){
    this->onLanded();
}
void Frog::encounteredWall(){
    this->setFlippedX(!this->isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
void Frog::encounteredCliff(int direction){
    
}

///////////////////////////////////////
// RandomBox
///////////////////////////////////////
RandomBox* RandomBox::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    RandomBox* enemy = new RandomBox();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->wantsToStop = true;
    return enemy;
}

///////////////////////////////////////
// EnemyWithGun
///////////////////////////////////////
void EnemyWithGun::startJumping(){
    this->schedule(schedule_selector(EnemyWithGun::jumping), 0.2);
}
void EnemyWithGun::jumping(float dt){
    jumpTimeElapse -= dt;
    if (jumpTimeElapse < 0) {
        jumpTimeElapse = 0.5f + (rand()%20)*0.1f;
        wantsToJump = true;
        buttonReleased = true;
    }
}
void EnemyWithGun::movingSchedule(float dt){
    if(!this->isVisible()){
        return;
    }
    findTarget(target, monitoringDistance);
    
    
    if (this->isTargetFound && this->currentMoveType != this->moveTypeAfterFindHero) {
        this->setCurrentMoveType(this->moveTypeAfterFindHero);
    }
    
    if (this->isTargetFound) {
        elapsedReloadingTime -= dt;
        double angle = -GameManager::getInstance()->getAngle(this->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2)) + 180;
        if (sptGun != nullptr) {
            
        }
        
        Point msPos = Point::ZERO;
        setCurrentMoveType(moveTypeAfterFindHero);
        float msStartPoint = 0;
        if (sptGun) {
            float radius = getContentSize().width/2 + 2;
            msStartPoint = radius + sptGun->getContentSize().width;
            sptGun->setRotation(angle);
            float x = cos(angle*3.14/180)*radius;
            float y = -sin(angle*3.14/180)*radius;
            Point center = Point(this->getContentSize().width/2, getContentSize().height/2);
            Point gunPos = center + Point(x, y);
            sptGun->setPosition(gunPos);
            
            if (angle < 270 && angle > 90) {
                sptGun->setFlippedY(true);
            }else{
                sptGun->setFlippedY(false);
            }
        }
        
        if (missileType == MISSILE_NONE) {
            return;
        }
        
        if (elapsedReloadingTime < 0) {
            if (loadedBulletCount <= 0) {
                elapsedReloadingTime = maxReloadingTime;
                loadedBulletCount = maxLoadedBulletCount;
            }
            loadedBulletCount--;
            
            GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
            Point gunTipPos = this->getPosition() + Point(cos(angle*3.14/180)*msStartPoint, -sin(angle*3.14/180)*msStartPoint);
            if (missileType == MISSILE_SPIRAL) {
                float radius = 10;
                int addAngle = 30;
                gunTipPos = this->getPosition() + Point(isTowardLeft?-20:4, 16);
                if (energy < maxEnergy/3) {
                    addAngle = 15;
                }
                Point extraPos = Point(-10, 40);
                for (int i = 0; i < 360; i += addAngle) {
                    SpinMissile* missile = SpinMissile::create(UNIT_MISSILE_STRAIGHT, "bombSmallEffect0.png", Point(cos(i*3.14/180)*radius, sin(i*3.14/180)*radius), gunTipPos);
                    missile->isFregile = true;
                    missile->extraSpeed = 0;
                    missile->weight = 0.1f;
                    missile->setScale(0.1);
                    missile->angleVar = 50;
                    this->missileArray.pushBack(missile);
                    this->getParent()->addChild(missile);
                }
            }else{
                int missileCount = 1;
                if (missileType == MISSILE_ARC ||
                    missileType == MISSILE_COLUMN) {
                    missileCount = 5;
                }else if (missileType == MISSILE_CIRCLE ||
                          missileType == MISSILE_FOUNTAIN){
                    missileCount = 10;
                }else if(missileType == MISSILE_RANDOM){
                    missileCount = 6;
                }
                for (int i = 0; i < missileCount; i++) {
                    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "bombSmallEffect0.png");
                    missile->isFregile = true;
                    missile->movingAngle = -angle;
                    missile->extraSpeed = bulletSpeed;
                    
                    if (missileType == MISSILE_ARC) {
                        missile->movingAngle = -angle - 70 + i*28;
                        missile->extraSpeed = 60;
                    }else if(missileType == MISSILE_SINGLE){
                        
                    }else if(missileType == MISSILE_RANDOM){
                        int radious = 30;
                        missile->runAction(Sequence::create(DelayTime::create(i*0.2), MoveTo::create(0.2, Point(gunTipPos + Point(radious - rand()%(radious*2), 50 + radious/2 - rand()%(radious)))), nullptr));
                        missile->freezeForAWhile(missileCount*0.2 + 0.5);
                        missile->target = this->target;
                        missile->fixAngleToTargetWhenReleaseFreezing = true;
                    }else if(missileType == MISSILE_FOUNTAIN){
                        gunTipPos = this->getPosition() + Point(isTowardLeft?-10:10, 30);
                        int radious = 40;
                        int height = 20;
                        missile->runAction(Sequence::create(DelayTime::create(i*0.2), JumpBy::create(0.5 + 0.1*i, Point(radious - rand()%(radious*2), -200), height, 1), nullptr));
                        missile->freezeForAWhile(missileCount*0.2 + 0.7);
                        missile->target = this->target;
                        missile->fixAngleToTargetWhenReleaseFreezing = true;
                    }else if(missileType == MISSILE_CIRCLE){
                        float radious = 20;
                        float circleAngle = 360.0f*i/missileCount;
                        gunTipPos = this->getPosition();
                        missile->runAction(Sequence::create(DelayTime::create(i*0.1), MoveTo::create(0.2, Point(this->getPosition() + Point(0, 30) + Point(cos(circleAngle*3.14/180)*radious, -sin(circleAngle*3.14/180)*radious))), nullptr));
                        missile->freezeForAWhile(missileCount*0.1 + 0.5);
                        missile->target = this->target;
                        missile->fixAngleToTargetWhenReleaseFreezing = true;
                    }else if(missileType == MISSILE_COLUMN){
                        gunTipPos = this->getPosition();
                        missile->runAction(Sequence::create(DelayTime::create(i*0.2), MoveTo::create(0.2, Point(this->getPosition() + Point(0, 20 + 10*i))), nullptr));
                        missile->freezeForAWhile(missileCount*0.2 + 0.5);
                        missile->target = this->target;
                        missile->fixAngleToTargetWhenReleaseFreezing = true;
                    }
                    missile->setPosition(gunTipPos);
                    
                    missile->weight = 0.1f;
                    missile->setScale(0.1);
                    this->missileArray.pushBack(missile);
                    this->getParent()->addChild(missile);
                }
                
                if (missileType == MISSILE_ARC) {
                    
                }else if(missileType == MISSILE_SINGLE){
                    
                }else if(missileType == MISSILE_RANDOM){
                    this->freezeForAWhile(2);
                }else if(missileType == MISSILE_FOUNTAIN){
                    this->freezeForAWhile(3);
                    if (missileCount == 1) {
                        this->runAnimation("pierrotTall", false);
                    }else{
                        this->runAnimation("pierrotBlueTall", false);
                    }
                }else if(missileType == MISSILE_CIRCLE){
                    this->freezeForAWhile(2);
                }else if(missileType == MISSILE_COLUMN){
                    this->freezeForAWhile(2);
                }
            }
            
            if (missileType == MISSILE_SINGLE ||
                missileType == MISSILE_ARC) {
                Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
                this->getParent()->addChild(explode);
                explode->setPosition(gunTipPos);
                explode->setAnchorPoint(Point(1, 0.5));
                Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
                Animate* animate = Animate::create(animation);
                explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
                explode->setRotation(angle + 180);
                if (rand()%2 == 0) {
                    explode->setFlippedY(true);
                }
            }
            
            bool isTargetOnLeft = this->getPosition().x > target->getPosition().x;
            setFlippedX(isTargetOnLeft);
            if (currentMoveType == ENEMY_MOVE_CROSS_TO_TARGET) {
                
                wantsToGoLeft = isTargetOnLeft;
                wantsToGoRight = !isTargetOnLeft;
                
            }
            if (currentMoveType == ENEMY_MOVE_TO_CLIFF) {
                //bool isTargetOnLeft = this->getPosition().x > target->getPosition().x;
                //setFlippedX(isTargetOnLeft);
                if ((cliffStatus == CLIFF_LEFT && !isTargetOnLeft) || (cliffStatus == CLIFF_RIGHT && isTargetOnLeft) || !wantsToStop) {
                    wantsToStop = false;
                    wantsToGoLeft = isTargetOnLeft;
                    wantsToGoRight = !isTargetOnLeft;
                }
            }
            
            if (fireAniName.size() > 0) {
                runAnimation(fireAniName.c_str(), false);
            }
        }
    }
}

void EnemyWithGun::runSchedule(){
    this->schedule(schedule_selector(EnemyWithGun::movingSchedule), 0.2);
}

void EnemyWithGun::setGunInfo(int bulletCount, float reloadingTime, int gun, int speed){
    maxLoadedBulletCount = bulletCount;
    loadedBulletCount = maxLoadedBulletCount;
    maxReloadingTime = reloadingTime;
    elapsedReloadingTime = 1;
    if (gun == ENEMY_GUN_TYPE_HANDGUN) {
        gunType = gun;
        sptGun = Sprite::createWithSpriteFrameName("pierrotGun.png");
        this->addChild(sptGun);
        sptGun->setAnchorPoint(Point(0, 0.5));
        sptGun->setPosition(Point(this->getContentSize().width, this->getContentSize().height/2));
    }
    bulletSpeed = speed;
    if (missileType == MISSILE_CIRCLE ||
        missileType == MISSILE_COLUMN) {
        sptGun->setVisible(false);
    }
}
EnemyWithGun* EnemyWithGun::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    EnemyWithGun* enemy = new EnemyWithGun();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}
void EnemyWithGun::encounteredCliff(int direction){
    if (currentMoveType == ENEMY_MOVE_TO_CLIFF) {
        wantsToGoLeft = false;
        wantsToGoRight = false;
        wantsToStop = true;
        cliffStatus = direction;
    }else if (currentMoveType == ENEMY_MOVE_WANDER_CROSS_PLATFORM ||
              currentMoveType == ENEMY_MOVE_CROSS_TO_TARGET) {
        
    }else{
        setFlippedX(!isFlippedX());
        wantsToGoLeft = isTowardLeft;
        wantsToGoRight = !isTowardLeft;
    }
}
void EnemyWithGun::encounteredWall(){
    setFlippedX(!isFlippedX());
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
void EnemyWithGun::setFlippedX(bool flip){
    Droppable::setFlippedX(flip);
    if (sptGun && !isTargetFound) {
        float angle = flip?180:0;
        float radius = getContentSize().width/2;
        sptGun->setRotation(angle);
        float x = cos(angle*3.14/180)*radius;
        float y = -sin(angle*3.14/180)*radius;
        Point center = Point(this->getContentSize().width/2, getContentSize().height/2);
        Point gunPos = center + Point(x, y);
        sptGun->setPosition(gunPos);
        
        if (angle < -90 || angle > 90) {
            sptGun->setFlippedY(true);
        }else{
            sptGun->setFlippedY(false);
        }
    }
}

NPCBase::NPCBase(){}
NPCBase::~NPCBase(){}

NPCBase* NPCBase::create(std::string sptName){
    NPCBase* npcBase = new NPCBase();
    npcBase->init(UNIT_NPC, 1, 80, sptName.c_str());
    return npcBase;
}
std::string NPCBase::getTalk(){
    return "";
}
// TheKnight

//TheKnight::TheKnight(){}
//TheKnight::~TheKnight(){}
TheKnight* TheKnight::create(){
    TheKnight* knight = new TheKnight();
    knight->initKnight();
    return knight;
}
void TheKnight::initKnight() {
    Node* skeleton = CSLoader::createNode("knight/knightSkeleton.csb");
    this->addChild(skeleton);
    Size knightSize = skeleton->getBoundingBox().size;
    this->setContentSize(knightSize);
    this->setAnchorPoint(Point(0.5, 0));
    skeleton->setPosition(this->getContentSize()/2);
    animation = CSLoader::createTimeline("knight/knightSkeleton.csb");
    skeleton->runAction(animation);
    animation->gotoFrameAndPause(0);
}
void TheKnight::doKnightAction(int action){
    state = action;
    if (action == KNIGHT_SWING_LEFT_BOTTOM) {
        animation->play("swingBottomLeft", false);
    }else if(action == KNIGHT_SWING_BACK_FROM_LEFT_BOTTOM){
        animation->play("swingBackFromBottomLeft", false);
    }
}
std::string TheKnight::getTalk(){
    bool isAllClear = UserDefault::getInstance()->getBoolForKey(KEY_IS_ALL_CLEAR, false);
    
    int index=0;
    if (UserDefault::getInstance()->getBoolForKey(KEY_IS_TUTORIAL_CLEAR, false)) {
        if (isAllClear) {
            index = talkIndex + 3;  // 3, 4, 5
        }else{
            index = 2;
            talkIndex = 1; // keep index 2
        }
    }else{
        index = talkIndex;
        if (talkIndex > 0) { // keep index 1
            talkIndex = 0;
        }
    }
    talkIndex++;
    return LanguageManager::getInstance()->getText(__String::createWithFormat("knight%d", index)->getCString());
}
// TutorialGhost
TutorialGhost* TutorialGhost::create(){
    UserDefault::getInstance()->setIntegerForKey(KEY_TUTORIAL_GHOST_IDNEX, -1);
    
    TutorialGhost* ghost = new TutorialGhost();
    ghost->init(UNIT_HOOD_GHOST, 50, 80, "tGhost0.png");
    ghost->setOpacity(100);
    
    ghost->sptTalkBalloon = ImageView::create("talkBox.png", Widget::TextureResType::PLIST);
    ghost->addChild(ghost->sptTalkBalloon);
    ghost->sptTalkBalloon->setCapInsets(Rect(3, 3, ghost->sptTalkBalloon->getContentSize().width - 6, ghost->sptTalkBalloon->getContentSize().height - 6));
    ghost->sptTalkBalloon->setScale9Enabled(true);
    ghost->sptTalkBalloon->setAnchorPoint(Point(0.5, 0));
    ghost->sptTalkBalloon->setPosition(Point(ghost->getContentSize().width/2, ghost->getContentSize().height + 6));
    
    Sprite* point = Sprite::createWithSpriteFrameName("talkBoxPoint.png");
    ghost->addChild(point);
    point->setAnchorPoint(Point(0.5, 1));
    point->setPosition(ghost->sptTalkBalloon->getPosition() + Point(0, 2));
    
    ghost->lblTalk = LanguageManager::getInstance()->getLocalizedLabel();
    ghost->lblTalk->setScale(0.18f);
    ghost->lblTalk->setColor(Color3B::BLACK);
    ghost->addChild(ghost->lblTalk);
    ghost->lblTalk->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
    ghost->runSchedule();
    return ghost;
}

void TutorialGhost::runSchedule(){
    this->schedule(schedule_selector(TutorialGhost::movingSchedule));
}
void TutorialGhost::movingSchedule(float dt){
    nextTalkTimeLeft -= dt;
    if (nextTalkTimeLeft < 0) {
        talk();
    }
    if(target != nullptr){
        Point targetPos = target->getPosition() + Point(0, 30);
        this->setPosition(targetPos.lerp(this->getPosition(), 0.96f));
        this->setFlippedX(targetPos.x > this->getPositionX());
    }
}
void TutorialGhost::talk(){
//    if (sptTalkBalloon != nullptr) {
//        sptTalkBalloon->setVisible(false);
//    }
    
    int index = UserDefault::getInstance()->getIntegerForKey(KEY_TUTORIAL_GHOST_IDNEX, -1);
    index++;
    if (index > 18) {
        index = 0;
    }
    UserDefault::getInstance()->setIntegerForKey(KEY_TUTORIAL_GHOST_IDNEX, index);
    std::string strKey = "tutorial_ghost_";
    if (index >= 0 && index <= 5) {
        strKey = StringUtils::format("tutorial_ghost_intro_%d", index);
    }else if (index >= 6 && index <= 12) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        strKey = StringUtils::format("tutorial_ghost_mobile_%d", index-6);
#else
        strKey = StringUtils::format("tutorial_ghost_pc_%d", index-6);
#endif
        
    }else if (index >= 13 && index <= 18) {
        strKey = StringUtils::format("tutorial_ghost_general_%d", index-13);
    }
    
    lblTalk->setString(LanguageManager::getInstance()->getText(strKey.c_str()));
    nextTalkTimeLeft = lblTalk->getStringLength()/10;
    Size size = Size(lblTalk->getContentSize().width*lblTalk->getScale(), lblTalk->getContentSize().height*lblTalk->getScale());
    float labelWidth = 600;
    if (lblTalk->getContentSize().width > labelWidth) {
        lblTalk->setWidth(labelWidth);
    }
    size = Size(lblTalk->getContentSize().width*lblTalk->getScale(), lblTalk->getContentSize().height*lblTalk->getScale());
    float padding = 5;
    lblTalk->setPosition(sptTalkBalloon->getPosition() + Point(0, padding/2 + size.height/2));
    
    sptTalkBalloon->setContentSize(Size(size.width + padding, size.height + padding));
}


// HoodGhost
HoodGhost* HoodGhost::create(){
    HoodGhost* ghost = new HoodGhost();
    ghost->init(UNIT_HOOD_GHOST, 1000, 80, "hoodGhost0.png");
    ghost->sptTalkBalloon = Sprite::createWithSpriteFrameName("exclaim.png");
    ghost->addChild(ghost->sptTalkBalloon);
    ghost->sptTalkBalloon->setAnchorPoint(Point(0.5, 0));
    ghost->sptTalkBalloon->setPosition(Point(ghost->getContentSize().width/2, ghost->getContentSize().height + 4));
    
    std::string key = ghost->getKeyToTalk();
    if(!UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_DOES_IT_TALKED_FORMAT, key.c_str())->getCString(), false)){
        ghost->sptTalkBalloon->setVisible(true);
    }else{
        ghost->sptTalkBalloon->setVisible(false);
    }
    
    return ghost;
}
std::string HoodGhost::getKeyToTalk(){
    std::string talk;

    if (GameManager::getInstance()->currentStageIndex == -1) {
        bool isAllClear = UserDefault::getInstance()->getIntegerForKey(KEY_CLEAR_COUNT, 0) > 0;
        if (isAllClear) {
            int collectedJewel = GameManager::getInstance()->collectedJewelIndex;
            int storyIndex = -1;
            if (collectedJewel >= 0) {
                storyIndex = collectedJewel + 1;
            }
            
            bool jewelCollected0 = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 0)->getCString());
            bool jewelCollected1 = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 1)->getCString());
            bool jewelCollected2 = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 2)->getCString());
            bool jewelCollected3 = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString());
            if (jewelCollected3) {
                storyIndex = 4;
            }else if (jewelCollected2) {
                storyIndex = 3;
            }else if (jewelCollected1) {
                storyIndex = 2;
            }else if (jewelCollected0) {
                storyIndex = 1;
            }else{
                storyIndex = 0;
            }
            UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, storyIndex-1)->getCString(), true);
            return __String::createWithFormat("story%d", storyIndex)->getCString();
        }else{
            std::string strToReturn;
            if (UserDefault::getInstance()->getBoolForKey(KEY_IS_TUTORIAL_CLEAR, false)) {
                strToReturn = __String::createWithFormat("readyToEnter%d", talkIndex)->getCString();
                if (talkIndex >= 2) {
                    talkIndex = 0;
                }
                return strToReturn;
            }
            strToReturn = __String::createWithFormat("welcome%d", talkIndex)->getCString();
            if (talkIndex >= 4) {
                talkIndex = 0;
            }
            return strToReturn;
        }
    }else if(GameManager::getInstance()->currentStageIndex >= TUTORIAL_STAGE_START && GameManager::getInstance()->currentStageIndex <= TUTORIAL_STAGE_END ) {
        return __String::createWithFormat("tutorial%d", GameManager::getInstance()->currentStageIndex - TUTORIAL_STAGE_START)->getCString();
    }
}
std::string HoodGhost::getTalk(){
    if (sptTalkBalloon != nullptr) {
        sptTalkBalloon->setVisible(false);
    }
    std::string key = getKeyToTalk();
    talkIndex++;
    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_DOES_IT_TALKED_FORMAT, key.c_str())->getCString(), true);
    return LanguageManager::getInstance()->getText(key.c_str());
    bool isKnightClear = UserDefault::getInstance()->getBoolForKey(KEY_IS_KNIGHT_CLEAR, false);
    std::string talk;
    if (GameManager::getInstance()->currentStageIndex == -1) {
        if (!isKnightClear) {
            if (UserDefault::getInstance()->getBoolForKey(KEY_IS_TUTORIAL_CLEAR, false)) {
                return LanguageManager::getInstance()->getText("readyToEnter");
            }
            int startIndex = 0;
            int endIndex = 2;
            if (talkIndex == 1) {
                startIndex = 3;
                endIndex = 4;
            }else if(talkIndex == 2){
                startIndex = 5;
                endIndex = 5;
            }else if(talkIndex >= 3){
                startIndex = 6;
                endIndex = 6;
            }
            for (int i = startIndex; i < endIndex + 1; i++) {
                talk.append(LanguageManager::getInstance()->getText(__String::createWithFormat("welcome%d", i)->getCString()));
                if (i != endIndex) {
                    talk.append("|");
                }
            }
        }else{
            talk.append(LanguageManager::getInstance()->getText(__String::createWithFormat("welcome%d", talkIndex + 7)->getCString()));
            talkIndex = 0;
        }
    }else if(GameManager::getInstance()->currentStageIndex >= TUTORIAL_STAGE_START && GameManager::getInstance()->currentStageIndex <= TUTORIAL_STAGE_END ) {
        talk.append(LanguageManager::getInstance()->getText(__String::createWithFormat("tutorial%d", GameManager::getInstance()->currentStageIndex - TUTORIAL_STAGE_START)->getCString()));
    }
    
    talkIndex++;
    return talk;
}

// Seller
Seller* Seller::create(){
    Seller* ghost = new Seller();
    ghost->init(UNIT_NPC, 1000, 80, "seller0.png");
    return ghost;
}
std::string Seller::getTalk(){
    std::string talk;
    for (int i = 0; i < 5; i++) {
        talk.append(LanguageManager::getInstance()->getText(__String::createWithFormat("seller%d", i)->getCString()) + "|");
    }
    return talk;
}

// OnSale
OnSale* OnSale::create(int theItem){
    OnSale* ghost = new OnSale();
    ghost->init(UNIT_NPC, 1000, 80, "onSale.png");
    ghost->setItem(theItem);
    return ghost;
}
void OnSale::setItem(int theItem){
    item = theItem;
    isReadyToSale = true;
    price = 100;
    const char* itemName;
    float scale = 0.25f;
    float itemScale = 0.5f;
    bool isAni = false;
    if (item == SELLING_ITEM_KEY) {
        price = 90;
        itemName = "blueKey.png";
    }else if (item == SELLING_ITEM_AMMO) {
        price = 80;
        itemName = "ammo.png";
        itemScale = 1;
    }else if (item == SELLING_ITEM_HEART) {
        price = 120;
        itemName = "heart.png";
    }else if (item == SELLING_ITEM_DRAGON_BABY) {
        price = 500;
        itemName = "dora_0_0.png";
        isAni = true;
    }else if (item == SELLING_ITEM_DRAGON_ADOLESCENT) {
        price = 600;
        itemName = "dora_1_0.png";
        isAni = true;
    }else if (item == SELLING_ITEM_DRAGON_ADULT) {
        price = 700;
        itemName = "dora_2_0.png";
        isAni = true;
    }else if (item == SELLING_ITEM_DRAGON_SENIOR) {
        price = 1000;
        itemName = "dora_3_0.png";
        isAni = true;
    }else if (item >= SELLING_ITEM_WEAPON) {
        price = 50 + item*30;
        itemScale = 0.5f;
        itemName = GameManager::getInstance()->getWeaponImageFileName(item);
    }
    
    Droppable* sptItem = Droppable::createDroppable(0, 0, 0, itemName);
    if (isAni) {
        sptItem->runAnimation(itemName, true);
    }
    
    this->addChild(sptItem);
    sptItem->setScale(itemScale);
    sptItem->setTag(77);
    sptItem->setPosition(Point(27, 16));
    priceTag = Node::create();
    this->addChild(priceTag);
    Label* lblPrice = LanguageManager::getInstance()->getLocalizedLabel(Value(price).asString().c_str(), Color4B::YELLOW);
    priceTag->addChild(lblPrice);
    lblPrice->setAnchorPoint(Point(1, 0));
    lblPrice->setScale(0.25f);
    lblPrice->setPosition(this->getContentSize());
    Sprite* sptCoin = Sprite::createWithSpriteFrameName("silverCoin0.png");
    priceTag->addChild(sptCoin);
    sptCoin->setScale(0.5);
    sptCoin->setPosition(lblPrice->getPosition() + Point(-lblPrice->getBoundingBox().size.width -2, lblPrice->getBoundingBox().size.height/2));
    sptCoin->setAnchorPoint(Point(1, 0.5));
    int height = sptCoin->getContentSize().height > lblPrice->getContentSize().height?sptCoin->getContentSize().height:lblPrice->getContentSize().height;
    DrawNode* dnTagBack = DrawNode::create();
    dnTagBack->drawSolidRect(sptCoin->getPosition() + Point(-sptCoin->getContentSize().width-2, -height/2-2), lblPrice->getPosition() + Point(2, height/2 + 2), Color4F(0,0,0,0.5));
    
}
void OnSale::buyItem(){
    int coin = GameManager::getInstance()->getCoin();
    if (coin >= price) {
        this->removeChildByTag(77);
        this->setSpriteFrame("soldOut.png");
        GameManager::getInstance()->addCoin(-price);
        GameManager::getInstance()->playSoundEffect(SOUND_COIN);
        GameManager::getInstance()->playSoundEffect(SOUND_POWER_UP_45);
        GameManager::getInstance()->getHudLayer()->updateCoin(GameManager::getInstance()->getCoin());
        this->setTag(UNIT_SOLDOUT);
        if (item == SELLING_ITEM_KEY) {
            GameManager::getInstance()->collectedKeyCount++;
            GameManager::getInstance()->getHudLayer()->keyCollected(GameManager::getInstance()->collectedKeyCount);
        }else if(item == SELLING_ITEM_HEART){
            Hero* player = GameManager::getInstance()->getCurrentStageLayer()->player;
            player->energy += player->maxEnergy;
            
            GameManager::getInstance()->getHudLayer()->setLife(GameManager::getInstance()->getCurrentStageLayer()->player->energy);
        }else if(item == SELLING_ITEM_AMMO){
            GameManager::getInstance()->getCurrentStageLayer()->recharge(GameManager::getInstance()->getCurrentStageLayer()->currentSlot);
        }else if(item == SELLING_ITEM_DRAGON_BABY){
            HelloWorld* stage = GameManager::getInstance()->getCurrentStageLayer();
            Buddy* skyBuddy = GameManager::getInstance()->getPet(PET_DORA_0);
            skyBuddy->setPosition(stage->player->getPosition() + Point(10, 0));
            stage->spriteBatch->addChild(skyBuddy, 9);
            skyBuddy->setFlippedX(false);
            stage->buddyArray.pushBack(skyBuddy);
            stage->skyBuddy = skyBuddy;
            GameManager::getInstance()->currentPet = PET_DORA_0;
        }else if(item == SELLING_ITEM_DRAGON_ADOLESCENT){
            HelloWorld* stage = GameManager::getInstance()->getCurrentStageLayer();
            Buddy* skyBuddy = GameManager::getInstance()->getPet(PET_DORA_1);
            skyBuddy->setPosition(stage->player->getPosition() + Point(10, 0));
            stage->spriteBatch->addChild(skyBuddy, 9);
            skyBuddy->setFlippedX(false);
            stage->buddyArray.pushBack(skyBuddy);
            stage->skyBuddy = skyBuddy;
            GameManager::getInstance()->currentPet = PET_DORA_1;
        }else if(item == SELLING_ITEM_DRAGON_ADULT){
            HelloWorld* stage = GameManager::getInstance()->getCurrentStageLayer();
            Buddy* skyBuddy = GameManager::getInstance()->getPet(PET_DORA_2);
            skyBuddy->setPosition(stage->player->getPosition() + Point(10, 0));
            stage->spriteBatch->addChild(skyBuddy, 9);
            skyBuddy->setFlippedX(false);
            stage->buddyArray.pushBack(skyBuddy);
            stage->skyBuddy = skyBuddy;
            GameManager::getInstance()->currentPet = PET_DORA_2;
        }else if(item == SELLING_ITEM_DRAGON_SENIOR){
            HelloWorld* stage = GameManager::getInstance()->getCurrentStageLayer();
            Buddy* skyBuddy = GameManager::getInstance()->getPet(PET_DORA_3);
            skyBuddy->setPosition(stage->player->getPosition() + Point(10, 0));
            stage->spriteBatch->addChild(skyBuddy, 9);
            skyBuddy->setFlippedX(false);
            stage->buddyArray.pushBack(skyBuddy);
            stage->skyBuddy = skyBuddy;
            GameManager::getInstance()->currentPet = PET_DORA_3;
        }else if(item >= SELLING_ITEM_WEAPON){
            GameManager::getInstance()->getCurrentStageLayer()->addWeapon(item);
            GM->addBagContent(item, 0);
            GM->setItemCollectionFound(item%1000, COLLECTION_FOUND);
            GM->saveBagContent();
        }
        
        log("bought");
    }else{
        GameManager::getInstance()->playSoundEffect(SOUND_ROCKET_LAUNCH);
        GM->showDisposableMessage(strmake(LM->getText("not enough format").c_str(), LM->getText("coin").c_str()).c_str(), HUD);
        log("not enough mineral");
    }
}
std::string OnSale::getTalk(){
    std::string talk;
    if (item == SELLING_ITEM_KEY) {
        talk.append(LanguageManager::getInstance()->getText("itemDescriptionKey"));
    }else if(item == SELLING_ITEM_HEART){
        talk.append(LanguageManager::getInstance()->getText("itemDescriptionHeart"));
    }else if(item == SELLING_ITEM_AMMO){
        talk.append(LanguageManager::getInstance()->getText("itemDescriptionAmmo"));
    }else if(item == SELLING_ITEM_DRAGON_BABY){
        talk.append(LanguageManager::getInstance()->getText("itemDescriptionDragonBaby"));
    }else if(item == SELLING_ITEM_DRAGON_ADOLESCENT){
        talk.append(LanguageManager::getInstance()->getText("itemDescriptionDragonAdolescent"));
    }else if(item == SELLING_ITEM_DRAGON_ADULT){
        talk.append(LanguageManager::getInstance()->getText("itemDescriptionDragonAdult"));
    }else if(item == SELLING_ITEM_DRAGON_SENIOR){
        talk.append(LanguageManager::getInstance()->getText("itemDescriptionDragonSenior"));
    }else if(item >= SELLING_ITEM_WEAPON){
        talk.append(LanguageManager::getInstance()->getText(__String::createWithFormat("weaponName%d", item)->getCString()) + ". " + LanguageManager::getInstance()->getText(__String::createWithFormat("weaponDesc%d", item)->getCString()));
    }
    isExplained = true;
    return talk;
}
// Bazooka
void Bazooka::movingSchedule(float dt){
    timeLeftToNextAction -= dt;
    stopTime-=dt;
    
    if (timeLeftToNextAction < 0) {
        float timeLeftDefault = 3;
        float interval = 0.24f;
        if (currentAction == BAZOOKA_ACTION_SHOOT) {
            shootCount = 0;
            shootCountMax = 5;
            if (energy < maxEnergy/3) {
                shootCountMax = 9;
            }
            interval = 0.24f;
            timeLeftDefault = interval*shootCountMax;
            this->schedule(schedule_selector(Bazooka::shoot), interval, shootCountMax, 0.4);
            
            this->sptGun->setVisible(true);
            this->runAnimation("bazookaShootReady", false);
        }else if (currentAction == BAZOOKA_ACTION_MINI){
            shootCount = 0;
            interval = 1.5f;
            shootCountMax = 4;
            if (energy < maxEnergy/3) {
                shootCountMax = 6;
            }
            this->schedule(schedule_selector(Bazooka::shootMiniReady), interval, shootCountMax, 0.14f);
            
            this->runAnimation("bazookaDowning", false);
        }else if (currentAction == BAZOOKA_ACTION_BIG){
            shootCount = 0;
            interval = 2.0f;
            shootCountMax = 1;
            if (energy < maxEnergy/3) {
                shootCountMax = 3;
            }
            this->schedule(schedule_selector(Bazooka::shootBigReady), interval, 0, 0.14f);
            this->runAnimation("bazookaDowning", false);
        }else if (currentAction == BAZOOKA_ACTION_CIRCLE){
            shootCount = 0;
            interval = 1.0f;
            shootCountMax = 3;
            this->schedule(schedule_selector(Bazooka::shootCircleReady), interval, shootCountMax, 0.64f);
            this->runAnimation("bazookaCircle", false);
        }
        timeLeftDefault = interval*shootCountMax;
        stopTime = timeLeftDefault + 2;
        if (energy < maxEnergy/3) {
            if (!finalActionTaken) {
                finalActionTaken = true;
                currentAction = 3;
            }else{
                currentAction = rand()%4;
            }
        }else{
            currentAction = rand()%3;
        }
        
        timeLeftToNextAction = 2 + timeLeftDefault + (rand()%50)*0.1f;
        wantsToStop = true;
    }
    if (stopTime > 0) {
        if (!wantsToStop) {
            stop();
        }
    }else{
        if (wantsToStop) {
            isTowardLeft?goLeft():goRight();
            this->runAnimation("bazooka", true);
            sptGun->setVisible(false);
        }
    }
    
    // gun aiming
    double angle = -GameManager::getInstance()->getAngle(this->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2)) + 180;
    Point msPos = Point::ZERO;
    
    float msStartPoint = 0;
    if (sptGun->isVisible()) {
        float radius = 6;
        msStartPoint = radius + sptGun->getContentSize().width;
        sptGun->setRotation(angle);
        float x = cos(angle*3.14/180)*radius;
        float y = -sin(angle*3.14/180)*radius;
        Point center = Point(this->getContentSize().width/2, 6);
        Point gunPos = center + Point(x, y);
        sptGun->setPosition(gunPos);
        
        if (angle < 270 && angle > 90) {
            sptGun->setFlippedY(true);
        }else{
            sptGun->setFlippedY(false);
        }
    }
}


void Bazooka::shootCircleReady(float dt){
    this->setFlippedX(target->getPositionX() < this->getPositionX());
    this->runAnimation("bazookaMini", false, 0.14f);
    this->scheduleOnce(schedule_selector(Bazooka::shootCircle), 0.56f);
}
void Bazooka::shootCircle(float dt){
    Point gunTipPos = this->getPosition() + Point(isTowardLeft?-22:22, -8);
    for (int i = shootCount*10; i < 390; i += 30) {
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 60, 0, 0, "bombSmallEffect0.png");
        missile->isFregile = true;
        missile->movingAngle = i;
        missile->extraSpeed = 10;
        //missile->secondTag = UNIT_BAZOOKA_BIG;
        
        missile->target = this->target;
        
        missile->setPosition(gunTipPos);
        
        missile->weight = 0.1f;
        missile->setScale(0.1);
        this->missileArray.pushBack(missile);
        this->getParent()->addChild(missile);
    }
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(isTowardLeft?0:180);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
    
    
    shootCount++;
}
void Bazooka::shootBigReady(float dt){
    this->setFlippedX(target->getPositionX() < this->getPositionX());
    this->runAnimation("bazookaBig", false, 0.14f);
    this->scheduleOnce(schedule_selector(Bazooka::shootBig), 0.98f);
}
void Bazooka::shootBig(float dt){
    Point gunTipPos = this->getPosition() + Point(isTowardLeft?-22:22, -8);
    Missile* missile = Missile::createMissile(UNIT_MISSILE_CHASING, 700, 0, 0, "pierrotBoss0_missile_big.png");
    missile->isFregile = true;
    missile->movingAngle = isTowardLeft?180:0;
    missile->extraSpeed = 10;
    missile->secondTag = UNIT_BAZOOKA_BIG;
    missile->target = this->target;
    missile->energyBarExist = false;
    missile->setPosition(gunTipPos);
    
    missile->weight = 0.1f;
    missile->setScale(0.1);
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);
    GameManager::getInstance()->playSoundEffect(SOUND_ROCKET_LAUNCH, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(isTowardLeft?0:180);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
}
void Bazooka::shootMiniReady(float dt){
    this->setFlippedX(target->getPositionX() < this->getPositionX());
    this->runAnimation("bazookaMini", false, 0.14f);
    this->scheduleOnce(schedule_selector(Bazooka::shootMini), 0.56f);
}
void Bazooka::shootMini(float dt){
    Point gunTipPos = this->getPosition() + Point(isTowardLeft?-22:22, -8);
    Missile* missile = Missile::createMissile(UNIT_MISSILE_CHASING, 400, 0, 0, "pierrotBoss0_missile_small.png");
    missile->isFregile = true;
    missile->movingAngle = isTowardLeft?180:0;
    missile->extraSpeed = 10;
    missile->secondTag = UNIT_BAZOOKA_MINI;
    missile->energyBarExist = false;
    
    missile->target = this->target;
    
    missile->setPosition(gunTipPos);
    
    missile->weight = 0.1f;
    missile->setScale(0.1);
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);
    GameManager::getInstance()->playSoundEffect(SOUND_ROCKET_LAUNCH, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(isTowardLeft?0:180);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
}
void Bazooka::shoot(float dt){
    this->setFlippedX(target->getPositionX() < this->getPositionX());
    double angle = -GameManager::getInstance()->getAngle(this->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2)) + 180;
    //Point gunTipPos = this->getPosition() + Point(isTowardLeft?-12:12, -8);
    
    Point gunTipPos = this->getPosition() + Point(0, -8) + Point(cos(angle*3.14/180)*12, -sin(angle*3.14/180)*12);
    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 40, 0, 0, "bombSmallEffect0.png");
    missile->isFregile = true;
    missile->movingAngle = -angle;
    missile->extraSpeed = bulletSpeed;
    
    /*missile->runAction(Sequence::create(DelayTime::create(shootCount*0.1), MoveTo::create(0.2, Point(this->getPosition() + Point(0, 30) + Point(cos(circleAngle*3.14/180)*radious, -sin(circleAngle*3.14/180)*radious))), nullptr));
    missile->freezeForAWhile(shootCountMax*0.1 + 0.5);
    missile->target = this->target;
    missile->fixAngleToTargetWhenReleaseFreezing = true;*/
    
    missile->setPosition(gunTipPos);
    
    missile->weight = 0.1f;
    missile->setScale(0.1);
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(angle + 180);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
    
    shootCount++;
}
void Bazooka::runSchedule(){
    this->schedule(schedule_selector(Bazooka::movingSchedule), 0.1);
}
Bazooka* Bazooka::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance){
    Bazooka* bazooka = new Bazooka();
    bazooka->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_MOVE, ENEMY_ACTION_MOVE, 0);
    bazooka->runAnimation("bazooka", true);
    bazooka->setGunInfo(5, 0, ENEMY_GUN_TYPE_HANDGUN, 100);
    bazooka->sptGun->setSpriteFrame("pierrotGun.png");
    bazooka->sptGun->setVisible(false);
    bazooka->goLeft();
    bazooka->isTargetFound = true;
    return bazooka;
}

cocos2d::Rect Bazooka::damageBoundingBox(){
    return Rect(this->getPositionX() - 5, this->getPositionY() - this->getContentSize().height/2, 10, 15);
}
void Bazooka::onLanded(){
    //wantsToJump = true;
}
void Bazooka::encounteredWall(){
    if (stopTime <= 0) {
        wantsToJump = true;
    }
    wantsToGoLeft = !wantsToGoLeft;
    wantsToGoRight = !wantsToGoRight;
}
void Bazooka::encounteredCliff(int direction){
    if (stopTime <= 0) {
        wantsToJump = true;
    }
}

// Machinegun
void Machinegun::movingSchedule(float dt){
    timeLeftToNextAction -= dt;
    stopTime-=dt;
    
    if (timeLeftToNextAction < 0) {
        float timeLeftDefault = 3;
        float interval = 0.24f;
        if (currentAction == MACHINEGUN_ACTION_SHOOT) {
            shootCount = 0;
            shootCountMax = 5;
            if (energy < maxEnergy/3) {
                shootCountMax = 9;
            }
            interval = 0.24f;
            timeLeftDefault = interval*shootCountMax;
            this->schedule(schedule_selector(Machinegun::shoot), interval, shootCountMax, 0.4);
            
            this->sptGun->setVisible(true);
            this->runAnimation("machinegunShoot", false);
        }else if (currentAction == MACHINEGUN_ACTION_MINI){
            this->setFlippedX(target->getPositionX() < this->getPositionX());
            shootCount = 0;
            interval = 0.1f;
            shootCountMax = 30;
            if (energy < maxEnergy/3) {
                shootCountMax = 50;
            }
            this->schedule(schedule_selector(Machinegun::shootMini), interval, shootCountMax, 0.98f);
            
            this->runAnimation("machinegunMiniReady", false);
            this->runAnimation("machinegunMini", true, 0.98f);
            
        }else if (currentAction == MACHINEGUN_ACTION_BIG){
            shootCount = 0;
            interval = 0.1f;
            shootCountMax = 15;
            if (energy < maxEnergy/3) {
                shootCountMax = 30;
            }
            this->schedule(schedule_selector(Machinegun::shootBig), interval, shootCountMax, 0.70f);
            
            this->runAnimation("machinegunBigReady", false);
            this->runAnimation("machinegunBig", true, 0.70f);
        }else if (currentAction == MACHINEGUN_ACTION_CIRCLE){
            shootCount = 0;
            interval = 0.1f;
            shootCountMax = 30;
            if (energy < maxEnergy/3) {
                shootCountMax = 50;
            }
            this->schedule(schedule_selector(Machinegun::shootCircle), interval, shootCountMax, 0.64f);
            this->schedule(schedule_selector(Machinegun::turnAround), 0.12f, interval*shootCountMax/0.12f, 0.64f);
            this->runAnimation("machinegunCircle", true);
        }
        timeLeftDefault = interval*shootCountMax;
        stopTime = timeLeftDefault + 2;
        if (energy < maxEnergy/3) {
            if (!finalActionTaken) {
                finalActionTaken = true;
                currentAction = 3;
            }else{
                currentAction = rand()%4;
            }
        }else{
            currentAction = rand()%3;
        }
        
        timeLeftToNextAction = 2 + timeLeftDefault + (rand()%50)*0.1f;
        wantsToStop = true;
    }
    if (stopTime > 0) {
        if (!wantsToStop) {
            stop();
        }
    }else{
        if (wantsToStop) {
            isTowardLeft?goLeft():goRight();
            this->runAnimation("machinegun", true);
            sptGun->setVisible(false);
        }
    }
    
    // gun aiming
    double angle = -GameManager::getInstance()->getAngle(this->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2)) + 180;
    Point msPos = Point::ZERO;
    
    float msStartPoint = 0;
    if (sptGun->isVisible()) {
        float radius = 6;
        msStartPoint = radius + sptGun->getContentSize().width;
        sptGun->setRotation(angle);
        float x = cos(angle*3.14/180)*radius;
        float y = -sin(angle*3.14/180)*radius;
        Point center = Point(this->getContentSize().width/2, 6);
        Point gunPos = center + Point(x, y);
        sptGun->setPosition(gunPos);
        
        if (angle < 270 && angle > 90) {
            sptGun->setFlippedY(true);
        }else{
            sptGun->setFlippedY(false);
        }
    }
}


void Machinegun::turnAround(float dt){
    this->setFlippedX(!isFlippedX());
}
void Machinegun::shootCircleReady(float dt){
    this->setFlippedX(target->getPositionX() < this->getPositionX());
    this->runAnimation("machinegunMini", false, 0.14f);
    this->scheduleOnce(schedule_selector(Machinegun::shootCircle), 0.56f);
}
void Machinegun::shootCircle(float dt){
    float angle = 180 + rand()%180;
    float radius = 22;
    Point gunTipPos = this->getPosition() + Point(0, -8) + Point(cos(angle*3.14/180)*radius, -sin(angle*3.14/180)*radius);
    
    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 60, 0, 0, "bombSmallEffect0.png");
    missile->isFregile = true;
    missile->movingAngle = -angle;
    missile->extraSpeed = 200;
    missile->setScale(1.5, 0.5);
    missile->setRotation(angle);
    missile->target = this->target;
    
    missile->setPosition(gunTipPos);
    missile->secondTag = UNIT_MACHINEGUN_MINI;
    
    missile->weight = 0.1f;
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);

    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(angle + 180);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
    shootCount++;
    if (shootCount == shootCountMax) {
        this->runAnimation("machinegunBigAfter", false);
    }
}
void Machinegun::shootBigReady(float dt){
    this->setFlippedX(target->getPositionX() < this->getPositionX());
    this->runAnimation("machinegunBig", false, 0.14f);
    this->scheduleOnce(schedule_selector(Machinegun::shootBig), 0.98f);
}
void Machinegun::shootBig(float dt){
    Point gunTipPos = this->getPosition() + Point((isTowardLeft?-7:7) + rand()%3, 13);
    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 40, 0, 0, "bombSmallEffect0.png");
    missile->isFregile = true;
    missile->movingAngle = 90;
    missile->extraSpeed = 200;
    missile->secondTag = UNIT_MACHINEGUN_BIG;
    missile->setScale(1.5, 0.5);
    missile->target = this->target;
    missile->setRotation(90);
    missile->setPosition(gunTipPos);
    
    missile->weight = 0.1f;
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);
    GameManager::getInstance()->playSoundEffect(rand()%2==0?SOUND_MACHINE_GUN_1:SOUND_MACHINE_GUN_2, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(90);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
    shootCount++;
    if (shootCount == shootCountMax) {
        this->runAnimation("machinegunBigAfter", false);
    }
}
void Machinegun::shootMiniReady(float dt){
    this->setFlippedX(target->getPositionX() < this->getPositionX());
    this->runAnimation("machinegunMini", false, 0.14f);
    this->scheduleOnce(schedule_selector(Machinegun::shootMini), 0.56f);
}
void Machinegun::shootMini(float dt){
    Point gunTipPos = this->getPosition() + Point(isTowardLeft?-30:30, -8 + rand()%3);
    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 40, 0, 0, "bombSmallEffect0.png");
    missile->isFregile = true;
    missile->movingAngle = isTowardLeft?180:0;
    missile->extraSpeed = 200;
    missile->secondTag = UNIT_MACHINEGUN_MINI;
    missile->setScale(1.5, 0.5);
    missile->target = this->target;
    
    missile->setPosition(gunTipPos);
    
    missile->weight = 0.1f;
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);
    GameManager::getInstance()->playSoundEffect(rand()%2==0?SOUND_MACHINE_GUN_1:SOUND_MACHINE_GUN_2, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(isTowardLeft?0:180);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
    shootCount++;
    if (shootCount == shootCountMax) {
        this->runAnimation("machinegunMiniAfter", false);
    }
}
void Machinegun::shoot(float dt){
    this->setFlippedX(target->getPositionX() < this->getPositionX());
    double angle = -GameManager::getInstance()->getAngle(this->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2)) + 180;
    //Point gunTipPos = this->getPosition() + Point(isTowardLeft?-12:12, -8);
    
    Point gunTipPos = this->getPosition() + Point(0, -8) + Point(cos(angle*3.14/180)*12, -sin(angle*3.14/180)*12);
    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 40, 0, 0, "bombSmallEffect0.png");
    missile->isFregile = true;
    missile->movingAngle = -angle;
    missile->extraSpeed = bulletSpeed;
    
    /*missile->runAction(Sequence::create(DelayTime::create(shootCount*0.1), MoveTo::create(0.2, Point(this->getPosition() + Point(0, 30) + Point(cos(circleAngle*3.14/180)*radious, -sin(circleAngle*3.14/180)*radious))), nullptr));
     missile->freezeForAWhile(shootCountMax*0.1 + 0.5);
     missile->target = this->target;
     missile->fixAngleToTargetWhenReleaseFreezing = true;*/
    
    missile->setPosition(gunTipPos);
    
    missile->weight = 0.1f;
    missile->setScale(0.1);
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(angle + 180);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
    
    shootCount++;
}
void Machinegun::runSchedule(){
    this->schedule(schedule_selector(Machinegun::movingSchedule), 0.1);
}
Machinegun* Machinegun::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance){
    Machinegun* bazooka = new Machinegun();
    bazooka->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_MOVE, ENEMY_ACTION_MOVE, 0);
    bazooka->runAnimation("machinegun", true);
    bazooka->setGunInfo(5, 0, ENEMY_GUN_TYPE_HANDGUN, 100);
    bazooka->sptGun->setSpriteFrame("pierrotGun.png");
    bazooka->sptGun->setVisible(false);
    bazooka->goLeft();
    bazooka->isTargetFound = true;
    return bazooka;
}

cocos2d::Rect Machinegun::damageBoundingBox(){
    return Rect(this->getPositionX() - 5, this->getPositionY() - this->getContentSize().height/2, 10, 15);
}
void Machinegun::onLanded(){
    //wantsToJump = true;
}
void Machinegun::encounteredWall(){
    if (stopTime <= 0) {
        wantsToJump = true;
    }
    wantsToGoLeft = !wantsToGoLeft;
    wantsToGoRight = !wantsToGoRight;
}
void Machinegun::encounteredCliff(int direction){
    if (stopTime <= 0) {
        wantsToJump = true;
    }
}


// Giant
void Giant::moveHands(){
    stopHands();
    this->schedule(schedule_selector(Giant::movingHands), 0.07);
}
void Giant::stopHands(){
    this->unschedule(schedule_selector(Giant::movingHands));
}
void Giant::movingHands(float dt){
    handsAngle += 10;
    Point newPos = leftHandPos + Point(cos(handsAngle*3.14/180)*handsMovingRadius, sin(handsAngle*3.14/180)*handsMovingRadius);
    leftHand->runAction(MoveTo::create(0.07, newPos));
    newPos = rightHandPos + Point(cos(-handsAngle*3.14/180)*handsMovingRadius, sin(-handsAngle*3.14/180)*handsMovingRadius);
    rightHand->runAction(MoveTo::create(0.07, newPos));
}
void Giant::movingSchedule(float dt){
    timeLeftToNextAction -= dt;
    stopTime-=dt;
    
    if (target != nullptr && !wantsToStop) {
        Point position = target->getPosition() + Point(0, 100);
        float angle = GameManager::getInstance()->getAngle(position, this->getPosition());
        
        speed += 0.1f;
        if (speed > maxSpeed) {
            speed = maxSpeed;
        }
        Point gap = Point(cos(angle*3.14/180)*speed, sin(angle*3.14/180)*speed);
        float slowDownDistance = 20;
        float speedLimit = 0;
        float absDistance = sqrtf(pow(position.x - this->getPositionX(), 2) + pow(position.y - this->getPositionY(), 2));
        if (absDistance < slowDownDistance ) {
            speedLimit = maxSpeed*absDistance/slowDownDistance;
        }
        if (speedLimit > 0 && speed > speedLimit) {
            speed = speedLimit;
            gap = Point(cos(angle*3.14/180)*speed, sin(angle*3.14/180)*speed);
        }
        
        this->runAction(MoveTo::create(0.07, Point(this->getPosition() + gap)));
    }
    if (timeLeftToNextAction < 0) {
        float timeLeftDefault = 3;
        float interval = 0.24f;
        if (currentAction == GIANT_ACTION_SHOOT) {
            shootCount = 0;
            shootCountMax = 5;
            if (energy < maxEnergy/3) {
                shootCountMax = 9;
            }
            interval = 0.24f;
            timeLeftDefault = interval*shootCountMax;
            this->schedule(schedule_selector(Giant::shoot), interval, shootCountMax + 1, 0.4);
            
            isShootLeftAttack = rand()%2==0?true:false;
            stopHands();
            (isShootLeftAttack?leftHand:rightHand)->setSpriteFrame("bigBossHandPoint.png");
            leftHand->runAction(Sequence::create(MoveBy::create(0.1, Point(rand()%4, rand()%4)),
                                                 MoveBy::create(0.1, Point(rand()%4, rand()%4)),
                                                 MoveBy::create(0.1, Point(rand()%4, rand()%4)),
                                                 MoveBy::create(0.1, Point(rand()%4, rand()%4)), NULL));
            wantsToStop = true;
        }else if (currentAction == GIANT_ACTION_SLAP){
            shootCount = 0;
            shootCountMax = 1;
            
            interval = 10000.0f;
            timeLeftDefault = 100;
            this->scheduleOnce(schedule_selector(Giant::shootSlap), 0.5f);
            
            isSlapLeftAttack = rand()%2==0?true:false;
            (isSlapLeftAttack?leftHand:rightHand)->runAction(FadeOut::create(0.5f));
            
        }else if (currentAction == GIANT_ACTION_STAMP){
            shootCount = 0;
            interval = 1.0f;
            shootCountMax = 3;
            if (energy < maxEnergy/3) {
                shootCountMax = 6;
            }
            this->schedule(schedule_selector(Giant::shootStamp), interval, shootCountMax, 0.70f);
            leftHand->runAction(FadeOut::create(0.5f));
            rightHand->runAction(FadeOut::create(0.5f));
        }else if (currentAction == GIANT_ACTION_SLAP_SHOOT){
            shootCount = 0;
            shootCountMax = 5;
            if (energy < maxEnergy/3) {
                shootCountMax = 9;
            }
            interval = 0.24f;
            timeLeftDefault = interval*shootCountMax;
            this->schedule(schedule_selector(Giant::shoot), interval, shootCountMax + 1, 0.4);
            
            isShootLeftAttack = rand()%2==0?true:false;
            stopHands();
            (isShootLeftAttack?leftHand:rightHand)->setSpriteFrame("bigBossHandPoint.png");
            leftHand->runAction(Sequence::create(MoveBy::create(0.1, Point(rand()%4, rand()%4)),
                                                 MoveBy::create(0.1, Point(rand()%4, rand()%4)),
                                                 MoveBy::create(0.1, Point(rand()%4, rand()%4)),
                                                 MoveBy::create(0.1, Point(rand()%4, rand()%4)), NULL));
            wantsToStop = true;
            
            interval = 10000.0f;
            timeLeftDefault = 100;
            this->scheduleOnce(schedule_selector(Giant::shootSlap), 0.5f);
            
            isSlapLeftAttack = !isShootLeftAttack;
            (isSlapLeftAttack?leftHand:rightHand)->runAction(FadeOut::create(0.5f));
        }
        timeLeftDefault = interval*shootCountMax;
        stopTime = timeLeftDefault + 2;
        if (energy < maxEnergy/3) {
            if (!finalActionTaken) {
                finalActionTaken = true;
                currentAction = 3;
            }else{
                currentAction = rand()%4;
            }
        }else{
            currentAction = rand()%3;
        }
        
        timeLeftToNextAction = 2 + timeLeftDefault + (rand()%50)*0.1f;
        wantsToStop = true;
    }
    if (stopTime > 0) {
        if (!wantsToStop) {
            stop();
        }
    }else{
        if (wantsToStop) {
            isTowardLeft?goLeft():goRight();
            this->runAnimation("machinegun", true);
            sptGun->setVisible(false);
        }
    }
    
    // gun aiming
    double angle = -GameManager::getInstance()->getAngle(this->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2)) + 180;
    Point msPos = Point::ZERO;
    
    float msStartPoint = 0;
    if (sptGun->isVisible()) {
        float radius = 6;
        msStartPoint = radius + sptGun->getContentSize().width;
        sptGun->setRotation(angle);
        float x = cos(angle*3.14/180)*radius;
        float y = -sin(angle*3.14/180)*radius;
        Point center = Point(this->getContentSize().width/2, 6);
        Point gunPos = center + Point(x, y);
        sptGun->setPosition(gunPos);
        
        if (angle < 270 && angle > 90) {
            sptGun->setFlippedY(true);
        }else{
            sptGun->setFlippedY(false);
        }
    }
}
void Giant::shootStampReady(float dt){
    this->setFlippedX(target->getPositionX() < this->getPositionX());
    this->runAnimation("machinegunBig", false, 0.14f);
    this->scheduleOnce(schedule_selector(Giant::shootStamp), 0.98f);
}
void Giant::shootStamp(float dt){
    Missile* missile = Missile::createMissile(UNIT_MISSILE_CUSTOM, 70, 0, 0, "bigBossHandDown.png");
    missile->setFlippedX(rand()%2==0);
    missile->setPosition(Point(target->getPositionX(), 300));
    missile->runAction(Sequence::create(MoveTo::create(1.4f, Point(missile->getPositionX(), 30)), CallFunc::create(CC_CALLBACK_0(HelloWorld::shakeScreenOnce, GameManager::getInstance()->getCurrentStageLayer())), CallFunc::create(CC_CALLBACK_0(Giant::playKoongSound, this)), MoveBy::create(0.1f, Point(0, 3)), CallFunc::create(CC_CALLBACK_0(Droppable::dontHurt, missile)), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(HelloWorld::missileMoveDone,GameManager::getInstance()->getCurrentStageLayer())), DelayTime::create(0.3f),  NULL));
    missile->secondTag = UNIT_MISSILE_THROUGH;
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);
    
    shootCount++;
    if (shootCount == shootCountMax) {
        leftHand->runAction(Sequence::create(DelayTime::create(2.3f), FadeTo::create(0.5, 100), NULL));
        rightHand->runAction(Sequence::create(DelayTime::create(2.3f), FadeTo::create(0.5, 100), NULL));
        timeLeftToNextAction = 5;
    }
}
void Giant::playKoongSound(){
    GameManager::getInstance()->playSoundEffect(SOUND_KOONG);
}
void Giant::shootSlapReady(float dt){
    this->setFlippedX(target->getPositionX() < this->getPositionX());
    this->runAnimation("machinegunMini", false, 0.14f);
    this->scheduleOnce(schedule_selector(Giant::shootSlap), 0.56f);
}
void Giant::shootSlap(float dt){
    Missile* missile = Missile::createMissile(UNIT_MISSILE_CUSTOM, 70, 0, 0, "bigBossHandSlap.png");
    missile->setFlippedX(isSlapLeftAttack);
    missile->setPosition(target->getPosition() + Point(isSlapLeftAttack?-180:180, 0));
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);
    slapHand = missile;
    slapTimeout = 5;
    this->schedule(schedule_selector(Giant::movingSlap), 0.1f);
}
void Giant::onDead(){
    head->stopAllActions();
    head->setSpriteFrame("bigBossHeadHit.png");
    leftHand->runAction(FadeOut::create(5));
    rightHand->runAction(FadeOut::create(5));
    head->runAction(FadeOut::create(5));
    nose->runAction(FadeOut::create(5));
}
void Giant::onHit(){
    hit();
}
void Giant::hit(){
    hitCount++;
    GameManager::getInstance()->shakeIt(this);
    head->setSpriteFrame("bigBossHead.png");
    head->blinkForAWhile();
    head->runAnimation("giantHit", false);
    
    if (hitCount > 10) {
        this->untouchable = true;
        nose->runAction(FadeTo::create(0.1, 100));
        hitCount = 0;
        this->scheduleOnce(schedule_selector(Giant::getBackToNormalNose), 10);
    }
}
void Giant::getBackToNormalNose(float dt){
    nose->runAction(FadeIn::create(0.5));
    this->untouchable = false;
}
void Giant::movingSlap(float dt){
    slapAngle = GameManager::getInstance()->getAngle(target->getPosition(), slapHand->getPosition());
    float slapSpeed = 20;
    slapHand->runAction(MoveBy::create(0.1f, Point(cos(slapAngle*3.14/180)*slapSpeed, sin(slapAngle*3.14/180)*slapSpeed)));
    slapTimeout -= dt;
    if (slapHand->getBoundingBox().containsPoint(target->getPosition()) || (isSlapLeftAttack?(target->getPositionX() < slapHand->getPositionX()):(slapHand->getPositionX() < target->getPositionX())) || slapTimeout < 0) {
        float moveDistance = 200;
        slapHand->runAction(Sequence::create(MoveBy::create(1, Point(cos(slapAngle*3.14/180)*moveDistance, sin(slapAngle*3.14/180)*moveDistance)), CallFuncN::create(CC_CALLBACK_1(HelloWorld::missileMoveDone, GameManager::getInstance()->getCurrentStageLayer())), NULL));
        slapHand->runAction(Sequence::create(DelayTime::create(0.5), FadeOut::create(0.5), NULL));
        this->unschedule(schedule_selector(Giant::movingSlap));
        (isSlapLeftAttack?leftHand:rightHand)->runAction(Sequence::create(DelayTime::create(1), FadeTo::create(1, 100), nullptr));
        timeLeftToNextAction = 3;
    }
}
void Giant::shoot(float dt){
    Sprite* theHand = isShootLeftAttack?leftHand:rightHand;
    if (shootCount == shootCountMax) {
        theHand->setSpriteFrame("bigBossHandIdle.png");
        moveHands();
        wantsToStop = false;
        return;
    }
    Point gunTipPos = this->getPosition() + theHand->getPosition() + Point(isShootLeftAttack?20:-80, -20);
    double angle = GameManager::getInstance()->getAngle(this->target->getPosition() + Point(0, TILE_SIZE/2), gunTipPos);
    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 70, 0, 0, "bombSmallEffect0.png");
    missile->isFregile = true;
    missile->movingAngle = angle;
    missile->extraSpeed = bulletSpeed;
    
    missile->setPosition(gunTipPos);
    
    missile->weight = 0.1f;
    missile->setScale(0.1);
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(isShootLeftAttack?180:0);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
    
    shootCount++;
    
}
void Giant::setPositions(){
    leftHandPos = Point(-100 + handsMovingRadius, -40);
    rightHandPos = Point(100 + handsMovingRadius*2, -40);
    leftHand->setPosition(leftHandPos + Point(handsMovingRadius/2, 0));
    rightHand->setPosition(rightHandPos + Point(handsMovingRadius/2, 0));
    nose->setPosition(Point(this->getContentSize().width/2, 0));
    head->setPosition(Point(this->getContentSize().width/2, 30));
    head->setOpacity(100);
    leftHand->setOpacity(100);
    rightHand->setOpacity(100);
}
void Giant::runSchedule(){
    this->schedule(schedule_selector(Giant::movingSchedule), 0.1);
}
Giant* Giant::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance){
    Giant* giant = new Giant();
    giant->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_MOVE, ENEMY_ACTION_MOVE, 0);
    giant->runAnimation("machinegun", true);
    giant->setOpacity(0);
    giant->setGunInfo(5, 0, ENEMY_GUN_TYPE_HANDGUN, 100);
    giant->sptGun->setSpriteFrame("pierrotGun.png");
    giant->sptGun->setVisible(false);
    giant->goLeft();
    giant->isTargetFound = true;
    giant->head = Droppable::createDroppable(UNIT_BAT, 1, 0, "bigBossHead.png");
    giant->addChild(giant->head);
    giant->nose = Sprite::createWithSpriteFrameName("bigBossNose.png");
    giant->addChild(giant->nose, 1);
    giant->leftHand = Sprite::createWithSpriteFrameName("bigBossHandIdle.png");
    giant->addChild(giant->leftHand, 10);
    giant->leftHand->setFlippedX(true);
    giant->rightHand = Sprite::createWithSpriteFrameName("bigBossHandIdle.png");
    giant->addChild(giant->rightHand, 10);
    giant->setPositions();
    giant->moveHands();
    return giant;
}

cocos2d::Rect Giant::damageBoundingBox(){
    return Rect(this->getPositionX() - 18, this->getPositionY() - 31, 36, 34);
}
void Giant::onLanded(){
    //wantsToJump = true;
}
void Giant::encounteredWall(){
    if (stopTime <= 0) {
        wantsToJump = true;
    }
    wantsToGoLeft = !wantsToGoLeft;
    wantsToGoRight = !wantsToGoRight;
}
void Giant::encounteredCliff(int direction){
    if (stopTime <= 0) {
        wantsToJump = true;
    }
}

// GiantHood
void GiantHood::movingSchedule(float dt){
    timeLeftToNextAction -= dt;
    stopTime-=dt;
    if (target != nullptr && !wantsToStop) {
        Point position = target->getPosition() + Point(0, 1);
        float angle = GameManager::getInstance()->getAngle(position, this->getPosition());
        
        speed += 0.1f;
        if (speed > maxSpeed) {
            speed = maxSpeed;
        }
        Point gap = Point(cos(angle*3.14/180)*speed, sin(angle*3.14/180)*speed);
        float slowDownDistance = 20;
        float speedLimit = 0;
        float absDistance = sqrtf(pow(position.x - this->getPositionX(), 2) + pow(position.y - this->getPositionY(), 2));
        if (absDistance < slowDownDistance ) {
            speedLimit = maxSpeed*absDistance/slowDownDistance;
        }
        if (speedLimit > 0 && speed > speedLimit) {
            speed = speedLimit;
            gap = Point(cos(angle*3.14/180)*speed, sin(angle*3.14/180)*speed);
        }
        this->setFlippedX(target->getPositionX() < this->getPositionX());
        this->runAction(MoveTo::create(0.07, Point(this->getPosition() + gap)));
    }
    if (timeLeftToNextAction < 0) {
        float timeLeftDefault = 3;
        float interval = 0.24f;
        if (currentAction == GIANT_HOOD_ACTION_SHOOT) {
            shootCount = 0;
            shootCountMax = 14;
            interval = 0.1f;
            shootAngle = 0;
            this->schedule(schedule_selector(GiantHood::shoot), interval, shootCountMax, 0.4);
            wantsToStop = true;
        }else if (currentAction == GIANT_HOOD_ACTION_SPIRAL){
            shootCount = 0;
            shootCountMax = 20;
            if (energy < maxEnergy/3) {
                shootCountMax = 10;
            }
            spinAngle = rand()%100;
            spiralMissileMoveAngle = 40 + rand()%30;
            interval = 0.1f;
            wantsToStop = true;
            this->schedule(schedule_selector(GiantHood::shootSpiral), interval, shootCountMax, 0.14f);
        }else if (currentAction == GIANT_HOOD_ACTION_SPIN){
            shootCount = 0;
            shootCountMax = 30;
            interval = 0.1f;
            if (energy < maxEnergy/3) {
                shootCountMax = 50;
            }
            wantsToStop = true;
            this->schedule(schedule_selector(GiantHood::shootSpin), interval, shootCountMax, 0.14f);
        }else if (currentAction == GIANT_HOOD_ACTION_RANDOM){
            shootCount = 0;
            interval = 0.07f;
            shootCountMax = 30;
            this->schedule(schedule_selector(GiantHood::shootRandom), interval, shootCountMax, 0.64f);
        }
        timeLeftDefault = interval*shootCountMax;
        stopTime = timeLeftDefault + 2;
        if (energy < maxEnergy/3) {
            if (!finalActionTaken) {
                finalActionTaken = true;
                currentAction = 3;
            }else{
                currentAction = rand()%4;
            }
        }else{
            currentAction = rand()%3;
        }
        
        timeLeftToNextAction = 2 + timeLeftDefault + (rand()%50)*0.1f;
    }
}

void GiantHood::shootRandom(float dt){
    this->setFlippedX(target->getPositionX() < this->getPositionX());
    double angle = -GameManager::getInstance()->getAngle(this->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2)) + 180;
    angle += rand()%90 - 45;
    
    Point gunTipPos = this->getPosition();
    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 80, 0, 0, "bombSmallEffect0.png");
    missile->isFregile = true;
    missile->movingAngle = -angle;
    missile->extraSpeed = bulletSpeed;
    
    missile->setPosition(gunTipPos);
    
    missile->weight = 0.1f;
    missile->setScale(1, 0.5f);
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(angle + 180);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
    
    shootCount++;
    if (shootCount == shootCountMax) {
        
    }
}
void GiantHood::shootSpin(float dt){
    spinAngle += 3;
    Point gunTipPos = this->getPosition() + Point(isTowardLeft?-22:22, -8);
    for (int i = spinAngle; i < 360 + spinAngle; i += 36) {
        Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 80, 0, 0, "bombSmallEffect0.png");
        missile->isFregile = true;
        missile->movingAngle = i;
        missile->extraSpeed = 20;//10 + shootCount*4;
        
        missile->setPosition(gunTipPos);
        
        missile->weight = 0.1f;
        missile->setScale(0.1);
        this->missileArray.pushBack(missile);
        this->getParent()->addChild(missile);
    }
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(isTowardLeft?0:180);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
    
    shootCount++;
    if (shootCount == shootCountMax) {
        wantsToStop = false;
    }
}
void GiantHood::shootSpiral(float dt){
    float radius = 10;
    int addAngle = 30;
    
    if (energy < maxEnergy/3) {
        addAngle = 15;
    }
    for (int i = 0; i < 360; i += addAngle) {
        SpinMissile* missile = SpinMissile::create(UNIT_MISSILE_STRAIGHT, "bombSmallEffect0.png", Point(cos(i*3.14/180)*radius, sin(i*3.14/180)*radius), this->getPosition());
        missile->isFregile = true;
        missile->extraSpeed = 0;
        missile->weight = 0.1f;
        missile->setScale(0.1);
        missile->angleVar = spiralMissileMoveAngle;
        this->missileArray.pushBack((Missile*)missile);
        this->getParent()->addChild(missile);
    }
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(getPosition());
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(isTowardLeft?0:180);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
    
    shootCount++;
    if (shootCount == shootCountMax) {
        wantsToStop = false;
    }
}
void GiantHood::shoot(float dt){
    double angle = -GameManager::getInstance()->getAngle(this->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2)) + 180;
    Point gunTipPos = this->getPosition() + Point(isTowardLeft?-3:3, 7);
    float radius = 30;
    shootAngle += 360.0f/shootCountMax;
    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 80, 0, 0, "bombSmallEffect0.png");
    missile->isFregile = true;
    missile->movingAngle = -angle;
    missile->extraSpeed = bulletSpeed;
    
    missile->runAction(Sequence::create(MoveTo::create(0.2, Point(this->getPosition() + Point(0, 0) + Point(cos(shootAngle*3.14/180)*radius, -sin(shootAngle*3.14/180)*radius))), nullptr));
    missile->freezeForAWhile(shootCountMax*0.1 + 0.5);
    missile->target = this->target;
    missile->fixAngleToTargetWhenReleaseFreezing = true;
    
    missile->setPosition(gunTipPos);
    
    missile->weight = 0.1f;
    missile->setScale(0.1);
    this->missileArray.pushBack(missile);
    this->getParent()->addChild(missile);
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0, GameManager::getInstance()->getSoundVolumnByDistance(target, this));
    /*
    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
    this->getParent()->addChild(explode);
    explode->setPosition(gunTipPos);
    explode->setAnchorPoint(Point(1, 0.5));
    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
    Animate* animate = Animate::create(animation);
    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
    explode->setRotation(angle + 180);
    if (rand()%2 == 0) {
        explode->setFlippedY(true);
    }
     */
    shootCount++;
    if (shootCount == shootCountMax) {
        wantsToStop = false;
    }
}
void GiantHood::runSchedule(){
    this->schedule(schedule_selector(GiantHood::movingSchedule), 0.1);
}
GiantHood* GiantHood::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance){
    GiantHood* gHood = new GiantHood();
    gHood->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_MOVE, ENEMY_ACTION_MOVE, 0);
    gHood->runAnimation("hoodGhost", true);
    gHood->setGunInfo(5, 0, ENEMY_GUN_TYPE_HANDGUN, 100);
    gHood->sptGun->setSpriteFrame("pierrotGun.png");
    gHood->sptGun->setVisible(false);
    gHood->goLeft();
    gHood->isTargetFound = true;
    gHood->setScale(2);
    return gHood;
}

void GiantHood::changeMoveTarget(float dt){
    
}
cocos2d::Rect GiantHood::damageBoundingBox(){
    return Rect(this->getPositionX() - 5, this->getPositionY() - this->getContentSize().height/2, 10, 15);
}
void GiantHood::onLanded(){
    //wantsToJump = true;
}
void GiantHood::encounteredWall(){
    if (stopTime <= 0) {
        wantsToJump = true;
    }
    wantsToGoLeft = !wantsToGoLeft;
    wantsToGoRight = !wantsToGoRight;
}
void GiantHood::encounteredCliff(int direction){
    if (stopTime <= 0) {
        wantsToJump = true;
    }
}

// SpinMissile
void SpinMissile::movingSchedule(float dt){
    movingAngle += angleVar;
    if (movingAngle > 360) {
        movingAngle -= 360;
    }
    Point targetPos = getPosition() + center + Point(cos(movingAngle*3.14/180)*radius, sin(movingAngle*3.14/180)*radius);
    //log("current: %f, %f / next: %f, %f / center: %f, %f", getPositionX(), getPositionY(), targetPos.x, targetPos.y, center.x, center.y);
    this->runAction(MoveTo::create(0.07, targetPos));
}
SpinMissile* SpinMissile::create(int unit, const char* sptName, Point centerPoint, cocos2d::Point initPos){
    SpinMissile* sMissile = new SpinMissile();
    sMissile->init(UNIT_MISSILE_CUSTOM, 0, 100, sptName);
    sMissile->center = centerPoint;
    sMissile->setPosition(initPos);
    sMissile->radius = sqrtf(pow(centerPoint.x, 2) + pow(centerPoint.y, 2));
    sMissile->movingAngle = GameManager::getInstance()->getAngle(centerPoint, initPos) + 90;
    sMissile->schedule(schedule_selector(SpinMissile::movingSchedule), 0.07);
    return sMissile;
}

// Alphabet
void Alphabet::movingSchedule(float dt){
    if (lblChar != nullptr) {
        if (keyDownTime >= 0) {
            keyDownTime -= dt;
            lblChar->setPosition(getPosition() + lblPos + Point(0, -5));
            this->setSpriteFrame("keyboardKeyDown.png");
        }else{
            lblChar->setPosition(getPosition() + lblPos);
            this->setSpriteFrame("keyboardKey.png");
        }
    }
}
void Alphabet::initLabel(std::string str, int fontSize, cocos2d::Point pos){
    float scale = 10;
    lblChar = Label::createWithTTF(str, GameManager::getInstance()->getFont(FONT_VISITOR), fontSize*scale);
    this->getParent()->getParent()->addChild(lblChar, 999999);
    lblChar->setScale(1/scale);
    lblPos = pos;
}
Alphabet* Alphabet::create(int unit, const char* sptName){
    Alphabet* sMissile = new Alphabet();
    sMissile->init(unit, 10000, 100, sptName);
    sMissile->schedule(schedule_selector(Alphabet::movingSchedule), 0.07);
    return sMissile;
}
