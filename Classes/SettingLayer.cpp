//
//  SettingLayer.cpp
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 14. 9. 3..
//
//

#include "SettingLayer.h"
#include "SimpleAudioEngine.h"
#include "GameManager.h"
#include "LanguageManager.h"
#include "MyMessageBox.h"
#include "Title.h"
#include "NativeInterface.h"

bool SettingLayer::init()
{
    Layer::init();
    size = Director::getInstance()->getWinSize();
    this->setKeypadEnabled(true);
    
    settingLayer = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("LegendDary_UI_Setting.json"));
    settingLayer->setPosition(Point(size.width/2 - settingLayer->getContentSize().width/2, 0));
    if (size.width < 960) {
        float scaleX = size.width/960;
        settingLayer->setScaleX(scaleX);
        settingLayer->setPositionX(0);
    }
    this->addChild(settingLayer, -1);
    settingLayer->setTouchEnabled(true);
    settingLayer->setScaleY(size.height/settingLayer->getContentSize().height);
    
    Button* sptBlack = Button::create("whiteRect.png");
    sptBlack->setColor(Color3B::BLACK);
    sptBlack->setScale(size.width, size.height);
    this->addChild(sptBlack, -100);
    sptBlack->setPosition(Point(size.width/2, size.height/2));
    
    Button* btnLike = dynamic_cast<Button*>(Helper::seekWidgetByName(settingLayer, "btnLike"));
    btnLike->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onLike, this));
    
    btnConnect = dynamic_cast<Button*>(Helper::seekWidgetByName(settingLayer, "btnConnect"));
    btnConnect->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onConnect, this));
    
    Button* btnCoupon = Button::create("backButtonBase.png");
    btnCoupon->setPosition(btnConnect->getPosition() + Point(-btnConnect->getContentSize().width/2 + btnCoupon->getContentSize().width/2, btnConnect->getContentSize().height/2 + btnCoupon->getContentSize().height/2 + 10));
    btnCoupon->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onCoupon, this));
    btnConnect->getParent()->addChild(btnCoupon);
    btnCoupon->setTitleFontName(GameManager::getInstance()->getFont(FONT_BITDUST_ONE));
    btnCoupon->setTitleColor(Color3B(28, 135, 171));
    btnCoupon->setTitleFontSize(30);
    btnCoupon->setTitleText("COUPON");
    
    btnSave = Button::create("backButtonBase.png");
    btnSave->setPosition(btnCoupon->getPosition() + Point(btnCoupon->getContentSize().width/2 + btnSave->getContentSize().width/2 + 20, 0));
    btnSave->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onSaveGame, this));
    btnCoupon->getParent()->addChild(btnSave);
    btnSave->setTitleFontName(GameManager::getInstance()->getFont(FONT_BITDUST_ONE));
    btnSave->setTitleColor(Color3B(28, 135, 171));
    btnSave->setTitleFontSize(26);
    btnSave->setTitleText("SAVE\nLOAD");
    
