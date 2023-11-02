//
//  HudLayer.cpp
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 29..
//
//
#include "DualHudLayer.h"
#include "LegendDaryButton.h"
#include "GameManager.h"
#include "DualWorld.h"
#include "MyMessageBox.h"
#include "NativeInterface.h"
#include "LanguageManager.h"
#include "Title.h"
#include "ServerManager.h"
#include "EndingCredit.h"
#include "CursorLayer.h"
#include "ui/UITextBMFont.h"
#include "Firework.h"
//#include "HttpsManager.h"
#include "SceneChanger.h"
bool DualHudLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    weaponIndex = 0;
    size = Director::getInstance()->getWinSize();
    isRecording = false;
    listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(DualHudLayer::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(DualHudLayer::onKeyReleased, this);
    
    SceneChanger* changer = SceneChanger::create();
    std::string strTitle = GM->isLocalGamePlay?"avatar challenge":"network battle";
    if(GM->isBossFight){
        strTitle = "boss";
    }
    changer->setLayer(false, strTitle, strmake("tip%d", GM->tipIndex), nullptr);
    this->addChild(changer, 99999);
    
    Sprite* sptBackground = Sprite::create("background.png");
    this->addChild(sptBackground);
    sptBackground->setPosition(size.width/2, size.height/2);
    sptBackground->runAction(MoveBy::create(1.3f, Point(0, size.height)));
    
    cursorLayer = CursorLayer::create();
    this->addChild(cursorLayer);
    
//    shopLayer = ShopLayer::create();
//    this->addChild(shopLayer);
//    shopLayerMoveYDistance = 0;//120;
//    shopLayer->setPositionY(size.height - shopLayer->getContentSize().height + shopLayerMoveYDistance);
    
    hudLayer = CSLoader::createNode("EnergyBar.csb");
//    hudLayer->setContentSize(size);
    ui::Helper::doLayout(hudLayer);
    this->addChild(hudLayer);
    hudLayer->setPosition(Point(size.width/2 - hudLayer->getContentSize().width/2, size.height/2 - hudLayer->getContentSize().height/2));
    
    Button* btn = (Button*)hudLayer->getChildByName("btnBack");
    btn->addClickEventListener(CC_CALLBACK_0(DualHudLayer::onBack, this));
    btn = (Button*)hudLayer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(DualHudLayer::onTapToContinue, this));
    btn = (Button*)hudLayer->getChildByName("btnNext");
    btn->setVisible(false);
    btn->addClickEventListener(CC_CALLBACK_0(DualHudLayer::onNext, this));
    Text* lbl = (Text*)hudLayer->getChildByName("lblBack");
    LM->setLocalizedString(lbl, "cancel");
    lbl = (Text*)hudLayer->getChildByName("lblNext");
    LM->setLocalizedString(lbl, "next search");
    lbl = (Text*)hudLayer->getChildByName("lblTapToContinue");
    GameManager::getInstance()->makeItSiluk(lbl);
    LM->setLocalizedString(lbl, "touch to start");
    lbl = (Text*)hudLayer->getChildByName("lblCoin");
    lbl->setString(Value(getNextMatchPrice()).asString());
    Node* sptCoin = hudLayer->getChildByName("sptCoin");
    GameManager::getInstance()->alignToCenter(sptCoin, lbl, 50, btn->getPositionX(), 0);
    
//    int enemyFamily = 1;// test  GameManager::getInstance()->enemyDoc["family"].GetInt();
//    int playerFamily = UserDefault::getInstance()->getIntegerForKey(KEY_FAMILY, 0);
    ImageView* playerFlag = (ImageView*)hudLayer->getChildByName("imgPlayerFlag");
//    GameManager::getInstance()->loadFlagImage(playerFlag, playerFamily);
    ImageView* enemyFlag = (ImageView*)hudLayer->getChildByName("imgEnemyFlag");
//    GameManager::getInstance()->loadFlagImage(enemyFlag, enemyFamily);
    
//    lblKeyCollected = (TextBMFont*)hudLayer->getChildByName("lblKeyCollected");
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
//    if(GameManager::getInstance()->showVPad){
//        Texture2D* leftRightTexture = Director::getInstance()->getTextureCache()->addImage("leftRight.png");
//        Texture2D* aTexture = Director::getInstance()->getTextureCache()->addImage("btnA.png");
//        Texture2D* bTexture = Director::getInstance()->getTextureCache()->addImage("btnB.png");
//
//        btnLeftRight = LegendDaryButton::buttonWithTexture(leftRightTexture, BUTTON_LEFT_RIGHT);
//        btnLeftRight->setPosition( Point(200, 80) );
//        btnLeftRight->addListener();
//        btnLeftRight->setVisible(false);
//        buttonArray.pushBack( btnLeftRight);
//        //    btnLeftRight->setOpacity(155);
////        addChild(btnLeftRight);
//
//        btnA = LegendDaryButton::buttonWithTexture( aTexture, BUTTON_A );
//        btnA->setPosition( Point(size.width - 320, 80) );
//        btnA->addListener();
//        btnA->setVisible(false);
//        buttonArray.pushBack( btnA);
//        //    btnA->setOpacity(155);
////        addChild(btnA);
//
//        btnB = LegendDaryButton::buttonWithTexture( bTexture, BUTTON_B );
//        btnB->setPosition( Point(size.width - 120, 80) );
//        btnB->addListener();
//        btnB->setVisible(false);
//        buttonArray.pushBack( btnB);
//        //    btnB->setOpacity(155);
////        addChild(btnB);
//
//        float siz = GameManager::getInstance()->getButtonSize();
//        setButtonSize(siz);
//    }
    
    talkLayer = NULL;
    talkLabel = NULL;
    
    gameOverLayer = NULL;
    reviveLayer = NULL;
    pauseLayer = NULL;
    gameClearLayer = NULL;
    
    GameManager::getInstance()->firePressed = false;
    GameManager::getInstance()->jumpPressed = false;
    GameManager::getInstance()->leftPressed = false;
    GameManager::getInstance()->rightPressed = false;
    
    
//    lblCoin = LanguageManager::getInstance()->getLocalizedLabel("0", Color4B::WHITE);
//    this->addChild(lblCoin);
//    lblCoin->setPosition(Point(size.width/2, size.height - lblCoin->getContentSize().height));
//    lblCoin->setAnchorPoint(Point(0, 0.5));
//    
//    Sprite* sptCoin = Sprite::createWithSpriteFrameName("silverCoin0.png");
//    this->addChild(sptCoin);
//    sptCoin->setAnchorPoint(Point(1, 0.5));
//    sptCoin->setScale(2);
//    sptCoin->setPosition(lblCoin->getPosition() + Point(-4, 0));
    
//    Sprite* bar = Sprite::create("blackSquare.png");
//    addChild(bar, -1);
//    bar->setScaleX(size.width);
//    bar->setScaleY(60);
//    bar->setPosition(Point(size.width/2, size.height - 15));
    
    
/*    coinLabel = Label::createWithTTF(Value(GameManager::getInstance()->getCoin()).asString(), GameManager::getInstance()->getFont(FONT_BITDUST_ONE), 24);
    coinLabel->setPosition(Point(size.width - 5, size.height - 15));
    coinLabel->setAnchorPoint(Point(1,0.5));
//    coinLabel->enableOutline(Color4B(0,0,0, 255), 4);
    
//    CCLabelBMFont::create(Value(GameManager::getInstance()->getCoin()).asString(), "Legendary.fnt", size.width, kCCTextAlignmentRight);
    coinLabel->setColor(Color3B(255, 255, 255));
    coinLabel->getTexture()->setAliasTexParameters();
    coinLabel->setScale(4);
    this->addChild(coinLabel);
    coinLabel->setAnchorPoint(Point(1,0.5));
    coinLabel->setPosition(Point(size.width - 10, size.height - 25));*/
    
//    coin = Sprite::createWithSpriteFrameName("goldCoin0.png");
//    addChild(coin);
//    coin->setScale(3.2f);
//    coin->setPosition(coinLabel->getPosition() + Point(-coinLabel->getContentSize().width*coinLabel->getScale() - 24, 0));

    pauseItem = MenuItemImage::create("pauseButton.png", "pauseButton.png", CC_CALLBACK_0(DualHudLayer::showPauseLayer, this));
    pauseItem->setVisible(false);
//    recordItem = MenuItemImage::create("recordButton.png", "recordButton.png", CC_CALLBACK_0(DualHudLayer::startRecord, this));
//    recordItem->setVisible(false);
//    bombItem = MenuItemImage::create("aButton.png", "aButton.png", CC_CALLBACK_0(DualHudLayer::useBomb, this));
//    bombItem->setPosition(Point(size.width - pauseItem->getContentSize().width - 5, size.height/2));
    
//    potionItem = MenuItemImage::create("aButton.png", "aButton.png", CC_CALLBACK_0(DualHudLayer::usePotion, this));
//    potionItem->setPosition(Point(potionItem->getContentSize().width + 10, size.height/2));
    
//    Menu* menu =  Menu::create(pauseItem, NULL);
//    menu->setPosition(Point(0, 0));
//    this->addChild(menu, 10);
    
    blackScreen = Sprite::create("blackSquare.png");
    this->addChild(blackScreen);
    blackScreen->setScaleX(size.width);
    blackScreen->setScaleY(size.height);
    blackScreen->setPosition(Point(size.width/2, size.height/2));
    blackScreen->setVisible(false);
    
    float gap = 5;
    bool isRankingEnabled = false;
    if (GameManager::getInstance()->currentStageIndex == STAGE_LOBBY) {
        // ranking
        if (isRankingEnabled) {
            rankingLayer = Layout::create();
            //rankingLayer->setClippingEnabled(true);
            float rankingWidth = 200 + gap;
            float nameWidth = 130;
            float scoreWidth = rankingWidth - nameWidth - gap;
            float lineSpace = 20;
            int rowCount = 11;
            Label* lblName;Label* lblScore;
            std::vector<std::string> nameList;
            nameList.push_back("Dary");
            nameList.push_back("Aron");
            nameList.push_back("Choppa");
            nameList.push_back("Luffy");
            nameList.push_back("Dary");
            nameList.push_back("Aokizi");
            nameList.push_back("Robin");
            nameList.push_back("Zoro");
            nameList.push_back("Nami");
            nameList.push_back("Buggy");
            nameList.push_back("Sanji");
            for (int i = 0; i < rowCount; i++) {
                lblName = Label::createWithTTF(__String::createWithFormat("%d. %s", i+1, nameList.at(i).c_str())->getCString(), GameManager::getInstance()->getFont(FONT_VISITOR), 20);
                lblName->setName(__String::createWithFormat("name_%d", i)->getCString());
                rankingLayer->addChild(lblName);
                lblName->setAnchorPoint(Point(0, 0));
                lblName->setPosition(Point(gap, gap + lineSpace*(rowCount - 1 - i)));
                GameManager::getInstance()->makeLabelEllipsis(lblName, nameWidth);
                
                lblScore = Label::createWithTTF(Value(391132).asString(), GameManager::getInstance()->getFont(FONT_VISITOR), 20);
                lblScore->setName(__String::createWithFormat("score_%d", i)->getCString());
                rankingLayer->addChild(lblScore);
                lblScore->setAnchorPoint(Point(0, 0));
                lblScore->setPosition(Point(gap + nameWidth + gap, gap + lineSpace*(rowCount - 1 - i)));
            }
            // draw splitter
            DrawNode* dnLines = DrawNode::create();
            dnLines->drawLine(Point(0, gap + lineSpace - 1), Point(rankingWidth, gap + lineSpace - 1), Color4F::WHITE);
            dnLines->drawLine(Point(0, gap + lineSpace*rowCount - 1), Point(rankingWidth, gap + lineSpace*rowCount - 1), Color4F::WHITE);
            dnLines->drawLine(Point(0, gap + lineSpace*(rowCount + 1)), Point(rankingWidth, gap + lineSpace*(rowCount + 1)), Color4F::WHITE);
            dnLines->drawLine(Point(0, 0), Point(rankingWidth, 0), Color4F::WHITE);
            // column lines
            dnLines->drawLine(Point(0, 0), Point(0, gap + lineSpace*(rowCount + 1)), Color4F::WHITE);
            dnLines->drawLine(Point(gap + nameWidth + gap/2, 0), Point(gap + nameWidth + gap/2, gap + lineSpace*rowCount), Color4F::WHITE);
            dnLines->drawLine(Point(rankingWidth, 0), Point(rankingWidth, gap + lineSpace*(rowCount + 1)), Color4F::WHITE);
            
            int ranking = GameManager::getInstance()->ranking;
            std::string nameStr;
            if (ranking < 0) {
                nameStr = __String::createWithFormat("100+. %s", UserDefault::getInstance()->getStringForKey(KEY_NAME, "Newbie").c_str())->getCString();
            }else{
                nameStr = __String::createWithFormat("%d. %s", ranking + 1, UserDefault::getInstance()->getStringForKey(KEY_NAME, "Newbie").c_str())->getCString();
            }
            lblName->setString(nameStr);
            GameManager::getInstance()->makeLabelEllipsis(lblName, nameWidth);
//            int lastDay = UserDefault::getInstance()->getIntegerForKey(KEY_DAY_COUNT, 0);
//            int today = SM->getCurrentTime()/86400;//GameManager::getInstance()->dayCount;
//            if (today >= 0 && lastDay != today) {
//                UserDefault::getInstance()->setIntegerForKey(KEY_DAY_COUNT, today);
//                UserDefault::getInstance()->setIntegerForKey(KEY_TODAYS_SCORE, 0);
//            }
            lblScore->setString(Value(UserDefault::getInstance()->getIntegerForKey(KEY_TODAYS_SCORE, 0)).asString());
            
            rankingLayer->addChild(dnLines);
            Label* lbl = Label::createWithTTF("YOU ->", GameManager::getInstance()->getFont(FONT_VISITOR), 20);
            rankingLayer->addChild(lbl);
            lbl->setAnchorPoint(Point(1, 0));
            lbl->setPosition(Point(-gap, gap));
            lbl->runAction(Sequence::create(DelayTime::create(5), FadeOut::create(5), NULL));
            
            lbl = Label::createWithTTF("TODAY'S RANKING", GameManager::getInstance()->getFont(FONT_VISITOR), 20);
            rankingLayer->addChild(lbl);
            lbl->setAnchorPoint(Point(0, 0));
            lbl->setPosition(Point(gap, gap + lineSpace*rowCount));
            
            rankingLayer->setContentSize(Size(rankingWidth, rowCount*lineSpace));
            rankingLayer->setPosition(Point(size.width - rankingWidth - gap, gap));
            this->addChild(rankingLayer);
        }
    }else if(GameManager::getInstance()->currentStageIndex >= 0){
        lblScore = Label::createWithTTF(__String::createWithFormat("SCORE: %d", 0)->getCString(), GameManager::getInstance()->getFont(FONT_VISITOR), 20);
        lblScore->setPosition(Point(size.width - gap, gap));
        lblScore->setAnchorPoint(Point(1, 0));
        this->addChild(lblScore);
    }
    
