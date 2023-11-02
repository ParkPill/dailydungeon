//
//  Buddies.cpp
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 14. 7. 15..
//
//

#include "Buddies.h"
#include "GameManager.h"
Buddy::Buddy(){
    
}
Buddy::~Buddy(){
    
}
void Buddy::initBuddy(int unit, int eng, int petNumber, const char* sptName, int monitorDistance){
    this->positionArray = PointArray::create(50);
    this->positionArray->retain();
    this->behinder = NULL;
//    this->autorelease();
    this->Sprite::init();
    SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(sptName);
    this->setDisplayFrame(frame);
    this->getTexture()->setAliasTexParameters();
    this->energy = eng;
    this->maxEnergy = eng;
    this->setTag(unit);
    this->weight = 1;
    this->petNumber = petNumber;
    this->fireCoolTimeMax = 0.6f;
    this->fireCoolTime = fireCoolTimeMax;
    this->isReadyToFire = false;
    this->monitoringDistance = monitorDistance;
    this->extraSpeed = 60;
    this->runSchedule();
}
void Buddy::runSchedule(){
    CCLOG("Buddy run schedule");
}

void Buddy::movingSchedule(float dt){
    
}
void Buddy::fire(){
}
void Buddy::fireSchedule(float dt){
}
void Buddy::encounteredWall(){
    isEncounteredWall = true;
}
void Buddy::encounteredCliff(){
    isEncounteredCliff = true;
}

void Buddy::setPositionForBoth(Point pos){
    positionArray->addControlPoint(pos);
    
    this->setPosition(pos);
    
    if (positionArray->count() > 9) {
        if (behinder) {
            behinder->setPositionForBoth(positionArray->getControlPointAtIndex(0));
        }
        
        positionArray->removeControlPointAtIndex(0);
    }
}
void Buddy::walkToTarget(Droppable* tar, float distance){
    float dist = this->findTarget(tar, distance);
    if(dist > 100){
        this->setPosition(tar->getPosition() + Point(0, 10));
        return;
    }
    if (tar->currentPlatform) {
        this->setPosition(tar->getPosition());
        ignoreGravity = true;
        return;
    }else{
        if (ignoreGravity) {
            ignoreGravity = false;
        }
    }
    
    float absoluteDistanceX = this->getPosition().x - tar->getPosition().x;
    if(absoluteDistanceX < 0)
        absoluteDistanceX = -absoluteDistanceX;
    float absoluteDistanceY = this->getPosition().y - tar->getPosition().y;
    if(absoluteDistanceY < 0)
        absoluteDistanceY = -absoluteDistanceY;
    
    if(!onGround && this->getPosition().y - tar->getPosition().y > 2 && absoluteDistanceX < 2){
        wantsToGoRight = false;
        wantsToGoLeft = false;
        this->velocity = Point(0, velocity.y);
        return;
    }
    
    if (isTargetFound) {
        if (absoluteDistanceX > 10) {
            wantsToGoRight = this->getPosition().x < tar->getPosition().x;
            wantsToGoLeft = this->getPosition().x >= tar->getPosition().x;
            wantsToStop = false;
//            return;
        }else{
            wantsToStop = true;
        }
        if(absoluteDistanceY > 15){
            if (isEncounteredWall || onGround || inWater) {
                if(tar->getPosition().y - this->getPosition().y > 10){
                    wantsToJump = true;
                }else{
                    this->oneWayCancelY = this->getPosition().y - this->getContentSize().height/2;
                }
            }
//            return;
        }
//        wantsToGoRight = false;
//        wantsToGoLeft = false;
//        wantsToJump = false;
        if(this->velocity.y < 1 && this->velocity.y > -1){
            this->velocity = Point(0, velocity.y);
        }
    }else{
        if (absoluteDistanceX < 10) {
            if (tar->getPosition().y - this->getPosition().y > 10) {
                wantsToGoRight = false;
                wantsToGoLeft = false;
                
                if(onGround)
                    wantsToJump = true;
            }else{
                wantsToGoRight = false;
                wantsToGoLeft = false;
                wantsToJump = false;
            }
            if(this->velocity.y < 1 && this->velocity.y > -1){
                this->velocity = Point(0, velocity.y);
            }
        }else{
            if (isEncounteredCliff || isEncounteredWall) {
                isEncounteredCliff = false;
                isEncounteredWall = false;
                if (this->getPosition().y <= tar->getPosition().y) {
                    if(onGround)
                        wantsToJump = true;
                }
            }
            if (this->getPosition().x < tar->getPosition().x) {
                wantsToGoRight = true;
                wantsToGoLeft = false;
            }else{
                wantsToGoLeft = true;
                wantsToGoRight = false;
            }
            wantsToStop = false;
        }
    }
}
///////////////////////////////////////////////
// GroundPet
///////////////////////////////////////////////
GroundPet* GroundPet::create(int unit, int eng, int petNumber, const char* sptName, int monitorDistance){
    GroundPet* fox = new GroundPet();
    fox->initBuddy(unit, eng, petNumber, sptName, monitorDistance);
    fox->moveType = PET_MOVE_WALK;
    return fox;
}
void GroundPet::runSchedule(){
//    this->schedule(schedule_selector(GroundPet::movingSchedule), 0.2);
    this->schedule(schedule_selector(GroundPet::fireSchedule), 1.8);
}
void GroundPet::fireSchedule(float dt){
    isReadyToFire = true;
}