//    btnSave->setVisible(false);
    /*Button* btnLoad = Button::create("backButtonBase.png");
    btnLoad->setPosition(btnSave->getPosition() + Point(btnSave->getContentSize().width/2 + btnLoad->getContentSize().width/2 + 20, 0));
    btnLoad->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onLoadGame, this));
    btnCoupon->getParent()->addChild(btnLoad);
    btnLoad->setTitleFontName(GameManager::getInstance()->getFont(FONT_BITDUST_ONE));
    btnLoad->setTitleColor(Color3B(28, 135, 171));
    btnLoad->setTitleFontSize(30);
    btnLoad->setTitleText("LOAD");*/
    
    ImageView* sptIcon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(settingLayer, "Image_8"));;
    sptIcon->setPosition(sptIcon->getPosition() + Point(40, 0));
    
    Text* lblText = dynamic_cast<Text*>(Helper::seekWidgetByName(settingLayer, "Label_7_0"));;
    lblText->setFontSize(30);
    lblText->setTextColor(Color4B(253,220,37,255));
    lblText->setColor(Color3B::WHITE);
    ((Label*)lblText->getVirtualRenderer())->enableOutline(Color4B(68, 80, 82, 255), 5);
    
    btnBack = dynamic_cast<Button*>(Helper::seekWidgetByName(settingLayer, "btnBack_0"));
    btnBack->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onBack, this));//    btnBack->setTitleText(LanguageManager::getInstance()->getText(STR_BACK));
    
    float soundVolumn = CocosDenshion::SimpleAudioEngine::getInstance()->getEffectsVolume();
    soundBar = dynamic_cast<Slider*>(Helper::seekWidgetByName(settingLayer, "pbSound"));
    soundBar->addEventListener(CC_CALLBACK_2(SettingLayer::soundSliderEvent, this));
    soundBar->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onSoundSliderTouch, this));
    soundBar->setPercent(soundVolumn*100);
    
    float musicVolumn = CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicVolume();
    musicBar = dynamic_cast<Slider*>(Helper::seekWidgetByName(settingLayer, "pbMusic"));
    musicBar->addEventListener(CC_CALLBACK_2(SettingLayer::musicSliderEvent, this));
    musicBar->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onMusicSliderTouch, this));
    musicBar->setPercent(musicVolumn*100);
    
    float buttonSize = GameManager::getInstance()->getButtonSize();
    buttonSizeBar = dynamic_cast<Slider*>(Helper::seekWidgetByName(settingLayer, "pbButtonSize"));
    buttonSizeBar->addEventListener(CC_CALLBACK_2(SettingLayer::buttonSizeSliderEvent, this));
    buttonSizeBar->setPercent(buttonSize*100);
    buttonSizeBar->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onButtonSizeTouch, this));
    action = SETTING_ACTION_NONE;

    sptSoundBarContent = Sprite::create("progressBarContent.png");
    soundBar->addChild(sptSoundBarContent, -1);
    sptSoundBarContent->setAnchorPoint(Point(0, 0.5));
    sptSoundBarContent->setPosition(Point(0, soundBar->getContentSize().height/2));
    
    sptMusicBarContent = Sprite::create("progressBarContent.png");
    musicBar->addChild(sptMusicBarContent, -1);
    sptMusicBarContent->setAnchorPoint(Point(0, 0.5));
    sptMusicBarContent->setPosition(Point(0, musicBar->getContentSize().height/2));
    
    sptButtonSizeBarContent = Sprite::create("progressBarContent.png");
    buttonSizeBar->addChild(sptButtonSizeBarContent, -1);
    sptButtonSizeBarContent->setAnchorPoint(Point(0, 0.5));
    sptButtonSizeBarContent->setPosition(Point(0, buttonSizeBar->getContentSize().height/2));
    
    updateBarContent();
    
  /*  sptSoundButton = Sprite::create("progressBarButton.png");
    this->addChild(sptSoundButton);
    sptSoundButton->setPosition(Point(soundBar->getPosition().x - soundBar->getContentSize().width/2 + soundBar->getContentSize().width*soundVolumn, soundBar->getPosition().y));
    
    sptMusicButton = Sprite::create("progressBarButton.png");
    this->addChild(sptMusicButton);
    sptMusicButton->setPosition(Point(musicBar->getPosition().x - musicBar->getContentSize().width/2 + musicBar->getContentSize().width*soundVolumn, musicBar->getPosition().y));
    
    sptButtonSizeButton = Sprite::create("progressBarButton.png");
    this->addChild(sptButtonSizeButton);
    sptButtonSizeButton->setPosition(Point(buttonSizeBar->getPosition().x - buttonSizeBar->getContentSize().width/2 + buttonSizeBar->getContentSize().width*soundVolumn, buttonSizeBar->getPosition().y));*/
    
    // for button size
    btnSizeBackground = Sprite::create("blackSquare.png");
    addChild(btnSizeBackground);
    btnSizeBackground->setScaleX(size.width/btnSizeBackground->getContentSize().width);
    btnSizeBackground->setScaleY((size.height*2/5)/btnSizeBackground->getContentSize().height);
    btnSizeBackground->setAnchorPoint(Point(0, 0));
    btnSizeBackground->setColor(Color3B(22, 22, 22));
    btnSizeBackground->setOpacity(150);
    btnSizeBackground->setVisible(false);
    
    Texture2D* leftRightTexture = Director::getInstance()->getTextureCache()->addImage("leftRight.png");
