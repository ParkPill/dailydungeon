//
//  MovingPlatform.cpp
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 26..
//
//

#include "MovingPlatform.h"
#include "GameManager.h"
#include "Droppable.h"
#include "Hero.h"
//bool MovingPlatform::init(){}
MovingPlatform* MovingPlatform::create(const char* name, Point start, Point end)
{
    MovingPlatform* drop = new MovingPlatform();//(MovingPlatform*)Sprite::create("dary.png");
    drop->initWithSpriteFrameName(name);
    drop->getTexture()->setAliasTexParameters();
    drop->startPosition = start;
    drop->endPosition = end;
    return drop;
}
void MovingPlatform::updatePosition(float dt)
{
    if (isTriggerPlatform && (!isTriggered || (endPosition.x == getPosition().x && endPosition.y == getPosition().y))) {
        return;
    }
    
    Point current = this->getPosition();
    Point targetPoint = isTowardEnd?endPosition:startPosition;
    bool isXToPlus = current.x < targetPoint.x;
    bool isYToPlus = current.y < targetPoint.y;
    float moveDistance = 50 * dt;
    float targetX = current.x + moveDistance*(isXToPlus?1:-1);
    if (isXToPlus) {
        if (targetX > targetPoint.x) {
            targetX = targetPoint.x;
        }
    }else{
        if (targetX < targetPoint.x) {
            targetX = targetPoint.x;
        }
    }
    
    float targetY = current.y + moveDistance*(isYToPlus?1:-1);
    if (isYToPlus) {
        if (targetY > targetPoint.y) {
            targetY = targetPoint.y;
        }
    }else{
        if (targetY < targetPoint.y) {
            targetY = targetPoint.y;
        }
    }
    
    if (targetPoint.x == targetX && targetPoint.y == targetY) {
        isTowardEnd = !isTowardEnd;
        isTriggered = false;
    }
    
    
//    desiredRect = RectMake(targetX, targetY, boundingBox().size.width, boundingBox().size.height);
    Point dPoint = Point(targetX, targetY);
    Point diff = dPoint - current;
    
    desiredRect = RectOffset(boundingBox(), diff.x, diff.y);
    
    if (player) {
        Point playerPos = player->getPosition();
        Hero* drop = (Hero*)player;
        drop->desiredPosition = Point(drop->desiredPosition.x + targetX - current.x, drop->desiredPosition.y + targetY - current.y);
        
        if (drop->collisionBoundingBox().origin.x > boundingBox().origin.x + boundingBox().size.width ||
            drop->collisionBoundingBox().origin.x + drop->collisionBoundingBox().size.width < boundingBox().origin.x) {
            this->player = NULL;
            drop->currentPlatform = NULL;
        }
    }
}

Rect MovingPlatform::RectOffset(Rect rect, float x, float y)
{
    return Rect(rect.origin.x + x, rect.origin.y + y, rect.size.width, rect.size.height);
}