//    registerControllerListener();
    // lr
//    joystickLR = SneakyJoystickSkinnedBase::create();
//    Sprite* pBack = Sprite::create("joystickBack.png");
//    joystickLR->setBackgroundSprite(pBack);
//    Sprite* pThumb = Sprite::create("joystickBall.png");
//    joystickLR->setThumbSprite(pThumb);
//    joystickLR->setScale(4);
//    joystickLR->setVisible(GameManager::getInstance()->showVPad);
//    //joystick->setContentSize(Size(200, 200));
//
//    js = SneakyJoystick::create();
//    js->stickType = STICK_LEFT;
//    joystickLR->setBackgroundSprite(pBack);
//    joystickLR->setThumbSprite(pThumb);
//    joystickLR->setJoystick(js);
//    joystickLRPos = Point(pBack->getContentSize().width*joystickLR->getScale(), pBack->getContentSize().height*joystickLR->getScale());
//    joystickLR->setPosition(joystickLRPos + Point(0, 0));
//    this->addChild(joystickLR, 10);//move to addJoystickToParent()
//
//    // aim
//    joystickAim = SneakyJoystickSkinnedBase::create();
//    pBack = Sprite::create("joystickBack.png");
//    joystickAim->setBackgroundSprite(pBack);
//    pThumb = Sprite::create("joystickBall.png");
//    joystickAim->setThumbSprite(pThumb);
//    joystickAim->setScale(3);
//    joystickAim->setVisible(GameManager::getInstance()->showVPad);
//    //joystick->setContentSize(Size(200, 200));
//
//    js2 = SneakyJoystick::create();
//    js2->stickType = STICK_RIGHT;
//    joystickAim->setBackgroundSprite(pBack);
//    joystickAim->setThumbSprite(pThumb);
//    joystickAim->setJoystick(js2);
////    joystickAimPos = Point(size.width - pBack->getContentSize().width*joystickAim->getScale(), pBack->getContentSize().height*joystickAim->getScale());
//    joystickAimPos = Point(size.width - 180, pBack->getContentSize().height*joystickAim->getScale());
//    joystickAim->setPosition(joystickAimPos + Point(0, 0));
//    this->addChild(joystickAim, 10);//move to addJoystickToParent()
//
//
//    btnJump = VirtualPadButton::create(BUTTON_JUMP);
//    this->addChild(btnJump);
////    btnJumpPos= Point(size.width - btnJump->getContentSize().width*btnJump->getScaleX()*2, btnJump->getContentSize().height*btnJump->getScaleY()*0.7);
//    btnJump->setPosition(Point(size.width - 80, 80));
//    btnJump->setScale(3);
//    btnJump->setVisible(GameManager::getInstance()->showVPad);
//
//    btnReload = VirtualPadButton::create(BUTTON_RELOAD);
//    this->addChild(btnReload);
////    btnReloadPos= Point(size.width - btnReload->getContentSize().width*btnReload->getScaleX()*2, btnReload->getContentSize().height*btnReload->getScaleY()*0.7);
//    btnReloadPos = Point( 200, 200);
//    btnReload->setPosition(Point(size.width - 110, 180));
//    btnReload->setScale(2.5);
//    btnReload->setVisible(GameManager::getInstance()->showVPad);
//
//    btnWeapon = VirtualPadButton::create(BUTTON_WEAPON);
//    this->addChild(btnWeapon);
//    btnWeaponPos= Point(size.width - btnWeapon->getContentSize().width*btnWeapon->getScaleX()*2, btnWeapon->getContentSize().height*btnWeapon->getScaleY()*0.7);
//    btnWeapon->setPosition(Point(size.width - 185, 225));
//    btnWeapon->setScale(2.0);
//    btnWeapon->setVisible(GameManager::getInstance()->showVPad);
//
//    btnAction = VirtualPadButton::create(BUTTON_ACTION);
//    this->addChild(btnAction);
//    btnAction->setPosition(Point(size.width - 260, 230));
//    btnAction->setScale(1.8);
//    btnAction->setVisible(GameManager::getInstance()->showVPad);

    lblReady = Label::createWithTTF("", "visitor1.ttf", 120);
    this->addChild(lblReady);
    lblReady->setPosition(Point(size.width/2, size.height*3/5));
    
    // init done
    return true;
}

void DualHudLayer::onBack(){
    removeListener();
    Scene* scene = Scene::create();
    scene->addChild(Title::create());
    Director::getInstance()->replaceScene(TransitionCrossFade::create(0.5f, scene));
}
void DualHudLayer::onNext(){
    int price = getNextMatchPrice();
    int coinYouHave = GameManager::getInstance()->getCoin();
    if (coinYouHave >= price) {
        GameManager::getInstance()->addCoin(-price);
        
        this->schedule(schedule_selector(DualHudLayer::onWaitingMatching), 0.1f);
        waitingTime = 0;
        
        Text* lbl = (Text*)hudLayer->getChildByName("lblTapToContinue");
        LM->setLocalizedString(lbl, "connect");
        
        Sprite* sptBackground = Sprite::create("background.png");
        this->addChild(sptBackground);
        sptBackground->setPosition(size.width/2, size.height/2);
    }else{
        GameManager::getInstance()->showDisposableMessage("not enough coin text only", this);
    }
}
void DualHudLayer::onWaitingMatching(float dt){
    if (!GM->isLocalGamePlay) {
        
        int timeout = 5;
        if (SM->isServerFailed) {
//            showMessageBox(NN_MULTI_SEARCH, "network fail", "ok", false);
            showMessageBox(0, "network fail", "ok", false); // test
            return;
        }
        if (waitingTime > timeout || (isMultiSearchDone && !isMultiSearchSuccess)) { // SM should set those in his class
            waitingTime = -1000;
//            this->unschedule(schedule_selector(DualHudLayer::onWaitingMatching)); // test
            
//            GameManager::getInstance()->getAIData(); // test
            
            Scene* scene = DualWorld::scene(0);
            Director::getInstance()->replaceScene(TransitionCrossFade::create(0.3f, scene));
            return;
        }
        
        if (isMultiSearchDone && isMultiSearchSuccess) {
            
        }else{
            return;
        }
    }else{
//        GameManager::getInstance()->getAIData(); // test
    }
    
    waitingTime = -1000;
//    this->unschedule(schedule_selector(Title::onWaitingMatching)); // test 
    
    Scene* scene = DualWorld::scene(0);
    Director::getInstance()->replaceScene(scene);
}
void DualHudLayer::onTapToContinue(){
    hudLayer->getChildByName("lblTapToContinue")->removeFromParent();
    hudLayer->getChildByName("btnBlock")->removeFromParent();
    hudLayer->getChildByName("btnBack")->removeFromParent();
    hudLayer->getChildByName("lblBack")->removeFromParent();
    hudLayer->getChildByName("btnNext")->removeFromParent();
    hudLayer->getChildByName("lblNext")->removeFromParent();
    hudLayer->getChildByName("lblCoin")->removeFromParent();
    hudLayer->getChildByName("sptCoin")->removeFromParent();
    GameManager::getInstance()->getCurrentDualLayer()->startBattle();
}