//    Texture2D* rightTexture = Director::getInstance()->getTextureCache()->addImage("rightPress.png");
//    Texture2D* leftTexture = Director::getInstance()->getTextureCache()->addImage("leftPress.png");
    Texture2D* aTexture = Director::getInstance()->getTextureCache()->addImage("btnA.png");
//    Texture2D* aDownTexture = Director::getInstance()->getTextureCache()->addImage("aButtonDown.png");
    Texture2D* bTexture = Director::getInstance()->getTextureCache()->addImage("btnB.png");
//    Texture2D* bDownTexture = Director::getInstance()->getTextureCache()->addImage("bButtonDown.png");
    
    btnLeftRight = LegendDaryButton::buttonWithTexture(leftRightTexture, BUTTON_LEFT_RIGHT);
    btnLeftRight->setPosition( Point(200, 100) );
    addChild(btnLeftRight);
    btnLeftRight->setVisible(false);
    
    btnA = LegendDaryButton::buttonWithTexture( aTexture, BUTTON_A );
    btnA->setPosition( Point(size.width - 320, 100) );
    addChild(btnA);
    btnA->setVisible(false);
    
    btnB = LegendDaryButton::buttonWithTexture( bTexture, BUTTON_B );
    btnB->setPosition( Point(size.width - 120, 100) );
    addChild(btnB);
    btnB->setVisible(false);
    
    btnConnect->setPosition(Point(settingLayer->getContentSize().width - 20, btnConnect->getContentSize().height/2 + 45));
    btnConnect->setAnchorPoint(Point(1, 0.5));
    
//    topBar->updateLabels();
    loginState = false;
    if (GameManager::getInstance()->isGoogleSigned) {
        loginState = true;
        changeLoginState(loginState);
    }

    if (true) {
        btnSave->setVisible(false);
        if (GameManager::getInstance()->market == MARKET_APPSTORE_FREE) {
            btnConnect->setVisible(false);
        }
        
        btnLike->setVisible(false);
        btnCoupon->setVisible(false);
        
        int marginX = 20;
        int lineMax = 7;
        int btnY = size.height/2 - 120;
        int gapY = 110;
        for (int i = 0; i < 12; i++) {
            Button* btnLang = Button::create("backButton.png");
            Sprite* sptFlag;
            if (i == 0) {
                sptFlag = Sprite::create("flag0.png");
                btnLang->setTag((int)LanguageType::KOREAN);
            }else if (i == 1) {
                sptFlag = Sprite::create("flag1.png");
                btnLang->setTag((int)LanguageType::ENGLISH);
            }else if (i == 2) {
                sptFlag = Sprite::create("flag4.png");
                btnLang->setTag((int)LanguageType::RUSSIAN);
            }else if (i == 3) {
                sptFlag = Sprite::create("flag8.png");
                btnLang->setTag((int)LanguageType::FRENCH);
            }else if (i == 4) {
                sptFlag = Sprite::create("flag9.png");
                btnLang->setTag((int)LanguageType::ITALIAN);
            }else if (i == 5) {
                sptFlag = Sprite::create("flag10.png");
                btnLang->setTag((int)LanguageType::GERMAN);
            }else if (i == 6) {
                sptFlag = Sprite::create("flag11.png");
                btnLang->setTag((int)LanguageType::JAPANESE);
            }else if (i == 7) {
                sptFlag = Sprite::create("flag5.png");
                btnLang->setTag((int)LanguageType::CHINESE);
            }else if (i == 8) {
                sptFlag = Sprite::create("flag3.png");
//                btnLang->setTag((int)LanguageType::CHINESE_TRADITIONAL);
            }else if (i == 9) {
                sptFlag = Sprite::create("flag2.png");
                btnLang->setTag((int)LanguageType::PORTUGUESE);
            }else if (i == 10) {
                sptFlag = Sprite::create("flag7.png");
//                btnLang->setTag((int)LanguageType::THAILAND);
            }else if (i == 11) {
                sptFlag = Sprite::create("flag6.png");
                btnLang->setTag((int)LanguageType::SPANISH);
            }
            sptFlag->setScale(1.3);
            btnLang->addChild(sptFlag);
            sptFlag->setPosition(Point(btnLang->getContentSize().width/2, btnLang->getContentSize().height/2 + 3));

            settingLayer->addChild(btnLang);
            btnLang->setPosition(Point(marginX + btnLang->getContentSize().width/2 + ((960 - marginX*2 - btnLang->getContentSize().width)/(lineMax - 1))*(i%lineMax), btnY - gapY*(i/lineMax)));
            btnLang->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onLanguage, this));
        }
    }else{

    }
    return true;
}
void SettingLayer::onLanguage(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    Button* btn = (Button*)pSender;
    UserDefault::getInstance()->setIntegerForKey(KEY_SELECTED_LANGUAGE, btn->getTag());
//    LanguageManager::getInstance()->setLanguage();
    GameManager::getInstance()->setPetInfos();
    Scene* scene = Scene::create();
    Title* title = Title::create();
    scene->addChild(title);
    Director::getInstance()->replaceScene(scene);
    
//    GameManager::getInstance()->settingLayer = SettingLayer::create();
//    title->addChild(GameManager::getInstance()->settingLayer, 200);
//    GameManager::getInstance()->page = PAGE_SETTING;
    
    
}

