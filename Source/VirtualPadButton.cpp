//
//  VirtualPadButton.cpp
//  MrBoss
//
//  Created by SUNG PILL PACK on 14. 7. 1..
//
//

#include "VirtualPadButton.h"
#include "GameManager.h"
VirtualPadButton* VirtualPadButton::create(int btnType){
    VirtualPadButton* btn = new VirtualPadButton();
    btn->buttonType = btnType;
    switch (btnType) {
        case BUTTON_JUMP:
            btn->initWithFile("btnJump.png");
            break;
        case BUTTON_RELOAD:
            btn->initWithFile("btnReload.png");
            break;
        case BUTTON_WEAPON:
            btn->initWithFile("btnWeapon.png");
            break;
        case BUTTON_ACTION:
            btn->initWithFile("btnAction.png");
            break;
        case BUTTON_SHOOT:
            btn->initWithFile("btnShoot.png");
            break;
        case BUTTON_AUTO_TOGGLE:
            btn->initWithFile("UI/btnToggleAuto.png");
            break;
        default:
            break;
    }
    btn->setScale(2);
    btn->addListener();

    btn->autorelease();
    return btn;
    
}
void VirtualPadButton::addListener(){
    listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        Point p = touch->getLocation();
        cocos2d::Rect rect = this->getBoundingBox();
        
        if(rect.containsPoint(p))
        {
            this->onTouchBegan(touch, event);
            return true; // to indicate that we have consumed it.
        }
        
        return false; // we did not consume this event, pass thru.
    };
    
    listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        this->onTouchEnded(touch, event);
    };
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 30);
    

}
bool VirtualPadButton::onTouchBegan(Touch *touch, Event *unused_event)
{
//	Point location = CCDirector::getInstance()->convertToGL(touch->getLocationInView());
//	location = this->convertToNodeSpace(location);
    if(!this->isVisible()){
        return false;
    }
    this->setOpacity(100);
	switch (this->buttonType) {
        case BUTTON_JUMP:
            GameManager::getInstance()->buttonDown(BUTTON_B);
            break;
        case BUTTON_RELOAD:
            GameManager::getInstance()->getCurrentStageLayer()->reload();
            break;
        case BUTTON_WEAPON:
        {
            int lastSlot = GameManager::getInstance()->getCurrentStageLayer()->currentSlot;
            lastSlot++;
            int weapon = 0 ;
            if (GameManager::getInstance()->getCurrentStageLayer()->getWeaponAtSlot(lastSlot) < 0) {
                GameManager::getInstance()->getCurrentStageLayer()->changeWeapon(0);
                weapon = GameManager::getInstance()->getCurrentStageLayer()->getWeaponAtSlot(0);
            }else{
                GameManager::getInstance()->getCurrentStageLayer()->changeWeapon(lastSlot);
                weapon = GameManager::getInstance()->getCurrentStageLayer()->getWeaponAtSlot(lastSlot);
            }
            GM->lastWeaponSlot = lastSlot;
            
            GameManager::getInstance()->showDisposableMessage(GameManager::getInstance()->getWeaponName(weapon).c_str(), GameManager::getInstance()->getHudLayer());
        }
            break;
        case BUTTON_ACTION:
            GameManager::getInstance()->getCurrentStageLayer()->takeAction();
            break;
        case BUTTON_SHOOT:
            GameManager::getInstance()->getHudLayer()->cursorLayer->isMouseLeftDown = true;
            break;
        case BUTTON_AUTO_TOGGLE:
            GameManager::getInstance()->getHudLayer()->toggleAuto();
            break;
        default:
            break;
    }
    
	return false;
}

void VirtualPadButton::onTouchMoved(Touch *touch, Event *unused_event)
{
}

void VirtualPadButton::onTouchEnded(Touch *touch, Event *unused_event)
{
    this->setOpacity(255);
    switch (this->buttonType) {
        case BUTTON_JUMP:
            GameManager::getInstance()->buttonUp(BUTTON_B);
            break;
        case BUTTON_RELOAD:
//            GameManager::getInstance()->buttonUp(BUTTON_RELOAD);
            break;
        case BUTTON_WEAPON:
            GameManager::getInstance()->buttonUp(BUTTON_WEAPON);
//            GameManager::getInstance()->doButtonB(false);
            break;
        case BUTTON_ACTION:
            
            break;
        case BUTTON_SHOOT:
            GameManager::getInstance()->getHudLayer()->cursorLayer->isMouseLeftDown = false;
            break;
        case BUTTON_AUTO_TOGGLE:
//            GameManager::getInstance()->getHudLayer()->toggleAuto();
            break;
        default:
            break;
    }
}

void VirtualPadButton::removeListener(){
    Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
}
