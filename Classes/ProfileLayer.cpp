//
//  ProfileLayer.cpp
//  LegendDaryDungeon
//
//  Created by PACKSUNG PILL on 03/01/2019.
//

#include "ProfileLayer.h"
bool ProfileLayer::init()
{
    Layer::init();
    size = Director::getInstance()->getWinSize();
    
    // init done
    return true;
}
