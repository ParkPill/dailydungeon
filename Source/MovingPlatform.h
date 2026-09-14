//
//  Droppable.h
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 26..
//
//

#ifndef AlmostLegendary_MovingPlatform_h
#define AlmostLegendary_MovingPlatform_h

#include "cocos2d.h"


using namespace cocos2d;

class MovingPlatform : public cocos2d::Sprite
{
private:
    
public:
    cocos2d::Point startPosition;
    cocos2d::Point endPosition;
    cocos2d::Rect desiredRect;
    Sprite* player;
    bool isTowardEnd;
    bool isTriggerPlatform;
    bool isTriggered;
    static MovingPlatform* create(const char* name, cocos2d::Point start, cocos2d::Point end);
    void updatePosition(float dt);
    cocos2d::Rect RectOffset(cocos2d::Rect rect, float x, float y);
};
#endif