void SettingLayer::onCoupon(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
        MyMessageBox::getInstance()->showDialog(this->getParent(), "NOT AVAILABLE");
        return;
    }
//    NativeInterface::NativeInterface::showCoupon();
}
void SettingLayer::onSaveGame(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
        MyMessageBox::getInstance()->showDialog(this->getParent(), "NOT AVAILABLE");
        return;
    }
    MyMessageBox::getInstance()->showDialogWithCloseBox(this, CallFuncN::create(CC_CALLBACK_1(SettingLayer::onDataWarningClosed, this)), LanguageManager::getInstance()->getText("datawarning").c_str(), LanguageManager::getInstance()->getText("ok").c_str());
}
void SettingLayer::onDataWarningClosed(Ref* obj){
    if (this->getTag() == MESSAGE_BOX_RESULT_OK) {
        GameManager::getInstance()->saveGameData();
    }else{
        
    }
}
void SettingLayer::onLoadGame(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
//    NativeInterface::NativeInterface::loadGame();
}
void SettingLayer::onConnect(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
        MyMessageBox::getInstance()->showDialog(this->getParent(), "NOT AVAILABLE");
        return;
    }
    if(GameManager::getInstance()->isGoogleSigned){
//        NativeInterface::NativeInterface::googleSignOut();
    }else{
//        NativeInterface::NativeInterface::googleSignIn();
    }
}
void SettingLayer::onLike(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    Application::getInstance()->openURL("http://www.facebook.com/FifteenSixKorea");
}
void SettingLayer::onBack(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    back();
}
void SettingLayer::back(){
    this->removeFromParent();
    GameManager::getInstance()->saveCoin();
    GameManager::getInstance()->settingLayer = NULL;
    GameManager::getInstance()->page = PAGE_TITLE;
}
void SettingLayer::updateBarContent(){
    sptMusicBarContent->setScaleX(musicBar->getContentSize().width*musicBar->getPercent()/200);
    sptSoundBarContent->setScaleX(soundBar->getContentSize().width*soundBar->getPercent()/200);
    sptButtonSizeBarContent->setScaleX(buttonSizeBar->getContentSize().width*buttonSizeBar->getPercent()/200);
}
void SettingLayer::musicSliderEvent(Ref *pSender, Slider::EventType type)
{
    
    if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        Slider* slider = dynamic_cast<Slider*>(pSender);
        int percent = slider->getPercent();
        GameManager::getInstance()->setMusicVolumn(percent*0.01f);
        updateBarContent();
    }
}

