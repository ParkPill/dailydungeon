//
//  Weapon.hpp
//  DaryDual
//
//  Created by PACKSUNG PILL on 11/05/2017.
//
//

#ifndef Weapon_hpp
#define Weapon_hpp

#include <stdio.h>
#include "cocos2d.h"
#include "Laser.h"

using namespace cocos2d;
class Weapon : public Sprite
{
private:
    
public:
    virtual bool init();
    CREATE_FUNC(Weapon);
    int value;
    int weaponIndex;
//    int weaponType = 0;
//    int starCount = 0;
//    int starLevel = 0;
//    int attackType = 0;
    bool isEnemy = false;
    float occupyAmount = 0;
//    float totalPower = 0;
    float powerPerAttack = 0;
    float criticalPossibility;
    float criticalPower;
    float shootInterval = 0;
    float shootTimeElapse = 0;
    bool isShooting = false;
    bool shouldFire = false;
    void setWeapon(int value);
//    void setWeapon(int weapon, int star_count, int star_level, int occupy_amount, int total_power);
    void startShooting();
    void stopShooting();
    void shooting(float dt);
    void onFire();
    Laser* laser;
    void removeLaser();
//    std::string toString();
//    void fromString(std::string str);
    void updateLaser();
    Vector<EnemyBase*> targetList;
};


#endif /* Weapon_hpp */