int DualHudLayer::getNextMatchPrice(){
    int level = UserDefault::getInstance()->getIntegerForKey(KEY_LEVEL, 0);
    return 50 + level*45;
}
//void DualHudLayer::showResult(bool win){
//    Node* resultPopup = CSLoader::createNode("ResultPopup.csb");
////    resultPopup->setContentSize(size);
////    ui::Helper::doLayout(hudLayer);
//    this->addChild(resultPopup);
////    resultPopup->setPosition(Point(size.width/2, size.height/2));
//    
//    cocostudio::timeline::ActionTimeline* animation = CSLoader::createTimeline("ResultPopup.csb");
//    animation->gotoFrameAndPause(70);
//    animation->play("ending", false);
//    resultPopup->runAction(animation);
//    Text* lblResult = (Text*)resultPopup->getChildByName("lblResult");
//    Text* lblDescription = (Text*)resultPopup->getChildByName("lblDescription");
//    LM->setLocalizedString(lblResult, "result");
//    if (win) {
//        this->addChild(Firework::create());
//        LM->setLocalizedString(lblDescription, "win description");
//    }else{
//        LM->setLocalizedString(lblDescription, "lose description");
//    }
//    
//    Button* btnOk = (Button*)resultPopup->getChildByName("btnOk");
//    btnOk->addClickEventListener(CC_CALLBACK_0(DualHudLayer::onResultOk, this));
//    Text* lblOk = (Text*)resultPopup->getChildByName("lblOk");
//    LM->setLocalizedString(lblOk, "ok");
//    
//    int coinToAdd = 100;
////    shopLayer->coinTo = shopLayer->currentCoin + coinToAdd;
//}
void DualHudLayer::onResultOk(){
    Scene* scene = Scene::create();
    scene->addChild(Title::create());
    Director::getInstance()->replaceScene(TransitionCrossFade::create(0.3f, scene));
}
void DualHudLayer::setInfo(std::string playerName, std::string enemyName, int playerHealth, int playerAttack, int enemyHealth, int enemyAttack, int playerTrophy, int enemyTrophy){
    Text* lbl = (Text*)hudLayer->getChildByName("lblPlayerName");
//    LM->setLocalizedStringNotKey(lbl, playerName);
    lbl->setString(playerName);
    lbl = (Text*)hudLayer->getChildByName("lblEnemyName");
//    LM->setLocalizedStringNotKey(lbl, enemyName);
    lbl->setString(enemyName);
    lbl = (Text*)hudLayer->getChildByName("lblPlayerTrophy");
    lbl->setString(Value(playerTrophy).asString());
    lbl = (Text*)hudLayer->getChildByName("lblEnemyTrophy");
    lbl->setString(Value(enemyTrophy).asString());
//    lbl = (Text*)hudLayer->getChildByName("lblPlayerAttack");
//    lbl->setString(Value(playerAttack).asString());
//    lbl = (Text*)hudLayer->getChildByName("lblEnemyAttack");
//    lbl->setString(Value(enemyAttack).asString());
}
void DualHudLayer::setPlayerHealth(int percent){
    LoadingBar* lb = (LoadingBar*)hudLayer->getChildByName("lbPlayerEnergy");
    lb->setPercent(percent);
    if (percent <= 30) {
        lb->setColor(Color3B(230, 38, 33));
    }
}
void DualHudLayer::setEnemyHealth(int percent){
    LoadingBar* lb = (LoadingBar*)hudLayer->getChildByName("lbEnemyEnergy");
    lb->setPercent(percent);
    if (percent <= 30) {
        lb->setColor(Color3B(230, 38, 33));
    }
}
void DualHudLayer::readySetBattle(){
    if (readySetBattleState == 0) {
        lblReady->setString("READY");
    }else if (readySetBattleState == 1) {
        lblReady->setString("SET");
    }else if (readySetBattleState == 2) {
        lblReady->setString("BATTLE!");
        lblReady->runAction(Sequence::create(RotateBy::create(0.07f, 2), RotateBy::create(0.07f, -5),RotateBy::create(0.07f, 5),RotateBy::create(0.07f, -5),RotateBy::create(0.07f, 5),RotateBy::create(0.07f, -5),RotateBy::create(0.07f, 5),RotateBy::create(0.07f, -5),RotateBy::create(0.07f, 5),RotateBy::create(0.07f, -5),RotateBy::create(0.07f, 5),RotateBy::create(0.07f, -5),RotateBy::create(0.07f, 3), DelayTime::create(1), CallFuncN::create(CC_CALLBACK_1(Label::removeFromParentAndCleanup, lblReady)),NULL));
    }
    lblReady->setScale(0.4f);
    lblReady->runAction(EaseBackOut::create(ScaleTo::create(0.1f, 1)));
    readySetBattleState++;
}
void DualHudLayer::setScore(int score){
    if(GameManager::getInstance()->currentStageIndex >= 0){
        lblScore->setString(__String::createWithFormat("Score: %d", GameManager::getInstance()->totalScore)->getCString());
    }
}

void DualHudLayer::registerControllerListener()
{
    
    bool okay = true;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//    float version = NativeInterface::NativeInterface::getSystemVersion();
//    CCLOG("** iOS Version: %f", version);
//    if (version < 7) {
//        okay = false;
//    }
#endif
    if (okay) {
        //create an evnetListenerController
        _listener = EventListenerController::create();
        
        //bind onConneected event call function
        _listener->onConnected = CC_CALLBACK_2(DualHudLayer::onConnectController,this);
        
        //bind disconnect event call function
        _listener->onDisconnected = CC_CALLBACK_2(DualHudLayer::onDisconnectedController,this);
        
        //bind onKeyDown event call function
        _listener->onKeyDown = CC_CALLBACK_3(DualHudLayer::onKeyDown, this);
        
        //bind onKeyUp event call function
        _listener->onKeyUp = CC_CALLBACK_3(DualHudLayer::onKeyUp, this);
        
        //bind onAxis event call function, onAxis will be called when analog stick is changed
        _listener->onAxisEvent = CC_CALLBACK_3(DualHudLayer::onAxisEvent, this);
        
        //Activate the listener into the event dispatcher
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_listener, this);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        //This function should be called for iOS platform
        Controller::startDiscoveryController();
#endif
        
    }
}

//Controller is the obejects of the Controller，keyCode means the keycode of the controller you click down
void DualHudLayer::onKeyDown(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event)
{
    CCLOG("KeyDown:%d", keyCode);
}

void DualHudLayer::onKeyUp(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event)
{
    //You can get the controller by tag, deviceId or devicename if there are multiple controllers
    CCLOG("tag:%d DeviceId:%d DeviceName:%s", controller->getTag(), controller->getDeviceId(), controller->getDeviceName().c_str());
    CCLOG("KeyUp:%d", keyCode);
}

//The axis includes X-axis and Y-axis and its range is from -1 to 1. X-axis is start from left to right and Y-axis is bottom to top.
void DualHudLayer::onAxisEvent(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event)
{
//    const auto& keyStatus = controller->getKeyStatus(keyCode);
//    CCLOG("Axis KeyCode:%d Axis Value:%f", keyCode, keyStatus.value);
}

void DualHudLayer::onConnectController(Controller* controller, Event* event)
{
    CCLOG("Game controller connected");
}

void DualHudLayer::onDisconnectedController(Controller* controller, Event* event)
{
    CCLOG("Game controller disconnected");
}
const char* DualHudLayer::getWeaponName(int index){
//    GameManager::getInstance()->getWeaponName(index);
    return "";
}
void DualHudLayer::addHeartAnimated(Point pos){
    Sprite* heart = lifeArray.at(lifeArray.size() - 1);
    Sprite* spt = Sprite::create("heart.png");
    spt->setPosition(pos);
    this->addChild(spt);
    spt->runAction(Sequence::create(EaseIn::create(MoveTo::create(1, heart->getPosition() + Point(50, 0)), 3), CallFuncN::create(CC_CALLBACK_1(DualHudLayer::addHeartDone, this)), CallFuncN::create(CC_CALLBACK_1(DualHudLayer::spriteMoveDone, this)), NULL));
}
void DualHudLayer::addHeartDone(Ref* obj){
    setLife(lifeArray.size() + 1);
    Sprite* heart = lifeArray.at(lifeArray.size() - 1);
    
    Sprite* spt = Sprite::create("backLight.png");
    spt->setScale(0.5);
    spt->runAction(Sequence::create(RotateBy::create(1, 300), CallFuncN::create(CC_CALLBACK_1(DualHudLayer::spriteMoveDone, this)), NULL));
    
    heart->addChild(spt);
    spt->setPosition(Point(heart->getContentSize().width/2, heart->getContentSize().height/2));
    
    GameManager::getInstance()->playSoundEffect(SOUND_MEDICINE);
}
// Implementation of the keyboard event callback function prototype
void DualHudLayer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (isInScene || GameManager::getInstance()->getCurrentDualLayer()->indexToWarp != -9999) {
        return;
    }
    if (dialogBox != nullptr) {
        dialogBox->showNextTalk();
        return;
    }
