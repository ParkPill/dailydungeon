//
//  Title.cpp
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 14. 9. 14..
//
//

#include "Title.h"
#include "GameManager.h"
#include "MyMessageBox.h"
#include "LanguageManager.h"
//#include "Episode.h"
#include "Hero.h"
bool Title::init()
{
    GameManager::getInstance()->setCoin(0);
    GameManager::getInstance()->page = PAGE_TITLE;
    Layer::init();
    size = Director::getInstance()->getWinSize();
    
    titleLayer = CSLoader::createNode("Title.csb");
    titleLayer->setPositionX(size.width/2 - titleLayer->getContentSize().width/2);
    this->addChild(titleLayer, 2003);
    cocostudio::timeline::ActionTimeline* action = CSLoader::createTimeline("Title.csb");
    titleLayer->runAction(action);
    titleLayer->setVisible(true);
    action->gotoFrameAndPlay(0, true);
    
    pnlTitle = titleLayer->getChildByName("pnlTitle");
    pnlTitle->setVisible(true);
    
    pnlOption = titleLayer->getChildByName("pnlOption");
    pnlOption->setVisible(false);
    
    pnlAchievement = titleLayer->getChildByName("pnlAchievement");
    pnlAchievement->setVisible(false);
    
    lblPlay = (Text*)pnlTitle->getChildByName("lblPlay");
    lblPlay->addClickEventListener(CC_CALLBACK_0(Title::onPlayClick, this));
    currentNode = lblPlay;
    
    lblOption = (Text*)pnlTitle->getChildByName("lblOption");
    lblOption->addClickEventListener(CC_CALLBACK_0(Title::onOptionClick, this));
    
    lblExit = (Text*)pnlTitle->getChildByName("lblExit");
    lblExit->addClickEventListener(CC_CALLBACK_0(Title::onExitClick, this));
    
    lblBackFromOption = (Text*)pnlOption->getChildByName("lblBack");
    lblBackFromOption->addClickEventListener(CC_CALLBACK_0(Title::onBackFromOptionClick, this));
    lblAchievement = (Text*)pnlOption->getChildByName("lblAchievement");
    lblAchievement->addClickEventListener(CC_CALLBACK_0(Title::onAchievementClick, this));
    lblKeySetting = (Text*)pnlOption->getChildByName("lblKeySetting");
    lblKeySetting->addClickEventListener(CC_CALLBACK_0(Title::onKeySettingClick, this));
    lblOnline = (Text*)pnlOption->getChildByName("lblOnline");
    lblOnline->addClickEventListener(CC_CALLBACK_0(Title::onOnlineClick, this));
    lblSound = (Text*)pnlOption->getChildByName("lblSound");
    lblSound->addClickEventListener(CC_CALLBACK_0(Title::onSoundClick, this));
    lblSound->setString(__String::createWithFormat("Sound: %s", GameManager::getInstance()->getSoundVolumn() == 0?"Off":"On")->getCString());
    lblMusic = (Text*)pnlOption->getChildByName("lblMusic");
    lblMusic->addClickEventListener(CC_CALLBACK_0(Title::onMusicClick, this));
    lblMusic->setString(__String::createWithFormat("Music: %s", GameManager::getInstance()->getMusicVolumn() == 0?"Off":"On")->getCString());
    
    lblBackFromAchievement = (Text*)pnlAchievement->getChildByName("lblBack");
    lblBackFromAchievement->addClickEventListener(CC_CALLBACK_0(Title::onBackFromAchievementClick, this));
    
    pnlKeySetting = CSLoader::createNode("KeySetting.csb");
    pnlKeySetting->setPositionX(size.width/2 - titleLayer->getContentSize().width/2);
    this->addChild(pnlKeySetting, 2003);
    pnlKeySetting->setVisible(false);
    
    lblBackFromKeySetting = (Text*)pnlKeySetting->getChildByName("lblBack");
    lblBackFromKeySetting->addClickEventListener(CC_CALLBACK_0(Title::onBackFromKeySettingClick, this));
    GameManager::getInstance()->playSoundEffect(SOUND_THUNDER);
    GameManager::getInstance()->playBGM(BGM_NOVEMBER);
    addListener();

    cursor = Sprite::create("targetWhite.png");
    titleLayer->addChild(cursor, 999);
    cursor->runAction(RepeatForever::create(RotateBy::create(1, 100)));
    
    Sprite* innerCursor = Sprite::create("targetWhite.png");
    cursor->addChild(innerCursor, 999);
    innerCursor->runAction(RepeatForever::create(RotateBy::create(1, -160)));
    innerCursor->setScale(0.7);
    innerCursor->setPosition(Point(cursor->getContentSize().width/2, cursor->getContentSize().height/2));
    
    updateCursor();
    
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    lblExit->setVisible(false);
//    lblPlay->setVisible(false);
//    lblOption->setVisible(false);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    lblExit->setVisible(false);
//    lblPlay->setVisible(false);
//    lblOption->setVisible(false);
    
#else
    cursorLayer = CursorLayer::create();
    this->addChild(cursorLayer, 9999);
    this->schedule(schedule_selector(Title::monitoringCursorLayer));
#endif
    
    // test
//    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 0)->getCString(), true);
//    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 1)->getCString(), true);
//    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 2)->getCString(), true);
//    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString(), false);
    //    GameManager::getInstance()->isGameClear = true;
    
    // init done
    CCLOG("** title done");
    return true;
}
void Title::monitoringCursorLayer(float dt){
    if(cursorLayer->isMouseLeftDown == true){
        onExecuted();
        cursorLayer->isMouseLeftDown = false;
    }
    Point pos = cursorLayer->cursor->getPosition() - titleLayer->getPosition();
    Node* hoverNode = GameManager::getInstance()->getNodeAtThisPoint(pos, currentNode);
    if (hoverNode != nullptr && hoverNode != currentNode) {
        GameManager::getInstance()->playSoundEffect(SOUND_FA);
        currentNode = hoverNode;
        updateCursor();
    }
}
void Title::onKeySettingClick(){
    pnlOption->setVisible(false);
    pnlKeySetting->setVisible(true);
    currentNode = lblBackFromKeySetting;
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
    updateCursor();
}
void Title::onOnlineClick(){
    bool onOff = GameManager::getInstance()->isOnlineEnabled;
    onOff = !onOff;
    lblOnline->setString(__String::createWithFormat("Online: %s", onOff?"On":"Off")->getCString());
    GameManager::getInstance()->isOnlineEnabled = onOff;
    GameManager::getInstance()->playSoundEffect(SOUND_FA);
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
}
void Title::addListener(){
    keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(Title::onKeyPressed, this);
//    keyListener->onKeyReleased = CC_CALLBACK_2(HudLayer::onKeyReleased, this);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);
    log("addListener");
//    touchListener = EventListenerTouchAllAtOnce::create();
//    touchListener->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesBegan");
//        auto last = touches.back();
//        Point pos = touches.at(0)->getLocation();
//        if (lblPlay->getBoundingBox().containsPoint(pos)) {
//            log("sdf");
//        }else{
//            log("x: %f, y: %f", pos.x, pos.y);
//        }
//        //        chopad->TouchesBegan(touches, evt);
//        // check if last touch point is in which button
//    };
//    touchListener->onTouchesCancelled = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesCancelled");
//        //        chopad->TouchesCancelled(touches, evt);
//    };
//    touchListener->onTouchesEnded = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesEnded");
//        //        chopad->TouchesEnded(touches, evt);
//    };
//    touchListener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesMoved");
//        //        chopad->TouchesMoved(touches, evt);
//    };
//    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    /*
    //create an evnetListenerController
    EventListenerController* _listener = EventListenerController::create();
    
    //bind onConneected event call function
    _listener->onConnected = CC_CALLBACK_2(Title::onConnectController,this);
    
    //bind disconnect event call function
    _listener->onDisconnected = CC_CALLBACK_2(Title::onDisconnectedController,this);
    
    //bind onKeyDown event call function
    _listener->onKeyDown = CC_CALLBACK_3(Title::onKeyDown, this);
    
    //bind onKeyUp event call function
    _listener->onKeyUp = CC_CALLBACK_3(Title::onKeyUp, this);
    
    //bind onAxis event call function, onAxis will be called when analog stick is changed
    _listener->onAxisEvent = CC_CALLBACK_3(Title::onAxisEvent, this);
    
    //Activate the listener into the event dispatcher
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
    
    //This function should be called for iOS platform
    Controller::startDiscoveryController();
 */
}
/*
// controller methods
//Controller is the obejects of the Controller，keyCode means the keycode of the controller you click down
void Title::onKeyDown(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event)
{
    CCLOG("KeyDown:%d", keyCode);
    
}

void Title::onKeyUp(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event)
{
    //You can get the controller by tag, deviceId or devicename if there are multiple controllers
    CCLOG("tag:%d DeviceId:%d DeviceName:%s", controller->getTag(), controller->getDeviceId(), controller->getDeviceName().c_str());
    CCLOG("KeyUp:%d", keyCode);
}

//The axis includes X-axis and Y-axis and its range is from -1 to 1. X-axis is start from left to right and Y-axis is bottom to top.
void Title::onAxisEvent(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event)
{
    const auto& keyStatus = controller->getKeyStatus(keyCode);
    CCLOG("Axis KeyCode:%d Axis Value:%f", keyCode, keyStatus.value);
}

void Title::onConnectController(Controller* controller, Event* event)
{
    CCLOG("Game controller connected");
}

void Title::onDisconnectedController(Controller* controller, Event* event)
{
    CCLOG("Game controller disconnected");
}

// end controller methods
*/
void Title::removeListener(){
    log("removeListener");
    Director::getInstance()->getEventDispatcher()->removeEventListener(keyListener);
//    Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
    this->unschedule(schedule_selector(Title::monitoringCursorLayer));
}

