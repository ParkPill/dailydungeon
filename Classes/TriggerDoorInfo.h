//
//  TriggerDoorInfo.h
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 11. 3..
//
//

#ifndef __AlmostLegendary__TriggerDoorInfo__
#define __AlmostLegendary__TriggerDoorInfo__

#include "cocos2d.h"

#define DOOR_TYPE_1 0
#define DOOR_TYPE_2 1
#define DOOR_TYPE_3 2
using namespace cocos2d;
class TriggerDoorInfo : public Node
{
private:
    
public:
    TriggerDoorInfo();
    
    bool isOpen;
    bool alreadyTriggered;
    PointArray* pointArray;
    Sprite* doorSprite;
    Sprite* trigger;
    Sprite* doorTalk;
    Sprite* triggerTalk;
    int doorType;
    
    static TriggerDoorInfo* create(int type, SpriteBatchNode* batch, cocos2d::Point pos, bool open);
    
    Vector<Sprite*> spriteArray;
    
    void openDoor();
    void closeDoor();
};

#endif /* defined(__AlmostLegendary__TriggerDoorInfo__) */