//    CCLOG("Key with keycode %d pressed", keyCode);
    
    if(keyCode == EventKeyboard::KeyCode::KEY_Z ||
       keyCode == EventKeyboard::KeyCode::KEY_DPAD_CENTER){
        GameManager::getInstance()->buttonDown(BUTTON_A);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_W){
        GameManager::getInstance()->buttonDown(BUTTON_B);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_C ||
             keyCode == EventKeyboard::KeyCode::KEY_Y){
        GameManager::getInstance()->cPressed = true;
    }else if(keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_UP ){
        GameManager::getInstance()->buttonDown(BUTTON_B);
    }else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW ||
              keyCode == EventKeyboard::KeyCode::KEY_S  ||
              keyCode == EventKeyboard::KeyCode::KEY_DPAD_DOWN ){
        GameManager::getInstance()->downPressed = true;
    }else if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_LEFT ||
             keyCode == EventKeyboard::KeyCode::KEY_A ){
        GameManager::getInstance()->buttonDown(BUTTON_LEFT);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_D  ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_RIGHT ){
        GameManager::getInstance()->buttonDown(BUTTON_RIGHT);
    }else if(keyCode >= EventKeyboard::KeyCode::KEY_1 &&
             keyCode <= EventKeyboard::KeyCode::KEY_9){
        GameManager::getInstance()->getCurrentDualLayer()->changeWeapon((int)keyCode - (int)EventKeyboard::KeyCode::KEY_1);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_E){
        GameManager::getInstance()->getCurrentDualLayer()->takeAction();
    }else if(keyCode == EventKeyboard::KeyCode::KEY_TAB){
        GameManager::getInstance()->getCurrentDualLayer()->showBigMiniMap(true);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_ENTER){
        tryResultOK();
    }else if(keyCode == EventKeyboard::KeyCode::KEY_ESCAPE){
//        if(GameManager::getInstance()->currentStageIndex == STAGE_LOBBY){
            auto scene = Scene::create();
            scene->addChild(Title::create());
            Director::getInstance()->replaceScene(scene);
//        }
    }
}
void DualHudLayer::tryResultOK()
{
    if(GameManager::getInstance()->getCurrentDualLayer()->resultLayer != nullptr){
        if(GameManager::getInstance()->getCurrentDualLayer()->resultDone){
            if (GameManager::getInstance()->isGameClear) {
                GameManager::getInstance()->getCurrentDualLayer()->resultLayer->removeFromParentAndCleanup(true);
            }else{
                GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
                
                Scene* scene = Scene::create();
                scene->addChild(DualWorld::scene(STAGE_LOBBY));
                Director::getInstance()->replaceScene(scene);
            }
        }else{
            GameManager::getInstance()->getCurrentDualLayer()->unschedule(schedule_selector(DualWorld::updateResult));
            GameManager::getInstance()->getCurrentDualLayer()->schedule(schedule_selector(DualWorld::updateResult));
        }
    }
}
void DualHudLayer::showTyping(){
    lblTyping = Label::createWithTTF("", GameManager::getInstance()->getFont(FONT_VISITOR), 30);
    this->addChild(lblTyping, 1);
    lblTyping->setPosition(Point(size.width/2, size.height/4));
    
    Sprite* sptBack = Sprite::create("whiteBigCircle.png");
    this->addChild(sptBack, 0);
    sptBack->setColor(Color3B::BLACK);
    sptBack->setOpacity(160);
    sptBack->setScale(2, 0.5);
    sptBack->stopAllActions();
    sptBack->setPosition(lblTyping->getPosition());
    
    this->schedule(schedule_selector(DualHudLayer::typingCursorBlinking), 1);
}
void DualHudLayer::typing(std::string str){
    if (originalString.size() > 9 && str.compare("<-") != 0) {
        return;
    }
    if (str.compare("<-") == 0) {
        originalString = originalString.substr(0, originalString.size() - 1);
    }else{
        originalString += str;
    }
    lblTyping->setString(originalString);
}
void DualHudLayer::typingCursorBlinking(float dt){
    if (lblTyping->getString().size() != originalString.size()) {
        lblTyping->setString(originalString);
    }else{
        lblTyping->setString(originalString + "_");
    }
}
void DualHudLayer::showStageTitle(){
    char bufTitle[100];
    char bufStage[50];
    int stageIndex = GameManager::getInstance()->currentStageIndex;
    if (stageIndex == -1) {
        sprintf(bufStage, "%s", LanguageManager::getInstance()->getText("stageLobby").c_str());
    }else{
        sprintf(bufStage, "%s %d", LanguageManager::getInstance()->getText("stage").c_str(), stageIndex + 1);
    }
    
    if(stageIndex >= TUTORIAL_STAGE_START && stageIndex <= TUTORIAL_STAGE_END){
        sprintf(bufStage, "%s %d", LanguageManager::getInstance()->getText("lesson").c_str(), stageIndex - TUTORIAL_STAGE_START + 1);
        if (stageIndex == TUTORIAL_STAGE_END) {
            sprintf(bufTitle, "");
            sprintf(bufStage, "%s", LanguageManager::getInstance()->getText("theLastLesson").c_str());
        }else{
            
        }
        sprintf(bufTitle, "%s", LanguageManager::getInstance()->getText(__String::createWithFormat("tutorialTitle%d", stageIndex - TUTORIAL_STAGE_START)->getCString()).c_str());
    }else if(stageIndex == STAGE_NAMING){
        sprintf(bufTitle, "%s", LanguageManager::getInstance()->getText(__String::createWithFormat("stageTitle%d", stageIndex)->getCString()).c_str());
        sprintf(bufStage, "%s", LanguageManager::getInstance()->getText("stageTypeYourName").c_str());
        showTyping();
    }else{
        sprintf(bufTitle, "%s", LanguageManager::getInstance()->getText(__String::createWithFormat("stageTitle%d", stageIndex)->getCString()).c_str());
    }
    float y = size.height*3/4 + 20;
    Label* lblTitle = LanguageManager::getInstance()->getLocalizedLabel(bufTitle, Color4B(235, 235, 235, 255));
    lblTitle->setAnchorPoint(Point(0.5, 0.5));
    lblTitle->setPosition(Point(size.width/2, y));
    this->addChild(lblTitle);
    lblTitle->setOpacity(0);
    
    
    Label* lblStage = LanguageManager::getInstance()->getLocalizedLabel(bufStage, Color4B(235, 235, 235, 255));
    lblStage->setAnchorPoint(Point(0.5, 0.5));
    lblStage->setPosition(Point(size.width/2, y - lblTitle->getBoundingBox().size.height - 2));
    this->addChild(lblStage);
    lblStage->setOpacity(0);
    
    lineLength = lblTitle->getBoundingBox().size.width>lblStage->getBoundingBox().size.width?lblTitle->getBoundingBox().size.width:lblStage->getBoundingBox().size.width;
    currentLineLength =0;
 
    dnTitleLine = DrawNode::create();
    this->addChild(dnTitleLine);
    dnTitleLine->setPosition(lblTitle->getBoundingBox().origin + Point(0, -4));
    
    
    float dur = 0.5f;
    sptWhiteGun = Sprite::create("whiteGun.png");
    sptWhiteGun->setAnchorPoint(Point(0.2, 0.3));
    sptWhiteGun->setScale(3);
    sptWhiteGun->setPosition(Point(dnTitleLine->getPosition() + Point(-60, -13)));
    this->addChild(sptWhiteGun);
    sptWhiteGun->runAction(Sequence::create(RotateBy::create(dur, 360), CallFunc::create(CC_CALLBACK_0(DualHudLayer::whiteGunFired, this)), EaseOut::create(RotateBy::create(0.1, -30), 2), RotateBy::create(0.4, 30), DelayTime::create(4.0f), FadeOut::create(2.0f), NULL));
    
    Sprite* sptGunEffect = Sprite::create("whiteGunFireEffect.png");
    sptGunEffect->setAnchorPoint(Point(0, 0.5));
    sptGunEffect->setScale(3);
    sptGunEffect->setOpacity(0);
    sptGunEffect->setPosition(sptWhiteGun->getPosition() + Point(50, 15));
    sptGunEffect->runAction(Sequence::create(DelayTime::create(dur), FadeIn::create(0.05f), DelayTime::create(0.1f), FadeOut::create(0.05f), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptGunEffect)), NULL));
    this->addChild(sptGunEffect);
    
    lblTitle->runAction(Sequence::create(DelayTime::create(dur + 0.5f), FadeIn::create(0.5f), DelayTime::create(3.5f), FadeOut::create(2), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, lblTitle)), NULL));
    lblStage->runAction(Sequence::create(DelayTime::create(dur + 0.5f), FadeIn::create(0.5f), DelayTime::create(3.5f), FadeOut::create(2), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, lblStage)), NULL));
}
void DualHudLayer::stageTitleLineUpdate(float dt){
    dnTitleLine->clear();
    float bulletWidth = 10;
    float gap = 4;
    if (currentLineLength > 10) {
        dnTitleLine->drawSolidRect(Point::ZERO, Point(currentLineLength, 4), Color4F(235.0f/255, 235.0f/255, 235.0f/255, sptWhiteGun->getOpacity()/255.0f));
    }
    if (currentLineLength < lineLength) {
        currentLineLength += lineLength*dt/0.5f;
    }
    if (currentLineLength > lineLength) {
        currentLineLength = lineLength;
    }
    dnTitleLine->drawSolidRect(Point(currentLineLength + gap, 0), Point(currentLineLength + bulletWidth + gap, 4), Color4F(235.0f/255, 235.0f/255, 235.0f/255, sptWhiteGun->getOpacity()/255.0f));
    
    if (sptWhiteGun->getOpacity() == 0) {
        sptWhiteGun->removeFromParentAndCleanup(true);
        dnTitleLine->removeFromParentAndCleanup(true);
        this->unschedule(schedule_selector(DualHudLayer::stageTitleLineUpdate));
    }
}
void DualHudLayer::whiteGunFired(){
    this->schedule(schedule_selector(DualHudLayer::stageTitleLineUpdate));
}
void DualHudLayer::showDialog(std::string str){
    if (dialogBox == nullptr) {
        dialogBox = AwesomeDialogBox::create();
        this->addChild(dialogBox);
    }
    CallFunc* call = CallFunc::create(CC_CALLBACK_0(DualHudLayer::onDialogDone, this));
    call->retain();
    dialogBox->ShowDialog(str.c_str(), this, call);
}
void DualHudLayer::onDialogDone(){
    if(dialogBox != nullptr){
        dialogBox->removeFromParentAndCleanup(true);
        dialogBox = nullptr;
    }
    if(UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_DOES_IT_TALKED_FORMAT, "story4")->getCString(), false)){
        showEnding();
    }
}
void DualHudLayer::showEnding(){
    DualWorld* stage = (DualWorld*)GameManager::getInstance()->getCurrentDualLayer();
    stage->unschedule(schedule_selector(DualWorld::gravityUpdate));
    stage->runAction(MoveBy::create(0.5, Point(-100, -160)));
    isInScene = true;
    this->setVisible(false);
    
    Hero* player = GameManager::getInstance()->getCurrentDualLayer()->player;
    player->freezed = true;
    NPCBase* theKnight;
    HoodGhost* son;
    for(auto knight : GameManager::getInstance()->getCurrentDualLayer()->npcArray){
        if (knight->getTag() == UNIT_THE_KNIGHT) {
            theKnight = knight;
        }else if(knight->getTag() == UNIT_HOOD_GHOST){
            son = (HoodGhost*)knight;
        }
    }
    HoodGhost* dad = HoodGhost::create();
    dad->setTarget(player);
    dad->runAnimation("hoodGhost", true);
    stage->spriteBatch->addChild(dad);
    dad->setPosition(player->getPosition() + Point(200, 90));
    dad->setFlippedX(true);
    dad->runAction(Sequence::create(DelayTime::create(2.0), MoveTo::create(0.5, player->getPosition() + Point(110, 50)), MoveTo::create(8, player->getPosition() + Point(100, 45)), NULL));
    theKnight->setPosition(player->getPosition() + Point(160, 180));
    theKnight->runAction(Sequence::create(MoveTo::create(0.5, player->getPosition() + Point(30, 50)), MoveTo::create(8, player->getPosition() + Point(20, 45)), NULL));
    player->runAction(Sequence::create(DelayTime::create(4.0), ScaleTo::create(0.3, -1, 1), NULL));
    player->stopFootStep();
    cocostudio::timeline::ActionTimeline* animation = CSLoader::createTimeline("knight/knightSkeleton.csb");
    animation->gotoFrameAndPause(70);
    animation->play("ending", false);
    theKnight->runAction(animation);
    
    float fixTime = -1.7;
    theKnight->showTalk(LanguageManager::getInstance()->getText("princessTalk"),    3, 45, 3 + fixTime);
    dad->showTalk(LanguageManager::getInstance()->getText("dadTalk"),               3, 15, 4.5 + fixTime);
    son->showTalk(LanguageManager::getInstance()->getText("sonTalk"),               3, 12, 6 + fixTime);
    player->showTalk(LanguageManager::getInstance()->getText("daryTalk"),           3, 10, 7.5 + fixTime);
    
    theKnight->imgTalkBalloon->setPosition(Point(0, 0));
    
    this->scheduleOnce(schedule_selector(DualHudLayer::showCredit), 5);
}
void DualHudLayer::showCredit(float dt){
    
}
void DualHudLayer::showNextTalk(){
    dialogBox->showNextTalk();
}
void DualHudLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (GameManager::getInstance()->getCurrentDualLayer()->indexToWarp != -9999) {
        return;
    }
    if(keyCode == EventKeyboard::KeyCode::KEY_Z ||
       keyCode == EventKeyboard::KeyCode::KEY_DPAD_CENTER){
        GameManager::getInstance()->buttonUp(BUTTON_A);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_W){
        GameManager::getInstance()->buttonUp(BUTTON_B);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_C){
        GameManager::getInstance()->cPressed = false;
    }else if(keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_UP){
        GameManager::getInstance()->buttonUp(BUTTON_B);
    }else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW ||
              keyCode == EventKeyboard::KeyCode::KEY_S ||
              keyCode == EventKeyboard::KeyCode::KEY_DPAD_DOWN){
        GameManager::getInstance()->downPressed = false;
    }else if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_LEFT  ||
             keyCode == EventKeyboard::KeyCode::KEY_A ){
        GameManager::getInstance()->buttonUp(BUTTON_LEFT);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_D ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_RIGHT){
        GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_BACK){
//        resumeGame(NULL, ui::Widget::TouchEventType::ENDED);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_TAB){
        GameManager::getInstance()->getCurrentDualLayer()->showBigMiniMap(false);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_Y){
        onDialogDone();
    }
//    CCLOG("Key with keycode %d released", keyCode);
}
void DualHudLayer::useBomb(){
    if (GameManager::getInstance()->developer) {
        GameManager::getInstance()->getCurrentDualLayer()->useBomb();
        return;
    }
    int bombCount = GameManager::getInstance()->getBombCount();
    if (bombCount > 0) {
        GameManager::getInstance()->getCurrentDualLayer()->useBomb();
        bombCount--;
        GameManager::getInstance()->setBombCount(bombCount);
    }else{
        suggestBombShop();
    }
}
void DualHudLayer::suggestBombShop(){
    // show Bomb layer
    CCLOG("show bomb layer");
}
void DualHudLayer::buyBomb(){
    
}

void DualHudLayer::usePotion(){
    if (GameManager::getInstance()->developer) {
        GameManager::getInstance()->getCurrentDualLayer()->usePotion();
        return;
    }
    int potionCount = GameManager::getInstance()->getPotionCount();
    if (potionCount > 0) {
        GameManager::getInstance()->getCurrentDualLayer()->usePotion();
        potionCount--;
        GameManager::getInstance()->setPotionCount(potionCount);
    }else{
        suggestPotionShop();
    }
}
void DualHudLayer::suggestPotionShop(){
    // show Potion layer
    CCLOG("show potion layer");
}
void DualHudLayer::buyPotion(){
    
}
void DualHudLayer::buyGems(){
    
}
void DualHudLayer::keyDown(int key) {
    switch (key) {
        case 6: // Z
            GameManager::getInstance()->firePressed = true;
            break;
        case 7: // X
            GameManager::getInstance()->jumpPressed = true;
            break;
        case 8: // C
            GameManager::getInstance()->cPressed = true;
            break;
        case 123:
            GameManager::getInstance()->leftPressed = true;
            break;
        case 124:
            GameManager::getInstance()->rightPressed = true;
            break;
        case 125:
            GameManager::getInstance()->downPressed = true;
            break;
        case 126:
            GameManager::getInstance()->upPressed = true;
            break;
    }
//    CCLOG("%d pressed", key);
}

