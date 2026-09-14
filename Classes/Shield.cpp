//
//  Shield.cpp
//  DaryDual
//
//  Created by PACKSUNG PILL on 12/05/2017.
//
//

#include "Shield.h"
#include "GameManager.h"
using namespace cocos2d;


// on "init" you need to initialize your instance
bool Shield::init()
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(! Sprite::init());
        
        bRet = true;
    } while (0);
    
    return bRet;
}
void Shield::setShield(int value){
    initWithSpriteFrameName(strmake("shield/shield%d", (value%1000)%600).c_str());
    this->setScale(0.5f);
    energy = GM->getShieldHPRuneApplied(value);
    maxEnergy = energy;
    DP = GM->getShieldDefRuneApplied(value);
}
//void Shield::setShield(int shield, int star_count, int star_level, int occupy_amount, int total_health){
//    shieldType = shield;
//    defenceType = GameManager::getInstance()->getDefenceType(shield);
//    starCount = star_count;
//    starLevel = star_level;
//    occupyAmount = occupy_amount;
//    energy = total_health;
//}
//Vector<Shield*> Shield::getShields(){
//
//}
//std::string Shield::toString(){
//
//}
//void Shield::fromString(std::string str){
//    ValueVector rows = GameManager::getInstance()->split(str, ",");
//    setShield(rows.at(1).asInt(), rows.at(3).asInt(), rows.at(4).asInt(), rows.at(5).asInt(), rows.at(2).asInt());
//
//    initWithSpriteFrameName(StringUtils::format("shield%d.png", rows.at(1).asInt()));
//    //    this->setScale(5);
////    Sprite* sptHandle = Sprite::createWithSpriteFrameName(StringUtils::format("holder_%d.png", starLevel).c_str());
////    sptHandle->setName("holder");
////    this->addChild(sptHandle);
//}