void SettingLayer::soundSliderEvent(Ref *pSender, Slider::EventType type)
{
    CCLOG("soundSliderEvent");
    if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        Slider* slider = dynamic_cast<Slider*>(pSender);
        int percent = slider->getPercent();
        
        GameManager::getInstance()->setSoundVolumn(percent*0.02f);
        updateBarContent();
    }
}

void SettingLayer::buttonSizeSliderEvent(Ref *pSender, Slider::EventType type)
{
    CCLOG("soundSliderEvent");
    if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        Slider* slider = dynamic_cast<Slider*>(pSender);
        int percent = slider->getPercent();
        
        float siz = percent*0.01f;
        
        float scaleFact = 0.5f + siz;
        btnLeftRight->setScale(scaleFact);
        btnA->setScale(scaleFact);
        btnB->setScale(scaleFact);
        int y = 45;
        btnLeftRight->setPosition(Point(100 + 130*siz, y + 65*siz));
        btnA->setPosition(Point(size.width - 130 - siz*200, y + 65*siz));// + (1-siz)*72));
        btnB->setPosition(Point(size.width - 60 - siz*50, y + 65*siz));

        btnLeftRight->setOpacity(siz > 0?255:0);
        btnA->setOpacity(siz > 0?255:0);
        btnB->setOpacity(siz > 0?255:0);
        updateBarContent();
    }
    if(type == Slider::EventType::ON_PERCENTAGE_CHANGED){
        
    }
}

void SettingLayer::onMusicSliderTouch(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    GameManager::getInstance()->setMusicVolumn(musicBar->getPercent()*0.01f);
}
void SettingLayer::onSoundSliderTouch(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    GameManager::getInstance()->setSoundVolumn(soundBar->getPercent()*0.01f);
}
void SettingLayer::onButtonSizeTouch(Ref* pSender, ui::Widget::TouchEventType eEventType){
    switch (eEventType)
    {
        case Widget::TouchEventType::BEGAN:
            
            btnLeftRight->setVisible(true);
            btnA->setVisible(true);
            btnB->setVisible(true);
            btnSizeBackground->setVisible(true);
            break;
            
        case Widget::TouchEventType::MOVED:

            break;
            
        case Widget::TouchEventType::ENDED:
            btnLeftRight->setVisible(false);
            btnA->setVisible(false);
            btnB->setVisible(false);
            btnSizeBackground->setVisible(false);
            break;
            
        case Widget::TouchEventType::CANCELED:
            break;
            
        default:
            break;
    }
    
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    GameManager::getInstance()->setButtonSize(buttonSizeBar->getPercent()*0.01f);
}
void SettingLayer::onEnter(){
    Layer::onEnter();
}
void SettingLayer::onExit(){
    Layer::onExit();
}
void SettingLayer::changeLoginStateLater(float dt){
    if (loginState) {
        btnConnect->loadTextureNormal("signOut.png", Button::TextureResType::LOCAL);
//        btnSave->setVisible(true);
    }else{
        btnConnect->loadTextureNormal("signIn.png", Button::TextureResType::LOCAL);
//        btnSave->setVisible(false);
    }
}
void SettingLayer::changeLoginState(bool signIn){
    loginState = signIn;
    this->scheduleOnce(schedule_selector(SettingLayer::changeLoginStateLater), 0.1);
}
void SettingLayer::onKeyReleased( cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event *event )
{
    if (keycode == EventKeyboard::KeyCode::KEY_BACK &&
        GameManager::getInstance()->page == PAGE_SETTING) {
//        back();
        event->stopPropagation();
    }
}
