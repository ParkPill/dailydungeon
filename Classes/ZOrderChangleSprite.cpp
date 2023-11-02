//
//  ZOrderChangleSprite.cpp
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 12/14/14.
//
//

#include "ZOrderChangleSprite.h"

void ZOrderChangableSprite::changeZOrderUp(){
    this->setLocalZOrder(10);
}
void ZOrderChangableSprite::changeZOrderDown(){
    this->setLocalZOrder(-10);
}

void ZOrderChangableSprite::initWithSpriteFrameNameYo(const char* name){
    this->initWithSpriteFrameName(name);
}