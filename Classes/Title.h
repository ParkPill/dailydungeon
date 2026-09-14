//
//  Title.h
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 14. 9. 14..
//
//

#ifndef __LegendDaryTwo__Title__
#define __LegendDaryTwo__Title__


#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "ui/UIScrollView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"
#include "ui/UIPageView.h"
#include "CursorLayer.h"
#include "SettingLayer.h"
#include "base/CCGameController.h"
#define COIN_FARM_MAX 400


using namespace cocos2d;
using namespace cocos2d::ui;
class Title : public Layer
{
private:
public:
    virtual bool init();
    CREATE_FUNC(Title);

    Size size;
    
    Node* pnlTitle;
    Node* pnlOption;
    Node* pnlAchievement;
    Node* pnlKeySetting;
    Node* titleLayer;
    Text* lblPlay;
    Text* lblOption;
    Text* lblExit;
    Text* lblBackFromOption;
    Text* lblAchievement;
    Text* lblKeySetting;
    Text* lblOnline;
    Text* lblSound;
    Text* lblMusic;
    Text* lblBackFromKeySetting;
    Text* lblBackFromAchievement;
    
    Node* currentPanel;
    Node* currentNode;
    Sprite* cursor;
    
    void addListener();
    EventListenerTouchOneByOne* listener;
    virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    EventListenerKeyboard* keyListener;
    void removeListener();
    
    void onPlayClick();
    void onOptionClick();
    void onExitClick();
    void onBackFromOptionClick();
    void onBackFromKeySettingClick();
    void onSoundClick();
    void onMusicClick();
    void onAchievementClick();
    void onNativeAchievementClick();
    void onBackFromAchievementClick();
    void onKeySettingClick();
    void onOnlineClick();
    
    CursorLayer* cursorLayer;
    void monitoringCursorLayer(float dt);
    void onExecuted();
    
    void goToStageLater(float dt);
    void updateCursor();
//    EventListenerTouchAllAtOnce* touchListener;
    /*// controller
    void onKeyDown(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event);
    void onKeyUp(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event);
    void onAxisEvent(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event);
    void onConnectController(Controller* controller, Event* event);
    void onDisconnectedController(Controller* controller, Event* event);*/
    
    Node* bonusWeaponLayer;
    void updateBonusWeaponLayer(float dt);
    int countDown = 5;
    Sprite* getLightSpin(float persistTime);
    std::vector<int> bonusWeapon;
};



#endif /* defined(__LegendDaryTwo__Title__) */
