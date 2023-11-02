//
//  Weapon.cpp
//  DaryDual
//
//  Created by PACKSUNG PILL on 11/05/2017.
//
//

#include "Weapon.h"
#include "GameManager.h"
using namespace cocos2d;

// on "init" you need to initialize your instance
bool Weapon::init()
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(! Sprite::init());
        
        bRet = true;
    } while (0);
    
    return bRet;
}
void Weapon::setWeapon(int value){
    this->value = value;
    this->weaponIndex = value%1000;
//    switch (weaponType) {
//        case WEAPON_PISTOL: // 20
//            shootInterval = 0.5f;
//            break;
//        case WEAPON_MAGNUM: // 25
//            shootInterval = 0.4f;
//            break;
//        case WEAPON_UZI: // 34
//            shootInterval = 0.2f;
//            break;
//        case WEAPON_RIFLE: // 25
//            shootInterval = 0.5f;
//            break;
//        case WEAPON_FIRE_BOTTLER: // 42
//            shootInterval = 0.7f;
//            break;
//        case WEAPON_ANTI_BAT: // 60
//            shootInterval = 0.5f;
//            break;
//        case WEAPON_SNIPER_RIFLE: // 36
//            shootInterval = 0.8f;
//            break;
//        case WEAPON_RIFLE_GRENADE: // 35
//            shootInterval = 0.6f;
//            break;
//        case WEAPON_CROSS_BOW: // 49
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_MACHINE_GUN: // 110
//            shootInterval = 0.2f;
//            break;
//        case WEAPON_STRIKER: // 61
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_NIFE_THROWER: // 88
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_BALL_SHOOTER: // 111
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_FLAME_THROWER: // 207
//            shootInterval = 0.2f;
//            break;
//        case WEAPON_ROCKET_LAUNCHER: // 132
//            shootInterval = 0.7f;
//            break;
//        case WEAPON_BAZOOKA: // 44
//            shootInterval = 0.7f;
//            break;
//        case WEAPON_MATT_A4: // 164
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_LASER_GUN: // 161
//            shootInterval = 0.1f;
//            break;
//        case WEAPON_ENERGY_GUN: // 85
//            shootInterval = 0.4f;
//            break;
//        case WEAPON_GUIDED_ROCKET_LAUNCHER: // 107
//            shootInterval = 0.7f;
//            break;
//        case WEAPON_SLUGGER: // 171
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_DRAGON_RIFLE: // 282
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_SLUGGER_NG: // 171
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_LIGHTNING_GUN: // 171
//            shootInterval = 0.2f;
//            break;
//        case WEAPON_SHARK_GUN: // 171
//            shootInterval = 1.0f;
//            break;
//        case WEAPON_LASER_COIN: // 171
//            shootInterval = 0.1f;
//            break;
//        default:
//            break;
//    }
    initWithSpriteFrameName(GM->getWeaponImageFileName(value%1000));
    this->setScale(0.5f);
    shootInterval = GM->getWeaponCoolTime(value%1000)*(1 + GM->getResearchCoolTime()*0.01f);
    powerPerAttack = GM->getWeaponPowerRuneResearchApplied(value);
    criticalPossibility = GM->getWeaponCriticalPossibilityRuneResearchApplied(value);
    criticalPower = GM->getResearchCriticalPower()*0.01f;
}
//
//void Weapon::setWeapon(int weapon, int star_count, int star_level, int occupy_amount, int total_power){
//    weaponType = weapon; //test 
//    attackType = weapon%4;//GameManager::getInstance()->getWeaponAttackType(weapon);
//    starCount = star_count;
//    starLevel = star_level;
//    occupyAmount = occupy_amount;
//    totalPower = total_power;
//
//    switch (weaponType) {
//        case WEAPON_PISTOL: // 20
//            shootInterval = 0.5f;
//            break;
//        case WEAPON_MAGNUM: // 25
//            shootInterval = 0.4f;
//            break;
//        case WEAPON_UZI: // 34
//            shootInterval = 0.2f;
//            break;
//        case WEAPON_RIFLE: // 25
//            shootInterval = 0.5f;
//            break;
//        case WEAPON_FIRE_BOTTLER: // 42
//            shootInterval = 0.7f;
//            break;
//        case WEAPON_ANTI_BAT: // 60
//            shootInterval = 0.5f;
//            break;
//        case WEAPON_SNIPER_RIFLE: // 36
//            shootInterval = 0.8f;
//            break;
//        case WEAPON_RIFLE_GRENADE: // 35
//            shootInterval = 0.6f;
//            break;
//        case WEAPON_CROSS_BOW: // 49
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_MACHINE_GUN: // 110
//            shootInterval = 0.2f;
//            break;
//        case WEAPON_STRIKER: // 61
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_NIFE_THROWER: // 88
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_BALL_SHOOTER: // 111
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_FLAME_THROWER: // 207
//            shootInterval = 0.2f;
//            break;
//        case WEAPON_ROCKET_LAUNCHER: // 132
//            shootInterval = 0.7f;
//            break;
//        case WEAPON_BAZOOKA: // 44
//            shootInterval = 0.7f;
//            break;
//        case WEAPON_MATT_A4: // 164
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_LASER_GUN: // 161
//            shootInterval = 0.1f;
//            break;
//        case WEAPON_ENERGY_GUN: // 85
//            shootInterval = 0.4f;
//            break;
//        case WEAPON_GUIDED_ROCKET_LAUNCHER: // 107
//            shootInterval = 0.7f;
//            break;
//        case WEAPON_SLUGGER: // 171
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_DRAGON_RIFLE: // 282
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_SLUGGER_NG: // 171
//            shootInterval = 0.3f;
//            break;
//        case WEAPON_LIGHTNING_GUN: // 171
//            shootInterval = 0.2f;
//            break;
//        case WEAPON_SHARK_GUN: // 171
//            shootInterval = 1.0f;
//            break;
//        case WEAPON_LASER_COIN: // 171
//            shootInterval = 0.1f;
//            break;
//        default:
//            break;
//    }
//    shootInterval /= 2;
//    powerPerAttack = totalPower*shootInterval;
//}
void Weapon::startShooting(){
    isShooting = true;
    shouldFire = false;
//    Node* world;
//    if(GM->currentStageIndex == STAGE_DUAL){
//        world = DUAL_WORLD;
//    }else{
//        world = WORLD;
//    }
    if(weaponIndex == WEAPON_LASER_GUN || weaponIndex == WEAPON_LASER_COIN){
        if (laser == nullptr) {
            if (value%1000 == WEAPON_LASER_GUN) {
                GameManager::getInstance()->playSoundEffect(SOUND_LASER_BUZZ, 0.5f);
                laser = Laser::create("laserThinBlue.png", "laserThinBlueGathering.png", "laserThinBlueHit", DUAL_WORLD->theMap);
            }else{
                GameManager::getInstance()->playSoundEffect(SOUND_LASER_WOONG, 0.5f);
                laser = Laser::create("bigLaserLine.png", "bigLaserCircle.png", "", DUAL_WORLD->theMap);
                laser->setScaleY(0.4);
                laser->sptBeam->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
                laser->sptHit->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
                //            laser->sptHit->addChild(getLight());
                //            laser->sptBeam->addChild(getLight());
                laser->sptHit->setScale(2, 1.6);
            }
            if(targetList.size() > 0){
                laser->enemyArray = targetList;
            }
            //        laser->enemyArray = enemyArray;
            //        laser->rectArray = stageTileRectArray;
            DUAL_WORLD->addChild(laser);
        }
    }
//    if (wp->value%1000 == WEAPON_LASER_GUN || wp->value%1000 == WEAPON_LASER_COIN) {
//
//        if (wp->value%1000 == WEAPON_LASER_GUN) {
//            GameManager::getInstance()->playSoundEffect(SOUND_LASER_BUZZ);
//            laser = Laser::create("laserThinBlue.png", "laserThinBlueGathering.png", "laserThinBlueHit");
//        }else{
//            GameManager::getInstance()->playSoundEffect(SOUND_LASER_WOONG);
//            laser = Laser::create("bigLaserLine.png", "bigLaserCircle.png", "");
//            laser->setScaleY(0.4);
//            laser->sptBeam->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
//            laser->sptHit->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
//            laser->sptHit->addChild(getLight());
//            laser->sptBeam->addChild(getLight());
//            laser->sptHit->setScale(2, 1.6);
//        }
//        for(auto shield:enemyShields){
//            laser->enemyArray.pushBack((EnemyBase*)shield);
//        }
//        laser->enemyArray.pushBack((EnemyBase*)enemy);
//        laser->rectArray = stageTileRectArray;
//        this->addChild(laser);
//        laser->setTag((int)laserArray.size() + 793);
//        wp->setTag((int)laserArray.size() + 793);
//        laserArray.pushBack(laser);
//    }
}
void Weapon::removeLaser(){
    if (laser!=nullptr) {
        laser->removeFromParent();
        laser = nullptr;
        GameManager::getInstance()->stopSoundEffect(SOUND_LASER_BUZZ);
    }
}
void Weapon::stopShooting(){
    if(weaponIndex == WEAPON_LASER_GUN || weaponIndex == WEAPON_LASER_COIN){
        removeLaser();
    }
}
void Weapon::shooting(float dt){
    shootTimeElapse -= dt;
    if (shootTimeElapse <= 0) {
        shouldFire = true;
    }
    if(shouldFire){
        if(weaponIndex == WEAPON_LASER_GUN || weaponIndex == WEAPON_LASER_COIN){
            updateLaser();
        }
    }
}
void Weapon::updateLaser(){
    float angle = getRotation();
    float gunWidth = getContentSize().width;
    float x = cos(angle*3.14/180)*gunWidth;
    float y = -sin(angle*3.14/180)*gunWidth;
    Point gunLengthPos = Point(x, y);
    laser->setLaser(this->getPosition() + gunLengthPos, angle);
    if (laser->hitEnemy != nullptr && laser->hitEnemy->energy > 0)
    {
        int damage = powerPerAttack;//GameManager::getInstance()->getWeaponPower(player->weapon);
        log("weapon power: %d", damage);
        laser->hitEnemy->energy -= (int)damage;
        laser->hitEnemy->showEnergy();
        
        DUAL_WORLD->showDamage(damage, laser->hitEnemy->getPosition());
        if (laser->hitEnemy->energy <= 0 && !DUAL_WORLD->enemiesToRemove.contains(laser->hitEnemy)) {
            DUAL_WORLD->enemiesToRemove.pushBack(laser->hitEnemy);
//            Shield* theShield = (Shield*)laser->hitEnemy;
//            if(isEnemy){
//                DUAL_WORLD->playerShields.eraseObject(theShield);
//            }else{
//                DUAL_WORLD->enemyShields.eraseObject(theShield);
//            }
            //                laser->enemyArray = enemyArray;
            //                laser->enemyArray.eraseObject(laser->hitEnemy);
            
            DUAL_WORLD->runEffect(EFFECT_EXPLODE_BIG, laser->hitEnemy->getPosition());
//            DUAL_WORLD->createRewards(laser->hitEnemy);
            if (DUAL_WORLD->enemyArray.contains(laser->hitEnemy)) {
                DUAL_WORLD->enemyKilledCount++;
            }
            //                enemyArray.eraseObject(laser->hitEnemy);
        }else
        {
            laser->hitEnemy->blinkForAWhile();
        }
        laser->hitEnemy = nullptr;
    }
//    if (DUAL_WORLD->loadedBulletCount <= 0) {
//        removeLaser();
//    }
}
void Weapon::onFire(){
    shootTimeElapse += shootInterval;
    shouldFire = false;
}
//
//std::string Weapon::toString(){
//    return "";
//}
//void Weapon::fromString(std::string str){
//    ValueVector rows = GameManager::getInstance()->split(str, ",");
//    setWeapon(rows.at(1).asInt(), rows.at(3).asInt(), rows.at(4).asInt(), rows.at(5).asInt(), GameManager::getInstance()->getItemLeveledStat(str));
//
//    initWithSpriteFrameName(GameManager::getInstance()->getWeaponImageFileName(rows.at(1).asInt()));
////    this->setScale(5);
//    Sprite* sptHandle = Sprite::createWithSpriteFrameName(StringUtils::format("holder_%d.png", starLevel).c_str());
//    sptHandle->setName("holder");
//
//    this->addChild(sptHandle);
//}
