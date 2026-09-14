//
//  CursorLayer.cpp
//  LegendDaryTheTower
//
//  Created by SUNG PILL PACK on 14. 5. 19..
//
//
#include "CursorLayer.h"

// on "init" you need to initialize your instance
bool CursorLayer::init()
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(! Layer::init());
		
        cursor = Sprite::createWithSpriteFrameName("targetRed.png");
        this->addChild(cursor);
        cursor->setPosition(Point(500, 300));
        cursor->runAction(RepeatForever::create(RotateBy::create(1.5f, 100)));

        bRet = true;
    } while (0);

    
//    mouseListner = EventListenerMouse::create();
//    mouseListner->onMouseDown = CC_CALLBACK_1(CursorLayer::onMouseDown, this);
//    mouseListner->onMouseUp = CC_CALLBACK_1(CursorLayer::onMouseUp, this);
//    mouseListner->onMouseMove = CC_CALLBACK_1(CursorLayer::onMouseMove, this);
//    mouseListner->onMouseScroll = CC_CALLBACK_1(CursorLayer::onMouseScroll, this);
//    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListner, this);
    
    return bRet;
}

void CursorLayer::Dispose(){
    _eventDispatcher->removeEventListener(mouseListner);
}

void CursorLayer::onMouseDown(cocos2d::Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    Vec2 location = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
    auto isRight = mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT;
//    auto isRight = mouseEvent->getMouseButton();
    isMouseLeftDown = !isRight;
    isMouseRightDown = isRight;
}
void CursorLayer::onMouseUp(cocos2d::Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    Vec2 location = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
//    auto isRight = mouseEvent->getMouseButton();
    isMouseLeftDown = false;
    isMouseRightDown = false;
}
void CursorLayer::onMouseMove(cocos2d::Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    Vec2 location = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
    cursor->setPosition(location);
}
void CursorLayer::onMouseScroll(cocos2d::Event* event)
{
    auto mouseEvent = static_cast<EventMouse*>(event);
    Vec2 location = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
}