void GroundPet::movingSchedule(float dt){
    this->walkToTarget(master, this->monitoringDistance);
    
    if(isReadyToFire) return;
    fireCoolTime += dt;
    if (fireCoolTimeMax > fireCoolTime) {
        return;
    }
    fireCoolTime = 0;
}

void GroundPet::fire(){
    int att = GameManager::getInstance()->getPetAttack(this->petNumber, GameManager::getInstance()->getPetLevel(GameManager::getInstance()->getGroundPetIndex()));
    Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, att, 0, 0, "msFire0.png");
    switch (petNumber) {
        case PET_BOOGI:
            // msBlue.png
            // msEye.png
            // msFire0.png
            // msFire1.png
            // msFire2.png
            // msFire3.png
            // msFire4.png
            
            // msFlameThrower.png
            // msGreen0.png 0,1,2 animation
            // msPurple.png
            // msRing0.png 0,1 animation
            missile->setSpriteFrame("msFire0.png");
            break;
        case PET_BURI:
            missile->setSpriteFrame("msFire1.png");
            break;
        case PET_CHUS:
            missile->setSpriteFrame("msFire1.png");
            break;
        case PET_KIWI:
            missile->setSpriteFrame("msFire2.png");
            break;
        case PET_KUMURI:
            missile->setSpriteFrame("msFire2.png");
            break;
        case PET_LEXY:
            missile->setSpriteFrame("msFire3.png");
            break;
        case PET_NEPTUN:
            missile->setSpriteFrame("msFire4.png");
            break;
        case PET_OXTO:
            missile->setSpriteFrame("msEye.png");
            GameManager::getInstance()->makeItSiluk(missile);
            break;
        case PET_PLUTO:
            missile->setSpriteFrame("msFire4.png");
            break;
        case PET_RIUS:
            missile->runAnimation("msRing", true);
            GameManager::getInstance()->makeItSiluk(missile);
            break;
        case PET_TERRY:
            missile->setSpriteFrame("msFire4.png");
            break;
        case PET_TORI:
            missile->setSpriteFrame("msPurple.png");
            break;
        case PET_TORORI:
            missile->setSpriteFrame("msFlameThrower.png");
            break;
        case PET_UNO:
            missile->setSpriteFrame("msBlue.png");
            GameManager::getInstance()->makeItSiluk(missile);
            break;
        case PET_VETH:
            missile->setSpriteFrame("msFlameThrower.png");
            GameManager::getInstance()->makeItSiluk(missile);
            break;
            
        default:
            
            break;
    }
    missile->movingAngle = isTowardLeft?180:0;
    missile->setRotation(-missile->movingAngle);
    missile->setPosition(this->getPosition());
    missile->extraSpeed = 80;

    this->missile = missile;
    this->getParent()->addChild(missile);
    
    isReadyToFire = false;
}

