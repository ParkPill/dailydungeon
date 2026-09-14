//
//  HudLayer.cpp
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 29..
//
//
#include "SceneChanger.h"
#include "FifteenSixLogo.h"
#include "ProfileLayer.h"
#include "GameSharing.h"
#include "ServerManager.h"
#include "HudLayer.h"
#include "LegendDaryButton.h"
#include "GameManager.h"
#include "HelloWorldScene.h"
#include "MyMessageBox.h"
#include "NativeInterface.h"
#include "LanguageManager.h"
#include "Title.h"
#include "EndingCredit.h"
#include "CursorLayer.h"
#include "ui/UITextBMFont.h"
#include "Firework.h"

bool HudLayer::init()
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
    listener->onKeyPressed = CC_CALLBACK_2(HudLayer::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(HudLayer::onKeyReleased, this);
    
    cursorLayer = CursorLayer::create();
    this->addChild(cursorLayer);
    
    hudLayer = CSLoader::createNode("HudLayer.csb");
    hudLayer->setContentSize(size);
    
    Button* btn = (Button*)hudLayer->getChildByName("btnProfile");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onProfileClick, this));
    btn->getChildByName("redDot")->setVisible(false);
    btn = (Button*)hudLayer->getChildByName("btnSetting");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onSettingClick, this));
    btn = (Button*)hudLayer->getChildByName("btnVideoStore");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onVideoStoreClick, this));
    
    btn = (Button*)hudLayer->getChildByName("btnTestLeft");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onTestLeftClick, this));
    btn = (Button*)hudLayer->getChildByName("btnTestRight");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onTestRightClick, this));
    btn = (Button*)hudLayer->getChildByName("btnTestUp");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onTestUpClick, this));
    btn = (Button*)hudLayer->getChildByName("btnTestDown");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onTestDownClick, this));
    
    
    ui::Helper::doLayout(hudLayer);
    this->addChild(hudLayer);
    btn = (Button*)hudLayer->getChildByName("btnCrystal");
//    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onGemShopClick, this));
    gemBar = btn;
    gemBar->removeFromParentAndCleanup(false);
    this->addChild(btn, POPUP_ZORDER + 100);
    btn = (Button*)hudLayer->getChildByName("btnMission");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onMissionClick, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "mission");
    btn = (Button*)hudLayer->getChildByName("btnShop");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onGemShopClick, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "shop");
    btn = (Button*)hudLayer->getChildByName("btnMenu");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onMenuClick, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "main menu");
    bool isSomethingNew = GM->isCollectionHasNew();
    btn->getChildByName("redDot")->setVisible(isSomethingNew);
    btn = (Button*)hudLayer->getChildByName("btnBattle");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onBattleClick, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "battle");
    
    updateProfile();
    
    lblKeyCollected = (TextBMFont*)hudLayer->getChildByName("lblKeyCollected");
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    if(GameManager::getInstance()->showVPad){
        Texture2D* leftRightTexture = Director::getInstance()->getTextureCache()->addImage("leftRight.png");
        Texture2D* aTexture = Director::getInstance()->getTextureCache()->addImage("btnA.png");
        Texture2D* bTexture = Director::getInstance()->getTextureCache()->addImage("btnB.png");

        btnLeftRight = LegendDaryButton::buttonWithTexture(leftRightTexture, BUTTON_LEFT_RIGHT);
        btnLeftRight->setPosition( Point(200, 80) );
        btnLeftRight->addListener();
        btnLeftRight->setVisible(false);
        buttonArray.pushBack( btnLeftRight);
        //    btnLeftRight->setOpacity(155);
//        addChild(btnLeftRight);
        
        btnA = LegendDaryButton::buttonWithTexture( aTexture, BUTTON_A );
        btnA->setPosition( Point(size.width - 320, 80) );
        btnA->addListener();
        btnA->setVisible(false);
        buttonArray.pushBack( btnA);
        //    btnA->setOpacity(155);
//        addChild(btnA);
        
        btnB = LegendDaryButton::buttonWithTexture( bTexture, BUTTON_B );
        btnB->setPosition( Point(size.width - 120, 80) );
        btnB->addListener();
        btnB->setVisible(false);
        buttonArray.pushBack( btnB);
        //    btnB->setOpacity(155);
//        addChild(btnB);
        
        float siz = GameManager::getInstance()->getButtonSize();
        setButtonSize(siz);
    }
    
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
    
    
    lblCoin = LanguageManager::getInstance()->getLocalizedLabel("0", Color4B::WHITE);
    this->addChild(lblCoin);
    lblCoin->setPosition(Point(size.width/2, size.height - lblCoin->getContentSize().height));
    lblCoin->setAnchorPoint(Point(0, 0.5));
    lblCoin->setVisible(false);
    
    lblTouch = Label::createWithTTF("TOUCH", GameManager::getInstance()->getFont(FONT_VISITOR), 30);
    this->addChild(lblTouch);
    lblTouch->setPosition(Point(size.width/2, size.height - lblCoin->getContentSize().height - lblTouch->getContentSize().height - 10));
    lblTouch->setOpacity(0);
    
    
//    Sprite* spt = Sprite::create("gem.png");
//    this->addChild(spt, lbl->getLocalZOrder());
//    spt->setPosition(Point(size.width/2 - 20, lbl->getPositionY()));
    
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

    pauseItem = MenuItemImage::create("pauseButton.png", "pauseButton.png", CC_CALLBACK_0(HudLayer::showPauseLayer, this));
    pauseItem->setVisible(false);
//    recordItem = MenuItemImage::create("recordButton.png", "recordButton.png", CC_CALLBACK_0(HudLayer::startRecord, this));
//    recordItem->setVisible(false);
//    bombItem = MenuItemImage::create("aButton.png", "aButton.png", CC_CALLBACK_0(HudLayer::useBomb, this));
//    bombItem->setPosition(Point(size.width - pauseItem->getContentSize().width - 5, size.height/2));
    
//    potionItem = MenuItemImage::create("aButton.png", "aButton.png", CC_CALLBACK_0(HudLayer::usePotion, this));
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
    
    GameManager::getInstance()->page = PAGE_STAGE;
    
    registerControllerListener();
    // lr
    joystickLR = SneakyJoystickSkinnedBase::create();
    Sprite* pBack = Sprite::create("UI/btnLR.png");
    joystickLR->setBackgroundSprite(pBack);
    Sprite* pThumb = Sprite::create("joystickBall.png");
    joystickLR->setThumbSprite(pThumb);
    joystickLR->setScale(4);
    pThumb->setVisible(false);
    //joystick->setContentSize(Size(200, 200));
    joystickLR->setTag(BUTTON_LEFT_RIGHT);
    
    js = SneakyJoystick::create();
    js->stickType = STICK_LEFT;
    js->sptBack = pBack;
    
    joystickLR->setBackgroundSprite(pBack);
    joystickLR->setThumbSprite(pThumb);
    joystickLR->setJoystick(js);
    joystickLRPos = Point(pBack->getContentSize().width*joystickLR->getScale(), pBack->getContentSize().height*joystickLR->getScale());
    joystickLR->setPosition(joystickLRPos + Point(-30, -40));
    this->addChild(joystickLR, 10);//move to addJoystickToParent()
    
    isForceTouchAvailable = false;
#if defined(__IPHONE_9_0) && (__IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_9_0)
    isForceTouchAvailable = true;
    if (UserDefault::getInstance()->getBoolForKey(KEY_FIRST_LAUNCH, true)) {
        UserDefault::getInstance()->setBoolForKey(KEY_FIRST_LAUNCH, false);
        UserDefault::getInstance()->setBoolForKey(KEY_AUTO_TARGET_SELECTED, false);
    }
#endif
//    isForceTouchAvailable = false; // test
    
    float yOffset = -10;
    btnToggle = VirtualPadButton::create(BUTTON_AUTO_TOGGLE);
    this->addChild(btnToggle);
    btnToggle->setPosition(Point(size.width - 225, 220 + yOffset));
    btnToggle->setScale(2.3);
    btns.pushBack(btnToggle);
//    btnToggle->setVisible(forceTouchPossible);
//    btnToggle->setVisible(false);
    
    // aim
    joystickAim = SneakyJoystickSkinnedBase::create();
    pBack = Sprite::create("joystickBack.png");
    joystickAim->setBackgroundSprite(pBack);
    pThumb = Sprite::create("joystickBall.png");
    joystickAim->setThumbSprite(pThumb);
    joystickAim->setScale(3);
    //joystick->setContentSize(Size(200, 200));
    
    js2 = SneakyJoystick::create();
    js2->stickType = STICK_RIGHT;
    joystickAim->setBackgroundSprite(pBack);
    joystickAim->setThumbSprite(pThumb);
    joystickAim->setJoystick(js2);
//    joystickAimPos = Point(size.width - pBack->getContentSize().width*joystickAim->getScale(), pBack->getContentSize().height*joystickAim->getScale());
    joystickAimPos = Point(size.width - 200, pBack->getContentSize().height*joystickAim->getScale());
    joystickAim->setPosition(joystickAimPos + Point(0, yOffset));
    this->addChild(joystickAim, 10);//move to addJoystickToParent()
    bool isAutoSelected = UserDefault::getInstance()->getBoolForKey(KEY_AUTO_TARGET_SELECTED, true);
//    js2->setVisible(forceTouchPossible || !isAutoSelected);
//    joystickAim->setVisible(forceTouchPossible || !isAutoSelected);
    js2->setVisible(true);// touch and aim to shoot
    joystickAim->setVisible(true);// touch and aim to shoot
    
    btnShoot = VirtualPadButton::create(BUTTON_SHOOT);
    this->addChild(btnShoot);
    btnShoot->setPosition(Point(size.width - 250, 100 + yOffset));
    btnShoot->setScale(3);
    btns.pushBack(btnShoot);
//    btnShoot->setVisible(!forceTouchPossible && isAutoSelected);
    btnShoot->setVisible(false); // touch and aim to shoot
    
    btnJump = VirtualPadButton::create(BUTTON_JUMP);
    this->addChild(btnJump);
//    btnJumpPos= Point(size.width - btnJump->getContentSize().width*btnJump->getScaleX()*2, btnJump->getContentSize().height*btnJump->getScaleY()*0.7);
    btnJump->setPosition(Point(size.width - 85, 100 + yOffset));
    btnJump->setScale(3);
    btns.pushBack(btnJump);
    
    btnReload = VirtualPadButton::create(BUTTON_RELOAD);
    this->addChild(btnReload);
//    btnReloadPos= Point(size.width - btnReload->getContentSize().width*btnReload->getScaleX()*2, btnReload->getContentSize().height*btnReload->getScaleY()*0.7);
    btnReloadPos = Point( 200, 200);
    btnReload->setPosition(Point(size.width - 45, 220 + yOffset));
    btnReload->setScale(2.3);
    btns.pushBack(btnReload);
    
    btnWeapon = VirtualPadButton::create(BUTTON_WEAPON);
    this->addChild(btnWeapon);
    btnWeaponPos= Point(size.width - btnWeapon->getContentSize().width*btnWeapon->getScaleX()*2, btnWeapon->getContentSize().height*btnWeapon->getScaleY()*0.7);
    btnWeapon->setPosition(Point(size.width - 135, 220 + yOffset));
    btnWeapon->setScale(2.3);
    btns.pushBack(btnWeapon);
    
    btnAction = VirtualPadButton::create(BUTTON_ACTION);
    this->addChild(btnAction);
    btnAction->setPosition(Point(size.width - 260, 230 + yOffset));
    btnAction->setScale(1.8);
    btnAction->setVisible(false);
    btns.pushBack(btnAction);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//    joystickAim->setPositionY(-2000);
//    hideBtns(); // test
#endif
    
    bool lastJewelCollected = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString());
    bool gameCleared = GameManager::getInstance()->isGameClear;
    if (lastJewelCollected && gameCleared) {
        hideBtns();
    }
//    onUpgradeClick(); // test
    updateGem(GM->getGem());
    this->schedule(schedule_selector(HudLayer::updateUI));
    
    // test
//    for(int i = 0; i < 8; i++){
//        GM->addBagContent(i, 0);
//    }
//    GM->addBagContent(RUNE_BEGIN, 0);
//    GM->addBagContent(SHIELD_BEGIN, 0);
//    GM->saveBagContent();
    
    // test
//    this->runAction(Sequence::create(DelayTime::create(0.1f), CallFunc::create(CC_CALLBACK_0(HudLayer::onTest, this)), nullptr));
//    GM->addGem(99999);// test
//    GM->addBagContent(WEAPON_MACHINE_GUN, 0); // test
//    GM->addBagContent(WEAPON_BAZOOKA, 0);
//    GM->addBagContent(WEAPON_NIFE_THROWER, 0);
//    GM->addBagContent(WEAPON_BALL_SHOOTER, 0);
//    GM->addBagContent(WEAPON_LASER_COIN, 0);
    
//    UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 1).c_str(), WEAPON_LASER_GUN); // test
    
    isGettingAllUser = true;
    
    
    
    
    handleRedDots();
//    if(GM->tutorialType == TUTORIAL_DUNGEON){
//        GM->tutorialType = TUTORIAL_MENU; // test
//        GM->tutorialIndex = 10; // test
//    }
    
    this->schedule(schedule_selector(HudLayer::update));
    
//    UDSetInt(strmake(KEY_ACHIEVE_FORMAT, 0).c_str(), 100); // test 
    
    // init done
    return true;
}
void HudLayer::adjustHandPos(Vec2 pos){
    if(WORLD->player->isFlippedX()){
        WORLD->player->flippedHandPos = WORLD->player->flippedHandPos + pos;
        log("center: %d, %d", (int)WORLD->player->flippedHandPos.x, (int)WORLD->player->flippedHandPos.y);
    }else{
        WORLD->player->handPos = WORLD->player->handPos + pos;
        log("center: %d, %d", (int)WORLD->player->handPos.x, (int)WORLD->player->handPos.y);
    }
    WORLD->player->stopAnimationAndStand();
    WORLD->player->setGunPos(WORLD->player->lastAngle);
}
void HudLayer::onTestLeftClick(){
    adjustHandPos(Vec2(-1, 0));
}
void HudLayer::onTestRightClick(){
    adjustHandPos(Vec2(1, 0));
}
void HudLayer::onTestUpClick(){
    adjustHandPos(Vec2(0, 1));
}
void HudLayer::onTestDownClick(){
    adjustHandPos(Vec2(0, -1));
}
void HudLayer::update(float dt){
    if (GM->iapFlag == IAP_FLAG_SUCCESS) {
        GM->consumeSkuNameList.push_back(GM->iapDetail);
        log("consumeSkuNameList push_back: %d", (int)GM->consumeSkuNameList.size());
        // set iapFlag = IAP_FLAG_INIT after giving rewards. See below.
    }
    if(GM->consumeSkuNameList.size() > 0){
        for (int i = 0; i < GM->consumeSkuNameList.size(); i++) {
            GM->iapDetail = GM->consumeSkuNameList.at(i);
            GM->consumeSkuNameList.erase(GM->consumeSkuNameList.begin() + i);
            break;
        }
        
        Node* layer = this->getChildByName("shopPopup");
        if(layer == nullptr){
            onGemShopClick();
        }
        std::string strExtra = "";
        if (GM->iapDetail.compare(IAP_DETAIL_STARTER1) == 0) {
            ScrollView* sv = (ScrollView*)layer->getChildByName("imgBackground")->getChildByName("sv");
            Button* btn = (Button*)sv->getChildByName(strmake("btnStarter%d", 0));
            ImageView* img = (ImageView*)((Widget*)layer->getChildByName("imgPurchased"))->clone();
            btn->addChild(img, 100);
            img->setPosition(Vec2(46, 57));
            btn->setEnabled(false);
            img->setScale(1);
            UDSetBool(KEY_REMOVE_ADS_GET, true);
            std::vector<int> array;
            for (int i = 0; i < 5; i++) {
                array.push_back(getRandomWeapon(ITEM_RARENESS_3STAR));
            }
            showItemGetAndAddToBag(array);
        }else if (GM->iapDetail.compare(IAP_DETAIL_STARTER2) == 0) {
            ScrollView* sv = (ScrollView*)layer->getChildByName("imgBackground")->getChildByName("sv");
            Button* btn = (Button*)sv->getChildByName(strmake("btnStarter%d", 1));
            ImageView* img = (ImageView*)((Widget*)layer->getChildByName("imgPurchased"))->clone();
            btn->addChild(img, 100);
            img->setPosition(Vec2(46, 57));
            btn->setEnabled(false);
            img->setScale(1);
            GM->addGem(500);
            std::vector<int> array;
            for (int i = 0; i < 5; i++) {
                array.push_back(getRandomWeapon(ITEM_RARENESS_3STAR));
            }
            showItemGetAndAddToBag(array);
        }else if (GM->iapDetail.compare(IAP_DETAIL_STARTER3) == 0) {
            ScrollView* sv = (ScrollView*)layer->getChildByName("imgBackground")->getChildByName("sv");
            Button* btn = (Button*)sv->getChildByName(strmake("btnStarter%d", 2));
            ImageView* img = (ImageView*)((Widget*)layer->getChildByName("imgPurchased"))->clone();
            btn->addChild(img, 100);
            img->setPosition(Vec2(46, 57));
            btn->setEnabled(false);
            img->setScale(1);
            GM->addGem(3000);
            std::vector<int> array;
            for (int i = 0; i < 5; i++) {
                array.push_back(getRandomWeapon(ITEM_RARENESS_4STAR));
            }
            showItemGetAndAddToBag(array);
        }else if (GM->iapDetail.compare(IAP_DETAIL_PACKAGE1) == 0) {
            GM->addGem(500);
            std::vector<int> array;
            for (int i = 0; i < 5; i++) {
                array.push_back(getRandomWeapon(ITEM_RARENESS_3STAR));
                array.push_back(getRandomShield(ITEM_RARENESS_3STAR));
                array.push_back(getRandomRune(ITEM_RARENESS_2STAR));
            }
            showItemGetAndAddToBag(array);
        }else if (GM->iapDetail.compare(IAP_DETAIL_PACKAGE2) == 0) {
            GM->addGem(3000);
            std::vector<int> array;
            for (int i = 0; i < 5; i++) {
                array.push_back(getRandomWeapon(ITEM_RARENESS_4STAR));
                array.push_back(getRandomShield(ITEM_RARENESS_4STAR));
                array.push_back(getRandomRune(ITEM_RARENESS_3STAR));
            }
            showItemGetAndAddToBag(array);
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM1) == 0) { // MONTHLY
            GM->addGem(300);
            int endDay = UDGetInt(KEY_MONTHLY_GEM_END_DATE, 0);
            int today = SM->getCurrentTime()/86400;
            if(endDay < today){
                endDay = today;
            }
            endDay += 30;
            
            strExtra = strmake("&monthly_gem_end_date=%d&monthly_gem_last_receive_date=0&monthly_gem_instant_receive=0", endDay);
            
            Button* btn = (Button*)this->getChildByName("shopPopup")->getChildByName("imgBackground")->getChildByName("sv")->getChildByName("btnMonthlyGem");
            if(endDay >= today){
                Text* lbl = (Text*)btn->getChildByName("lblDesc");
                if (endDay - today < 5) {
                    lbl->setString(strmake("%s: %d\n%s", LM->getText("day left").c_str(), endDay - today, LM->getText("almost expire buy again").c_str()));
                }else{
                    lbl->setString(strmake("%s: %d", LM->getText("day left").c_str(), endDay - today));
                }
            }
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM2) == 0) {
            GM->addGem(110);
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM3) == 0) {
            GM->addGem(360);
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM4) == 0) {
            GM->addGem(1300);
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM5) == 0) {
            GM->addGem(4500);
        }else if (GM->iapDetail.compare(IAP_DETAIL_MONTHLY_TICKET) == 0) { // monthly
            GM->localTicketCount += 30;
            GM->networkTicketCount += 30;
            int endDay = UDGetInt(KEY_MONTHLY_TICKET_END_DATE, 0);
            int today = SM->getCurrentTime()/86400;
            if(endDay < today){
                endDay = today;
            }
            endDay += 30;
            Button* btn = (Button*)this->getChildByName("shopPopup")->getChildByName("imgBackground")->getChildByName("sv")->getChildByName("btnMonthlyTicket");
            if(endDay >= today){
                Text* lbl = (Text*)btn->getChildByName("lblDesc2");
                if (endDay - today < 5) {
                    lbl->setString(strmake("%s: %d\n%s", LM->getText("day left").c_str(), endDay - today, LM->getText("almost expire buy again").c_str()));
                }else{
                    lbl->setString(strmake("%s: %d", LM->getText("day left").c_str(), endDay - today));
                }
            }
            strExtra = strmake("&monthly_ticket_end_date=%d&monthly_ticket_last_receive_date=0&monthly_ticket_instant_receive=0", endDay);
        }
        GM->iapList += "_" + GM->iapDetail;
        SM->saveUserData(strmake("gem=%d&iap_list=%s%s", GM->getGem(), GM->iapList.c_str(), strExtra.c_str()));
        updateGem(GM->getGem());
        hideIndicator();
    }else if(GM->iapFlag == IAP_FLAG_FAILED) {
        GM->iapFlag = IAP_FLAG_INIT;
        hideIndicator();
    }
    // consume after give reward
    if(GM->iapFlag == IAP_FLAG_SUCCESS){
        GM->iapFlag = IAP_FLAG_INIT;
        GameSharing::consumePurchased();
    }
}
void HudLayer::onVideoStoreClick(){
    Node* layer = CSLoader::createNode("VideoStore.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("videoStore");
    Button* btn;
    setTitle(layer->getChildByName("titleBack"), "video store");
    setCloseButtonAndBlock(layer);
    layer->setTag(0);
    
    for(int i = 0; i < 3; i++){
        btn = (Button*)layer->getChildByName(strmake("btnTab%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onVideoStoreTabClick, this));
    }
    onVideoStoreTabClick(layer->getChildByName("btnTab0"));
    
    for (int i = 0; i < 12; i++) {
        btn = (Button*)layer->getChildByName(strmake("btn%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onVideoStoreItemClick, this));
    }
}

void HudLayer::onVideoStoreTabClick(Ref* ref){
    BTN_FROM_REF
    int tab = btn->getTag();
    Node* layer = btn->getParent();
    for (int i = 0; i < 3; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnTab%d", i));
        btn->loadTextureNormal(i == tab?"btnBaseHighlight.png":"btnBase.png");
    }
    selectedVideoStoreTabIndex = tab;
    Text* lbl;ImageView* img;float delay = 0;
    for (int i = 0; i < 12; i++) {
        btn = (Button*)layer->getChildByName(strmake("btn%d", i));
        lbl = (Text*)btn->getChildByName("lbl");
        
        img = (ImageView*)btn->getChildByName("Image_1");
        if(i == 11 || i == 5){
            if(tab == 0){
                LM->setLocalizedString(lbl, "weapon");
            }else if(tab == 1){
                LM->setLocalizedString(lbl, "shield");
            }else if(tab == 2){
                lbl->setString("100");
            }
            img->setColor(Color3B::BLACK);
            lbl = (Text*)btn->getChildByName("lblQuestion");
            lbl->setVisible(true);
            if(tab == 0){
                img->loadTexture("machineGun.png");
                img->setContentSize(Size(13, 9));
            }else if(tab == 1){
                img->loadTexture("shield.png");
                img->setContentSize(Size(11.5f, 13));
            }else if(tab == 2){
                img->loadTexture("crystal.png");
                img->setContentSize(Size(9, 11));
                img->setColor(Color3B::WHITE);
                lbl->setVisible(false);
            }
        }
        if(i > 5){
            btn->setPosition(Vec2(201.64f + 143.93f*(i - 6), 201.27f));
        }else{
            btn->setPosition(Vec2(201.64f + 143.93f*i, 313.44f));
        }
        btn->runAction(Sequence::create(DelayTime::create(delay), JumpBy::create(0.2f, Vec2(0, 0), 16, 1), NULL));
        btn->runAction(Sequence::create(DelayTime::create(delay), EaseOut::create(RotateBy::create(0.2f, 360), 2), NULL));
        delay += 0.015f;
    }
    updateUI(0);
}
void HudLayer::onVideoStoreItemClick(Ref* ref){
    BTN_FROM_REF
    
    if(isNoAdsPurchased || GM->developer){
        GM->videoIndex = VIDEO_VIDEOSTORE;
        videoDone();
        return;
    }else if(!GM->isVideoReady()){ // is video ready
        showInstanceMessage(LM->getText("ads not ready try later"));
        return;
    }
    
    int index = btn->getTag();
    selectedVideoStoreItemIndex = index;
    if(selectedVideoStoreTabIndex == 2 || index < 5 || (index > 5 && index < 12)){
        Text* lbl = (Text*)btn->getChildByName("lbl");
        selectedVideoStoreItemCount = Value(lbl->getString()).asInt();
    }
//    double watchedTime = UDGetDouble(strmake(KEY_VIDEO_STORE_WATCHED_TIME_FORMAT, selectedVideoStoreTabIndex).c_str(), 0);
//    int nextIndex = UDGetInt(strmake(KEY_VIDEO_STORE_RECEIVED_INDEX_FORMAT, selectedVideoStoreTabIndex).c_str(), -1) + 1;
    double watchedTime = 0;
    int nextIndex = 0;
    if (selectedVideoStoreTabIndex == 0) {
        nextIndex = GM->videoStore0Index + 1;
        watchedTime = GM->videoStore0WatchedTime;
    }else if (selectedVideoStoreTabIndex == 1) {
        nextIndex = GM->videoStore1Index + 1;
        watchedTime = GM->videoStore1WatchedTime;
    }else if (selectedVideoStoreTabIndex == 2) {
        nextIndex = GM->videoStore2Index + 1;
        watchedTime = GM->videoStore2WatchedTime;
    }
    double now = SM->getCurrentTime();
    if(videoStoreWaitForNextVideoTime - (now - watchedTime) > 0 || !GM->isVideoReady()){
        showInstanceMessage(LM->getText("ads not ready try later"));
        return;
    }
    
    GM->showVideo(VIDEO_VIDEOSTORE);
    
//    GM->showVideoDone(); // test
}
void HudLayer::handleRedDots(){
    Button* btn = (Button*)hudLayer->getChildByName("btnShop");
    double availbleTime = GM->videoShopWatchedTime + 60*15;//UDGetDouble(KEY_VIDEO_NEXT_AVAILABLE_TIME, -1);
    double now = SM->getCurrentTime();
//    btn->getChildByName("redDot")->setVisible(UDGetInt(KEY_VIDEO_ADS_LEFT, 10) > 0 && availbleTime < now);
    btn->getChildByName("redDot")->setVisible(10 - GM->videoShopIndex > 0 && availbleTime < now);
}
void HudLayer::onGemShopClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("Shop.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("shopPopup");
    Button* btn;Text* lbl;
    setTitle(layer->getChildByName("titleBack"), "shop");
    setCloseButtonAndBlock(layer);
    layer->setTag(0);
    Node* background = layer->getChildByName("imgBackground");
    ScrollView* sv = (ScrollView*)background->getChildByName("sv");
    for (int i = 0; i < 5; i++) {
        btn = (Button*)background->getChildByName(strmake("btnTab%d", i));
        btn->loadTextureNormal(i == layer->getTag()?"btnBaseHighlight.png":"btnBase.png");
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onGemShopTabClick, this));
        lbl = (Text*)btn->getChildByName("lbl");
        if(i == 0){
            LM->setLocalizedString(lbl, "starter pack");
        }else if(i == 1){
            LM->setLocalizedString(lbl, "package product");
        }else if(i == 2){
            LM->setLocalizedString(lbl, "gem");
        }else if(i == 3){
            LM->setLocalizedString(lbl, "ticket");
        }else if(i == 4){
            LM->setLocalizedString(lbl, "free");
        }
    }
    float x = 0.61f;
    float gapX = 95;
    float y = 127.46f;
    // starter
    std::string strDesc;
    for (int i = 0; i < 3; i++) {
        btn = (Button*)sv->getChildByName(strmake("btnStarter%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onStarterPackClick, this));
        btn->setPosition(Vec2(x, y));
        x += gapX;
        if(UDGetBool(KEY_STARTER_PACK_PURCAHSED, false)){
            btn->removeAllChildren();
            Sprite* spt = Sprite::create("iconCheck.png");
            btn->addChild(spt);
            spt->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2 + 20));
            Label* lblPurchaseComplete = LM->getLocalizedLabel("purchase complete", Color4B::WHITE);
            btn->addChild(lblPurchaseComplete);
            lblPurchaseComplete->setPosition(spt->getPosition() + Vec2(0, -20));
            continue;
        }
        
        LM->setLocalizedString((Text*)btn->getChildByName("lblSubTitle"), "buy once description");
        lbl = (Text*)btn->getChildByName("lblTitle");
        bool isPurchased = false;
        if(i == 0){
            LM->setLocalizedString(lbl, "essential pack");
            strDesc = strmake("%s +\n%s x 5", LM->getText("remove ads").c_str(), LM->getText("3-star weapon").c_str());
            addStarsToIcon(btn->getChildByName("btnSlotGun"), 3);
//            lbl = (Text*)btn->getChildByName("lblPrice");
//            LM->setLocalizedStringNotKey(lbl, GM->priceList.at(IAP_STARTER_0));
            isPurchased = GM->iapList.find(IAP_DETAIL_STARTER1) != std::string::npos;
        }else if(i == 1){
            LM->setLocalizedString(lbl, "quick starter");
            strDesc = strmake("%s 500\n%s x 5", LM->getText("gem").c_str(), LM->getText("3-star weapon").c_str());
            addStarsToIcon(btn->getChildByName("btnSlotGun"), 3);
            setPriceInfo(btn, GM->priceList.at(IAP_STARTER_1_ORIGINAL), GM->priceList.at(IAP_STARTER_0 + i));
            isPurchased = GM->iapList.find(IAP_DETAIL_STARTER2) != std::string::npos;
        }else if(i == 2){
            LM->setLocalizedString(lbl, "genuine user");
            strDesc = strmake("%s 3000\n%s x 5", LM->getText("gem").c_str(), LM->getText("4-star weapon").c_str());
            addStarsToIcon(btn->getChildByName("btnSlotGun"), 4);
            setPriceInfo(btn, GM->priceList.at(IAP_STARTER_2_ORIGINAL), GM->priceList.at(IAP_STARTER_0 + i));
            isPurchased = GM->iapList.find(IAP_DETAIL_STARTER3) != std::string::npos;
        }
        if (isPurchased) {
            ImageView* img = (ImageView*)((Widget*)layer->getChildByName("imgPurchased"))->clone();
            btn->addChild(img, 100);
            img->setPosition(Vec2(46, 57));
            btn->setEnabled(false);
            img->setScale(1);
        }
        
        LM->setLocalizedStringNotKey((Text*)btn->getChildByName("lblDesc"), strDesc);
    }
    
    x = 0.61f;
    gapX = 95;
    //  package
    for (int i = 0; i < 2; i++) {
        btn = (Button*)sv->getChildByName(strmake("btnPackage%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onPackageClick, this));
        btn->setPosition(Vec2(x, y));
        x += gapX;
        LM->setLocalizedString((Text*)btn->getChildByName("lblTitle"), strmake("package %d", i));
        if(i == 0){
            strDesc = strmake("%sx5 + %sx5\n%s 500 + %sx5", LM->getText("3-star weapon").c_str(), LM->getText("3-star shield").c_str(), LM->getText("gem").c_str(), LM->getText("2-star rune").c_str());
            addStarsToIcon(btn->getChildByName("btnWeapon"), 3);
            addStarsToIcon(btn->getChildByName("btnShield"), 3);
            addStarsToIcon(btn->getChildByName("btnRune"), 2);
        }else if(i == 1){
            strDesc = strmake("%sx5 + %sx5\n%s 3000 + %sx5", LM->getText("4-star weapon").c_str(), LM->getText("4-star shield").c_str(), LM->getText("gem").c_str(), LM->getText("3-star rune").c_str());
            addStarsToIcon(btn->getChildByName("btnWeapon"), 4);
            addStarsToIcon(btn->getChildByName("btnShield"), 4);
            addStarsToIcon(btn->getChildByName("btnRune"), 3);
        }
        setPriceInfo(btn, GM->priceList.at(IAP_PACKAGE_0_ORIGINAL + i), GM->priceList.at(IAP_PACKAGE_0 + i));
        LM->setLocalizedStringNotKey((Text*)btn->getChildByName("lblDesc"), strDesc);
    }
    
    ImageView* img;
    int today = SM->getCurrentTime()/86400;
    x = 0.61f;
    gapX = 75;
    //  gem
    int endDay = UDGetInt(KEY_MONTHLY_GEM_END_DATE, 0);
    btn = (Button*)sv->getChildByName("btnMonthlyGem");
    btn->setPosition(Vec2(x, y));
    x += gapX;
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onGemPackClick, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lblTitle"), "monthly product");
    LM->setLocalizedString((Text*)btn->getChildByName("lblDesc"), "gem monthly desc");
    setPriceInfo(btn, GM->priceList.at(IAP_GEM_0_ORIGINAL), GM->priceList.at(IAP_GEM_0));
    if(endDay >= today){
        lbl = (Text*)btn->getChildByName("lblDesc");
        if (endDay - today < 5) {
            lbl->setString(strmake("%s: %d\n%s", LM->getText("day left").c_str(), endDay - today, LM->getText("almost expire buy again").c_str()));
        }else{
            lbl->setString(strmake("%s: %d", LM->getText("day left").c_str(), endDay - today));
        }
    }
    
    for (int i = 0; i < 4; i++) {
        btn = (Button*)sv->getChildByName(strmake("btnGem%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onGemPackClick, this));
        btn->setPosition(Vec2(x, y));
        x += gapX;
        LM->setLocalizedStringNotKey((Text*)btn->getChildByName("lblDesc"), strDesc);
//        LM->setLocalizedString((Text*)btn->getChildByName("lblPrice"), "buy");
        lbl = (Text*)btn->getChildByName("lblTitle");
        LM->setLocalizedString(lbl, "gem");
        img = (ImageView*)btn->getChildByName("imgGemTitle");
        GM->alignToCenter(img, lbl, 5, btn->getContentSize().width/2, 0);
        lbl = (Text*)btn->getChildByName("lblDesc");
        LM->setLocalizedString(lbl, "total");
        
    }
    
    x = 0.61f;
    gapX = 95;
    // ticket
    endDay = UDGetInt(KEY_MONTHLY_TICKET_END_DATE, 0);
    btn = (Button*)sv->getChildByName("btnMonthlyTicket");
    btn->setPosition(Vec2(x, y));
    x += gapX;
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onTicketPackClick, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lblTitle"), "monthly product");
    LM->setLocalizedString((Text*)btn->getChildByName("lblDesc0"), "instant provide");
    LM->setLocalizedString((Text*)btn->getChildByName("lblDesc1"), "provided for 30 days");
    LM->setLocalizedString((Text*)btn->getChildByName("lblDesc2"), "ticket monthly desc");
    setPriceInfo(btn, GM->priceList.at(IAP_TICKET_0_ORIGINAL), GM->priceList.at(IAP_TICKET_0));
    if(endDay >= today){
        lbl = (Text*)btn->getChildByName("lblDesc2");
        if (endDay - today < 5) {
            lbl->setString(strmake("%s: %d\n%s", LM->getText("day left").c_str(), endDay - today, LM->getText("almost expire buy again").c_str()));
        }else{
            lbl->setString(strmake("%s: %d", LM->getText("day left").c_str(), endDay - today));
        }
    }
    for (int i = 0; i < 2; i++) {
        btn = (Button*)sv->getChildByName(strmake("btnTicket%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onTicketPackClick, this));
        btn->setPosition(Vec2(x, y));
        x += gapX;
        if(i == 0){
            LM->setLocalizedString((Text*)btn->getChildByName("lblTitle"), "local ticket");
        }else if(i == 1){
            LM->setLocalizedString((Text*)btn->getChildByName("lblTitle"), "network ticket");
        }
        
//        LM->setLocalizedString((Text*)btn->getChildByName("lblPrice"), "buy");
    }
    
    // video ads
    for (int i = 0; i < 10; i++) {
        btn = (Button*)sv->getChildByName("ndAds")->getChildByName(strmake("btn%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onVideoAdsClick, this));
    }
    
    lbl = (Text*)sv->getChildByName("ndAds")->getChildByName("lblAdsLeft");
    LM->setLocalizedStringNotKey(lbl, strmake("%s: (%d/10)", LM->getText("ads left").c_str(), "Loading..."));
    
    int timeLeft = 86400 - ((int)SM->getCurrentTime());
    lbl = (Text*)sv->getChildByName("ndAds")->getChildByName("lblTimeLeft");
    LM->setLocalizedStringNotKey(lbl, strmake("%s: %s", LM->getText("reset in").c_str(), GM->getTimeLeftInStringHMS(timeLeft).c_str()));
    lbl = (Text*)sv->getChildByName("ndAds")->getChildByName("lblAdsCanSkip");
    LM->setLocalizedString(lbl, "ads can skip by essential pack");
    sv->setScrollBarEnabled(false);
    sv->setClippingEnabled(true);
    updateGemShopButtonPositions(0);
}
void HudLayer::setPriceInfo(Node* btn, std::string before, std::string sale){
    Text* lblBefore = (Text*)btn->getChildByName("lblPriceBefore");
    LM->setLocalizedStringNotKey(lblBefore, before);
    ImageView* line = (ImageView*)btn->getChildByName("imgRedLine");
    line->setContentSize(Size(lblBefore->getContentSize().width*lblBefore->getScale() + 6, 0.5f));
    Text* lblSale = (Text*)btn->getChildByName("lblPrice");
    lblSale->setString(sale);
    GM->alignToCenter(line, lblSale, 5, btn->getContentSize().width, -btn->getContentSize().width/2);
    lblBefore->setPosition(line->getPosition());
}
void HudLayer::onStarterPackClick(Ref* ref){
    BTN_FROM_REF
    int index = btn->getTag();
    if(GM->iapFlag == IAP_FLAG_INIT){
        showIndicator();
        GM->buyItem(IAP_STARTER_0 + index);
    }else{
        showInstanceMessage(LM->getText("iap not ready"));
    }
}
void HudLayer::onPackageClick(Ref* ref){
    BTN_FROM_REF
    int index = btn->getTag();
    if(GM->iapFlag == IAP_FLAG_INIT){
        showIndicator();
        GM->buyItem(IAP_PACKAGE_0 + index);
    }else{
        showInstanceMessage(LM->getText("iap not ready"));
    }
}
void HudLayer::onGemPackClick(Ref* ref){
    BTN_FROM_REF
    int index = btn->getTag();
    if(GM->iapFlag == IAP_FLAG_INIT){
        showIndicator();
        GM->buyItem(IAP_GEM_0 + index);
    }else{
        showInstanceMessage(LM->getText("iap not ready"));
    }
}
void HudLayer::onTicketPackClick(Ref* ref){
    BTN_FROM_REF
    int index = btn->getTag();
    if(index > 0){
        int gem = GM->getGem();
        if(index == 1){
            int price = 25;
            if (gem >= price) {
                GM->addGem(-price);
                GM->localTicketCount += 5;
                SM->saveUserData(strmake("gem=%d&local_ticket_count=%d", GM->getGem(), GM->localTicketCount));
            }else{
                showInstanceMessage(LM->getText("not enough gem"));
            }
        }else if(index == 2){
            int price = 35;
            if (gem >= price) {
                GM->addGem(-price);
                GM->networkTicketCount += 5;
                SM->saveUserData(strmake("gem=%d&network_ticket_count=%d", GM->getGem(), GM->networkTicketCount));
            }else{
                showInstanceMessage(LM->getText("not enough gem"));
            }
        }
        return;
    }
    
    if(GM->iapFlag == IAP_FLAG_INIT){
        showIndicator();
        GM->buyItem(IAP_TICKET_0 + index);
    }else{
        showInstanceMessage(LM->getText("iap not ready"));
    }
}
void HudLayer::giveAwayIAPForTest(){
    hideIndicator();
    GM->iapFlag = IAP_FLAG_SUCCESS;
}
void HudLayer::onVideoAdsClick(Ref* ref){
    BTN_FROM_REF
    log("video clicked: %d", btn->getTag());
    double availableTime = GM->videoShopWatchedTime + 60*15;//UDGetDouble(KEY_VIDEO_NEXT_AVAILABLE_TIME, -1);
    double now = SM->getCurrentTime();
    if (availableTime > now) {
        return;
    }
    showIndicator();
    
    if(isNoAdsPurchased || GM->developer){
        GM->videoIndex = VIDEO_SHOP;
        videoDone();
        log("isNoAdsPurchased: %d", isNoAdsPurchased);
    }else if(GM->isVideoReady()){ // is video ready
        GM->showVideo(VIDEO_SHOP);
//        GM->showVideoDone(); // test
    }else{
        hideIndicator();
        showInstanceMessage(LM->getText("ads not ready try later"));
    }
    updateUI(0);
}
void HudLayer::onGemShopTabClick(Ref* ref){
    BTN_FROM_REF
    int index = btn->getTag();
    updateGemShopButtonPositions(index);
    updateUI(0);
}
void HudLayer::updateGemShopButtonPositions(int index){
    Node* layer = this->getChildByName("shopPopup");
    layer->setTag(index);
    Node* background = layer->getChildByName("imgBackground");
    ScrollView* sv = (ScrollView*)background->getChildByName("sv");
    Button* btn;
    for(int i = 0; i < 5; i++){
        btn = (Button*)background->getChildByName(strmake("btnTab%d", i));
        btn->loadTextureNormal(index == i?"btnBaseHighlight.png":"btnBase.png");
    }
    
    float x = 0.61f;
    float gapX = 5;
    float y = 113.70f;
    float svHeight = sv->getContentSize().height;
    x = 0.61f;
    // starter
    std::string strDesc;Text* lbl;
    for (int i = 0; i < 3; i++) {
        btn = (Button*)sv->getChildByName(strmake("btnStarter%d", i));
        btn->setPosition(Vec2(x, y));
        x += btn->getContentSize().width + gapX;
        btn->setVisible(index == 0);
        
        lbl = (Text*)btn->getChildByName("lblPrice");
        if(i == 0){
            LM->setLocalizedStringNotKey(lbl, GameSharing::getPriceLocale(IAP_DETAIL_STARTER1));
        }else if(i == 1){
            LM->setLocalizedStringNotKey(lbl, GameSharing::getPriceLocale(IAP_DETAIL_STARTER2));
        }else if(i == 2){
            LM->setLocalizedStringNotKey(lbl, GameSharing::getPriceLocale(IAP_DETAIL_STARTER3));
        }
    }
    if(index == 0){
        sv->setInnerContainerSize(Size(x-gapX, svHeight));
    }
    x = 0.61f;
    // package
    for (int i = 0; i < 2; i++) {
        btn = (Button*)sv->getChildByName(strmake("btnPackage%d", i));
        btn->setPosition(Vec2(x, y));
        x += btn->getContentSize().width + gapX;
        btn->setVisible(index == 1);
        
        lbl = (Text*)btn->getChildByName("lblPrice");
        if(i == 0){
            LM->setLocalizedStringNotKey(lbl, GameSharing::getPriceLocale(IAP_DETAIL_PACKAGE1));
        }else if(i == 1){
            LM->setLocalizedStringNotKey(lbl, GameSharing::getPriceLocale(IAP_DETAIL_PACKAGE2));
        }
    }
    if(index == 1){
        sv->setInnerContainerSize(Size(x-gapX, svHeight));
    }
    
    int today = SM->getCurrentTime()/86400;
    x = 0.61f;
    // gem
    int endDay = UDGetInt(KEY_MONTHLY_GEM_END_DATE, 0);
    btn = (Button*)sv->getChildByName("btnMonthlyGem");
    btn->setPosition(Vec2(x, y));
    x += btn->getContentSize().width + gapX;
    btn->setVisible(index == 2);
    lbl = (Text*)btn->getChildByName("lblPrice");
    LM->setLocalizedStringNotKey(lbl, GameSharing::getPriceLocale(IAP_DETAIL_GEM1));
    
    for (int i = 0; i < 4; i++) {
        btn = (Button*)sv->getChildByName(strmake("btnGem%d", i));
        btn->setPosition(Vec2(x, y));
        x += btn->getContentSize().width + gapX;
        btn->setVisible(index == 2);
        lbl = (Text*)btn->getChildByName("lblPrice");
        
        if(i == 0){
            LM->setLocalizedStringNotKey(lbl, GameSharing::getPriceLocale(IAP_DETAIL_GEM2));
        }else if(i == 1){
            LM->setLocalizedStringNotKey(lbl, GameSharing::getPriceLocale(IAP_DETAIL_GEM3));
        }else if(i == 2){
            LM->setLocalizedStringNotKey(lbl, GameSharing::getPriceLocale(IAP_DETAIL_GEM4));
        }else if(i == 3){
            LM->setLocalizedStringNotKey(lbl, GameSharing::getPriceLocale(IAP_DETAIL_GEM5));
        }
    }
    if(index == 2){
        sv->setInnerContainerSize(Size(x - gapX, svHeight));
    }
    
    x = 0.61f;
    // ticket
    endDay = UDGetInt(KEY_MONTHLY_TICKET_END_DATE, 0);
    
    btn = (Button*)sv->getChildByName("btnMonthlyTicket");
    btn->setPosition(Vec2(x, y));
    x += btn->getContentSize().width + gapX;
    btn->setVisible(index == 3);
    lbl = (Text*)btn->getChildByName("lblPrice");
    LM->setLocalizedStringNotKey(lbl, GameSharing::getPriceLocale(IAP_DETAIL_MONTHLY_TICKET));
    
    for (int i = 0; i < 2; i++) {
        btn = (Button*)sv->getChildByName(strmake("btnTicket%d", i));
        btn->setPosition(Vec2(x, y));
        x += btn->getContentSize().width + gapX;
        btn->setVisible(index == 3);
    }
    if(index == 3){
        sv->setInnerContainerSize(Size(x-gapX, svHeight));
    }
    
    if(index == 4){
        sv->setInnerContainerSize(sv->getContentSize());
    }
    
    // video ads
    int videoAdsForShopLeft = 10 - GM->videoShopIndex;//UDGetInt(KEY_VIDEO_ADS_LEFT, 10);
    double availbleTime = GM->videoShopWatchedTime + 60*15;//UDGetDouble(KEY_VIDEO_NEXT_AVAILABLE_TIME, -1);
    double now = SM->getCurrentTime();
    background->getChildByName("btnTab4")->getChildByName("redDot")->setVisible(videoAdsForShopLeft > 0 && availbleTime < now);
    
    lbl = (Text*)sv->getChildByName("ndAds")->getChildByName("lblAdsLeft");
    lbl->setString(strmake("%s: (%d/10)", LM->getText("ads left").c_str(), videoAdsForShopLeft));
    lbl->setVisible(index == 4);
    lbl = (Text*)sv->getChildByName("ndAds")->getChildByName("lblTimeLeft");
    int timeLeft = 86400 - ((int)SM->getCurrentTime())%86400;
    lbl->setString(strmake("%s: %s", LM->getText("reset in").c_str(), GM->getTimeLeftInStringHMS(timeLeft).c_str()));
    lbl->setVisible(index == 4);
    lbl = (Text*)sv->getChildByName("ndAds")->getChildByName("lblAdsCanSkip");
    lbl->setVisible(index == 4);
    
    for (int i = 0; i < 10; i++) {
        btn = (Button*)sv->getChildByName("ndAds")->getChildByName(strmake("btn%d", i));
        btn->setVisible(index == 4);
        btn->setEnabled(10 - videoAdsForShopLeft == i);
        ImageView* img = (ImageView*)btn->getChildByName("imgCheck");
        if(i < 10 - videoAdsForShopLeft){
            img->loadTexture("iconCheck.png");
            img->setContentSize(Size(12, 9));
        }else if(i == 10 - videoAdsForShopLeft){
            img->loadTexture("iconPlay.png");
            img->setContentSize(Size(11, 12));
        }else{
            img->loadTexture("iconLockSmall.png");
            img->setContentSize(Size(9, 12));
        }
        
    }
}
void HudLayer::onTest(){
    onMenuClick();
    onPromoteClick();
//    Button* test = Button::create();
//    test->setTag(-1);
//    test->setName("btnBest0");
//    onRegisterForBestClick(test);
}
void HudLayer::onMissionClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("Mission.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("missionPopup");
    Button* btn;Text* lbl;
    setTitle(layer->getChildByName("titleBack"), "mission");
    setCloseButtonAndBlock(layer);
    
    Node* background = layer->getChildByName("imgBackground");
    ScrollView* sv = (ScrollView*)background->getChildByName("sv");
    
    sv->setScrollBarEnabled(false);
    Button* btnTemp = (Button*)sv->getChildByName("btnTemp");
    int itemCount = 12;
    int itemHeight = 26;
    int gap = 2;
    int scrollHeight = (itemHeight+gap)*itemCount;
    std::string strEquip;
    std::string btnText; std::string reward;bool isGemTrueExpFalse = false;
    int missingCount = 0;
    bool isLocked = false;
    Vector<Button*> btnList;
    for (int i = 0; i < itemCount; i++) {
        isLocked = false;
        if(i >= 0 && i <= 2){
            if (getUnlockLevel(MENU_CHEST) > GM->getGameLevel()) {
                isLocked = true;
            }
        }else if(i == 3){
            if (getUnlockLevel(MENU_DUNGEON) > GM->getGameLevel()) {
                isLocked = true;
            }
        }else if(i == 3){
            if (getUnlockLevel(MENU_DUNGEON) > GM->getGameLevel()) {
                isLocked = true;
            }
        }else if(i == 4){
            if (getUnlockLevel(MENU_MERGE) > GM->getGameLevel()) {
                isLocked = true;
            }
        }else if(i == 7 || i == 8 || i == 11){
            if (getUnlockLevel(MENU_BATTLE) > GM->getGameLevel()) {
                isLocked = true;
            }
        }else if(i == 9){
            if (getUnlockLevel(MENU_SEARCH) > GM->getGameLevel()) {
                isLocked = true;
            }
        }else if(i == 10){
            if (getUnlockLevel(MENU_SHOP) > GM->getGameLevel()) {
                isLocked = true;
            }
        }
        if(isLocked || GM->dailyMissionComplete[i] == MISSION_STATE_REWARDED){
            missingCount++; // test
            if (i == 11) {
                sv->getChildByName("btnCollectTemp")->setVisible(false);
            }
            continue;
        }
        if(i < 5){
            reward = "100";
            isGemTrueExpFalse = true;
        }else{
            reward = "30";
            isGemTrueExpFalse = false;
        }
        if (i == 11) {
            btn = (Button*)sv->getChildByName("btnCollectTemp");
        }else{
            btn = (Button*)btnTemp->clone();
            sv->addChild(btn);
            btn->setTag(i);
        }
        
        btnList.pushBack(btn);
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onMissionItemClick, this));
        btn->setPosition(Vec2(156.81f, scrollHeight - (itemHeight+gap)*(i-missingCount) - itemHeight/2));
        
        lbl = (Text*)btn->getChildByName("lblDesc");
        LM->setLocalizedString(lbl, strmake("mission desc %d", i));
        if (i != 11) {
            lbl = (Text*)btn->getChildByName("lblPrice");
            lbl->setString(reward);
            lbl = (Text*)btn->getChildByName("lblEXP");
            LM->setLocalizedString(lbl, "exp");
        }
        if(i == 11){
            int lastFoodType = -1;
            int foodIndex = 0;
            int collectedCount = 0;
            int maxCount = 0;
            Node* img;Text* lbl;
            for (int i = 0; i < 4; i++) {
                int foodType = UDGetInt(strmake(KEY_COLLECT_MISSION_TYPE_FORMAT, i).c_str(), 0);
                img = btn->getChildByName(strmake("imgItem%d", foodIndex));
                lbl = (Text*)btn->getChildByName(strmake("lblItemCount%d", foodIndex));
                if (foodType != lastFoodType) {
                    maxCount = UDGetInt(strmake(KEY_COLLECT_MISSION_MAX_COUNT_FORMAT, i).c_str(), 2);
                    collectedCount = UDGetInt(strmake(KEY_COLLECT_MISSION_COLLECTED_COUNT_FORMAT, i).c_str(), 0);
                    foodIndex++;
                    lastFoodType = foodType;
                    foodType--;
                }else{
                    maxCount += UDGetInt(strmake(KEY_COLLECT_MISSION_MAX_COUNT_FORMAT, i).c_str(), 2);
                    collectedCount += UDGetInt(strmake(KEY_COLLECT_MISSION_COLLECTED_COUNT_FORMAT, i).c_str(), 0);
                }
                lbl->setString(strmake("%d/%d", collectedCount, maxCount));
                Sprite* spt = Sprite::createWithSpriteFrameName(GM->getFoodSpriteName(foodType+1));
                spt->setScale(0.5f);
                btn->addChild(spt);
                spt->setPosition(img->getPosition());
                img->removeFromParent();
                spt->setName(strmake("imgItem%d", foodIndex));
            }
            for (int i = foodIndex+1; i < 4; i++) {
                btn->getChildByName(strmake("imgItem%d", foodIndex))->setVisible(false);
                btn->getChildByName(strmake("lblItemCount%d", foodIndex))->setVisible(false);
            }
        }else if(isGemTrueExpFalse){
            GM->alignToCenter(btn->getChildByName("img"), btn->getChildByName("lblPrice"), 5, btn->getContentSize().width/2, 0);
            btn->getChildByName("lblEXP")->setVisible(false);
        }else{
            GM->alignToCenter(btn->getChildByName("lblEXP"), btn->getChildByName("lblPrice"), 5, btn->getContentSize().width/2, 0);
            btn->getChildByName("img")->setVisible(false);
        }
        lbl = (Text*)btn->getChildByName("lbl");
        if(GM->dailyMissionComplete[i] == MISSION_STATE_NOT_CLEAR){
            LM->setLocalizedString(lbl, "move");
        }else if(GM->dailyMissionComplete[i] == MISSION_STATE_CLEAR){
            LM->setLocalizedString(lbl, "receive");
        }
    }
    int finalScrollHeight = scrollHeight - missingCount*(itemHeight+gap);
    sv->setInnerContainerSize(Size(116, finalScrollHeight));
    int offsetY = 0;
    if (finalScrollHeight < sv->getContentSize().height) {
        offsetY = sv->getContentSize().height - finalScrollHeight;
    }
    for (int i = 0; i < btnList.size(); i++) {
        btn = btnList.at(i);
        btn->setPositionY(btn->getPositionY() - missingCount*(itemHeight+gap) + offsetY);
    }
    btnList.clear();
    btnTemp->removeFromParent();
}
void HudLayer::onMissionItemClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    int index = btn->getTag();
    log("mission state: %d", GM->dailyMissionComplete[index]);
    if (GM->dailyMissionComplete[index] == MISSION_STATE_CLEAR) {
        setDailyMissionState(index, MISSION_STATE_REWARDED);
        closePopup();
        if(index < 5){
            Button* btn = showMessageBox("reward", "you received reward", 100);
            GM->addGem(100);
            btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopupAndSaveGem, this));
        }else if(index == 11){
            showItemGetAndAddToBag(getRandomWeapon());
        }else{
            Button* btn = showMessageBox("reward", "you received reward", 0);
            btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
            
            GM->addGameExp(30);
            SM->saveUserData(strmake("level=%d&exp=%d", GM->getGameLevel(), GM->getCurrentExp()));
        }
    }else if(GM->dailyMissionComplete[index] == MISSION_STATE_NOT_CLEAR){
        closePopup();
        if (index >= 0 && index <= 2) {
            onChestClick();
        }else if (index == 3) {
            onDungeonClick();
        }else if (index == 4) {
            onMenuClick();
            isPromoteShouldBeShown = true;
        }else if (index == 5) {
            
        }else if (index == 6) {
            onMenuClick();
            isGiftShouldBeShown = true;
        }else if (index == 7 || index == 8 || index == 11) {
            onBattleClick();
        }else if (index == 9) {
            onMenuClick();
            isSearchShouldBeShown = true;
        }else if (index == 10) {
            onGemShopClick();
        }
    }
}
void HudLayer::onFreeGemClick(){
    
}
void HudLayer::onMenuClick(){
    showIndicator();
    showMenuRequired = true;
    SM->getUserData("inventory=1&equip=1&gem=1");
}
void HudLayer::showMenu(){
    hideBtns();
    
    if(GM->isTutorialOn){
        if(GM->tutorialType != TUTORIAL_MENU || GM->tutorialIndex != 9){
            showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
            return;
        }
    }
    
//    gemBar->stopAllActions();
//    gemBar->runAction(EaseInOut::create(MoveTo::create(0.6f, Vec2(911, 607.39f)), 2));
    Node* layer = CSLoader::createNode("Menu.csb");
    menuLayer = layer;
    layer->setName("menuLayer");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    log("%f, %f, %f", size.width, layer->getContentSize().width, layer->getPositionX());
//    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
//    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    if(GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 9){
        attachArrowForTutorial(layer->getChildByName("btnGift"));
    }
    
    Button* btn;
    std::string weaponName;
    setTitle(layer->getChildByName("titleBack"), "main menu");
    for (int i = 0; i < 4; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnWeapon%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onYourWeaponClick, this));
    }
    
    for (int i = 0; i < 4; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnShield%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onYourShieldClick, this));
    }
    
    btn = (Button*)layer->getChildByName("btnAvatar");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onAvatarClick, this));
    
    Node* ndTopPlayer;
    int avatar;
    int limit = 12;
    for (int i = 0; i < topAvatarArray.size(); i++) {
        ndTopPlayer = layer->getChildByName(strmake("top%d", i));
//        ndTopPlayer->setVisible(false); // test 
        ndTopPlayer->removeChildByName("spt");
        avatar = topAvatarArray.at(i);
        Sprite* spt = GM->getAvatarIcon(avatar);//Sprite::createWithSpriteFrameName(strmake("hero%d_0.png", avatar));
        spt->setScale(0.5f);
        ndTopPlayer->addChild(spt);
        std::string name = strmake("%d. %s", i+1, topNameArray.at(i).c_str());
        if(name.length() > limit){
            name = name.substr(0, limit) + "..";
        }
//        ndTopPlayer->getChildByName("sptSearchIcon")->setVisible(false);
        Node* ndEquip = Node::create();
        ndTopPlayer->addChild(ndEquip);
        std::string strEquip = topEquipArray.at(i);
        ValueVector list = GM->split(strEquip, ",");
        int speed = 60;
        ndEquip->runAction(RepeatForever::create(RotateBy::create(1, speed)));
        int length = 12;
        for (int j = 0; j < 8; j++) {
            int index = list.at(j).asInt();
            if(index < 0) continue;
            Sprite* spt = GM->getItemSprite(index);
            if(GM->getItemType(index) == ITEM_TYPE_SHIELD){
                spt->setScale(0.5f);
            }
            ndEquip->addChild(spt);
            if(j < 4){
                spt->setPosition(Vec2(cos((j%4)*2*360/8*3.14f/180)*length, sin((j%4)*2*360/8*3.14f/180)*length));
            }else{
                spt->setPosition(Vec2(cos(((j%4)*2 + 1)*360/8*3.14f/180)*length, sin(((j%4)*2 + 1)*360/8*3.14f/180)*length));
            }
            spt->setAnchorPoint(Vec2(0.5f, 0));
            spt->runAction(RepeatForever::create(RotateBy::create(1, -speed)));
//            spt->setPosition(Vec2(25.11f + j*6.5f, 7.98f));
            spt->setScale(0.4f);
            
            int rareness = GM->getItemRareness(index);
            Vector<Node*> starArray = addStarsToIcon(spt, rareness+1);
            for (int k = 0; k < starArray.size(); k++) {
                starArray.at(k)->setPositionY(starArray.at(k)->getPositionY() - 6);
            }
        }
        
        
        Text* lbl = Text::create(name, "Helvetica", 24);
        ndTopPlayer->addChild(lbl);
        lbl->setTextAreaSize(Size(130, 30));
        lbl->setTextHorizontalAlignment(TextHAlignment::CENTER);
        lbl->setScale(0.2f);
        lbl->setPosition(Vec2(0, -13));
        
        btn = (Button*)layer->getChildByName(strmake("btnTop%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onTopPlayerClick, this));
    }
    
    updateUI(0);
    checkForLockedMenu();
    Sprite* spt;
    for (int i = 0; i < 14; i++) {
        btn = (Button*)layer->getChildByTag(i);
        spt = Sprite::create("redDot.png");
        btn->addChild(spt);
        spt->setTag(0);
        spt->setPosition(btn->getContentSize());
        spt->setVisible(false);
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onMenuInMenuClick, this));
        switch (i) {
            case MENU_MERGE:
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "merge");
                break;
            case MENU_BAG :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "bag");
                break;
            case MENU_COLLECTION :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "collect");
                if(GM->isCollectionHasNew()){
                    spt->setVisible(true);
                }
                break;
            case MENU_SEARCH :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "search");
                break;
            case MENU_DUNGEON :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "dungeon");
                break;
            case MENU_BOSS :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "boss");
                break;
            case MENU_GIFT :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "gift");
                break;
            case MENU_ARMOR :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "armor");
                break;
            case MENU_RESEARCH :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "research");
                break;
            case MENU_ACHIEVE :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "achievement");
                break;
            case MENU_ATTEND :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "attend");
                break;
            case MENU_RUNE :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "rune");
                break;
            case MENU_POST :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "post");
                break;
            case MENU_CHEST :
                LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "chest");
                break;
            default:
                break;
        }
    }
    
    btn = (Button*)layer->getChildByName("btnQuestion");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onFAQClick, this));
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    
    updateAvatarInMenu();
    updateMenuLayer();
    
    if (isPromoteShouldBeShown) {
        isPromoteShouldBeShown = false;
        onPromoteClick();
    }else if(isGiftShouldBeShown){
        isGiftShouldBeShown = false;
        onGiftClick();
    }else if(isSearchShouldBeShown){
        isSearchShouldBeShown = false;
        onSearchClick();
    }
}
void HudLayer::onAvatarClick(){
    Node* layer = CSLoader::createNode("AvatarSelect.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setTag(0);
    setTitle(layer->getChildByName("titleBack"), "avatar");
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    sv->setScrollBarEnabled(false);
    Button* btnTemp = (Button*)sv->getChildByName("btnTemp");
    int columnCount = 4;
    Size btnSize = btnTemp->getBoundingBox().size;
    int rowCount = AVATAR_COUNT/columnCount + (AVATAR_COUNT%columnCount == 0?0:1);
    float xGap = (sv->getContentSize().width - btnSize.width)/(columnCount - 1);
    float yGap = 120;
    float scrollHeight = rowCount*yGap;
    sv->setInnerContainerSize(Size(sv->getContentSize().width, scrollHeight));
    float y = scrollHeight - btnSize.height/2;
    float x = btnSize.width/2;
    int selectedAvatar = UDGetInt(KEY_AVATAR, 0);
    for (int i = 0; i < AVATAR_COUNT; i++) {
        btn = (Button*)btnTemp->clone();
        btn->setTag(i);
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onAvatarSelect, this));
        btn->loadTextureNormal(i == selectedAvatar?"btnBaseHighlight.png":"btnBase.png");
        Sprite* spt = GM->getAvatarIcon(i);
        btn->addChild(spt);
        spt->setScale(0.7f);
        spt->setPosition(btn->getContentSize()/2);
        btn->setPosition(Vec2(x + xGap*(i%columnCount), y - yGap*(i/columnCount)));
        int unlockedLocalEnemyIndex = UDGetInt(KEY_UNLOCKED_LOCAL_ENEMY, 0);
        if(unlockedLocalEnemyIndex < i){
            spt->setColor(Color3B::BLACK);
            spt = Sprite::create("iconLock.png");
            btn->addChild(spt);
            spt->setPosition(btn->getContentSize()/2);
        }
        sv->addChild(btn);
    }
    btnTemp->removeFromParent();
}
void HudLayer::onAvatarSelect(Ref* ref){
    BTN_FROM_REF
    int unlockedLocalEnemyIndex = UDGetInt(KEY_UNLOCKED_LOCAL_ENEMY, 0);
    int index = btn->getTag();
    if(unlockedLocalEnemyIndex < index){ // test
        return;
    }
    WORLD->player->heroType = index;
    WORLD->player->stopAnimationAndStand();
    UDSetInt(KEY_AVATAR, index);
    SM->saveUserData(strmake("avatar=%d", btn->getTag()));
    updateAvatarInMenu();
    closePopup();
}
void HudLayer::updateAvatarInMenu(){
    Node* layer = this->getChildByName("menuLayer");
    Button* btn = (Button*)layer->getChildByName("btnAvatar");
    int selectedAvatar = UDGetInt(KEY_AVATAR, 0);
    btn->removeChildByName("imgAvatar");
    Sprite* spt = GM->getAvatarIcon(selectedAvatar);
    btn->addChild(spt);
    spt->setName("imgAvatar");
    spt->setPosition(btn->getContentSize()/2);
    spt->setScale(0.7f);
}
void HudLayer::onTopPlayerClick(Ref* ref){
    BTN_FROM_REF
    
}
void HudLayer::onMenuInMenuClick(Ref* ref){
    BTN_FROM_REF
    int level = GM->getGameLevel();
    int unlockedLevel = getUnlockLevel(btn->getTag());
    if(unlockedLevel > level){
        showInstanceMessage(strmake(LM->getText("unlock at format").c_str(), unlockedLevel));
        return;
    }
    
    switch (btn->getTag()) {
        case MENU_MERGE:
            onPromoteClick();
            break;
        case MENU_BAG :
            onBagClick();
            break;
        case MENU_COLLECTION :
            onCollectClick(ITEM_TYPE_WEAPON);
            break;
        case MENU_SEARCH :
            onSearchClick();
            retreiveSearchState();
            break;
        case MENU_DUNGEON :
            onDungeonClick();
            break;
        case MENU_BOSS :
            onBossClick();
            break;
        case MENU_GIFT :
            onGiftClick();
            break;
        case MENU_ARMOR :
            onArmorClick();
            break;
        case MENU_RESEARCH :
            onResearchClick();
            break;
        case MENU_ACHIEVE :
            onAchieveClick();
            break;
        case MENU_ATTEND :
            onAttendClick();
            break;
        case MENU_RUNE :
            onRuneClick();
            break;
        case MENU_POST :
            onPostClick();
            break;
        case MENU_CHEST :
            onChestClick();
            break;
        default:
            break;
    }
}
void HudLayer::checkForLockedMenu(){
    Button* btn = (Button*)menuLayer->getChildByName("btnChest");
    ImageView* img = (ImageView*)btn->getChildByName("Image_1");
    img->loadTexture("iconSuitcase.png");
    int level = GM->getGameLevel();
    for (int i = 0; i < 14; i++) {
        if(getUnlockLevel(i) > level){
            btn = (Button*)menuLayer->getChildByTag(i);
            ImageView* img = (ImageView*)btn->getChildByName("Image_1");
            img->loadTexture("iconLock.png");
            img->setContentSize(Size(11, 13));
        }
    }
}
int HudLayer::getUnlockLevel(int index){
    return 0; // test now
    switch (index) {
        case MENU_MERGE:
//            return 0; // test
            return 5;
            break;
        case MENU_BAG :
            return 0;
            break;
        case MENU_COLLECTION :
            return 0; // test
//            return 1;
            break;
        case MENU_SEARCH :
//            return 0; // test
            return 6;
            break;
        case MENU_DUNGEON :
//            return 999; // test
//            return 8;
            break;
        case MENU_BOSS :
//            return 999; // test
            return 10;
            break;
        case MENU_GIFT :
            return 0;
            break;
        case MENU_ARMOR :
//            return 0; // test
            return 11;
            break;
        case MENU_RESEARCH :
//            return 0; // test
            return 3;
            break;
        case MENU_ACHIEVE :
//            return 0; // test
            return 7;
            break;
        case MENU_ATTEND :
//            return 0; // test
            return 2;
            break;
        case MENU_RUNE :
//            return 999; // test 
            return 9;
            break;
        case MENU_POST :
//            return 0; // test
            return 2;
            break;
        case MENU_CHEST :
//            return 0; // test
            return 4;
            break;
        case MENU_SHOP :
            return 1;
            break;
        default:
            break;
    }
    return -1;
}
void HudLayer::onBattleClick(){
//    return; // test
    if(GM->isTutorialOn){
        if(GM->tutorialType != TUTORIAL_MENU || GM->tutorialIndex < 10){
            showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
            return;
        }
    }
    Node* layer = CSLoader::createNode("Battle.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("battlePopup");
    
    
    Button* btn;Text* lbl;
    setTitle(layer->getChildByName("titleBack"), "chest");
    
    btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onCloseBattleLayer, this));
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onCloseBattleLayer, this));
    
    Node* background = layer->getChildByName("imgBackground");
    btn = (Button*)layer->getChildByName("btnLocalTicket");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onPlusTicketClick, this));
    
    btn = (Button*)layer->getChildByName("btnNetworkTicket");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onPlusTicketClick, this));
    
    btn = (Button*)background->getChildByName("btnRefresh");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onRefreshNetworkClick, this));
    
    lbl = (Text*)background->getChildByName("lblNextMatch");
    LM->setLocalizedString(lbl, "next match");
    
    setTitle(layer->getChildByName("titleBack"), "battle");
    setTitle(layer->getChildByName("avatarTitle"), "avatar challenge");
    Node* title = layer->getChildByName("networkTitle");
    setTitle(title, "network battle");
    Node* sptGun = title->getChildByName("sptGun0");
    float offset = 3;
    sptGun->setPositionX(offset);
    sptGun = title->getChildByName("sptGun1");
    sptGun->setPositionX(title->getContentSize().width - offset);
    
    ScrollView* sv = (ScrollView*)background->getChildByName("svLocal");
    sv->setScrollBarEnabled(false);
    Button* btnTemp = (Button*)sv->getChildByName("btnTemp");
    Node* img;
    int charCount = AVATAR_COUNT;
    int itemHeight = 26;
    int gap = 2;
    int scrollHeight = (itemHeight+gap)*charCount;
    sv->setInnerContainerSize(Size(116, scrollHeight));
    std::string strEquip;
    int unlockedLocalEnemyIndex = UDGetInt(KEY_UNLOCKED_LOCAL_ENEMY, 0);
    
    for (int i = 0; i < charCount; i++) {
        btn = (Button*)btnTemp->clone();
        sv->addChild(btn);
        btn->setTag(i);
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onLocalPlayClick, this));
        btn->setPosition(Vec2(0.44f, scrollHeight - (itemHeight+gap)*i - itemHeight/2));
        Sprite* spt = GM->getAvatarIcon(i);
        spt->setScale(0.5f);
        btn->addChild(spt);
        img = btn->getChildByName("img");
        spt->setPosition(img->getPosition());
        img->removeFromParent();
        lbl = (Text*)btn->getChildByName("lblName");
        lbl->setString(LM->getText(strmake("char name %d", i)));
        lbl->setFontName("Helvetica");
        lbl->setFontSize(lbl->getFontSize()*0.8f);
        
        if(i > unlockedLocalEnemyIndex){
            spt->setColor(Color3B::BLACK);
            btn->setEnabled(false);
            btn->getChildByName("imgTicket")->setVisible(false);
            btn->getChildByName("lblTicketCount")->setVisible(false);
            lbl->setString("???");
        }
        strEquip = GM->getLocalBattleEnemyEquipInString(i);
        log("str: %s", strEquip.c_str());
        ValueVector list = GM->split(strEquip, ",");
        for (int j = 0; j < 8; j++) {
            if(list.size() >= j){
                continue;
            }
            Sprite* spt = GM->getItemSprite(list.at(j).asInt());
            if(GM->getItemType(j) == ITEM_TYPE_SHIELD){
                spt->setScale(0.5f);
            }
            btn->addChild(spt);
            spt->setName(strmake("equip%d", i));
            spt->setPosition(Vec2(25.11f + j*6.5f, 7.98f));
            spt->setScale(0.4f);
            if(j < 4){
                spt->setRotation(-45);
            }
            if(i > unlockedLocalEnemyIndex){
                spt->setColor(Color3B::BLACK);
            }
        }
        
        
        lbl = (Text*)btn->getChildByName("lblExp");
        LM->setLocalizedString(lbl, "exp");
        img = btn->getChildByName("imgFruit");
        Sprite* sptFruit = Sprite::createWithSpriteFrameName(GM->getFoodSpriteName(i));
        btn->addChild(sptFruit);
        sptFruit->setAnchorPoint(Vec2(0, 0.5));
        sptFruit->setPosition(img->getPosition());
        sptFruit->setScale(0.5f);
        img->removeFromParent();
        img = btn->getChildByName("imgWeapon");
        img->setPosition(Vec2(sptFruit->getPositionX() + sptFruit->getContentSize().width*sptFruit->getScaleX() + 2.1f, sptFruit->getPositionY()));
        Sprite* sptWeapon = Sprite::createWithSpriteFrameName(GM->getWeaponImageFileName(i+1));
        sptWeapon->setScale(0.5f);
        btn->addChild(sptWeapon);
        sptWeapon->setAnchorPoint(Vec2(0, 0.5));
        sptWeapon->setPosition(img->getPosition());
        sptWeapon->setScale(0.5f);
        img->removeFromParent();
        
        btn = (Button*)btn->getChildByName("btnSelect");
        btn->setTag(i);
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onSelectCharClick, this));
        spt = GM->getAvatarIcon(i);//Sprite::createWithSpriteFrameName(strmake("hero%d_0.png", i));
        spt->setScale(0.7f);
        btn->addChild(spt);
        img = btn->getChildByName("img");
        spt->setPosition(img->getPosition());
        img->removeFromParent();
        lbl = (Text*)btn->getChildByName("lblSelect");
        LM->setLocalizedString(lbl, "select");
        
        if(i > unlockedLocalEnemyIndex){
            spt->setColor(Color3B::BLACK);
            btn->setEnabled(false);
            lbl->setVisible(false);
        }
    }
    btnTemp->removeFromParent();
    
    
    if (GM->isTutorialOn && GM->tutorialType == TUTORIAL_MENU) {
        if(GM->tutorialIndex == 11){
            showTutorialDialog();
        }
        attachArrowForTutorial(this->getChildByName("battlePopup")->getChildByName("imgBackground")->getChildByName("svLocal")->getChildByTag(0));
    }
    
    // check existing matches
    // if exist
    // then generate matches
    // else if empty
    // then find match from server
    // when get matches from server
    // save matches to server and generate matches
    if (!GM->isTutorialOn) {
        if (GM->strNetworkMatchDatas.length() < 10 || GM->shouldNetworkMatchRefresh) {
            GM->shouldNetworkMatchRefresh = false;
            SM->findMatches(UDGetInt(KEY_TROPHY, 1000));
            isFindMatchesRequested = true;
            showIndicator();
        }else{
            generateMatches();
        }
    }
    updateBattleLayer();
}
void HudLayer::generateMatches(){
    hideIndicator();
    log("lets generate matches");
    if(isNetworkMatchDataSet || true){ // test 
        networkMatchAvatars.clear();
        networkMatchNames.clear();
        networkMatchTrophys.clear();
        networkMatchEquips.clear();
        networkMatchRuneEquips.clear();
        networkMatchArmors.clear();
        networkMatchResearchs.clear();
        if(GM->endsWith(GM->strNetworkMatchAvatars, ",")){
            GM->strNetworkMatchAvatars = GM->strNetworkMatchAvatars.substr(0, GM->strNetworkMatchAvatars.length() - 1);
        }
        ValueVector list = GM->split(GM->strNetworkMatchAvatars, ",");
        for (auto value : list) {
            networkMatchAvatars.push_back(value.asInt());
        }
        list = GM->split(GM->strNetworkMatchNames, ",");
        for (auto value : list) {
            networkMatchNames.push_back(value.asString());
        }
        log("%s", GM->strNetworkMatchNames.c_str());
        list = GM->split(GM->strNetworkMatchTrophys, ",");
        for (auto value : list) {
            networkMatchTrophys.push_back(value.asInt());
        }
        list = GM->split(GM->strNetworkMatchEquips, "_");
        for (auto value : list) {
            networkMatchEquips.push_back(value.asString());
        }
        list = GM->split(GM->strNetworkMatchRunes, "_");
        for (auto value : list) {
            networkMatchRuneEquips.push_back(value.asString());
        }
        list = GM->split(GM->strNetworkMatchArmors, "_");
        for (auto value : list) {
            networkMatchArmors.push_back(value.asString());
        }
        list = GM->split(GM->strNetworkMatchResearchs, "_");
        
        for (auto value : list) {
            networkMatchResearchs.push_back(value.asString());
        }
        
        createMatchesUIFromNetworkData();
        return;
    }
    networkMatchIDs.clear();
    networkMatchIndex = 0;
//    ValueVector list = GM->split(GM->strNetworkMatches, ",");
//    log("list count: %d", (int)list.size());
//    for (int i = 0; i < list.size(); i++) {
//        if(list.at(i).asString().length() > 0){
//            networkMatchIDs.push_back(list.at(i).asInt());
//        }
//    }
//    isRequestingNetworkMatchUserData = true;
//    SM->getOtherUserData(strmake("id=%d&name=1&trophy=1&avatar=1&armor=1&research=1&equip=1&rune_equip=1", networkMatchIDs.at(networkMatchIndex)));
    
}
void HudLayer::onRefreshNetworkClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    int price = 110;
    if (price <= GM->getGem()) {
        GM->addGem(-price);
        SM->findMatches(UDGetInt(KEY_TROPHY, 1000));
        isFindMatchesRequested = true;
        showIndicator();
    }else{
        showInstanceMessage(LM->getText("not enough gem"));
    }
}
void HudLayer::finishGenerateMatches(){
    isRequestingNetworkMatchUserData = false;
//    for (int i = 0; i < networkMatchEquips.size(); i++) {
//
//    }
    int aiCount = 10 - networkMatchEquips.size();
    int trophy;std::string str;
    std::vector<int> weapons;
    std::vector<int> shields;
    std::vector<int> runes;
    std::vector<int> armors;
    std::vector<int> researchs;
    int weaponMin;int weaponMax;int shieldMin; int shieldMax;
    for (int i = 0; i < aiCount; i++){
        weapons.clear();
        shields.clear();
        runes.clear();
        networkMatchIDs.push_back(-1);
        networkMatchAvatars.push_back(rand()%7);
        trophy = UDGetInt(KEY_TROPHY, 1000) - rand()%500;
        if(trophy < 0){
            trophy = rand()%10;
        }
        networkMatchTrophys.push_back(trophy);
        
        if(trophy < 10){
            weaponMin = 1;
            weaponMax = 10;
            shieldMin = 0;
            shieldMax = 5;
        }else if(trophy < 30){
            weaponMin = 5;
            weaponMax = 14;
            shieldMin = 2;
            shieldMax = 7;
        }else if(trophy < 70){
            weaponMin = 10;
            weaponMax = 18;
            shieldMin = 4;
            shieldMax = 10;
        }else if(trophy < 120){
            weaponMin = 13;
            weaponMax = 21;
            shieldMin = 8;
            shieldMax = 16;
        }else if(trophy < 200){
            weaponMin = 14;
            weaponMax = 24;
            shieldMin = 10;
            shieldMax = 16;
        }else if(trophy < 300){
            weaponMin = 16;
            weaponMax = 25;
            shieldMin = 11;
            shieldMax = 17;
        }else{
            weaponMin = 18;
            weaponMax = 26;
            shieldMin = 14;
            shieldMax = 20;
        }
        weapons = GM->getRandomNumbers(weaponMin, weaponMax, 4);
        shields = GM->getRandomNumbers(shieldMin, shieldMax, 4);
        runes = GM->getRandomNumbers(800, 816, 4);
        str = strmake("%d,%d,%d,%d,%d,%d,%d,%d", weapons.at(0), weapons.at(1), weapons.at(2), weapons.at(3), 600 + shields.at(0), 600 + shields.at(1), 600 + shields.at(2), 600 + shields.at(3));
        networkMatchEquips.push_back(str);
        str = strmake("%d,%d,%d,%d", runes.at(0), runes.at(1), runes.at(2), runes.at(3));
        networkMatchRuneEquips.push_back(str);
        str = strmake("%d,%d,%d,%d", runes.at(0), runes.at(1), runes.at(2), runes.at(3));
        networkMatchArmors.push_back(str);
        str = strmake("%d,%d,%d,%d", rand()%10, rand()%10, rand()%10, rand()%10);
        networkMatchResearchs.push_back(str);
        networkMatchNames.push_back(getRandomName());
    }
    createMatchesUIFromNetworkData();
    isNetworkMatchDataSet = true;
}
void HudLayer::createMatchesUIFromNetworkData(){
    Node* layer = this->getChildByName("battlePopup");
    Button* btn;Text* lbl;
    if(layer == nullptr){
        return;
    }
    Node* background = layer->getChildByName("imgBackground");
    ScrollView* sv = (ScrollView*)background->getChildByName("svNetwork");
    sv->setScrollBarEnabled(false);
    Button* btnTemp = (Button*)sv->getChildByName("btnTemp");
    Node* img;
    int charCount = 10;
    if(charCount > networkMatchAvatars.size()){
        charCount = networkMatchAvatars.size();
    }
    int itemHeight = 26;
    int gap = 2;
    int scrollHeight = (itemHeight+gap)*charCount;
    sv->setInnerContainerSize(Size(116, scrollHeight));
    std::string strEquip;
    for (int i = 0; i < charCount; i++) {
        btn = (Button*)btnTemp->clone();
        sv->addChild(btn);
        btn->setTag(i);
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onNetworkPlayClick, this));
        btn->setPosition(Vec2(0.44f, scrollHeight - (itemHeight+gap)*i - itemHeight/2));
        Sprite* spt = GM->getAvatarIcon(networkMatchAvatars.at(i));//Sprite::createWithSpriteFrameName(strmake("hero%d_0.png", networkMatchAvatars.at(i)));
        btn->addChild(spt);
        spt->setScale(0.5f);
        img = btn->getChildByName("img");
        if(img != nullptr){
            spt->setPosition(img->getPosition());
            img->removeFromParent();
        }
        
        lbl = (Text*)btn->getChildByName("lblName");
        lbl->setString(networkMatchNames.at(i));
        lbl->setFontName("Helvetica");
        lbl->setFontSize(lbl->getFontSize()*0.8f);
        
        int clearState = UDGetInt(strmake(KEY_NETWORK_BATTLE_RESULT_FORMAT, i).c_str(), 0);
        Button* btnReward = (Button*)btn->getChildByName("btnReward");
        btnReward->addClickEventListener(CC_CALLBACK_1(HudLayer::onNetworkRewardClick, this));
        btnReward->setTag(i);
        btnReward->setEnabled(clearState == 1);
        if(clearState == 2){
            Sprite* checkSprite = Sprite::create("iconCheck.png");
            btnReward->addChild(checkSprite);
            checkSprite->setPosition(btnReward->getContentSize()/2);
        }
        
        strEquip = networkMatchEquips.at(i);
        ValueVector list = GM->split(strEquip, ",");
        for (int j = 0; j < list.size(); j++) {
            int value = list.at(j).asInt();
            if(value < 0) continue;
            Sprite* spt = GM->getItemSprite(value);
            if(GM->getItemType(j) == ITEM_TYPE_SHIELD){
                spt->setScale(0.5f);
            }
            btn->addChild(spt);
            spt->setName(strmake("equip%d", i));
            spt->setPosition(Vec2(25.11f + j*6.5f, 7.98f));
            spt->setScale(0.4f);
            if(j < 4){
                spt->setRotation(-45);
            }
        }
        
        img = (ImageView*)btn->getChildByName("imgReward");
        // hide and show check if defeat
    }
    btnTemp->removeFromParent();
}
void HudLayer::onNetworkPlayClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    
    if(getTotalEquippedWeaponCount() < 3){
        showInstanceMessage(LM->getText("must equip 3 weapons"));
        return;
    }
    
    int index = btn->getTag();
    
    if(GM->networkTicketCount > 0){
        GM->networkTicketCount--;
        double now = SM->getCurrentTime();
        if(GM->networkTicketCount == 4){
            SM->saveUserData(strmake("network_ticket_count=%d&network_ticket_refill_start_time=%d", GM->networkTicketCount, (int)now));
        }else{
            SM->saveUserData(strmake("network_ticket_count=%d", GM->networkTicketCount));
        }

        GM->dualEnemyName = networkMatchNames.at(index);
        GM->dualEnemyChar = networkMatchAvatars.at(index);
        GM->dualEnemyEquip = networkMatchEquips.at(index);//GM->getLocalBattleEnemyEquipInString(index);//"-1,-1,17,25,602,-1,602";
        GM->dualEnemyRune = networkMatchRuneEquips.at(index);//"-1,-1,-1,-1";
        GM->dualEnemyArmor = networkMatchArmors.at(index);
        GM->dualEnemyResearch = networkMatchResearchs.at(index);
        GM->dualEnemyTrophy = networkMatchTrophys.at(index);
        removeUsedAssets();

        GM->isLocalGamePlay = false;
        GM->networkPlayIndex = index;
        SceneChanger* changer = SceneChanger::create();
        changer->setLayer(true, "network battle", "tip0", DualWorld::scene(STAGE_DUAL));
        this->addChild(changer, 99999);

    }else{
        showInstanceMessage(LM->getText("not enough ticket"));
    }
}
void HudLayer::onNetworkRewardClick(Ref* ref){
    BTN_FROM_REF
    GM->addGem(10);
    updateGem(GM->getGem());
    SM->saveUserData(strmake("gem=%d", GM->getGem()));
    GM->showSpriteExplosion(btn, "iconCheck.png", btn->getContentSize()/2, 20, 15, 0.3f);
    UDSetInt(strmake(KEY_NETWORK_BATTLE_RESULT_FORMAT, btn->getTag()).c_str(), 2);
    btn->setEnabled(false);
    Sprite* checkSprite = Sprite::create("iconCheck.png");
    btn->addChild(checkSprite);
    checkSprite->setPosition(btn->getContentSize()/2);
    
    std::string strData = "network_matches_defeat=";
    for (int i = 0; i < 10; i++) {
        strData = strmake("%s%d", strData.c_str(), UDGetInt(strmake(KEY_NETWORK_BATTLE_RESULT_FORMAT, i).c_str(), 0));
    }
    SM->saveUserData(strData);
}
int HudLayer::getTotalEquippedWeaponCount(){
    int count = 0;
    for (int i = 0; i < 4; i++) {
        if (UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, i).c_str(), -1) > 0) {
            count++;
        }
    }
    return count;
}
void HudLayer::onLocalPlayClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    if(getTotalEquippedWeaponCount() < 3){
        showInstanceMessage(LM->getText("must equip 3 weapons"));
        return;
    }
    
    int index = btn->getTag();
    
    if(GM->localTicketCount > 0){
        GM->localTicketCount--;
        double now = SM->getCurrentTime();
        if(GM->localTicketCount == 4){
            SM->saveUserData(strmake("local_ticket_count=%d&local_ticket_refill_start_time=%d", GM->localTicketCount, (int)now));
        }else{
            SM->saveUserData(strmake("local_ticket_count=%d", GM->localTicketCount));
        }
        
        
        GM->dualEnemyName = LM->getText(strmake("char name %d", index));
        GM->dualEnemyChar = index;
        GM->dualEnemyEquip = GM->getLocalBattleEnemyEquipInString(index);//"-1,-1,17,25,602,-1,602";
        GM->dualEnemyRune = "-1,-1,-1,-1";
        GM->dualEnemyArmor = "0,0,0,0";
        GM->dualEnemyResearch = "0,0,0,0";
        GM->dualEnemyTrophy = 1000 + index*100;
        removeUsedAssets();

        GM->isLocalGamePlay = true;
        GM->localPlayIndex = index;
        SceneChanger* changer = SceneChanger::create();
        changer->setLayer(true, "avatar challenge", "tip0", DualWorld::scene(STAGE_DUAL));
        this->addChild(changer, 99999);
        
//        Scene* scene = DualWorld::scene(STAGE_DUAL);
//        Director::getInstance()->replaceScene(scene);
    }else{
        showInstanceMessage(LM->getText("not enough ticket"));
    }
}
void HudLayer::onSelectCharClick(Ref* ref){
    BTN_FROM_REF
    UDSetInt(KEY_AVATAR, btn->getTag());
    SM->saveUserData(strmake("avatar=%d", btn->getTag()));
    updateBattleLayer();
}
void HudLayer::onPlusTicketClick(Ref* ref){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    closePopup();
    onGemShopClick();
    updateGemShopButtonPositions(3);
}
void HudLayer::updateBattleLayer(){
    Node* layer = this->getChildByName("battlePopup");
    Node* background = layer->getChildByName("imgBackground");
    ScrollView* sv = (ScrollView*)background->getChildByName("svLocal");
    
    int charCount = AVATAR_COUNT;
    int itemHeight = 26;
    int gap = 2;
    Button* btn;Text* lbl;
    int selectedAvatarIndex = UDGetInt(KEY_AVATAR, 0);
    for (int i = 0; i < charCount; i++) {
        btn = (Button*)sv->getChildByTag(i);
        btn = (Button*)btn->getChildByName("btnSelect");
        lbl = (Text*)btn->getChildByName("lblSelect");
        if(selectedAvatarIndex == i){
            lbl->setString(LM->getText("selected"));
            btn->loadTextureNormal("btnBaseHighlight.png");
        }else{
            lbl->setString(LM->getText("select"));
            btn->loadTextureNormal("btnBase.png");
            if (i != 0 && i == UDGetInt(KEY_UNLOCKED_LOCAL_ENEMY, 0)) {
                btn->setEnabled(false);
                lbl->setString(LM->getText("locked"));
            }
        }
    }
    
//    HUD->strNetworkMatches
}
void HudLayer::onYourWeaponClick(Ref* ref){
    BTN_FROM_REF
    weaponSlotIndexForEquip = Value(btn->getName().substr(9, 1)).asInt();
    if(GM->isTutorialOn && weaponSlotIndexForEquip > 2){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    int value = UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, weaponSlotIndexForEquip).c_str(), -1);
    if(value < 0){
        showBag(BAG_FILTER_WEAPON, BAG_JOB_REGISTER_FOR_WEAPON_EQUIP);
    }else{
        UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, weaponSlotIndexForEquip).c_str(), -1);
        GM->addBagContent(value%1000, value/1000);
        GM->saveBagContent();
        updateMenuLayer();
        HUD->showIndicator();
        SM->saveUserData(strmake("inventory=%s&equip=%s", UDGetStr(KEY_BAG_CONTENT).c_str(), getWeaponEquipData().c_str()));
        isWeaponEquipChanged = true;
    }
}
void HudLayer::onYourShieldClick(Ref* ref){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    BTN_FROM_REF
    shieldSlotIndexForEquip = Value(btn->getName().substr(9, 1)).asInt();
    int value = UDGetInt(strmake(KEY_SHIELD_SLOT_FORMAT, shieldSlotIndexForEquip).c_str(), -1);
    if(value < 0){
        showBag(BAG_FILTER_SHIELD, BAG_JOB_REGISTER_FOR_SHIELD_EQUIP);
    }else{
        UDSetInt(strmake(KEY_SHIELD_SLOT_FORMAT, shieldSlotIndexForEquip).c_str(), -1);
        GM->addBagContent(value%1000, value/1000);
        GM->saveBagContent();
        updateMenuLayer();
        showIndicator();
        SM->saveUserData(strmake("inventory=%s&equip=%s", UDGetStr(KEY_BAG_CONTENT).c_str(), getWeaponEquipData().c_str()));
        isWeaponEquipChanged = true;
    }
}
void HudLayer::onGiftClick(){
    Node* layer = CSLoader::createNode("Gift.csb");
    giftLayer = layer;
    this->addChild(layer, POPUP_ZORDER + 101);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("gift");
    layer->setTag(0);
    
    
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    setTitle(layer->getChildByName("titleBack"), "gift");
    lbl = (Text*)background->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "gift desc");
    
    if(GM->isTutorialOn && GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 9){
        attachArrowForTutorial(background->getChildByName("btnClaim"));
        UDSetInt(key_GIFT_TAKE_DAY, 0);
    }
    setCloseButtonAndBlock(layer);
    
    btn = (Button*)background->getChildByName("btnClaim");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onGetGiftClick, this));
 
    updateUI(0); // enable button
}
void HudLayer::onGetGiftClick(){
    int day = (int)SM->getCurrentTime()/86400;
    confirmGiftRequired = true;
    setDailyMissionState(DAILY_MISSION_OPEN_CHEST, MISSION_STATE_CLEAR);
    SM->saveUserData(strmake("gift_day=%d", day));
    if(GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 9){
        giftLayer->getChildByName("imgBackground")->getChildByName("btnClaim")->removeChildByName("arrow");
    }
}
void HudLayer::confirmGift(){
    int day = (int)SM->getCurrentTime()/86400;
    UDSetInt(key_GIFT_TAKE_DAY, day);
    
    std::vector<int> array;int newItem;
    if (GM->isTutorialOn && GM->tutorialType == TUTORIAL_MENU) {
        array.push_back(1);
        array.push_back(2);
        array.push_back(3);
    }else{
        for (int i = 0; i < 3; i++) {
            int randValue = rand()%3;
            if(randValue<2){
                newItem = getRandomWeapon();
            }else{
                newItem = getRandomShield();
            }
            array.push_back(newItem);
        }
    }
    
    showItemGetAndAddToBag(array);
}
void HudLayer::onChestClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("Chest.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setTag(0);
    Button* btn;Text* lbl;
    setTitle(layer->getChildByName("titleBack"), "chest");
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    for(int i = 0; i < 3; i++){
        lbl = (Text*)background->getChildByName(strmake("lblPickOne%d", i));
        LM->setLocalizedString(lbl, "pick one");
        lbl = (Text*)background->getChildByName(strmake("lblPickTen%d", i));
        LM->setLocalizedString(lbl, "pick ten");
        btn = (Button*)background->getChildByName(strmake("btnPick%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onPickOneClick, this));
        btn = (Button*)background->getChildByName(strmake("btnPickTen%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onPickTenClick, this));
    }
    SM->getUserData("gem=1");
}
void HudLayer::onPickOneClick(Ref* ref){
    BTN_FROM_REF
    Text* lbl = (Text*)btn->getChildByName("lbl");
    int price = Value(lbl->getString()).asInt();
    if(GM->getGem() >= price){
        GM->addGem(-price);
        if(btn->getTag() == 0){
            showItemGetAndAddToBag(getRandomWeapon());
            setDailyMissionState(0, MISSION_STATE_CLEAR);
            GameSharing::firebaseLog("gacha_weapon_1", "gacha_weapon_1", "gacha_weapon_1");
        }else if(btn->getTag() == 1){
            showItemGetAndAddToBag(getRandomShield());
            setDailyMissionState(1, MISSION_STATE_CLEAR);
            GameSharing::firebaseLog("gacha_armor_1", "gacha_armor_1", "gacha_armor_1");
        }else if(btn->getTag() == 2){
            showItemGetAndAddToBag(getRandomRune());
            setDailyMissionState(2, MISSION_STATE_CLEAR);
            GameSharing::firebaseLog("gacha_manastone_1", "gacha_manastone_1", "gacha_manastone_1");
        }
    }else{
        showInstanceMessage(LM->getText("not enough gem"));
    }
}

int HudLayer::getRandomWeapon(int star){
    if(star == ITEM_RARENESS_1STAR){
        return WEAPON_BEGIN + rand()%(WEAPON_2STAR_START - WEAPON_BEGIN);
    }else if(star == ITEM_RARENESS_2STAR){
        return WEAPON_2STAR_START + rand()%(WEAPON_3STAR_START - WEAPON_2STAR_START);
    }else if(star == ITEM_RARENESS_3STAR){
        return WEAPON_3STAR_START + rand()%(WEAPON_4STAR_START - WEAPON_3STAR_START);
    }else if(star == ITEM_RARENESS_4STAR){
        return WEAPON_4STAR_START + rand()%(WEAPON_5STAR_START - WEAPON_4STAR_START);
    }else if(star == ITEM_RARENESS_5STAR){
        return WEAPON_5STAR_START + rand()%(TOTAL_WEAPON_COUNT - WEAPON_5STAR_START);
    }
    return -1;
}
int HudLayer::getRandomWeapon(){
    int randValue = rand()%1000;
    int fiveStarRate = 15;
    int fourStarRate = 50;
    int newItem = 1;
    if(randValue < fiveStarRate){
        newItem = getRandomWeapon(ITEM_RARENESS_5STAR);
    }else if(randValue < fourStarRate + fiveStarRate){
        newItem = getRandomWeapon(ITEM_RARENESS_4STAR);
    }else if(randValue < 100 + fourStarRate + fiveStarRate){
        newItem = getRandomWeapon(ITEM_RARENESS_3STAR);
    }else if(randValue < 300 + 100 + fourStarRate + fiveStarRate){
        newItem = getRandomWeapon(ITEM_RARENESS_2STAR);
    }else{
        newItem = getRandomWeapon(ITEM_RARENESS_1STAR);
        if(newItem == 0){
            newItem = 1;
        }
    }
    return newItem;
}
int HudLayer::getRandomShield(int star){
    if(star == ITEM_RARENESS_1STAR){
        return SHIELD_BEGIN + rand()%(SHIELD_2STAR_START - SHIELD_BEGIN);
    }else if(star == ITEM_RARENESS_2STAR){
        return SHIELD_2STAR_START + rand()%(SHIELD_3STAR_START - SHIELD_2STAR_START);
    }else if(star == ITEM_RARENESS_3STAR){
        return SHIELD_3STAR_START + rand()%(SHIELD_4STAR_START - SHIELD_3STAR_START);
    }else if(star == ITEM_RARENESS_4STAR){
        return SHIELD_4STAR_START + rand()%(SHIELD_5STAR_START - SHIELD_4STAR_START);
    }else if(star == ITEM_RARENESS_5STAR){
        return SHIELD_5STAR_START + rand()%(TOTAL_SHIELD_COUNT - SHIELD_5STAR_START);
    }
    return -1;
}
int HudLayer::getRandomShield(){
    int randValue = rand()%1000;
    int newItem = 1;
    if(randValue < 1){
        newItem = getRandomShield(ITEM_RARENESS_5STAR);
    }else if(randValue < 10 + 1){
        newItem = getRandomShield(ITEM_RARENESS_4STAR);
    }else if(randValue < 100 + 10 + 1){
        newItem = getRandomShield(ITEM_RARENESS_3STAR);
    }else if(randValue < 300 + 100 + 10 + 1){
        newItem = getRandomShield(ITEM_RARENESS_2STAR);
    }else{
        newItem = getRandomShield(ITEM_RARENESS_1STAR);
    }
    return newItem;
}
int HudLayer::getRandomRune(){
    int stone = rand()%4;
    int symbol = rand()%4;
    
    return stone*4 + symbol + RUNE_BEGIN;
}
int HudLayer::getRandomRune(int star){
    int rune = getRandomRune();
    return rune + star*1000;
}
void HudLayer::onPickTenClick(Ref* ref){
    BTN_FROM_REF
    Text* lbl = (Text*)btn->getChildByName("lbl");
    int price = Value(lbl->getString()).asInt();
    if(GM->getGem() >= price){
        GM->addGem(-price);
        std::vector<int> array;
        if (btn->getTag() == 0) {
            GameSharing::firebaseLog("gacha_weapon_11", "gacha_weapon_11", "gacha_weapon_11");
        }else if (btn->getTag() == 1) {
            GameSharing::firebaseLog("gacha_armor_11", "gacha_armor_11", "gacha_armor_11");
        }else if (btn->getTag() == 2) {
            GameSharing::firebaseLog("gacha_manastone_11", "gacha_manastone_11", "gacha_manastone_11");
        }
        for(int i = 0; i < 11; i++){
            if(btn->getTag() == 0){
                array.push_back(getRandomWeapon());
            }else if(btn->getTag() == 1){
                array.push_back(getRandomShield());
            }else if(btn->getTag() == 2){
                array.push_back(getRandomRune());
            }
        }
        showItemGetAndAddToBag(array);
    }else{
        showInstanceMessage(LM->getText("not enough gem"));
    }
}
void HudLayer::onPromoteClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("Promote.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("promotePopup");
    layer->setTag(0);
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    setTitle(layer->getChildByName("titleBack"), "merging");
    lbl = (Text*)background->getChildByName("lblResultBest");
    LM->setLocalizedString(lbl, "result");
    lbl = (Text*)background->getChildByName("lblResultBetter");
    LM->setLocalizedString(lbl, "result");
    lbl = (Text*)background->getChildByName("lblIngredientForBest");
    LM->setLocalizedString(lbl, "register");
    lbl = (Text*)background->getChildByName("lblIngredientForBetter");
    LM->setLocalizedString(lbl, "register");
    lbl = (Text*)background->getChildByName("btnGun")->getChildByName("lbl");
    LM->setLocalizedString(lbl, "weapon");
    lbl = (Text*)background->getChildByName("btnShield")->getChildByName("lbl");
    LM->setLocalizedString(lbl, "shield");
    lbl = (Text*)background->getChildByName("btnAutoForBest")->getChildByName("lbl");
    LM->setLocalizedString(lbl, "auto register");
    lbl = (Text*)background->getChildByName("btnAutoForBetter")->getChildByName("lbl");
    LM->setLocalizedString(lbl, "auto register");
    lbl = (Text*)background->getChildByName("btnPromoteForBetter")->getChildByName("lblFree");
    LM->setLocalizedString(lbl, "free");
    lbl = (Text*)background->getChildByName("btnPromoteForBest")->getChildByName("lblFree");
    LM->setLocalizedString(lbl, "free");
    lbl = (Text*)background->getChildByName("btnPromoteForBest")->getChildByName("lbl");
    LM->setLocalizedString(lbl, "merge");
    lbl = (Text*)background->getChildByName("btnPromoteForBetter")->getChildByName("lbl");
    LM->setLocalizedString(lbl, "merge");
    
    setCloseButtonAndBlock(layer);
    
    btn = (Button*)background->getChildByName("btnGun");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onPromoteTabClick, this));
    btn = (Button*)background->getChildByName("btnShield");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onPromoteTabClick, this));
    
//    for (int i = 0; i < 5; i++) {
//        btn = (Button*)background->getChildByName(strmake("btnBest%d", i));
//        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onRegisterForBestClick, this));
//    }
//    btn = (Button*)background->getChildByName("btnAutoForBest");
//    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onAutoRegisterForBestClick, this));
//    btn = (Button*)background->getChildByName("btnPromoteForBest");
//    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onPromoteForBestClick, this));
    
    for (int i = 0; i < 5; i++) {
        btn = (Button*)background->getChildByName(strmake("btnBetter%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onRegisterForPromoteClick, this));
    }
    for (int i = 0; i < 4; i++) {
        btn = (Button*)background->getChildByName(strmake("btnStar%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onStarInPromoteClick, this));
    }
    btn = (Button*)background->getChildByName("btnAutoForBetter");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onAutoRegisterForPromoteClick, this));
    btn = (Button*)background->getChildByName("btnPromoteForBetter");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onPromoteToNextClick, this));
    
    updatePromoteLayer();
}

void HudLayer::onPromoteTabClick(Ref* ref){
    BTN_FROM_REF
    
    promoteTypeIndex = btn->getTag();
    promoteStarIndex = 0;
    Node* layer = this->getChildByName("promotePopup");
    int previousLayer = layer->getTag();
    layer->setTag(btn->getTag());
    if(previousLayer == btn->getTag()){
        return;
    }
    Node* background = layer->getChildByName("imgBackground");
//    for (int i = 0; i < 3; i++) {
//        btn = (Button*)background->getChildByName(strmake("btnBest%d", i));
//        btn->setTag(-1);
//    }
    for (int i = 0; i < 5; i++) {
        btn = (Button*)background->getChildByName(strmake("btnBetter%d", i));
        btn->setTag(-1);
    }
    updatePromoteLayer();
}
//void HudLayer::onRegisterForBestClick(Ref* ref){
//    BTN_FROM_REF
//    if(btn->getTag() < 0){
//        Node* layer = this->getChildByName("promotePopup");
//        Node* background = layer->getChildByName("imgBackground");
//        log("name: %s", btn->getName().c_str());
//        background->setTag(Value(btn->getName().substr(7)).asInt());
//        showBag(layer->getTag()==0?BAG_FILTER_WEAPON:BAG_FILTER_SHIELD, BAG_JOB_REGISTER_FOR_BEST);
//    }else{
//        btn->setTag(-1);
//        updatePromoteLayer();
//    }
//}
void HudLayer::showBag(int filter, int job){
    Node* layer = CSLoader::createNode("Bag.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("bagPopup");
    layer->setTag(job);
    Node* background = layer->getChildByName("imgBackground");
    ScrollView* sv = (ScrollView*)background->getChildByName("sv");
    setCloseButtonAndBlock(layer);
    std::vector<BagItem*> array = GM->getBagContents(filter);
    int itemCount = array.size();
    Widget* itemTemp = (Widget*)sv->getChildByName("btnTemp");
    setTitle(layer->getChildByName("titleBack"), "bag");
    float itemHeight = itemTemp->getContentSize().height;
    float itemWidth = itemTemp->getContentSize().width;
    int itemCountInRow = 5;
    sv->setScrollBarEnabled(false);
    float gap = (sv->getContentSize().width - itemWidth*itemCountInRow)/(itemCountInRow+1);
    int rowCount = itemCount/itemCountInRow + (itemCount%itemCountInRow==0?-1:0) + 1;
    float height = rowCount*itemHeight + (rowCount + 1)*gap;
    if(height < sv->getContentSize().height){
        height = sv->getContentSize().height;
    }
    sv->setInnerContainerSize(Size(sv->getContentSize().width, height));
    int indexInRow = 0;
    int itemIndex;
    int itemLevel;
    int value;
    Node* imgIcon;Sprite* spt;Button* btn;
    for (int i = 0; i < itemCount; i++) {
        btn = (Button*)itemTemp->clone();
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onItemInBagClick, this));
        
        if(i == 0 && GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 10){
            attachArrowForTutorial(btn);
        }
        
        sv->addChild(btn);
        indexInRow = i%itemCountInRow;
        btn->setPosition(Vec2(gap + itemWidth/2 + indexInRow*(itemWidth + gap), sv->getInnerContainerSize().height - (itemHeight/2 + (itemHeight + gap)*(i/itemCountInRow)) - gap));
        value = array.at(i)->value;
        float extraY = 2;
        int rareness = GM->getItemRareness(value);
        setButtonTexture(btn, rareness);
        if(rareness == ITEM_RARENESS_NORMAL){
            addStarsToIcon(btn, value/1000+1);
        }else{
            addStarsToIcon(btn, rareness+1, value/1000);
        }
        
        btn->setTag(array.at(i)->index);
        itemIndex = value%1000;
        itemLevel = value/1000;
        imgIcon = btn->getChildByName("imgIcon");
        spt = GM->getItemSprite(value);
        if(GM->getItemType(value) == ITEM_TYPE_SHIELD){
            spt->setScale(0.5f);
        }
        btn->addChild(spt);
        spt->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2 + extraY));
        imgIcon->removeFromParent();
        
        if(job == BAG_JOB_REGISTER_FOR_WEAPON_RANK_UP && array.at(i)->index == selectedItemForRankUp){
            btn->setEnabled(false);
            btn->setOpacity(100);
        }
    }
    itemTemp->removeFromParent();
}
void HudLayer::showItemDetail(int index, bool showNewStar){
    int value = GM->getBagItem(index)->value;
    registeredIndexArrayForRankUp.clear();
    selectedItemForRankUp = index;
    showItemInfo(value, true, showNewStar);
}
void HudLayer::showItemInfo(int value, bool isFromInventory, bool showNewStar){
    itemDetailValue = value;
    int index = value%1000;
    int rank = value/1000;
    Node* layer = CSLoader::createNode("ItemInfo.csb");
    layer->setName("itemDetail");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setTag(value);
    Node* background = layer->getChildByName("imgBackground");
    if(!isFromInventory){
        background->setContentSize(Size(167, 78));
        for(auto child:background->getChildren()){
            child->setPositionY(child->getPositionY() - 70.97f);
        }
        layer->setPositionY(layer->getPositionY() - 108.97f/2);
    }
    Text* lbl;
    Node* titleBack = layer->getChildByName("titleBack");
    setTitle(titleBack, "collect");
    lbl = (Text*)titleBack->getChildByName("lblTitle");
    lbl->setString(GM->getItemName(value));
    ImageView* img = (ImageView*)titleBack;
    img->setContentSize(Size(lbl->getContentSize().width*lbl->getScaleX() + 25, 15));
    lbl->setPositionX(titleBack->getContentSize().width/2);
    
    Button* btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    btn = (Button*)background->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    Node* imgRune = background->getChildByName("imgRune");
    ImageView* imgIconBack = (ImageView*)background->getChildByName("imgIconBack");
    int rareness = GM->getItemRareness(value);
    int itemType = GM->getItemType(value);
    if(itemType == ITEM_TYPE_WEAPON){
        lbl = (Text*)background->getChildByName("lblInfo0");
        std::string str = strmake("%s: %s", LM->getText("tier").c_str(), LM->getText(strmake("tier%d", rareness)).c_str());
        if(rank > 0){
            str = strmake("%s/%s: +%d", str.c_str(), LM->getText("rank up").c_str(), rank);
        }
        LM->setLocalizedStringNotKey(lbl, str);
        
        lbl = (Text*)background->getChildByName("lblInfo1");
        float temp = GM->getWeaponPower(index, rank)*60/GM->getWeaponCoolTime(index);
        std::string rankInfo = "";
        if(rank > 0){
            rankInfo = strmake("(+%d%%)", rank*10);
        }
        if(temp == (int)temp){
            LM->setLocalizedStringNotKey(lbl, strmake("%s: %d%s", LM->getText("power").c_str(), (int)temp, rankInfo.c_str()));
        }else{
            LM->setLocalizedStringNotKey(lbl, strmake("%s: %.2f%s", LM->getText("power").c_str(), temp, rankInfo.c_str()));
        }
        
        lbl = (Text*)background->getChildByName("lblInfo2");
        temp = GM->getWeaponPower(index, rank);
        if(temp == (int)temp){
            LM->setLocalizedStringNotKey(lbl, strmake("%s: %d", LM->getText("single damage").c_str(), (int)temp));
        }else{
            LM->setLocalizedStringNotKey(lbl, strmake("%s: %.2f", LM->getText("single damage").c_str(), temp));
        }
        
        lbl = (Text*)background->getChildByName("lblInfo3");
        temp = GM->getWeaponCoolTime(index);
        if(temp == (int)temp){
            LM->setLocalizedStringNotKey(lbl, strmake("%s: %d", LM->getText("weapon interval").c_str(), (int)temp));
        }else{
            LM->setLocalizedStringNotKey(lbl, strmake("%s: %.2f", LM->getText("weapon interval").c_str(), temp));
        }
        
        int value = RUNE_A_RED + index%4;
        Sprite* spt = GM->getItemSprite(value);
        if(GM->getItemType(index) == ITEM_TYPE_SHIELD){
            spt->setScale(0.5f);
        }
        spt->setPosition(imgRune->getPosition());
        background->addChild(spt);
        imgRune->removeFromParent();
    }else if(itemType == ITEM_TYPE_SHIELD){
        lbl = (Text*)background->getChildByName("lblInfo0");
        LM->setLocalizedStringNotKey(lbl, strmake("%s: %s", LM->getText("tier").c_str(), LM->getText(strmake("tier%d", rareness)).c_str()));
        lbl = (Text*)background->getChildByName("lblInfo1");
        float temp = GM->getShieldHp(index, rank);
        if(temp == (int)temp){
            LM->setLocalizedStringNotKey(lbl, strmake("%s: %d", LM->getText("hp").c_str(), (int)temp));
        }else{
            LM->setLocalizedStringNotKey(lbl, strmake("%s: %.2f", LM->getText("hp").c_str(), temp));
        }
        
        lbl = (Text*)background->getChildByName("lblInfo2");
        temp = GM->getShieldDef(index, rank);
        if(temp == (int)temp){
            LM->setLocalizedStringNotKey(lbl, strmake("%s: %d", LM->getText("defence").c_str(), (int)temp));
        }else{
            LM->setLocalizedStringNotKey(lbl, strmake("%s: %.2f", LM->getText("defence").c_str(), temp));
        }
        
        lbl = (Text*)background->getChildByName("lblInfo3");
        lbl->setVisible(false);
        
        Sprite* spt = GM->getItemSprite(RUNE_A_RED + index%4);
        spt->setPosition(imgRune->getPosition());
        background->addChild(spt);
        imgRune->removeFromParent();
    }else if(itemType == ITEM_TYPE_RUNE){
        lbl = (Text*)background->getChildByName("lblInfo0");
        lbl->setVisible(false);
        lbl = (Text*)background->getChildByName("lblInfo1");
        index = index%RUNE_BEGIN;
        if(index/4 == RUNE_BUFF_TYPE_ATTACK){
            LM->setLocalizedStringNotKey(lbl, strmake("%s: +%d %%", LM->getText("power").c_str(), GM->getRuneBuff(rank, RUNE_BUFF_TYPE_ATTACK)));
        }else if(index/4 == RUNE_BUFF_TYPE_DEF){
            LM->setLocalizedStringNotKey(lbl, strmake("%s: +%d %%", LM->getText("defence").c_str(), GM->getRuneBuff(rank, RUNE_BUFF_TYPE_DEF)));
        }else if(index/4 == RUNE_BUFF_TYPE_HP){
            LM->setLocalizedStringNotKey(lbl, strmake("%s: +%d %%", LM->getText("hp").c_str(), GM->getRuneBuff(rank, RUNE_BUFF_TYPE_HP)));
        }else if(index/4 == RUNE_BUFF_TYPE_CRI){
            LM->setLocalizedStringNotKey(lbl, strmake("%s: +%d %%", LM->getText("critical possibility").c_str(), GM->getRuneBuff(rank, RUNE_BUFF_TYPE_CRI)));
        }
        lbl = (Text*)background->getChildByName("lblInfo2");
        lbl->setVisible(false);
        lbl = (Text*)background->getChildByName("lblInfo3");
        lbl->setVisible(false);
        lbl = (Text*)background->getChildByName("lblInfo4");
        lbl->setPositionY(background->getChildByName("lblInfo3")->getPositionY());
        
        //        Sprite* spt = GM->getItemSprite(RUNE_A_RED + index%4);
        //        spt->setPosition(imgRune->getPosition());
        //        background->addChild(spt);
        imgRune->removeFromParent();
        
        imgIconBack->loadTexture("pnlBaseSimple.png");
    }
    
    if(rareness == ITEM_RARENESS_5STAR){
        imgIconBack->setContentSize(Size(40, 30));
    }
    //    if(itemType == ITEM_TYPE_WEAPON || itemType == ITEM_TYPE_SHIELD){
    lbl = (Text*)background->getChildByName("lblInfo4");
    std::string color;
    if(index%4 == 0){
        color = "red";
    }else if(index%4 == 1){
        color = "yellow";
    }else if(index%4 == 2){
        color = "green";
    }else if(index%4 == 3){
        color = "blue";
    }
    LM->setLocalizedStringNotKey(lbl, strmake("%s: %s", LM->getText("mana color").c_str(), LM->getText(color).c_str()));
    //    }
    
    imgIconBack->loadTexture(getCardNameForRareness(rareness));
    imgIconBack->setScale9Enabled(true);
    
    ImageView* imgIcon = (ImageView*)background->getChildByName("imgIcon");
    Sprite* sptIcon = GM->getItemSprite(value);
    if(GM->getItemType(value) == ITEM_TYPE_SHIELD){
        sptIcon->setScale(0.5f);
    }
    background->addChild(sptIcon);
    sptIcon->setPosition(imgIcon->getPosition());
    imgIcon->removeFromParent();
    
    Node* pnlLevelUp = background->getChildByName("pnlLevelUp");
    if(!isFromInventory){
        pnlLevelUp->setVisible(false);
        return;
    }
    
    Vector<Node*> array = addStarsToIcon(background->getChildByName("imgIconBack"), rareness + 1, rank);
    if (showNewStar && array.size() > 0) {
        Node* lastStar = array.at(rank-1);
        lastStar->setOpacity(0);
        lastStar->setScale(5);
        float fadeTime = 0.8f;
        lastStar->runAction(FadeIn::create(fadeTime*2/3));
        lastStar->runAction(ScaleTo::create(fadeTime, 1));
        lastStar->runAction(RotateBy::create(fadeTime, 360));
    }
    
    // rank up
    lbl = (Text*)pnlLevelUp->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "rank up");
    if(rareness == ITEM_RARENESS_5STAR){
        pnlLevelUp->getChildByName("sptIcon")->setVisible(false);
        pnlLevelUp->getChildByName("lblLockDescription")->setVisible(false);
        for(int i = 0; i < 4; i++){
            btn = (Button*)pnlLevelUp->getChildByName(strmake("btnSlot%d", i));
            btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onSlotForRankUpClick, this));
        }
        btn = (Button*)pnlLevelUp->getChildByName("btnAuto");
        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onAutoRegisterForRankUpClick, this));
        btn = (Button*)pnlLevelUp->getChildByName("btnRankUp");
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onPromoteForRankUpClick, this));
    }else{
        lbl = (Text*)pnlLevelUp->getChildByName("lblLockDescription");
        if(itemType == ITEM_TYPE_RUNE){
            LM->setLocalizedString(lbl, "rune rank up description");
        }else{
            LM->setLocalizedString(lbl, "rank up description");
        }
        
        for(int i = 0; i < 4; i++){
            pnlLevelUp->getChildByName(strmake("btnSlot%d", i))->setVisible(false);
        }
        pnlLevelUp->getChildByName("btnAuto")->setVisible(false);
        pnlLevelUp->getChildByName("btnRankUp")->setVisible(false);
    }
}
Vector<Node*> HudLayer::addStarsToIcon(Node* btn, int starCount, int purpleStarCount){
    while(btn->getChildByName("star") != nullptr){
        btn->removeChildByName("star");
    }
    Vector<Node*> array;
    if(starCount <= 0){
        return array;
    }
    int count = starCount < purpleStarCount?purpleStarCount:starCount;
    for (int i = 0; i < count; i++) {
        ImageView* spt = ImageView::create(i < purpleStarCount?"starPurple.png":"star.png");
        btn->addChild(spt, 1);
        spt->setName("star");
        spt->setPosition(Vec2(0, 5));
        array.pushBack(spt);
    }
    float gap = -0.5f;
    if(count > 5){
        Vector<Node*> subArray;
        for(int i = 0;i < 5;i++){
            array.at(i)->setPositionY(2);
            array.at(i)->setLocalZOrder(2);
            subArray.pushBack(array.at(i));
        }
        GM->alignToCenter(subArray, gap, btn->getContentSize().width/2);
        subArray.clear();
        
        for(int i = 5;i < count;i++){
            array.at(i)->setPositionY(7);
            subArray.pushBack(array.at(i));
        }
        GM->alignToCenter(subArray, gap, btn->getContentSize().width/2);
        subArray.clear();
    }else{
        GM->alignToCenter(array, gap, btn->getContentSize().width/2);
    }
    return array;
}
void HudLayer::onAutoRegisterForRankUpClick(){
    int candidateCount = 0;
    Node* layer = this->getChildByName("itemDetail");
    int value = GM->getBagItem(selectedItemIndexInBag)->value;
    bool isRankedUpItemDetected = false;
    for (int i = 0; i < GM->bagContentArray.size(); i++) {
        if(GM->getItemType(value) == ITEM_TYPE_WEAPON && GM->getItemType(GM->getBagContentItem(i)) != ITEM_TYPE_WEAPON){
            continue;
        }
        if(GM->getItemType(value) == ITEM_TYPE_SHIELD && GM->getItemType(GM->getBagContentItem(i)) != ITEM_TYPE_SHIELD){
            continue;
        }
        if(GM->getItemRareness(GM->getBagContentItem(i)) == ITEM_RARENESS_5STAR && selectedItemIndexInBag != i){
            if(GM->getBagItem(i)->value < 1000){
                candidateCount++;
            }else{
                isRankedUpItemDetected = true;
            }
        }
    }
    if(candidateCount >= 4){
        candidateCount = 0;
        Node* btn;
        for (int i = 0; i < GM->bagContentArray.size(); i++) {
            if(GM->getItemType(value) == ITEM_TYPE_WEAPON && GM->getItemType(GM->getBagContentItem(i)) != ITEM_TYPE_WEAPON){
                continue;
            }
            if(GM->getItemType(value) == ITEM_TYPE_SHIELD && GM->getItemType(GM->getBagContentItem(i)) != ITEM_TYPE_SHIELD){
                continue;
            }
            if(GM->getItemRareness(GM->bagContentArray.at(i)->value) >= ITEM_RARENESS_5STAR && selectedItemIndexInBag != i && GM->getBagItem(i)->value < 1000){
                btn = layer->getChildByName("imgBackground")->getChildByName("pnlLevelUp")->getChildByName(strmake("btnSlot%d", candidateCount));
                btn->setTag(i);
                updateItemDetail();
                candidateCount++;
                if(candidateCount >= 4){
                    break;
                }
            }
        }
    }else if(isRankedUpItemDetected){
        showInstanceMessage(LM->getText("not enough level 0 5 star"));
    }else{
        showInstanceMessage(LM->getText("not enough items in same tier"));
    }
}
void HudLayer::updateItemDetail(){
    Node* layer = this->getChildByName("itemDetail");
    Node* background = layer->getChildByName("imgBackground")->getChildByName("pnlLevelUp");
    Button* btn;
    ImageView* img;
    bool isAllRegistered = true;
    Sprite* spt;
    int value = layer->getTag();//GM->getBagItem(layer->getTag())->value;
    bool isWeaponTab = GM->getItemType(value);
    registeredIndexArrayForRankUp.clear();
    for (int i = 0; i < 4; i++) {
        btn = (Button*)background->getChildByName(strmake("btnSlot%d", i));
        img = (ImageView*)btn->getChildByName("imgIcon");
        img->setOpacity(btn->getTag() < 0?255:0);
        if (btn->getTag() < 0) {
            img->loadTexture(isWeaponTab?"machineGun.png":"iconShield.png");
            btn->removeChildByName("spt");
        }else{
            registeredIndexArrayForRankUp.push_back(btn->getTag());
            if(btn->getChildByName("spt") == nullptr){
                spt = GM->getItemSprite(GM->getBagContentItem(btn->getTag()));
                if(GM->getItemType(btn->getTag()) == ITEM_TYPE_SHIELD){
                    spt->setScale(0.5f);
                }
                spt->setName("spt");
                spt->setPosition(btn->getContentSize()/2);
                btn->addChild(spt);
            }
        }
        if(btn->getTag() < 0){
            isAllRegistered = false;
        }
    }
    
    btn = (Button*)background->getChildByName("btnRankUp");
    btn->setEnabled(isAllRegistered);
    if(btn->isEnabled()){
        GM->makeItSiluk(btn);
    }else{
        btn->stopAllActions();
        btn->setScale(1);
    }
}
void HudLayer::onPromoteForRankUpClick(Ref* ref){
    Node* layer = this->getChildByName("itemDetail");
    Node* background = layer->getChildByName("imgBackground")->getChildByName("pnlLevelUp");
    Text* lbl = (Text*)background->getChildByName("btnRankUp")->getChildByName("lblPrice");
    int price = Value(lbl->getString()).asInt();
    if(price > GM->getGem()){
        showInstanceMessage(LM->getText("not enough gem"));
        return;
    }
    
    GM->addGem(-price);
    BTN_FROM_REF_AND_DISABLE
    
    for (int i = 0; i < 4; i++) {
        btn = (Button*)background->getChildByName(strmake("btnSlot%d", i));
        GM->removeBagContentWithoutUpdateOrder(btn->getTag());
        btn->setTag(-1);
    }
    
    BagItem* item = GM->getBagItem(selectedItemIndexInBag);
    item->value += 1000;
    int newIndex = -1;
    for(int i = 0; i < GM->bagContentArray.size(); i++){
        if(GM->bagContentArray.at(i)->index == item->index){
            newIndex = i;
            break;
        }
    }
//    updateItemDetail();
    GM->updateBagItemIndices();
    GM->saveBagContent();
    closePopup();
    closePopup();
    onBagClick();
    showItemDetail(newIndex, true);
    HUD->showIndicator();
    SM->saveUserData(strmake("inventory=%s", UDGetStr(KEY_BAG_CONTENT).c_str()));
}
void HudLayer::onSlotForRankUpClick(Ref* ref){
    BTN_FROM_REF
    if(btn->getTag() < 0){
        Node* layer = this->getChildByName("itemDetail");
        Node* background = layer->getChildByName("imgBackground")->getChildByName("pnlLevelUp");
        background->setTag(Value(btn->getName().substr(7)).asInt());
        int rareness = GM->getItemRareness(itemDetailValue);
        int itemType = GM->getItemType(GM->getBagContentItem(layer->getTag()));
        if(itemType == ITEM_TYPE_WEAPON){
            showBag(BAG_FILTER_WEAPON_1STAR + rareness, BAG_JOB_REGISTER_FOR_WEAPON_RANK_UP);
        }else{
            showBag(BAG_FILTER_SHIELD_1STAR + rareness, BAG_JOB_REGISTER_FOR_SHIELD_RANK_UP);
        }
    }else{
        btn->setTag(-1);
        updateItemDetail();
    }
}
void HudLayer::onItemInBagClick(Ref* ref){
    BTN_FROM_REF
    Node* layer;
    for(int i = popupArray.size() - 1; i >= 0; i--){
        Node* node = popupArray.at(i);
        if(node->getName().compare("bagPopup") == 0){
            layer = node;
            break;
        }
    }
    int job = layer->getTag();
    int value = GM->getBagItem(btn->getTag())->value;
    if(layer->getTag() == BAG_JOB_NONE){
        selectedItemIndexInBag = btn->getTag();
        showItemDetail(btn->getTag());
    }else if(job >= BAG_JOB_REGISTER_FOR_PROMOTE_0 && job <= BAG_JOB_REGISTER_FOR_PROMOTE_4){
        Node* promoteLayer = this->getChildByName("promotePopup");
        Node* background = promoteLayer->getChildByName("imgBackground");
        Button* btnSlot;
        bool isValidItem = true;
        
        btnSlot = (Button*)background->getChildByName(strmake("btnBetter%d", background->getTag()));
        
        if(GM->getItemRareness(GM->getBagContentItem(btn->getTag())) != GM->getItemRareness(value)){
            isValidItem = false;
            showInstanceMessage(LM->getText("not same tier item"));
            return;
        }else{
            bool isAlreadyRegistered = false;
            for (int i = 0; i < registeredIndexArrayForPromote.size(); i++) {
                if(registeredIndexArrayForPromote.at(i) == btn->getTag()){
                    isAlreadyRegistered = true;
                }
            }
            if(isAlreadyRegistered){
                isValidItem = false;
                showInstanceMessage(LM->getText("already registered"));
                return;
            }else{
                registeredIndexArrayForPromote.push_back(btn->getTag());
            }
        }
        
        if(isValidItem){
            btnSlot->setTag(btn->getTag());
            updatePromoteLayer();
        }
        closePopup();
    }else if(layer->getTag() == BAG_JOB_REGISTER_FOR_WEAPON_RANK_UP){
        Node* promoteLayer = this->getChildByName("itemDetail");
        Node* background = promoteLayer->getChildByName("imgBackground")->getChildByName("pnlLevelUp");
        Button* btnSlot;
        bool isValidItem = true;
        btnSlot = (Button*)background->getChildByName(strmake("btnSlot%d", background->getTag()));
        
        bool isAlreadyRegistered = false;
        for (int i = 0; i < registeredIndexArrayForRankUp.size(); i++) {
            if(registeredIndexArrayForRankUp.at(i) == btn->getTag()){
                isAlreadyRegistered = true;
                break;
            }
        }
        if(isAlreadyRegistered){
            isValidItem = false;
            showInstanceMessage(LM->getText("already registered"));
            return;
        }
        
        if(isValidItem){
            btnSlot->setTag(btn->getTag());
            updateItemDetail();
        }
        closePopup();
    }else if(layer->getTag() == BAG_JOB_REGISTER_FOR_WEAPON_EQUIP || layer->getTag() == BAG_JOB_REGISTER_FOR_SHIELD_EQUIP){
        std::string key;
        int value = GM->getBagItem(btn->getTag())->value;
        if(layer->getTag() == BAG_JOB_REGISTER_FOR_WEAPON_EQUIP){
            for (int i = 0; i < 4; i++) {
                if(weaponSlotIndexForEquip == i){
                    continue;
                }
                key = strmake(KEY_WEAPON_SLOT_FORMAT, i).c_str();
                if(UDGetInt(key.c_str())%1000 == value%1000){
                    showInstanceMessage(LM->getText("can not select same weapon"));
                    return;
                }
            }
        }
        
        GM->removeBagContent(btn->getTag());
        GM->saveBagContent();
        key = strmake(KEY_WEAPON_SLOT_FORMAT, weaponSlotIndexForEquip).c_str();
        if(layer->getTag() == BAG_JOB_REGISTER_FOR_SHIELD_EQUIP){
            key = strmake(KEY_SHIELD_SLOT_FORMAT, shieldSlotIndexForEquip).c_str();
        }
        UDSetInt(key.c_str(), value);
        updateMenuLayer();
        closePopup();
        HUD->showIndicator();
        getWeaponEquipData();
        SM->saveUserData(strmake("inventory=%s&equip=%s", UDGetStr(KEY_BAG_CONTENT).c_str(), getWeaponEquipData().c_str()));
        isWeaponEquipChanged = true;
        
        if(GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 10){
            if (UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 0).c_str(), -1) != -1 &&
                UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 1).c_str(), -1) != -1 &&
                UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 2).c_str(), -1) != -1) {
                currentPopup->getChildByName("btnWeapon0")->removeChildByName("arrow");
                currentPopup->getChildByName("btnWeapon1")->removeChildByName("arrow");
                currentPopup->getChildByName("btnWeapon2")->removeChildByName("arrow");
                showTutorialDialog();
            }
        }
    }else if(layer->getTag() == BAG_JOB_REGISTER_FOR_RUNE_EQUIP){
        int value = GM->getBagItem(btn->getTag())->value;
        GM->removeBagContent(btn->getTag());
        GM->saveBagContent();
        UDSetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, runeSlotIndexToEquip).c_str(), value);
        Node* layer = this->getChildByName("runePopup");
        Node* background = layer->getChildByName("imgBackground");
        Button* btnSlot;
        for (int i = 0; i < 2; i++) {
            btnSlot = (Button*)background->getChildByName(strmake("btnRankUp%d", i));
            btnSlot->setTag(-1);
        }
        updateRunePopup();
        closePopup();
        HUD->showIndicator();
        SM->saveUserData(strmake("inventory=%s&rune_equip=%s", UDGetStr(KEY_BAG_CONTENT).c_str(), getRuneEquipData().c_str()));
    }else if(layer->getTag() == BAG_JOB_REGISTER_FOR_RUNE_RANK_UP){
        Node* layer = this->getChildByName("runePopup");
        Node* background = layer->getChildByName("imgBackground");
        Button* btnSlot;
        bool isAlreadyRegistered = false;
        int firstValue = -1;
        for (int i = 0; i < 2; i++) {
            btnSlot = (Button*)background->getChildByName(strmake("btnRankUp%d", i));
            if(btnSlot->getTag() == btn->getTag()){
                isAlreadyRegistered = true;
                break;
            }
            if(i == 0){
                if(btnSlot->getTag() >= 0){
                    firstValue = GM->getBagItem(btnSlot->getTag())->value;
                }
            }else{
                if(firstValue >= 0 && firstValue != GM->getBagItem(btn->getTag())->value){
                    
                }
            }
            
            if(runeSlotIndexToEquip == i){
                continue;
            }
            if(btnSlot->getTag() >= 0 && GM->getBagItem(btnSlot->getTag())->value != GM->getBagItem(btn->getTag())->value){
                showInstanceMessage(LM->getText("not same rune"));
                return;
            }
        }
        if(isAlreadyRegistered){
            showInstanceMessage(LM->getText("already registered"));
            return;
        }
        
        btnSlot = (Button*)background->getChildByName(strmake("btnRankUp%d", runeSlotIndexToRankUp));
        btnSlot->setTag(btn->getTag());
        updateRunePopup();
        closePopup();
    }
}
std::string HudLayer::getWeaponEquipData(){
    std::string key;
    std::string data = "";
    for (int i = 0; i < 4; i++){
        key = strmake(KEY_WEAPON_SLOT_FORMAT, i);
        data += Value(UDGetInt(key.c_str(), -1)).asString() + ",";
    }
    for (int i = 0; i < 4; i++){
        key = strmake(KEY_SHIELD_SLOT_FORMAT, i);
        data += Value(UDGetInt(key.c_str(), -1)).asString() + ",";
    }
    return data;
}
std::string HudLayer::getRuneEquipData(){
    std::string key;
    std::string data = "";
    for (int i = 0; i < 4; i++){
        key = strmake(KEY_RUNE_EQUIPPED_FORMAT, i);
        data += Value(UDGetInt(key.c_str(), -1)).asString() + ",";
    }
    return data;
}
void HudLayer::setSizeForRareness(Node* img, int rareness){
    if(rareness == ITEM_RARENESS_5STAR){
        img->setContentSize(Size(40, 30));
    }else{
        img->setContentSize(Size(29, 26));
    }
}
void HudLayer::updateMenuLayer(){
    Button* btn;Sprite* spt;Text* lbl;
    for (int i = 0; i < 4; i++) {
        btn = (Button*)menuLayer->getChildByName(strmake("btnWeapon%d", i));
        int value = UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, i).c_str(), -1);
        btn->getChildByName("imgIcon")->setVisible(value < 0);
        btn->removeChildByName("spt");
        int rareness = -1;
        if(value >= 0){
            spt = GM->getItemSprite(value%1000);
            if(GM->getItemType(value%1000) == ITEM_TYPE_SHIELD){
                spt->setScale(0.5f);
            }
            spt->setName("spt");
            btn->addChild(spt);
            lbl = (Text*)btn->getChildByName("lbl");
            rareness = GM->getItemRareness(value);
            btn->loadTextureNormal(getCardNameForRareness(rareness));
            setSizeForRareness(btn, rareness);
            spt->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2 + 2));
        }else{
            btn->loadTextureNormal("btnBaseHighlight.png");
            btn->setContentSize(Size(29, 26));
        }
        addStarsToIcon(btn, rareness+1, value/1000);
        btn->getChildByName("imgIcon")->setPosition(btn->getContentSize()/2);
    }
    for (int i = 0; i < 4; i++) {
        btn = (Button*)menuLayer->getChildByName(strmake("btnShield%d", i));
        int value = UDGetInt(strmake(KEY_SHIELD_SLOT_FORMAT, i).c_str(), -1);
        btn->getChildByName("imgIcon")->setVisible(value < 0);
        btn->removeChildByName("spt");
        int rareness = -1;
        if(value >= 0){
            spt = GM->getItemSprite(value%1000);
            if(GM->getItemType(value%1000) == ITEM_TYPE_SHIELD){
                spt->setScale(0.5f);
            }
            spt->setName("spt");
            btn->addChild(spt);
            lbl = (Text*)btn->getChildByName("lbl");
            rareness = GM->getItemRareness(value);
            btn->loadTextureNormal(getCardNameForRareness(rareness));
            setSizeForRareness(btn, rareness);
            spt->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2 + 2));
        }else{
            btn->loadTextureNormal("btnBaseHighlight.png");
            btn->setContentSize(Size(29, 26));
        }
        addStarsToIcon(btn, rareness+1);
        btn->getChildByName("imgIcon")->setPosition(btn->getContentSize()/2);
    }
}
//void HudLayer::onAutoRegisterForBestClick(){
//    int candidateCount = 0;
//    Node* layer = this->getChildByName("promotePopup");
//    bool isWeaponTab = layer->getTag() == 0;
//    for (int i = 0; i < GM->bagContentArray.size(); i++) {
//        if(isWeaponTab && GM->getItemType(GM->getBagContentItem(i)) == ITEM_TYPE_SHIELD){
//            continue;
//        }
//        if(!isWeaponTab && GM->getItemType(GM->getBagContentItem(i)) == ITEM_TYPE_WEAPON){
//            continue;
//        }
//        if(GM->getItemRareness(GM->getBagContentItem(i)) == ITEM_RARENESS_BETTER){
//            candidateCount++;
//        }
//    }
//    if(candidateCount >= 5){
//        candidateCount = 0;
//        Node* layer = this->getChildByName("promotePopup");
//        Node* background = layer->getChildByName("imgBackground");
//        Node* btn;
//        for (int i = 0; i < GM->bagContentArray.size(); i++) {
//            if(isWeaponTab && GM->getItemType(GM->getBagContentItem(i)) == ITEM_TYPE_SHIELD){
//                continue;
//            }
//            if(!isWeaponTab && GM->getItemType(GM->getBagContentItem(i)) == ITEM_TYPE_WEAPON){
//                continue;
//            }
//            if(GM->getItemRareness(GM->getBagContentItem(i)) == ITEM_RARENESS_BETTER){
//                btn = background->getChildByName(strmake("btnBest%d", candidateCount));
//                btn->setTag(i);
//                updatePromoteLayer();
//                candidateCount++;
//                if(candidateCount >= 5){
//                    break;
//                }
//            }
//        }
//    }else{
//        showInstanceMessage(LM->getText("not enough better weapons"));
//    }
//}
//void HudLayer::onPromoteForBestClick(Ref* ref){
//    BTN_FROM_REF_AND_DISABLE
//    int itemInButton = -1;
//    Node* layer = this->getChildByName("promotePopup");
//    Node* background = layer->getChildByName("imgBackground");
//    for (int i = 4; i >= 0; i--) {
//        btn = (Button*)background->getChildByName(strmake("btnBest%d", i));
//        itemInButton = btn->getTag();
//        btn->setTag(-1);
//        GM->removeBagContentWithoutUpdateOrder(itemInButton);
//    }
//
//    GM->updateBagItemIndices();
//    updatePromoteLayer();
//    bool isWeaponTab = layer->getTag() == 0;
//    int newItem;
//    if(isWeaponTab){
//        newItem = WEAPON_BEST_START + rand()%(TOTAL_WEAPON_COUNT - WEAPON_BEST_START);
//    }else{
//        newItem = SHIELD_BEST_START + rand()%(SHIELD_END - SHIELD_BEST_START);
//    }
//    showItemGetAndAddToBag(newItem);
//}
void HudLayer::showItemGetAndAddToBag(int item){
    std::vector<int> array;
    array.push_back(item);
    showItemGetAndAddToBag(array);
}
void HudLayer::showItemGetAndAddToBag(std::vector<int> array){
    for(int i = 0; i < array.size(); i++){
        GM->addBagContent(array.at(i)%1000, array.at(i)/1000);
    }
    GM->saveBagContent();
    Node* layer = CSLoader::createNode("ItemGet.csb");
//    layer->setPosition(size/2);
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("itemGet");
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    btn->setOpacity(0);
    ImageView* img;ImageView* imgWhiteClone;
    ImageView* imgWhite = (ImageView*)layer->getChildByName("imgWhiteCard");
    int value;
    int index;
    Sprite* spt;
    ImageView* imgTemp =(ImageView*)layer->getChildByName("imgTemp");
    float initTime = 0.3f;
    float fadeInTime = 0.3f;
    float fadeOutTime = 1.0f;
    float centerY = 368;
    
    float gap = 30;
    int columnCount = 5;
    if (columnCount > array.size()) {
        columnCount = array.size();
    }
    float scale = 4;
    int a = (int)(array.size())/columnCount;
    int b = array.size()%columnCount==0?0:1;
    int rowCount = a + b;
    float y = centerY - (imgWhite->getContentSize().height*scale + gap)*(rowCount-1)/2;
    float centerX = layer->getContentSize().width/2;
    int itemCountInThisRow = 0;
    float totalWidthInThisRow = 0;
    float itemWidth = imgTemp->getContentSize().width;
    float whiteStayTime = 0.5f;
    float showOffTime = 0.5f;
    float delayBetweenItems = 0.2f;
    float totalDelay = 0;
    for (int i = 0; i < array.size(); i++) {
        img = (ImageView*)imgTemp->clone();
        layer->addChild(img);
        value = array.at(i);
        GM->setItemCollectionFound(value%1000, COLLECTION_FOUND);
        spt = GM->getItemSprite(value);
        if(GM->getItemType(value) == ITEM_TYPE_SHIELD){
            spt->setScale(0.5f);
        }
        img->addChild(spt);
        spt->setOpacity(0);
        spt->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime), FadeIn::create(fadeInTime), NULL));
        if(spt->getChildren().size() > 0){
            spt->getChildren().at(0)->setOpacity(0);
            spt->getChildren().at(0)->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime), FadeIn::create(fadeInTime), NULL));
        }
        imgWhiteClone = (ImageView*)imgWhite->clone();
        imgWhiteClone->setOpacity(0);
        img->getChildByName("lbl")->runAction(Sequence::create(DelayTime::create(showOffTime + initTime), FadeOut::create(fadeOutTime), NULL));
        int rareness = GM->getItemRareness(value);
        imgWhiteClone->runAction(Sequence::create(DelayTime::create(showOffTime + initTime), FadeIn::create(fadeInTime), DelayTime::create(whiteStayTime + rareness*0.1f), FadeOut::create(fadeOutTime), NULL));
        img->loadTexture(getCardNameForRareness(rareness));
        img->setContentSize(Size(29, 26));
        if(rareness == ITEM_RARENESS_NORMAL){
            img->setCapInsets(Rect(5, 5, 5, 5));
            addStarsToIcon(img, value/1000 + 1);
        }else if(rareness == ITEM_RARENESS_5STAR){
            img->setContentSize(Size(40, 30));
        }
        if(rareness >= ITEM_RARENESS_1STAR && rareness <= ITEM_RARENESS_5STAR){
            addStarsToIcon(img, rareness + 1);
        }
        addGlowBack(img, rareness*0.15f);
        img->getChildByName("lbl")->setPosition(img->getContentSize()/2);
        spt->setPosition(Vec2(img->getContentSize().width/2, img->getContentSize().height/2 + 2));
        if(array.size() - i > 0){
            itemCountInThisRow = columnCount;
        }else{
            itemCountInThisRow = array.size()%columnCount;
        }
        totalWidthInThisRow = itemCountInThisRow*(itemWidth + gap) - gap;
        img->setPosition(Vec2(centerX - totalWidthInThisRow*scale/2 + (i%columnCount)*(itemWidth + gap)*scale + itemWidth*scale/2, y + (i/columnCount)*(img->getContentSize().height*scale + gap)));
        
        layer->addChild(imgWhiteClone);
        imgWhiteClone->setPosition(img->getPosition());
        
        float dropDistance = 500;
        img->setPositionY(img->getPositionY() + dropDistance);
        img->runAction(Sequence::create(DelayTime::create(totalDelay), EaseBackOut::create(MoveBy::create(showOffTime, Vec2(0, -dropDistance))), NULL));
        
        
        showOffTime += delayBetweenItems;
    }
    btn->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime + whiteStayTime), FadeIn::create(fadeInTime), NULL));
    imgTemp->removeFromParent();
    imgWhite->removeFromParent();
    HUD->showIndicator();
    SM->saveUserData(strmake("inventory=%s&gem=%d", UDGetStr(KEY_BAG_CONTENT).c_str(), GM->getGem()));
}
void HudLayer::addGlowBack(Node* parent, float scale){
    if(scale == 0){
        return;
    }
    
    for(int i = 0; i < 4; i++){
        Sprite* spt = Sprite::create("whiteBigCircle.png");
        parent->addChild(spt, -1);
        spt->setRotation(i*60);
        spt->setScale(scale*2.6f, scale);
        spt->setPosition(parent->getContentSize()/2);
        spt->setColor(Color3B(200, 200, 100));
        spt->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
        if(i == 0){
            spt->runAction(RepeatForever::create(RotateBy::create(1, -60)));
        }else{

            spt->runAction(RepeatForever::create(RotateBy::create(1, 60)));
        }
    }
}
void HudLayer::onRegisterForPromoteClick(Ref* ref){
    BTN_FROM_REF
    if(btn->getTag() < 0){
        Node* layer = this->getChildByName("promotePopup");
        Node* background = layer->getChildByName("imgBackground");
        int index = Value(btn->getName().substr(9)).asInt();
        background->setTag(index);
        showBag(layer->getTag()==0?(BAG_FILTER_WEAPON_1STAR+promoteStarIndex):(BAG_FILTER_SHIELD_1STAR+promoteStarIndex), BAG_JOB_REGISTER_FOR_PROMOTE_0 + index);
    }else{
        btn->setTag(-1);
        updatePromoteLayer();
    }
}
void HudLayer::onAutoRegisterForPromoteClick(){
    int candidateCount = 0;
    Node* layer = this->getChildByName("promotePopup");
    bool isWeaponTab = layer->getTag() == 0;
    int starTab = promoteStarIndex;
    int rareness = ITEM_RARENESS_1STAR + starTab;
    for (int i = 0; i < GM->bagContentArray.size(); i++) {
        if(isWeaponTab && GM->getItemType(GM->getBagContentItem(i)) == ITEM_TYPE_SHIELD){
            continue;
        }
        if(!isWeaponTab && GM->getItemType(GM->getBagContentItem(i)) == ITEM_TYPE_WEAPON){
            continue;
        }
        if(GM->getItemRareness(GM->getBagContentItem(i)) == rareness){
            candidateCount++;
        }
    }
    if(candidateCount >= 5){
        candidateCount = 0;
        Node* layer = this->getChildByName("promotePopup");
        Node* background = layer->getChildByName("imgBackground");
        Node* btn;
        for (int i = 0; i < GM->bagContentArray.size(); i++) {
            if(isWeaponTab && GM->getItemType(GM->getBagContentItem(i)) == ITEM_TYPE_SHIELD){
                continue;
            }
            if(!isWeaponTab && GM->getItemType(GM->getBagContentItem(i)) == ITEM_TYPE_WEAPON){
                continue;
            }
            if(GM->getItemRareness(GM->getBagContentItem(i)) == rareness){
                btn = background->getChildByName(strmake("btnBetter%d", candidateCount));
                btn->setTag(i);
                
                candidateCount++;
                if(candidateCount >= 5){
                    updatePromoteLayer();
                    break;
                }
            }
        }
    }else{
        showInstanceMessage(LM->getText("not enough items in same tier"));
    }
}
void HudLayer::onPromoteToNextClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    int itemInButton = -1;
    Node* layer = this->getChildByName("promotePopup");
    Node* background = layer->getChildByName("imgBackground");
    for (int i = 4; i >= 0; i--) {
        btn = (Button*)background->getChildByName(strmake("btnBetter%d", i));
        itemInButton = btn->getTag();
        btn->setTag(-1);
        GM->removeBagContentWithoutUpdateOrder(itemInButton);
    }
    
    GM->updateBagItemIndices();
    GM->saveBagContent();
    bool isWeaponTab = promoteTypeIndex == 0;
    int star = promoteStarIndex;
    int newItem;
    if(isWeaponTab){
        newItem = getRandomWeapon(star + 1);
        setDailyMissionState(4, MISSION_STATE_CLEAR);
    }else{
        newItem = getRandomShield(star + 1);
    }
    
    showItemGetAndAddToBag(newItem);
    updatePromoteLayer();
}
void HudLayer::onStarInPromoteClick(Ref* ref){
    BTN_FROM_REF
    promoteStarIndex = btn->getTag();
    Node* layer = this->getChildByName("promotePopup");
    Node* background = layer->getChildByName("imgBackground");
    for(int i = 0; i < 5; i++){
        btn = (Button*)background->getChildByName(strmake("btnBetter%d", i));
        btn->setTag(-1);
    }
    updatePromoteLayer();
}
void HudLayer::updatePromoteLayer(){
    Node* layer = this->getChildByName("promotePopup");
    Node* background = layer->getChildByName("imgBackground");
    bool isWeaponTab = layer->getTag() == 0;
    Button* btn = (Button*)background->getChildByName("btnGun");
    btn->loadTextureNormal(isWeaponTab?"btnBaseHighlight.png":"btnBase.png");
    btn = (Button*)background->getChildByName("btnShield");
    btn->loadTextureNormal(!isWeaponTab?"btnBaseHighlight.png":"btnBase.png");
    for (int i = 0; i < 4; i++) {
        btn = (Button*)background->getChildByName(strmake("btnStar%d", i));
        btn->loadTextureNormal(promoteStarIndex == i?"btnBaseHighlight.png":"btnBase.png");
    }
    ImageView* img;
    bool isAllRegistered = true;
    Sprite* spt;
    
    btn = (Button*)background->getChildByName("btnPromoteForBest");
    btn->setEnabled(isAllRegistered);
    if(btn->isEnabled()){
        GM->makeItSiluk(btn);
    }else{
        btn->stopAllActions();
        btn->setScale(1);
    }
    
    isAllRegistered = true;
    registeredIndexArrayForPromote.clear();
    img = (ImageView*)background->getChildByName("imgResult");
//    img->loadTexture(getCardNameForRareness(ITEM_RARENESS_1STAR + promoteStarIndex + 1));
    setImageTexture(img, ITEM_RARENESS_1STAR + promoteStarIndex + 1);
    img->getChildByName("lbl")->setPosition(img->getContentSize()/2);
    for (int i = 0; i < 5; i++) {
        btn = (Button*)background->getChildByName(strmake("btnBetter%d", i));
        setButtonTexture(btn, ITEM_RARENESS_1STAR + promoteStarIndex);
        img = (ImageView*)btn->getChildByName("imgIcon");
        img->setOpacity(btn->getTag() < 0?255:0);
        if (btn->getTag() < 0) {
            img->loadTexture(isWeaponTab?"machineGun.png":"iconShield.png");
            btn->removeChildByName("spt");
        }else{
            registeredIndexArrayForPromote.push_back(btn->getTag());
            if(btn->getChildByName("spt") == nullptr){
                spt = GM->getItemSprite(GM->getBagContentItem(btn->getTag()));
                if(GM->getItemType(GM->getBagContentItem(btn->getTag())) == ITEM_TYPE_SHIELD){
                    spt->setScale(0.5f);
                }
                spt->setName("spt");
                spt->setPosition(btn->getContentSize()/2);
                btn->addChild(spt);
            }
        }
        if(btn->getTag() < 0){
            isAllRegistered = false;
        }
    }
    
    btn = (Button*)background->getChildByName("btnPromoteForBetter");
    btn->setEnabled(isAllRegistered);
    if(btn->isEnabled()){
        GM->makeItSiluk(btn);
    }else{
        btn->stopAllActions();
        btn->setScale(1);
    }
}
void HudLayer::onBagClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    showBag(BAG_FILTER_NONE, BAG_JOB_NONE);
}
void HudLayer::onCollectClick(int itemType){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    itemTypeForCollection = itemType;
    showCollectionRequired = true;
    if(itemType == ITEM_TYPE_WEAPON){
        SM->getUserData("weapon_collect=1");
    }else if(itemType == ITEM_TYPE_SHIELD){
        SM->getUserData("shield_collect=1");
    }else if(itemType == ITEM_TYPE_RUNE){
        SM->getUserData("rune_collect=1");
    }
}
void HudLayer::showCollection(){
    int itemType = itemTypeForCollection;
    Node* layer = CSLoader::createNode("Collection.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("collect");
    layer->setTag(itemType);
    Button* btn;
    Node* background = layer->getChildByName("imgBackground");
    setTitle(layer->getChildByName("titleBack"), "collect");
    btn = (Button*)background->getChildByName("btnGun");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onCollectTabClick, this));
    btn->loadTextureNormal(itemTypeForCollection == ITEM_TYPE_WEAPON?"btnBaseHighlight.png":"btnBase.png");
    btn = (Button*)background->getChildByName("btnShield");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onCollectTabClick, this));
    btn->loadTextureNormal(itemTypeForCollection == ITEM_TYPE_SHIELD?"btnBaseHighlight.png":"btnBase.png");
    btn = (Button*)background->getChildByName("btnRune");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onCollectTabClick, this));
    btn->loadTextureNormal(itemTypeForCollection == ITEM_TYPE_RUNE?"btnBaseHighlight.png":"btnBase.png");
    ScrollView* sv = (ScrollView*)background->getChildByName("sv");
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    int itemCount = 0;
    std::string key;
    if(itemType == ITEM_TYPE_WEAPON){
        itemCount = TOTAL_WEAPON_COUNT;
        key = KEY_COLLECTION_WEAPON_FOUND;
    }else if(itemType == ITEM_TYPE_SHIELD){
        itemCount = TOTAL_SHIELD_COUNT - SHIELD_BEGIN;
        key = KEY_COLLECTION_SHIELD_FOUND;
    }else if(itemType == ITEM_TYPE_RUNE){
        itemCount = RUNE_END - RUNE_BEGIN;
        key = KEY_COLLECTION_RUNE_FOUND;
    }
    Widget* itemTemp = (Widget*)sv->getChildByName("btnTemp");
    float itemHeight = itemTemp->getContentSize().height;
    float itemWidth = itemTemp->getContentSize().width;
    int itemCountInRow = 5;
    if(itemType != ITEM_TYPE_WEAPON){
        itemCountInRow = 4;
    }
    sv->setScrollBarEnabled(false);
    float gap = (sv->getContentSize().width - itemWidth*itemCountInRow)/(itemCountInRow+1);
    int rowCount = itemCount/itemCountInRow + (itemCount%itemCountInRow==0?-1:0) + 1;
    float height = rowCount*itemHeight + (rowCount + 1)*gap;
    if(height < sv->getContentSize().height){
        height = sv->getContentSize().height;
    }
    sv->setInnerContainerSize(Size(sv->getContentSize().width, height));
    int indexInRow = 0;
    int itemIndex;
    int itemLevel;
    int value;
    Node* imgIcon;Sprite* spt;
    std::string data = UDGetStr(key.c_str(), "");
    int state;
    int array[itemCount];
    for (int i = 0; i < itemCount; i++) {
        if(data.size() > i){
            state = Value(data.substr(i, 1)).asInt();
        }else{
            state = COLLECTION_NOT_FOUND;
        }
        
        if(itemType == ITEM_TYPE_WEAPON){
            value = WEAPON_BEGIN + i;
        }else if(itemType == ITEM_TYPE_SHIELD){
            value = SHIELD_BEGIN + i;
        }else if(itemType == ITEM_TYPE_RUNE){
            value = RUNE_BEGIN + i;
        }
        
        if(state == COLLECTION_NOT_FOUND && GM->getItemCountInBag(value%1000) > 0){
            state = COLLECTION_FOUND;
        }else if(i == 0){
            state = COLLECTION_REWARDED;
        }
//        state = COLLECTION_REWARDED; // test 
        array[i] = state;
        btn = (Button*)itemTemp->clone();
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onItemInCollectionClick, this));
        sv->addChild(btn);
        if(state == COLLECTION_NOT_FOUND){
            btn->setEnabled(false);
        }
        
        indexInRow = i%itemCountInRow;
        btn->setPosition(Vec2(gap + itemWidth/2 + indexInRow*(itemWidth + gap), (itemHeight/2 + (itemHeight + gap)*(i/itemCountInRow)) + gap));
        btn->setTag(value);
        
        float extraY = 0;
        int rareness = GM->getItemRareness(value);
        setButtonTexture(btn, rareness);
        if(rareness == ITEM_RARENESS_NORMAL){
            addStarsToIcon(btn, value/1000+1);
        }else{
            addStarsToIcon(btn, rareness+1);
        }
        
        itemIndex = value%1000;
        itemLevel = value/1000;
        imgIcon = btn->getChildByName("imgIcon");
        spt = GM->getItemSprite(value);
        if(GM->getItemType(value) == ITEM_TYPE_SHIELD){
            spt->setScale(0.5f);
        }
        if(state == COLLECTION_NOT_FOUND){
            spt->setColor(Color3B::BLACK);
            for(auto child: spt->getChildren()){
                child->setColor(Color3B::BLACK);
            }
        }
        btn->addChild(spt);
        spt->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2 + extraY));
        if(state == COLLECTION_FOUND){
            Sprite* spt = Sprite::create("redDot.png");
            btn->addChild(spt);
            spt->setPosition(btn->getContentSize());
            spt->setName("redDot");
        }
        imgIcon->removeFromParent();
    }
    itemTemp->removeFromParent();
    
    std::string newData = "";
    for(int i = 0; i < itemCount;i++){
        newData.append(Value(array[i]).asString());
    }
    UDSetStr(key.c_str(), newData);
}
void HudLayer::onItemInCollectionClick(Ref* ref){
    BTN_FROM_REF
    log("I: %d", btn->getTag());
    
    showItemInfo(btn->getTag(), false);
    int index = btn->getTag()%1000;
    Node* sptRedDot = btn->getChildByName("redDot");
    if(sptRedDot != nullptr){ // reward newly found item
        sptRedDot->removeFromParent();
        Node* layer = this->getChildByName("collect");
        std::string key;
        int itemCount=0;
        int initNumber = 0;
        int itemType = layer->getTag();
        std::string dataKey;
        if(itemType == ITEM_TYPE_WEAPON){
            itemCount = TOTAL_WEAPON_COUNT;
            key = KEY_COLLECTION_WEAPON_FOUND;
            initNumber = WEAPON_BEGIN;
            dataKey = "weapon_collect";
        }else if(itemType == ITEM_TYPE_SHIELD){
            itemCount = TOTAL_SHIELD_COUNT - SHIELD_BEGIN;
            key = KEY_COLLECTION_SHIELD_FOUND;
            initNumber = SHIELD_BEGIN;
            dataKey = "shield_collect";
        }else if(itemType == ITEM_TYPE_RUNE){
            itemCount = RUNE_END - RUNE_BEGIN;
            key = KEY_COLLECTION_RUNE_FOUND;
            initNumber = RUNE_BEGIN;
            dataKey = "rune_collect";
        }
        
        std::string data = UDGetStr(key.c_str(), "");
        int state;
        int array[itemCount];
        for (int i = 0; i < itemCount; i++) {
            state = Value(data.substr(i, 1)).asInt();
            if(index - initNumber == i){
                array[i] = COLLECTION_REWARDED;
                showReward(NEW_ITEM_FOUND);
            }else{
                array[i] = state;
            }
        }
        
        std::string newData = "";
        for(int i = 0; i < itemCount;i++){
            newData.append(Value(array[i]).asString());
        }
        UDSetStr(key.c_str(), newData);
        SM->saveUserData(strmake("%s=%s&gem=%d",dataKey.c_str(), newData.c_str(), GM->getGem()));
        updateGem(GM->getGem());
    }
}
void HudLayer::showReward(int popupType){
    std::string key = "";
    if(popupType == NEW_ITEM_FOUND){
        key = "new item found";
    }else if(popupType == NEW_UI_FOUND){
        key = "new ui found";
    }
    Node* layer = CSLoader::createNode("FoundRewardPopup.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    
    Text* lbl =(Text*)background->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, key);
    
    lbl = (Text*)background->getChildByName("imgIconBack")->getChildByName("lblCount");
    int rewardCount = Value(lbl->getString()).asInt();
    GM->addGem(rewardCount);
}
void HudLayer::onCollectTabClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    closePopup();
    onCollectClick(btn->getTag());
    currentPopup->stopAllActions();
    currentPopup->setScale(1);
}
void HudLayer::onSearchClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("Search.csb");
    layer->setName("search");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    Text* lbl = (Text*)background->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "search for broken guns");
    lbl = (Text*)background->getChildByName("lblNextSearch");
    LM->setLocalizedString(lbl, "next search");
    Button* btn = (Button*)background->getChildByName("btnUpdate");
    GM->alignToCenter(lbl, btn, 5, background->getContentSize().width/2, 0);
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::updateSearchClick, this));
    setTitle(layer->getChildByName("titleBack"), "search");
    for(int i = 0;i<3;i++){
        lbl = (Text*)background->getChildByName(strmake("lblForceFix%d", i));
        LM->setLocalizedString(lbl, "engineer fix");
        btn = (Button*)background->getChildByName(strmake("btnForceFix%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onForceFixClick, this));
        btn = (Button*)background->getChildByName(strmake("btnTryFix%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onTryFixClick, this));
        lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "try fix");
        lbl = (Text*)background->getChildByName(strmake("lblObtained%d", i));
        LM->setLocalizedString(lbl, "obtained");
    }
    
    updateSearchItems();
    if(UDGetBool("neverSearched", true)){
        updateSearchClick();
        UDSetBool("neverSearched", false);
    }
}
void HudLayer::updateSearchClick(){
    int ticketCount = GM->searchTicketCount;
    if(ticketCount > 0){
        ticketCount--;
        for(int i = 0; i < 3; i++){
            UDSetInt(strmake(KEY_SEARCH_STATE_FORMAT, i).c_str(), SEARCH_STATE_NOT_TRY);
        }
        
        GM->searchTicketCount = ticketCount;
        
        for(int i = 0; i < 3; i++){
            int weapon = getRandomWeapon();
            log("weapon: %d", weapon);
            UDSetInt(strmake(KEY_SEARCH_ITEM_INDICES_FORMAT, i).c_str(), weapon);
        }
        closePopup();
        onSearchClick();
        updateSearchPopupRequired = true;
        GM->searchTicketRefillTimeLeft = SEARCH_TICKET_REFILL_TIME;
        saveSearchState();
    }else{
        showInstanceMessage(LM->getText("not enough search ticket"));
    }
}
void HudLayer::retreiveSearchState(){
    showIndicator();
    updateSearchPopupRequired = true;
    SM->getUserData("search_state=1&search_items=1&search_ticket_count=1&search_refill_start_time=1");
}
void HudLayer::saveSearchState(){
//    showIndicator();
    updateSearchPopupRequired = true;
    std::string str = strmake("%d%d%d", UDGetInt(strmake(KEY_SEARCH_STATE_FORMAT, 0).c_str()), UDGetInt(strmake(KEY_SEARCH_STATE_FORMAT, 1).c_str()), UDGetInt(strmake(KEY_SEARCH_STATE_FORMAT, 2).c_str()));
    std::string strItems = strmake("%d,%d,%d", UDGetInt(strmake(KEY_SEARCH_ITEM_INDICES_FORMAT, 0).c_str()), UDGetInt(strmake(KEY_SEARCH_ITEM_INDICES_FORMAT, 1).c_str()), UDGetInt(strmake(KEY_SEARCH_ITEM_INDICES_FORMAT, 2).c_str()));
    double now = SM->getCurrentTime();
    SM->saveUserData(strmake("search_ticket_count=%d&search_state=%s&search_items=%s&search_refill_start_time=%d&gem=%d&inventory=%s", GM->searchTicketCount, str.c_str(), strItems.c_str(), (int)now, GM->getGem(), UDGetStr(KEY_BAG_CONTENT).c_str()));
}
void HudLayer::onForceFixClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    Text* lbl = (Text*)btn->getChildByName("lbl");
    int price = Value(lbl->getString()).asInt();
    if(GM->getGem() >= price){
        GM->addGem(-price);
        UDSetInt(strmake(KEY_SEARCH_STATE_FORMAT, btn->getTag()).c_str(), SEARCH_STATE_GET);
        closePopup();
        onSearchClick();
        showSearchResult(UDGetInt(strmake(KEY_SEARCH_ITEM_INDICES_FORMAT, btn->getTag()).c_str()), true);
        SM->saveUserData(strmake("gem=%d&local_ticket_count=%d", GM->getGem(), GM->localTicketCount));
    }else{
        btn->setEnabled(true);
    }
}
void HudLayer::onTryFixClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    int value = UDGetInt(strmake(KEY_SEARCH_ITEM_INDICES_FORMAT, btn->getTag()).c_str());
    bool success = false;
    if(GM->getItemRareness(value) == ITEM_RARENESS_5STAR){
        success = rand()%1000 < 10;
    }else if(GM->getItemRareness(value) == ITEM_RARENESS_4STAR){
        success = rand()%1000 < 50;
    }else if(GM->getItemRareness(value) == ITEM_RARENESS_3STAR){
        success = rand()%1000 < 100;
    }else if(GM->getItemRareness(value) == ITEM_RARENESS_2STAR){
        success = rand()%1000 < 300;
    }else{
        success = rand()%1000 < 500;
    }
    UDSetInt(strmake(KEY_SEARCH_STATE_FORMAT, btn->getTag()).c_str(), success?SEARCH_STATE_GET:SEARCH_STATE_TRIED);
    if(success){
        closePopup();
        onSearchClick();
    }
    showSearchResult(value, success);
}
void HudLayer::showSearchResult(int index, bool success){
    Node* layer = CSLoader::createNode("SearchResult.csb");
    this->addChild(layer, POPUP_ZORDER);
    layer->setName("searchResult");
    layer->setTag(index);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
//    Node* background = layer->getChildByName("imgBackground");
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onOkSearchResultClick, this));
    
    Button* img = (Button*)layer->getChildByName("btnSlot");
    int value = index;
    GM->setItemCollectionFound(value%1000, COLLECTION_FOUND);
    Sprite* spt = GM->getItemSprite(value);
    if(GM->getItemType(value) == ITEM_TYPE_SHIELD){
        spt->setScale(0.5f);
    }
    img->addChild(spt);
    spt->setPosition(img->getContentSize()/2);
    Node* imgWhiteClone = layer->getChildByName("imgWhiteCard");
    imgWhiteClone->setOpacity(0);
    img->setTouchEnabled(false);
    float showOffTime = 0.5f;
    float fadeInTime = 0.3f;
    float fadeOutTime = 0.8f;
    float whiteStayTime = 0.8f;
    img->loadTextureNormal(getCardNameForRareness(GM->getItemRareness(value)));
    
    imgWhiteClone->runAction(Sequence::create(DelayTime::create(showOffTime), FadeIn::create(fadeInTime), DelayTime::create(whiteStayTime), ScaleTo::create(0.2f, 100), FadeOut::create(fadeOutTime), NULL));
    
    img->setContentSize(Size(29, 26));
    Text* lbl = (Text*)layer->getChildByName("lblResult");
    lbl->setOpacity(0);
    lbl->runAction(Sequence::create(DelayTime::create(showOffTime + fadeInTime + whiteStayTime), FadeIn::create(0.2f), NULL));
    LM->setLocalizedString(lbl, success?"success":"failed");
    lbl->setColor(success?Color3B::GREEN:Color3B::RED);
    lbl->setTag(success);
    GM->makeItSiluk(lbl);
    
    if(success){ // success
        img->getChildByName("sptGrass")->runAction(Sequence::create(DelayTime::create(showOffTime + fadeInTime + whiteStayTime + fadeOutTime), FadeOut::create(0.3f), NULL));
        img->getChildByName("sptGrass")->runAction(Sequence::create(DelayTime::create(showOffTime + fadeInTime + whiteStayTime + fadeOutTime), ScaleTo::create(0.3f, 2), NULL));
        img->runAction(Sequence::create(DelayTime::create(showOffTime + fadeInTime + whiteStayTime), CallFunc::create(CC_CALLBACK_0(HudLayer::addGlowToSearchResult, this)), NULL));
        HUD->setDailyMissionState(DAILY_MISSION_FIX_WEAPON, MISSION_STATE_CLEAR);
    }else{
        imgWhiteClone->runAction(Sequence::create(DelayTime::create(showOffTime + fadeInTime + whiteStayTime), TintTo::create(fadeOutTime, Color3B(220, 0, 220)), NULL));
    }
//    img->setPosition(Vec2(0, size.height));
    
//    layer->addChild(imgWhiteClone);
    imgWhiteClone->setPosition(img->getPosition());
    
    float dropDistance = 500;
    img->setPositionY(img->getPositionY() + dropDistance);
    img->runAction(Sequence::create(EaseBackOut::create(MoveBy::create(showOffTime, Vec2(0, -dropDistance))), NULL));
    
    saveSearchState();
}
void HudLayer::onOkSearchResultClick(){
    Node* layer = this->getChildByName("searchResult");
    
    Text* lbl = (Text*)layer->getChildByName("lblResult");
    if(lbl->getOpacity() == 255){
        closePopup();
        return;
    }else{
        
    }
    
    lbl->stopAllActions();
    lbl->setOpacity(255);
    
//    Button* btn = (Button*)layer->getChildByName("btnOk");
    
    Button* img = (Button*)layer->getChildByName("btnSlot");
    Node* imgWhiteClone = layer->getChildByName("imgWhiteCard");
    imgWhiteClone->removeFromParent();
    
    
    if(lbl->getTag()){ // success
        img->getChildByName("sptGrass")->removeFromParent();
        img->stopAllActions();
        addGlowToSearchResult();
    }else{
        
    }
}
void HudLayer::addGlowToSearchResult(){
    Node* layer = this->getChildByName("searchResult");
    if(layer == nullptr){
        return;
    }
        
    int index = layer->getTag();
    Node* img = layer->getChildByName("btnSlot");
    addGlowBack(img, (GM->getItemRareness(index)+1)*0.15f);
    
//    layer->addChild(Firework::createOnce(18));
}
void HudLayer::updateSearchItems(){
    Node* layer = this->getChildByName("search");
    if(layer == nullptr){
        return;
    }
    Node* background = layer->getChildByName("imgBackground");
    Sprite* spt;Text* lbl;Button* btn;
    for(int i = 0; i < 3; i++){
        int state = UDGetInt(strmake(KEY_SEARCH_STATE_FORMAT, i).c_str(), SEARCH_STATE_NOT_TRY);
        btn = (Button*)background->getChildByName(strmake("btnTryFix%d", i));
        btn->setEnabled(state == SEARCH_STATE_NOT_TRY);
//        btn->setEnabled(true); // test
        btn->setVisible(state != SEARCH_STATE_GET); // test 
        btn = (Button*)background->getChildByName(strmake("btnForceFix%d", i));
        btn->setVisible(state != SEARCH_STATE_GET);
        lbl = (Text*)background->getChildByName(strmake("lblForceFix%d", i));
        lbl->setVisible(state != SEARCH_STATE_GET);
        lbl = (Text*)background->getChildByName(strmake("lblObtained%d", i));
        lbl->setVisible(state == SEARCH_STATE_GET);
        
        int value = UDGetInt(strmake(KEY_SEARCH_ITEM_INDICES_FORMAT, i).c_str());
        int index = value%1000;
        lbl = (Text*)btn->getChildByName("lbl");
        int price = 0;
        int rareness = GM->getItemRareness(value);
        if(rareness == ITEM_RARENESS_1STAR){
            price = 50 + (index - WEAPON_BEGIN)*1;
        }else if(rareness == ITEM_RARENESS_2STAR){
            price = 150 + (index - WEAPON_2STAR_START)*2;
        }else if(rareness == ITEM_RARENESS_3STAR){
            price = 450 + (index - WEAPON_3STAR_START)*10;
        }else if(rareness == ITEM_RARENESS_4STAR){
            price = 950 + (index - WEAPON_4STAR_START)*40;
        }else if(rareness == ITEM_RARENESS_5STAR){
            price = 2250 + (index - WEAPON_5STAR_START)*77;
        }
        lbl->setString(Value(price).asString());
        btn = (Button*)background->getChildByName(strmake("btnSlot%d", i));
        
        btn->setTag(value);
        btn->removeChildByName("spt");
        spt = GM->getItemSprite(value);
        if(GM->getItemType(value) == ITEM_TYPE_SHIELD){
            spt->setScale(0.5f);
        }
        btn->addChild(spt);
        spt->setName("spt");
        spt->setPosition(btn->getContentSize()/2);
        lbl = (Text*)background->getChildByName(strmake("lblWeaponName%d", i));
        LM->setLocalizedStringNotKey(lbl, GM->getWeaponName(value));
        
        btn->loadTextureNormal(getCardNameForRareness(GM->getItemRareness(value)));
        btn->loadTexturePressed(getCardNameForRareness(GM->getItemRareness(value)));
    }
}
void HudLayer::onDungeonClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("DailyDungeon.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("dungeonPopup");
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    
    setTitle(layer->getChildByName("titleBack"), "7 day dungeon");
    Text* lbl = (Text*)background->getChildByName("lblDay0");
    LM->setLocalizedString(lbl, "monday");
    lbl = (Text*)background->getChildByName("lblDay1");
    LM->setLocalizedString(lbl, "friday");
    lbl = (Text*)background->getChildByName("lblDay2");
    LM->setLocalizedString(lbl, "tuesday");
    lbl = (Text*)background->getChildByName("lblDay3");
    LM->setLocalizedString(lbl, "friday");
    lbl = (Text*)background->getChildByName("lblDay4");
    LM->setLocalizedString(lbl, "wednesday");
    lbl = (Text*)background->getChildByName("lblDay5");
    LM->setLocalizedString(lbl, "saturday");
    lbl = (Text*)background->getChildByName("lblDay6");
    LM->setLocalizedString(lbl, "thursday");
    lbl = (Text*)background->getChildByName("lblDay7");
    LM->setLocalizedString(lbl, "saturday");
    
    lbl = (Text*)background->getChildByName("lblScore");
    LM->setLocalizedString(lbl, "score");
    lbl = (Text*)background->getChildByName("lblReward");
    LM->setLocalizedString(lbl, "reward");
    lbl = (Text*)background->getChildByName("lblTicket");
    LM->setLocalizedString(lbl, "ticket");
    
    long now = SM->getCurrentTime();
    int day = (int)now/86400 - 1;
    int whatDay = day%7;
    Button* btn;
    for (int i = 0; i < 4; i++) {
        btn = (Button*)background->getChildByName(strmake("btnEnter%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onEnterDailyDungeonClick, this));
        lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "enter");
        if(i == 0){
            btn->setEnabled(whatDay == 0 || whatDay == 4 || whatDay == 6);
        }else if(i == 1){
            btn->setEnabled(whatDay == 1 || whatDay == 4 || whatDay == 6);
        }else if(i == 2){
            btn->setEnabled(whatDay == 2 || whatDay == 5 || whatDay == 6);
        }else if(i == 3){
            btn->setEnabled(whatDay == 3 || whatDay == 5 || whatDay == 6);
        }
        if(GM->dungeonTicketCount <= 0){
            btn->setEnabled(false);
        }
    }
    updateUI(0);
    showIndicator();
    SM->getUserData("dungeon_ticket_count=1&dungeon_ticket_refill_start_time=1");
}
void HudLayer::onEnterDailyDungeonClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    int index = btn->getTag();
    int countLeft = GM->dungeonTicketCount;
    if(countLeft > 0){
        countLeft--;
        GM->dungeonTicketCount = countLeft;
        if(countLeft == 2){
            SM->saveUserData(strmake("dungeon_ticket_count=%d&dungeon_ticket_refill_start_time=%d", GM->dungeonTicketCount, (int)SM->getCurrentTime()));
        }else{
            SM->saveUserData(strmake("dungeon_ticket_count=%d", GM->dungeonTicketCount));
        }
        setDailyMissionState(3, MISSION_STATE_CLEAR);
        this->removeUsedAssets();
        if(index == 0){
            Director::getInstance()->replaceScene(HelloWorld::scene(STAGE_DAILY_DUNGEON_0, false));
        }else if(index == 1){
            Director::getInstance()->replaceScene(HelloWorld::scene(STAGE_DAILY_DUNGEON_1, false));
        }else if(index == 2){
            Director::getInstance()->replaceScene(HelloWorld::scene(STAGE_DAILY_DUNGEON_2, false));
        }else if(index == 3){
            Director::getInstance()->replaceScene(HelloWorld::scene(STAGE_DAILY_DUNGEON_3, false));
        }
    }else{
        
    }
}
void HudLayer::setDailyMissionState(int index, int state){
    int saveState = false;
    if (state == MISSION_STATE_CLEAR && GM->dailyMissionComplete[index] == MISSION_STATE_NOT_CLEAR) {
        saveState = true;
    }else if (state == MISSION_STATE_REWARDED && GM->dailyMissionComplete[index] == MISSION_STATE_CLEAR) {
        saveState = true;
    }
    if (saveState) {
        GM->dailyMissionComplete[index] = state;
        std::string strState = "";
        for (int i = 0; i < 12; i++) {
            strState += Value(GM->dailyMissionComplete[i]).asString();
        }
        SM->saveUserData(strmake("dailymission=%s", strState.c_str()));
    }
}
void HudLayer::onBossClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("Boss.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("bossPopup");
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    
    setTitle(layer->getChildByName("titleBack"), "boss");
    Text* lbl = (Text*)background->getChildByName("lblScore");
    LM->setLocalizedString(lbl, "score");
    lbl = (Text*)background->getChildByName("lblReward");
    LM->setLocalizedString(lbl, "reward");
    
    Button* btn = (Button*)background->getChildByName("btnBattle");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onEnterBossDungeon, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "enter");
    
    showIndicator();
    SM->getUserData("boss_ticket_count=1&boss_ticket_refill_start_time=1");
}
void HudLayer::onEnterBossDungeon(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    this->unschedule(schedule_selector(HudLayer::updateUI));
    if(GM->bossTicketCount > 0){
        GM->bossTicketCount--;
        if(GM->bossTicketCount == 0){
            SM->saveUserData(strmake("boss_ticket_count=%d&boss_ticket_refill_start_time=%d", GM->bossTicketCount, (int)SM->getCurrentTime()));
        }else{
            SM->saveUserData(strmake("boss_ticket_count=%d", GM->bossTicketCount));
        }
        
        this->removeUsedAssets();
        
        GM->dualEnemyName = "BOSS";
//        GM->dualEnemyChar = index;
        GM->dualEnemyEquip = "25,25,25,25,-1,-1,-1";
        GM->dualEnemyRune = "-1,-1,-1,-1";
        GM->dualEnemyArmor = "0,0,0,0";
        GM->dualEnemyResearch = "0,0,0,0";
        
        GM->isLocalGamePlay = true;
//        GM->localPlayIndex = index;
        GM->isBossFight = true;
        SceneChanger* changer = SceneChanger::create();
        GM->tipIndex = rand()%9;
        changer->setLayer(true, "boss", strmake("tip%d", GM->tipIndex), DualWorld::scene(STAGE_DUAL));
        this->addChild(changer, 99999);
        
    }
}
void HudLayer::onArmorClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("Armor.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("armorPopup");
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    
    setTitle(layer->getChildByName("titleBack"), "armor");
    Text* lbl = (Text*)background->getChildByName("lblHP");
    LM->setLocalizedString(lbl, "hp");
    lbl = (Text*)background->getChildByName("lblDefence");
    LM->setLocalizedString(lbl, "defence");
    lbl = (Text*)background->getChildByName("lblExtraDefence");
    LM->setLocalizedString(lbl, "extra defence");
    lbl = (Text*)background->getChildByName("lblDodge");
    LM->setLocalizedString(lbl, "dodge");
    Button* btn;
    for(int i = 0; i < 4; i++){
        btn = (Button*)background->getChildByName(strmake("btnUpgrade%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onUpgradeArmorClick, this));
        lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "upgrade");
    }
    
    updateArmorLayer();
    showIndicator();
    updateArmorRequired = true;
    SM->getUserData("armor=1");
}
void HudLayer::onUpgradeArmorClick(Ref* ref){
    BTN_FROM_REF
    int index = btn->getTag();
    Text* lbl = (Text*)btn->getChildByName("lblCount");
    int price = Value(lbl->getString()).asInt();
    int youHave = GM->getCurrency(CURRENCY_TYPE_GEM_PURPLE);
    if(youHave >= price){
        GM->addCurrency(-price, CURRENCY_TYPE_GEM_PURPLE);
        int level = UDGetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, index).c_str(), 0);
        level++;
        UDSetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, index).c_str(), level);
        updateArmorLayer();
        SM->saveUserData(strmake("crystal_purple=%d", GM->getCurrency(CURRENCY_TYPE_GEM_PURPLE)));
        std::string str = strmake("%d,%d,%d,%d", GM->getHeroHP(), GM->getHeroDefence(), GM->getHeroDefenceBuff(), GM->getHeroDodge());
        SM->saveUserData(strmake("armor=%s", str.c_str()));
    }else{
        showInstanceMessage(strmake(LM->getText("not enough format").c_str(), LM->getText("purple crystal").c_str()));
    }
}

void HudLayer::updateArmorLayer(){
    Node* layer = this->getChildByName("armorPopup");
    Node* background = layer->getChildByName("imgBackground");
    Text* lbl = (Text*)background->getChildByName("lblHPValue");
    lbl->setString(strmake("%d -> %d", GM->getHeroHP(), GM->getHeroNextHP()));
    lbl = (Text*)background->getChildByName("lblHP");
    lbl->setString(strmake("%s (Lv.%d)", LM->getText("hp").c_str(), GM->getHeroHP()));
    
    lbl = (Text*)background->getChildByName("lblDefenceValue");
    lbl->setString(strmake("%d -> %d", GM->getHeroDefence(), GM->getHeroNextDefence()));
    lbl = (Text*)background->getChildByName("lblDefence");
    lbl->setString(strmake("%s (Lv.%d)", LM->getText("defence").c_str(), GM->getHeroDefence()));
    
    lbl = (Text*)background->getChildByName("lblExtraDefenceValue");
    lbl->setString(strmake("%d%% -> %d%%", GM->getHeroDefenceBuff(), GM->getHeroNextDefenceBuff()));
    lbl = (Text*)background->getChildByName("lblExtraDefence");
    lbl->setString(strmake("%s (Lv.%d)", LM->getText("extra defence").c_str(), GM->getHeroDefenceBuff()));
    
    lbl = (Text*)background->getChildByName("lblDodgeValue");
    lbl->setString(strmake("%.1f%% -> %.1f%%", GM->getHeroDodge(), GM->getHeroNextDodge()));
    lbl = (Text*)background->getChildByName("lblDodge");
    lbl->setString(strmake("%s (Lv.%d)", LM->getText("dodge").c_str(), GM->getHeroDodge()));
    
    int youHave = GM->getCurrency(CURRENCY_TYPE_GEM_PURPLE);
    lbl = (Text*)background->getChildByName("imgYouHave")->getChildByName("lbl");
    lbl->setString(Value(youHave).asString());
    
    Button* btn;
    int maxLevel = 100; int level;int price;
    for(int i = 0; i < 4; i++){
        btn = (Button*)background->getChildByName(strmake("btnUpgrade%d", i));
        lbl = (Text*)btn->getChildByName("lblCount");
        lbl->setString(Value(0).asString());
        level = UDGetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, i).c_str(), 0);
        price = (1 + level)*20;
        btn->setEnabled(maxLevel > level);
        if(maxLevel <= level){
            btn->getChildByName("lblCount")->setVisible(false);
            lbl = (Text*)btn->getChildByName("lbl");
            lbl->setString("MAX");
            
        }else{
            lbl->setString(Value(price).asString());
        }
    }
}
void HudLayer::onResearchClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("Research.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("researchPopup");
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    
    setTitle(layer->getChildByName("titleBack"), "research");
    Text* lbl;Button* btn;
    for(int i = 0; i < 4; i++){
        lbl = (Text*)background->getChildByName(strmake("lbl%d", i));
        switch (i) {
            case 0:
                LM->setLocalizedString(lbl, "cool time");
                break;
            case 1:
                LM->setLocalizedString(lbl, "weapon power");
                break;
            case 2:
                LM->setLocalizedString(lbl, "critical possibility");
                break;
            case 3:
                LM->setLocalizedString(lbl, "critical power");
                break;
        }
        
        btn = (Button*)background->getChildByName(strmake("btnUpgrade%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onUpgradeResearchClick, this));
        lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "upgrade");
    }
    
    updateResearchLayer();
    updateResearchRequired = true;
    showIndicator();
    SM->getUserData("research=1");
}
void HudLayer::updateResearchLayer(){
    Node* layer = this->getChildByName("researchPopup");
    Node* background = layer->getChildByName("imgBackground");
    Text* lbl;
    Button* btn;
    int maxLevel = 100; int level;int price;int youHave;
    for(int i = 0; i < 4; i++){
        lbl = (Text*)background->getChildByName(strmake("lbl%dValue", i));
        switch (i) {
            case 0:
                youHave = GM->getCurrency(CURRENCY_TYPE_GEM_GRAY);
                lbl->setString(strmake("%.1f%% -> %.1f%%", GM->getResearchCoolTime(), GM->getResearchNextCoolTime()));
                break;
            case 1:
                youHave = GM->getCurrency(CURRENCY_TYPE_GEM_GREEN);
                lbl->setString(strmake("%d%% -> %d%%", GM->getResearchPower(), GM->getResearchNextPower()));
                break;
            case 2:
                youHave = GM->getCurrency(CURRENCY_TYPE_GEM_ORANGE);
                lbl->setString(strmake("%.1f%% -> %.1f%%", GM->getResearchCriticalPossibility(), GM->getResearchNextCriticalPossibility()));
                break;
            case 3:
                youHave = GM->getCurrency(CURRENCY_TYPE_GEM_RED);
                lbl->setString(strmake("%d%% -> %d%%", GM->getResearchCriticalPower(), GM->getResearchNextCriticalPower()));
                break;
        }
        lbl = (Text*)background->getChildByName(strmake("lbl%d", i));
        int level = UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, i).c_str(), 0);
        switch (i) {
            case 0:
                lbl->setString(strmake("%s (Lv.%d)", LM->getText("cool time").c_str(), level));
                break;
            case 1:
                lbl->setString(strmake("%s (Lv.%d)", LM->getText("weapon power").c_str(), level));
                break;
            case 2:
                lbl->setString(strmake("%s (Lv.%d)", LM->getText("critical possibility").c_str(), level));
                break;
            case 3:
                lbl->setString(strmake("%s (Lv.%d)", LM->getText("critical power").c_str(), level));
                break;
        }
        
        lbl = (Text*)background->getChildByName(strmake("imgYouHave%d", i))->getChildByName("lbl");
        lbl->setString(Value(youHave).asString());
        
        btn = (Button*)background->getChildByName(strmake("btnUpgrade%d", i));
        lbl = (Text*)btn->getChildByName("lblCount");
        lbl->setString(Value(0).asString());
        level = UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, i).c_str(), 0);
        price = (1 + level)*20;
        btn->setEnabled(maxLevel > level);
        if(maxLevel <= level){
            btn->getChildByName("lblCount")->setVisible(false);
            lbl = (Text*)btn->getChildByName("lbl");
            lbl->setString("MAX");
            
        }else{
            lbl->setString(Value(price).asString());
        }
    }
}
void HudLayer::onUpgradeResearchClick(Ref* ref){
    BTN_FROM_REF
    int index = btn->getTag();
    Text* lbl = (Text*)btn->getChildByName("lblCount");
    int price = Value(lbl->getString()).asInt();
    int currencyType = 0;
    std::string crystalName;
    switch (index) {
        case 0:
            currencyType = CURRENCY_TYPE_GEM_GRAY;
            crystalName = "gray crystal";
            break;
        case 1:
            currencyType = CURRENCY_TYPE_GEM_GREEN;
            crystalName = "green crystal";
            break;
        case 2:
            currencyType = CURRENCY_TYPE_GEM_ORANGE;
            crystalName = "orange crystal";
            break;
        case 3:
            currencyType = CURRENCY_TYPE_GEM_RED;
            crystalName = "red crystal";
            break;
    }
    int youHave = GM->getCurrency(currencyType);
    if(youHave >= price){
        GM->addCurrency(-price, currencyType);
        int level = UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, index).c_str(), 0);
        level++;
        UDSetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, index).c_str(), level);
        if(index == CURRENCY_TYPE_GEM_GRAY){
            SM->saveUserData(strmake("crystal_gray=%d", GM->getCurrency(CURRENCY_TYPE_GEM_GRAY)));
        }else if(index == CURRENCY_TYPE_GEM_GREEN){
            SM->saveUserData(strmake("crystal_green=%d", GM->getCurrency(CURRENCY_TYPE_GEM_GREEN)));
        }else if(index == CURRENCY_TYPE_GEM_ORANGE){
            SM->saveUserData(strmake("crystal_orange=%d", GM->getCurrency(CURRENCY_TYPE_GEM_ORANGE)));
        }else if(index == CURRENCY_TYPE_GEM_RED){
            SM->saveUserData(strmake("crystal_red=%d", GM->getCurrency(CURRENCY_TYPE_GEM_RED)));
        }
        std::string str = strmake("%d,%d,%d,%d", UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 0).c_str()), UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 1).c_str()), UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 2).c_str()), UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 3).c_str()));;
        SM->saveUserData(strmake("research=%s", str.c_str()));
        
        updateResearchLayer();
    }else{
        showInstanceMessage(strmake(LM->getText("not enough format").c_str(), LM->getText(crystalName).c_str()));
    }
}
void HudLayer::onAchieveClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    
    Node* layer = CSLoader::createNode("Achieve.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    setTitle(layer->getChildByName("titleBack"), "achievement");
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    Text* lbl;
    int current, max, level;
    Sprite* spt;Sprite* sptFrame;
    for (int i = 0; i < 4; i++) {
        lbl = (Text*)background->getChildByName(strmake("lbl%d", i));
        current = getAchievementCurrent(i);
        max = getAchievementMax(i);
        level = getAchievementLevel(i);
        if(max < 0){
            LM->setLocalizedStringNotKey(lbl, strmake("%s(%s)", LM->getText(strmake("achieve name %d", i)).c_str(), LM->getText("complete").c_str()));
        }else{
            LM->setLocalizedStringNotKey(lbl, strmake("Lv.%d %s(%d/%d)", level+1, LM->getText(strmake("achieve name %d", i)).c_str(), current, max));
        }
        
        sptFrame = (Sprite*)background->getChildByName(strmake("spt%d", i));
        Vec2 extraPos;
        if(i == 0){
            spt = Sprite::createWithSpriteFrameName("pierrotBoss0_12.png");
            spt->setTextureRect(Rect(spt->getTextureRect().origin.x, spt->getTextureRect().origin.y, 21, 21));
            extraPos= Vec2(-1, 1);
        }else if(i == 1){
            spt = Sprite::createWithSpriteFrameName("pierrotBoss1_1.png");
            spt->setTextureRect(Rect(spt->getTextureRect().origin.x + 10, spt->getTextureRect().origin.y, 21, 21));
            extraPos= Vec2(-9, 2);
        }else if(i == 2){
            spt = Sprite::createWithSpriteFrameName("bigBossHead.png");
            spt->setTextureRect(Rect(spt->getTextureRect().origin.x + 30, spt->getTextureRect().origin.y + 45, 38, 38));
            spt->setScale(0.5f);
        }else if(i == 3){
            spt = Sprite::createWithSpriteFrameName("hoodGhost1.png");
        }
        spt->setPosition(sptFrame->getPosition() + extraPos);
        
        background->addChild(spt);
        
        Button* btn = (Button*)background->getChildByName(strmake("btn%d", i));
        btn->setEnabled(current >= max);
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onReceiveAchievementRewardClick, this));
        lbl = (Text*)btn->getChildByName("lblCount");
        int reward = getAchievementRewardGemCount(i, level);
        lbl->setString(Value(reward).asString());
        if(max < 0){
            btn->setVisible(false);
            
        }
    }
}
void HudLayer::onReceiveAchievementRewardClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    int index = btn->getTag();
    Text* lbl = (Text*)btn->getChildByName("lblCount");
    int gemCount = Value(lbl->getString()).asInt();
    int level = getAchievementLevel(index);
    level++;
    UDSetInt(strmake(KEY_ACHIEVE_LEVEL_FORMAT, index).c_str(), level);
    GM->addGem(gemCount);
    SM->saveUserData(strmake("gem=%d&achieve%d=%d", Value(GM->getGem()).asInt(), index, level*1000 + getAchievementCurrent(index)));
    closePopup();
    onAchieveClick();
    
    btn = showMessageBox("reward", "achievement complete", gemCount);
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
}
int HudLayer::getAchievementRewardGemCount(int index, int level){
    if (index == 0) {
        return 50 + level*50;
    }else if (index == 1) {
        return 100 + level*50;
    }else if (index == 2) {
        return 150 + level*50;
    }else if (index == 3) {
        return 200 + level*50;
    }
    return 10;
}
int HudLayer::getAchievementCurrent(int index){
    return UDGetInt(strmake(KEY_ACHIEVE_FORMAT, index).c_str());
}
int HudLayer::getAchievementMax(int index){
    int level;
    level = getAchievementLevel(index);
    if(index == 0){ // first boss
        if(level == 0){
            return 3;
        }else if(level == 1){
            return 7;
        }else if(level == 2){
            return 15;
        }else if(level == 3){
            return 30;
        }else if(level == 4){
            return 50;
        }
    }else if(index == 1){
        if(level == 0){
            return 2;
        }else if(level == 1){
            return 4;
        }else if(level == 2){
            return 7;
        }else if(level == 3){
            return 15;
        }else if(level == 4){
            return 30;
        }
    }else if(index == 2){
        if(level == 0){
            return 2;
        }else if(level == 1){
            return 4;
        }else if(level == 2){
            return 6;
        }else if(level == 3){
            return 8;
        }else if(level == 4){
            return 10;
        }
    }else if(index == 3){
        if(level == 0){
            return 1;
        }else if(level == 1){
            return 3;
        }else if(level == 2){
            return 5;
        }else if(level == 3){
            return 7;
        }else if(level == 4){
            return 9;
        }
    }
    return -1;
}
int HudLayer::getAchievementLevel(int index){
    return UDGetInt(strmake(KEY_ACHIEVE_LEVEL_FORMAT, index).c_str());
}
void HudLayer::onAttendClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    showIndicator();
    showAttendRequired = true;
    SM->getUserData("attend=1");
}
void HudLayer::onRuneClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("Rune.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("runePopup");
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    Button* btn = (Button*)layer->getChildByName("btnQuestion");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onRuneInfoClick, this));
    for (int i = 0; i < 4; i++) {
        btn = (Button*)background->getChildByName(strmake("btnSlot%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onRuneSlotForEquipClick, this));
    }
    for (int i = 0; i < 2; i++) {
        btn = (Button*)background->getChildByName(strmake("btnRankUp%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onRuneSlotForRankUpClick, this));
    }
    
    setTitle(layer->getChildByName("titleBack"), "rune");
    
    btn = (Button*)background->getChildByName("btnRankUp");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onRuneRankUpClick, this));
    btn = (Button*)background->getChildByName("btnAutoRegister");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onRuneAutoRegisterClick, this));
    updateRunePopup();
}
void HudLayer::onRuneRankUpClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    int gem = GM->getGem();
    Text* lbl = (Text*)btn->getChildByName("lblPrice");
    int price = Value(lbl->getString()).asInt();
    if(gem >= price){
        GM->addGem(-price);
        int index;
        int value;
        Node* btnRankUp;
        for (int i = 0; i < 2; i++) {
            btnRankUp = btn->getParent()->getChildByName(strmake("btnRankUp%d", i));
            index = btnRankUp->getTag();
            btnRankUp->setTag(-1);
            value = GM->getBagItem(index)->value;
            GM->removeBagContentWithoutUpdateOrder(index);
        }
        GM->updateBagItemIndices();
        GM->saveBagContent();
        showItemGetAndAddToBag(value + 1000);
        updateRunePopup();
//        HUD->showIndicator();
//        SM->saveUserData(strmake("inventory=%s", UDGetStr(KEY_BAG_CONTENT).c_str()));
    }else{
        btn->setEnabled(true);
        showInstanceMessage(LM->getText("not enough gem"));
    }
}
void HudLayer::onRuneAutoRegisterClick(){
    int candidateCount = 0;
    Node* layer = this->getChildByName("runePopup");
    Node* background = layer->getChildByName("imgBackground");
    int firstRune = -1;
    int secondRune = -1;
    BagItemInfo* item;
    for(int i = 0; i < GM->bagContentArray.size(); i++){
        item = GM->bagContentArray.at(i);
        if(GM->getItemType(item->value) == ITEM_TYPE_RUNE){
            for(int j = i+1; j < GM->bagContentArray.size(); j++){
                if(GM->bagContentArray.at(j)->value == item->value){
                    firstRune = i;
                    secondRune = j;
                    break;
                }
            }
        }
        if(firstRune >= 0){
            break;
        }
    }
    
    if(firstRune < 0){
        showInstanceMessage(LM->getText("not enough items in same tier"));
    }else{
        background->getChildByName(strmake("btnRankUp%d", 0))->setTag(firstRune);
        background->getChildByName(strmake("btnRankUp%d", 1))->setTag(secondRune);
        updateRunePopup();
    }
}
void HudLayer::onRuneSlotForRankUpClick(Ref* ref){
    BTN_FROM_REF
    if(btn->getTag() < 0){
        runeSlotIndexToRankUp = Value(btn->getName().substr(9, 1)).asInt();
        showBag(BAG_FILTER_RUNE, BAG_JOB_REGISTER_FOR_RUNE_RANK_UP);
    }else{
        btn->setTag(-1);
        updateRunePopup();
    }
}
void HudLayer::onRuneSlotForEquipClick(Ref* ref){
    BTN_FROM_REF
    runeSlotIndexToEquip = Value(btn->getName().substr(7, 1)).asInt();
    int value = UDGetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, runeSlotIndexToEquip).c_str(), -1);
    if(value < 0){
        showBag(BAG_FILTER_RUNE, BAG_JOB_REGISTER_FOR_RUNE_EQUIP);
    }else{
        UDSetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, runeSlotIndexToEquip).c_str(), -1);
        GM->addBagContent(value%1000, value/1000);
        GM->saveBagContent();
        updateRunePopup();
        HUD->showIndicator();
        SM->saveUserData(strmake("inventory=%s&rune_equip=%s", UDGetStr(KEY_BAG_CONTENT).c_str(), getRuneEquipData().c_str()));
    }
}
void HudLayer::updateRunePopup(){
    Node* layer = this->getChildByName("runePopup");
    Node* background = layer->getChildByName("imgBackground");
    Button* btn;
    int value;int index;int level;
    for (int i = 0; i < 4; i++) {
        btn = (Button*)background->getChildByName(strmake("btnSlot%d", i));
        value = UDGetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, i).c_str(), -1);
        index = value%1000;
        level = value/1000;
        btn->setTag(value);
        btn->removeChildByName("sptRune");
        btn->getChildByName("imgIcon")->setVisible(value < 0);
        if(value < 0){
            addStarsToIcon(btn, 0);
            continue;
        }
        
        Sprite* spt = GM->getItemSprite(index);
        btn->addChild(spt);
        spt->setName("sptRune");
        spt->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2 + 2));
        addStarsToIcon(btn, level+1);
    }
    Text* lbl;
    int att = 0;
    int def = 0;
    int hp = 0;
    int cri = 0;
    for (int i = 0; i < 4; i++) {
        btn = (Button*)background->getChildByName(strmake("btnSlot%d", i));
        value = UDGetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, i).c_str(), -1);
        index = value%1000;
        level = value/1000;
        btn->setTag(value);
        if(value < 0){
            continue;
        }
        
        if(index >= RUNE_A_RED && index <= RUNE_A_BLUE){
            att += GM->getRuneBuff(level, RUNE_BUFF_TYPE_ATTACK);
        }else if(index >= RUNE_D_RED && index <= RUNE_D_BLUE){
            def += GM->getRuneBuff(level, RUNE_BUFF_TYPE_DEF);
        }else if(index >= RUNE_S_RED && index <= RUNE_S_BLUE){
            hp += GM->getRuneBuff(level, RUNE_BUFF_TYPE_HP);
        }else if(index >= RUNE_Q_RED && index <= RUNE_Q_BLUE){
            cri += GM->getRuneBuff(level, RUNE_BUFF_TYPE_CRI);
        }
    }
    
    for (int i = 0; i < 4; i++) {
        lbl = (Text*)background->getChildByName(strmake("lblEffect%d", i));
        switch (i) {
            case 0:
                lbl->setString(strmake("ATT: %d%%", att));
                break;
            case 1:
                lbl->setString(strmake("S.DEF: %d%%", def));
                break;
            case 2:
                lbl->setString(strmake("S.HP: %d%%", hp));
                break;
            case 3:
                lbl->setString(strmake("CRI: %d%%", cri));
                break;
            default:
                break;
        }
        
    }
    bool isReady = true;
    for (int i = 0; i < 2; i++) {
        btn = (Button*)background->getChildByName(strmake("btnRankUp%d", i));
        int indexInBag = btn->getTag();
        
        btn->removeChildByName("sptRune");
        btn->getChildByName("imgIcon")->setVisible(indexInBag < 0);
        if(indexInBag < 0){
            isReady = false;
            addStarsToIcon(btn, 0);
            continue;
        }
        value = GM->getBagItem(indexInBag)->value;
        index = value%1000;
        level = value/1000;
        
        Sprite* spt = GM->getItemSprite(index);
        btn->addChild(spt);
        spt->setName("sptRune");
        spt->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2 + 2));
        addStarsToIcon(btn, level+1);
    }
    int index0 = background->getChildByName("btnRankUp0")->getTag();
    int index1 = background->getChildByName("btnRankUp1")->getTag();
    if(index0 < 0 || index1 < 0 || GM->getBagItem(index0)->value != GM->getBagItem(index1)->value){
        isReady = false;
    }
    
    btn = (Button*)background->getChildByName("btnRankUpResult");
    btn->removeChildByName("sptRune");
    btn->getChildByName("imgIcon")->setVisible(!isReady);
    if(isReady){
        Sprite* spt = GM->getItemSprite(index);
        btn->addChild(spt);
        spt->setName("sptRune");
        spt->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2 + 2));
        addStarsToIcon(btn, level+1);
        addStarsToIcon(btn, level+2);
    }else{
        addStarsToIcon(btn, 0);
    }
    btn = (Button*)background->getChildByName("btnRankUp");
    btn->setEnabled(isReady);
}
void HudLayer::onRuneInfoClick(){
    Node* layer = CSLoader::createNode("RuneDesc.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    setTitle(layer->getChildByName("titleBack"), "rune info");
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    ScrollView* sv = (ScrollView*)background->getChildByName("sv");
    sv->setScrollBarEnabled(false);
    Text* lbl;std::string color;std::string item;std::string stat;
    for (int i = 0; i < 16; i++) {
        lbl = (Text*)sv->getChildByName(strmake("lbl%d", i));
        if(i >= 0 && i <= 3){
            item = LM->getText("weapon");
            stat = LM->getText("power");
        }else if(i >= 4 && i <= 7){
            item = LM->getText("shield");
            stat = LM->getText("defence");
        }else if(i >= 8 && i <= 11){
            item = LM->getText("shield");
            stat = LM->getText("hp");
        }else if(i >= 12 && i <= 15){
            item = LM->getText("weapon");
            stat = LM->getText("critical possibility");
        }
        if(i%4 == 0){
            color = LM->getText("red");
        }else if(i%4 == 1){
            color = LM->getText("yellow");
        }else if(i%4 == 2){
            color = LM->getText("green");
        }else if(i%4 == 3){
            color = LM->getText("blue");
        }
        LM->setLocalizedStringNotKey(lbl, strmake(LM->getText("rune desc format").c_str(), color.c_str(), item.c_str(), stat.c_str()));
    }
}
void HudLayer::onPostClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("Post.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("postPopup");
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    setTitle(layer->getChildByName("titleBack"), "post");
    
    setCloseButtonAndBlock(layer);
    btn = (Button*)background->getChildByName("btnGetAll");
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "receive all");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onGetAllPostClick, this));
    
    getPostBoxItemFromServer();
}
void HudLayer::onGetAllPostClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    
    bool isFromServer = false;
    for (int i = 0; i < postBoxArray.size(); i++) {
        PostBoxItem* item = postBoxArray.at(i);
        std::string strMsg;
        if(item->postType == POST_TYPE_GEM){
            int gem = GM->getGem();
            log("gem: %d, value: %d", gem, item->value);
            GM->addGem(item->value);
            SM->saveUserData(strmake("gem=%d", Value(GM->getGem()).asInt()));
            strMsg = strmake("gem_%d", item->value);
        }else if(item->postType == POST_TYPE_LOCAL_TICKET){
            GM->localTicketCount += item->value;
            SM->saveUserData(strmake("local_ticket_count=%d", GM->localTicketCount));
            strMsg = strmake("localticket_%d", item->value);
        }else if(item->postType == POST_TYPE_NETWORK_TICKET){
            GM->networkTicketCount += item->value;
            SM->saveUserData(strmake("network_ticket_count=%d", GM->networkTicketCount));
            strMsg = strmake("networkticket_%d", item->value);
        }else if(item->postType == POST_TYPE_SEARCH_TICKET){
            GM->searchTicketCount += item->value;
            SM->saveUserData(strmake("search_ticket_count=%d", GM->searchTicketCount));
            strMsg = strmake("searchticket_%d", item->value);
        }
        if(LM->getText("access reward am79").compare(item->text) == 0){
            UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_AM79, true);
        }else if(LM->getText("access reward pm1214").compare(item->text) == 0){
            UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1214, true);
        }else if(LM->getText("access reward pm1720").compare(item->text) == 0){
            UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1720, true);
        }else if(LM->getText("access reward pm2022").compare(item->text) == 0){
            UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2022, true);
        }else if(LM->getText("access reward pm2224").compare(item->text) == 0){
            UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2224, true);
        }
    }
    postBoxArray.clear();
    
    if (isFromServer) {
        showIndicator();
        
        UDSetStr(KEY_POST_MESSAGES, "");
        SM->saveUserData("post=,");
        reopenPostPopup = true;
    }else{
        closePopup();
        onPostClick();
    }
}
void HudLayer::getPostBoxItemFromServer(){
    postBoxArray.clear();
    showIndicator();
    SM->getPostBoxItem();
}
void HudLayer::onPostReceivedFromServer(){
    hideIndicator();
    updatePostBox();
}
void HudLayer::updatePostBox(){
    postBoxArray.clear();
    double now = ((int)SM->getCurrentTime())%86400;
    int am7 = 25200;//60*60*7
    int am9 = 32400;//60*60*9
    int pm12 = 43200;//
    int pm14 = 50400;//
    int pm17 = 64800;//
    int pm20 = 72000;//
    int pm22 = 79200;//
    int am24 = 86400;//
    if (am7 <= now && now<= am9) {
        if(!UDGetBool(KEY_ACCEESS_REWARD_RECEIVED_AM79, false)){
            postBoxArray.push_back(getPostBoxItem(POST_TYPE_GEM, 30, LM->getText("access reward am79")));
        }
    }else if (pm12 <= now && now<= pm14) {
        if(!UDGetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1214, false)){
            postBoxArray.push_back(getPostBoxItem(POST_TYPE_LOCAL_TICKET, 10, LM->getText("access reward pm1214")));
        }
    }else if (pm17 <= now && now<= pm20) {
        if(!UDGetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1720, false)){
            postBoxArray.push_back(getPostBoxItem(POST_TYPE_NETWORK_TICKET, 10, LM->getText("access reward pm1720")));
        }
    }else if (pm20 <= now && now<= pm22) {
        if(!UDGetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2022, false)){
            postBoxArray.push_back(getPostBoxItem(POST_TYPE_GEM, 50, LM->getText("access reward pm2022")));
        }
    }else if (pm22 <= now && now<= am24) {
        if(!UDGetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2224, false)){
            postBoxArray.push_back(getPostBoxItem(POST_TYPE_NETWORK_TICKET, 10, LM->getText("access reward pm2224")));
        }
    }
    
    std::string strMessages = UDGetStr(KEY_POST_MESSAGES, "");
    ValueVector list = GM->split(strMessages, ",");
    ValueVector innerList;
    for (int i = 0; i < list.size(); i++) {
        if(list.at(i).asString().length() > 0){
            innerList = GM->split(list.at(i).asString(), "_");
            if(innerList.at(0).asString().compare("gem") == 0){
                postBoxArray.push_back(getPostBoxItem(POST_TYPE_GEM, innerList.at(1).asInt(), LM->getText("gift from operator")));
            }else if(innerList.at(0).asString().compare("localticket") == 0){
                postBoxArray.push_back(getPostBoxItem(POST_TYPE_LOCAL_TICKET, innerList.at(1).asInt(), LM->getText("gift from operator")));
            }else if(innerList.at(0).asString().compare("networkticket") == 0){
                postBoxArray.push_back(getPostBoxItem(POST_TYPE_NETWORK_TICKET, innerList.at(1).asInt(), LM->getText("gift from operator")));
            }else if(innerList.at(0).asString().compare("searchticket") == 0){
                postBoxArray.push_back(getPostBoxItem(POST_TYPE_SEARCH_TICKET, innerList.at(1).asInt(), LM->getText("gift from operator")));
            }
        }
    }
    int endDay = UDGetInt(KEY_MONTHLY_GEM_END_DATE, 0);
    bool isInstantRewardGet = UDGetBool(KEY_MONTHLY_GEM_IS_INSTANT_RECEIVED, false);
    int receiveDay = UDGetInt(KEY_MONTHLY_GEM_RECEIVE_DATE, 0);
    int today = SM->getCurrentTime()/86400;
    
    if (endDay >= today) {
        if (!isInstantRewardGet) {
            postBoxArray.push_back(getPostBoxItem(POST_TYPE_MONTHLY_INSTANT_GEM, 300, LM->getText("monthly instant reward")));
        }
        if(today > receiveDay){
            postBoxArray.push_back(getPostBoxItem(POST_TYPE_MONTHLY_DAILY_GEM, 100, LM->getText("monthly daily reward")));
        }
    }
    
    endDay = UDGetInt(KEY_MONTHLY_TICKET_END_DATE, 0);
    isInstantRewardGet = UDGetBool(KEY_MONTHLY_TICKET_IS_INSTANT_RECEIVED, false);
    receiveDay = UDGetInt(KEY_MONTHLY_TICKET_RECEIVE_DATE, 0);
    
    if (endDay >= today) {
        if (!isInstantRewardGet) {
            postBoxArray.push_back(getPostBoxItem(POST_TYPE_MONTHLY_INSTANT_TICKETS, 30, LM->getText("monthly instant reward")));
        }
        if(today > receiveDay){
            postBoxArray.push_back(getPostBoxItem(POST_TYPE_MONTHLY_DAILY_TICKETS, 15, LM->getText("monthly daily reward")));
        }
    }
    
    Node* layer = this->getChildByName("postPopup");
    Node* background = layer->getChildByName("imgBackground");
    
    ScrollView* sv = (ScrollView*)background->getChildByName("sv");
    Widget* itemTemp = (Widget*)sv->getChildByName("itemTemplate");
    Button* btn;Text* lbl;PostBoxItem* item;Widget* bar;
    float itemHeight = 31;
    int gap = 4;
    float scrollHeight = 109;
    float heightSum = postBoxArray.size()*(itemHeight + gap);
    if(heightSum > scrollHeight){
        scrollHeight = heightSum;
    }
    sv->setInnerContainerSize(Size(180, scrollHeight));
    ImageView* img;
    for (int i = 0; i < postBoxArray.size(); i++) {
        item = postBoxArray.at(i);
        bar = (Widget*)itemTemp->clone();
        sv->addChild(bar);
        img = (ImageView*)bar->getChildByName("img");
        if(item->postType == POST_TYPE_GEM){
            img->loadTexture("crystal.png");
        }else if(item->postType == POST_TYPE_LOCAL_TICKET){
            img->loadTexture("ticket.png");
            img->setContentSize(Size(17, 10));
        }else if(item->postType == POST_TYPE_NETWORK_TICKET){
            img->loadTexture("networkTicket.png");
            img->setContentSize(Size(17, 10));
        }else if(item->postType == POST_TYPE_SEARCH_TICKET){
            img->loadTexture("ticket.png");
            img->setContentSize(Size(17, 10));
            img->setColor(Color3B::GREEN);
        }else if(item->postType == POST_TYPE_MONTHLY_INSTANT_TICKETS ||
                 item->postType == POST_TYPE_MONTHLY_DAILY_TICKETS){
            img->loadTexture("ticket.png");
            img->setContentSize(Size(17, 10));
            img->setPosition(img->getPosition() + Vec2(-5, 2));
            ImageView* imgClone = (ImageView*)img->clone();
            imgClone->loadTexture("networkTicket.png");
            imgClone->setContentSize(Size(17, 10));
            img->getParent()->addChild(imgClone);
            imgClone->setPosition(img->getPosition() + Vec2(3, -3));
        }
        
        lbl = (Text*)bar->getChildByName("lblValue");
        lbl->setString(Value(item->value).asString());
        lbl = (Text*)bar->getChildByName("lblTitle");
        LM->setLocalizedStringNotKey(lbl, item->text);
        bar->setPosition(Vec2(0, scrollHeight - itemHeight/2 - i*(itemHeight + gap)));
        btn = (Button*)bar->getChildByName("btnGet");
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onReceivePostItemClick, this));
        btn->setTag(i);
        lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "receive");
    }
    
    
    
    itemTemp->removeFromParent();
}
void HudLayer::onReceivePostItemClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    PostBoxItem* item = postBoxArray.at(btn->getTag());
    std::string strMsgToErase;
    postBoxArray.erase(postBoxArray.begin() + btn->getTag());
    
    showIndicator();
    
    std::string strAccessReward = "";
    if(item->postType == POST_TYPE_GEM){
        if(LM->getText("access reward am79").compare(item->text) == 0){
            UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_AM79, true);
            strAccessReward = strmake("&%s", getAccessRewardStringForServer().c_str());
        }else if(LM->getText("access reward pm2022").compare(item->text) == 0){
            UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2022, true);
            strAccessReward = strmake("&%s", getAccessRewardStringForServer().c_str());
        }
        int gem = GM->getGem();
        log("gem: %d, value: %d", gem, item->value);
        GM->addGem(item->value);
        SM->saveUserData(strmake("gem=%d%s", Value(GM->getGem()).asInt(), strAccessReward.c_str()));
        strMsgToErase = strmake("gem_%d", item->value);
    }else if(item->postType == POST_TYPE_LOCAL_TICKET){
        if(LM->getText("access reward pm1214").compare(item->text) == 0){
            UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1214, true);
            strAccessReward = strmake("&%s", getAccessRewardStringForServer().c_str());
        }
        GM->localTicketCount += item->value;
        SM->saveUserData(strmake("local_ticket_count=%d%s", GM->localTicketCount, strAccessReward.c_str()));
        strMsgToErase = strmake("localticket_%d", item->value);
    }else if(item->postType == POST_TYPE_NETWORK_TICKET){
        if(LM->getText("access reward pm1720").compare(item->text) == 0){
            UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1720, true);
            strAccessReward = strmake("&%s", getAccessRewardStringForServer().c_str());
        }else if(LM->getText("access reward pm2224").compare(item->text) == 0){
            UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2224, true);
            strAccessReward = strmake("&%s", getAccessRewardStringForServer().c_str());
        }
        GM->networkTicketCount += item->value;
        SM->saveUserData(strmake("network_ticket_count=%d%s", GM->networkTicketCount, strAccessReward.c_str()));
        strMsgToErase = strmake("networkticket_%d", item->value);
    }else if(item->postType == POST_TYPE_SEARCH_TICKET){
        GM->searchTicketCount += item->value;
        SM->saveUserData(strmake("search_ticket_count=%d", GM->searchTicketCount));
        strMsgToErase = strmake("searchticket_%d", item->value);
    }else if(item->postType >= POST_TYPE_MONTHLY_INSTANT_GEM && item->postType <= POST_TYPE_MONTHLY_DAILY_TICKETS){
        int today = SM->getCurrentTime()/86400;
        if (item->postType == POST_TYPE_MONTHLY_INSTANT_GEM) {
            GM->addGem(item->value);
            SM->saveUserData(strmake("gem=%d&monthly_gem_instant_receive=1", Value(GM->getGem()).asInt()));
            UDSetBool(KEY_MONTHLY_GEM_IS_INSTANT_RECEIVED, true);
        }else if (item->postType == POST_TYPE_MONTHLY_INSTANT_TICKETS) {
            GM->localTicketCount += item->value;
            GM->networkTicketCount += item->value;
            SM->saveUserData(strmake("local_ticket_count=%d&network_ticket_count=%d&monthly_ticket_instant_receive=1", GM->localTicketCount, GM->networkTicketCount));
            UDSetBool(KEY_MONTHLY_TICKET_IS_INSTANT_RECEIVED, true);
        }else if (item->postType == POST_TYPE_MONTHLY_DAILY_GEM) {
            GM->addGem(item->value);
            SM->saveUserData(strmake("gem=%d&monthly_gem_last_receive_date=%d", GM->getGem(), today));
            UDSetInt(KEY_MONTHLY_GEM_RECEIVE_DATE, today);
            HUD->setDailyMissionState(DAILY_MISSION_GET_MONTHLY, MISSION_STATE_CLEAR);
        }else if (item->postType == POST_TYPE_MONTHLY_DAILY_TICKETS) {
            GM->localTicketCount += item->value;
            GM->networkTicketCount += item->value;
            SM->saveUserData(strmake("local_ticket_count=%d&network_ticket_count=%d&monthly_ticket_last_receive_date=%d", GM->localTicketCount, GM->networkTicketCount, today));
            UDSetInt(KEY_MONTHLY_TICKET_RECEIVE_DATE, today);
            HUD->setDailyMissionState(DAILY_MISSION_GET_MONTHLY, MISSION_STATE_CLEAR);
        }
//        수련 - 입장권 > 경험치 + 무기 || 스킬강화 재료
//        수련 - 로컬 티켓 > 경험치 + 무기 + 미션 재료(방패)
//        배틀 - 네트워크 티켓 > 경험치 + 미션 재료(룬)
    }
    
    
    std::string strMessages = UDGetStr(KEY_POST_MESSAGES, "");
    GM->eraseSubStr(strMessages, strMsgToErase);
    UDSetStr(KEY_POST_MESSAGES, strMessages);
    SM->saveUserData(strmake("post=%s", strMessages.c_str()));
    reopenPostPopup = true;
}
std::string HudLayer::getAccessRewardStringForServer(){
    return strmake("access_reward_receive=%d%d%d%d%d",
                   UDGetBool(KEY_ACCEESS_REWARD_RECEIVED_AM79, false)?1:0,
                   UDGetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1214, false)?1:0,
                   UDGetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1720, false)?1:0,
                   UDGetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2022, false)?1:0,
                   UDGetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2224, false)?1:0);
}
PostBoxItem* HudLayer::getPostBoxItem(int postType, int value, std::string text){
    PostBoxItem* item = new PostBoxItem();
    item->postType = postType;
    item->value = value;
    item->text = text;
    return item;
}
void HudLayer::onFAQClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    
}
void HudLayer::hideBtns(){
    if (joystickAim->getPositionY() < 0) {
        return;
    }
    for (auto btn: btns){
        btn->setTag(btn->getPositionY());
        btn->runAction(MoveBy::create(0.1f, Point(0, -300)));
    }
    joystickAim->runAction(MoveBy::create(0.1f, Point(0, -300)));
    joystickLR->runAction(MoveBy::create(0.1f, Point(0, -300)));
    joystickAim->setTag(joystickAim->getPositionY());
    joystickLR->setTag(joystickLR->getPositionY());
}
void HudLayer::toggleAuto(bool showMsg){
    bool isAuto = !GameManager::getInstance()->getCurrentStageLayer()->isAutoTargetingOn;
    GameManager::getInstance()->getCurrentStageLayer()->isAutoTargetingOn = isAuto;
    btnShoot->setVisible(isAuto);
    joystickAim->setVisible(!isAuto);
    js2->setVisible(!isAuto);
    Sprite* spt = Sprite::create(isAuto?"UI/btnToggleManual.png":"UI/btnToggleAuto.png");
    btnToggle->setSpriteFrame(spt->getSpriteFrame());
    UserDefault::getInstance()->setBoolForKey(KEY_AUTO_TARGET_SELECTED, isAuto);
    if (showMsg) {
        GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText(isAuto?"auto targeting":"manual targeting").c_str(), this);
    }
}
void HudLayer::setScore(int score){
    if(GameManager::getInstance()->currentStageIndex >= 0){
        lblScore->setString(__String::createWithFormat("Score: %d", GameManager::getInstance()->totalScore)->getCString());
    }
}

void HudLayer::registerControllerListener()
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
        _listener->onConnected = CC_CALLBACK_2(HudLayer::onConnectController,this);
        
        //bind disconnect event call function
        _listener->onDisconnected = CC_CALLBACK_2(HudLayer::onDisconnectedController,this);
        
        //bind onKeyDown event call function
        _listener->onKeyDown = CC_CALLBACK_3(HudLayer::onKeyDown, this);
        
        //bind onKeyUp event call function
        _listener->onKeyUp = CC_CALLBACK_3(HudLayer::onKeyUp, this);
        
        //bind onAxis event call function, onAxis will be called when analog stick is changed
        _listener->onAxisEvent = CC_CALLBACK_3(HudLayer::onAxisEvent, this);
        
        //Activate the listener into the event dispatcher
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_listener, this);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        //This function should be called for iOS platform
        Controller::startDiscoveryController();
#endif
        
    }
}

//Controller is the obejects of the Controller，keyCode means the keycode of the controller you click down
void HudLayer::onKeyDown(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event)
{
    CCLOG("KeyDown:%d", keyCode);
}

void HudLayer::onKeyUp(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event)
{
    //You can get the controller by tag, deviceId or devicename if there are multiple controllers
    CCLOG("tag:%d DeviceId:%d DeviceName:%s", controller->getTag(), controller->getDeviceId(), controller->getDeviceName().c_str());
    CCLOG("KeyUp:%d", keyCode);
}

//The axis includes X-axis and Y-axis and its range is from -1 to 1. X-axis is start from left to right and Y-axis is bottom to top.
void HudLayer::onAxisEvent(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event)
{
//    const auto& keyStatus = controller->getKeyStatus(keyCode);
//    CCLOG("Axis KeyCode:%d Axis Value:%f", keyCode, keyStatus.value);
}

void HudLayer::onConnectController(Controller* controller, Event* event)
{
    CCLOG("Game controller connected");
}

void HudLayer::onDisconnectedController(Controller* controller, Event* event)
{
    CCLOG("Game controller disconnected");
}
const char* HudLayer::getWeaponName(int index){
    GameManager::getInstance()->getWeaponName(index);
    return "";
}
void HudLayer::addHeartAnimated(Point pos){
    Sprite* heart = lifeArray.at(lifeArray.size() - 1);
    Sprite* spt = Sprite::create("heart.png");
    spt->setPosition(pos);
    this->addChild(spt);
    spt->runAction(Sequence::create(EaseIn::create(MoveTo::create(1, heart->getPosition() + Point(50, 0)), 3), CallFuncN::create(CC_CALLBACK_1(HudLayer::addHeartDone, this)), CallFuncN::create(CC_CALLBACK_1(HudLayer::spriteMoveDone, this)), NULL));
}
void HudLayer::addHeartDone(Ref* obj){
    setLife(lifeArray.size() + 1);
    Sprite* heart = lifeArray.at(lifeArray.size() - 1);
    
    Sprite* spt = Sprite::create("backLight.png");
    spt->setScale(0.5);
    spt->runAction(Sequence::create(RotateBy::create(1, 300), CallFuncN::create(CC_CALLBACK_1(HudLayer::spriteMoveDone, this)), NULL));
    
    heart->addChild(spt);
    spt->setPosition(Point(heart->getContentSize().width/2, heart->getContentSize().height/2));
    
    GameManager::getInstance()->playSoundEffect(SOUND_MEDICINE);
}
// Implementation of the keyboard event callback function prototype
void HudLayer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (isInScene || GameManager::getInstance()->getCurrentStageLayer()->indexToWarp != -9999) {
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
        GameManager::getInstance()->getCurrentStageLayer()->changeWeapon((int)keyCode - (int)EventKeyboard::KeyCode::KEY_1);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_E){
        GameManager::getInstance()->getCurrentStageLayer()->takeAction();
    }else if(keyCode == EventKeyboard::KeyCode::KEY_TAB){
        GameManager::getInstance()->getCurrentStageLayer()->showBigMiniMap(true);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_ENTER){
        tryResultOK();
    }else if(keyCode == EventKeyboard::KeyCode::KEY_ESCAPE){
//        if(GameManager::getInstance()->currentStageIndex == STAGE_LOBBY){
//            auto scene = Scene::create();
//            scene->addChild(Title::create());
//            Director::getInstance()->replaceScene(scene);
//        }
//        removeListener();
        if (popupArray.size() == 0) {
            askExit();
        }else{
            closePopup();
        }
    }
}

void HudLayer::askExit(){
    Node* layer = CSLoader::createNode("Exit.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    Node* background = layer->getChildByName("imgBackground");
    setTitle(layer->getChildByName("titleBack"), "exit");
    Text* lbl = (Text*)background->getChildByName("lbl");
    LM->setLocalizedString(lbl, "exit game");
    
    Button* btn = (Button*)background->getChildByName("btnNo");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    btn = (Button*)background->getChildByName("btnYes");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onExitYesClick, this));
}
void HudLayer::onExitYesClick(){
    GM->exitGame();
}
void HudLayer::tryResultOK()
{
    if(GameManager::getInstance()->getCurrentStageLayer()->resultLayer != nullptr){
        if(GameManager::getInstance()->getCurrentStageLayer()->resultDone){
            if (GameManager::getInstance()->isGameClear) {
                GameManager::getInstance()->getCurrentStageLayer()->resultLayer->removeFromParentAndCleanup(true);
            }else{
                removeListener();
                GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
                Director::getInstance()->replaceScene(HelloWorld::scene(STAGE_LOBBY, false));
            }
        }else{
            GameManager::getInstance()->getCurrentStageLayer()->unschedule(schedule_selector(HelloWorld::updateResult));
            GameManager::getInstance()->getCurrentStageLayer()->schedule(schedule_selector(HelloWorld::updateResult));
        }
    }
}
void HudLayer::showTyping(){
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
    
    this->schedule(schedule_selector(HudLayer::typingCursorBlinking), 1);
}
void HudLayer::typing(std::string str){
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
void HudLayer::typingCursorBlinking(float dt){
    if (lblTyping->getString().size() != originalString.size()) {
        lblTyping->setString(originalString);
    }else{
        lblTyping->setString(originalString + "_");
    }
}
void HudLayer::showStageTitle(){
    Sprite* sptBlack = Sprite::create("whiteBigCircle.png");
    sptBlack->setColor(Color3B::BLACK);
    this->addChild(sptBlack, 10000);
    sptBlack->setPosition(size/2);
    sptBlack->setScale(5);
    sptBlack->runAction(ScaleTo::create(0.5f, 15));
    
    hideBtns();
    
    this->setVisible(true);
    
    char bufTitle[100];
    char bufStage[50];
    int stageIndex = GameManager::getInstance()->getCurrentStageLayer()->indexToWarp;//GameManager::getInstance()->currentStageIndex;
    if (stageIndex == -1) {
        sprintf(bufStage, "%s", LanguageManager::getInstance()->getText("stageLobby").c_str());
    }else{
        if (GameManager::getInstance()->getCurrentStageLayer()->stageIndex >= 0 && !GameManager::getInstance()->getCurrentStageLayer()->isBossMap) {
            sprintf(bufStage, "BOSS");
        }else{
            sprintf(bufStage, "STAGE %d", stageIndex + 1);
        }
    }
    
    if(stageIndex >= TUTORIAL_STAGE_START && stageIndex <= TUTORIAL_STAGE_END){
//        sprintf(bufStage, "%s %d", LanguageManager::getInstance()->getText("lesson").c_str(), stageIndex - TUTORIAL_STAGE_START + 1);
        sprintf(bufStage, "LESSON %d", stageIndex - TUTORIAL_STAGE_START + 1);
        if (stageIndex == TUTORIAL_STAGE_END) {
            sprintf(bufTitle, "");
//            sprintf(bufStage, "%s", LanguageManager::getInstance()->getText("theLastLesson").c_str());
            sprintf(bufStage, "LAST LESSON");
        }else{
            
        }
        sprintf(bufTitle, "%s", LanguageManager::getInstance()->getText(__String::createWithFormat("tutorialTitle%d", stageIndex - TUTORIAL_STAGE_START)->getCString()).c_str());
    }else if(stageIndex == STAGE_NAMING){
        sprintf(bufTitle, "%s", LanguageManager::getInstance()->getText(__String::createWithFormat("stageTitle%d", stageIndex)->getCString()).c_str());
        sprintf(bufStage, "%s", LanguageManager::getInstance()->getText("stageTypeYourName").c_str());
        showTyping();
    }else{
        if (GameManager::getInstance()->getCurrentStageLayer()->stageIndex >= 0 && !GameManager::getInstance()->getCurrentStageLayer()->isBossMap) {
            sprintf(bufTitle, "%s", LanguageManager::getInstance()->getText(__String::createWithFormat("bossTitle%d", stageIndex)->getCString()).c_str());
        }else{
            sprintf(bufTitle, "%s", LanguageManager::getInstance()->getText(__String::createWithFormat("stageTitle%d", stageIndex)->getCString()).c_str());
        }
        
    }
    float y = size.height*3/4 + 20;
    Label* lblTitle = Label::createWithTTF(bufTitle, "visitor1.ttf", 30);
    lblTitle->setColor(Color3B(235, 235, 235));
    lblTitle->setAnchorPoint(Point(0.5, 0.5));
    lblTitle->setPosition(Point(size.width/2, y));
    this->addChild(lblTitle, 10000);
    lblTitle->setOpacity(0);
    
    
    Label* lblStage = Label::createWithTTF(bufStage, "visitor1.ttf", 30);//LanguageManager::getInstance()->getLocalizedLabel(bufStage, Color4B(235, 235, 235, 255));
    lblTitle->setColor(Color3B(235, 235, 235));
    lblStage->setAnchorPoint(Point(0.5, 0.5));
    lblStage->setPosition(Point(size.width/2, y - lblTitle->getBoundingBox().size.height - 2));
    this->addChild(lblStage, 10000);
    lblStage->setOpacity(0);
    
    lineLength = lblTitle->getBoundingBox().size.width>lblStage->getBoundingBox().size.width?lblTitle->getBoundingBox().size.width:lblStage->getBoundingBox().size.width;
    currentLineLength =0;
 
    dnTitleLine = DrawNode::create();
    this->addChild(dnTitleLine, 10000);
    dnTitleLine->setPosition(lblTitle->getBoundingBox().origin + Point(0, -4));
    
    
    float dur = 0.5f;
    sptWhiteGun = Sprite::create("whiteGun.png");
    sptWhiteGun->setAnchorPoint(Point(0.2, 0.3));
    sptWhiteGun->setScale(3);
    sptWhiteGun->setPosition(Point(dnTitleLine->getPosition() + Point(-60, -13)));
    this->addChild(sptWhiteGun, 10000);
    sptWhiteGun->runAction(Sequence::create(RotateBy::create(dur, 360), CallFunc::create(CC_CALLBACK_0(HudLayer::whiteGunFired, this)), EaseOut::create(RotateBy::create(0.1, -30), 2), RotateBy::create(0.4, 30), DelayTime::create(2.0f), FadeOut::create(1.0f), NULL));
    
    Sprite* sptGunEffect = Sprite::create("whiteGunFireEffect.png");
    sptGunEffect->setAnchorPoint(Point(0, 0.5));
    sptGunEffect->setScale(3);
    sptGunEffect->setOpacity(0);
    sptGunEffect->setPosition(sptWhiteGun->getPosition() + Point(50, 15));
    sptGunEffect->runAction(Sequence::create(DelayTime::create(dur), FadeIn::create(0.05f), DelayTime::create(0.1f), FadeOut::create(0.05f), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptGunEffect)), NULL));
    this->addChild(sptGunEffect, 10000);
    
    lblTitle->runAction(Sequence::create(DelayTime::create(dur + 0.5f), FadeIn::create(0.5f), DelayTime::create(1.5f), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, lblTitle)), NULL));
    lblStage->runAction(Sequence::create(DelayTime::create(dur + 0.5f), FadeIn::create(0.5f), DelayTime::create(1.5f), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, lblStage)), NULL));
    
}
void HudLayer::stageTitleLineUpdate(float dt){
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
        this->unschedule(schedule_selector(HudLayer::stageTitleLineUpdate));
    }
}
void HudLayer::whiteGunFired(){
    GM->playSoundEffect(SOUND_GUN_FIRE_0);
    this->schedule(schedule_selector(HudLayer::stageTitleLineUpdate));
}
void HudLayer::showPrincessSmallTalk(std::string str){
    if (dialogBox == nullptr) {
        dialogBox = AwesomeDialogBox::create();
        this->addChild(dialogBox, TOP_MOST_ZORDER - 1);
        setAsPopup(dialogBox);
    }
    CallFunc* call = CallFunc::create(CC_CALLBACK_0(HudLayer::onPrincessSmallTalkDone, this));
    call->retain();
    dialogBox->ShowDialog(str.c_str(), this, call, Sprite::create("princessTall.png"), 6);
}
void HudLayer::onPrincessSmallTalkDone(){
    closePopup();
}
void HudLayer::showTutorialDialog(){
    std::string str;
    if (GM->tutorialType == TUTORIAL_DUNGEON) {
        if (GM->tutorialIndex > 8) {
//            GM->tutorialType = TUTORIAL_MENU; // test
//            GM->tutorialIndex = 0; // test
//            return; // test
            sptArrowToTutorialDungeon = Sprite::create("arrowUpIcon.png");
            WORLD->addChild(sptArrowToTutorialDungeon, WORLD->player->getLocalZOrder());
            GM->makeItSiluk(sptArrowToTutorialDungeon);
            closePopup();
            return;
        }else{
            str = LM->getText(strmake("tutorial dungeon %d", GM->tutorialIndex));
        }
    }
    if (GM->tutorialType == TUTORIAL_MENU) {
        if (GM->tutorialIndex > 12) {

        }else{
            str = LM->getText(strmake("tutorial menu %d", GM->tutorialIndex));
        }
    }
    if (GM->tutorialType == TUTORIAL_AFTER_BATTLE) {
        if (GM->tutorialIndex > 5) {
            closePopup();
            return;
        }else{
            str = LM->getText(strmake("tutorial after battle %d", GM->tutorialIndex));
        }
    }
    if (dialogBox == nullptr) {
        dialogBox = AwesomeDialogBox::create();
        this->addChild(dialogBox, TOP_MOST_ZORDER - 1);
        setAsPopup(dialogBox);
    }
    CallFunc* call = CallFunc::create(CC_CALLBACK_0(HudLayer::onTutorialDialogDone, this));
    call->retain();
    if(GM->tutorialIndex%5==0){
        dialogBox->ShowDialog(str.c_str(), this, call, Sprite::create("princessTallSmile.png"), 6);
    }else{
        dialogBox->ShowDialog(str.c_str(), this, call, Sprite::create("princessTall.png"), 6);
    }
    GM->tutorialIndex++;
}

void HudLayer::onTutorialDialogDone(){
//    dialogBox->removeFromParent();
//    dialogBox = nullptr;
    bool closePopupRequired = true;
    if(GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 9){
        attachArrowForTutorial(hudLayer->getChildByName("btnMenu"));
    }else if(GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 10){
        attachArrowForTutorial(giftLayer->getChildByName("btnClose"));
    }else if(GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 11){
        attachArrowForTutorial(menuLayer->getChildByName("btnClose"));
    }else if(GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 13){
//        attachArrowForTutorial(menuLayer->getChildByName("btnClose"));
    }else if(GM->tutorialType == TUTORIAL_AFTER_BATTLE && GM->tutorialIndex == 6){
        closePopupRequired = false;
        closePopup();
        Button* btn = showMessageBox("reward", "tutorial complete", 100);
        GM->addGem(100);
        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
        gemBar->setVisible(true);
        GM->isTutorialOn = false;
        SM->saveUserData(strmake("tutorial=0&gem=%d", GM->getGem()));
    }else{
        showTutorialDialog();
        closePopupRequired = false;
    }
    if(closePopupRequired){
        closePopup();
    }
}
void HudLayer::attachArrowForTutorial(Node* btn){
    if(btn->getChildByName("arrow")) return;
    Sprite* spt = Sprite::create("arrowUpIcon.png");
    btn->addChild(spt);
    spt->setName("arrow");
    spt->setAnchorPoint(Vec2(0.5, 0));
    spt->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height));
    spt->setRotation(180);
    float dur = 0.3f;
    float distance = 10;
    spt->runAction(RepeatForever::create(Sequence::create(MoveBy::create(dur, Vec2(0, distance)), MoveBy::create(dur, Vec2(0, -distance)), NULL)));
}
void HudLayer::showDialog(std::string str){
    if (dialogBox == nullptr) {
        dialogBox = AwesomeDialogBox::create();
        this->addChild(dialogBox);
        setAsPopup(dialogBox);
    }
    CallFunc* call = CallFunc::create(CC_CALLBACK_0(HudLayer::onDialogDone, this));
    call->retain();
    dialogBox->ShowDialog(str.c_str(), this, call);
//    hideBtns();
}
void HudLayer::onDialogDone(){
    if(dialogBox != nullptr){
        closePopup();
    }
    if(UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_DOES_IT_TALKED_FORMAT, "story4")->getCString(), false)){
        UDSetInt(KEY_CLEAR_COUNT, 0);
        showEnding();
    }
}
void HudLayer::showBtns(){
    if (joystickAim->getPositionY() > 0) {
        return;
    }
    for (auto btn: btns){
        btn->runAction(MoveTo::create(0.1f, Point(btn->getPositionX(), btn->getTag())));
    }
    joystickAim->runAction(MoveTo::create(0.1f, Point(joystickAim->getPositionX(), joystickAim->getTag())));
    joystickLR->runAction(MoveTo::create(0.1f, Point(joystickLR->getPositionX(), joystickLR->getTag())));
}
void HudLayer::showEnding(){
    hideBtns();
    HelloWorld* stage = (HelloWorld*)GameManager::getInstance()->getCurrentStageLayer();
    stage->unschedule(schedule_selector(HelloWorld::gravityUpdate));
    stage->runAction(MoveBy::create(0.5, Point(-100, -160)));
    isInScene = true;
    this->setVisible(false);
    
    Hero* player = GameManager::getInstance()->getCurrentStageLayer()->player;
    player->freezed = true;
    NPCBase* theKnight;
    HoodGhost* son;
    for(auto knight : GameManager::getInstance()->getCurrentStageLayer()->npcArray){
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
    dad->setPosition(player->getPosition() + Point(250, 90));
    dad->setFlippedX(true);
    dad->runAction(Sequence::create(DelayTime::create(2.0), MoveTo::create(0.5, player->getPosition() + Point(110, 50)), MoveTo::create(8, player->getPosition() + Point(100, 45)), NULL));
    dad->setScale(2);
    theKnight->setPosition(player->getPosition() + Point(160, 180));
    theKnight->runAction(Sequence::create(MoveTo::create(0.5, player->getPosition() + Point(30, 50)), MoveTo::create(4, player->getPosition() + Point(16, 40)), NULL));
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
    
    this->scheduleOnce(schedule_selector(HudLayer::showCredit), 6.5f);
}
void HudLayer::showCredit(float dt){
    this->setVisible(true);
    Node* credit = CSLoader::createNode("Credit.csb");
    credit->setOpacity(0);
    credit->runAction(FadeIn::create(1));
    credit->setPositionX(size.width/2 - credit->getContentSize().width/2);
    
    this->addChild(credit, 1000);
    float dur = 15;
    float delay = 3;
    credit->getChildByName("labels")->runAction(Sequence::create(DelayTime::create(delay), MoveBy::create(dur, Point(0, 1900)), CallFunc::create(CC_CALLBACK_0(HudLayer::onEndCredit, this)), NULL));
    credit->getChildByName("labels")->runAction(Sequence::create(DelayTime::create(delay + dur-0.5f), FadeOut::create(1), NULL));
}
void HudLayer::onEndCredit(){
    removeListener();
    
    Scene* scene = Scene::create();
    scene->addChild(Title::create());
//    scene->addChild(HelloWorld::scene(STAGE_LOBBY, false));
    Director::getInstance()->replaceScene(scene);
}
void HudLayer::showNextTalk(){
    dialogBox->showNextTalk();
}
void HudLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (GameManager::getInstance()->getCurrentStageLayer()->indexToWarp != -9999) {
        return;
    }
    if(keyCode == EventKeyboard::KeyCode::KEY_Z ||
       keyCode == EventKeyboard::KeyCode::KEY_DPAD_CENTER){
        GameManager::getInstance()->buttonUp(BUTTON_A);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_E){
        if(HUD && HUD->popupArray.size() == 0){
            GM->getCurrentStageLayer()->reload();
        }
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
//    }else if(keyCode == EventKeyboard::KeyCode::KEY_BACK){
        resumeGame(NULL, ui::Widget::TouchEventType::ENDED);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_TAB){
        GameManager::getInstance()->getCurrentStageLayer()->showBigMiniMap(false);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_Y){
        onDialogDone();
    }
//    CCLOG("Key with keycode %d released", keyCode);
}
void HudLayer::useBomb(){
    if (GameManager::getInstance()->developer) {
        GameManager::getInstance()->getCurrentStageLayer()->useBomb();
        return;
    }
    int bombCount = GameManager::getInstance()->getBombCount();
    if (bombCount > 0) {
        GameManager::getInstance()->getCurrentStageLayer()->useBomb();
        bombCount--;
        GameManager::getInstance()->setBombCount(bombCount);
    }else{
        suggestBombShop();
    }
}
void HudLayer::suggestBombShop(){
    // show Bomb layer
    CCLOG("show bomb layer");
}
void HudLayer::buyBomb(){
    
}

void HudLayer::usePotion(){
    if (GameManager::getInstance()->developer) {
        GameManager::getInstance()->getCurrentStageLayer()->usePotion();
        return;
    }
    int potionCount = GameManager::getInstance()->getPotionCount();
    if (potionCount > 0) {
        GameManager::getInstance()->getCurrentStageLayer()->usePotion();
        potionCount--;
        GameManager::getInstance()->setPotionCount(potionCount);
    }else{
        suggestPotionShop();
    }
}
void HudLayer::suggestPotionShop(){
    // show Potion layer
    CCLOG("show potion layer");
}
void HudLayer::buyPotion(){
    
}
void HudLayer::buyGems(){
    
}
void HudLayer::keyDown(int key) {
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

void HudLayer::keyUp(int key) {
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
void HudLayer::showTalk(const char* talk){
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
void HudLayer::hideTalk(){
    talkLabel->runAction(FadeOut::create(0.5));
    talkLabel->runAction(MoveBy::create(0.5, Point(0, 40)));
}
void HudLayer::showCoinShopLayer(){
    showShopLayer();
//    ((ShopLayer*)GameManager::getInstance()->getShopLayer())->openShop();
}

void HudLayer::showAchievementComplete(const char* text){
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
    
    achieveNode->runAction(Sequence::create(EaseIn::create(MoveBy::create(0.5f, Point(0, -achieveNode->getContentSize().height)),0.2f), CallFuncN::create(CC_CALLBACK_1(HudLayer::achievementEffect, this)), DelayTime::create(1), EaseIn::create(MoveBy::create(0.5f, Point(0, achieveNode->getContentSize().height)),4), CallFuncN::create(CC_CALLBACK_1(HudLayer::nodeMoveDone, this)), NULL));
}

void HudLayer::achievementEffect(Node* node){
    Sprite* achieveNode = (Sprite*)node;
    Sprite* checkSprite = Sprite::create("iconCheck.png");
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
    particle->setTexture(Director::getInstance()->getTextureCache()->addImage("iconCheck.png"));          // 이미지 변경
    achieveNode->addChild(particle);
    
    achieveNode->addChild(checkSprite);
}
void HudLayer::nodeMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Node* node = (Node*)obj;
    node->stopAllActions();
    node->removeFromParentAndCleanup(true);
}

void HudLayer::setButtonSize(float siz) // 0 ~ 1
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

void HudLayer::updateCoin(int howMuch)
{
//    Text* lbl = (Text*)hudLayer->getChildByName("btnCoin")->getChildByName("lbl");
    Text* lbl = (Text*)hudLayer->getChildByName("lblCoin");
    lbl->setString(__String::createWithFormat("%d", howMuch)->getCString());
}
void HudLayer::updateGem(int howMuch)
{
    Text* lbl = (Text*)this->getChildByName("btnCrystal")->getChildByName("lbl");
    lbl->setString(__String::createWithFormat("%d", howMuch)->getCString());
    Node* imgGem = this->getChildByName("btnCrystal")->getChildByName("imgGem");
    GM->alignToCenter(imgGem, lbl, 4, this->getChildByName("btnCrystal")->getContentSize().width/2, 0);
}

void HudLayer::enableJoystick(bool enable)
{
    for(auto btn: buttonArray)
    {
        btn->enableButton(enable);
    }
}
void HudLayer::addEnergyEffect(){
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
        spt->runAction(Sequence::create(DelayTime::create(i*interval), EaseIn::create(MoveTo::create(dur, Point(x, y)), 3), CallFuncN::create(CC_CALLBACK_1(HudLayer::spriteMoveDone, this)), NULL));
    }
    this->scheduleOnce(schedule_selector(HudLayer::addEnergyDone), dur + interval*effectCount);
}
void HudLayer::addEnergyDone(float dt){
    setLife((int)lifeArray.size() + 1);
    float x = 10 + 47*(lifeArray.size() + 1);
    float y = size.height - 28;
    Sprite* spt = Sprite::create("hudHeart.png");
    this->addChild(spt);
    spt->setPosition(Point(x, y));
    float dur =0.5;
    spt->runAction(ScaleTo::create(dur, 3));
    spt->runAction(Sequence::create(FadeOut::create(dur), CallFuncN::create(CC_CALLBACK_1(HudLayer::spriteMoveDone, this)), NULL));
    GameManager::getInstance()->playSoundEffect(SOUND_MEDICINE);
    GameManager::getInstance()->getCurrentStageLayer()->player->energy = (int)lifeArray.size();
    GameManager::getInstance()->getCurrentStageLayer()->player->maxEnergy = (int)lifeArray.size();
}
void HudLayer::initializeSreen(int lifeCount)
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

void HudLayer::reduceLife()
{
    Sprite* sprite = (Sprite*)lifeArray.at(lifeArray.size()-1);
    lifeArray.eraseObject(sprite);
    sprite->removeFromParentAndCleanup(true);
}

void HudLayer::setWeapon(int weapon){
    Sprite* sptWeapon = Sprite::createWithSpriteFrameName(GameManager::getInstance()->getWeaponImageFileName(weapon));
    sptWeapon->setScale(0.5f);
    Node* checkBoard = hudLayer->getChildByName("sptCheckBoard");
    ((Sprite*)checkBoard->getChildByName("sptWeapon"))->setSpriteFrame(sptWeapon->getSpriteFrame());
    float recommendedWidth = checkBoard->getContentSize().width - 2;
    float recommendedScale = 0.5f;
//    if (recommendedWidth < sptWeapon->getContentSize().width*recommendedScale) {
//        ((Sprite*)checkBoard->getChildByName("sptWeapon"))->setScale(recommendedWidth/sptWeapon->getContentSize().width);
//    }else{
//        ((Sprite*)checkBoard->getChildByName("sptWeapon"))->setScale(recommendedScale);
//    }    
}
void HudLayer::setTotalBulletCount(int count, int maxCount){
    Node* checkBoard = hudLayer->getChildByName("sptCheckBoard");
    TextBMFont* lblCount = (TextBMFont*)checkBoard->getChildByName("lblBulletLeft");
    if (maxCount < 0) {
        lblCount->setString("00");
    }else{
        lblCount->setString(__String::createWithFormat("%d/%d", count, maxCount)->getCString());
    }
}
void HudLayer::reduceBulletCount(){
    if (bulletArray.size() <= 0) {
        return;
    }
    Sprite* sptBullet = bulletArray.at(bulletArray.size() - 1);
    bulletArray.eraseObject(sptBullet);
    sptBullet->removeFromParentAndCleanup(true);
}
void HudLayer::setBulletCount(int count, int maxCount){
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
void HudLayer::setLife(int eng)
{
    Droppable* player = WORLD->player;
    float energy = player->energy;
    if(energy < 0){
        energy = 0;
    }
    float max = player->maxEnergy;
    if(max < 0){
        max = 0;
    }
    Text* lbl = (Text*)hudLayer->getChildByName("lblEnergy");
    lbl->setString(strmake("%d/%d", (int)energy, (int)max));
    Sprite* sptEnergyBar = (Sprite*)hudLayer->getChildByName("sptEnergyBar");
    sptEnergyBar->setTextureRect(Rect(0, 0, 74*energy/max, 8));
    sptEnergyBar->setColor(energy/max > 0.3f?Color3B::GREEN:Color3B::RED);
    
//
//    TextBMFont* lbl = (TextBMFont*)hudLayer->getChildByName("lblHeart");
//    if (eng > 10) {
//        lbl->setVisible(true);
//        for (int i = 1; i < 5; i++) {
//            hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", i)->getCString())->setVisible(false);
//        }
//        ((ImageView*)hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", 0)->getCString()))->loadTexture("heart.png");
//        lbl->setString(__String::createWithFormat("x%d", eng)->getCString());
//    }else{
//        lbl->setVisible(false);
//        int fullHeatCount = eng/2;
//        bool halfExist = eng%2 == 1;
//        for (int i = 0; i < fullHeatCount; i++) {
//            ((ImageView*)hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", i)->getCString()))->loadTexture("heart.png");
//        }
//        for (int i = 0; i < 5; i++) {
//            hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", i)->getCString())->setVisible(i <= fullHeatCount);
//        }
//        if (fullHeatCount < 5) {
//            ((ImageView*)hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", fullHeatCount)->getCString()))->loadTexture("heartHalf.png");
//            hudLayer->getChildByName(__String::createWithFormat("sptHeart%d", fullHeatCount)->getCString())->setVisible(halfExist);
//        }
//    }
    
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

void HudLayer::keyCollected(int count)
{
    lblKeyCollected->setString(__String::createWithFormat("x %d", count)->getCString());
    GameManager::getInstance()->showParticleExplosion(this, "blueKey.png",lblKeyCollected->getPosition(), 2);
}
void HudLayer::starCollected(int count)
{
    Point pos;
    
    
    GameManager::getInstance()->showParticleExplosion(this, "goldStar.png", pos, 2);

    starCount = count;
}


void HudLayer::starCollectedFancy(int count, Point collectedPos)
{
    
}

void HudLayer::starChange(float dt)
{
    
}

void HudLayer::spriteMoveDone(Node* sprite)
{
    sprite->removeFromParentAndCleanup(true);
}

void HudLayer::resumeGame(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(pauseLayer == NULL) return;
    
    blackScreen->setVisible(false);
    
    if (pauseLayer){
        pauseLayer->removeFromParentAndCleanup(true);
        pauseLayer = NULL;
    }
    
    GameManager::getInstance()->isInMiddleOfGame = true;
//    NativeInterface::NativeInterface::destroyAds();
    
    GameManager::getInstance()->getCurrentStageLayer()->resumeLayer();
    this->enableButtons(true);
}
void HudLayer::showPauseLayer()
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
    btnResume->addTouchEventListener(CC_CALLBACK_2(HudLayer::resumeGame, this));
    
    Button* btnRestart = dynamic_cast<Button*>(Helper::seekWidgetByName(pauseLayer, "Button_14_0"));
    btnRestart->addTouchEventListener(CC_CALLBACK_2(HudLayer::retryStage, this));
    
    Button* btnStages = dynamic_cast<Button*>(Helper::seekWidgetByName(pauseLayer, "Button_14_1"));
    btnStages->addTouchEventListener(CC_CALLBACK_2(HudLayer::toStageSelect, this));
    
//    NativeInterface::NativeInterface::showRectAds();
    GameManager::getInstance()->isInMiddleOfGame = false;
    
    GameManager::getInstance()->saveCoin();
    
    enableButtons(false);

    GameManager::getInstance()->getCurrentStageLayer()->pauseLayer();
}

void HudLayer::showShopLayer()
{
    enableButtons(false);
    Layer* layer = GameManager::getInstance()->getShopLayer();
//    ((ShopLayer*)layer)->updateCoin();
    if (layer->getParent()) {
        layer->removeFromParentAndCleanup(false);
    }
	this->addChild(layer);
    
//    Director::getInstance()->pause();
    GameManager::getInstance()->getCurrentStageLayer()->pauseLayer();
}

void HudLayer::showGameOverLayer(float dt)
{
    if (isRecording) {
        isRecording = false;
//        NativeInterface::NativeInterface::stopRecording();
    }
    
    hideBtns();
    
    gameOverLayer = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("LegendDary_UI_GameOver.json"));
    gameOverLayer->setPosition(Point(size.width/2 - gameOverLayer->getContentSize().width/2, 0));
    GameManager::getInstance()->animateFadeIn(gameOverLayer, this);
    
    Button* btnHome = dynamic_cast<Button*>(Helper::seekWidgetByName(gameOverLayer, "btnHome"));
    btnHome->addTouchEventListener(CC_CALLBACK_2(HudLayer::toMain, this));
    
    Button* btnRetry = dynamic_cast<Button*>(Helper::seekWidgetByName(gameOverLayer, "btnRetry"));
    btnRetry->addTouchEventListener(CC_CALLBACK_2(HudLayer::retryStage, this));
    
    Button* btnToStages = dynamic_cast<Button*>(Helper::seekWidgetByName(gameOverLayer, "btnToStages"));
    btnToStages->addTouchEventListener(CC_CALLBACK_2(HudLayer::toStageSelect, this));

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
void HudLayer::showStageClearLayer(int coin, int enemyKilled, int enemyMax){
   
    
}
void HudLayer::onFreeCoin(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
        MyMessageBox::getInstance()->showDialog(this->getParent(), "NOT AVAILABLE");
        return;
    }
    
    GameManager::getInstance()->showVideo(VIDEO_GAME_OVER);
}
void HudLayer::videoDone(){
    log("videoDone in HudLayer");
    isVideoDone = true;
}
void HudLayer::videoFailed(){
    if(GameManager::getInstance()->getHudLayer()){
        GameManager::getInstance()->getHudLayer()->scheduleOnce(schedule_selector(HudLayer::closeVideoFailed), 0.1);
    }
}
void HudLayer::closeVideoDone(float dt){
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

void HudLayer::closeVideoFailed(float dt){
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

void HudLayer::shareFacebook(){
    this->scheduleOnce(schedule_selector(HudLayer::facebookSafe), 0.1);
}

void HudLayer::facebookSafe(float dt){
    const char* link = "http://www.facebook.com/FifteenSixKorea";
    if (GameManager::getInstance()->market == MARKET_PLAYSTORE_PAID ||
        GameManager::getInstance()->market == MARKET_NAVER_PAID ||
        GameManager::getInstance()->market == MARKET_TSTORE_PAID) {
        link = "https://play.google.com/store/apps/details?id=com.magmon.LegendDaryTwo";
    }
}
void HudLayer::playCoinSoundLater(){
    GameManager::getInstance()->playSoundEffect(SOUND_COIN);
}
void HudLayer::coinLabelScheduler(float dt){
    CCLOG("coinLabelScheduler");
    increasingCoinCount += coinCount/10;
    increasingCounter++;
    if (increasingCounter >= increasingCounterMax) {
        increasingCoinCount = coinCount;
        increasingCounter = 0;
        this->schedule(schedule_selector(HudLayer::enemyLabelScheduler), 0.05f);
        this->unschedule(schedule_selector(HudLayer::coinLabelScheduler));
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
void HudLayer::enemyLabelScheduler(float dt){
    increasingEnemyCount += enemyKilledCount/10.0f;
    increasingCounter++;
    char buf[20];
    if (increasingCounter >= increasingCounterMax) {
        increasingEnemyCount = enemyKilledCount;
        increasingCounter = 0;

        sprintf(buf, "%d/%d", (int)increasingEnemyCount, enemyMaxCount);
        lblEnemyCount->setString(buf);
        this->schedule(schedule_selector(HudLayer::expLabelScheduler), 0.05f);
        this->unschedule(schedule_selector(HudLayer::enemyLabelScheduler));
        return;
    }
    
    sprintf(buf, "%d/%d", (int)increasingEnemyCount, enemyMaxCount);
    lblEnemyCount->setString(buf);
    GameManager::getInstance()->playSoundEffect(SOUND_FA);
}
void HudLayer::expLabelScheduler(float dt){
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
        this->unschedule(schedule_selector(HudLayer::expLabelScheduler));
        
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
void HudLayer::showGameContinueLayer()
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
    btnClose->addTouchEventListener(CC_CALLBACK_2(HudLayer::closeReviveLayer, this));
    
    Button* btnRevive = dynamic_cast<Button*>(Helper::seekWidgetByName(reviveLayer, "btnContinue"));
    btnRevive->addTouchEventListener(CC_CALLBACK_2(HudLayer::tryGameContinue, this));
    
    
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

void HudLayer::closeReviveLayer(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
//    if(!GameManager::getInstance()->isPaidGame){
//        NativeInterface::NativeInterface::showChartboost();
//    }
    GameManager::getInstance()->isInMiddleOfGame = false;
    
    GameManager::getInstance()->animateFadeOut(reviveLayer);
    reviveLayer = NULL;
    this->scheduleOnce(schedule_selector(HudLayer::showGameOverLayer), 0.2);
}
void HudLayer::tryGameContinue(Ref* pSender, ui::Widget::TouchEventType eEventType){
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
        
        this->scheduleOnce(schedule_selector(HudLayer::reviveLater), duration*2/3);
        
        Button* btnRevive = dynamic_cast<Button*>(Helper::seekWidgetByName(reviveLayer, "btnContinue"));
        btnRevive->removeFromParentAndCleanup(true);
        Button* btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(reviveLayer, "btnClose"));
        btnClose->removeFromParentAndCleanup(true);
    }else{
        CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(HudLayer::recommandCoinShopClosed, this));
        MyMessageBox::getInstance()->showDialogWithCloseBox(this, call, LanguageManager::getInstance()->getText("no").c_str(), LanguageManager::getInstance()->getText("ok").c_str());
    }
}

void HudLayer::recommandCoinShopClosed(Node* sender){
    if(this->getTag() == MESSAGE_BOX_RESULT_OK){
        
    }
}
void HudLayer::notEnoughGemClosed(Ref* obj){
    if(this->getTag() == MESSAGE_BOX_RESULT_OK){
//        IAPManager::getInstance()->purchase(3);
        MyMessageBox::getInstance()->showDialog(this, "WAIT...");
    }
}
void HudLayer::closeWaitMessageBoxLater(){
    this->scheduleOnce(schedule_selector(HudLayer::closeWaitMessageBox), 0.01);
}
void HudLayer::closeWaitMessageBox(float dt){
    if(MyMessageBox::getInstance()->messageBox){
        MyMessageBox::getInstance()->messageBox->removeFromParentAndCleanup(true);
        MyMessageBox::getInstance()->messageBox = NULL;
    }
}
void HudLayer::closeWaitMessageBoxLaterAndShowSucess(){
    this->scheduleOnce(schedule_selector(HudLayer::closeWaitMessageBoxAndShowSuccess), 0.01);
}
void HudLayer::closeWaitMessageBoxAndShowSuccess(float dt){
    if(MyMessageBox::getInstance()->messageBox){
        MyMessageBox::getInstance()->messageBox->removeFromParentAndCleanup(true);
        MyMessageBox::getInstance()->messageBox = NULL;
    }
//    MyMessageBox::getInstance()->showDialog(this, "COIN PURCHASED!");
}
void HudLayer::reviveFromVideo(){
    blackScreen->setVisible(false);
    GameManager::getInstance()->animateFadeOut(reviveLayer);
    reviveLayer = NULL;
    pauseItem->setVisible(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    recordItem->setVisible(true);
#endif
    this->scheduleOnce(schedule_selector(HudLayer::playBackgroundMusicLater), 2);
    GameManager::getInstance()->getCurrentStageLayer()->revive();
    
    int count = reviveCount;
    count++;
    reviveCount = count;
    GameManager::getInstance()->saveCoin();
}
void HudLayer::reviveLater(float dt){
    blackScreen->setVisible(false);
    GameManager::getInstance()->animateFadeOut(reviveLayer);
    reviveLayer = NULL;
    pauseItem->setVisible(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    recordItem->setVisible(true);
#endif
    this->scheduleOnce(schedule_selector(HudLayer::playBackgroundMusicLater), 2);
    GameManager::getInstance()->getCurrentStageLayer()->revive();
    
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
void HudLayer::playBackgroundMusicLater(float dt){
    
}
void HudLayer::buyGemClosed(Ref* obj){
    if(this->getTag() == MESSAGE_BOX_RESULT_BUTTON_2){
        // buy gem
        CCLOG("buy gem!");
    }else{
        
    }
}
void HudLayer::removeUsedAssets(){
    this->unschedule(schedule_selector(HudLayer::update));
    GameManager::getInstance()->getCurrentStageLayer()->removeUsedAssets();
    if(GameManager::getInstance()->showVPad){
        btnA->removeUsedAssets();
        btnB->removeUsedAssets();
        btnLeftRight->removeUsedAssets();
    }
    btnToggle->removeListener();
    btnShoot->removeListener();
    btnJump->removeListener();
    btnWeapon->removeListener();
    btnReload->removeListener();
    btnAction->removeListener();
    js->removeListener();
    js2->removeListener();
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
    
    GameManager::getInstance()->getCurrentStageLayer()->removeUsedAssets();*/
}
void HudLayer::toMain(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    SM->getHttpTime();
    GameManager::getInstance()->getCurrentStageLayer()->saveCoinIfStarCollected();
    whereToGo = CLOSE_TO_MAIN;
    this->scheduleOnce(schedule_selector(HudLayer::closeSchedule), 0.01);
}
void HudLayer::toStageSelect(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    SM->getHttpTime();
    GameManager::getInstance()->getCurrentStageLayer()->saveCoinIfStarCollected();
    
    whereToGo = CLOSE_TO_STAGES;
    this->scheduleOnce(schedule_selector(HudLayer::closeSchedule), 0.01);
}
void HudLayer::closeSchedule(float dt){
//    Director::getInstance()->popScene();
    removeUsedAssets();
//    ((TitleLayer*)GameManager::getInstance()->titleLayer)->closeTo(whereToGo);
    int toWhat = whereToGo;
    if(toWhat == CLOSE_TO_NEXT){
        
        if(GameManager::getInstance()->theme*24 + GameManager::getInstance()->currentStageIndex == GameManager::getInstance()->totalStage - 1){
            
            GameManager::getInstance()->page = PAGE_STAGE_SELECT;
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->setStageSelect();
            return;
        }
        if (!UserDefault::getInstance()->getBoolForKey(KEY_AD_NOT_TODAY, false) &&
            GameManager::getInstance()->market == MARKET_PLAYSTORE_FREE) {
            GameManager::getInstance()->page = PAGE_STAGE_SELECT;
            
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->setStageSelect();
            return;
        }
        if(GameManager::getInstance()->currentStageIndex == 23 ){
            GameManager::getInstance()->page = PAGE_STAGE_SELECT;
            
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->backFromStages();
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->setThemeSelect();
        }else{
            GameManager::getInstance()->page = PAGE_STAGE_SELECT;
            
//            int stage = GameManager::getInstance()->currentStageIndex + 1;
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->goStage(stage);
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->setStageSelect();
            return;
        }
    }else if(toWhat == CLOSE_TO_RETRY){
        Director::getInstance()->popScene();
//        int stage = GameManager::getInstance()->currentStageIndex;
//        ((TitleLayer*)GameManager::getInstance()->titleLayer)->stageTouched();
        
    }else if(toWhat == CLOSE_TO_STAGES){
        if (GameManager::getInstance()->market == MARKET_PLAYSTORE_FREE ||
            GameManager::getInstance()->market == MARKET_APPSTORE_FREE) {
            GameManager::getInstance()->showInterstitialAds();
        }
        
        GameManager::getInstance()->page = PAGE_STAGE_SELECT;
        
//        ((TitleLayer*)GameManager::getInstance()->titleLayer)->topBar->updateLabels();
//        ((TitleLayer*)GameManager::getInstance()->titleLayer)->setStageSelect();
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
void HudLayer::nextStage(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    removeListener();
    
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
//                UserDefault::getInstance()->setIntegerForKey(KEY_LIFE_SPENT_TIME, TimeManager::getInstance()->getCurrentTime());
//            }
        }
        whereToGo = CLOSE_TO_NEXT;
        this->scheduleOnce(schedule_selector(HudLayer::closeSchedule), 0.01);
    }else{
        whereToGo = CLOSE_TO_STAGES;
        this->scheduleOnce(schedule_selector(HudLayer::closeSchedule), 0.01);
    }
}
void HudLayer::retryStage(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    GameManager::getInstance()->getCurrentStageLayer()->saveCoinIfStarCollected();
    whereToGo = CLOSE_TO_RETRY;
    this->scheduleOnce(schedule_selector(HudLayer::closeSchedule), 0.01);
}
void HudLayer::onReplayVideo(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
        MyMessageBox::getInstance()->showDialog(this->getParent(), "NOT AVAILABLE");
        return;
    }
    
}

void HudLayer::enableButtons(bool enb)
{
    btnLeftRight->enabled = enb;
    btnA->enabled = enb;
    btnB->enabled = enb;
//    enableLayerAndMenus(enb);
}

void HudLayer::displayBossEnergy(const char* name)
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

void HudLayer::setBossEnergy(int percent)
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
void HudLayer::removeListener(){
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
    for (auto btn: btns){
        btn->removeListener();
    }
    this->unschedule(schedule_selector(HudLayer::update));
    js->removeListener();
    js2->removeListener();
}
void HudLayer::addListener(){
    touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesBegan");
//        auto last = touches.back();
        if (GameManager::getInstance()->getCurrentStageLayer()->isGameOver) {
            tryResultOK();
        }else if (dialogBox == nullptr) {
            GameManager::getInstance()->getCurrentStageLayer()->takeAction();
        }else{
//            GameManager::getInstance()->getCurrentStageLayer()->showBigMiniMap(true);
        }
        
//        chopad->TouchesBegan(touches, evt);
        // check if last touch point is in which button
    };
    touchListener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesMoved");
//        chopad->TouchesMoved(touches, evt);
    };
    touchListener->onTouchesEnded = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesEnded");
//        GameManager::getInstance()->getCurrentStageLayer()->showBigMiniMap(false);
        //        chopad->TouchesEnded(touches, evt);
    };
    touchListener->onTouchesCancelled = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesCancelled");
        //        chopad->TouchesCancelled(touches, evt);
    };
    getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, 1);
    /*touchListener = cocos2d::EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    
    touchListener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        Point p = touch->getLocation();
        Point location = p;
        GameManager::getInstance()->getCurrentStageLayer()->isTouchStarted = true;
        
        GameManager::getInstance()->getCurrentStageLayer()->touchStart = location;
        
        return true; // we did not consume this event, pass thru.
    };
    
    touchListener->onTouchMoved = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
        
        Point location = touch->getLocationInView();
        location = Director::getInstance()->convertToGL(location);
        
        if (!GameManager::getInstance()->getCurrentStageLayer()->isTouchStarted) {
            return;
        }
        int minUp = 100;
        int minDown = 30;
        int toDown = GameManager::getInstance()->getCurrentStageLayer()->touchStart.y - location.y;
        int toLeft = GameManager::getInstance()->getCurrentStageLayer()->touchStart.x - location.x;
        if (toLeft< 0) {
            toLeft = -toLeft;
        }
        
        if(toDown > toLeft && toDown > minDown){
            if (GameManager::getInstance()->getCurrentStageLayer()->player->oneWayCancelY == 0) {
                GameManager::getInstance()->getCurrentStageLayer()->getDown();
            }else if(!GameManager::getInstance()->getCurrentStageLayer()->player->tumbled &&
                     GameManager::getInstance()->getCurrentStageLayer()->player->jumped){
                GameManager::getInstance()->getCurrentStageLayer()->player->tumbled = true;
                GameManager::getInstance()->jumpPressed = true;
            }else if(GameManager::getInstance()->getCurrentStageLayer()->player->tumbled){
                GameManager::getInstance()->getCurrentStageLayer()->player->buttonReleased = true;
                GameManager::getInstance()->jumpPressed = true;
            }
        }else if(toDown < -toLeft && toDown < -minUp && GameManager::getInstance()->getCurrentStageLayer()->player->vehicleType != VEHICLE_NONE){
            GameManager::getInstance()->getCurrentStageLayer()->getOffVehicle();
        }
#endif
    };
    
    touchListener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        GameManager::getInstance()->getCurrentStageLayer()->isTouchStarted = false;
    };
    
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, 30);*/
}

void HudLayer::showDialog(const char* message, const char* btn1, const char* btn2)
{
    CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(HudLayer::messageBoxClosed, this));
    MyMessageBox::getInstance()->showDialog(this, call, message, btn1, btn2);
    GameManager::getInstance()->getHudLayer()->enableJoystick(false);

    this->setTouchEnabled(false);
    
    ((HelloWorld*)GameManager::getInstance()->getCurrentStageLayer())->pauseLayer();
}

void HudLayer::messageBoxClosed(Node* node)
{
    ((HelloWorld*)GameManager::getInstance()->getCurrentStageLayer())->resumeLayer();
	this->setTouchEnabled(true);
    //	_dialogBox->setVisible(false);
	
    GameManager::getInstance()->getHudLayer()->enableJoystick(true);
	
    if (this->getTag() == 0) {
//        Director::getInstance()->replaceScene(StageSelectScene::scene(GameManager::getInstance()->theme));
    }
}
Node* HudLayer::getPopup(std::string name){
    for(int i = 0; i < popupArray.size(); i++){
        if(popupArray.at(i)->getName().compare(name) == 0){
            return popupArray.at(i);
        }
    }
    return nullptr;
}
void HudLayer::onCloseBattleLayer(){
    if(GM->isTutorialOn){
        if (GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 13) {
            showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
            return;
        }
    }
    closePopup();
}
void HudLayer::closePopup(){
    if(currentPopup->getName().compare("dailyDungeonResult") == 0){
        removeUsedAssets();
        Director::getInstance()->replaceScene(HelloWorld::scene(STAGE_LOBBY, false));
        return;
    }
    if(popupArray.size() > 0){
        Node* popup = popupArray.at(popupArray.size() - 1);
        popupArray.eraseObject(popup);
        if(popup == giftLayer){
            giftLayer = nullptr;
        }else if(popup == menuLayer){
            menuLayer = nullptr;
        }
        popup->removeFromParent();
    }
    if(dialogBox){
        dialogBox = nullptr;
    }
    if(popupArray.size() > 0){
        currentPopup = popupArray.at(popupArray.size() - 1);
        if (GM->isTutorialOn) {
            if(popupArray.size() == 1 && currentPopup == menuLayer){
                if (GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 10) {
                    menuLayer->getChildByName("btnGift")->removeChildByName("arrow");
                    if (UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 0).c_str(), -1) == -1) {
                        attachArrowForTutorial(currentPopup->getChildByName("btnWeapon0"));
                    }else if (UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 1).c_str(), -1) == -1) {
                        attachArrowForTutorial(currentPopup->getChildByName("btnWeapon1"));
                        currentPopup->getChildByName("btnWeapon0")->removeChildByName("arrow");
                    }else if (UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 2).c_str(), -1) == -1) {
                        attachArrowForTutorial(currentPopup->getChildByName("btnWeapon2"));
                        currentPopup->getChildByName("btnWeapon1")->removeChildByName("arrow");
                    }
                }
            }else if(popupArray.size() == 2 && currentPopup == giftLayer){
                if (GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 9) {
                    showTutorialDialog();
                }
            }
        }
    }else{
//        gemBar->stopAllActions();
//        gemBar->runAction(EaseInOut::create(MoveTo::create(0.6f, Vec2(972, 607.39f)), 2));
        currentPopup = nullptr;
//        WORLD->resumeLayer();
        WORLD->isCameraInCustomMoving = false;
        if(isWeaponEquipChanged){
            updateEquipedWeapons();
        }
        showBtns();
        
        if(GM->isTutorialOn){
            if (GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex == 11) {
                hudLayer->getChildByName("btnMenu")->removeChildByName("arrow");
                attachArrowForTutorial(hudLayer->getChildByName("btnBattle"));
            }
        }
    }
}
// set as popup should be set after currentSelector & currentSelectedItem
void HudLayer::setAsPopup(Node* node){
    hideBtns();
//    node->setContentSize(size);
    currentPopup = node;
    if(dialogBox == node){
        // no show up animation
    }else{
        GM->showPopup(node);
    }
    popupArray.pushBack(node);
    node->setLocalZOrder(POPUP_ZORDER + (int)popupArray.size());
//    if(popupArray.size() == 1){
//        WORLD->pauseLayer();
//    }
}
void HudLayer::showIndicator(){
    log("show Indicator!");
    Layer* layer = Layer::create();
    this->addChild(layer, 10000);
//    setAsPopup(layer);
    layer->setName("indicator");
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
    
    this->scheduleOnce(schedule_selector(HudLayer::indicatorTimeOver), 30);
}
void HudLayer::indicatorTimeOver(float dt){
    showInstanceMessage(LM->getText("time over"));
    hideIndicator();
}
void HudLayer::hideIndicator(){
    this->unschedule(schedule_selector(HudLayer::indicatorTimeOver));
    Node* layer = this->getChildByName("indicator");
    if(layer != nullptr){
//        popupArray.eraseObject(layer);
        layer->removeFromParent();
        indicator = nullptr;
    }else if(indicator != nullptr){
        indicator->removeFromParent();
    }
    indicator = nullptr;
}
void HudLayer::showInstanceMessage(std::string msg, float stayTime){
    this->removeChildByName("instanceMessage");
    Sprite* spt = Sprite::create("alertBar.png");
    spt->setScale(4);
    this->addChild(spt, 299);
    Size cSize = spt->getContentSize();
    spt->setPosition(Point(size.width/2, size.height - 714 - spt->getContentSize().height/2 + 50));
    spt->setName("instanceMessage");
    Label* lbl = LM->getLocalizedLabel(msg.c_str(), Color4B::WHITE);
    spt->addChild(lbl, 299);
//    lbl->setScale(0.4);
//    lbl->enableBold();
    lbl->setScale(lbl->getScale()*0.25f);
    lbl->setAnchorPoint(Point(0.5, 0));
    lbl->enableOutline(Color4B::BLACK, 5);
    lbl->setWidth(size.width/lbl->getScaleX());
    lbl->setPosition(Vec2(spt->getContentSize().width/2, spt->getContentSize().height/2 - 5));
    lbl->setDimensions((size.width - 100)/lbl->getScale(), 300);
    lbl->setOverflow(Label::Overflow::SHRINK);
    lbl->setHorizontalAlignment(TextHAlignment::CENTER);
    lbl->setVerticalAlignment(TextVAlignment::BOTTOM);
    float dur = 0.3f;
    float distanceToMove = 80;
    spt->runAction(Sequence::create(MoveBy::create(dur, Point(0, distanceToMove)), DelayTime::create(stayTime), MoveBy::create(dur, Point(0, -distanceToMove)), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, spt)), nullptr));
}
void HudLayer::showAttend(){
    Node* layer = CSLoader::createNode("Attend.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("attendPopup");
    setCloseButtonAndBlock(layer);
    
    setTitle(layer->getChildByName("titleBack"), "daily reward");
    
    int dayCounter = UDGetInt(KEY_DAY_COUNTER, 1);
    int rewardCounter = UDGetInt(KEY_DAILY_REWARD_COUNTER, 0);
    if(rewardCounter + 1 < dayCounter){
        rewardCounter = dayCounter - 1;
    }
    Node* dayNode;Button* btn;
    for (int i = 0; i < 7; i++) {
        dayNode = layer->getChildByName(strmake("day%d", i));
        btn = (Button*)dayNode->getChildByName("btnClaim");
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onAttendRewardClick, this));
        btn->setEnabled(rewardCounter%7 == i && rewardCounter < dayCounter);
        btn->setVisible(rewardCounter%7 <= i);
        if(!btn->isVisible()){
            Sprite* spt = Sprite::create("iconCheck.png");
            btn->getParent()->addChild(spt);
            spt->setPosition(btn->getPosition());
        }
    }
}
void HudLayer::onAttendRewardClick(Ref* ref){
    int dayCounter = UDGetInt(KEY_DAY_COUNTER, 1);
    UDSetInt(KEY_DAILY_REWARD_COUNTER, dayCounter);
    
    BTN_FROM_REF_AND_DISABLE
    std::string extraData = "";
    switch (btn->getTag()) {
        case 0:
            GM->addGem(100);
            break;
        case 1:
            GM->addCurrency(100, CURRENCY_TYPE_GEM_GRAY);
            extraData = strmake("&crystal_gray=%d", GM->getCurrency(CURRENCY_TYPE_GEM_GRAY));
            break;
        case 2:
            GM->addCurrency(100, CURRENCY_TYPE_GEM_GREEN);
            extraData = strmake("&crystal_green=%d", GM->getCurrency(CURRENCY_TYPE_GEM_GREEN));
            break;
        case 3:
            GM->addCurrency(100, CURRENCY_TYPE_GEM_ORANGE);
            extraData = strmake("&crystal_orange=%d", GM->getCurrency(CURRENCY_TYPE_GEM_ORANGE));
            break;
        case 4:
            GM->addCurrency(100, CURRENCY_TYPE_GEM_RED);
            extraData = strmake("&crystal_red=%d", GM->getCurrency(CURRENCY_TYPE_GEM_RED));
            break;
        case 5:
            GM->addCurrency(200, CURRENCY_TYPE_GEM_PURPLE);
            GM->localTicketCount += 20;
            extraData = strmake("&local_ticket_count=%d&crystal_purple=%d", GM->localTicketCount, GM->getCurrency(CURRENCY_TYPE_GEM_PURPLE));
            break;
        case 6:
            GM->addGem(500);
            GM->networkTicketCount += 20;
            extraData = strmake("&network_ticket_count=%d", GM->networkTicketCount);
            break;
        default:
            break;
    }
    closePopup();
    showAttendRequired = true;
    SM->saveUserData(strmake("attend=%d,%d&gem=%d%s", dayCounter, dayCounter, GM->getGem(), extraData.c_str()));
}
void HudLayer::updateUI(float dt){
    if (!SM->timeEstablished) {
        return;
    }
    hudTimer += dt;
//    if(networkStateGetData == NETWORK_HANDLE_STATE_SHOULD_REQUEST){
//        networkStateGetData = NETWORK_HANDLE_STATE_REQUESTED;
//        showIndicator();
//        SM->getAllUserData();
//    }else
    double now = SM->getCurrentTime();
    int timeLeftToMidnight = 86400 - (long)now%86400;
    bool isNewDay = false;
    int currentDay = UDGetInt(KEY_DAY_COUNT, -1);
    int checkedDay = UDGetInt(KEY_DAY_CHECKED, -1);
    bool dayIsDifferent = currentDay != checkedDay;
    int today = (int)(now/86400);
    int lastDay = UDGetInt(KEY_TODAY_COUNT, 0);
//    std::string strSavedToday = UDGetStr(KEY_TODAY_STRING, "").substr(0, 10);
//    std::string strToday = std::string(BSM->getStrFromTime(now)).substr(0, 10);
    if(today > lastDay){
        GM->shouldNetworkMatchRefresh = true;
        UDSetInt(KEY_TODAY_COUNT, today);
        log("today: %d/%d", today, lastDay);
        isNewDay = true;
    }
    
    if(lastTimeLeftToMidnight == 0){
        lastTimeLeftToMidnight = timeLeftToMidnight;
    }
    if((lastTimeLeftToMidnight < timeLeftToMidnight && lastTimeLeftToMidnight < 10) || dayIsDifferent){
        isNewDay = true;
        UDSetInt(KEY_DAY_CHECKED, currentDay);
        log("timeLeftToMidnight: %f/%f - %d", lastTimeLeftToMidnight, timeLeftToMidnight, (int)dayIsDifferent);
//        for (int i = 0; i < 10; i++) {
//            UDSetInt(strmake(KEY_NETWORK_BATTLE_RESULT_FORMAT, i).c_str(), 0);
//        }
    }
    if(isNewDay){
//        for (int i = 0; i < 3; i++) {
//            UDSetInt(strmake(KEY_VIDEO_STORE_RECEIVED_INDEX_FORMAT, i).c_str(), -1);
//            UDSetDouble(strmake(KEY_VIDEO_STORE_WATCHED_TIME_FORMAT, i).c_str(), 0);
//        }
        GM->videoStore0Index = 0;
        GM->videoStore1Index = 0;
        GM->videoStore2Index = 0;
        GM->videoStore0WatchedTime = 0;
        GM->videoStore1WatchedTime = 0;
        GM->videoStore2WatchedTime = 0;
//        UDSetDouble(KEY_VIDEO_NEXT_AVAILABLE_TIME, -1);
        GM->videoShopWatchedTime = 0;
//        UDSetInt(KEY_VIDEO_ADS_LEFT, 10);
        GM->videoShopIndex = 0;
        SM->saveUserData("video_shop_index=0&video_store_0_index=0&video_store_1_index=0&video_store_2_index=0");
        
    }
    Node* layer = getChildByName("videoStore");
    Text* lbl;
    if(layer != nullptr && popupArray.at(popupArray.size() - 1) == layer){
        if (isNewDay) {
            closePopup();
            onVideoStoreClick();
        }
        lbl = (Text*)layer->getChildByName("lblTimer");
        lbl->setString(GM->getTimeLeftInString(timeLeftToMidnight));
        
        int nextIndex;
        double watchedTime;
//        watchedTime = UDGetDouble(strmake(KEY_VIDEO_STORE_WATCHED_TIME_FORMAT, selectedVideoStoreTabIndex).c_str(), 0);
//        nextIndex = UDGetInt(strmake(KEY_VIDEO_STORE_RECEIVED_INDEX_FORMAT, selectedVideoStoreTabIndex).c_str(), -1) + 1;
        if (selectedVideoStoreTabIndex == 0) {
            watchedTime = GM->videoStore0WatchedTime;
            nextIndex = GM->videoStore0Index;
        }else if (selectedVideoStoreTabIndex == 1) {
            watchedTime = GM->videoStore1WatchedTime;
            nextIndex = GM->videoStore1Index;
        }else if (selectedVideoStoreTabIndex == 2) {
            watchedTime = GM->videoStore2WatchedTime;
            nextIndex = GM->videoStore2Index;
        }
        
        Button* btn;
        for (int i = 0; i < 12; i++) {
            btn = (Button*)layer->getChildByName(strmake("btn%d", i));
            btn->setOpacity(nextIndex < i?100:255);
            btn->setEnabled(nextIndex == i);
            
            lbl = (Text*)btn->getChildByName("lbl");
            if (i == 5 || i == 11) {
                if(selectedVideoStoreTabIndex == 0){
                    LM->setLocalizedString(lbl, "weapon");
                }else if(selectedVideoStoreTabIndex == 1){
                    LM->setLocalizedString(lbl, "shield");
                }else if(selectedVideoStoreTabIndex == 2){
                    lbl->setString("100");
                }
            }else{
                lbl->setString("2");
            }
        }
        if(nextIndex < 12){
            Button* btn = (Button*)layer->getChildByName(strmake("btn%d", nextIndex));
//            Node* img = btn->getChildByName("imgIconGold");
            bool isReady = now - watchedTime > videoStoreWaitForNextVideoTime;
//            img->setVisible(isReady);
            
            if (isReady) {
//                GM->alignToCenter(img, lbl, 60, btn->getContentSize().width/2, 0);
                
                
//                Node* lblQuestion = btn->getChildByName("lblQuestion");
//                if(lblQuestion){
//                    lblQuestion->setPosition(img->getPosition());
//                }
            }else{
                lbl = (Text*)btn->getChildByName("lbl");
                lbl->setPositionX(btn->getContentSize().width/2);
                lbl->setString(GM->getTimeLeftInString(videoStoreWaitForNextVideoTime - (now - watchedTime)));
            }
        }
    }
    lastTimeLeftToMidnight = timeLeftToMidnight;
    if (SM->isServerFailed){
        SM->isServerFailed = false;
        onServerFailed();
    }
    if(networkStateGetData == NETWORK_HANDLE_STATE_ARRIVED){
        networkStateGetData = NETWORK_HANDLE_STATE_HANDLED;
        hideIndicator();
        updateGem(GM->getGem());
        if(isTopPlayerForFieldRequired){
            isTopPlayerForFieldRequired = true;
            getTopPlayersOnField();
        }
        
        if(isGettingAllUser){
            isGettingAllUser = false;
            updateProfile();
        }
        if(showAttendRequired){
            showAttendRequired = false;
            showAttend();
        }
        if(updateArmorRequired){
            updateArmorRequired = false;
            updateArmorLayer();
        }
        if(updateResearchRequired){
            updateResearchRequired = false;
            updateResearchLayer();
        }
        if(showCollectionRequired){
            showCollectionRequired = false;
            showCollection();
        }
        if(isGettingTopPlayers){
            isGettingTopPlayers = false;
            showMenu();
        }
        if(showMenuRequired){
            showMenuRequired = false;
            isGettingTopPlayers = true;
            SM->getTopPlayers(3, "trophy");
        }
        if (isChangingAccount) {
            isChangingAccount = false;
            if(isChangingAccountSuccess){
                showInstanceMessage(LM->getText("login success"));
                this->runAction(Sequence::create(DelayTime::create(2), CallFunc::create(CC_CALLBACK_0(HudLayer::restartGame, this)), NULL));
                showIndicator();
            }else{
                showInstanceMessage(LM->getText("login failed"));
            }
        }
        
        if (isLoadingRank) {
            isLoadingRank = false;
            if(isLoadingRankSuccess){
                Node* layer = this->getChildByName("profilePopup");
                Node* background = layer->getChildByName("imgBackground");
                Text* lbl= (Text*)background->getChildByName("lblRank");
                LM->setLocalizedStringNotKey(lbl, strmake(LM->getText("rank th").c_str(), myRank));
                if (LM->getLanguageType() == LanguageType::ENGLISH) {
                    std::string text = lbl->getString();
                    std::string strTH = "th";
                    if(myRank == 1){
                        strTH = "st";
                    }else if(myRank == 2){
                        strTH = "nd";
                    }else if(myRank == 3){
                        strTH = "rd";
                    }
                    GM->replaceAll(text, "[th]", strTH);
                    lbl->setString(text);
                }
            }else{
                showInstanceMessage(LM->getText("network retry"));
            }
        }
        if (isChangingName) {
            isChangingName = false;
            if(isChangingNameSuccess){
                showInstanceMessage(LM->getText("name registered"));
                Node* layer = this->getChildByName("profilePopup");
                Node* background = layer->getChildByName("imgBackground");
                Text* lbl= (Text*)background->getChildByName("lblLevelAndName");
                LM->setLocalizedStringNotKey(lbl, strmake("Lv.%d %s", GM->getGameLevel(), playerName.c_str()));
                updateProfile();
                
                closePopup();
                
                GM->addGem(-100);
                SM->saveUserData(strmake("gem=%d", GM->getGem()));
            }else{
                showInstanceMessage(LM->getText("same name exist"));
            }
        }
        if (isFindMatchesRequested) {
            isFindMatchesRequested = false;
//            SM->saveUserData(strmake("network_matches=%s", GM->strNetworkMatches.c_str()));
//            generateMatches();
            closePopup();
            onBattleClick();
        }
//        if (isRequestingNetworkMatchUserData) {
//            if(SM->isFailedToGetNetworkData){
//                GM->strNetworkMatchDatas = "";
//                SM->findMatches(UDGetInt(KEY_TROPHY, 1000));
//                isFindMatchesRequested = true;
//                showIndicator();
//                return;
//            }
//            networkMatchAvatars.push_back(SM->otherUserAvatar);
//            networkMatchNames.push_back(SM->strOtherUserName);
//            networkMatchTrophys.push_back(SM->otherUserTrophy);
//            networkMatchEquips.push_back(SM->strOtherUserEquip);
//            networkMatchRuneEquips.push_back(SM->strOtherUserRuneEquip);
//            networkMatchArmors.push_back(SM->strOtherUserArmor);
//            networkMatchResearchs.push_back(SM->strOtherUserResearch);
//            networkMatchIndex++;
//            if(networkMatchIDs.size() > networkMatchIndex){
//                SM->getOtherUserData(strmake("id=%d&name=1&trophy=1&avatar=1&armor=1&research=1&equip=1&rune_equip=1", networkMatchIDs.at(networkMatchIndex)));
//            }else{
//                finishGenerateMatches();
//            }
//        }
    }
    if(GM->isTutorialOn && WORLD && WORLD->stageIndex == STAGE_LOBBY && hudTimer > 0.5f){
        gemBar->setVisible(false);
        if (GM->tutorialType == TUTORIAL_DUNGEON && GM->tutorialIndex == 0) { // start
            //                GM->tutorialType = TUTORIAL_MENU; // test
            //                GM->tutorialIndex = 8; // test
            //                GM->tutorialType = TUTORIAL_AFTER_BATTLE; // test
            //                GM->tutorialIndex = 0; // test
            //                UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 0).c_str(), -1); // test
            //                UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 1).c_str(), -1); // test
            //                UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 2).c_str(), -1); // test
            showTutorialDialog();
        }else if(GM->tutorialType == TUTORIAL_MENU){ // after clear tutorial dungeon
            //                GM->tutorialType = TUTORIAL_MENU;
            //                GM->tutorialIndex = 0;
            if(GM->tutorialIndex == 0){
                showTutorialDialog();
            }
            if(dialogBox == nullptr && GM->tutorialIndex == 1){
                GM->tutorialIndex = 0;
                showTutorialDialog();
            }
        }
        if(GM->tutorialType == TUTORIAL_MENU && GM->tutorialIndex >= 12 && GM->isBattleWon){
            GM->tutorialType = TUTORIAL_AFTER_BATTLE;
            GM->tutorialIndex = 0;
            showTutorialDialog();
        }
//        log("tutorial type: %d, tutorialIndex: %d", GM->tutorialType, GM->tutorialIndex);
    }
    if (sptArrowToTutorialDungeon) {
        Vec2 targetPos = Vec2(80, 101);
        Vec2 playerPos = WORLD->player->getPosition();
        float angle = GM->getAngle(targetPos, playerPos);
        float radius = 40;
        Vec2 extraPos = Vec2(radius*cos(angle*3.14f/180), radius*sin(angle*3.14f/180));
        sptArrowToTutorialDungeon->setPosition(playerPos + extraPos);
        sptArrowToTutorialDungeon->setRotation(-angle + 90);
    }
    if(networkStateSendData == NETWORK_HANDLE_STATE_ARRIVED){
        networkStateSendData = NETWORK_HANDLE_STATE_HANDLED;
        hideIndicator();
        updateGem(GM->getGem());
        if(reopenPostPopup){
            reopenPostPopup = false;
            closePopup();
            onPostClick();
        }
        if(updateSearchPopupRequired){
            updateSearchPopupRequired = false;
            updateSearchItems();
        }
        if(confirmGiftRequired){
            confirmGiftRequired = false;
            confirmGift();
        }
        if(showAttendRequired){
            showAttendRequired = false;
            showAttend();
        }
    }
    int lastRefreshedDay = UDGetInt(KEY_LAST_REFRESHED_DAY, 0);
//    int today = ((int)now)/86400;
    if(lastRefreshedDay != today){
//        UserDefault::getInstance()->setIntegerForKey(KEY_TODAYS_SCORE, 0);
//        lblScore->setString(Value(UserDefault::getInstance()->getIntegerForKey(KEY_TODAYS_SCORE, 0)).asString());
        
        UDSetInt(KEY_LAST_REFRESHED_DAY, today);
//        UDSetInt(KEY_VIDEO_ADS_LEFT, 10);
//        GM->videoShopIndex = 0;
        GM->setCollectMission();
        
        int dayCounter = UDGetInt(KEY_DAY_COUNTER, 1);
        dayCounter++;
        UDSetInt(KEY_DAY_COUNTER, dayCounter);
        SM->saveUserData(strmake("attend=%s", strmake("%d,%d", dayCounter, UDGetInt(KEY_DAILY_REWARD_COUNTER, 0)).c_str()));
        UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_AM79, false);
        UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1214, false);
        UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1720, false);
        UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2022, false);
        UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2224, false);
    }
    
    if(giftLayer != nullptr){
        Node* background = giftLayer->getChildByName("imgBackground");
        Text* lbl = (Text*)background->getChildByName("lblTime");
        lbl->setString("yes!");
        long now = SM->getCurrentTime();
        int allDaySeconds = 86400;
        int day = (int)now/allDaySeconds;
        int seconds = (int)now%allDaySeconds;
        int giftTookDay = UDGetInt(key_GIFT_TAKE_DAY, 0);
        bool didTakeToday = giftTookDay == day;
//        didTakeToday = false; // test
        lbl->setVisible(didTakeToday);
        menuLayer->getChildByTag(6)->getChildByTag(0)->setVisible(!didTakeToday);
        if (didTakeToday){
            lbl->setString(GM->getTimeLeftInString(allDaySeconds - seconds));
        }
        Button* btn = (Button*)background->getChildByName("btnClaim");
        btn->setEnabled(!didTakeToday);
    }
    if(isVideoDone){
        isVideoDone = false;
        handleVideoDone();
    }
    if(currentPopup != nullptr){
        if(GM->searchTicketCount < 3){
            GM->searchTicketRefillTimeLeft -= dt;
            if(GM->searchTicketRefillTimeLeft <= 0){
                GM->searchTicketRefillTimeLeft += SEARCH_TICKET_REFILL_TIME;
                GM->searchTicketCount++;
            }
        }
        if(currentPopup->getName().compare("search") == 0){
            Node* background = currentPopup->getChildByName("imgBackground");
            Button* btn = (Button*)background->getChildByName("btnUpdate");
            Text* lbl = (Text*)btn->getChildByName("lbl");
            if(GM->searchTicketCount >= 3){
                lbl->setString(strmake("%d/3", GM->searchTicketCount));
            }else{
                lbl->setString(strmake("%d/3(%s)", GM->searchTicketCount, GM->getTimeLeftInStringHMS((long)GM->searchTicketRefillTimeLeft).c_str()));
            }
            btn->setEnabled(GM->searchTicketCount > 0);
        }
        
        if(GM->dungeonTicketCount < 3){
            GM->dungeonTicketRefillTimeLeft -= dt;
            if(GM->dungeonTicketRefillTimeLeft <= 0){
                GM->dungeonTicketRefillTimeLeft += DUNGEON_TICKET_REFILL_TIME;
                GM->dungeonTicketCount++;
            }
        }
        if(currentPopup->getName().compare("dungeonPopup") == 0){ // 3 tickets per day
            Text* lbl = (Text*)currentPopup->getChildByName("imgBackground")->getChildByName("lblTicket");
            if(GM->dungeonTicketCount >= 3){
                lbl->setString(strmake("%s: %d/3", LM->getText("ticket").c_str(), GM->dungeonTicketCount));
            }else{
                lbl->setString(strmake("%s: %d/3(%s)", LM->getText("ticket").c_str(), GM->dungeonTicketCount, GM->getTimeLeftInStringHMS(GM->dungeonTicketRefillTimeLeft).c_str()));
            }
        }
        
        if(GM->bossTicketCount < 1){
            GM->bossTicketRefillTimeLeft -= dt;
            if(GM->bossTicketRefillTimeLeft <= 0){
                GM->bossTicketRefillTimeLeft += 82800;
                GM->bossTicketCount++;
            }
        }
        if(currentPopup->getName().compare("bossPopup") == 0){
            Text* lbl = (Text*)currentPopup->getChildByName("imgBackground")->getChildByName("lblTime");
            if(GM->bossTicketCount <= 0){
                lbl->setString(GM->getTimeLeftInStringHMS(GM->bossTicketRefillTimeLeft));
            }
            lbl->setVisible(GM->bossTicketCount <= 0);
            Node* background = currentPopup->getChildByName("imgBackground");
            Button* btn = (Button*)background->getChildByName("btnBattle");
            btn->setEnabled(GM->bossTicketCount > 0); // test
        }
        
        if(GM->localTicketCount < 5){
            GM->localTicketRefillTimeLeft -= dt;
            if(GM->localTicketRefillTimeLeft <= 0){
                GM->localTicketRefillTimeLeft += LOCAL_TICKET_REFILL_TIME;
                GM->localTicketCount++;
            }
        }
        if(GM->networkTicketCount < 5){
            GM->networkTicketRefillTimeLeft -= dt;
            if(GM->networkTicketRefillTimeLeft <= 0){
                GM->networkTicketRefillTimeLeft += NETWORK_TICKET_REFILL_TIME;
                GM->networkTicketCount++;
            }
        }
        if(currentPopup->getName().compare("battlePopup") == 0){
            Button* btn = (Button*)currentPopup->getChildByName("btnLocalTicket");
            Text* lbl = (Text*)btn->getChildByName("lblCount");
            lbl->setString(Value(GM->localTicketCount).asString());
            lbl = (Text*)btn->getChildByName("lblTime");
            lbl->setVisible(GM->localTicketCount < 5);
            lbl->setString(GM->getTimeLeftInStringHMS(GM->localTicketRefillTimeLeft));
            
            btn = (Button*)currentPopup->getChildByName("btnNetworkTicket");
            lbl = (Text*)btn->getChildByName("lblCount");
            lbl->setString(Value(GM->networkTicketCount).asString());
            lbl = (Text*)btn->getChildByName("lblTime");
            lbl->setVisible(GM->networkTicketCount < 5);
            lbl->setString(GM->getTimeLeftInStringHMS(GM->networkTicketRefillTimeLeft));
            
            Node* background = currentPopup->getChildByName("imgBackground");
            lbl = (Text*)background->getChildByName("lblNextMatchTime");
            long now = SM->getCurrentTime();
            int allDaySeconds = 86400;
            int seconds = (int)now%allDaySeconds;
            lbl->setString(GM->getTimeLeftInString(allDaySeconds - seconds));
        }
        if(currentPopup->getName().compare("shopPopup") == 0){
            Node* layer = this->getChildByName("shopPopup");
            Node* noAds = currentPopup->getChildByName("imgBackground")->getChildByName("sv")->getChildByName("ndAds");
            Text* lbl = (Text*)noAds->getChildByName("lblTimeLeft");
            int timeLeft = 86400 - ((int)now)%86400;
            lbl->setString(strmake("%s: %s", LM->getText("reset in").c_str(), GM->getTimeLeftInStringHMS(timeLeft).c_str()));
            if(layer->getTag() == 4){
                Node* btnLock = noAds->getChildByName("btnLock");
                int availableTime = GM->videoShopWatchedTime + 60*15;
                double now = SM->getCurrentTime();
                btnLock->setVisible(availableTime >= now);
                if (availableTime >= now) {
                    lbl = (Text*)btnLock->getChildByName("lbl");
                    lbl->setString(GM->getTimeLeftInString(availableTime - now));
                    int adsLeft = 10 - GM->videoShopIndex;//UDGetInt(KEY_VIDEO_ADS_LEFT, 10);
                    btnLock->setPosition(noAds->getChildByName(strmake("btn%d", 10 - adsLeft))->getPosition() + Vec2(0, - 45.0f/2));
                }
            }
        }
    }
    Button* btn = (Button*)hudLayer->getChildByName("btnMenu");
    btn->getChildByName("redDot")->setVisible(GM->isCollectionHasNew());
}
void HudLayer::getTopPlayersForLobby(){
    isTopPlayerForFieldRequired = true;
    SM->getTopPlayers(3, "trophy");
}
void HudLayer::getTopPlayersOnField(){
    if(isTopPlayerForFieldReceived){
        return;
    }
    isTopPlayerForFieldReceived = true;
    int limit = 12;
    for (int i = 0; i < topAvatarArray.size(); i++) {
        Node* ndTopPlayer = Node::create();
        WORLD->addChild(ndTopPlayer);
        Vec2 extraPos = Vec2(8, 10);
        if(i == 0){
            ndTopPlayer->setPosition(Vec2(10*TILE_SIZE, 11*TILE_SIZE) + extraPos);
        }else if(i == 1){
            ndTopPlayer->setPosition(Vec2(8*TILE_SIZE, 9*TILE_SIZE) + extraPos);
        }else if(i == 2){
            ndTopPlayer->setPosition(Vec2(12*TILE_SIZE, 9*TILE_SIZE) + extraPos);
        }
        
        ndTopPlayer->removeChildByName("spt");
        int avatar = topAvatarArray.at(i);
        Sprite* spt = GM->getAvatarIcon(avatar);//Sprite::createWithSpriteFrameName(strmake("hero%d_0.png", avatar));
        spt->setScale(0.5f);
        ndTopPlayer->addChild(spt);
        std::string name = strmake("%d. %s", i+1, topNameArray.at(i).c_str());
        if(name.length() > limit){
            name = name.substr(0, limit) + "..";
        }
        
        Node* ndEquip = Node::create();
        ndTopPlayer->addChild(ndEquip);
        std::string strEquip = topEquipArray.at(i);
        ValueVector list = GM->split(strEquip, ",");
        int speed = 60;
        ndEquip->runAction(RepeatForever::create(RotateBy::create(1, speed)));
        int length = 12;
        for (int j = 0; j < 8; j++) {
            int index = list.at(j).asInt();
            if(index < 0) continue;
            Sprite* spt = GM->getItemSprite(index);
            if(GM->getItemType(index) == ITEM_TYPE_SHIELD){
                spt->setScale(0.5f);
            }
            ndEquip->addChild(spt);
            if(j < 4){
                spt->setPosition(Vec2(cos((j%4)*2*360/8*3.14f/180)*length, sin((j%4)*2*360/8*3.14f/180)*length));
            }else{
                spt->setPosition(Vec2(cos(((j%4)*2 + 1)*360/8*3.14f/180)*length, sin(((j%4)*2 + 1)*360/8*3.14f/180)*length));
            }
            spt->setAnchorPoint(Vec2(0.5f, 0));
            spt->runAction(RepeatForever::create(RotateBy::create(1, -speed)));
            //            spt->setPosition(Vec2(25.11f + j*6.5f, 7.98f));
            spt->setScale(0.4f);
            
            int rareness = GM->getItemRareness(index);
            Vector<Node*> starArray = addStarsToIcon(spt, rareness+1);
            for (int k = 0; k < starArray.size(); k++) {
                starArray.at(k)->setPositionY(starArray.at(k)->getPositionY() - 6);
            }
        }
        
        
        Text* lbl = Text::create(name, "Helvetica", 24);
        ndTopPlayer->addChild(lbl);
        lbl->setTextAreaSize(Size(130, 30));
        lbl->setTextHorizontalAlignment(TextHAlignment::CENTER);
        lbl->setScale(0.2f);
        lbl->setPosition(Vec2(0, -13));
    }
}
void HudLayer::handleVideoDone(){
    if(GameManager::getInstance()->getHudLayer()){
        if (GameManager::getInstance()->videoIndex == VIDEO_GAME_OVER) {
            GameManager::getInstance()->getHudLayer()->scheduleOnce(schedule_selector(HudLayer::closeVideoDone), 0.05);
        }else if (GameManager::getInstance()->videoIndex == VIDEO_SHOP) {
            
            double now = SM->getCurrentTime();
//            UDSetDouble(KEY_VIDEO_NEXT_AVAILABLE_TIME, now + 60*15);
            GM->videoShopWatchedTime = now;
            
            int adsLeft = 10 - GM->videoShopIndex;//UDGetInt(KEY_VIDEO_ADS_LEFT, 10);
            if(adsLeft == 9 || adsLeft == 5){
                int ticket = 3;
                if(adsLeft == 5) ticket = 4;
                GM->localTicketCount += ticket;
                SM->saveUserData(strmake("local_ticket_count=%d", GM->localTicketCount));
            }else if(adsLeft == 8 || adsLeft == 4){
                int ticket = 4;
                if(adsLeft == 4) ticket = 5;
                GM->networkTicketCount += ticket;
                SM->saveUserData(strmake("network_ticket_count=%d", GM->networkTicketCount));
            }else{
                int gemToAdd = 10;
                if(adsLeft == 1){
                    gemToAdd = 50;
                }
                GM->addGem(gemToAdd);
                SM->saveUserData(strmake("gem=%d", GM->getGem()));
            }
            adsLeft--;
//            UDSetInt(KEY_VIDEO_ADS_LEFT, adsLeft);
            GM->videoShopIndex = 10 - adsLeft;
            SM->saveUserData(strmake("video_shop_index=%d&video_shop_watched_time=%s", GM->videoShopIndex, Value(GM->videoShopWatchedTime).asString().c_str()));
            updateGemShopButtonPositions(4);
            hideIndicator();
            log("adsLeft: %d", adsLeft);
            showMessageBox("reward", "you received reward", 0)->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
        }else if(GameManager::getInstance()->videoIndex == VIDEO_WEAPON_RENT){
            messageBox->removeFromParent();
        }else if(GameManager::getInstance()->videoIndex == VIDEO_VIDEOSTORE){
            double now = SM->getCurrentTime();
//            UDSetDouble(strmake(KEY_VIDEO_STORE_WATCHED_TIME_FORMAT, selectedVideoStoreTabIndex).c_str(), now);
//            UDSetInt(strmake(KEY_VIDEO_STORE_RECEIVED_INDEX_FORMAT, selectedVideoStoreTabIndex).c_str(), selectedVideoStoreItemIndex);
            if (selectedVideoStoreTabIndex == 0) {
                GM->videoStore0WatchedTime = now;
                GM->videoStore0Index = selectedVideoStoreItemIndex+1;
                SM->saveUserData(strmake("video_store_0_index=%d&video_store_0_watched_time=%s", GM->videoStore0Index, Value(GM->videoStore0WatchedTime).asString().c_str()));
            }else if (selectedVideoStoreTabIndex == 1) {
                GM->videoStore1WatchedTime = now;
                GM->videoStore1Index = selectedVideoStoreItemIndex+1;
                SM->saveUserData(strmake("video_store_1_index=%d&video_store_1_watched_time=%s", GM->videoStore1Index, Value(GM->videoStore1WatchedTime).asString().c_str()));
            }else if (selectedVideoStoreTabIndex == 2) {
                GM->videoStore2WatchedTime = now;
                GM->videoStore2Index = selectedVideoStoreItemIndex+1;
                SM->saveUserData(strmake("video_store_2_index=%d&video_store_2_watched_time=%s", GM->videoStore2Index, Value(GM->videoStore2WatchedTime).asString().c_str()));
            }
            if(selectedVideoStoreTabIndex == 2 || selectedVideoStoreItemIndex < 5 || (selectedVideoStoreItemIndex > 5 && selectedVideoStoreItemIndex < 11)){
                log("gem get: %d", selectedVideoStoreItemCount);
                Button* btn = showMessageBox("reward", "you received reward", selectedVideoStoreItemCount);
                GM->addGem(selectedVideoStoreItemCount);
                SM->saveUserData(strmake("gem=%d", GM->getGem()));
                btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
            }else if(selectedVideoStoreTabIndex == 0){
                log("get random weapon");
                std::vector<int> array;
                array.push_back(getRandomWeapon());
                showItemGetAndAddToBag(array);
            }else if(selectedVideoStoreTabIndex == 1){
                log("get random shield");
                std::vector<int> array;
                array.push_back(getRandomShield());
                showItemGetAndAddToBag(array);
            }
        }
    }
}
void HudLayer::updateEquipedWeapons(){
    WORLD->setWeaponsFromEquiped();
}
std::string HudLayer::getCardNameForRareness(int rareness){
    if(rareness == ITEM_RARENESS_5STAR){
        return "cardLegend.png";
    }else if(rareness == ITEM_RARENESS_4STAR){
        return "cardHero.png";
    }else if(rareness == ITEM_RARENESS_3STAR){
        return "cardBest.png";
    }else if(rareness == ITEM_RARENESS_2STAR){
        return "cardBetter.png";
    }else if(rareness == ITEM_RARENESS_1STAR){
        return "cardGood.png";
    }else{
        return "pnlBaseSimple.png";
    }
    return "";
}
void HudLayer::setButtonTexture(Button* btn, int rareness){
    btn->loadTextureNormal(getCardNameForRareness(rareness));
    btn->loadTexturePressed(getCardNameForRareness(rareness));
    btn->setScale9Enabled(true);
    btn->setContentSize(getCardSize(rareness));
}
void HudLayer::setImageTexture(ImageView* img, int rareness){
    img->loadTexture(getCardNameForRareness(rareness));
    img->setScale9Enabled(true);
    img->setContentSize(getCardSize(rareness));
}
cocos2d::Size HudLayer::getCardSize(int rareness){
    if(rareness == ITEM_RARENESS_5STAR){
        return Size(40, 30);
    }else{
        return Size(29, 26);
    }
}
void HudLayer::setTitle(Node* titleBack, std::string menuKey){
    Text* lbl = (Text*)titleBack->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, menuKey);
    ImageView* img = (ImageView*)titleBack;
    img->setContentSize(Size(lbl->getContentSize().width*lbl->getScaleX() + 25, 15));
    img->setScale9Enabled(true);
    lbl->setPositionX(titleBack->getContentSize().width/2);
//    img->setPositionX(size.width/2);
}
void HudLayer::onServerFailed(){
    hideIndicator();
    showInstanceMessage(LM->getText("network failed restart"));
    if (GM->currentStageIndex == STAGE_LOBBY) {
        this->runAction(Sequence::create(DelayTime::create(3), CallFunc::create(CC_CALLBACK_0(HudLayer::restartGame, this)), nullptr));
    }
}
void HudLayer::restartGame(){
    removeUsedAssets();
    auto scene = Scene::create();
    scene->addChild(MobileTitle::create());
//    scene->addChild(FifteenSixLogo::create());
    Director::getInstance()->replaceScene(scene);
}
std::string HudLayer::getRandomName(){
    int index = rand()%30;
    if(index == 0) return strmake("ParkPill%d", rand()%10);
    else if(index == 1) return strmake("Asdf%d", rand()%10);
    else if(index == 2) return strmake("LumberJack%d", rand()%10);
    else if(index == 3) return strmake("BigCat%d", rand()%10);
    else if(index == 4) return strmake("1506_%d", rand()%10);
    else if(index == 5) return strmake("BuggyLand%d", rand()%10);
    else if(index == 6) return strmake("CartoonCraft_%d", rand()%10);
    else if(index == 7) return strmake("Witch%d", rand()%10);
    else if(index == 8) return strmake("Magician%d", rand()%10);
    else if(index == 9) return strmake("Jimmy%d", rand()%10);
    else if(index == 10) return strmake("Like%d", rand()%10);
    else if(index == 11) return strmake("Wizard%d", rand()%10);
    else if(index == 12) return strmake("Gomez%d", rand()%10);
    else if(index == 13) return strmake("James%d", rand()%10);
    else if(index == 14) return strmake("Lua%d", rand()%10);
    else if(index == 15) return strmake("Jimmy%d", rand()%10);
    else if(index == 16) return strmake("대리%d", rand()%10);
    else if(index == 17) return strmake("구라%d", rand()%10);
    else if(index == 18) return strmake("멜깁%d", rand()%10);
    else if(index == 19) return strmake("원스%d", rand()%10);
    else if(index == 20) return strmake("제너두%d", rand()%10);
    else if(index == 21) return strmake("그레이트%d", rand()%10);
    else if(index == 22) return strmake("위치원%d", rand()%10);
    else if(index == 23) return strmake("그래년%d", rand()%10);
    else if(index == 24) return strmake("나다냥%d", rand()%10);
    else if(index == 25) return strmake("대장%d", rand()%10);
    else if(index == 26) return strmake("팔라딘%d", rand()%10);
    else if(index == 27) return strmake("Paladin%d", rand()%10);
    else if(index == 28) return strmake("Sonia%d", rand()%10);
    else if(index == 29) return strmake("Once%d", rand()%10);
    return "Legend Dary";
}
void HudLayer::onSettingClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Node* layer = CSLoader::createNode("Setting.csb");
    this->addChild(layer, POPUP_ZORDER);
    layer->setName("settingPopup");
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    Button* btn;Text* lbl;
    setTitle(layer->getChildByName("titleBack"), "setting");
    setCloseButtonAndBlock(layer);
    
    Node* background = layer->getChildByName("imgBackground");
    lbl = (Text*)background->getChildByName("lblReportDesc");
    LM->setLocalizedString(lbl, "report bug or cheating");
    
    btn = (Button*)background->getChildByName("btnMusic");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onMusicClick, this));
    btn = (Button*)background->getChildByName("btnAlert");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onAlertClick, this));
    btn = (Button*)background->getChildByName("btnSound");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onSoundClick, this));
    btn = (Button*)background->getChildByName("btnMailTo");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onMailToClick, this));
    
    btn = (Button*)background->getChildByName("btnLanguage");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onLanguageClick, this));
    
    lbl = (Text*)background->getChildByName("lblLanguage");
    LM->setLocalizedString(lbl, "language");
    
    updateSettingPopup();
}
void HudLayer::onLanguageClick(){
    Node* layer = CSLoader::createNode("LanguageSelect.csb");
    this->addChild(layer, POPUP_ZORDER);
    layer->setName("languagePopup");
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    Button* btn;Text* lbl;
    setTitle(layer->getChildByName("titleBack"), "language");
    setCloseButtonAndBlock(layer);
    
    Node* background = layer->getChildByName("imgBackground");
    selectedLanguageWhenOpenLanguagePopup = UDGetInt(KEY_SELECTED_LANGUAGE, -1);
    btn = (Button*)background->getChildByName("btn0");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onLanguageButtonClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    lbl->setFontName("fonts/BMDOHYEON.ttf");
    lbl->setString("한국어");
    
    btn = (Button*)background->getChildByName("btn1");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onLanguageButtonClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    lbl->setFontName("visitor1.ttf");
    lbl->setString("ENGLISH");
    
    btn = (Button*)background->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onLanguageOkClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "ok");
    updateLanguagePopup();
}
void HudLayer::onLanguageButtonClick(Ref* ref){
    BTN_FROM_REF
    if(btn->getTag() == 0){
        UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::KOREAN);
    }else if(btn->getTag() == 1){
        UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::ENGLISH);
    }
    updateLanguagePopup();
}
void HudLayer::updateLanguagePopup(){
    Node* layer = this->getChildByName("languagePopup");
    Node* background = layer->getChildByName("imgBackground");
    Button* button;Text* lbl;
    int selectedLanguage = (int)LM->getLanguageType();
    for (int i = 0; i < 2; i++) {
        button = (Button*)background->getChildByName(strmake("btn%d", i));
        lbl = (Text*)button->getChildByName("lbl");
        if((i == 0 && selectedLanguage == (int)LanguageType::KOREAN) ||
           (i == 1 && selectedLanguage == (int)LanguageType::ENGLISH)){
            button->loadTextureNormal("btnYellow.png");
            lbl->setTextColor(Color4B::BLACK);
        }else{
            button->loadTextureNormal("btnBase.png");
            lbl->setTextColor(Color4B::WHITE);
        }
    }
}
void HudLayer::onLanguageOkClick(){
    if(selectedLanguageWhenOpenLanguagePopup != UDGetInt(KEY_SELECTED_LANGUAGE, -1)){
        restartGame();
    }else{
        closePopup();
    }
}
void HudLayer::updateSettingPopup(){
    Node* layer = this->getChildByName("settingPopup");
    int isOn = UDGetBool(KEY_SOUND_ONOFF, true);
    layer->getChildByName("imgBackground")->getChildByName("btnSound")->getChildByName("imgBan")->setVisible(!isOn);
    isOn = UDGetBool(KEY_MUSIC_ONOFF, true);
    layer->getChildByName("imgBackground")->getChildByName("btnMusic")->getChildByName("imgBan")->setVisible(!isOn);
    isOn = UDGetBool(KEY_ALERT_ONOFF, true);
    layer->getChildByName("imgBackground")->getChildByName("btnAlert")->getChildByName("imgBan")->setVisible(!isOn);
    
    Node* background = layer->getChildByName("imgBackground");
    Button* btn = (Button*)background->getChildByName("btnLanguage");
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LanguageType selectedLanguage = LM->getLanguageType();
    if(selectedLanguage == LanguageType::KOREAN){
        LM->setLocalizedStringNotKey(lbl, "한국어");
    }else if(selectedLanguage == LanguageType::ENGLISH){
        LM->setLocalizedStringNotKey(lbl, "ENGLISH");
    }else{
        LM->setLocalizedStringNotKey(lbl, "ENGLISH");
    }
}
void HudLayer::onSoundClick(){
    bool isOn = UDGetBool(KEY_SOUND_ONOFF, true);
    isOn = !isOn;
    UDSetBool(KEY_SOUND_ONOFF, isOn);
    GM->setSoundVolumn(isOn?1:0);
    updateSettingPopup();
    if(isOn){
        CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(1);
    }else{
        CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(0);
    }
}
void HudLayer::onMusicClick(){
    bool isOn = UDGetBool(KEY_MUSIC_ONOFF, true);
    isOn = !isOn;
    UDSetBool(KEY_MUSIC_ONOFF, isOn);
    GM->setMusicVolumn(isOn?0.7f:0);
    updateSettingPopup();
    if(isOn){
        GM->resumeBGM();
    }else{
        GM->stopBGM();
    }
}
void HudLayer::onAlertClick(){
    bool isOn = UDGetBool(KEY_ALERT_ONOFF, true);
    isOn = !isOn;
    UDSetBool(KEY_ALERT_ONOFF, isOn);
    
    updateSettingPopup();
}
void HudLayer::onMailToClick(){
//    Application::getInstance()->openURL("mailto:sachoi1103@gmail.com?subject=Daily%20Dungeon"); // space is %20
    Application::getInstance()->openURL("mailto:help@morongames.co.kr?subject=Daily%20Dungeon"); // space is %20
}

void HudLayer::onProfileClick(){
    if(GM->isTutorialOn){
        showPrincessSmallTalk(LM->getText("in tutorial follow guide").c_str());
        return;
    }
    Button* btn = (Button*)hudLayer->getChildByName("btnProfile");
    btn->getChildByName("redDot")->setVisible(false);
    Node* layer = CSLoader::createNode("Profile.csb");
    this->addChild(layer, POPUP_ZORDER);
    layer->setName("profilePopup");
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    setTitle(layer->getChildByName("titleBack"), "profile");
    setCloseButtonAndBlock(layer);
    
    Node* background = layer->getChildByName("imgBackground");
    
    Sprite* spt = (Sprite*)background->getChildByName("sptAvatar");
    Sprite* sptAvatar = GM->getAvatarIcon(UDGetInt(KEY_AVATAR, 0));//Sprite::createWithSpriteFrameName(strmake("hero%d_0.png", UDGetInt(KEY_AVATAR, 0)));
    spt->setScale(0.5f);
    background->addChild(sptAvatar);
    sptAvatar->setPosition(spt->getPosition());
    spt->removeFromParent();
    
    Text* lbl= (Text*)background->getChildByName("lblAvatar");
    LM->setLocalizedString(lbl, "avatar");
    
    lbl= (Text*)background->getChildByName("lblLevelAndName");
    lbl->setString(strmake("Lv.%d %s", GM->getGameLevel(), playerName.c_str()));
    lbl= (Text*)background->getChildByName("lblExp");
    LM->setLocalizedStringNotKey(lbl, strmake("EXP %d/%d", GM->getCurrentExp(), GM->getCurrentExpMax()));
    lbl= (Text*)background->getChildByName("lblTrophy");
    LM->setLocalizedStringNotKey(lbl, Value(UDGetInt(KEY_TROPHY, 1000)).asString());
    lbl= (Text*)background->getChildByName("lblRank");
    LM->setLocalizedStringNotKey(lbl, "Loading...");//strmake("%s loading...", LM->getText("rank th").c_str()));
    lbl= (Text*)background->getChildByName("lblID");
    lbl->setString(strmake("ID: %s", UDGetStr(KEY_SAVED_ID,"-1").c_str()));
    
    btn = (Button*)background->getChildByName("btnGiftFromDev");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onGiftFromDevInProfileClick, this));
    lbl= (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "gift from dev team");
    
    btn = (Button*)background->getChildByName("btnVisitForum");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onVisitForum, this));
    lbl= (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "visit forum");
    
    btn = (Button*)background->getChildByName("btnChangeAccount");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onChangeAccountClick, this));
    lbl= (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "account change");
    
    btn = (Button*)background->getChildByName("btnChangeName");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onChangeNameClick, this));
    lbl= (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "change name");
    
    btn = (Button*)background->getChildByName("btnSendMessage");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onMailToClick, this));
    lbl= (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "message to dev team");
    
    showIndicator();
    SM->getMyRank(UDGetInt(KEY_TROPHY, 1000));
    isLoadingRank = true;
}
void HudLayer::onVisitForum(){
//    Button* btn = showMessageBox("reward", "visit forum reward message", 20);
//    GM->addGem(20);
//    SM->saveUserData(strmake("gem=%d", GM->getGem()));
//    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onReceiveVisitForumOk, this));
    onReceiveVisitForumOk();
}
void HudLayer::closePopupAndSaveGem(){
    closePopup();
    SM->saveUserData(strmake("gem=%d", GM->getGem()));
}
Button* HudLayer::showMessageBox(std::string title, std::string msg, int gemReward){
    Node* layer = CSLoader::createNode("MessageBox.csb");
    this->addChild(layer, POPUP_ZORDER);
    layer->setName("messageBox");
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    setTitle(layer->getChildByName("titleBack"), title);
    
    Node* background = layer->getChildByName("imgBackground");
    Button* btn = (Button*)background->getChildByName("btnOk");
    
    Text* lbl = (Text*)background->getChildByName("lblDesc");
    LM->setLocalizedString(lbl, msg);
    
    lbl = (Text*)background->getChildByName("lblCount");
    if(gemReward > 0){
        int rewardCount = gemReward;
        lbl->setString(Value(rewardCount).asString());
    }else{
        lbl->setVisible(false);
        background->getChildByName("sptGem")->setVisible(false);
    }
    
    GM->alignToCenter(background->getChildByName("sptGem"), lbl, 10, background->getContentSize().width/2, 0);
    
    return btn;
}
void HudLayer::onReceiveVisitForumOk(){
    closePopup();
    if (LM->getLanguageType() == LanguageType::KOREAN) {
        Application::getInstance()->openURL("http://www.1506games.com/");
    }else{
        Application::getInstance()->openURL("http://www.1506games.com/private_policy.html");
    }
}
void HudLayer::updateProfile(){
    Node* btnProfile = hudLayer->getChildByName("btnProfile");
    Text* lblLevel = (Text*)btnProfile->getChildByName("lblLevel");
    Text* lblExp = (Text*)btnProfile->getChildByName("lblExp");
    lblLevel->setString(Value(GM->getGameLevel()).asString());
    lblExp->setString(strmake("%d%%", GM->getCurrentExp()*100/GM->getCurrentExpMax()));
    Text* lblName = (Text*)hudLayer->getChildByName("lblName");
    lblName->setFontName("Helvetica");
    playerName = UDGetStr(KEY_NAME, "No name");
    lblName->setString(playerName);
}

void HudLayer::onGiftFromDevInProfileClick(){
    Button* btn = showMessageBox("reward", "you received reward", 20);
    GM->addGem(20);
    SM->saveUserData(strmake("gem=%d", GM->getGem()));
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopupAndSaveGem, this));
}
void HudLayer::setCloseButtonAndBlock(Node* layer){
    Button* btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
}
void HudLayer::onChangeAccountClick(){
    Node* layer = CSLoader::createNode("ExistNameAndID.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("accountChange");
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    
    setTitle(layer->getChildByName("titleBack"), "existing user");
    
    lbl = (Text*)background->getChildByName("lblName");
    LM->setLocalizedString(lbl, "name");
    
    lbl = (Text*)background->getChildByName("lblPassword");
    LM->setLocalizedString(lbl, "password");
    
    TextField* tfName = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfName");
    tfName->setPlaceHolder(LM->getText("name"));
    TextField* tfPassword = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfPassword");
    tfPassword->setPlaceHolder(LM->getText("password"));
    
    btn = (Button*)background->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onOkChangeAccountClick, this));
}
void HudLayer::onOkChangeAccountClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    Node* layer = this->getChildByName("accountChange");
    TextField* tfName = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfName");
    TextField* tfPassword = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfPassword");
    showIndicator();
    isChangingAccount = true;
    SM->changeAccount(tfName->getString(), tfPassword->getString());
}

void HudLayer::onChangeNameClick(){
    Node* layer = CSLoader::createNode("ChangeName.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("changeName");
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    setCloseButtonAndBlock(layer);
    
    setTitle(layer->getChildByName("titleBack"), "change name");
    
    TextField* tfName = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfName");
    tfName->setPlaceHolder(LM->getText("name"));
    
    lbl = (Text*)background->getChildByName("lblName");
    LM->setLocalizedString(lbl, "name");
    
    lbl = (Text*)background->getChildByName("lblAlert");
    LM->setLocalizedString(lbl, "change name alert");
    
    btn = (Button*)background->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onOkChangeNameClick, this));
    
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "change");
}
void HudLayer::onOkChangeNameClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    Node* layer = this->getChildByName("changeName");
    TextField* tfName = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfName");
    std::string name = tfName->getString();
    if(name.length() == 0){
        showInstanceMessage(LM->getText("input name"));
        return;
    }
    showIndicator();
    isChangingName = true;
    SM->changeName(name);
}

void HudLayer::showDailyDungeonResult(){
    Node* layer = CSLoader::createNode("DailyDungeonResult.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    //layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    //layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setName("dailyDungeonResult");
    Button* btn;Text* lbl;
    setTitle(layer->getChildByName("titleBack"), "result");
    setCloseButtonAndBlock(layer);
    
    Node* background = layer->getChildByName("imgBackground");
    
    int enemyTotal[8] = {0,0,0,0,0,0,0,0};
    int enemyKilled[8] = {0,0,0,0,0,0,0,0};
    int score = 0;
    for (int i = 0; i < WORLD->removedEnemyList.size(); i++) {
        if (WORLD->removedEnemyList.at(i) == UNIT_PIERROT_DUMB || WORLD->removedEnemyList.at(i) == UNIT_DUMMY ) {
            enemyKilled[0]++;
            score += 1500;
        }else if (WORLD->removedEnemyList.at(i) == UNIT_PIERROT_SCOT) {
            enemyKilled[1]++;
            score += 5500;
        }else if (WORLD->removedEnemyList.at(i) == UNIT_PIERROT_DOG) {
            enemyKilled[2]++;
            score += 12500;
        }else if (WORLD->removedEnemyList.at(i) == UNIT_PIERROT_TALL) {
            enemyKilled[3]++;
            score += 34000;
        }else if (WORLD->removedEnemyList.at(i) == UNIT_PIERROT_CROSS) {
            enemyKilled[4]++;
            score += 40000;
        }else if (WORLD->removedEnemyList.at(i) == UNIT_PIERROT_GHOST) {
            enemyKilled[5]++;
            score += 50000;
        }else if (WORLD->removedEnemyList.at(i) == UNIT_PIERROT_VERTIAL_EYE) {
            enemyKilled[6]++;
            score += 60000;
        }else if (WORLD->removedEnemyList.at(i) == UNIT_RUNNING_HUD) {
            enemyKilled[7]++;
            score += 70500;
        }
    }
    for (int i = 0; i < WORLD->loadedEnemyList.size(); i++) {
        if (WORLD->loadedEnemyList.at(i) == UNIT_DUMMY) {
            enemyTotal[0]++;
        }else if (WORLD->loadedEnemyList.at(i) == UNIT_PIERROT_SCOT) {
            enemyTotal[1]++;
        }else if (WORLD->loadedEnemyList.at(i) == UNIT_PIERROT_DOG) {
            enemyTotal[2]++;
        }else if (WORLD->loadedEnemyList.at(i) == UNIT_PIERROT_TALL) {
            enemyTotal[3]++;
        }else if (WORLD->loadedEnemyList.at(i) == UNIT_PIERROT_CROSS) {
            enemyTotal[4]++;
        }else if (WORLD->loadedEnemyList.at(i) == UNIT_PIERROT_GHOST) {
            enemyTotal[5]++;
        }else if (WORLD->loadedEnemyList.at(i) == UNIT_PIERROT_VERTIAL_EYE) {
            enemyTotal[6]++;
        }else if (WORLD->loadedEnemyList.at(i) == UNIT_RUNNING_HUD) {
            enemyTotal[7]++;
        }
    }
    Sprite* spt;
    for (int i = 0; i < 8; i++) {
        Node* img = background->getChildByName(strmake("spt%d", i));
        if(i == 0){
            spt = Sprite::createWithSpriteFrameName("dummy.png");
        }else if(i == 1){
            spt = Sprite::createWithSpriteFrameName("pierrotScot0.png");
        }else if(i == 2){
            spt = Sprite::createWithSpriteFrameName("pierrotDog0.png");
        }else if(i == 3){
            spt = Sprite::createWithSpriteFrameName("pierrotTall0.png");
        }else if(i == 4){
            spt = Sprite::createWithSpriteFrameName("pierrotCrossEye0.png");
        }else if(i == 5){
            spt = Sprite::createWithSpriteFrameName("pierrotGhost0.png");
        }else if(i == 6){
            spt = Sprite::createWithSpriteFrameName("pierrotVerticalEye0.png");
        }else if(i == 7){
            spt = Sprite::createWithSpriteFrameName("runningHud0.png");
        }
        spt->setScale(0.7f);
        spt->setPosition(img->getPosition());
        img->removeFromParent();
        background->addChild(spt);
        
        lbl = (Text*)background->getChildByName(strmake("lblKill%d", i));
        lbl->setString(strmake("%d/%d", enemyKilled[i], enemyTotal[i]));
    }
    
    lbl = (Text*)background->getChildByName("lblSuccess");
    LM->setLocalizedString(lbl, enemyKilled[7] > 0?"success":"failed");
    
    lbl = (Text*)background->getChildByName("lblScore");
    LM->setLocalizedStringNotKey(lbl, strmake("%s: %d", LM->getText("score").c_str(), score));
    
    int rewardCount = 0;
    if (score >= 600000){
        rewardCount = 100;
    }else if (score >= 400000){
        rewardCount = 90;
    }else if (score >= 200000){
        rewardCount = 80;
    }else if (score >= 70000){
        rewardCount = 50;
    }else if (score >= 40000){
        rewardCount = 40;
    }else if (score >= 20000){
        rewardCount = 25;
    }else if (score >= 10000){
        rewardCount = 15;
    }
    std::string str;
    int crystal;
    ImageView* imgCrystal = (ImageView*)background->getChildByName("imgCrystal");
    if (GM->currentStageIndex == STAGE_DAILY_DUNGEON_0) {
        str = "crystal_gray";
        crystal = UDGetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, CURRENCY_TYPE_GEM_GRAY).c_str(), 0);
        imgCrystal->loadTexture("stone6.png");
    }else if (GM->currentStageIndex == STAGE_DAILY_DUNGEON_1) {
        str = "crystal_green";
        crystal = UDGetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, CURRENCY_TYPE_GEM_GREEN).c_str(), 0);
        imgCrystal->loadTexture("stone3.png");
    }else if (GM->currentStageIndex == STAGE_DAILY_DUNGEON_2) {
        str = "crystal_orange";
        crystal = UDGetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, CURRENCY_TYPE_GEM_ORANGE).c_str(), 0);
        imgCrystal->loadTexture("stone2.png");
    }else if (GM->currentStageIndex == STAGE_DAILY_DUNGEON_3) {
        str = "crystal_red";
        crystal = UDGetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, CURRENCY_TYPE_GEM_RED).c_str(), 0);
        imgCrystal->loadTexture("stone0.png");
    }
    
    lbl = (Text*)background->getChildByName("lblCrystal");
    lbl->setString(Value(rewardCount).asString());
    
    
    crystal += rewardCount;
    str = strmake("%s=%d", str.c_str(), crystal);
    showIndicator();
    SM->saveUserData(str);
}



//Cartoon Craft,Buggy Land,Defense Saga,Don't Be Greedy,Platform Defense,Blind Slash,Puzzle Royale,SPIN HIT,Brave Rascals,Legend Dary F,Pig Wing,Space Fall - M,Master Of Battle,Dee-Bi-Deep Dungeon,Puppy Moon,Dogstronaut,Mr Boss,Ridiculous Soccer,Legend Dary,Mayday Mayday,대리의 전설 for Kakao,Doodle Girls,Bom Bomy,Mayday Tilt,Gem Master! - The most difficult puzzle!,RCPMan-Let Him Win,RCPMan - For surplus time,Remember Order,Mayday Battle,Mayday Timeattack,