void Title::updateCursor(){
    cursor->setPosition(currentNode->getPosition() + Point(-10 - cursor->getContentSize().width/2, 0));
}
void Title::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    Node* selectedNode = nullptr;
    if(keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW){
        selectedNode = GameManager::getInstance()->findDown(currentNode);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW){
        selectedNode = GameManager::getInstance()->findUp(currentNode);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ALT){
        selectedNode = GameManager::getInstance()->findLeft(currentNode);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ALT){
        selectedNode = GameManager::getInstance()->findRight(currentNode);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_ENTER){
        onExecuted();
    }
    if (selectedNode != nullptr) {
        GameManager::getInstance()->playSoundEffect(SOUND_FA);
        currentNode = selectedNode;
        updateCursor();
    }
}

void Title::onExecuted()
{
    bool actionTaken = true;
    Point lastPos = cursor->getPosition();
    if (currentNode == lblPlay) {
        onPlayClick();
        Sprite* sptHole = Sprite::createWithSpriteFrameName("bulletHole.png");
        titleLayer->addChild(sptHole);
        sptHole->setPosition(lastPos + Point(rand()%10 - 5, rand()%10 - 5));
        sptHole->runAction(Sequence::create(DelayTime::create(0.4), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptHole)), NULL));
        cursor->setVisible(false);
        removeListener();
    }else if (currentNode == lblOption) {
        onOptionClick();
    }else if (currentNode == lblExit) {
        onExitClick();
    }else if (currentNode == lblBackFromOption) {
        onBackFromOptionClick();
    }else if (currentNode == lblAchievement) {
        onAchievementClick();
    }else if (currentNode == lblBackFromAchievement) {
        onBackFromAchievementClick();
    }else if (currentNode == lblKeySetting) {
        onKeySettingClick();
    }else if (currentNode == lblBackFromKeySetting) {
        onBackFromKeySettingClick();
    }else if (currentNode == lblOnline) {
        onOnlineClick();
    }else if (currentNode == lblSound) {
        onSoundClick();
    }else if (currentNode == lblMusic) {
        onMusicClick();
    }else{
        actionTaken = false;
    }
    if (actionTaken) {
        
        this->runAction(Sequence::create(MoveBy::create(0.05, Point(2, 2)), MoveBy::create(0.05, Point(-4, -4)), MoveTo::create(0.05, Point::ZERO), NULL));
    }
}
void Title::onPlayClick(){
    currentNode = lblPlay;
    updateCursor();
    Point lastPos = cursor->getPosition();
    for (int i = 0; i < 4; i++) {
        Sprite* sptSmoke = Sprite::createWithSpriteFrameName("smoke.png");
        titleLayer->addChild(sptSmoke);
        sptSmoke->setPosition(lastPos + Point(rand()%10 - 5, rand()%10 - 5));
        sptSmoke->setScale(2);
        float radius = 50;
        int angle = i*90 + 50;
        float x = cos(angle*3.14/180)*radius;
        float y = -sin(angle*3.14/180)*radius;
        sptSmoke->runAction(EaseOut::create(MoveBy::create(0.3, Point(x, y)), 5));
        sptSmoke->runAction(EaseOut::create(ScaleTo::create(0.3, 0.2), 3));
        sptSmoke->runAction(RotateBy::create(1, 100 + rand()%200));
        sptSmoke->runAction(Sequence::create(DelayTime::create(0.4), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptSmoke)), NULL));
    }
    
    lblPlay->runAction(RepeatForever::create(Sequence::create(FadeOut::create(0.05), DelayTime::create(0.05), FadeIn::create(0.05), DelayTime::create(0.05), NULL)));
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
    this->scheduleOnce(schedule_selector(Title::goToStageLater), 7);
    
    bonusWeaponLayer = CSLoader::createNode("StartWeapon.csb");
    bonusWeaponLayer->setPositionX(size.width/2 - bonusWeaponLayer->getContentSize().width/2);
    this->addChild(bonusWeaponLayer, 3004);
    bonusWeaponLayer->setOpacity(0);
    bonusWeaponLayer->runAction(Sequence::create(DelayTime::create(2), FadeIn::create(0), NULL));
    
    for (int i = 0; i < 3; i++) {
        Droppable* suitcase = Droppable::createDroppable(UNIT_SUITCASE, 1, 0, __String::createWithFormat("suitCase%d_0.png", i)->getCString());
        bonusWeaponLayer->addChild(suitcase);
        suitcase->setScale(4);
        suitcase->secondTag = i + 1;
        suitcase->setPosition(bonusWeaponLayer->getChildByName(StringUtils::format("imgSlot%d", i))->getPosition() + Point(0, -35));
        suitcase->setName(StringUtils::format("suitcase%d", i));
        ((Text*)bonusWeaponLayer->getChildByName(StringUtils::format("lblWeaponName%d", i)))->setVisible(false);
    }
    
    this->schedule(schedule_selector(Title::updateBonusWeaponLayer), 1, 5, 2);
}
void Title::onOptionClick(){
    pnlTitle->setVisible(false);
    pnlOption->setVisible(true);
    currentNode = lblBackFromOption;
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
    updateCursor();
}
void Title::onExitClick(){
    GameManager::getInstance()->exitGame();
}
void Title::onBackFromOptionClick(){
    pnlTitle->setVisible(true);
    pnlOption->setVisible(false);
    currentNode = lblOption;
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
    updateCursor();
}
void Title::onBackFromKeySettingClick(){
    pnlOption->setVisible(true);
    pnlKeySetting->setVisible(false);
    currentNode = lblKeySetting;
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
    updateCursor();
}
void Title::onSoundClick(){
    bool onOff = GameManager::getInstance()->getSoundVolumn() == 1;
    onOff = !onOff;
    lblSound->setString(__String::createWithFormat("Sound: %s", onOff?"On":"OFf")->getCString());
    GameManager::getInstance()->setSoundVolumn(onOff?1:0);
    GameManager::getInstance()->playSoundEffect(SOUND_FA);
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
}
void Title::onMusicClick(){
    bool onOff = GameManager::getInstance()->getMusicVolumn() == 1;
    onOff = !onOff;
    lblMusic->setString(__String::createWithFormat("Music: %s", onOff?"On":"Off")->getCString());
    GameManager::getInstance()->setMusicVolumn(onOff?1:0);
    GameManager::getInstance()->playSoundEffect(SOUND_FA);
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
}
void Title::onAchievementClick(){
    pnlOption->setVisible(false);
    pnlAchievement->setVisible(true);
    currentNode = lblBackFromAchievement;
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
    updateCursor();
}
void Title::onNativeAchievementClick(){
    pnlOption->setVisible(true);
    pnlAchievement->setVisible(false);
    currentNode = lblAchievement;
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
    updateCursor();
}
void Title::onBackFromAchievementClick(){
    pnlOption->setVisible(true);
    pnlAchievement->setVisible(false);
    currentNode = lblAchievement;
    GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
    updateCursor();
}
void Title::goToStageLater(float dt){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    
#else
    cursorLayer->Dispose();
#endif

//    Scene* scene = Scene::create();
//    scene->addChild();
    Director::getInstance()->replaceScene(HelloWorld::scene(STAGE_LOBBY, false));
}
Sprite* Title::getLightSpin(float persistTime){
    Sprite* shining = Sprite::create("lightSpin.png");
    shining->runAction(RotateBy::create(persistTime, persistTime*90));
    shining->runAction(Sequence::create(DelayTime::create(persistTime), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, shining)), NULL));
    BlendFunc f = {GL_DST_COLOR, GL_DST_ALPHA};
    shining->setBlendFunc(f);
    
    Sprite* shining2 = Sprite::create("lightSpin.png");
    shining2->runAction(RotateBy::create(persistTime, -persistTime*180));
    shining2->runAction(Sequence::create(DelayTime::create(persistTime), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, shining)), NULL));
    f = {GL_DST_COLOR, GL_DST_ALPHA};
    shining2->setBlendFunc(f);
    shining->addChild(shining2);
    shining2->setPosition(Point(shining->getContentSize().width/2, shining->getContentSize().height/2));
    
    return shining;
}
void Title::updateBonusWeaponLayer(float dt){
    countDown--;
    Droppable* suitcase;
    bool weaponUnlocked = false;
    if (countDown == 4 ||countDown == 3 ||countDown == 2) {
        weaponUnlocked = true;
    }
    
    if (weaponUnlocked) {
        int index = 4 - countDown;
        suitcase = (Droppable*)bonusWeaponLayer->getChildByName(StringUtils::format("suitcase%d", index));
        suitcase->setSpriteFrame(__String::createWithFormat("suitCaseUnlocked%d.png", index)->getCString());
        
        Animation* animation = AnimationCache::getInstance()->getAnimation(__String::createWithFormat("suitcaseOpen%d", index)->getCString());
        Animate* animate = Animate::create(animation);
//        suitcase->runAction(Sequence::create(DelayTime::create(1), animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::openWeapon, this)),NULL));
        suitcase->runAction(animate);
        
        
        Sprite* lightSpin = getLightSpin(5);
        GameManager::getInstance()->playSoundEffect(SOUND_DDALKACK);
        int var = rand()%(WEAPON_NO_MORE/4);
        if (suitcase->secondTag == 3) {
            var++;
        }
        int weapon = 0;
        while(true){
            weapon = 1 + (WEAPON_NO_MORE/4)*(rand()%suitcase->secondTag + 1) + var;
            if (weapon >= WEAPON_NO_MORE) {
                weapon = WEAPON_NO_MORE - 1;
            }else if (weapon < 1) {
                weapon = 1;
            }
            bool pickAgain = false;
            for (int i = 0; i < bonusWeapon.size(); i++) {
                if (bonusWeapon.at(i) == weapon) {
                    pickAgain = true;
                    break;
                }
            }
            if (pickAgain) {
                continue;
            }else{
                break;
            }
        }
        bonusWeapon.push_back(weapon);
        
        //int weapon = WEAPON_LIGHTNING_GUN; test
        Sprite* sptWeapon = Sprite::createWithSpriteFrameName(GameManager::getInstance()->getWeaponImageFileName(weapon));
        sptWeapon->setScale(0.5f);
        sptWeapon->setOpacity(0);
        sptWeapon->setScale(4);
        lightSpin->setOpacity(0);
        lightSpin->setScale(4);
        sptWeapon->setPosition(suitcase->getPosition() + Point(90, 0));
        lightSpin->setPosition(suitcase->getPosition() + Point(90, 0));
        sptWeapon->runAction(FadeIn::create(0.3));
        lightSpin->runAction(FadeIn::create(0.3));
        sptWeapon->runAction(Sequence::create(MoveBy::create(1, Point(0, 18*4)), DelayTime::create(4), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptWeapon)), nullptr));
        lightSpin->runAction(MoveBy::create(1, Point(0, 18*4)));
        this->addChild(sptWeapon, 4000);
        this->addChild(lightSpin, 4000);
        
        UserDefault::getInstance()->setIntegerForKey(StringUtils::format(KEY_BONUS_WEAPON_FORMAT, index).c_str(), weapon);
        
        log("weapon get: %d", weapon);
        
        ((Text*)bonusWeaponLayer->getChildByName(StringUtils::format("lblWeaponName%d", index)))->setVisible(true);
        ((Text*)bonusWeaponLayer->getChildByName(StringUtils::format("lblWeaponName%d", index)))->setString(GameManager::getInstance()->getWeaponName(weapon));
    }
    
    
    ((Text*)bonusWeaponLayer->getChildByName("lblOk"))->setString(Value(countDown).asString());
    
}