///////////////////////////////////////////////
// SkyPet
///////////////////////////////////////////////
SkyPet* SkyPet::create(int unit, int eng, int petNumber, const char* sptName, int monitorDistance){
    SkyPet* dragon = new SkyPet();
    dragon->initBuddy(unit, eng, petNumber, sptName, monitorDistance);
//    dragon->setScale(1.3f);
    dragon->moveType = PET_MOVE_FLY;
    return dragon;
}
void SkyPet::runSchedule(){
    this->schedule(schedule_selector(SkyPet::movingSchedule), 0.2);
    this->schedule(schedule_selector(SkyPet::fireSchedule), 1.2);
}
void SkyPet::fireSchedule(float dt){
    isReadyToFire = true;
}
void SkyPet::movingSchedule(float dt){
    if(isReadyToFire) return;
    fireCoolTime += dt;
    if (fireCoolTimeMax > fireCoolTime) {
        return;
    }
    fireCoolTime = 0;
}
void SkyPet::setFlippedX(bool flip){
    if(this->petNumber == PET_ANGEL_DRAGON ||
       this->petNumber == PET_IRON_DRAGON ||
       this->petNumber == PET_WOOD_DRAGON ||
       this->petNumber == PET_STONE_DRAGON ||
       this->petNumber == PET_FALCON){
        this->setScaleX(flip?1:-1);
    }else{
        this->setScaleX(flip?-1:1);
    }
    isTowardLeft = flip;
}