void DualHudLayer::keyUp(int key) {
    switch (key) {
        case 6: // Z
            GameManager::getInstance()->firePressed = false;
            break;
        case 7: // X
            GameManager::getInstance()->jumpPressed = false;
            break;
        case 8: // C
            GameManager::getInstance()->cPressed = false;
            break;
        case 123:
            GameManager::getInstance()->leftPressed = false;
            break;
        case 124:
            GameManager::getInstance()->rightPressed = false;
            break;
        case 125:
            GameManager::getInstance()->downPressed = false;
            break;
        case 126:
            GameManager::getInstance()->upPressed = false;
            break;
    }
//    CCLOG("%d released", key);
}
void DualHudLayer::showTalk(const char* talk){
    if (talkLabel == NULL) {
//        talkLayer = Layer::create();
        talkLabel = Label::createWithTTF(talk, GameManager::getInstance()->getFont(FONT_DEFAULT), 30);//Label::createWithSystemFont(talk, GameManager::getInstance()->getFont(FONT_ARIAL), 30);
        
//        talkLayer->addChild(talkLabel);
//        talkLabel->enableOutline(Color4B(80,80,80,255), 7);
        talkLabel->enableShadow();
//        talkLabel->enableGlow(Color4B(200,200,200,255));
//        talkLabel->setColor(Color3B::BLACK);
        this->addChild(talkLabel);
//        talkLabel->setVisible(false);
        
    }else{
//        hideTalk();
    }
//    hideTalk();
//    CCLOG("showTalk: %s", talk);
    talkLabel->stopAllActions();
    talkLabel->setString(talk);
    talkLabel->setPosition(Point(size.width/2, size.height/2 + 75));
    talkLabel->setScale(0.1);
//    talkLabel->runAction(Sequence::create(CCFadeIn::create(0.4), DelayTime::create(2), CCFadeOut::create(0.4), NULL));
    talkLabel->runAction(Sequence::create(EaseOut::create(ScaleTo::create(0.4, 1), 8), DelayTime::create(2), EaseIn::create(ScaleTo::create(0.4, 0.1), 8), NULL));
    talkLabel->runAction(Sequence::create(EaseOut::create(MoveBy::create(0.4, Point(0, 50)), 8), DelayTime::create(2), EaseIn::create(MoveBy::create(0.4, Point(0, -50)), 8), MoveBy::create(0.01, Point(0, 1000)),NULL));
//    talkLabel->runAction(Sequence::create(MoveBy::create(0.4, Point(0, 50)), DelayTime::create(2), MoveBy::create(0.4, Point(0, 50)),MoveBy::create(0.01, Point(0, 500)), NULL));
}
void DualHudLayer::hideTalk(){
    talkLabel->runAction(FadeOut::create(0.5));
    talkLabel->runAction(MoveBy::create(0.5, Point(0, 40)));
}
void DualHudLayer::showCoinShopLayer(){
    showShopLayer();
//    ((ShopLayer*)GameManager::getInstance()->getShopLayer())->openShop();
}

void DualHudLayer::showAchievementComplete(const char* text){
    CCLOG("show achievement");
    Sprite* achieveNode = Sprite::create("achievementBar.png");
    Sprite* spt = Sprite::create("achievementIconEnabled.png");
    achieveNode->addChild(spt);
    spt->setPosition(Point(80, achieveNode->getContentSize().height/2));
    this->addChild(achieveNode, 10000);
    
    Label* lblText = Label::createWithSystemFont(text, "Arial", 30);
    lblText->setAnchorPoint(Point(0, 0.5));
    lblText->setPosition(Point(spt->getPosition().x + 80, achieveNode->getContentSize().height/2));
    lblText->setTag(722);
    achieveNode->addChild(lblText);
    
    ((Label*)achieveNode->getChildByTag(722))->setString(text);
    achieveNode->setPosition(Point(size.width/2 + 15, size.height + (achieveNode->getContentSize().height/2)));
    
    achieveNode->runAction(Sequence::create(EaseIn::create(MoveBy::create(0.5f, Point(0, -achieveNode->getContentSize().height)),0.2f), CallFuncN::create(CC_CALLBACK_1(DualHudLayer::achievementEffect, this)), DelayTime::create(1), EaseIn::create(MoveBy::create(0.5f, Point(0, achieveNode->getContentSize().height)),4), CallFuncN::create(CC_CALLBACK_1(DualHudLayer::nodeMoveDone, this)), NULL));
}

void DualHudLayer::achievementEffect(Node* node){
    Sprite* achieveNode = (Sprite*)node;
    Sprite* checkSprite = Sprite::create("checkImage.png");
    checkSprite->setPosition(Point(achieveNode->getContentSize().width - 80, achieveNode->getContentSize().height/2));
    
    GameManager::getInstance()->playSoundEffect(SOUND_POWER_UP_45);
    
    ParticleExplosion* particle = ParticleExplosion::create();
    particle->setLife(0.4);                          // 지속시간
    particle->setLifeVar(0.3);
    particle->setPosition(checkSprite->getPosition());        // 위치
    particle->setSpeed(70);                      // 속도
    particle->setTotalParticles(15);
    //    particle->setScale(0.3);                          // 크기
    particle->setStartColor(Color4F(205, 205, 0, 155));
    particle->setEndColor(Color4F(205, 205, 0, 0));
    particle->setTexture(Director::getInstance()->getTextureCache()->addImage("checkImage.png"));          // 이미지 변경
    achieveNode->addChild(particle);
    
    achieveNode->addChild(checkSprite);
}
void DualHudLayer::nodeMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Node* node = (Node*)obj;
    node->stopAllActions();
    node->removeFromParentAndCleanup(true);
}

void DualHudLayer::setButtonSize(float siz) // 0 ~ 1
{
    float scaleFact = 0.5f + siz;
    if(GameManager::getInstance()->showVPad){
        btnLeftRight->setScale(scaleFact);
        btnLeftRight->originalScale = scaleFact;
        btnA->setScale(scaleFact);
        btnA->originalScale = scaleFact;
        btnB->setScale(scaleFact);
        btnB->originalScale = scaleFact;
        int y = 45;
        btnLeftRight->setPosition(Point(100 + 130*siz, y + 65*siz));
        btnA->setPosition(Point(size.width - 130 - siz*200, y + 65*siz));// + (1-siz)*72));
        btnB->setPosition(Point(size.width - 60 - siz*50, y + 65*siz));

        if (siz == 0) {
            btnLeftRight->setOpacity(0);
            btnA->setOpacity(0);
            btnB->setOpacity(0);
        }
    }
}

void DualHudLayer::updateCoin(int howMuch)
{
    lblCoin->setString(__String::createWithFormat("%d", howMuch)->getCString());
//    coin->setPosition(coinLabel->getPosition() + Point(-coinLabel->getContentSize().width*coinLabel->getScale() - 24, 0));
}

void DualHudLayer::enableJoystick(bool enable)
{
    for(auto btn: buttonArray)
    {
        btn->enableButton(enable);
    }
}
void DualHudLayer::addEnergyEffect(){
    int effectCount = 15;
    float dur = 0.5;
    float x = 10 + 47*(lifeArray.size() + 1);
    float y = size.height - 28;
    float interval = 0.05f;
    Sprite* spt;
    int spread = 80;
    for (int i = 0; i < effectCount; i++) {
        spt = Sprite::createWithSpriteFrameName("particle.png");
        spt->setPosition(Point(size.width/2, size.height/2) + Point(rand()%spread - spread/2, rand()%spread - spread/2));
        Point pos = spt->getPosition();
        spt->setScale(3 + (rand()%10)*0.4);
        this->addChild(spt, 100);
        spt->runAction(FadeOut::create(0));
        spt->runAction(Sequence::create(DelayTime::create(i*interval), FadeIn::create(interval), NULL));
        spt->runAction(Sequence::create(DelayTime::create(i*interval), EaseIn::create(MoveTo::create(dur, Point(x, y)), 3), CallFuncN::create(CC_CALLBACK_1(DualHudLayer::spriteMoveDone, this)), NULL));
    }
    this->scheduleOnce(schedule_selector(DualHudLayer::addEnergyDone), dur + interval*effectCount);
}
void DualHudLayer::addEnergyDone(float dt){
    setLife((int)lifeArray.size() + 1);
    float x = 10 + 47*(lifeArray.size() + 1);
    float y = size.height - 28;
    Sprite* spt = Sprite::create("hudHeart.png");
    this->addChild(spt);
    spt->setPosition(Point(x, y));
    float dur =0.5;
    spt->runAction(ScaleTo::create(dur, 3));
    spt->runAction(Sequence::create(FadeOut::create(dur), CallFuncN::create(CC_CALLBACK_1(DualHudLayer::spriteMoveDone, this)), NULL));
    GameManager::getInstance()->playSoundEffect(SOUND_MEDICINE);
    GameManager::getInstance()->getCurrentDualLayer()->player->energy = (int)lifeArray.size();
    GameManager::getInstance()->getCurrentDualLayer()->player->maxEnergy = (int)lifeArray.size();
}
void DualHudLayer::initializeSreen(int lifeCount)
{
    
    
 /*   bar = Sprite::create("blueBar.png");
    this->addChild(bar, -1);
    bar->setPosition(star3->getPosition());
    bar->setAnchorPoint(Point(0.5, 0));
    bar->setScale(2);
    
    bar = Sprite::create("blueBar.png");
    this->addChild(bar, -1);
    bar->setPosition(star3->getPosition());
    bar->setRotation(180);
    bar->setAnchorPoint(Point(0.5, 0));
    bar->setScale(2);*/
    if(GameManager::getInstance()->showVPad){
        btnA->setVisible(true);
        btnB->setVisible(true);
        btnLeftRight->setVisible(true);
    }
//    btnA->setVisible(false);
//    btnB->setVisible(false);
//    btnLeftRight->setVisible(false);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//    recordItem->setVisible(true);w
#endif

//    recordItem->setPosition(pauseItem->getPosition() + Point(-pauseItem->getContentSize().width - 15, 0));
}

void DualHudLayer::reduceLife()
{
    Sprite* sprite = (Sprite*)lifeArray.at(lifeArray.size()-1);
    lifeArray.eraseObject(sprite);
    sprite->removeFromParentAndCleanup(true);
}

