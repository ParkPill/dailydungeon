//
//  TriggerDoorInfo.cpp
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 11. 3..
//
//

#include "TriggerDoorInfo.h"

TriggerDoorInfo::TriggerDoorInfo()
{
    isOpen = false;
    alreadyTriggered = false;
    
    pointArray = PointArray::create(10);
    pointArray->retain();
    
}

TriggerDoorInfo* TriggerDoorInfo::create(int type, SpriteBatchNode* batch, Point pos, bool open)
{
    TriggerDoorInfo* info  = new TriggerDoorInfo();
    info->autorelease();
    info->doorType = type;
    info->isOpen = open;
    Sprite* sprite;
    const char* flag = "triggerFlag0.png";
    const char* doorFlag = "triggerDoorFlag0.png";
    bool everySecond = false;
    Point dtPos = Point(18, -8);
    switch (type) {
        case DOOR_TYPE_1:
            for (int i = 0; i < 3; i++) {
                sprite = Sprite::createWithSpriteFrameName("doorForTrigger1.png");
                info->spriteArray.pushBack(sprite);
                sprite->setPosition(pos + Point(8, -4-i*16));
                batch->addChild(sprite);
                
                sprite = Sprite::createWithSpriteFrameName("doorForTrigger1.png");
                info->spriteArray.pushBack(sprite);
                sprite->setPosition(pos + Point(8,-4-i*16 - 8));
                batch->addChild(sprite);
            }
            info->trigger = Sprite::createWithSpriteFrameName(info->isOpen?"trigger1.png":"trigger0.png");
            break;
        case DOOR_TYPE_2:
            for (int i = 0; i < 3; i++) {
                sprite = Sprite::createWithSpriteFrameName(everySecond?"doorForTrigger0.png":"doorForTrigger0.png");
                info->spriteArray.pushBack(sprite);
                sprite->setPosition(pos + Point(8 + i*16, 4 - 16));
                batch->addChild(sprite);
                
                sprite = Sprite::createWithSpriteFrameName(everySecond?"doorForTrigger0.png":"doorForTrigger0.png");
                info->spriteArray.pushBack(sprite);

                sprite->setPosition(pos + Point(8 + i*16, 4 - 8));
                batch->addChild(sprite);
                
                everySecond = !everySecond;
            }
            info->trigger = Sprite::createWithSpriteFrameName(info->isOpen?"trigger1.png":"trigger0.png");
            dtPos += Point(38, 0);
            doorFlag = "triggerDoorFlag1.png";
            flag = "triggerFlag1.png";
            break;
        case DOOR_TYPE_3:
            for (int i = 0; i < 3; i++) {
                for (int j =0; j < 3; j++) {
                    sprite = Sprite::createWithSpriteFrameName(everySecond?"doorForTrigger0.png":"doorForTrigger0.png");
                    info->spriteArray.pushBack(sprite);
                    sprite->setPosition(pos + Point(8 + j*16, 4 - i*16 - 16));
                    batch->addChild(sprite);
                    
                    sprite = Sprite::createWithSpriteFrameName(everySecond?"doorForTrigger0.png":"doorForTrigger0.png");
                    info->spriteArray.pushBack(sprite);
                    sprite->setPosition(pos + Point(8 + j*16, 4 + 8 - i*16 - 16));
                    batch->addChild(sprite);
                    
                    everySecond = !everySecond;
                }
                everySecond = !everySecond;
            }
            
            info->trigger = Sprite::createWithSpriteFrameName(info->isOpen?"trigger1.png":"trigger0.png");
            
            doorFlag = "triggerDoorFlag2.png";
            flag = "triggerFlag2.png";
            dtPos += Point(38, -4);
            break;
        default:
            break;
    }
    batch->addChild(info->trigger);
    
    info->doorTalk = Sprite::createWithSpriteFrameName(doorFlag);
    info->doorTalk->setPosition(pos + dtPos);
    batch->addChild(info->doorTalk);
    
    info->triggerTalk = Sprite::createWithSpriteFrameName(flag);
    batch->addChild(info->triggerTalk);
    
    if (open) {
        info->openDoor();
    }

    return info;
}