void SkyPet::fire(){
    int att = GameManager::getInstance()->getPetAttack(this->petNumber, GameManager::getInstance()->getPetLevel(GameManager::getInstance()->getSkyPetIndex()));
    Missile* missile;
    if(petNumber == PET_ANGEL_DRAGON){
        missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, att, 0, 0, "angelRing.png");
    }else if(petNumber == PET_IRON_DRAGON){
        missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, att, 0, 0, "ironDragonBall.png");
    }else if(petNumber == PET_WOOD_DRAGON){
        missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, att, 0, 0, "woodDragonLeaf.png");
        Sprite* ms = Sprite::createWithSpriteFrameName("woodDragonLeaf.png");
        missile->addChild(ms);
        ms->setAnchorPoint(Point(0, 2));
        ms->runAction(RepeatForever::create(RotateBy::create(1, 360)));
        
        ms = Sprite::createWithSpriteFrameName("woodDragonLeaf.png");
        missile->addChild(ms);
        ms->setAnchorPoint(Point(1, -2));
        ms->runAction(RepeatForever::create(RotateBy::create(0.7, -360)));
        
    }else if(petNumber == PET_STONE_DRAGON){
        missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, att,0 , 0, "stoneDragonTail0.png");
        missile->runAction(RepeatForever::create(RotateBy::create(0.2, 360)));
    }else if(petNumber == PET_FALCON){
        missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, att, 0, 0, "feather.png");
        Sprite* ms = Sprite::createWithSpriteFrameName("feather.png");
        missile->addChild(ms);
        
        ms = Sprite::createWithSpriteFrameName("feather.png");
        missile->addChild(ms);
        ms->setPosition(Point(rand()%(int)missile->getContentSize().width, rand()%(int)missile->getContentSize().height));
        
        ms = Sprite::createWithSpriteFrameName("feather.png");
        missile->addChild(ms);
        ms->setPosition(Point(rand()%(int)missile->getContentSize().width, rand()%(int)missile->getContentSize().height));
    }else{
        missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, att, 0, 0, "msTwinGreen0.png");
        switch (petNumber) {
            case PET_DORA_0:
                missile->runAnimation("msFireBall", true);
                missile->setScale(1.0f);
                break;
            case PET_DORA_1:
                missile->runAnimation("msFireBall", true);
                missile->setScale(1.2f);
                break;
            case PET_DORA_2:
                missile->runAnimation("msFireBall", true);
                missile->setScale(1.5f);
                break;
            case PET_DORA_3:
                missile->runAnimation("msFireBall", true);
                missile->setScale(2.0f);
                break;
            case PET_DORIS_0:
                missile->runAnimation("msTwinGreen", true);
                missile->setScale(0.5f);
                break;
            case PET_DORIS_1:
                missile->runAnimation("msTwinGreen", true);
                missile->setScale(0.6f);
                break;
            case PET_DORIS_2:
                missile->runAnimation("msTwinGreen", true);
                missile->setScale(0.8f);
                break;
            case PET_DORIS_3:
                missile->runAnimation("msTwinGreen", true);
                break;
            case PET_FRITH_0:
                missile->runAnimation("msTwinBlue", true);
                missile->setScale(0.5f);
                break;
            case PET_FRITH_1:
                missile->runAnimation("msTwinBlue", true);
                missile->setScale(0.6f);
                break;
            case PET_FRITH_2:
                missile->runAnimation("msTwinBlue", true);
                missile->setScale(0.8f);
                break;
            case PET_FRITH_3:
                missile->runAnimation("msTwinBlue", true);
                break;
            case PET_LINDIS_0:
                missile->runAnimation("msTwinRed", true);
                missile->setScale(0.5f);
                break;
            case PET_LINDIS_1:
                missile->runAnimation("msTwinRed", true);
                missile->setScale(0.6f);
                break;
            case PET_LINDIS_2:
                missile->runAnimation("msTwinRed", true);
                missile->setScale(0.8f);
                break;
            case PET_LINDIS_3:
                missile->runAnimation("msTwinRed", true);
                break;
            case PET_ROLO_0:
                missile->runAnimation("msTwinGreen", true);
                missile->setScale(0.5f);
                break;
            case PET_ROLO_1:
                missile->runAnimation("msTwinGreen", true);
                missile->setScale(0.6f);
                break;
            case PET_ROLO_2:
                missile->runAnimation("msTwinGreen", true);
                missile->setScale(0.8f);
                break;
            case PET_ROLO_3:
                missile->runAnimation("msTwinGreen", true);
                break;
            case PET_RURIK_0:
                missile->runAnimation("msTwinBlue", true);
                missile->setScale(0.5f);
                break;
            case PET_RURIK_1:
                missile->runAnimation("msTwinBlue", true);
                missile->setScale(0.6f);
                break;
            case PET_RURIK_2:
                missile->runAnimation("msTwinBlue", true);
                missile->setScale(0.8f);
                break;
            case PET_RURIK_3:
                missile->runAnimation("msTwinBlue", true);
                break;
            case PET_WALOONG_0:
                missile->runAnimation("msTwinRed", true);
                missile->setScale(0.5f);
                break;
            case PET_WALOONG_1:
                missile->runAnimation("msTwinRed", true);
                missile->setScale(0.6f);
                break;
            case PET_WALOONG_2:
                missile->runAnimation("msTwinRed", true);
                missile->setScale(0.8f);
                break;
            case PET_WALOONG_3:
                missile->runAnimation("msTwinRed", true);
                break;
            default:
                
                break;
        }
    }
    
    missile->movingAngle = isTowardLeft?180:0;
    missile->setPosition(this->getPosition());
    missile->extraSpeed = 80;
    this->missile = missile;
//    missile->runAnimation("msTwinGreen", true);
    this->getParent()->addChild(missile);

    isReadyToFire = false;
}