void DualHudLayer::setWeapon(int weapon){
    Sprite* sptWeapon = Sprite::createWithSpriteFrameName(GameManager::getInstance()->getWeaponImageFileName(weapon));
    sptWeapon->setScale(0.5f);
    Node* checkBoard = hudLayer->getChildByName("sptCheckBoard");
    ((Sprite*)checkBoard->getChildByName("sptWeapon"))->setSpriteFrame(sptWeapon->getSpriteFrame());
    float recommendedWidth = checkBoard->getContentSize().width - 2;
    float recommendedScale = 1;
    if (recommendedWidth < sptWeapon->getContentSize().width*recommendedScale) {
        ((Sprite*)checkBoard->getChildByName("sptWeapon"))->setScale(recommendedWidth/sptWeapon->getContentSize().width);
    }else{
        ((Sprite*)checkBoard->getChildByName("sptWeapon"))->setScale(recommendedScale);
    }
    
}
void DualHudLayer::setTotalBulletCount(int count, int maxCount){
    Node* checkBoard = hudLayer->getChildByName("sptCheckBoard");
    TextBMFont* lblCount = (TextBMFont*)checkBoard->getChildByName("lblBulletLeft");
    if (maxCount < 0) {
        lblCount->setString("00");
    }else{
        lblCount->setString(__String::createWithFormat("%d/%d", count, maxCount)->getCString());
    }
}
void DualHudLayer::reduceBulletCount(){
    if (bulletArray.size() <= 0) {
        return;
    }
    Sprite* sptBullet = bulletArray.at(bulletArray.size() - 1);
    bulletArray.eraseObject(sptBullet);
    sptBullet->removeFromParentAndCleanup(true);
}
void DualHudLayer::setBulletCount(int count, int maxCount){
    Node* checkBoard = hudLayer->getChildByName("sptCheckBoard");
    int bulletCountInColumn = 5;
    if (maxCount > 20) {
        bulletCountInColumn = 10;
    }
    if (bulletArray.size() > 0) {
        for (int i = 0; i < bulletArray.size(); i++) {
            Sprite* sptBullet = bulletArray.at(i);
            sptBullet->removeFromParentAndCleanup(true);
        }
        bulletArray.clear();
    }
    int countLeft = count;
    int counter = 0;
    float x = checkBoard->getPosition().x + checkBoard->getContentSize().width*checkBoard->getScale()/2 + 2;
    float y = checkBoard->getPosition().y + checkBoard->getContentSize().height*checkBoard->getScale()/2;
    while(countLeft > 0){
        Sprite* sptBullet = Sprite::createWithSpriteFrameName("bulletPi.png");
        sptBullet->setAnchorPoint(Point(0, 1));
        sptBullet->setPosition(x, y);
        sptBullet->setScale((checkBoard->getContentSize().height*checkBoard->getScale() - 1*bulletCountInColumn)/(sptBullet->getContentSize().height*bulletCountInColumn));
        hudLayer->addChild(sptBullet);
        bulletArray.pushBack(sptBullet);
        countLeft--;
        counter++;
        y -= sptBullet->getContentSize().height*sptBullet->getScale() + 1;
        if (counter >= bulletCountInColumn) {
            x += sptBullet->getContentSize().width*sptBullet->getScale() + 2;
            y = checkBoard->getPosition().y + checkBoard->getContentSize().height*checkBoard->getScale()/2;
            counter = 0;
        }
    }
}
void DualHudLayer::setLife(int eng)
{
    TextBMFont* lbl = (TextBMFont*)hudLayer->getChildByName("lblHeart");
    if (eng > 10) {
        lbl->setVisible(true);
        for (int i = 1; i < 5; i++) {
            hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", i)->getCString())->setVisible(false);
        }
        ((ImageView*)hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", 0)->getCString()))->loadTexture("heart.png");
        lbl->setString(__String::createWithFormat("x%d", eng)->getCString());
    }else{
        
        lbl->setVisible(false);
        int fullHeatCount = eng/2;
        bool halfExist = eng%2 == 1;
        for (int i = 0; i < fullHeatCount; i++) {
            ((ImageView*)hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", i)->getCString()))->loadTexture("heart.png");
        }
        for (int i = 0; i < 5; i++) {
            hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", i)->getCString())->setVisible(i <= fullHeatCount);
        }
        if (halfExist) {
            ((ImageView*)hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", fullHeatCount)->getCString()))->loadTexture("heartHalf.png");
            hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", fullHeatCount)->getCString())->setVisible(true);
        }
    }
    
/*    CCLOG("set life: %d", eng);
    for(auto life:lifeArray)
    {
        life->removeFromParentAndCleanup(true);
    }
//    lifeArray.clear();
    
    float x = -10;
    float starY = size.height - 31;
    for (int i = 0; i < eng; i++) {
        Sprite* heart = Sprite::create("hudHeart.png");
        x += 10 + heart->getContentSize().width;
        heart->setPosition(Point(x, starY));
        this->addChild(heart);
        lifeArray.pushBack(heart);
        if (false && i == 3) {
            x = 0;
        }
    }*/
}

void DualHudLayer::keyCollected(int count)
{
    lblKeyCollected->setString(__String::createWithFormat("x %d", count)->getCString());
    GameManager::getInstance()->showParticleExplosion(this, "blueKey.png",lblKeyCollected->getPosition(), 2);
}
void DualHudLayer::starCollected(int count)
{
    Point pos;
    
    
    GameManager::getInstance()->showParticleExplosion(this, "goldStar.png", pos, 2);

    starCount = count;
}


void DualHudLayer::starCollectedFancy(int count, Point collectedPos)
{
    
}

void DualHudLayer::starChange(float dt)
{
    
}

void DualHudLayer::spriteMoveDone(Node* sprite)
{
    sprite->removeFromParentAndCleanup(true);
}

void DualHudLayer::resumeGame(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(pauseLayer == NULL) return;
    
    blackScreen->setVisible(false);
    
    if (pauseLayer){
        pauseLayer->removeFromParentAndCleanup(true);
        pauseLayer = NULL;
    }
    
    GameManager::getInstance()->isInMiddleOfGame = true;
//    NativeInterface::NativeInterface::destroyAds();
    
    GameManager::getInstance()->getCurrentDualLayer()->resumeLayer();
    this->enableButtons(true);
}
void DualHudLayer::showPauseLayer()
{
    recordItem->setVisible(false);
    
    if (isRecording) {
        isRecording = false;
        if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
            return;
        }
//        NativeInterface::NativeInterface::stopRecording();
    }
    
    if(pauseLayer != NULL) return;
    GameManager::getInstance()->firePressed = false;
    
    blackScreen->setVisible(true);
    
    pauseLayer = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("LegendDary_UI_Pause.json"));
    pauseLayer->setPosition(Point(size.width/2 - pauseLayer->getContentSize().width/2, 0));
    this->addChild(pauseLayer);
    
    Button* btnResume = dynamic_cast<Button*>(Helper::seekWidgetByName(pauseLayer, "Button_14"));
    btnResume->addTouchEventListener(CC_CALLBACK_2(DualHudLayer::resumeGame, this));
    
    Button* btnRestart = dynamic_cast<Button*>(Helper::seekWidgetByName(pauseLayer, "Button_14_0"));
    btnRestart->addTouchEventListener(CC_CALLBACK_2(DualHudLayer::retryStage, this));
    
    Button* btnStages = dynamic_cast<Button*>(Helper::seekWidgetByName(pauseLayer, "Button_14_1"));
    btnStages->addTouchEventListener(CC_CALLBACK_2(DualHudLayer::toStageSelect, this));
    
//    NativeInterface::NativeInterface::showRectAds();
    GameManager::getInstance()->isInMiddleOfGame = false;
    
    GameManager::getInstance()->saveCoin();
    
    enableButtons(false);

    GameManager::getInstance()->getCurrentDualLayer()->pauseLayer();
}

void DualHudLayer::showShopLayer()
{
//    enableButtons(false);
//    Layer* layer = GameManager::getInstance()->getShopLayer();
////    ((ShopLayer*)layer)->updateCoin();
//    if (layer->getParent()) {
//        layer->removeFromParentAndCleanup(false);
//    }
//    this->addChild(layer);
//
////    Director::getInstance()->pause();
//    GameManager::getInstance()->getCurrentDualLayer()->pauseLayer();
}

void DualHudLayer::showGameOverLayer(float dt)
{
    if (isRecording) {
        isRecording = false;
//        NativeInterface::NativeInterface::stopRecording();
    }
    
    gameOverLayer = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("LegendDary_UI_GameOver.json"));
    gameOverLayer->setPosition(Point(size.width/2 - gameOverLayer->getContentSize().width/2, 0));
    GameManager::getInstance()->animateFadeIn(gameOverLayer, this);
    
    Button* btnHome = dynamic_cast<Button*>(Helper::seekWidgetByName(gameOverLayer, "btnHome"));
    btnHome->addTouchEventListener(CC_CALLBACK_2(DualHudLayer::toMain, this));
    
    Button* btnRetry = dynamic_cast<Button*>(Helper::seekWidgetByName(gameOverLayer, "btnRetry"));
    btnRetry->addTouchEventListener(CC_CALLBACK_2(DualHudLayer::retryStage, this));
    
    Button* btnToStages = dynamic_cast<Button*>(Helper::seekWidgetByName(gameOverLayer, "btnToStages"));
    btnToStages->addTouchEventListener(CC_CALLBACK_2(DualHudLayer::toStageSelect, this));

    int moveY = 50;
    ImageView* sptGameOver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(gameOverLayer, "sptGameOver"));
    sptGameOver->setOpacity(10);
    sptGameOver->setPosition(sptGameOver->getPosition() + Point(0, -moveY));
    
    float duration = 1.5f;
    sptGameOver->runAction(MoveBy::create(duration, Point(0, moveY)));
    sptGameOver->runAction(FadeIn::create(duration));
    
    // save infos
    GameManager::getInstance()->addCoin(coinCount);
//    ((TitleLayer*)GameManager::getInstance()->titleLayer)->topBar->updateLabels();
    GameManager::getInstance()->saveCoin();
}
void DualHudLayer::showStageClearLayer(int coin, int enemyKilled, int enemyMax){
   
    
}
void DualHudLayer::onFreeCoin(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
        MyMessageBox::getInstance()->showDialog(this->getParent(), "NOT AVAILABLE");
        return;
    }
    
    GameManager::getInstance()->showVideo(VIDEO_GAME_OVER);
}
void DualHudLayer::videoDone(){
    if(GameManager::getInstance()->getHudLayer()){
        if (GameManager::getInstance()->videoIndex == VIDEO_GAME_OVER) {
            GameManager::getInstance()->getHudLayer()->scheduleOnce(schedule_selector(DualHudLayer::closeVideoDone), 0.05);
        }else if(GameManager::getInstance()->videoIndex == VIDEO_WEAPON_RENT){
            messageBox->removeFromParent();
        }
    }
}
void DualHudLayer::videoFailed(){
    if(GameManager::getInstance()->getHudLayer()){
        GameManager::getInstance()->getHudLayer()->scheduleOnce(schedule_selector(DualHudLayer::closeVideoFailed), 0.1);
    }
}
void DualHudLayer::closeVideoDone(float dt){
    if (btnFreeCoin == NULL) {
        return;
    }
    Sprite* sptCoin = Sprite::create("coin.png");
    btnFreeCoin->getParent()->addChild(sptCoin);
    sptCoin->setPosition(btnFreeCoin->getPosition() + Point(-40, 0));
    
    Label* lbl = Label::createWithTTF(__String::createWithFormat("+%d", coinCount)->getCString(), GameManager::getInstance()->getFont(FONT_BITDUST_ONE), 30);
    sptCoin->addChild(lbl);
    lbl->setPosition(Point(45, 19));
    lbl->setAnchorPoint(Point(0, 0.5));
    
    lbl = Label::createWithTTF(__String::createWithFormat("EXP %d", expCount)->getCString(), GameManager::getInstance()->getFont(FONT_BITDUST_ONE), 30);
    sptCoin->addChild(lbl);
    lbl->setPosition(Point(55, -10));
    
    btnFreeCoin->removeFromParent();
    btnFreeCoin = NULL;
    GameManager::getInstance()->addCoin(coinCount);
    GameManager::getInstance()->addGameExp(expCount);
    GameManager::getInstance()->saveCoin();
}

void DualHudLayer::closeVideoFailed(float dt){
    if (btnFreeCoin == NULL) {
        return;
    }
//    Sprite* sptCoin = Sprite::create("coin.png");
//    btnFreeCoin->getParent()->addChild(sptCoin);
//    sptCoin->setPosition(btnFreeCoin->getPosition() + Point(-40, 0));
    
    Label* lbl = Label::createWithTTF("ADS FAILED", GameManager::getInstance()->getFont(FONT_BITDUST_ONE), 30);
    btnFreeCoin->getParent()->addChild(lbl);
//    sptCoin->addChild(lbl);
//    lbl->setPosition(Point(80, 19));
    lbl->setPosition(btnFreeCoin->getPosition());
    
    btnFreeCoin->removeFromParent();
    btnFreeCoin = NULL;
//    GameManager::getInstance()->addCoin(50);
//    GameManager::getInstance()->saveCoin();
    
    /*Label* lbl = Label::createWithTTF(LanguageManager::getInstance()->getText(STR_VIDEO_FAILED), GameManager::getInstance()->getFont(FONT_DEFAULT), 24);
    sptBackLight2->getParent()->addChild(lbl);
    lbl->setPosition(sptBackLight2->getPosition() + Point(80, 115));
    
    btnFreeCoin->removeFromParent();
    btnFreeCoin = NULL;*/
}

void DualHudLayer::shareFacebook(){
    this->scheduleOnce(schedule_selector(DualHudLayer::facebookSafe), 0.1);
}

