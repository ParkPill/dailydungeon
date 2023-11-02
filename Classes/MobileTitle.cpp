//
//  MobileTitle.cpp
//  LegendDaryDungeon
//
//  Created by PACKSUNG PILL on 03/02/2019.
//

#include "MobileTitle.h"
#include "GameManager.h"
#include "LanguageManager.h"
#include "ServerManager.h"
#include "GameSharing.h"
bool MobileTitle::init()
{
    Layer::init();
    size = Director::getInstance()->getWinSize();
    MOBILE_TITLE = this;
    
    Node* titleLayer = CSLoader::createNode("Title.csb");
    titleLayer->setPositionX(size.width/2 - titleLayer->getContentSize().width/2);
    this->addChild(titleLayer, 0);
    cocostudio::timeline::ActionTimeline* action = CSLoader::createTimeline("Title.csb");
    titleLayer->runAction(action);
    titleLayer->setVisible(true);
    action->gotoFrameAndPlay(0, true);
    
    Button* btn = (Button*)titleLayer->getChildByName("btnContinue");
    btn->addClickEventListener(CC_CALLBACK_0(MobileTitle::onTouchTitle, this));
    
    Widget* pnlTitle = (Widget*)titleLayer->getChildByName("pnlTitle");
    Text* lbl = (Text*)pnlTitle->getChildByName("lblPlay");
    lbl->setVisible(false);
    
    lblID = (Text*)pnlTitle->getChildByName("lblID");
    lblID->setString("Loading...");
    
    lbl = (Text*)pnlTitle->getChildByName("lblOption");
    lbl->setVisible(false);
    
    lbl = (Text*)pnlTitle->getChildByName("lblExit");
    lbl->setVisible(false);
    
    GM->playSoundEffect(SOUND_THUNDER);
    GM->playBGM(BGM_NOVEMBER);
    
    lblLoading = LM->getLocalizedLabel("Connecting to server", Color4B::WHITE);
    this->addChild(lblLoading, POPUP_ZORDER);
    lblLoading->setPosition(size.width/2, 100);
    
    this->schedule(schedule_selector(MobileTitle::updateTitle), 0.1f);
    
    btn = (Button*)titleLayer->getChildByName("btnRestart");
    btn->addClickEventListener(CC_CALLBACK_0(MobileTitle::onRestartAsNew, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "new account");
    
    lblWelcome = (Text*)titleLayer->getChildByName("lblWelcome");
    lblWelcome->setVisible(false);
    
    keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(MobileTitle::onKeyPressed, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);
//    UDSetStr(KEY_SAVED_ID,"-1"); // test 
    std::string id = UDGetStr(KEY_SAVED_ID,"-1");
    if(id.compare("-1") == 0){
        
    }else{
        SM->requestedID = id;
        SM->getAllUserData();
    }
    
    
    float blackTime = 1.5f;
    btn = Button::create("whiteRect.png");
    this->addChild(btn, POPUP_ZORDER);
    btn->setScale(size.width/btn->getContentSize().width, size.height/btn->getContentSize().height);
    btn->setPosition(size/2);
    btn->runAction(Sequence::create(DelayTime::create(blackTime), DelayTime::create(1.9f), FadeOut::create(0.5f), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, btn)), NULL));
    btn->setGlobalZOrder(1);
    Sprite* spt = Sprite::create("res/logo_moron.png");
    spt->setPosition(size/2);
    this->addChild(spt, POPUP_ZORDER);
    spt->setScale(0.7f);
    spt->setOpacity(0);
    spt->runAction(Sequence::create(DelayTime::create(blackTime), FadeIn::create(0.5f), DelayTime::create(1.4f), FadeOut::create(0.5f), SPT_REMOVE_FUNC, NULL));
    spt->setGlobalZOrder(2);
    
    btn = Button::create("whiteRect.png");
    this->addChild(btn, POPUP_ZORDER);
    btn->setScale(size.width/btn->getContentSize().width, size.height/btn->getContentSize().height);
    btn->setPosition(size/2);
    btn->setColor(Color3B::BLACK);
    btn->runAction(Sequence::create(DelayTime::create(blackTime), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, btn)), NULL));
    
    // init done
    
    return true;
}

void MobileTitle::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event){
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK){
        if (popupArray.size() == 0 || (popupArray.size() == 1 && popupArray.at(0)->getName().compare("userSelect") == 0)) {
            askExit();
        }else{
            closePopup();
        }
    }
}

