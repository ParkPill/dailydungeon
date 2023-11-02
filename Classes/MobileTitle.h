//
//  MobileTitle.hpp
//  LegendDaryDungeon
//
//  Created by PACKSUNG PILL on 11/12/2018.
//

#ifndef MobileTitle_hpp
#define MobileTitle_hpp

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "ui/UIScrollView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"
#include "ui/UIPageView.h"
#include "base/CCGameController.h"


using namespace cocos2d;
using namespace cocos2d::ui;
class MobileTitle : public Layer
{
private:
public:
    cocos2d::Size size;
    virtual bool init();
    CREATE_FUNC(MobileTitle);
    Label* lblLoading;
    
    void updateTitle(float dt);
    void onTouchTitle();
    int nameHandleState = 0;
    Text* lblID;
    
    void showRegisterName();
    Vector<Node*> popupArray;
    void onUpdateClick();
    void closePopup();
    void setAsPopup(Node* node);
    void setTitle(Node* titleBack, std::string menuKey);
    void onOkNameClick(Ref* ref);
    void showInstanceMessage(std::string msg, float stayTime = 2);
    
    void showIndicator();
    void indicatorTimeOver(float dt);
    void hideIndicator();
    Node* indicator;
    void showUserSelect();
    void onNewUserSelect(Ref* ref);
    void onOldUserSelect(Ref* ref);
    void showInputUserIDAndName();
    void onCheckExistingUser(Ref* ref);
    
    bool isRestartingReserved = false;
    void restartTheGame();
    bool isSendingPassword = false;
    void onRestartAsNew();
    Text* lblWelcome;
    
    EventListenerKeyboard* keyListener;
    void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    void removeListener();
    void askExit();
    void onExitYesClick();
    
    std::string nameToRegister = "";
    int changeAccountResult = -1;
};



#endif /* MobileTitle_hpp */