void DualHudLayer::facebookSafe(float dt){
    const char* link = "http://www.facebook.com/FifteenSixKorea";
    if (GameManager::getInstance()->market == MARKET_PLAYSTORE_PAID ||
        GameManager::getInstance()->market == MARKET_NAVER_PAID ||
        GameManager::getInstance()->market == MARKET_TSTORE_PAID) {
        link = "https://play.google.com/store/apps/details?id=com.magmon.LegendDaryTwo";
    }
}
void DualHudLayer::playCoinSoundLater(){
    GameManager::getInstance()->playSoundEffect(SOUND_COIN);
}
void DualHudLayer::coinLabelScheduler(float dt){
    CCLOG("coinLabelScheduler");
    increasingCoinCount += coinCount/10;
    increasingCounter++;
    if (increasingCounter >= increasingCounterMax) {
        increasingCoinCount = coinCount;
        increasingCounter = 0;
        this->schedule(schedule_selector(DualHudLayer::enemyLabelScheduler), 0.05f);
        this->unschedule(schedule_selector(DualHudLayer::coinLabelScheduler));
    }
    if (UserDefault::getInstance()->getBoolForKey(KEY_REMOVE_ADS_GET, false)) {
        if (increasingCounter == 0) {
            lblCoinCount->setString(__String::createWithFormat("%d\n+%d", increasingCoinCount/2, increasingCoinCount/2)->getCString());
        }else{
            lblCoinCount->setString(Value(increasingCoinCount/2).asString());
        }
    }else{
        lblCoinCount->setString(Value(increasingCoinCount).asString());
    }
    GameManager::getInstance()->playSoundEffect(SOUND_FA);
}
void DualHudLayer::enemyLabelScheduler(float dt){
    increasingEnemyCount += enemyKilledCount/10.0f;
    increasingCounter++;
    char buf[20];
    if (increasingCounter >= increasingCounterMax) {
        increasingEnemyCount = enemyKilledCount;
        increasingCounter = 0;

        sprintf(buf, "%d/%d", (int)increasingEnemyCount, enemyMaxCount);
        lblEnemyCount->setString(buf);
        this->schedule(schedule_selector(DualHudLayer::expLabelScheduler), 0.05f);
        this->unschedule(schedule_selector(DualHudLayer::enemyLabelScheduler));
        return;
    }
    
    sprintf(buf, "%d/%d", (int)increasingEnemyCount, enemyMaxCount);
    lblEnemyCount->setString(buf);
    GameManager::getInstance()->playSoundEffect(SOUND_FA);
}
void DualHudLayer::expLabelScheduler(float dt){
    increasingExpCount += expCount/10;
    increasingCounter++;
    
    bool isDoubleExp = UserDefault::getInstance()->getBoolForKey(KEY_DOUBLE_EXP_GET, false);
    if (isDoubleExp) {
        lblExpCount->setString(Value(expCount/2).asString());
    }else{
        lblExpCount->setString(Value(expCount).asString());
    }
    pbExp->setPercent(increasingExpCount*100.0f/maxExpCount);
    
    if (increasingCounter >= increasingCounterMax) {
        increasingExpCount = expCount;
        increasingCounter = 0;
        if (isDoubleExp) {
            lblExpCount->setString(__String::createWithFormat("%d\n+%d", expCount/2, expCount/2)->getCString());
        }else{
            lblExpCount->setString(__String::createWithFormat("%d", expCount)->getCString());
        }
        pbExp->setPercent(increasingExpCount*100.0f/maxExpCount);
        this->unschedule(schedule_selector(DualHudLayer::expLabelScheduler));
        
        if (expCount >= maxExpCount) {
            sptMaxTalkBalloon->setPosition(sptMaxTalkBalloon->getPosition() + Point(0, 1000));
            sptMaxTalkBalloon->runAction(Sequence::create(ScaleTo::create(0.001, 0.5), ScaleTo::create(0.1, 1.1), ScaleTo::create(0.05, 1.0f), NULL));
        }
        
        btnNext->setVisible(true);
        btnNext->runAction(Sequence::create(ScaleTo::create(0.01, 0.5), ScaleTo::create(0.1, 1.1), ScaleTo::create(0.06, 1.0), NULL));
        btnRetry->setVisible(true);
        btnRetry->runAction(Sequence::create(ScaleTo::create(0.01, 0.5), ScaleTo::create(0.1, 1.1), ScaleTo::create(0.06, 1.0), NULL));
        btnHome->setVisible(true);
        btnHome->runAction(Sequence::create(ScaleTo::create(0.01, 0.5), ScaleTo::create(0.1, 1.1), ScaleTo::create(0.06, 1.0), NULL));
    }
    
    // sound
    GameManager::getInstance()->playSoundEffect(SOUND_FA);
}
void DualHudLayer::showGameContinueLayer()
{
    pauseItem->setVisible(false);
    reviveLayer = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("LegendDary_UI_Revive.json"));
    reviveLayer->setPosition(Point(size.width/2 - reviveLayer->getContentSize().width/2, 0));
    GameManager::getInstance()->animateFadeIn(reviveLayer, this);
    
    ImageView* kiwi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(reviveLayer, "sptKiwi"));
    kiwi->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.3), JumpBy::create(0.3, Point(0, 0), 20, 1), NULL)));
    ImageView* chus = dynamic_cast<ImageView*>(Helper::seekWidgetByName(reviveLayer, "sptChus"));
    chus->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.5), JumpBy::create(0.8, Point(0, 0), 50, 1), NULL)));
    
    ImageView* neptun = dynamic_cast<ImageView*>(Helper::seekWidgetByName(reviveLayer, "sptNeptun"));
    neptun->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.4), JumpBy::create(0.5, Point(0, 0), 30, 1), NULL)));
    
    Button* btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(reviveLayer, "btnClose"));
    btnClose->addTouchEventListener(CC_CALLBACK_2(DualHudLayer::closeReviveLayer, this));
    
    Button* btnRevive = dynamic_cast<Button*>(Helper::seekWidgetByName(reviveLayer, "btnContinue"));
    btnRevive->addTouchEventListener(CC_CALLBACK_2(DualHudLayer::tryGameContinue, this));
    
    
    int count = reviveCount;
    Text* lblCount = dynamic_cast<Text*>(Helper::seekWidgetByName(reviveLayer, "lblGemCount"));
    if (count == 0) {
        ImageView* sptCoin = dynamic_cast<ImageView*>(Helper::seekWidgetByName(reviveLayer, "sptGem"));
        sptCoin->loadTexture("videoIcon.png");
        sptCoin->setPosition(sptCoin->getPosition() + Point(50, 0));
        lblCount->setVisible(false);
    }else{
        int price = count*100 + 500;
        if(price > 2000){
            price = 2000;
        }
        
        lblCount->setString(Value(price).asString());
    }
}

void DualHudLayer::closeReviveLayer(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
//    if(!GameManager::getInstance()->isPaidGame){
//        NativeInterface::NativeInterface::showChartboost();
//    }
    GameManager::getInstance()->isInMiddleOfGame = false;
    
    GameManager::getInstance()->animateFadeOut(reviveLayer);
    reviveLayer = NULL;
    this->scheduleOnce(schedule_selector(DualHudLayer::showGameOverLayer), 0.2);
}
void DualHudLayer::tryGameContinue(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;

    int count = reviveCount;
    if (count == 0) {
        GameManager::getInstance()->showVideo(VIDEO_GAME_CONTINUE);
        return;
    }
    int price = count*100 + 500;
    if(price > 2000){
        price = 2000;
    }
    if (GameManager::getInstance()->getCoin() >= price) {
        GameManager::getInstance()->playBGM(BGM_REVIVE);
        float duration = 3;
        ImageView* darySleep = dynamic_cast<ImageView*>(Helper::seekWidgetByName(reviveLayer, "sptDary"));
        darySleep->runAction(EaseOut::create(MoveBy::create(duration, Point(0, 50)), 3));
        darySleep->loadTexture("backLight.png");
        darySleep->runAction(RepeatForever::create(RotateBy::create(duration, 360)));
        
        Sprite* dary = Sprite::create("daryWithGun.png");
        darySleep->getParent()->addChild(dary);
        dary->setScale(4);
        dary->getTexture()->setAliasTexParameters();
        dary->setPosition(darySleep->getPosition());
        dary->runAction(EaseOut::create(MoveBy::create(duration, Point(0, 50)), 3));
        
        this->scheduleOnce(schedule_selector(DualHudLayer::reviveLater), duration*2/3);
        
        Button* btnRevive = dynamic_cast<Button*>(Helper::seekWidgetByName(reviveLayer, "btnContinue"));
        btnRevive->removeFromParentAndCleanup(true);
        Button* btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(reviveLayer, "btnClose"));
        btnClose->removeFromParentAndCleanup(true);
    }else{
        CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(DualHudLayer::recommandCoinShopClosed, this));
        MyMessageBox::getInstance()->showDialogWithCloseBox(this, call, LanguageManager::getInstance()->getText("no").c_str(), LanguageManager::getInstance()->getText("ok").c_str());
    }
}

void DualHudLayer::recommandCoinShopClosed(Node* sender){
    if(this->getTag() == MESSAGE_BOX_RESULT_OK){
        
    }
}
void DualHudLayer::notEnoughGemClosed(Ref* obj){
    if(this->getTag() == MESSAGE_BOX_RESULT_OK){
//        IAPManager::getInstance()->purchase(3);
        MyMessageBox::getInstance()->showDialog(this, "WAIT...");
    }
}
void DualHudLayer::closeWaitMessageBoxLater(){
    this->scheduleOnce(schedule_selector(DualHudLayer::closeWaitMessageBox), 0.01);
}
void DualHudLayer::closeWaitMessageBox(float dt){
    if(MyMessageBox::getInstance()->messageBox){
        MyMessageBox::getInstance()->messageBox->removeFromParentAndCleanup(true);
        MyMessageBox::getInstance()->messageBox = NULL;
    }
}
void DualHudLayer::closeWaitMessageBoxLaterAndShowSucess(){
    this->scheduleOnce(schedule_selector(DualHudLayer::closeWaitMessageBoxAndShowSuccess), 0.01);
}
void DualHudLayer::closeWaitMessageBoxAndShowSuccess(float dt){
    if(MyMessageBox::getInstance()->messageBox){
        MyMessageBox::getInstance()->messageBox->removeFromParentAndCleanup(true);
        MyMessageBox::getInstance()->messageBox = NULL;
    }
//    MyMessageBox::getInstance()->showDialog(this, "COIN PURCHASED!");
}
void DualHudLayer::reviveFromVideo(){
    blackScreen->setVisible(false);
    GameManager::getInstance()->animateFadeOut(reviveLayer);
    reviveLayer = NULL;
    pauseItem->setVisible(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    recordItem->setVisible(true);
#endif
    this->scheduleOnce(schedule_selector(DualHudLayer::playBackgroundMusicLater), 2);
    GameManager::getInstance()->getCurrentDualLayer()->revive();
    
    int count = reviveCount;
    count++;
    reviveCount = count;
    GameManager::getInstance()->saveCoin();
}
void DualHudLayer::reviveLater(float dt){
    blackScreen->setVisible(false);
    GameManager::getInstance()->animateFadeOut(reviveLayer);
    reviveLayer = NULL;
    pauseItem->setVisible(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    recordItem->setVisible(true);
#endif
    this->scheduleOnce(schedule_selector(DualHudLayer::playBackgroundMusicLater), 2);
    GameManager::getInstance()->getCurrentDualLayer()->revive();
    
    int count = reviveCount;
    int price = count*100 + 500;
    if(price > 2000){
        price = 2000;
    }
    GameManager::getInstance()->addCoin(-price);
    count++;
    reviveCount = count;
    GameManager::getInstance()->saveCoin();

}
void DualHudLayer::playBackgroundMusicLater(float dt){
    
}
void DualHudLayer::buyGemClosed(Ref* obj){
    if(this->getTag() == MESSAGE_BOX_RESULT_BUTTON_2){
        // buy gem
        CCLOG("buy gem!");
    }else{
        
    }
}
void DualHudLayer::removeUsedAssets(){
    GameManager::getInstance()->getCurrentDualLayer()->removeUsedAssets();
    if(true){//GameManager::getInstance()->showVPad){
        btnA->removeUsedAssets();
        btnB->removeUsedAssets();
        btnLeftRight->removeUsedAssets();
    }
    _eventDispatcher->removeEventListener(touchListener);
    _eventDispatcher->removeEventListener(listener);
    /*if (gameOverLayer) {
        gameOverLayer->removeFromParentAndCleanup(true);
    }
    if (reviveLayer) {
        reviveLayer->removeFromParentAndCleanup(true);
    }
    if (pauseLayer) {
        pauseLayer->removeFromParentAndCleanup(true);
    }
    if (gameClearLayer) {
        gameClearLayer->removeFromParentAndCleanup(true);
    }
    this->removeFromParentAndCleanup(true);
    
    GameManager::getInstance()->getCurrentDualLayer()->removeUsedAssets();*/
}
void DualHudLayer::toMain(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    SM->getHttpTime();
    GameManager::getInstance()->getCurrentDualLayer()->saveCoinIfStarCollected();
    whereToGo = CLOSE_TO_MAIN;
    this->scheduleOnce(schedule_selector(DualHudLayer::closeSchedule), 0.01);
}
void DualHudLayer::toStageSelect(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    SM->getHttpTime();
    GameManager::getInstance()->getCurrentDualLayer()->saveCoinIfStarCollected();
    
    whereToGo = CLOSE_TO_STAGES;
    this->scheduleOnce(schedule_selector(DualHudLayer::closeSchedule), 0.01);
}
void DualHudLayer::closeSchedule(float dt){
//    Director::getInstance()->popScene();
    removeUsedAssets();
//    ((TitleLayer*)GameManager::getInstance()->titleLayer)->closeTo(whereToGo);
    int toWhat = whereToGo;
    if(toWhat == CLOSE_TO_NEXT){
        
    }else if(toWhat == CLOSE_TO_RETRY){
        Director::getInstance()->popScene();
//        int stage = GameManager::getInstance()->currentStageIndex;
//        ((TitleLayer*)GameManager::getInstance()->titleLayer)->stageTouched();
        
    }else if(toWhat == CLOSE_TO_STAGES){
        if (GameManager::getInstance()->market == MARKET_PLAYSTORE_FREE ||
            GameManager::getInstance()->market == MARKET_APPSTORE_FREE) {
            GameManager::getInstance()->showInterstitialAds();
        }
        
        
    }else if(toWhat == CLOSE_TO_MAIN){
        GameManager::getInstance()->page = PAGE_TITLE;
        
        Title* title = Title::create();
        Scene* scene = Scene::create();
        scene->addChild(title);
        Director::getInstance()->replaceScene(scene);
//        ((TitleLayer*)GameManager::getInstance()->titleLayer)->topBar->updateLabels();
//        ((TitleLayer*)GameManager::getInstance()->titleLayer)->onBackFromStageSelect(NULL, Widget::TouchEventType::ENDED);
//        ((TitleLayer*)GameManager::getInstance()->titleLayer)->onBackFromThemeSelect(NULL, Widget::TouchEventType::ENDED);
    }
}
void DualHudLayer::nextStage(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    if (GameManager::getInstance()->theme == 3 && GameManager::getInstance()->currentStageIndex == 23) {
        Scene* scene = Scene::create();
        scene->addChild(EndingCredit::create());
        Director::getInstance()->replaceScene(TransitionFade::create(3, scene));
        return;
    }
    if (GameManager::getInstance()->life > 0 || UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET, false)) {
        if (!UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET, false)) {
//            GameManager::getInstance()->life--;
//            if (GameManager::getInstance()->life == 4) {
//                UserDefault::getInstance()->setIntegerForKey(KEY_LIFE_SPENT_TIME, SM->getCurrentTime());
//            }
        }
        whereToGo = CLOSE_TO_NEXT;
        this->scheduleOnce(schedule_selector(DualHudLayer::closeSchedule), 0.01);
    }else{
        whereToGo = CLOSE_TO_STAGES;
        this->scheduleOnce(schedule_selector(DualHudLayer::closeSchedule), 0.01);
    }
}
void DualHudLayer::retryStage(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    GameManager::getInstance()->getCurrentDualLayer()->saveCoinIfStarCollected();
    whereToGo = CLOSE_TO_RETRY;
    this->scheduleOnce(schedule_selector(DualHudLayer::closeSchedule), 0.01);
}
void DualHudLayer::onReplayVideo(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
        MyMessageBox::getInstance()->showDialog(this->getParent(), "NOT AVAILABLE");
        return;
    }
    
}