void TriggerDoorInfo::openDoor()
{
    Sprite* sprite;
    float rt = 0.3f;
    switch (doorType) {
        case DOOR_TYPE_1:
            for (int i = 0; i < 6; i+=2) {
                sprite = spriteArray.at(i);
                sprite->runAction(EaseOut::create(MoveBy::create(0.3f, Point(-10, 0)), 2.5));
                sprite->runAction(FadeOut::create(0.3f));
                sprite = spriteArray.at(i+1);
                sprite->runAction(EaseOut::create(MoveBy::create(0.3f, Point(10, 0)), 2.5));
                sprite->runAction(FadeOut::create(0.3f));
            }
            doorTalk->runAction(EaseOut::create(MoveBy::create(0.3f, Point(-10, 0)), 2.5));
            doorTalk->runAction(FadeOut::create(0.3f));
            break;
        case DOOR_TYPE_2:
            for (int i = 0; i < 6; i+=2) {
                sprite = spriteArray.at(i);
                sprite->runAction(EaseOut::create(MoveBy::create(0.3f, Point(0, -10)), 2.5));
                sprite->runAction(FadeOut::create(rt));
                sprite = spriteArray.at(i+1);
                sprite->runAction(EaseOut::create(MoveBy::create(0.3f, Point(0, 10)), 2.5));
                sprite->runAction(FadeOut::create(rt));
            }
            doorTalk->runAction(EaseOut::create(MoveBy::create(0.3f, Point(0, 10)), 2.5));
            doorTalk->runAction(FadeOut::create(rt));
            break;
        case DOOR_TYPE_3:
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 6; j += 2) {
                    sprite = spriteArray.at(i*6 + j);
                    sprite->runAction(EaseOut::create(MoveBy::create(rt, Point(-10, 10)), 2.5));
                    sprite->runAction(FadeOut::create(rt));
                    
                    sprite = spriteArray.at(i*6 + j + 1);
                    sprite->runAction(EaseOut::create(MoveBy::create(rt, Point(10, -10)), 2.5));
                    sprite->runAction(FadeOut::create(rt));
                }
            }
            doorTalk->runAction(EaseOut::create(MoveBy::create(rt, Point(-10, 10)), 2.5));
            doorTalk->runAction(FadeOut::create(rt));
            
            break;
        default:
            break;
    }
    
    trigger->setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("trigger1.png"));
    isOpen = true;
}

void TriggerDoorInfo::closeDoor()
{
    Sprite* sprite;
    float rt = 0.3f;
    switch (doorType) {
        case DOOR_TYPE_1:
            for (int i = 0; i < 6; i+=2) {
                sprite = spriteArray.at(i);
                sprite->runAction(EaseOut::create(MoveBy::create(0.3f, Point(10, 0)), 5));
                sprite->runAction(FadeIn::create(0.3f));
                sprite = spriteArray.at(i+1);
                sprite->runAction(EaseOut::create(MoveBy::create(0.3f, Point(-10, 0)), 5));
                sprite->runAction(FadeIn::create(0.3f));
            }
            doorTalk->runAction(EaseOut::create(MoveBy::create(0.3f, Point(10, 0)), 5));
            doorTalk->runAction(FadeIn::create(0.3f));
            break;
        case DOOR_TYPE_2:
            for (int i = 0; i < 6; i+=2) {
                sprite = spriteArray.at(i);
                sprite->runAction(EaseOut::create(MoveBy::create(0.3f, Point(0, 10)), 5));
                sprite->runAction(FadeIn::create(0.3f));
                sprite = spriteArray.at(i+1);
                sprite->runAction(EaseOut::create(MoveBy::create(0.3f, Point(0, -10)), 5));
                sprite->runAction(FadeIn::create(0.3f));
            }
            doorTalk->runAction(EaseOut::create(MoveBy::create(0.3f, Point(0, -10)), 5));
            doorTalk->runAction(FadeIn::create(0.3f));
            break;
        case DOOR_TYPE_3:
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 6; j += 2) {
                    sprite = spriteArray.at(i*6 + j);
                    sprite->runAction(EaseOut::create(MoveBy::create(rt, Point(10, -10)), 2.5));
                    sprite->runAction(FadeIn::create(rt));
                    
                    sprite = spriteArray.at(i*6 + j +1);
                    sprite->runAction(EaseOut::create(MoveBy::create(rt, Point(-10, 10)), 2.5));
                    sprite->runAction(FadeIn::create(rt));
                }
            }
            doorTalk->runAction(EaseOut::create(MoveBy::create(rt, Point(10, -10)), 2.5));
            doorTalk->runAction(FadeIn::create(rt));
            break;
        default:
            break;
    }
    
    trigger->setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("trigger0.png"));
    isOpen = false;
}