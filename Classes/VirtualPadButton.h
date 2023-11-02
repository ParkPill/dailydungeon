//
//  VirtualPadButton.h
//  MrBoss
//
//  Created by SUNG PILL PACK on 14. 7. 1..
//
//

#ifndef __MrBoss__VirtualPadButton__
#define __MrBoss__VirtualPadButton__

#include "cocos2d.h"

#define BUTTON_JUMP 0
#define BUTTON_RELOAD 1
#define BUTTON_WEAPON 2
#define BUTTON_ACTION 3
#define BUTTON_SHOOT 4
#define BUTTON_AUTO_TOGGLE 5
using namespace cocos2d;
class VirtualPadButton : public cocos2d::Sprite
{
    
public:
    int buttonType;
    static VirtualPadButton* create(int btnType);
    void addListener();
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    void removeListener();
    EventListenerTouchOneByOne* listener;
};

#endif /* defined(__MrBoss__VirtualPadButton__) */