void DualHudLayer::enableButtons(bool enb)
{
    btnLeftRight->enabled = enb;
    btnA->enabled = enb;
    btnB->enabled = enb;
//    enableLayerAndMenus(enb);
}

void DualHudLayer::displayBossEnergy(const char* name)
{
//    bossEnergyBackground = Sprite::create("dialogBox.png");
//    this->addChild(bossEnergyBackground);
//    bossEnergyBackground->setPosition(Point(size.width/2, size.height/2 - 60));
//    bossEnergyBackground->setScaleY(0.5);
    bossEnergyBarWidth = 600;
    
    bossName = Label::createWithTTF(name, GameManager::getInstance()->getFont(FONT_VISITOR), 30);
    bossName->setColor(Color3B(255, 255, 255));
    bossName->enableShadow();
//    bossName->getTexture()->setAliasTexParameters();
    this->addChild(bossName);
    bossName->setAnchorPoint(Point(0,0));
    bossName->setPosition(Point(size.width/6, size.height*2/3));
    
    
    
    bossEnergyBarBack = DrawNode::create();
    this->addChild(bossEnergyBarBack);
    
    //bossEnergyBarRed->setAnchorPoint(Point(0, 0.5f));
    //bossEnergyBarRed->setScaleX(bossEnergyBarWidth);
    //bossEnergyBarRed->setScaleY(2);
    
    bossEnergyBar = DrawNode::create();
    this->addChild(bossEnergyBar);
    //bossEnergyBarYellow->setPosition(Point(size.width/2 - bossEnergyBarWidth/2, size.height/2 + 180));
    //bossEnergyBarYellow->setAnchorPoint(Point(0, 0.5f));
    //bossEnergyBarYellow->setScaleX(bossEnergyBarWidth);
    //bossEnergyBarYellow->setScaleY(2);
}

void DualHudLayer::setBossEnergy(int percent)
{
    bossEnergyBarBack->clear();
    bossEnergyBar->clear();
    float width = size.width*4/6;
    float energyLeft = width*percent/100.0f;
    bossEnergyBarBack->drawSolidRect(Point(size.width/6 - 1, size.height*2/3 - 3), Point(size.width/6 + width + 1, size.height*2/3 + 3), Color4F(235.0f/255, 235.0f/255, 235.0f/255, 100/255.0f));
    Color4F color;
    if (percent < 20) {
        color = Color4F(235.0f/255, 50.0f/255, 50.0f/255, 255/255.0f);
    }else{
        color = Color4F(255.0f/255, 208.0f/255, 59.0f/255, 255/255.0f);
    }
    bossEnergyBar->drawSolidRect(Point(size.width/6, size.height*2/3 - 2), Point(size.width/6 + energyLeft, size.height*2/3 + 2), color);
    if (percent <= 0) {
        bossEnergyBar->setVisible(false);
        bossEnergyBarBack->setVisible(false);
    }
}
void DualHudLayer::removeListener(){
//    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
//    btnAction->removeListener();
//    btnWeapon->removeListener();
//    btnReload->removeListener();
//    btnJump->removeListener();
//    js->removeListener();
//    js2->removeListener();
}
void DualHudLayer::addListener(){
    return;
    touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event* evt){
        log("onTouchesBegan");
        auto last = touches.back();
//        chopad->TouchesBegan(touches, evt);
        // check if last touch point is in which button
    };
    touchListener->onTouchesCancelled = [&](const std::vector<Touch*>& touches, Event* evt){
        log("onTouchesCancelled");
//        chopad->TouchesCancelled(touches, evt);
    };
    touchListener->onTouchesEnded = [&](const std::vector<Touch*>& touches, Event* evt){
        log("onTouchesEnded");
//        chopad->TouchesEnded(touches, evt);
    };
    touchListener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event* evt){
        log("onTouchesMoved");
//        chopad->TouchesMoved(touches, evt);
    };
    getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, 1);
    /*touchListener = cocos2d::EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    
    touchListener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        Point p = touch->getLocation();
        Point location = p;
        GameManager::getInstance()->getCurrentDualLayer()->isTouchStarted = true;
        
        GameManager::getInstance()->getCurrentDualLayer()->touchStart = location;
        
        return true; // we did not consume this event, pass thru.
    };
    
    touchListener->onTouchMoved = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
        
        Point location = touch->getLocationInView();
        location = Director::getInstance()->convertToGL(location);
        
        if (!GameManager::getInstance()->getCurrentDualLayer()->isTouchStarted) {
            return;
        }
        int minUp = 100;
        int minDown = 30;
        int toDown = GameManager::getInstance()->getCurrentDualLayer()->touchStart.y - location.y;
        int toLeft = GameManager::getInstance()->getCurrentDualLayer()->touchStart.x - location.x;
        if (toLeft< 0) {
            toLeft = -toLeft;
        }
        
        if(toDown > toLeft && toDown > minDown){
            if (GameManager::getInstance()->getCurrentDualLayer()->player->oneWayCancelY == 0) {
                GameManager::getInstance()->getCurrentDualLayer()->getDown();
            }else if(!GameManager::getInstance()->getCurrentDualLayer()->player->tumbled &&
                     GameManager::getInstance()->getCurrentDualLayer()->player->jumped){
                GameManager::getInstance()->getCurrentDualLayer()->player->tumbled = true;
                GameManager::getInstance()->jumpPressed = true;
            }else if(GameManager::getInstance()->getCurrentDualLayer()->player->tumbled){
                GameManager::getInstance()->getCurrentDualLayer()->player->buttonReleased = true;
                GameManager::getInstance()->jumpPressed = true;
            }
        }else if(toDown < -toLeft && toDown < -minUp && GameManager::getInstance()->getCurrentDualLayer()->player->vehicleType != VEHICLE_NONE){
            GameManager::getInstance()->getCurrentDualLayer()->getOffVehicle();
        }
#endif
    };
    
    touchListener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        GameManager::getInstance()->getCurrentDualLayer()->isTouchStarted = false;
    };
    
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, 30);*/
}

void DualHudLayer::showDialog(const char* message, const char* btn1, const char* btn2)
{
    CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(DualHudLayer::messageBoxClosed, this));
    MyMessageBox::getInstance()->showDialog(this, call, message, btn1, btn2);
    GameManager::getInstance()->getHudLayer()->enableJoystick(false);

    this->setTouchEnabled(false);
    
    ((DualWorld*)GameManager::getInstance()->getCurrentDualLayer())->pauseLayer();
}

void DualHudLayer::messageBoxClosed(Node* node)
{
    ((DualWorld*)GameManager::getInstance()->getCurrentDualLayer())->resumeLayer();
	this->setTouchEnabled(true);
    //	_dialogBox->setVisible(false);
	
    GameManager::getInstance()->getHudLayer()->enableJoystick(true);
	
    if (this->getTag() == 0) {
//        Director::getInstance()->replaceScene(StageSelectScene::scene(GameManager::getInstance()->theme));
    }
}
void DualHudLayer::showMessageBox(int mBoxType, std::string message, std::string ok, bool showClose){
    messageBox = (Layout*)CSLoader::createNode("MessageBox.csb");
    cocostudio::timeline::ActionTimeline* action = CSLoader::createTimeline("MessageBox.csb");
    messageBox->runAction(action);
    messageBox->setVisible(false);
    action->gotoFrameAndPlay(0, true);
    messageBox->setScale(0.9f);
    
    this->addChild(messageBox, 9999);
    messageBox->setVisible(true);
    messageBox->setTag(mBoxType);
    messageBox->setAnchorPoint(Point(0.5, 0.5));
    messageBox->setPosition(Point(size.width/2 - messageBox->getContentSize().width/2 + size.width/2, size.height/2));
    messageBox->runAction(Sequence::create(ScaleTo::create(0.06f, 1.1), ScaleTo::create(0.05f, 1), NULL));
    
    Button* btnOk = (Button*)messageBox->getChildByName("btnOk");
    btnOk->addClickEventListener(CC_CALLBACK_0(DualHudLayer::onMessageBoxOk, this));
    btnOk->setTag(MESSAGE_BOX_RESULT_OK);
    
    Button* btnCancel = (Button*)messageBox->getChildByName("btnCancel");
    btnCancel->addClickEventListener(CC_CALLBACK_0(DualHudLayer::onMessageBoxCancel, this));
    btnCancel->setTag(MESSAGE_BOX_RESULT_CANCEL);
    
    if (!showClose) {
        btnCancel->setVisible(false);
        messageBox->getChildByName("lblX")->setVisible(false);
    }
    
    Text* lblOk = (Text*)messageBox->getChildByName("lblOk");
    LM->setLocalizedString(lblOk, ok);
    
    Text* lblMessage = (Text*)messageBox->getChildByName("lblMessage");
    LM->setLocalizedString(lblMessage, message);
}

void DualHudLayer::onMessageBoxOk(){
//    if (messageBox->getTag() == NN_MULTI_SEARCH) {
    if (messageBox->getTag() == 1) { // test 
        onBack();
    }
}
void DualHudLayer::onMessageBoxCancel(){
    messageBox->removeFromParent();
}