void MobileTitle::askExit(){
    Node* layer = CSLoader::createNode("Exit.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    Node* background = layer->getChildByName("imgBackground");
    setTitle(layer->getChildByName("titleBack"), "exit");
    Text* lbl = (Text*)background->getChildByName("lbl");
    LM->setLocalizedString(lbl, "exit game");
    
    Button* btn = (Button*)background->getChildByName("btnNo");
    btn->addClickEventListener(CC_CALLBACK_0(MobileTitle::closePopup, this));
    btn = (Button*)background->getChildByName("btnYes");
    btn->addClickEventListener(CC_CALLBACK_0(MobileTitle::onExitYesClick, this));
}
void MobileTitle::onExitYesClick(){
    GM->exitGame();
}
void MobileTitle::onRestartAsNew(){
    UDSetStr(KEY_SAVED_ID,"-1");
    UDSetStr(KEY_RID,"");
    UDSetInt(KEY_AVATAR, 0);
    for (int i = 0; i < 4; i++) {
        UDSetInt(strmake(KEY_SHIELD_SLOT_FORMAT, i).c_str(), -1);
    }
    for (int i = 0; i < 4; i++) {
        UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, i).c_str(), -1);
    }
    restartTheGame();
}
void MobileTitle::onTouchTitle(){
    if(nameHandleState == NETWORK_HANDLE_STATE_COMPLETE){
        nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
        MOBILE_TITLE = nullptr;
//        Scene* scene = Scene::create();
//        scene->addChild();
        removeListener();
        Director::getInstance()->replaceScene(TransitionFade::create(1, HelloWorld::scene(STAGE_LOBBY, false)));
    }
}
void MobileTitle::restartTheGame(){
    removeListener();
    Scene* scene = Scene::create();
    scene->addChild(MobileTitle::create());
    Director::getInstance()->replaceScene(scene);
}
void MobileTitle::removeListener(){
    if(keyListener != nullptr){
        Director::getInstance()->getEventDispatcher()->removeEventListener(keyListener);
    }
}
void MobileTitle::updateTitle(float dt){
//    if (GM->iapFlag == IAP_FLAG_INIT) {
//        lblLoading->setString("Loading products...");
//        return;
//    }else if (GM->iapFlag == IAP_FLAG_INIT) {
//        lblLoading->setString("Setting products...");
////        GM->priceList.push_back(GameSharing::getIAPPrice("starter0"));// IAP_STARTER_0
////        GM->priceList.push_back(GameSharing::getIAPPrice("starter1"));// IAP_STARTER_1
////        GM->priceList.push_back(GameSharing::getIAPPrice("starter2"));// IAP_STARTER_2
////        GM->priceList.push_back(GameSharing::getIAPPrice("package0"));// IAP_PACKAGE_0
////        GM->priceList.push_back(GameSharing::getIAPPrice("package1"));// IAP_PACKAGE_1
////        GM->priceList.push_back(GameSharing::getIAPPrice("gem0"));// IAP_GEM_0
////        GM->priceList.push_back(GameSharing::getIAPPrice("gem1"));// IAP_GEM_1
////        GM->priceList.push_back(GameSharing::getIAPPrice("gem2"));// IAP_GEM_2
////        GM->priceList.push_back(GameSharing::getIAPPrice("gem3"));// IAP_GEM_3
////        GM->priceList.push_back(GameSharing::getIAPPrice("gem4"));// IAP_GEM_4
////        GM->priceList.push_back(GameSharing::getIAPPrice("ticket0"));// IAP_TICKET_0
////        GM->priceList.push_back(GameSharing::getIAPPrice("ticket1"));// IAP_TICKET_1
////        GM->priceList.push_back(GameSharing::getIAPPrice("ticket2"));// IAP_TICKET_2
////        GM->priceList.push_back(GameSharing::getIAPPrice("starter1original"));// IAP_STARTER_1_ORIGINAL 12
////        GM->priceList.push_back(GameSharing::getIAPPrice("starter2original"));// IAP_STARTER_2_ORIGINAL 13
////        GM->priceList.push_back(GameSharing::getIAPPrice("package0original"));// IAP_PACKAGE_0_ORIGINAL 14
////        GM->priceList.push_back(GameSharing::getIAPPrice("package1original"));// IAP_PACKAGE_1_ORIGINAL 15
////        GM->priceList.push_back(GameSharing::getIAPPrice("gem0original"));// IAP_GEM_0_ORIGINAL 16
////        GM->priceList.push_back(GameSharing::getIAPPrice("ticket0original"));// IAP_TICKET_0_ORIGINAL 17
//
//        GM->iapFlag = IAP_FLAG_INIT;
//        return;
//    }
    if(SM->isServerFailed){
        if(!isRestartingReserved){
            isRestartingReserved = true;
            showInstanceMessage(LM->getText("network fail"));
            this->runAction(Sequence::create(DelayTime::create(3), CallFunc::create(CC_CALLBACK_0(MobileTitle::restartTheGame, this)), NULL));
        }
        return;
    }
    if(nameHandleState == NETWORK_HANDLE_STATE_NOT_READY){
        lblLoading->setString("Checking id...");
        std::string id = UDGetStr(KEY_SAVED_ID,"-1");
        SM->requestedID = id;
        if(id.compare("-1") == 0){
            nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
            hideIndicator();
            showUserSelect();
        }else{
            nameHandleState = NETWORK_HANDLE_STATE_HANDLED;
            lblID->setString(strmake("ID: %s", id.c_str()));
//            LM->setLocalizedStringNotKey(lblWelcome, strmake(LM->getText("welcome remember password").c_str(), UDGetStr(KEY_NAME).c_str()));
            lblWelcome->setString(strmake(LM->getText("welcome remember password").c_str(), UDGetStr(KEY_NAME).c_str()));
            log("name: %s", UDGetStr(KEY_NAME).c_str());
        }
    }
    if(nameHandleState == NETWORK_HANDLE_STATE_ARRIVED){
        hideIndicator();
        if(isSendingPassword){
            nameHandleState = NETWORK_HANDLE_STATE_HANDLED;
        }else{
            
            if(SM->resultMessage.compare("same name exist") == 0){
                showInstanceMessage(LM->getText("same name exist"));
                nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
            }else if(SM->resultMessage.compare("save failed") == 0){
                showInstanceMessage(LM->getText("network retry"));
                nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
            }else if(SM->resultMessage.compare("database failure") == 0){
                showInstanceMessage(LM->getText("network retry"));
                nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
            }else{
                isSendingPassword = true;
                nameHandleState = NETWORK_HANDLE_STATE_REQUESTED;
                showIndicator();
                TextField* tfPassword = (TextField*)this->getChildByName("namePopup")->getChildByName("imgBackground")->getChildByName("tfPassword");
                SM->saveUserData(strmake("password=%s", tfPassword->getString().c_str()));
            }
        }
    }
    if (SM->isFailedToGetNetworkData){
        SM->isFailedToGetNetworkData = false;
    }
    if (GM->versionCode < GM->receivedVersionCode) {
        Node* layer = CSLoader::createNode("UnderConstruction.csb");
        this->addChild(layer, POPUP_ZORDER);
        layer->setGlobalZOrder(100);
        Node* background = layer->getChildByName("imgBackground");
        setTitle(layer->getChildByName("titleBack"), "update");
        Text* lbl = (Text*)background->getChildByName("lblDescription");
        LM->setLocalizedString(lbl, "update required");
        Button* btn = (Button*)background->getChildByName("btnOk");
        btn->addClickEventListener(CC_CALLBACK_0(MobileTitle::onUpdateClick, this));
        this->unschedule(schedule_selector(MobileTitle::updateTitle));
    }
    if (GM->serverMaintenance > 0) {
        Node* layer = CSLoader::createNode("UnderConstruction.csb");
        this->addChild(layer, POPUP_ZORDER);
        layer->setGlobalZOrder(100);
        Node* background = layer->getChildByName("imgBackground");
        setTitle(layer->getChildByName("titleBack"), "server maintenance");
        Text* lbl = (Text*)background->getChildByName("lblDescription");
        std::string str = strmake(LM->getText("server maintenance description format").c_str(), GM->serverMaintenance);
        LM->setLocalizedStringNotKey(lbl, str);
        Button* btn = (Button*)background->getChildByName("btnOk");
        btn->setVisible(false);
        this->unschedule(schedule_selector(MobileTitle::updateTitle));
    }
    if (nameHandleState == NETWORK_HANDLE_STATE_HANDLED) {
        nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
        
        std::string id = UDGetStr(KEY_SAVED_ID,"-1");
        lblID->setString(strmake("ID: %s", id.c_str()));
        
        LM->setLocalizedString(lblLoading, "touch to start");
        lblLoading->runAction(RepeatForever::create(Sequence::create(FadeOut::create(0.5f), FadeIn::create(0.5f), DelayTime::create(0.5f), NULL)));
        if(nameToRegister.length() > 0){
            UDSetStr(KEY_NAME, nameToRegister);
        }
        LM->setLocalizedStringNotKey(lblWelcome, strmake(LM->getText("welcome remember password").c_str(), UDGetStr(KEY_NAME).c_str()));
        
        lblWelcome->setVisible(true);
        closePopup();
        closePopup();
        
        if(id.compare("-1") != 0){
            SM->requestedID = id;
            SM->getAllUserData();
        }
    }
    
    if (changeAccountResult == 0) {
        changeAccountResult = -1;
        showInstanceMessage(LM->getText("login failed"));
    }else if (changeAccountResult == 1){
        changeAccountResult = -1;
        restartTheGame();
    }
}
void MobileTitle::onUpdateClick(){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    Application::getInstance()->openURL("itms-apps://itunes.apple.com/app/id1389133177?action=write-review");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    Application::getInstance()->openURL("https://play.google.com/store/apps/details?id=kr.co.morongames.dailydungeon");
#endif

}
void MobileTitle::showRegisterName(){
    Node* layer = CSLoader::createNode("RegisterName.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setName("namePopup");
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    lbl = (Text*)background->getChildByName("lblTitle");
    
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(MobileTitle::closePopup, this));
    
    setTitle(layer->getChildByName("titleBack"), "name");
    lbl = (Text*)layer->getChildByName("lblAlert");
    LM->setLocalizedString(lbl, "change name alert");
    lbl = (Text*)background->getChildByName("lblName");
    LM->setLocalizedString(lbl, "name");
    lbl = (Text*)background->getChildByName("lblPassword");
    LM->setLocalizedString(lbl, "password");
    lbl = (Text*)background->getChildByName("lblConfirm");
    LM->setLocalizedString(lbl, "confirm password");
    
    TextField* tf = (TextField*)background->getChildByName("tfName");
    tf->setPlaceHolder(LM->getText("name"));
    tf = (TextField*)background->getChildByName("tfPassword");
    tf->setPlaceHolder(LM->getText("password"));
    tf = (TextField*)background->getChildByName("tfConfirm");
    tf->setPlaceHolder(LM->getText("password"));
    
    btn = (Button*)background->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_1(MobileTitle::onOkNameClick, this));
}
void MobileTitle::onOkNameClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    TextField* tf = (TextField*)this->getChildByName("namePopup")->getChildByName("imgBackground")->getChildByName("tfName");
    TextField* tfPassword = (TextField*)this->getChildByName("namePopup")->getChildByName("imgBackground")->getChildByName("tfPassword");
    TextField* tfConfirm = (TextField*)this->getChildByName("namePopup")->getChildByName("imgBackground")->getChildByName("tfConfirm");
    if(tf->getString().size() <= 0){
        showInstanceMessage(LM->getText("input name"));
    }else if(tfPassword->getString().compare(tfConfirm->getString()) != 0){
        showInstanceMessage(LM->getText("password not match"));
    }else{
        nameToRegister = tf->getString();
        nameHandleState = NETWORK_HANDLE_STATE_REQUESTED;
        showIndicator();
        SM->registerName(tf->getString());
    }
}
void MobileTitle::showUserSelect(){
    Node* layer = CSLoader::createNode("UserSelect.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setName("userSelect");
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    lbl = (Text*)background->getChildByName("lblTitle");
    
    setTitle(layer->getChildByName("titleBack"), "new user");
    
    lbl = (Text*)background->getChildByName("lblDesc");
    LM->setLocalizedString(lbl, "is new user");
    
    btn = (Button*)background->getChildByName("btnNew");
    btn->addClickEventListener(CC_CALLBACK_1(MobileTitle::onNewUserSelect, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "new user");
    btn = (Button*)background->getChildByName("btnExist");
    btn->addClickEventListener(CC_CALLBACK_1(MobileTitle::onOldUserSelect, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "existing user");
}
void MobileTitle::onNewUserSelect(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    showRegisterName();
}
void MobileTitle::onOldUserSelect(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    showInputUserIDAndName();
}
void MobileTitle::showInputUserIDAndName(){
    Node* layer = CSLoader::createNode("ExistNameAndID.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setName("accountChange");
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    lbl = (Text*)background->getChildByName("lblTitle");
    setTitle(layer->getChildByName("titleBack"), "existing user");
    
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(MobileTitle::closePopup, this));
    
    lbl = (Text*)background->getChildByName("lblName");
    LM->setLocalizedString(lbl, "name");
    
    lbl = (Text*)background->getChildByName("lblPassword");
    LM->setLocalizedString(lbl, "password");
    
    TextField* tfName = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfName");
    tfName->setPlaceHolder(LM->getText("name"));
    TextField* tfPassword = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfPassword");
    tfPassword->setPlaceHolder(LM->getText("password"));
    
    btn = (Button*)background->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_1(MobileTitle::onCheckExistingUser, this));
}
void MobileTitle::onCheckExistingUser(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    Node* layer = this->getChildByName("accountChange");
    TextField* tfName = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfName");
    TextField* tfPassword = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfPassword");
    SM->changeAccount(tfName->getString(), tfPassword->getString());
}

void MobileTitle::showInstanceMessage(std::string msg, float stayTime){
    this->removeChildByName("instanceMessage");
    Sprite* spt = Sprite::create("alertBar.png");
    spt->setScale(4);
    this->addChild(spt, POPUP_ZORDER + 1);
    spt->setGlobalZOrder(1);
    Size cSize = spt->getContentSize();
    spt->setPosition(Point(size.width/2, size.height - 714 - spt->getContentSize().height/2 + 50));
    spt->setName("instanceMessage");
    Label* lbl = LM->getLocalizedLabel(msg.c_str(), Color4B::WHITE);
    spt->addChild(lbl, POPUP_ZORDER + 1);
    lbl->setGlobalZOrder(1);
    //    lbl->setScale(0.4);
    lbl->enableBold();
    lbl->setScale(lbl->getScale()*0.25f);
    lbl->setAnchorPoint(Point(0.5, 0));
    lbl->enableOutline(Color4B::BLACK, 10);
    lbl->setWidth(size.width/lbl->getScaleX());
    lbl->setPosition(Vec2(spt->getContentSize().width/2, spt->getContentSize().height/2 - 7));
    lbl->setDimensions((size.width - 100)/lbl->getScale(), 300);
    lbl->setOverflow(Label::Overflow::SHRINK);
    lbl->setHorizontalAlignment(TextHAlignment::CENTER);
    lbl->setVerticalAlignment(TextVAlignment::BOTTOM);
    float dur = 0.3f;
    float distanceToMove = 80;
    spt->runAction(Sequence::create(MoveBy::create(dur, Point(0, distanceToMove)), DelayTime::create(stayTime), MoveBy::create(dur, Point(0, -distanceToMove)), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, spt)), nullptr));
}
void MobileTitle::closePopup(){
    if(popupArray.size() > 0){
        Node* popup = popupArray.at(popupArray.size() - 1);
        popupArray.eraseObject(popup);
        popup->removeFromParent();
    }
}
// set as popup should be set after currentSelector & currentSelectedItem
void MobileTitle::setAsPopup(Node* node){
    GM->showPopup(node);
    popupArray.pushBack(node);
    node->setLocalZOrder(POPUP_ZORDER + (int)popupArray.size());
}
void MobileTitle::setTitle(Node* titleBack, std::string menuKey){
    Text* lbl = (Text*)titleBack->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, menuKey);
    ImageView* img = (ImageView*)titleBack;
    img->setContentSize(Size(lbl->getContentSize().width*lbl->getScaleX() + 25, 15));
    img->setScale9Enabled(true);
    lbl->setPositionX(titleBack->getContentSize().width/2);
}
void MobileTitle::showIndicator(){
    log("show Indicator!");
    Layer* layer = Layer::create();
    this->addChild(layer, 10000);
    setAsPopup(layer);
    indicator = layer;
    Button* btn = Button::create("btnBase.png");
    btn->setScale(100);
    btn->setOpacity(150);
    btn->setColor(Color3B::BLACK);
    layer->addChild(btn);
    btn->setPosition(size/2);
    
    Sprite* spt = Sprite::create("sandglass.png");
    layer->addChild(spt);
    spt->setPosition(Point(size.width/2, size.height/2 + 40));
    //    spt->setScale(2);
    spt->runAction(RepeatForever::create(RotateBy::create(1, 90)));
    
    GM->makeItSiluk(spt);
    
    Label* lbl = LM->getLocalizedLabel(LM->getText("please wait").c_str(), Color4B::WHITE);
    layer->addChild(lbl);
    lbl->setPosition(Point(size.width/2, size.height/2 - 35));
    lbl->enableOutline(Color4B::BLACK, 5);
    lbl->setScale(0.7);
    
    this->scheduleOnce(schedule_selector(MobileTitle::indicatorTimeOver), 30);
}
void MobileTitle::indicatorTimeOver(float dt){
    showInstanceMessage(LM->getText("time over"));
    closePopup();
}
void MobileTitle::hideIndicator(){
    this->unschedule(schedule_selector(MobileTitle::indicatorTimeOver));
    closePopup();
    indicator = nullptr;
}
