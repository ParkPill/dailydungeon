//
//  SettingLayer.h
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 14. 9. 3..
//
//

#ifndef __LegendDaryTwo__SettingLayer__
#define __LegendDaryTwo__SettingLayer__


#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "ui/UIScrollView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"
#include "LegendDaryButton.h"
//#include "TopBar.h"
#define SETTING_ACTION_NONE 0
#define SETTING_ACTION_SOUND 1
#define SETTING_ACTION_MUSIC 2
#define SETTING_ACTION_BUTTON_SIZE 3

using namespace cocos2d;
using namespace cocos2d::ui;
//using namespace cocostudio;
class SettingLayer : public Layer
{
    
private:
    
public:
    void back();
    virtual bool init();
    CREATE_FUNC(SettingLayer);
    static SettingLayer* getSettingLayer();
    Size size;
//    TopBar* topBar;
    Slider* soundBar;
    Slider* musicBar;
    Slider* buttonSizeBar;
    Layout* settingLayer;
    
    LegendDaryButton* btnLeftRight;
    LegendDaryButton* btnA;
    LegendDaryButton* btnB;
    Sprite* btnSizeBackground;
    
    Button* btnBack;
    Button* btnSave;
    void onLanguage(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onLike(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onConnect(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onCoupon(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onSaveGame(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onLoadGame(Ref* pSender, ui::Widget::TouchEventType eEventType);
    Sprite* sptSoundBarContent;
    Sprite* sptMusicBarContent;
    Sprite* sptButtonSizeBarContent;
    void updateBarContent();
    int action;
    Point touchStartPos;
    Button* btnConnect;
    void onBack(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void musicSliderEvent(Ref *pSender, Slider::EventType type);
    void soundSliderEvent(Ref *pSender, Slider::EventType type);
    void buttonSizeSliderEvent(Ref *pSender, Slider::EventType type);
    bool loginState;
    void changeLoginState(bool signIn);
    void changeLoginStateLater(float dt);
    virtual void onEnter() override;
    virtual void onExit() override;
    void onButtonSizeTouch(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onMusicSliderTouch(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onSoundSliderTouch(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onKeyReleased( cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event *event );
    void onDataWarningClosed(Ref* obj);
};



#endif /* defined(__LegendDaryTwo__SettingLayer__) */
