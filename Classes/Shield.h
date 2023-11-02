//
//  Shield.hpp
//  DaryDual
//
//  Created by PACKSUNG PILL on 12/05/2017.
//
//

#ifndef Shield_hpp
#define Shield_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "Enemies.h"

using namespace cocos2d;
class Shield : public EnemyBase
{
private:
    
public:
    virtual bool init();
    CREATE_FUNC(Shield);
//    float occupyAmount = 0;
    int value = 0;
//    int defenceType = 0;
//    int starCount = 0;
//    int starLevel = 0;
//    bool isShooting = false;
//    bool shouldFire = false;
//    void setShield(int shield, int star_count, int star_level, int occupy_amount, int total_health);
    void setShield(int value);
//    Vector<Shield*> getShields();
//    std::string toString();
//    void fromString(std::string str);
};

#endif /* Shield_hpp */
