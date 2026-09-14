#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "LegendDaryButton.h"
#include "HudLayer.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "MyMessageBox.h"
#include "MobileTitle.h"
#include "NativeInterface.h"
#include "Buddies.h"
#include "AwesomeDialogBox.h"
#include "LanguageManager.h"
#include "ServerManager.h"
using namespace cocos2d;
using namespace CocosDenshion;


Scene* HelloWorld::scene(int stage, bool boss)
{
//    stage = 1; // test
    
    GameManager::getInstance()->stopSoundEffect(SOUND_THUNDER);
    GameManager::getInstance()->leftPressed = false;
    GameManager::getInstance()->rightPressed = false;
    GameManager::getInstance()->firePressed = false;
    GameManager::getInstance()->jumpPressed = false;
    GameManager::getInstance()->cPressed = false;
    GameManager::getInstance()->downPressed = false;
    GameManager::getInstance()->upPressed = false;
    Size size = Director::getInstance()->getWinSize();
    Scene *scene = Scene::create();
    int theme = GameManager::getInstance()->theme;
    GameManager::getInstance()->currentStageIndex = stage;
    
    
//    GameManager::getInstance()->showInterstitialAds();
    HudLayer* hudLayer = HudLayer::create();
    scene->addChild(hudLayer, 10);
    GameManager::getInstance()->setHudLayer(hudLayer);
    hudLayer->addListener();
    if (stage == STAGE_LOBBY) {
        hudLayer->getTopPlayersForLobby();
    }
    
    HelloWorld *layer = HelloWorld::create();
    scene->addChild(layer);
    layer->isBossMap = boss;
    layer->addListener();
    int tag = rand()%50;
    layer->setTag(tag);
    hudLayer->setTag(tag);
    CCLOG("** New Tag: %d, theme: %d, stage: %d", tag, GameManager::getInstance()->theme, stage);
    
//    if (boss || true) {//test
    if(boss){
        layer->setBossMap(stage);
    }else{
        layer->setEntireMap(stage);
    }
    
    if(stage == STAGE_LOBBY){
        GM->setCoin(0);
        layer->collectedCoinCount = 0;
        layer->collectedCoinPart = 0;
        GM->appliedShieldArray.clear();
        GM->appliedShieldHPArray.clear();
//        hudLayer->networkStateGetData = NETWORK_HANDLE_STATE_SHOULD_REQUEST;
    }else if (stage == STAGE_ENTRANCE) {
        GameManager::getInstance()->getHudLayer()->hideBtns();
        layer->isGameOver = true;
        layer->blockKeys = true;
    }else{
        GameManager::getInstance()->playSoundEffect(SOUND_DUNGEON_NOISE);
    }
    
    
    hudLayer->hudLayer->getChildByName("btnBattle")->setVisible(stage == STAGE_LOBBY);
    hudLayer->hudLayer->getChildByName("btnMenu")->setVisible(stage == STAGE_LOBBY);
    hudLayer->hudLayer->getChildByName("btnMission")->setVisible(stage == STAGE_LOBBY);
    hudLayer->hudLayer->getChildByName("btnVideoStore")->setVisible(stage == STAGE_LOBBY);
    hudLayer->hudLayer->getChildByName("btnShop")->setVisible(stage == STAGE_LOBBY);
    hudLayer->getChildByName("btnCrystal")->setVisible(stage == STAGE_LOBBY);
    hudLayer->hudLayer->getChildByName("btnSetting")->setVisible(stage == STAGE_LOBBY);
    hudLayer->hudLayer->getChildByName("sptKey")->setVisible(stage != STAGE_LOBBY);
    hudLayer->hudLayer->getChildByName("lblKeyCollected")->setVisible(stage != STAGE_LOBBY);
    hudLayer->hudLayer->getChildByName("sptCoin")->setVisible(stage != STAGE_LOBBY);
    hudLayer->hudLayer->getChildByName("lblCoin")->setVisible(stage != STAGE_LOBBY);
    if(stage == STAGE_LOBBY){
        layer->dnMiniMap->getParent()->getParent()->setVisible(false);
    }
    
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    size = Director::getInstance()->getWinSize();
    //////////////////////////////
    // 1. super init first
    theBoss = NULL;
    
    // test
//    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString(), true);
//    GameManager::getInstance()->isGameClear = true;
    
    GameManager::getInstance()->isInMiddleOfGame = true;
    GameManager::getInstance()->setCurrentStageLayer(this);
    if (!Layer::init())
    {
        return false;
    }
    isReloading = false;
    center = cocos2d::Point(size.width/2, size.height/2);
    killCountForRecord = 0;
    isSetStageDone = false;
    everySecond = false;
    isTouchStarted = false;
    lastClearY = 0;
    isPaused = false;
    isTeleporting = false;
    coinDelay = 0;
    otherDelay = 0;
    missileEffectCollapsedTime = 0;
    playerFireCoolTime = 0;
    this->setKeypadEnabled(true);
//    this->setTouchEnabled(true);
    playerIgnoreGravity = false;
    bulletWasted = false;
    skyBuddyXDistanceMax = 30;
    skyBuddyXDistance = 0;
    skyBuddyHeight = 28;
    skyBuddyCurrentHeight = skyBuddyHeight;
    skyBuddyFloatingHeight = 8;
    isSkyBuddyFloatingUp = true;
    background = NULL;
    spriteBatch = SpriteBatchNode::create("legendDaryDungeon.png");
    spriteBatchForHero = SpriteBatchNode::create("legendDaryDungeon.png");
    
//    dnDamageBoxes = DrawNode::create();
//    this->addChild(dnDamageBoxes, 9999);
    
//    dnCollisionBoxes = DrawNode::create();
//    this->addChild(dnCollisionBoxes, 9999);
    
    enemyPointArray = PointArray::create(100);
    enemyPointArray->retain();
    
    
    // test
    /*
    int weapon;
    while(true){
        weapon = 1 + (WEAPON_NO_MORE/4)*(rand()%4) + rand()%(WEAPON_NO_MORE/4);
        GameManager::getInstance()->setWeaponCollected(weapon);
        
        bool allCollected = false;
        int weaponLeft = 0;
        for (int i = 0; i < WEAPON_NO_MORE; i++) {
            if(UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_WEAPON_COLLECTED_FORMAT, i)->getCString(), false)){
                
            }else{
                weaponLeft++;
            }
        }
        log("weapon left: %d", weaponLeft);
        if (weaponLeft <= 0) {
            break;
        }
    }*/
    
    
    bool isAuto = UserDefault::getInstance()->getBoolForKey(KEY_AUTO_TARGET_SELECTED, true);
    isAutoTargetingOn = isAuto;
    GameManager::getInstance()->getHudLayer()->toggleAuto(false);
    GameManager::getInstance()->getHudLayer()->toggleAuto(false);
    
    
    // init done
    return true;
}
HelloWorld::HelloWorld(){
    
}
void HelloWorld::getDown(){
    if (GameManager::getInstance()->theme == 3) {

    for (auto tele: teleportArray) {
//            projectileRect = RectInset(tele->getBoundingBox(), 4, 4);
//            Rect playerRect = player->collectBoundingBox();
//            if (projectileRect.intersectsRect(playerRect)) {
        Point belowPos = player->getPosition() + Point(0, -TILE_SIZE);
        if(tele->getBoundingBox().containsPoint(belowPos))
            if(!isTeleporting ){
                switch (tele->getTag()) {
                    case TAG_TELEPORT_0_ENTRY:
                    positionToTeleport = (getTeleport(TAG_TELEPORT_0_EXIT)->getPosition() + Point(0, -8));
                    break;
                    case TAG_TELEPORT_0_EXIT:
                    positionToTeleport = (getTeleport(TAG_TELEPORT_0_ENTRY)->getPosition() + Point(0, -8));
                    break;
                    case TAG_TELEPORT_1_ENTRY:
                    positionToTeleport = (getTeleport(TAG_TELEPORT_1_EXIT)->getPosition() + Point(0, -8));
                    break;
                    case TAG_TELEPORT_1_EXIT:
                    positionToTeleport = (getTeleport(TAG_TELEPORT_1_ENTRY)->getPosition() + Point(0, -8));
                    break;
                    case TAG_TELEPORT_2_ENTRY:
                    positionToTeleport = (getTeleport(TAG_TELEPORT_2_EXIT)->getPosition() + Point(0, -8));
                    break;
                    case TAG_TELEPORT_2_EXIT:
                    positionToTeleport = (getTeleport(TAG_TELEPORT_2_ENTRY)->getPosition() + Point(0, -8));
                    break;
                    default:
                    break;
                }
                
                float duration = 1;
                indestructableTime =  2.5f;
                this->schedule(schedule_selector(HelloWorld::immortalForAWhile), 0.1);
                
                playerIgnoreGravity = true;
                if (player->currentPlatform) {
                    player->currentPlatform->player = NULL;
                    player->currentPlatform = NULL;
                }
                player->setPosition(Point(tele->getPosition().x - 3, player->getPosition().y));
                player->runAction(MoveBy::create(duration, Point(0, -30)));
                
                spriteBatch->setLocalZOrder(3);
                
                this->scheduleOnce(schedule_selector(HelloWorld::teleportLater), duration);
                player->setAnchorPoint(Point(0.5, 2));
                isTeleporting = true;
                
                if(player->vehicleType != VEHICLE_NONE &&
                   player->vehicleType != VEHICLE_CART){
                    getOffVehicle();
                }
                GameManager::getInstance()->playSoundEffect(SOUND_POWER_UP_45);
                
                return;
            }
        }
    }

    player->oneWayCancelY = player->collisionBoundingBox().origin.y;
    player->onGround = false;
}
void HelloWorld::dialogFinished(Ref* obj){
    
}
void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* unused_event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE){
//        NativeInterface::NativeInterface::destroyAds();
//        Director::getInstance()->replaceScene(StageSelectScene::scene(GameManager::getInstance()->theme));
    }
}
void HelloWorld::showWarpAnimation(){
    Sprite* sptBeam = Sprite::createWithSpriteFrameName("blueBeamVertical0.png");
    sptBeam->setAnchorPoint(Point(0.5, 0));
    sptBeam->setScaleY(10);
    sptBeam->setScaleX(2);
    spriteBatchForHero->addChild(sptBeam, 11);
    const char* beamName = "blueBeam";
    if (stageIndex == indexToWarp || (isBossMap && !isSetStageDone)) {
        beamName = "redBeam";
    }
    Animation* animation = AnimationCache::getInstance()->getAnimation(beamName);
    Animate* animate = Animate::create(animation);
    sptBeam->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptBeam)), CallFunc::create(CC_CALLBACK_0( HelloWorld::checkEnding, this)), NULL));
    Point pos;
    if (player->encounteredNpc == nullptr) {
        pos = player->getPosition();
    }else{
        pos = player->encounteredNpc->getPosition() + Point(0, -player->encounteredNpc->getContentSize().height/2);
    }
    sptBeam->setPosition(pos + Point(0, 100));
    sptBeam->runAction(MoveTo::create(0.05, pos));
    CCLOG("Player pos: %f, %f", player->getPositionX(), player->getPositionY());
    GameManager::getInstance()->playSoundEffect(SOUND_LASER_WOONG);
    this->runAction(Sequence::create(DelayTime::create(18*0.04), CallFunc::create(CC_CALLBACK_0(HelloWorld::stopWoongSound, this)), nullptr));
    shakeScreen(6);
    
    
//    this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(HelloWorld::testFunction, this)), NULL));
}
void HelloWorld::checkEnding(){
    if (stageIndex != STAGE_LOBBY) {
        return;
    }
    // check if it's time to show ending
    bool lastJewelCollected = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString());
    bool gameCleared = GameManager::getInstance()->isGameClear;
    if (lastJewelCollected && gameCleared) {
        
        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_DOES_IT_TALKED_FORMAT, "story4")->getCString(), false);
        
        HoodGhost* son;
        for(auto npc : npcArray){
            log("*npc: %d", npc->getTag());
            if(npc->getTag() == UNIT_HOOD_GHOST){
                son = (HoodGhost*)npc;
            }
        }
        
        GameManager::getInstance()->getHudLayer()->showDialog(son->getTalk());
        
        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 0)->getCString(), false);
        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 1)->getCString(), false);
        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 2)->getCString(), false);
        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString(), false);
        
        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_DOES_IT_TALKED_FORMAT, "story4")->getCString(), false);
        
    }
}
void HelloWorld::testFunction(){
    GameManager::getInstance()->getHudLayer()->dialogBox->removeFromParent();
    GameManager::getInstance()->getHudLayer()->showEnding(); // test
}
void HelloWorld::takeAction(){
    log("take action");
    GameManager::getInstance()->life = player->energy;
    GameManager::getInstance()->getHudLayer()->lblTouch->stopAllActions();
    GameManager::getInstance()->getHudLayer()->lblTouch->setOpacity(0);
    if (player->encounteredNpc != nullptr) {
        if (player->encounteredNpc->getTag() == UNIT_GATE) {
            log("npc x, y: %d, %d", (int)player->encounteredNpc->getPosition().x, (int)player->encounteredNpc->getPosition().y);
//            int slot = 0;
//            while (getWeaponAtSlot(slot) >= 0){
//                changeWeapon(slot);
//                slot++;
//            }
            player->freezeForAWhile(1);
            isGameOver = true;
            if (stageIndex == STAGE_LOBBY) {
                Vec2 pos = player->encounteredNpc->getPosition();
                int x = player->encounteredNpc->getPosition().x;
                if (x == 272 ) {
                    if (UserDefault::getInstance()->getBoolForKey(KEY_IS_TUTORIAL_CLEAR, false)) {
                        indexToWarp = 0;//test
//                        indexToWarp = 2; // test
//                        stageIndex = -1; // test
                    }else{
                        GameManager::getInstance()->getHudLayer()->showDialog(theKnight->getTalk());
                        isGameOver = false;
                        return;
                    }
                }else if(player->encounteredNpc->getPosition().x == 624){
                    indexToWarp = STAGE_NAMING;
                }else{
                    indexToWarp = TUTORIAL_STAGE_START;
                    GameSharing::firebaseLog("tutorial_start", "tutorial_start", "tutorial_start");
                }
            }else if (stageIndex == STAGE_ENTRANCE) {
                indexToWarp = STAGE_LOBBY;
            }else if(stageIndex == STAGE_NAMING){
                UserDefault::getInstance()->setStringForKey(KEY_NAME, GameManager::getInstance()->getHudLayer()->originalString);
                indexToWarp = STAGE_LOBBY;
            }else{
                indexToWarp = player->encounteredNpc->secondTag;
                if (stageIndex >= TUTORIAL_STAGE_START && stageIndex <= TUTORIAL_STAGE_END) {
                    std::string str = strmake("tutorial_%d", stageIndex - TUTORIAL_STAGE_START + 1);
                    GameSharing::firebaseLog(str.c_str(), str.c_str(), str.c_str());
                }else if(stageIndex >= 0 && stageIndex <= 4 && isBossMap){
                    int clearCount = UDGetInt(KEY_CLEAR_COUNT, 0);
                    std::string str = strmake("dungeon_%d_%d", clearCount + 1, stageIndex + 1);
                    GameSharing::firebaseLog(str.c_str(), str.c_str(), str.c_str());
                }
                
                if (indexToWarp == TUTORIAL_STAGE_END + 1) {
                    indexToWarp = STAGE_LOBBY;
                    GameSharing::firebaseLog("tutorial_complete", "tutorial_complete", "tutorial_complete");
//                    UserDefault::getInstance()->setBoolForKey(KEY_IS_TUTORIAL_CLEAR, true);
                    if (GM->isTutorialOn) {
                        GM->tutorialType = TUTORIAL_MENU;
                        GM->tutorialIndex = 0;
                    }
                }
            }
            GameManager::getInstance()->setCoin(collectedCoinCount);
            player->gun->setVisible(false);
            player->velocity.x = 0;
            warpOut();
            GameManager::getInstance()->getHudLayer()->hideBtns();
        }else if (player->encounteredNpc->getTag() == UNIT_NPC ||
                  player->encounteredNpc->getTag() == UNIT_HOOD_GHOST ||
                  player->encounteredNpc->getTag() == UNIT_THE_KNIGHT) {
            GameManager::getInstance()->getHudLayer()->showDialog(player->encounteredNpc->getTalk());
            lastTalkedNpc = player->encounteredNpc;
        }else if(player->encounteredNpc->getTag() == UNIT_ONSALE){
            OnSale* onSale = (OnSale*)player->encounteredNpc;
//            onSale->buyItem();
            if (!onSale->isExplained) {
                GameManager::getInstance()->getHudLayer()->showDialog(player->encounteredNpc->getTalk());
            }
        }
    }
}
void HelloWorld::warpOut(){
    player->runAction(MoveTo::create(0.1, player->encounteredNpc->getPosition()));
    player->runAction(Sequence::create(DelayTime::create(0.0), FadeOut::create(0.4), NULL));
    player->face->runAction(Sequence::create(DelayTime::create(0.0), FadeOut::create(0.4), NULL));
    player->hand->runAction(Sequence::create(DelayTime::create(0.0), FadeOut::create(0.4), NULL));
    player->sptTalkBalloon->runAction(Sequence::create(DelayTime::create(0.3), FadeOut::create(0.1), NULL));
    for(auto shield : playerShields){
        shield->runAction(FadeOut::create(0.3f));
    }
    player->wantsToStop = true;
    player->wantsToGoLeft = false;
    player->wantsToGoRight = false;
    GameManager::getInstance()->leftPressed = false;
    GameManager::getInstance()->rightPressed = false;
    this->runAction(Sequence::create(DelayTime::create(0.1), CallFunc::create(CC_CALLBACK_0(HelloWorld::showWarpAnimation, this)), DelayTime::create(2), CallFunc::create(CC_CALLBACK_0(HelloWorld::warp, this)), NULL));
    
    GM->playerEnergy = player->energy;
    GM->playerEnergyMax = player->maxEnergy;
    GM->playerShieldIndices.clear();
    GM->playerShieldHps.clear();
    for(auto shield : playerShields){
        GM->playerShieldIndices.push_back(shield->unitType);
        GM->playerShieldHps.push_back(shield->energy);
    }
    
}
void HelloWorld::warp(){
    GameManager::getInstance()->getHudLayer()->showStageTitle();
    
    this->runAction(Sequence::create(DelayTime::create(4), CallFunc::create(CC_CALLBACK_0(HelloWorld::goToNextScene, this)), NULL));
}
void HelloWorld::goToNextScene(){
    GameManager::getInstance()->getHudLayer()->removeListener();
    Scene* scene = Scene::create();
    bool isBoss = false;
    if (stageIndex >= 0 && !isBossMap) {
        isBoss = true;
    }
    scene->addChild(HelloWorld::scene(indexToWarp, isBoss));
    Director::getInstance()->replaceScene(scene);
}
void HelloWorld::enemyUpdate(float dt)
{
    if (isGameOver) {
        return;
    }
//    dnDamageBoxes->clear();
//    dnDamageBoxes->drawRect(player->damageBoundingBox().origin, player->damageBoundingBox().origin + player->damageBoundingBox().size, Color4F::RED);
//    dnCollisionBoxes->clear();
//    dnCollisionBoxes->drawRect(player->collisionBoundingBox().origin, player->collisionBoundingBox().origin + player->collisionBoundingBox().size, Color4F::BLUE);
    Rect projectileRect;
    for (auto drop: enemyArray){
        if (drop->getPosition().y-player->getPosition().y > 50 || drop->getPosition().y - player->getPosition().y < -50) {
            continue;
        }
        
        if(drop->getTag() == UNIT_RED_THORN_BOTTOM && skyBuddy && skyBuddy->petNumber == PET_STONE_DRAGON){
            continue;
        }
        projectileRect = drop->damageBoundingBox();
//        dnDamageBoxes->drawRect(drop->damageBoundingBox().origin, drop->damageBoundingBox().origin + drop->damageBoundingBox().size, Color4F::RED);
        // Test codes
        /*Sprite* rectSprite = (Sprite*)drop->getUserData();
        if (!rectSprite) {
            rectSprite = Sprite::create("blackSquare.png");
            rectSprite->setAnchorPoint(Point::ZERO);
            this->addChild(rectSprite, 100);
            drop->setUserData(rectSprite);
        }
        rectSprite->setScale(projectileRect.size.width, projectileRect.size.height);
        rectSprite->setPosition(projectileRect.origin);
        
        rectSprite = (Sprite*)player->getUserData();
        if (!rectSprite) {
            rectSprite = Sprite::create("blackSquare.png");
            rectSprite->setAnchorPoint(Point::ZERO);
            this->addChild(rectSprite, 100);
            player->setUserData(rectSprite);
        }
        rectSprite->setScale(player->damageBoundingBox().size.width, player->damageBoundingBox().size.height);
        rectSprite->setPosition(player->damageBoundingBox().origin);
        */
        if (projectileRect.intersectsRect(player->damageBoundingBox())) {
            if (player->isDropping && player->getPosition().y > projectileRect.origin.y + projectileRect.size.height && !drop->untouchable && indestructableTime <= 0 && !player->immortal) {
//                player->onLanded();
//                player->wantsToJump = true;
//                player->tumbled = true;
                player->isDropping = false;
                player->desiredPosition = player->getPosition() + Point(0, 1);
                player->velocity = Point(player->velocity.x, 240);
                indestructableTime = 0.5;
                this->schedule(schedule_selector(HelloWorld::immortalForAWhile), 0.1f);
//                player->tumbleAnimation();
                player->sptDropEffect->setVisible(false);
                if (!drop->immortal) {
                    attackEnemy(drop, 10);
                }
            }else{
                if (hitDary(drop->maxEnergy*0.1f)) {
                    return;
                }
            }
        }
    }
    
    Trap* trap;
    for (int i = 0; i<trapArray.size(); i++) {
        trap = trapArray.at(i);
        
        if (!trap->_isHurting || trap->getPosition().y-player->getPosition().y > 50 || trap->getPosition().y - player->getPosition().y < -50) {
            continue;
        }
        
        projectileRect = trap->getBoundingBox();
        projectileRect = RectInset(projectileRect, 3, 3);
//        dnDamageBoxes->drawRect(projectileRect.origin, projectileRect.origin + projectileRect.size, Color4F::RED);
        
        if (projectileRect.intersectsRect(player->damageBoundingBox())) {
            hitDary(player->maxEnergy*0.25f);
//            if(!(trap->trapType == TRAP_TYPE_LASER && skyBuddy && skyBuddy->petNumber == PET_IRON_DRAGON)){
//                demagingUnit = trap->getTag();
//                if(hitDary(player->maxEnergy*0.25f)){
                    /*if (player->costume == COSTUME_THIEF) {
                     Droppable* coin;
                     for (int i = 0; i < 5; i++) {
                     if(rand()%101 < 20){
                     coin = Droppable::createDroppable(UNIT_GOLD_COIN, 1, 0, "goldCoin0.png");
                     }else{
                     coin = Droppable::createDroppable(UNIT_COIN, 1, 0, "silverCoin0.png");
                     }
                     coin->velocity = Point(400 - rand()%800, 50 + rand()%200);
                     coinArray.pushBack(coin);
                     spriteBatch->addChild(coin);
                     droppableCoinArray.pushBack(coin);
                     
                     coin->setPosition(player->getPosition() + Point(0, 36));
                     }
                     }*/
//                }
//            }
        }
    }
    Vector<Missile*> msToRemove;
    for (auto missile: enemyMissileArray) {
        projectileRect = Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
                              missile->getPosition().y - (missile->getContentSize().height*missile->getScaleY()/2),
                              missile->getContentSize().width*missile->getScaleX(),
                              missile->getContentSize().height*missile->getScaleY());
        projectileRect = RectInset(projectileRect, 3, 3);
        if (projectileRect.intersectsRect(player->damageBoundingBox()) && missile->isHurting) {
            if(missile->secondTag == UNIT_APPLEMON_WORM){
                player->setAppleWorm();
            }else{
                if(!player->immortal){
                    if(skyBuddy && skyBuddy->petNumber == PET_WOOD_DRAGON){
                        
                    }else{
                        demagingUnit = missile->secondTag;
                        hitDary(missile->energy);
                        enemyMissileArray.eraseObject(missile);
                    }
                }
            }
            enemyMissileArray.eraseObject(missile);
            break;
        }
        
        for (int i = 0; i<playerShields.size(); i++) {
            Droppable* shield = playerShields.at(i);
            
            if(!missile){
                break;
            }
            if(missile->isPiercing && missile->victimArray.size() >= 0 && missile->victimArray.size() < 10){
                log("missile->victimArray %d", (int)missile->victimArray.size());
                if(missile->victimArray.contains(shield)){
                    continue;
                }
            }
            
            if (shield->getPosition().y-missile->getPosition().y > 50 ||
                shield->getPosition().y - missile->getPosition().y < -50) {
                continue;
            }
            
            if (shield->getBoundingBox().intersectsRect(projectileRect)) {
                Point pos = shield->getPosition();
                float damage = missile->energy + missile->energy*extraPower/100.0f;
                
                if (damage >= 10) {
                    damage = damage*0.8f + (rand()%((int)(damage*0.4f)));
                }
                if (missile->parentWeaponIndex%4 == shield->unitType%4) {
                    damage *= 0.5f;
                }
                damage -= shield->DP;
                if(damage < 0){
                    damage = 1;
                }
                shield->energy -= damage;
                if(shield->energy <= 0){
                    playerShields.eraseObject(shield);
                    shield->removeFromParent();
                    break;
                }
                
                totalPower += damage;
                
                showDamage(damage, shield->getPosition());
                
                this->runEffect(EFFECT_EXPLODE_SMALL, shield->getPosition());
                
                if(missile->secondTag == WEAPON_FIRE_BOTTLER ||
                   missile->secondTag == WEAPON_RIFLE_GRENADE ||
                   missile->secondTag == WEAPON_BAZOOKA ||
                   missile->secondTag == WEAPON_ROCKET_LAUNCHER ||
                   missile->secondTag == WEAPON_GUIDED_ROCKET_LAUNCHER){
                    splashDemage(missile->getPosition(), 40, damage/2);
                    CCLOG("in missile updat");
                }
                if(missile->isPiercing  && missile->victimArray.size() >= 0 && missile->victimArray.size() < 10){
                    missile->victimArray.pushBack(shield);
                }else{
                    msToRemove.pushBack(missile);
                }
                this->runEffect(EFFECT_EXPLODE_SMALL, pos + Point(rand()%16-8, rand()%16-8));
                GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
                enemyMissileArray.eraseObject(missile);
                break;
            }
        }
    }
    
    for (auto ms: msToRemove) {
        enemyMissileArray.eraseObject(ms);
        ms->removeFromParent();
    }
    msToRemove.clear();

    for(auto mace: maceArray){
        float r = mace->getContentSize().height;
        float angle = -mace->getRotation() + 270;
        double rad = angle*3.14/180;
        Point pos = mace->getPosition() + Point(cos(rad)*r,sin(rad)*r) + Point(-mace->head->getContentSize().width/2, -mace->head->getContentSize().height/2);
        Rect headRect = Rect(pos.x, pos.y, mace->head->getContentSize().width, mace->head->getContentSize().height);
        if (headRect.intersectsRect(player->damageBoundingBox())) {
            hitDary(player->maxEnergy*0.25f);
            demagingUnit = UNIT_MACE_TRAP;
            break;
        }
    }
    for(auto mace: axeArray){
        float r = mace->getContentSize().height*1.4;
        float angle = -mace->getRotation() + 225;
        double rad = angle*3.14/180;
        Point pos = mace->getPosition() + Point(cos(rad)*r,sin(rad)*r) + Point(-mace->head->getContentSize().width/2, -mace->head->getContentSize().height/2);
        Rect headRect = Rect(pos.x, pos.y, mace->head->getContentSize().width, mace->head->getContentSize().height);
        if (headRect.intersectsRect(player->damageBoundingBox())) {
            hitDary(player->maxEnergy*0.25f);
            demagingUnit = UNIT_MACE_TRAP;
            break;
        }
    }
}
void HelloWorld::updateFireStick(float dt){
    
    float stickR = 0;
    float duration = 4;
    for(auto stick: fireStickArray){
        stick->currentAngle += (stick->isClockWise?-360:360)*dt/duration;
        stickR = stick->getContentSize().width;
        double rad = stick->currentAngle*3.14/180;
        for (auto ball: stick->fireArray) {
            Point pos = stick->getPosition() + Point(cos(rad)*stickR,sin(rad)*stickR);
            ball->setPosition(pos);
            
            Rect headRect = Rect(pos.x, pos.y, ball->getContentSize().width, ball->getContentSize().height);
            
            //        headRect = RectInset(headRect, 3, 3);
            if (headRect.intersectsRect(player->damageBoundingBox())) {
                hitDary(player->maxEnergy*0.25f);
                demagingUnit = UNIT_FIRE_STICK;
            }
            stickR += stick->getContentSize().width;
        }
        if (stick->boundingBox().intersectsRect(player->damageBoundingBox())) {
            hitDary(player->maxEnergy*0.25f);
            demagingUnit = UNIT_FIRE_STICK;
        }
    }
}
void HelloWorld::destructableUpdate()
{
    if (isGameOver) {
        return;
    }
    
    Rect projectileRect, mRect;
    Vector<Missile*> missileToRemove;
    EnemyBase* drop;
    for (auto missile: heroMissileArray) {
        for (int i = 0; i<destructableArray.size(); i++) {
            drop = destructableArray.at(i);
            
            if (drop->untouchable ||
                drop->getPosition().y-missile->getPosition().y > 50 ||
                drop->getPosition().y - missile->getPosition().y < -50) {
                continue;
            }
            
            mRect = Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
                         missile->getPosition().y - (missile->getContentSize().height*missile->getScaleY()/2),
                         missile->getContentSize().width*missile->getScaleX(),
                         missile->getContentSize().height*missile->getScaleY());
            projectileRect = drop->damageBoundingBox();
            if (projectileRect.intersectsRect(mRect)) {
                if (!drop->immortal) {
                    float damage = missile->energy;
                    log("damage: %f, missile->energy: %f", damage, missile->energy);
                    if (damage >= 10) {
                        damage = damage*0.8f + (rand()%((int)(damage*0.2f)));
                    }
                    if(rand()%100 < missile->criticalPossibility){
                        damage *= 2+criticalLevel;
                    }
                    
                    drop->energy -= (int)damage;
                    log("actual damage: %f", damage);
                    showDamage(damage, drop->getPosition());
                    if (drop->energy <= 0) {
                        destroyDestructable(drop);
                        currentScore += drop->secondTag*10;
                        GameManager::getInstance()->getHudLayer()->setScore(currentScore);
                        createRewards(drop);
                        destructableArray.eraseObject(drop);
                        drop->removeFromParentAndCleanup(true);
                        
                        GameManager::getInstance()->totalDestroy++;
                        GameManager::getInstance()->totalScore += 10;
                        GameManager::getInstance()->getHudLayer()->setScore(GameManager::getInstance()->totalScore);
                    }
                    
                    if (drop->getTag() == UNIT_KEYBOARD_KEY) {
                        ((Alphabet*)drop)->keyDownTime = 0.1f;
                        GameManager::getInstance()->getHudLayer()->typing(((Alphabet*)drop)->lblChar->getString());
                    }
                }
                
                missileToRemove.pushBack(missile);
                this->runEffect(EFFECT_BULLET_WALL_SMALL, drop->getPosition(), missile->getRotation() + 180);
                break;
            }
        }
    }
    
    for (auto missile: enemyMissileArray) {
        for (int i = 0; i<destructableArray.size(); i++) {
            drop = destructableArray.at(i);
            
            if (drop->untouchable ||
                drop->getPosition().y-missile->getPosition().y > 50 ||
                drop->getPosition().y - missile->getPosition().y < -50) {
                continue;
            }
            
            mRect = Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
                         missile->getPosition().y - (missile->getContentSize().height*missile->getScaleY()/2),
                         missile->getContentSize().width*missile->getScaleX(),
                         missile->getContentSize().height*missile->getScaleY());
            projectileRect = drop->damageBoundingBox();
            if (projectileRect.intersectsRect(mRect)) {
                if (!drop->immortal) {
                    float demage = missile->energy;
                    
                    drop->energy -= (int)10;
                }
                if (drop->energy <= 0) {
                    destroyDestructable(drop);
                    createRewards(drop);
                    destructableArray.eraseObject(drop);
                    drop->removeFromParentAndCleanup(true);
                }
                missileToRemove.pushBack(missile);
                this->runEffect(EFFECT_EXPLODE_MIDDLE, drop->getPosition());
                break;
            }
        }
    }
    
    for (auto spt: missileToRemove){
        spt->stopAllActions();
        heroMissileArray.eraseObject(spt);
        enemyMissileArray.eraseObject(spt);
        droppableMissileArray.eraseObject(spt);
        enemyArray.eraseObject((EnemyBase*)spt);
        if(spt->getTag() == UNIT_MISSILE_CHASING){
            chasingMissileArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_DROPPABLE){
            droppableMissileArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_CUSTOM){
            customMovingArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_STRAIGHT){
            straightMovingArray.eraseObject(spt);
        }
        spt->removeFromParentAndCleanup(true);
    }
//    enemiesToRemove.clear();
}
void HelloWorld::destroyDestructable(Droppable* drop){
    if (drop->secondTag < 0) {
        return;
    }
    const char* partName;
    if (drop->secondTag < 3) {
        partName = __String::createWithFormat("destructable%d_0.png", drop->secondTag)->getCString();
        GameManager::getInstance()->playSoundEffect(SOUND_GLASS_BREAK);
    }else{
        partName = "destructable3_0.png";
        GameManager::getInstance()->playSoundEffect(SOUND_WOOD_CRASH);
        GameManager::getInstance()->playSoundEffect(SOUND_WOOD_DRAP);
    }
    for (int i = 0; i < 3; i++) {
        Droppable* part = Droppable::createDroppable(UNIT_DESTRUCTABLE, 0, 0, partName);
        spriteBatch->addChild(part, 1);
        part->setPosition(drop->getPosition());
        part->weight = 1;
        auto rotate = RotateBy::create(1, rand()%2 == 0?300:-300);
        rotate->setTag(ACTION_TAG_ROTATION);
        part->runAction(rotate);
        part->velocity = Point(rand()%500 - 250, 250);
        part->runAction(Sequence::create(DelayTime::create(3), CallFuncN::create(CC_CALLBACK_1(HelloWorld::coinWaitDone, this)), NULL));
        droppableCoinArray.pushBack(part);
    }
    runEffect(EFFECT_EXPLODE_BIG, drop->getPosition());
}
void HelloWorld::showDamage(int damage, Point pos){
    if (damage <= 0) {
        return;
    }
    Node* sptNumber = createNumberSprite(damage);
    spriteBatch->addChild(sptNumber, 100);
    sptNumber->setScale(0.5);
    sptNumber->setPosition(pos);
    sptNumber->runAction(EaseIn::create(MoveBy::create(1, cocos2d::Point(0, 25)), 0.3));
    sptNumber->runAction(Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptNumber)), NULL));
}
void HelloWorld::attackEnemy(EnemyBase* drop, int demage){
    drop->energy -= (int)demage;
    
    showDamage(demage, drop->getPosition());
    
    if (drop->energy <= 0) {
        this->runEffect(EFFECT_EXPLODE_SMALL, drop->getPosition());
        this->createRewards(drop);
        
        killCountForRecord++;
        GameManager::getInstance()->totalKill++;
        GameManager::getInstance()->totalScore += 100;
        GameManager::getInstance()->getHudLayer()->setScore(GameManager::getInstance()->totalScore);
        
        destructableArray.eraseObject(drop);
        droppableArray.eraseObject(drop);
        spriteBatch->removeChild(drop, true);
        removeEnemy(drop);
        
    }
    else
    {
        drop->blinkForAWhile();
    }
    drop->showEnergy();
}
void HelloWorld::immortalForAWhile(float dt){
    indestructableTime -= dt;
    if (indestructableTime < 0) {
        this->unschedule(schedule_selector(HelloWorld::immortalForAWhile));
    }
}
bool HelloWorld::hitDary(float damage){
//    return true; // test
    if (GM->developer) return true;
    if (indestructableTime <= 0 && !player->immortal) {
        indestructableTime =  1.6f;
        this->schedule(schedule_selector(HelloWorld::immortalForAWhile), 0.1f);
        if (!GameManager::getInstance()->developer) {
            
            if(damage == 0){
                if(stageIndex <= -90){
                    
                }else{
                    player->energy -= player->maxEnergy*0.25f;
                }
                
            }else{
                player->energy -= damage;
            }
            GameManager::getInstance()->getHudLayer()->setLife(player->energy);
            showDamage(damage, player->getPosition());
        }
//        player->face->setVisible(true);
        player->freezed = true;
        player->stopAndStand();
        this->scheduleOnce(schedule_selector(HelloWorld::stepBackForHit), 0.02f);
        this->scheduleOnce(schedule_selector(HelloWorld::unfreezePlayer), 0.4f);
        
        blinkingTime = 1.6f;
        this->schedule(schedule_selector(HelloWorld::blinkingPlayer), 0.1f);
        
        if (player->energy <= 0) {
            gameOver();
        }
        return true;
    }
    return false;
}
void HelloWorld::stepBackForHit(float dt)
{
    player->velocity = Point(player->isTowardLeft?1000:-1000, player->inWater?50:220);
}
void HelloWorld::unfreezePlayer(float dt)
{
    player->freezed = false;
}

Rect HelloWorld::RectInset(Rect rect, float x, float y)
{
    return Rect(rect.origin.x + x, rect.origin.y + y, rect.size.width - x*2, rect.size.height - y*2);
}

void HelloWorld::angelReviveLater(float dt){
    revive();
    buddyArray.eraseObject(skyBuddy);
    runEffect(EFFECT_TWINKLE, skyBuddy->getPosition());
    skyBuddy->removeFromParentAndCleanup(true);
    skyBuddy = NULL;
}
void HelloWorld::endGame(){
    //this->unschedule(schedule_selector(HelloWorld::gravityUpdate));
    this->unschedule(schedule_selector(HelloWorld::gravityUpdateForCoins));
    this->unschedule(schedule_selector(HelloWorld::gravityUpdateForStraight));
    this->unschedule(schedule_selector(HelloWorld::gravityUpdateForCustomMoving));
    this->unschedule(schedule_selector(HelloWorld::gravityUpdateForFlyingOrSwimingEnemies));
    
    for(auto enemy : enemyArray){
        enemy->pause();
    }
    showResult();
}
void HelloWorld::showResult(){
    if (GM->currentStageIndex <= STAGE_DAILY_DUNGEON_0 && GM->currentStageIndex >= STAGE_DAILY_DUNGEON_3) {
        HUD->showDailyDungeonResult();
        return;
    }
    resultLayer = CSLoader::createNodeWithVisibleSize("Result.csb");
//    resultLayer->setPositionX(size.width/2 - resultLayer->getContentSize().width/2);
    this->getParent()->addChild(resultLayer, 2003);
    resultLayer->setPositionY(size.height);
    resultLayer->runAction(EaseElasticOut::create(MoveBy::create(1, Point(0, -size.height)), 2));
    
    resultLayer->getChildByName("lblKill")->setVisible(false);
    resultLayer->getChildByName("lblKillValue")->setVisible(false);
    resultLayer->getChildByName("lblDestroy")->setVisible(false);
    resultLayer->getChildByName("lblDestroyValue")->setVisible(false);
    resultLayer->getChildByName("lblCoin")->setVisible(false);
    resultLayer->getChildByName("lblCoinValue")->setVisible(false);
    resultLayer->getChildByName("lblRoom")->setVisible(false);
    resultLayer->getChildByName("lblRoomValue")->setVisible(false);
    resultLayer->getChildByName("lblBoss")->setVisible(false);
    resultLayer->getChildByName("lblBossValue")->setVisible(false);
    resultLayer->getChildByName("lblTime")->setVisible(false);
    resultLayer->getChildByName("lblTimeValue")->setVisible(false);
    //resultLayer->getChildByName("lblTotalScore")->setVisible(false);
    //resultLayer->getChildByName("lblTotalScoreValue")->setVisible(false);
    resultLayer->getChildByName("lblOk")->setVisible(false);
    ((Text*)resultLayer->getChildByName("lblOk"))->addClickEventListener(CC_CALLBACK_0(HudLayer::tryResultOK, GameManager::getInstance()->getHudLayer()));
    this->resumeLayer();
    this->schedule(schedule_selector(HelloWorld::updateResult), 0.05, CC_REPEAT_FOREVER, 1);
}
void HelloWorld::updateResult(float dt){
    if (resultDone) {
        this->unschedule(schedule_selector(HelloWorld::updateResult));
        return;
    }
    
    Text* lblExpValue = (Text*)resultLayer->getChildByName("lblExpValue");
    Text* lblKill = (Text*)resultLayer->getChildByName("lblKill");
    Text* lblKillValue = (Text*)resultLayer->getChildByName("lblKillValue");
    lblKill->setVisible(true);
    lblKillValue->setVisible(true);
    if (kill < GameManager::getInstance()->totalKill) {
        kill++;
        lblKillValue->setString(Value(kill).asString());
        GameManager::getInstance()->playSoundEffect(SOUND_MISOL);
        totalScore += 1;
        lblExpValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblDestroy = (Text*)resultLayer->getChildByName("lblDestroy");
    Text* lblDestroyValue = (Text*)resultLayer->getChildByName("lblDestroyValue");
    lblDestroy->setVisible(true);
    lblDestroyValue->setVisible(true);
    if (destroy < GameManager::getInstance()->totalDestroy) {
        destroy++;
        lblDestroyValue->setString(Value(destroy).asString());
        GameManager::getInstance()->playSoundEffect(SOUND_MISOL);
//        totalScore += 10;
//        lblExpValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblCoin = (Text*)resultLayer->getChildByName("lblCoin");
    Text* lblCoinValue = (Text*)resultLayer->getChildByName("lblCoinValue");
    lblCoin->setVisible(true);
    lblCoinValue->setVisible(true);
    if (coin < GameManager::getInstance()->totalCoin) {
        int coinAmountToAdd = 1;
        if (coin + 10 < GameManager::getInstance()->totalCoin) {
            coinAmountToAdd = 10;
        }else if (coin + 100 < GameManager::getInstance()->totalCoin) {
            coinAmountToAdd = 100;
        }
        coin += coinAmountToAdd;
        lblCoinValue->setString(Value(coin).asString());
        GameManager::getInstance()->playSoundEffect(SOUND_MISOL);
//        totalScore += 10*coinAmountToAdd;
//        lbllblTotalScoreValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblRoom = (Text*)resultLayer->getChildByName("lblRoom");
    Text* lblRoomValue = (Text*)resultLayer->getChildByName("lblRoomValue");
    lblRoom->setVisible(true);
    lblRoomValue->setVisible(true);
    if (room < GameManager::getInstance()->totalRoom) {
        room++;
        lblRoomValue->setString(Value(room).asString());
        GameManager::getInstance()->playSoundEffect(SOUND_MISOL);
        totalScore += 5;
        lblExpValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblBoss = (Text*)resultLayer->getChildByName("lblBoss");
    Text* lblBossValue = (Text*)resultLayer->getChildByName("lblBossValue");
    lblBoss->setVisible(true);
    lblBossValue->setVisible(true);
    if (boss < GameManager::getInstance()->totalBoss) {
        boss++;
        lblBossValue->setString(Value(boss).asString());
        GameManager::getInstance()->playSoundEffect(SOUND_MISOL);
        totalScore += 20;
        lblExpValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblTime = (Text*)resultLayer->getChildByName("lblTime");
    Text* lblTimeValue = (Text*)resultLayer->getChildByName("lblTimeValue");
    lblTime->setVisible(true);
    lblTimeValue->setVisible(true);
    if (!GameManager::getInstance()->isGameClear) {
        GameManager::getInstance()->totalTime = 0;
    }
    if (time < GameManager::getInstance()->totalTime) {
        time+=60;
        if (time > GameManager::getInstance()->totalTime) {
            time = GameManager::getInstance()->totalTime;
        }
        lblTimeValue->setString(String::createWithFormat("%2d:%2d", (60*60 - time)/60, (60*60 - time)%60)->getCString());
        GameManager::getInstance()->playSoundEffect(SOUND_MISOL);
//        totalScore += 600;
//        lbllblTotalScoreValue->setString(Value(totalScore).asString());
        return;
    }
    if (!resultDone) {
        GM->addGameExp(totalScore);
        SM->saveUserData(strmake("level=%d&exp=%d", GM->getGameLevel(), GM->getCurrentExp()));
    }
    Text* lblOk = (Text*)resultLayer->getChildByName("lblOk");
    resultDone = true;
    lblOk->setVisible(true);
    
    Sprite* cursor = Sprite::create("targetWhite.png");
    resultLayer->addChild(cursor, 99990);
    cursor->runAction(RepeatForever::create(RotateBy::create(1, 100)));
    cursor->setPosition(lblOk->getPosition() + Point(-20, 0));
    
    Sprite* innerCursor = Sprite::create("targetWhite.png");
    cursor->addChild(innerCursor, 99990);
    innerCursor->runAction(RepeatForever::create(RotateBy::create(1, -160)));
    innerCursor->setScale(0.7);
    innerCursor->setPosition(Point(cursor->getContentSize().width/2, cursor->getContentSize().height/2));
}
void HelloWorld::gameOver()
{
    pauseLayer();
    endGame();
    char stageBuf[20];
    sprintf(stageBuf, "%d-%d", GameManager::getInstance()->theme, GameManager::getInstance()->currentStageIndex);
    
    this->unschedule(schedule_selector(HelloWorld::showSuccessLayer));
    
    
    deadPoint = player->getPosition();
    isGameOver = true;
    
//    GameManager::getInstance()->showInterstitialAds();
    
    if (skyBuddy && skyBuddy->petNumber == PET_ANGEL_DRAGON) {
        MoveBy* moveUp = MoveBy::create(0.25f, Point(0, 40));
        player->runAction(Sequence::create(EaseIn::create(moveUp, 0.3f), NULL));
        
        Sprite* spt = Sprite::create("backLight.png");
        spt->setScale(0.1);
        spt->runAction(ScaleTo::create(0.4, 0.5));
        spt->runAction(Sequence::create(RotateBy::create(1.5, 300), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        spt->setPosition(skyBuddy->getPosition());
        this->addChild(spt, spriteBatch->getLocalZOrder()-1);

        spt = Sprite::create("backLight.png");
        spt->setScale(0.1);
        spt->runAction(ScaleTo::create(0.4, 0.5));
        spt->runAction(Sequence::create(RotateBy::create(1.5, -300), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        spt->setPosition(skyBuddy->getPosition());
        this->addChild(spt, spriteBatch->getLocalZOrder()-1);
        
        spt =  Sprite::createWithSpriteFrameName("angelDragon.png");
        spriteBatch->addChild(spt);
        spt->runAction(FadeOut::create(0.5));
        spt->runAction(ScaleTo::create(0.5, 10));
        spt->setPosition(skyBuddy->getPosition());
        
        this->resume();
        this->scheduleOnce(schedule_selector(HelloWorld::angelReviveLater), 1.5);
        return;
    }
    for (auto drop: enemyMissileArray) {
        drop->removeFromParentAndCleanup(true);
    }
    enemyMissileArray.clear();
    
    player->stopFootStep();
    GameManager::getInstance()->playSoundEffect(SOUND_GIGGLING);
    
    MoveBy* moveUp = MoveBy::create(0.25f, Point(0, 40));
    MoveBy* moveDown = MoveBy::create(0.5f, Point(0, -200));
    player->runAction(Sequence::create(EaseIn::create(moveUp, 0.3f), EaseOut::create(moveDown, 0.3f), NULL));
    GameManager::getInstance()->setCoin(0);
    return;
    CallFunc* call = CallFunc::create(CC_CALLBACK_0(HelloWorld::showGameOverLayer, this));
    DelayTime* time = DelayTime::create(1.6f);
    this->runAction(Sequence::create(time, call, NULL));
}

void HelloWorld::showGameOverLayer()
{
    GameManager::getInstance()->getHudLayer()->showGameContinueLayer();
}

void HelloWorld::missileUpdate(float dt)
{
    if (isGameOver) {
        return;
    }

    Rect mRect;
//    enemiesToRemove.clear();

    Vector<Missile*> missileToRemove;
    EnemyBase* drop;
    for (auto missile: heroMissileArray) {
        mRect = Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
                            missile->getPosition().y - (missile->getContentSize().height*missile->getScaleY()/2),
                            missile->getContentSize().width*missile->getScaleX(),
                            missile->getContentSize().height*missile->getScaleY());
        for (int i = 0; i<enemyArray.size(); i++) {
            drop = enemyArray.at(i);
            
            if(missile->isPiercing){
                if(((Missile*)missile)->victimArray.contains(drop)){
                    continue;
                }
            }
            
            if (drop->untouchable ||
                drop->getPosition().y-missile->getPosition().y > 50 ||
                drop->getPosition().y - missile->getPosition().y < -50) {
                continue;
            }
            
            if (drop->damageBoundingBox().intersectsRect(mRect)) {
                Point pos = drop->getPosition();
                if(!drop->untouchable){
                    if (!drop->immortal) {
                        float damage = missile->energy + missile->energy*extraPower/100.0f;
                        log("damage: %f, missile->energy: %f", damage, missile->energy);
                        if (rand()%1000 < (missile->criticalPossibility + extraCritical + GM->getResearchCriticalPossibility())*10) {//critical
                            damage*=GM->getResearchCriticalPower()*0.01f;
                            Sprite* critical = Sprite::createWithSpriteFrameName("critical.png");
                            spriteBatch->addChild(critical, 100);
                            critical->getTexture()->setAliasTexParameters();
                            //                            critical->setColor(aColor3B(0,0,0));
                            critical->setScale(0.5f);
                            critical->setPosition(drop->getPosition() + Point(0, 15));
                            critical->runAction(EaseIn::create(MoveBy::create(1, Point(0, 25)), 0.3));
                            critical->runAction(Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
                            
                            shakeScreen(1);
                        }
                        if(player->getTag() == CHARACTER_PAUL){
                            damage += 10;
                            
                        }
                        if (damage >= 10) {
                            damage = damage*0.8f + (rand()%((int)(damage*0.4f)));
                        }
                        if(player->getTag() == CHARACTER_JOHN){
                            Droppable* coin;
                            coin = Droppable::createDroppable(UNIT_COIN, 1, 0, "silverCoin0.png");
                            coin->runAnimation("silverCoin", true);
                            
                            coin->velocity = Point(400 - rand()%800, 50 + rand()%200);
                            coinArray.pushBack(coin);
                            spriteBatch->addChild(coin);
                            droppableCoinArray.pushBack(coin);
                            coin->setPosition(drop->getPosition());
                            coin->runAction(Sequence::create(DelayTime::create(10), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::coinWaitDone, this)), NULL));
                        }
//                        demage = 1000000; // test
                        drop->energy -= (int)damage;
                        drop->onHit();
                        if(drop->getTag() != UNIT_DRUM_BOMB){
                            drop->isTargetFound = true;
                            if(player->getTag() == CHARACTER_NINJA){
                                drop->removeChildByTag(CHARACTER_NINJA);
                            }
                        }
                        drop->showEnergy();
                        
                        totalPower += damage;
                        if (drop == theBoss) {
                            GameManager::getInstance()->getHudLayer()->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
                        }else{
                            if(droppableArray.find(drop) != droppableArray.end()){
                                drop->desiredPosition = drop->desiredPosition + Point(0, 2);
                                drop->setPosition(drop->getPosition() + Point(0, 2));
                            }
                        }
                        
                        showDamage(damage, drop->getPosition());
                        if (drop->getTag() == UNIT_DUMMY) {
                            for (int i = rand()%3 + drop->energy <= 0?5:0; i >= 0; i--) {
                                Sprite* sptStraw = Sprite::createWithSpriteFrameName("straw.png");
                                sptStraw->setPosition(drop->getPosition());
                                spriteBatch->addChild(sptStraw);
                                float dur = 0.3f + (rand()%10)*0.03f;
                                sptStraw->setRotation(rand()%180);
                                sptStraw->runAction(RotateTo::create(dur, 0));
                                sptStraw->runAction(Sequence::create(MoveBy::create(dur, Point(rand()%20 - 10, 6 + rand()%6)), MoveBy::create(dur, Point(5, -5)), MoveBy::create(dur, Point(-10, -5)), MoveBy::create(dur, Point(10, -5)), MoveBy::create(dur, Point(-10, -5)), NULL));
                                sptStraw->runAction(Sequence::create(DelayTime::create(dur), RotateTo::create(dur, -30), RotateTo::create(dur, 30), RotateTo::create(dur, -30), RotateTo::create(dur, 30), RotateTo::create(dur, 30), NULL));
                                sptStraw->runAction(Sequence::create(DelayTime::create(dur*3), FadeOut::create(dur*2), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptStraw)), NULL));
                            }
                            
                            drop->setRotation(drop->getPosition().x > missile->getPosition().x?20:-20);
                            drop->runAction(EaseElasticOut::create(RotateTo::create(1, 0), 0.5));
                        }
                        
                        if(player->getTag() == CHARACTER_PAUL){
                            Label* lblPlusDemage = Label::createWithBMFont("DemageNumber.fnt", Value(10).asString());
                            this->addChild(lblPlusDemage, 100);
                            lblPlusDemage->setScale(0.4);
                            lblPlusDemage->setRotation(30);
//                            lblPlusDemage->getTexture()->setAliasTexParameters();
                            lblPlusDemage->setPosition(drop->getPosition() + Point(13, 14));
                            lblPlusDemage->runAction(EaseIn::create(MoveBy::create(1, Point(0, 25)), 0.3));
                            lblPlusDemage->runAction(Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
                        }
                        if (drop->moveTypeAfterFindHero != drop->currentMoveType) {
                            //                    drop->doActionAfterFindTarget();
                        }
                        
                        if (drop->energy <= 0) {
                            enemiesToRemove.pushBack(drop);
                            this->runEffect(EFFECT_EXPLODE_BIG, drop->getPosition());
                            drop->onDead();
                            this->createRewards(drop);
                            if (drop->rewards > 0) {
                                enemyKilledCount++;
                                GM->totalKill++;
                            }
                        }
                        else
                        {
                            drop->blinkForAWhile();
                        }
                        
                        if(missile->secondTag == WEAPON_FIRE_BOTTLER ||
                           missile->secondTag == WEAPON_RIFLE_GRENADE ||
                           missile->secondTag == WEAPON_BAZOOKA ||
                           missile->secondTag == WEAPON_ROCKET_LAUNCHER ||
                           missile->secondTag == WEAPON_GUIDED_ROCKET_LAUNCHER){
                            splashDemage(missile->getPosition(), 40, damage/2);
                            CCLOG("in missile updat");
                        }
                    }
                    if(!missile->isPiercing){
                        missileToRemove.pushBack(missile);
                    }else{
                        ((Missile*)missile)->victimArray.pushBack(drop);
                    }
                    if(player->getTag() == CHARACTER_PAUL){
                        this->runEffect(EFFECT_EXPLODE_BIG, pos);
                        this->runEffect(EFFECT_EXPLODE_HUGE, pos);
                        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
                    }else{
                        this->runEffect(EFFECT_EXPLODE_SMALL, pos);
                        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
                    }
                    break;
                }
            }
        }
    }

    int killCount = 0;
    for (auto spt: enemiesToRemove){
        if (spt == theBoss) {
            keyCollected = true;
            createBossRewards();
            
            HUD->setDailyMissionState(DAILY_MISSION_DEFEAT_BAZOOKA, MISSION_STATE_CLEAR);
            this->schedule(schedule_selector(HelloWorld::bossExplode), 0.3, 20, 1);
            enemyArray.eraseObject(spt);
            theBoss->unscheduleAllCallbacks();
            theBoss->stopAllActions();
            droppableArray.eraseObject(theBoss);
            theBoss->hideEnergy();
            theBoss->setColor(Color3B::RED);
            theBoss->runAction(Sequence::create(DelayTime::create(3), FadeOut::create(2), NULL));
            this->scheduleOnce(schedule_selector(HelloWorld::makeGate), 6);
            GameManager::getInstance()->totalBoss++;
            GameManager::getInstance()->totalScore+=10000;
            GameManager::getInstance()->getHudLayer()->setScore(GameManager::getInstance()->totalScore);
            
            log("boss killed: %d", stageIndex);
            int achieve = UDGetInt(strmake(KEY_ACHIEVE_FORMAT, stageIndex).c_str(), 0);
            achieve++;
            UDSetInt(strmake(KEY_ACHIEVE_FORMAT, stageIndex).c_str(), achieve);
            int level = UDGetInt(strmake(KEY_ACHIEVE_LEVEL_FORMAT, stageIndex).c_str(), 0);
            SM->saveUserData(strmake("achieve%d=%d", stageIndex, achieve + level*1000));
            continue;
        }
        removeEnemy(spt);
    }

    for (auto spt: missileToRemove){
        spt->stopAllActions();
        heroMissileArray.eraseObject(spt);
        if(spt->getTag() == UNIT_MISSILE_CHASING){
            chasingMissileArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_DROPPABLE){
            droppableMissileArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_CUSTOM){
            customMovingArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_STRAIGHT){
            straightMovingArray.eraseObject(spt);
        }
        spt->removeFromParentAndCleanup(true);
    }
    enemiesToRemove.clear();
}
void HelloWorld::makeGate(float dt){
    NPCBase* gate = NPCBase::create("activeZone0.png");
    spriteBatch->addChild(gate);
    npcArray.pushBack(gate);
    gate->runAnimation("warp", true);
    gate->setTag(UNIT_GATE);
    gate->setPosition(playerSpawnPoint);
    gate->setScale(0.1);
    gate->runAction(ScaleTo::create(0.3, 1));
    if (stageIndex == 3) {
        int clearCount = UserDefault::getInstance()->getIntegerForKey(KEY_CLEAR_COUNT, 0);
        clearCount++;
        UserDefault::getInstance()->setIntegerForKey(KEY_CLEAR_COUNT, clearCount);
        GameManager::getInstance()->isGameClear = true;
        gate->secondTag = STAGE_LOBBY;
        
        // test
//        GameManager::getInstance()->isGameClear = true;
//        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 0)->getCString(), true);
//        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 1)->getCString(), true);
//        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 2)->getCString(), true);
//        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString(), true);
        
        if (GameManager::getInstance()->collectedJewelIndex >= 0) {
            UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, GameManager::getInstance()->collectedJewelIndex)->getCString(), true);
        }
        showResult();
    }else{
        gate->secondTag = stageIndex + 1;
    }
}
void HelloWorld::bossExplode(float dt){
    if (rand()%2 == 0) {
        runEffect(EFFECT_EXPLODE_HUGE, theBoss->getPosition() + Point(rand()%20 - 10, rand()%20 - 30));
    }else{
        runEffect(EFFECT_BOMB_WALL, theBoss->getPosition() + Point(rand()%20 - 10, rand()%30 - 30));
    }
}
void HelloWorld::shakeScreenOnce(){
    shakeScreen(2);
}
void HelloWorld::shakeScreen(int count){
    Vector<FiniteTimeAction*> array;
    for (int i = 0; i < count; i++) {
        array.pushBack(CallFunc::create(CC_CALLBACK_0(HelloWorld::shakeScreenFirst, this)));
        array.pushBack(DelayTime::create(0.03));
        array.pushBack(CallFunc::create(CC_CALLBACK_0(HelloWorld::shakeScreenSecond, this)));
        array.pushBack(DelayTime::create(0.05));
        array.pushBack(CallFunc::create(CC_CALLBACK_0(HelloWorld::shakeScreenEnd, this)));
        array.pushBack(DelayTime::create(0.03));
    }
    this->runAction(Sequence::create(array));
}
void HelloWorld::shakeScreenFirst(){
    extraCameraPos = Point((rand()%10)*0.1, (rand()%10)*0.1);
}
void HelloWorld::shakeScreenSecond(){
    extraCameraPos = Point(-(rand()%10)*0.1, -(rand()%10)*0.1);
}
void HelloWorld::shakeScreenEnd(){
    extraCameraPos = Point::ZERO;
}
void HelloWorld::removeEnemy(EnemyBase* spt){
    Droppable* drop;
    for (int i = 0; i < chasingMissileArray.size(); i++){
        drop = (Droppable*)chasingMissileArray.at(i);
        if (drop->target == spt) {
            drop->target = NULL;
        }
    }
    
    if(spt->getTag() == UNIT_FLYING_PENGUIN){
        WalkingPenguin* enemy = WalkingPenguin::create(UNIT_WALKING_PENGUIN, 42 + GameManager::getInstance()->currentStageIndex*(GameManager::getInstance()->theme+1), 5, "walkingPenguin0.png", 4, 10, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_ON_A_PLATFORM, 100);
        enemy->setTarget(player);
        spriteBatch->addChild(enemy);
        enemy->setPosition(spt->getPosition());
        enemyArray.pushBack(enemy);
        droppableArray.pushBack(enemy);
    }
    removedEnemyList.push_back(spt->unitType);
    
    spt->stopAllActions();
    chasingMissileArray.eraseObject(spt);
    straightMovingArray.eraseObject(spt);
    enemyArray.eraseObject(spt);
    destructableArray.eraseObject(spt);
    droppableArray.eraseObject(spt);
    flyingSwimingEnemyArray.eraseObject(spt);
    chasingMissileArray.eraseObject(spt);
    inWaterArray.eraseObject(spt);
    if(fireableArray.find((FireableBase*)spt) == fireableArray.end()){
        fireableArray.eraseObject((FireableBase*)spt);
    }
    
    spiderArray.eraseObject(spt);
    spt->removeFromParentAndCleanup(true);
    bool isAllClear = true;
    Rect rect = getTileMap(player->getPosition())->getBoundingBox();
    for(auto enemy : enemyArray){
        if (rect.containsPoint(enemy->getPosition()) && enemy->secondTag != UNIT_TRAP_KIND_OF) {
            isAllClear = false;
            break;
        }
    }
    if (isAllClear && enemiesToLoad.size() == 0) {
        if (GM->currentStageIndex <= STAGE_DAILY_DUNGEON_0 && GM->currentStageIndex >= STAGE_DAILY_DUNGEON_3) {
            HUD->showDailyDungeonResult();
            return;
        }else{
            TMXTiledMap* tileMap = getTileMap(player->getPosition());
            GameManager::getInstance()->playSoundEffect(SOUND_STONE_ROLL);
            //dnMiniMap->getParent()->runAction(MoveBy::create(0.5, miniMapPos));
            isMiniMapFadeIn = true;
            ScrollView* scrollView = (ScrollView*)dnMiniMap->getParent()->getParent();
            scrollView->setBackGroundColorOpacity(100);
            this->schedule(schedule_selector(HelloWorld::fadeMiniMap));
            
            // unlock doors
            for (int i = 1; i < tileMap->getMapSize().height; i++) {    // left
                if(tileMap->getLayer("stage")->getTileGIDAt(Point(1, i)) == 322){
                    tileMap->getLayer("stage")->setTileGID(0, Point(1, i));
                    addDirtToTile(tileMap, Point(1, i));
                }
            }
            for (int i = 1; i < tileMap->getMapSize().width; i++) {    // bottom
                if(tileMap->getLayer("stage")->getTileGIDAt(Point(i, 1)) == 322){
                    tileMap->getLayer("stage")->setTileGID(0, Point(i, 1));
                    addDirtToTile(tileMap, Point(i, 1));
                }
            }
            for (int i = 1; i < tileMap->getMapSize().height; i++) {    // right
                if(tileMap->getLayer("stage")->getTileGIDAt(Point(tileMap->getMapSize().width - 2, i)) == 322){
                    tileMap->getLayer("stage")->setTileGID(0, Point(tileMap->getMapSize().width - 2, i));
                    addDirtToTile(tileMap, Point(tileMap->getMapSize().width - 2, i));
                }
            }
            for (int i = 1; i < tileMap->getMapSize().width; i++) {    // top
                if(tileMap->getLayer("stage")->getTileGIDAt(Point(i, tileMap->getMapSize().height - 2)) == 322){
                    tileMap->getLayer("stage")->setTileGID(0, Point(i, tileMap->getMapSize().height - 2));
                    addDirtToTile(tileMap, Point(i, tileMap->getMapSize().height - 2));
                }
            }
            updateMiniMap();
        }
    }
}

void HelloWorld::fixStageLayerFourTiles(TMXTiledMap* map){
    TMXLayer* stageLayer = map->getLayer("stage");
    float y;
    for (int x = 2; x < map->getMapSize().width - 2; x++) {
        for (int rawY = 2; rawY < map->getMapSize().height - 2; rawY++) {
            y = map->getMapSize().height - rawY - 1;
            if (isBlock(stageLayer->getTileGIDAt(Point(x, y)))) {
                if (compareFourTiles(0, 1, 1, 1, x, y, stageLayer)) {// top
                    stageLayer->setTileGID(6, Point(x, y));
                }else if (compareFourTiles(1, 0, 1, 1, x, y, stageLayer)) { // left
                    stageLayer->setTileGID(37, Point(x, y));
                }else if (compareFourTiles(1, 1, 0, 1, x, y, stageLayer)) { // right
                    stageLayer->setTileGID(39, Point(x, y));
                }else if (compareFourTiles(1, 1, 1, 0, x, y, stageLayer)) { // bottom
                    stageLayer->setTileGID(70, Point(x, y));
                }else if (compareFourTiles(0, 0, 1, 1, x, y, stageLayer)) { // left top
                    stageLayer->setTileGID(5, Point(x, y));
                }else if (compareFourTiles(0, 1, 0, 1, x, y, stageLayer)) { // right top
                    stageLayer->setTileGID(7, Point(x, y));
                }else if (compareFourTiles(1, 0, 1, 0, x, y, stageLayer)) { // left bottom
                    stageLayer->setTileGID(69, Point(x, y));
                }else if (compareFourTiles(1, 1, 0, 0, x, y, stageLayer)) { // right bottom
                    stageLayer->setTileGID(71, Point(x, y));
                }else if (compareFourTiles(1, 1, 1, 1, x, y, stageLayer)) {
                    stageLayer->setTileGID(38, Point(x, y));
                }
            }
        }
    }
}

bool HelloWorld::compareFourTiles(int t,int l, int r, int b, int x, int y, TMXLayer* stageLayer){
    bool result = true;
    if (isBlock(stageLayer->getTileGIDAt(Point(x, y - 1))) != t) {
        return false;
    }else if (isBlock(stageLayer->getTileGIDAt(Point(x - 1, y))) != l) {
        return false;
    }else if (isBlock(stageLayer->getTileGIDAt(Point(x + 1, y))) != r) {
        return false;
    }else if (isBlock(stageLayer->getTileGIDAt(Point(x, y + 1))) != b) {
        return false;
    }
    return result;
}

void HelloWorld::fixStageLayerTiles(TMXTiledMap* map){
    TMXLayer* stageLayer = map->getLayer("stage");
    float y;
    for (int x = 1; x < map->getMapSize().width - 1; x++) {
        for (int rawY = 1; rawY < map->getMapSize().height - 1; rawY++) {
            y = map->getMapSize().height - rawY - 1;
            if (isBlock(stageLayer->getTileGIDAt(Point(x, y)))) {
                if (compareNineTiles(1, 1, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // center
                    stageLayer->setTileGID(38, Point(x, y));
                }else if (compareNineTiles(0, 0, 0, 1, 0, 1, 1, 0, x, y, stageLayer)) { // right top corner
                    stageLayer->setTileGID(7, Point(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 0, 1, 1, 0, x, y, stageLayer)) { // right corner
                    stageLayer->setTileGID(39, Point(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 0, 0, 0, 0, x, y, stageLayer)) { // right bottom corner
                    stageLayer->setTileGID(71, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 0, 0, 0, x, y, stageLayer)) { // bottom corner
                    stageLayer->setTileGID(70, Point(x, y));
                }else if (compareNineTiles(0, 1, 1, 0, 1, 0, 0, 0, x, y, stageLayer)) { // left bottom corner
                    stageLayer->setTileGID(69, Point(x, y));
                }else if (compareNineTiles(0, 1, 1, 0, 1, 0, 1, 1, x, y, stageLayer)) { // left corner
                    stageLayer->setTileGID(37, Point(x, y));
                }else if (compareNineTiles(0, 0, 0, 0, 1, 0, 1, 1, x, y, stageLayer)) { // left top corner
                    stageLayer->setTileGID(5, Point(x, y));
                }else if (compareNineTiles(0, 0, 0, 1, 1, 1, 1, 1, x, y, stageLayer)) { // top corner
                    stageLayer->setTileGID(6, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 0, 1, 1, x, y, stageLayer)) { // right top wall
                    stageLayer->setTileGID(15, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 0, 1, 1, 1, 1, x, y, stageLayer)) { // right wall
                    stageLayer->setTileGID(47, Point(x, y));
                }else if (compareNineTiles(0, 1, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // right bottom wall
                    stageLayer->setTileGID(79, Point(x, y));
                }else if (compareNineTiles(1, 0, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // bottom wall
                    stageLayer->setTileGID(78, Point(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 1, 1, 1, 1, x, y, stageLayer)) { // left bottom wall
                    stageLayer->setTileGID(77, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 0, 1, 1, 1, x, y, stageLayer)) { // left wall
                    stageLayer->setTileGID(45, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 1, 1, 0, x, y, stageLayer)) { // left top wall
                    stageLayer->setTileGID(13, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 1, 0, 1, x, y, stageLayer)) { // top wall
                    stageLayer->setTileGID(14, Point(x, y));
                }else if (compareNineTiles(0, 1, 1, 0, 1, 1, 1, 1, x, y, stageLayer)) { // right wall top oepn
                    stageLayer->setTileGID(47, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 0, 1, 0, 1, 1, x, y, stageLayer)) { // right wall bottom open
                    stageLayer->setTileGID(47, Point(x, y));
                }else if (compareNineTiles(0, 0, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // bottom wall left open
                    stageLayer->setTileGID(78, Point(x, y));
                }else if (compareNineTiles(1, 0, 0, 1, 1, 1, 1, 1, x, y, stageLayer)) { // bottom wall right open
                    stageLayer->setTileGID(78, Point(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 0, 1, 1, 1, x, y, stageLayer)) { // left wall top open
                    stageLayer->setTileGID(45, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 0, 1, 1, 0, x, y, stageLayer)) { // left wall bottom open
                    stageLayer->setTileGID(45, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 0, 0, 1, x, y, stageLayer)) { // top wall left open
                    stageLayer->setTileGID(14, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 1, 0, 0, x, y, stageLayer)) { // top wall right open
                    stageLayer->setTileGID(14, Point(x, y));
                }
            }
        }
    }
}
bool HelloWorld::compareNineTiles(int lt, int t, int rt, int l, int r, int lb, int b, int rb, int x, int y, TMXLayer* stageLayer){
    bool result = true;
    if (isBlock(stageLayer->getTileGIDAt(Point(x - 1, y - 1)) != lt)) {
        return false;
    }else if (isBlock(stageLayer->getTileGIDAt(Point(x, y - 1))) != t) {
        return false;
    }else if (isBlock(stageLayer->getTileGIDAt(Point(x + 1, y - 1))) != rt) {
        return false;
    }else if (isBlock(stageLayer->getTileGIDAt(Point(x - 1, y))) != l) {
        return false;
    }else if (isBlock(stageLayer->getTileGIDAt(Point(x + 1, y))) != r) {
        return false;
    }else if (isBlock(stageLayer->getTileGIDAt(Point(x - 1, y + 1))) != lb) {
        return false;
    }else if (isBlock(stageLayer->getTileGIDAt(Point(x, y + 1))) != b) {
        return false;
    }else if (isBlock(stageLayer->getTileGIDAt(Point(x + 1, y + 1))) != rb) {
        return false;
    }
    return result;
}
void HelloWorld::createRewards(EnemyBase* enemy){
    if (enemy->rewards <= 0 || enemy->rewards > 100000) {
        return;
    }
    Droppable* coin;
    for (int i = 0; i < enemy->rewards; i++) {
        if(rand()%101 < 3 + extraFruit){
            const char* name;
            int food = rand()%12;
            switch (food) {
                case FOOD_APPLE:
                    name = "apple.png";
                    break;
                case FOOD_GREEN_APPLE :
                    name = "greemApple.png";
                    break;
                case FOOD_PINE_APPLE :
                    name = "pineApple.png";
                    break;
                case FOOD_PEAR :
                    name = "pear.png";
                    break;
                case FOOD_BANANA :
                    name = "banana.png";
                    break;
                case FOOD_STRAWBERRY :
                    name = "strawberry.png";
                    break;
                case FOOD_CHERRY :
                    name = "cherry.png";
                    break;
                case FOOD_CARROT :
                    name = "carrot.png";
                    break;
                case FOOD_MUSHROOM :
                    name = "mushroom.png";
                    break;
                case FOOD_CORN :
                    name = "corn.png";
                    break;
                case FOOD_WATERMELON :
                    name = "waterMelon.png";
                    break;
                case FOOD_FISH :
                    name = "fish.png";
                    break;
            }
            coin = Droppable::createDroppable(UNIT_FOOD, 1, 0, name);
            coin->secondTag = food;
        }else if(rand()%101 < enemy->goldPossiblity){
            coin = Droppable::createDroppable(UNIT_GOLD_COIN, 1, 0, "goldCoin0.png");
            coin->runAnimation("goldCoin", true);
        }else{
            coin = Droppable::createDroppable(UNIT_COIN, 1, 0, "silverCoin0.png");
            coin->runAnimation("silverCoin", true);
        }
        coin->setScale(0.8);
        coin->velocity = Point(400 - rand()%800, 50 + rand()%200);
        coinArray.pushBack(coin);
        spriteBatch->addChild(coin, 2);
        droppableCoinArray.pushBack(coin);
        coin->setPosition(enemy->getPosition());
        coin->runAction(Sequence::create(DelayTime::create(8), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::coinWaitDone, this)), NULL));
    }
}
Sprite* HelloWorld::getLightSpin(float persistTime){
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
int HelloWorld::getSlotForWeapon(int weapon){
    int index = 0;
    while(true){
        if (getWeaponAtSlot(index) == weapon) {
            return index;
        }else if(getWeaponAtSlot(index) == -1){
            return -1;
        }
        index++;
    }
}

Sprite* HelloWorld::createNumberSprite(int number)
{
    //	int place = 1;
    int digit = 0;
    Vector<Sprite*> sprites;
    if (number < 0) return NULL;
    char buf[6];
    while (true){
        digit = number % 10;
        sprintf(buf, "%d.png", digit);
        Sprite* sptNumber = Sprite::createWithSpriteFrameName(buf);
        sprites.insert(0, sptNumber);
        number /= 10;
        if (0 >= number){
            break;
        }
    }
    int startWidth = 0;
    int index = 0;
    Sprite* first;
    for (auto sprite : sprites){
        if (index == 0) {
            index++;
            startWidth = sprite->getContentSize().width;
            first = sprite;
            continue;
        }
        sprite->setPosition(Point(startWidth + sprite->getContentSize().width/2, sprite->getContentSize().height/2));
        startWidth += (sprite->getContentSize().width - 1);
        first->addChild(sprite);
    }
    sprites.clear();
    return first;
}

void HelloWorld::onjewelryBoxOpen(Ref* sender){
    GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("jewel collected").c_str(), GameManager::getInstance()->getHudLayer());
    Droppable* jewelryBox = (Droppable*)sender;
    Sprite* lightSpin = getLightSpin(5);
    GameManager::getInstance()->playSoundEffect(SOUND_DDALKACK);
    
    Sprite* sptWeapon = Sprite::createWithSpriteFrameName(__String::createWithFormat("jewel%d.png", jewelryBox->secondTag)->getCString());
    sptWeapon->setOpacity(0);
    lightSpin->setOpacity(0);
    sptWeapon->setPosition(jewelryBox->getPosition());
    lightSpin->setPosition(jewelryBox->getPosition());
    sptWeapon->runAction(FadeIn::create(0.3));
    lightSpin->runAction(FadeIn::create(0.3));
    sptWeapon->runAction(Sequence::create(MoveBy::create(1, Point(0, 18)), DelayTime::create(4), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptWeapon)), nullptr));
    lightSpin->runAction(MoveBy::create(1, Point(0, 18)));
    spriteBatch->addChild(sptWeapon, 1);
    this->addChild(lightSpin);
    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, jewelryBox->secondTag)->getCString(), true);
    //player->freezed = false;
    
    GameManager::getInstance()->collectedJewelIndex = jewelryBox->secondTag;
}
void HelloWorld::openWeapon(Ref* sender){
    Droppable* suitcase = (Droppable*)sender;
    Sprite* lightSpin = getLightSpin(5);
    GameManager::getInstance()->playSoundEffect(SOUND_DDALKACK);
    int var = rand()%(WEAPON_NO_MORE/4);
    if (suitcase->secondTag == 3) {
        var++;
    }
    int weapon = 1 + (WEAPON_NO_MORE/4)*suitcase->secondTag + var;
    if (weapon >= WEAPON_NO_MORE) {
        weapon = WEAPON_NO_MORE - 1;
    }else if (weapon < 1) {
        weapon = 1;
    }
    
    //int weapon = WEAPON_LIGHTNING_GUN; test
    Sprite* sptWeapon = Sprite::createWithSpriteFrameName(GameManager::getInstance()->getWeaponImageFileName(weapon));
    sptWeapon->setScale(0.5f);
    sptWeapon->setOpacity(0);
    lightSpin->setOpacity(0);
    sptWeapon->setPosition(suitcase->getPosition());
    lightSpin->setPosition(suitcase->getPosition());
    sptWeapon->runAction(FadeIn::create(0.3));
    lightSpin->runAction(FadeIn::create(0.3));
    sptWeapon->runAction(Sequence::create(MoveBy::create(1, Point(0, 18)), DelayTime::create(4), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptWeapon)), nullptr));
    lightSpin->runAction(MoveBy::create(1, Point(0, 18)));
    spriteBatch->addChild(sptWeapon, 1);
    this->addChild(lightSpin);
    
    player->freezed = false;
    if (getSlotForWeapon(weapon) < 0) {
        addWeapon(weapon);
        GM->addBagContent(weapon, 0);
        GM->setItemCollectionFound(weapon%1000, COLLECTION_FOUND);
        GM->saveBagContent();
    }else{
        recharge(getSlotForWeapon(weapon));
    }
}
void HelloWorld::recharge(int slot){
    int weapon = getWeaponAtSlot(slot);
    if (currentSlot == slot) {
        totalBulletCount = getMaxTotalBulletCount(weapon);
        loadedBulletCount = getMaxLoadedBulletCount(weapon);
    }else{
        setTotalBulletCountAtSlot(slot, getMaxTotalBulletCount(weapon));
        setLoadedBulletCountAtSlot(slot, getMaxLoadedBulletCount(weapon));
    }
    
    changeWeapon(slot);
}
void HelloWorld::addWeapon(int weapon, int level){
    int index = 0;
    while(getWeaponAtSlot(index) >= 0){
        index++;
    }
    setWeaponAtSlot(index, weapon);
    setWeaponAtSlot(index+1, -1);
    setLoadedBulletCountAtSlot(index, getMaxLoadedBulletCount(weapon));
    setTotalBulletCountAtSlot(index, getMaxTotalBulletCount(weapon));
    changeWeapon(index);
//    GM->lastWeaponSlot = index;
}
int HelloWorld::getTotalBulletCountAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), 0);
}
void HelloWorld::setTotalBulletCountAtSlot(int slot, int count){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), count);
}
int HelloWorld::getLoadedBulletCountAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), 0);
}
void HelloWorld::setLoadedBulletCountAtSlot(int slot, int count){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), count);
}

void HelloWorld::changeWeapon(int slot){
    if (isReloading) {
        return;
    }
    isFired = true;
    if (getWeaponAtSlot(slot) < 0) {
        return;
    }
    if (totalBulletCount >= 0) {
        setTotalBulletCountAtSlot(currentSlot, totalBulletCount);
    }
    if (loadedBulletCount >= 0) {
        setLoadedBulletCountAtSlot(currentSlot, loadedBulletCount);
    }
    
    currentSlot = slot;
    int weapon = getWeaponAtSlot(slot);
    int level = 0;
    int value = UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, slot).c_str(), -1);
    if(slot < 4 && value > 0){
        level = value/1000;
    }
    log("weapon level: %d", level);
    
    setWeaponInfo(weapon, level);
    totalBulletCountMax = getMaxTotalBulletCount(weapon);
    totalBulletCount = getTotalBulletCountAtSlot(slot);
    loadedBulletCount = getLoadedBulletCountAtSlot(slot);
    loadedBulletCountMax = getMaxLoadedBulletCount(weapon);
    GameManager::getInstance()->getHudLayer()->setWeapon(weapon);
    GameManager::getInstance()->getHudLayer()->setBulletCount(loadedBulletCount,loadedBulletCountMax);
    GameManager::getInstance()->getHudLayer()->setTotalBulletCount(totalBulletCount, totalBulletCountMax);
}
void HelloWorld::removeLaser(){
    if (laser!=nullptr) {
        laser->removeFromParent();
        laser = nullptr;
        GameManager::getInstance()->stopSoundEffect(SOUND_LASER_BUZZ);
    }
}
int HelloWorld::getWeaponAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat("WeaponInSlot%d", slot)->getCString(), -1);
}
void HelloWorld::setWeaponAtSlot(int slot, int weapon){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat("WeaponInSlot%d", slot)->getCString(), weapon);
}
Sprite* HelloWorld::getShining(float delayTime){
    Sprite* shining = Sprite::createWithSpriteFrameName("shining0.png");
    Animation* animation = AnimationCache::getInstance()->getAnimation("shining");
    Animate* animate = Animate::create(animation);
    shining->runAction(Sequence::create(DelayTime::create(delayTime), MoveBy::create(0.7, Point(6, 6)), nullptr));
    shining->runAction(Sequence::create(DelayTime::create(delayTime), RotateBy::create(0.7, 250), nullptr));
    shining->runAction(Sequence::create(DelayTime::create(delayTime), animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, shining)), NULL));
    
    return shining;
}
void HelloWorld::npcUpdate(float dt){
    if (!player->onGround) {
        if (player->sptTalkBalloon != nullptr) {
            GameManager::getInstance()->getHudLayer()->lblTouch->stopAllActions();
            GameManager::getInstance()->getHudLayer()->lblTouch->setOpacity(0);
            player->sptTalkBalloon->setVisible(false);
        }
        return;
    }
    Rect projectileRect;
    bool hideTalk = true;
    for (auto npc: npcArray){
        projectileRect = npc->getBoundingBox();
        if (npc->getBoundingBox().intersectsRect(player->getBoundingBox())) {
            hideTalk = false;
            if (npc == player->encounteredNpc) {
                if (npc == lastTalkedNpc) {
                    hideTalk = true;
                }
                break;
            }
            if (player->encounteredNpc == nullptr && lastTalkedNpc == npc) {
                break;
            }
            Rect rect = intersection(npc->getBoundingBox(), player->getBoundingBox());
            if (npc->getTag() == UNIT_NPC || npc->getTag() == UNIT_THE_KNIGHT || npc->getTag() == UNIT_HOOD_GHOST) {
                player->setFeelingVisible(true, FEELING_TALK);
                GameManager::getInstance()->getHudLayer()->lblTouch->runAction(RepeatForever::create(Sequence::create(FadeIn::create(0.1), DelayTime::create(0.2f), FadeOut::create(0.2f), DelayTime::create(0.1f), NULL)));
            }else if(npc->getTag() == UNIT_GATE && rect.size.height > 5){
                player->setFeelingVisible(true, FEELING_EXCLAIM);
                if (stageIndex == -1 && npc->getPosition().x == 528) {
                    if (!UserDefault::getInstance()->getBoolForKey(KEY_IS_TUTORIAL_CLEAR, false)) {
                        for(auto knight : npcArray){
                            if (knight->getTag() == UNIT_THE_KNIGHT) {
                                ((TheKnight*)knight)->doKnightAction(KNIGHT_SWING_LEFT_BOTTOM);
                            }
                        }
                    }
                }
            }else if(npc->getTag() == UNIT_ONSALE){
                
            }else{
                continue;
            }
            
            if (npc != lastTalkedNpc) {
                if(player->encounteredNpc != npc){
                    if(ndBuyButton != nullptr){
                        ndBuyButton->runAction(Sequence::create(MoveBy::create(0.3f, Vec2(0, -300)), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, ndBuyButton)), nullptr));
                        ndBuyButton = nullptr;
                    }
                }
                player->encounteredNpc = npc;
                if(player->encounteredNpc->getTag() == UNIT_ONSALE){
                    if(ndBuyButton == nullptr){
                        ndBuyButton = CSLoader::createNode("BuyButton.csb");
                        HUD->addChild(ndBuyButton);
                        ndBuyButton->setPosition(Vec2(size.width/2, -500));
                        ndBuyButton->runAction(MoveBy::create(0.3f, Vec2(0, 560)));
                        Button* btn = (Button*)ndBuyButton->getChildByName("btn");
                        btn->addClickEventListener(CC_CALLBACK_0(HelloWorld::onBuyShopItem, this));
                        Text* lbl = (Text*)btn->getChildByName("lbl");
                        LM->setLocalizedString(lbl, "buy");
                    }
                }
                if (player->encounteredNpc->getTag() == UNIT_GATE) {
                    takeAction();
                }
            }
            
            break;
        }
    }
    if (hideTalk) {
        if (player->encounteredNpc != nullptr) {
            player->setFeelingVisible(false, FEELING_EXCLAIM);
            GameManager::getInstance()->getHudLayer()->lblTouch->stopAllActions();
            GameManager::getInstance()->getHudLayer()->lblTouch->setOpacity(0);
//            if (player->encounteredNpc != nullptr) {
//                if (player->encounteredNpc->getTag() == UNIT_GATE && stageIndex == -1 && player->encounteredNpc->getPosition().x == 528 ) {
                    for(auto knight : npcArray){
                        if (knight->getTag() == UNIT_THE_KNIGHT) {
                            if (((TheKnight*)knight)->state == KNIGHT_SWING_LEFT_BOTTOM) {
                                ((TheKnight*)knight)->doKnightAction(KNIGHT_SWING_BACK_FROM_LEFT_BOTTOM);
                            }
                        }
                    }
//                }
//            }
            player->encounteredNpc = nullptr;
            if(ndBuyButton != nullptr){
                ndBuyButton->runAction(Sequence::create(MoveBy::create(0.3f, Vec2(0, -300)), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, ndBuyButton)), nullptr));
                ndBuyButton = nullptr;
            }
            
        }else{
            lastTalkedNpc = nullptr;
        }
    }
}
void HelloWorld::onBuyShopItem(){
    if(player->encounteredNpc->getTag() == UNIT_ONSALE){
        OnSale* onSale = (OnSale*)player->encounteredNpc;
        onSale->buyItem();
    }
}
void HelloWorld::coinUpdate(float dt){
    if (isGameOver) {
        return;
    }

    Rect projectileRect;

    coinToRemove.clear();
    Point thisPos, spritePos;
    
    for (auto drop: coinArray){
        
        if (drop->getPosition().y-player->getPosition().y > 50 || drop->getPosition().y - player->getPosition().y < -50) {
            continue;
        }
        
//        projectileRect = Rect(drop->getPosition().x - (drop->getContentSize().width*drop->getScaleX()/2),
//                                    drop->getPosition().y - (drop->getContentSize().height*drop->getScaleY()/2),
//                                    drop->getContentSize().width*drop->getScaleX(),
//                                    drop->getContentSize().height*drop->getScaleY());
        int coin;
        if (drop->getBoundingBox().intersectsRect(player->collectBoundingBox())) {
            switch (drop->getTag()) {
                case UNIT_COIN:
                    coin = 1;
                    if (UserDefault::getInstance()->getBoolForKey(KEY_REMOVE_ADS_GET, false)) {
                        coin *= 2;
                    }
                    collectedCoinCount += coin;
                    HUD->updateCoin(collectedCoinCount);
                    if(droppableCoinArray.getIndex(drop) >= 0)
                        droppableCoinArray.eraseObject(drop);
                    showCoinCount(drop->getPosition(), coin);
                    GameManager::getInstance()->setCoin(collectedCoinCount);
                    GameManager::getInstance()->getHudLayer()->updateCoin(collectedCoinCount);
                    GameManager::getInstance()->totalCoin += coin;
                    GameManager::getInstance()->totalScore+=coin*10;
                    GameManager::getInstance()->getHudLayer()->setScore(GameManager::getInstance()->totalScore);
                    break;
                case UNIT_COIN_PART:
                    collectedCoinPart += 1;
                    if(droppableCoinArray.getIndex(drop) >= 0)
                        droppableCoinArray.eraseObject(drop);
                    if (collectedCoinPart > 4) {
                        collectedCoinPart -= 5;
                        coin = 1;
                        if (UserDefault::getInstance()->getBoolForKey(KEY_REMOVE_ADS_GET, false)) {
                            coin *= 2;
                        }
                        collectedCoinCount += coin;
                        showCoinCount(drop->getPosition(), coin);
                    }
                    GameManager::getInstance()->setCoin(collectedCoinCount);
                    GameManager::getInstance()->getHudLayer()->updateCoin(collectedCoinCount);
                    break;
                case UNIT_GOLD_COIN:
                    coin = 5;
                    if (UserDefault::getInstance()->getBoolForKey(KEY_REMOVE_ADS_GET, false)) {
                        coin *= 2;
                    }
                    collectedCoinCount += coin;
                    if(droppableCoinArray.getIndex(drop) >= 0)
                        droppableCoinArray.eraseObject(drop);
                    showCoinCount(drop->getPosition(), coin);
                    GameManager::getInstance()->setCoin(collectedCoinCount);
                    GameManager::getInstance()->getHudLayer()->updateCoin(collectedCoinCount);
                    GameManager::getInstance()->totalCoin += coin;
                    GameManager::getInstance()->totalScore+=coin*10;
                    GameManager::getInstance()->getHudLayer()->setScore(GameManager::getInstance()->totalScore);
                    break;
                case UNIT_FOOD:
                    GameManager::getInstance()->playSoundEffect(SOUND_COIN);
                    if(droppableCoinArray.getIndex(drop) >= 0)
                        droppableCoinArray.eraseObject(drop);
                    GameManager::getInstance()->addFoodCount((drop)->secondTag, 1);
                    break;
                case UNIT_MEDI_KIT:
                    player->energy = player->maxEnergy;
                    GameManager::getInstance()->getHudLayer()->setLife(player->maxEnergy);
                    // heal effect
                    GameManager::getInstance()->playSoundEffect(SOUND_MEDICINE);
                    break;
                case UNIT_KEY:
                    GameManager::getInstance()->collectedKeyCount++;
                    GameManager::getInstance()->getHudLayer()->keyCollected(GameManager::getInstance()->collectedKeyCount);
                    GameManager::getInstance()->playSoundEffect(SOUND_MEDICINE);
                    break;
                case UNIT_SUITCASE:
                case UNIT_JEWELRY_BOX:
                    if (GameManager::getInstance()->collectedKeyCount > 0) {
                        GameManager::getInstance()->collectedKeyCount--;
                        GameManager::getInstance()->getHudLayer()->keyCollected(GameManager::getInstance()->collectedKeyCount);
                        drop->stopAllActions();
                        player->stopAndStand();
//                        player->face->setVisible(true);
                        GameManager::getInstance()->playSoundEffect(SOUND_POWER_UP_45);
                        if (drop->getTag() == UNIT_SUITCASE) {
                            player->freezed = true;
                            Animation* animation = AnimationCache::getInstance()->getAnimation(__String::createWithFormat("suitcaseOpen%d", drop->secondTag)->getCString());
                            Animate* animate = Animate::create(animation);
                            drop->runAction(Sequence::create(DelayTime::create(1), animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::openWeapon, this)),NULL));
                            drop->setSpriteFrame(__String::createWithFormat("suitCaseUnlocked%d.png", drop->secondTag)->getCString());
                            
                        }else if(drop->getTag() == UNIT_JEWELRY_BOX) {
                            Animation* animation = AnimationCache::getInstance()->getAnimation("jewelryBoxOpen");
                            Animate* animate = Animate::create(animation);
                            drop->runAction(Sequence::create(DelayTime::create(1), animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::onjewelryBoxOpen, this)),NULL));
                            drop->secondTag = stageIndex;
                            GameManager::getInstance()->jewelryCollected = true;
                        }
                        drop->runAction(EaseOut::create(MoveBy::create(1, Point(0, 10)), 3));
                        coinArray.eraseObject(drop);
                        Sprite* shining = getShining(1);
                        this->addChild(shining, 30);
                        shining->setPosition(drop->getPosition());
                        shining->runAction(EaseOut::create(MoveBy::create(1, Point(0, 10)), 3));
                    }
                    continue;
                    break;
                case UNIT_STAR:
                    starCount++;
                    thisPos = this->getPosition();
                    spritePos = drop->getPosition();
                    
                    GameManager::getInstance()->getHudLayer()->starCollected(starCount);
                    GameManager::getInstance()->playSoundEffect(SOUND_COIN);

                    
                    indicatedArray.eraseObject(drop);
                    break;
                default:
                    break;
            }
            
            GameManager::getInstance()->playSoundEffect(SOUND_COIN);
            this->runEffect(EFFECT_TWINKLE, drop->getPosition());
            coinToRemove.pushBack(drop);
        }
    }
    

    for (auto drop: coinToRemove){
        drop->stopAllActions();
        coinArray.eraseObject(drop);
        spriteBatch->removeChild(drop, true);
        starArray.eraseObject(drop);
    }
    
//    coinToRemove.clear();


    for (auto drop: helperArray){
//    for (int i = 0; i<helperArray.size(); i++) {
//        sprite = (Sprite*)helperArray.at(i);
        
        if (drop->getPosition().y - player->getPosition().y > 50 || drop->getPosition().y - player->getPosition().y < -50) {
            continue;
        }
        
        projectileRect = Rect(drop->getPosition().x - (drop->getContentSize().width*drop->getScale()/2),
                                    drop->getPosition().y - (drop->getContentSize().height*drop->getScale()/2),
                                    drop->getContentSize().width*drop->getScale(),
                                    drop->getContentSize().height*drop->getScale());
        if (projectileRect.intersectsRect(player->collisionBoundingBox()) && player->velocity.y < -0.1 && !player->exitingVehicle) {
            switch (drop->getTag()) {
                case UNIT_SPRING_THEME0:
                {
                    this->schedule(schedule_selector(HelloWorld::jumpBySpring), 0.05f, 4, 0);
                    Animation* animation = AnimationCache::getInstance()->getAnimation("theme0Spring");
                    Animate* animate = Animate::create(animation);
                    drop->runAction(animate);
                    GameManager::getInstance()->playSoundEffect(SOUND_SPRING);
                    break;
                }
                case UNIT_SPRING_THEME2:
                {
                    this->schedule(schedule_selector(HelloWorld::jumpBySpring), 0.05f, 4, 0);
                    Animation* animation = AnimationCache::getInstance()->getAnimation("theme2Spring");
                    Animate* animate = Animate::create(animation);
                    drop->runAction(animate);
                    GameManager::getInstance()->playSoundEffect(SOUND_SPRING);
                    break;
                }
                case UNIT_BROOM:
                {
                    if(player->vehicleType == VEHICLE_NONE){
                        spriteBatch->removeChild(drop, true);
                        Sprite* board = Sprite::createWithSpriteFrameName("rideBroom.png");
                        board->setPosition(Point(5, 0));
                        board->setTag(UNIT_BROOM);
                        
                        player->getInTheVehicle(VEHICLE_BROOM, board);
                        setPlayerInfo();
                        helperArray.eraseObject(drop);
                        player->addChild(board, 100);
                    }
                    
                    break;
                }case UNIT_CART:{
                    if(player->vehicleType == VEHICLE_NONE){
                        spriteBatch->removeChild(drop, true);
                        Sprite* board = Sprite::createWithSpriteFrameName("rideCart.png");
                        board->setPosition(Point(6, 6));
                        board->setTag(UNIT_CART);
                        
                        player->getInTheVehicle(VEHICLE_CART, board);
                        setPlayerInfo();
                        helperArray.eraseObject(drop);
                        player->addChild(board, 100);
                    }
                    
                    break;
                }case UNIT_TOP:{
                    if(player->vehicleType == VEHICLE_NONE){
                        player->desiredPosition = drop->getPosition();
                        player->setPosition(drop->getPosition());
                        spriteBatch->removeChild(drop, true);
                        Sprite* board = Sprite::createWithSpriteFrameName("rideTop.png");
                        board->setPosition(Point(5, -5));
                        board->setTag(UNIT_TOP);
                        board->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.01), ScaleTo::create(0, 1, 1), DelayTime::create(0.01), ScaleTo::create(0, -1, 1), NULL)));
                        player->getInTheVehicle(VEHICLE_TOP, board);
                        setPlayerInfo();
                        helperArray.eraseObject(drop);
                        player->addChild(board, 100);
                    }

                    break;
                }case UNIT_BIRD:{
                    if(player->vehicleType == VEHICLE_NONE){
                        spriteBatch->removeChild(drop, true);
                        Sprite* board = Sprite::createWithSpriteFrameName("rideBird.png");
                        board->setPosition(Point(5, 32));
                        board->setTag(UNIT_BIRD);
                        player->getInTheVehicle(VEHICLE_BIRD, board);
                        setPlayerInfo();
                        helperArray.eraseObject(drop);
                        player->addChild(board, 100);
                    }
                    
                    break;
                }case UNIT_BALLOON:{
                    if(player->vehicleType == VEHICLE_NONE){
                        spriteBatch->removeChild(drop, true);
                        Sprite* board = Sprite::createWithSpriteFrameName("rideBalloon.png");
                        board->setPosition(Point(5, 35));
                        board->setTag(UNIT_BALLOON);
                        player->getInTheVehicle(VEHICLE_BALLOON, board);
                        setPlayerInfo();
                        helperArray.eraseObject(drop);
                        player->addChild(board, -1);
                    }
                    
                    break;
                }
            }
//            this->runEffect(EFFECT_TWINKLE, sprite->getPosition());
            GameManager::getInstance()->playSoundEffect(SOUND_COIN);
        }
    }

    if (GameManager::getInstance()->theme != 3) {
        bool teleporting = false;
        for (auto tele: teleportArray) {
            projectileRect = RectInset(tele->getBoundingBox(), 4, 4);
            Rect playerRect = player->collectBoundingBox();
            if (projectileRect.intersectsRect(playerRect)) {
                if(!isTeleporting ){
                    switch (tele->getTag()) {
                        case TAG_TELEPORT_0_ENTRY:
                        positionToTeleport = (getTeleport(TAG_TELEPORT_0_EXIT)->getPosition() + Point(0, -8));
                        break;
                        case TAG_TELEPORT_0_EXIT:
                        positionToTeleport = (getTeleport(TAG_TELEPORT_0_ENTRY)->getPosition() + Point(0, -8));
                        break;
                        case TAG_TELEPORT_1_ENTRY:
                        positionToTeleport = (getTeleport(TAG_TELEPORT_1_EXIT)->getPosition() + Point(0, -8));
                        break;
                        case TAG_TELEPORT_1_EXIT:
                        positionToTeleport = (getTeleport(TAG_TELEPORT_1_ENTRY)->getPosition() + Point(0, -8));
                        break;
                        case TAG_TELEPORT_2_ENTRY:
                        positionToTeleport = (getTeleport(TAG_TELEPORT_2_EXIT)->getPosition() + Point(0, -8));
                        break;
                        case TAG_TELEPORT_2_EXIT:
                        positionToTeleport = (getTeleport(TAG_TELEPORT_2_ENTRY)->getPosition() + Point(0, -8));
                        break;
                        default:
                        break;
                    }
                    
                    float duration = 1;
                    indestructableTime =  1.8f;
                    this->schedule(schedule_selector(HelloWorld::immortalForAWhile), 0.1);
                    
                    playerIgnoreGravity = true;
                    if (player->currentPlatform) {
                        player->currentPlatform->player = NULL;
                        player->currentPlatform = NULL;
                    }
                    
                    player->runAction(EaseIn::create(MoveTo::create(duration, tele->getPosition() + Vec2(0, 4)), 0.3));
                    player->runAction(RepeatForever::create(RotateBy::create(0.5, 360)));
                    player->runAction(ScaleTo::create(duration, 0.1));
                    this->scheduleOnce(schedule_selector(HelloWorld::teleportLater), duration);
                    player->setAnchorPoint(Point(0.5, 2));
                    isTeleporting = true;
                    
                    if(player->vehicleType != VEHICLE_NONE &&
                       player->vehicleType != VEHICLE_CART){
                        getOffVehicle();
                    }
                    GameManager::getInstance()->playSoundEffect(SOUND_POWER_UP_45);
                    
                    return;
                }
                teleporting = true;
            }
        }
        
        if (!teleporting && isTeleporting) {
            isTeleporting = false;
        }
    }
    
    bool doorTouch = false;
    for (auto info: triggerDoorArray){
        if (info->trigger->getPosition().y - player->getPosition().y > 50 || info->trigger->getPosition().y - player->getPosition().y < -50) {
            continue;
        }
        
        projectileRect = Rect(info->trigger->getPosition().x - (info->trigger->getContentSize().width*info->trigger->getScale()/2),
                                    info->trigger->getPosition().y - (info->trigger->getContentSize().height*info->trigger->getScale()/2),
                                    info->trigger->getContentSize().width*info->trigger->getScale(),
                                    info->trigger->getContentSize().height*info->trigger->getScale());
        if (projectileRect.intersectsRect(player->collisionBoundingBox())) {
            doorTouch = true;
            if (!info->alreadyTriggered) {
                if (info->isOpen) { // To close
                    info->closeDoor();
                    
                    for (int i = 0; i < info->pointArray->count(); i++) {
//                        stageLayer->setTileGID(8, info->pointArray->getControlPointAtIndex(i));
                    }
                }else{  // To open
                    info->openDoor();
                    
                    for (int i = 0; i < info->pointArray->count(); i++) {
//                        stageLayer->removeTileAt(info->pointArray->getControlPointAtIndex(i));
                    }
                }
                
                info->alreadyTriggered = true;
                
                runEffect(EFFECT_TWINKLE, (info->trigger->getPosition()));
                GameManager::getInstance()->playSoundEffect(SOUND_DDALKACK);
                break;
            }
        }
    }

    if (!doorTouch) {
        for (auto info: triggerDoorArray){
            if(info->alreadyTriggered)
            {
                info->alreadyTriggered = false;
            }
        }
    }
}

void HelloWorld::teleportLaterForThemeThree(float dt){
    isTeleporting = false;
    playerIgnoreGravity = false;
    player->setScale(1);
    player->stopAllActions();
    player->setRotation(0);
    player->onGround = false;
    player->velocity = Point::ZERO;
    player->desiredPosition = player->getPosition();
    spriteBatch->setLocalZOrder(3);
}
void HelloWorld::teleportLater(float dt){
    player->setAnchorPoint(Point(0.5, 0.5));
    if(GameManager::getInstance()->theme == 3){
        player->setPosition(positionToTeleport);
        Point pos = player->getPosition() + Point(0, TILE_SIZE*2);
        TMXTiledMap* map = getTileMap(pos);
        Point plPos = this->tileCoordForPosition(pos - map->getPosition(), map);
        int tgid = map->getLayer("stageLayer")->getTileGIDAt(plPos);
        bool isTopEmpty = false;
        if(tgid > 0){
            Value property = map->getPropertiesForGID(tgid);
            if(property.getType() != Value::Type::NONE && property.asValueMap().at("Type").asString().compare("OneWay") != 0){

            }else{
                isTopEmpty = true;
            }
        }else{
            isTopEmpty = true;
        }
        
        float dur = 1.0f;
        int dist = 30;
        player->runAction(MoveBy::create(dur, Point(0, isTopEmpty?dist:-dist)));
        this->scheduleOnce(schedule_selector(HelloWorld::teleportLaterForThemeThree), dur);
    }else{
        playerIgnoreGravity = false;
        player->setScale(1);
        player->stopAllActions();
        player->setPosition(positionToTeleport);
        player->setRotation(0);
        player->onGround = false;
        player->velocity = Point::ZERO;
    }
}
void HelloWorld::showCoinCount(Point pos, int count){
    return;
    lblCoinGot->setPosition(pos);
    lblCoinGot->setOpacity(255);
    lblCoinGot->setString(Value(count).asString());
    Action* action = Sequence::create(EaseOut::create(MoveBy::create(1, Point(0, 20)), 8), FadeOut::create(1), nullptr);
    action->setTag(ACTION_TAG_ANIMATION);
    lblCoinGot->stopActionByTag(ACTION_TAG_ANIMATION);
    lblCoinGot->runAction(action);
//    lblCoinGot->runAction(Sequence::create(DelayTime::create(1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::nodeMoveDone, this)), NULL));
//    lbl->getTexture()->setAliasTexParameters();
}
void HelloWorld::jumpBySpring(float dt)
{
    player->velocity.y = 500;
}
void HelloWorld::runEffect(int effect, Point point, float angle){
    float duration = 0.2;
    float baseDuration = 0.08;
    duration = baseDuration*2 + baseDuration*(rand()%3 + 1);
    if (effect == EFFECT_EXPLODE_BIG) {
        for (int i = 0; i < 4; i++) {
            Sprite* particle = Sprite::createWithSpriteFrameName(__String::createWithFormat("dirt%d.png", rand()%3)->getCString());
            spriteBatch->addChild(particle);
            int angle = rand()%360;
            float x = cos(angle*3.14/180)*10;
            float y = sin(angle*3.14/180)*10;
            particle->setScale(0.2);
            particle->setPosition(point);
            particle->runAction(EaseOut::create(RotateBy::create(baseDuration*6, 100 + rand()%200), 2));
            particle->runAction(Sequence::create(EaseOut::create(MoveBy::create(baseDuration*2, Point(x, y)), 2), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, particle)), NULL));
        }
    }else if(effect == EFFECT_BULLET_WALL_SMALL){
        Sprite* explode = Sprite::createWithSpriteFrameName("bulletSmallEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        explode->setRotation(angle);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bulletSmallEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        explode->setRotation(angle);
        if (angle < -90 || angle > 90) {
            explode->setFlippedY(true);
        }
        GameManager::getInstance()->playSoundEffect(SOUND_FIRE_SMALL);
    }else if(effect == EFFECT_BULLET_WALL){
        Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        GameManager::getInstance()->playSoundEffect(SOUND_FIRE_SMALL);
    }else if(effect == EFFECT_BOMB_WALL){
        Sprite* explode = Sprite::createWithSpriteFrameName("bombEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bombEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
    }else if(effect == EFFECT_EXPLODE_SMALL){
//        float scale = 0.5f;
        duration = 0.15;
        Sprite* explode = Sprite::createWithSpriteFrameName("bombSmallEffect0.png");
        spriteBatch->addChild(explode);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bombSmallEffect");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
        explode->setPosition(point);
    }else if(effect == EFFECT_EXPLODE_MIDDLE){
        int particleCount = 3;
        int range = 30;
        float easeRate = 2;
        float scale = 1.2f;
        
        Sprite* explode = Sprite::createWithSpriteFrameName("whiteExplode.png");
        spriteBatch->addChild(explode);
        explode->setScale(scale + (rand()%4 + 1)*0.1f - 0.2f);
        explode->setColor(Color3B(237, 70, 37));
        explode->runAction(Sequence::create(DelayTime::create(duration*0.8f), TintTo::create(duration*0.4f, 255, 205, 52), TintTo::create(duration/2, 127, 110, 105), NULL));
        explode->runAction(Sequence::create(DelayTime::create(duration*1.3f), ScaleTo::create(duration, 0.1), NULL));
        explode->runAction(Sequence::create(DelayTime::create(duration*2), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        explode->setPosition(point);
        
        for (int i = 0; i < particleCount; i++) {
            Sprite* particle = Sprite::createWithSpriteFrameName("whiteExplode.png");
            spriteBatch->addChild(particle);
            int angle = rand()%360;
            float x = cos(angle*3.14/180)*range*1.1f;
            float y = sin(angle*3.14/180)*range*1.1f;
            particle->setScale(0.3);
            particle->setColor(Color3B(237, 70, 37));
            particle->runAction(FadeOut::create(baseDuration));
            particle->setPosition(point);
            particle->runAction(Sequence::create(EaseOut::create(MoveBy::create(baseDuration, Point(x, y)), easeRate), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        }
    }else if(effect == EFFECT_EXPLODE_HUGE){
        Sprite* explode = Sprite::createWithSpriteFrameName("bombEffect0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bombEffect");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
    }else if(effect == EFFECT_SMOKE){
        float duration = 0.5;
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteExplode.png");
        spriteBatch->addChild(spt, -1);
        spt->setPosition(point);
        spt->setScale(0.5);
        spt->setRotation(rand()%360);
        spt->setColor(Color3B(200,200,200));
        spt->runAction(Sequence::create(EaseIn::create(MoveBy::create(duration, Point(0, 7)), 2), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
        spt->runAction(EaseOut::create(MoveBy::create(duration, Point(player->isTowardLeft?-8:8, 0)), 4));
        spt->runAction(EaseIn::create(FadeOut::create(duration), 2));
        
        spt->runAction(RotateBy::create(duration, 160));
    }else if(effect == EFFECT_GREEN_SMOKE){
        float duration = 0.5;
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteExplode.png");
        spriteBatch->addChild(spt, -1);
        spt->setPosition(point);
        spt->setRotation(rand()%360);
        spt->setScale(0.3);
        spt->setColor(Color3B(130,250,130));
        spt->runAction(Sequence::create(EaseIn::create(ScaleTo::create(duration, 0.05), 2), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
        //        spt->runAction(EaseOut::create(MoveBy::create(duration, Point(player->isTowardLeft?-8:8, 0)), 4));
        spt->runAction(EaseIn::create(FadeOut::create(duration), 2));
        //        spt->runAction(RotateBy::create(duration, 160));
    }else if(effect == EFFECT_RED_SMOKE){
        float duration = 0.5;
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteExplode.png");
        spriteBatch->addChild(spt, -1);
        spt->setPosition(point);
        spt->setRotation(rand()%360);
        spt->setScale(0.5);
        spt->setColor(Color3B(237, 70, 37));
        spt->runAction(Sequence::create(EaseIn::create(MoveBy::create(duration, Point(0, 7)), 2), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
        spt->runAction(EaseOut::create(MoveBy::create(duration, Point(player->isTowardLeft?-8:8, 0)), 4));
        spt->runAction(EaseIn::create(FadeOut::create(duration), 2));
        spt->runAction(RotateBy::create(duration, 160));
    }else if(effect == EFFECT_TWINKLE){
        int initRange = 10;
        int range = initRange;
        baseDuration = 0.3f;
        float duration = 0;
        for (int i = 0; i < 15; i++) {
            Sprite* particle = Sprite::createWithSpriteFrameName("particle.png");
            spriteBatch->addChild(particle, player->getLocalZOrder() + 1);
            int angle = rand()%360;
            range = initRange + initRange*(rand()%50)*0.02;
            float x = cos(angle*3.14/180)*range*1.1f;
            float y = sin(angle*3.14/180)*range*1.1f;
            particle->setScale(0.5 + (rand()%10)*0.05);
            particle->setColor(Color3B::YELLOW);
            duration = baseDuration + baseDuration*(rand()%10)*0.1;
            particle->runAction(FadeOut::create(duration));
            particle->setPosition(point);
            particle->runAction(Sequence::create(EaseOut::create(MoveBy::create(duration, Point(x, y)), 1.6f), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        }
    }
}
void HelloWorld::runEffect(int effect, Point point)
{
    runEffect(effect, point, 0);
}
/*
void HelloWorld::addGlowEffect(Sprite* sprite,const Color3B& colour, const Size& size)
{
    Point pos = Point(sprite->getContentSize().width / 2,
                      sprite->getContentSize().height / 2);
    
    Sprite* glowSprite = Sprite::createWithSpriteFrameName("lizardStand.png");
    
    glowSprite->setColor(colour);
    glowSprite->setPosition(pos);
    glowSprite->setRotation(sprite->getRotation());
    
    ccBlendFunc f = {GL_ONE, GL_ONE};
    glowSprite->setBlendFunc(f);
    sprite->addChild(glowSprite, -1);
    
    // Run some animation which scales a bit the glow
    ScaleTo* scale1= ScaleTo::create(0.9f, 4);
    ScaleTo* scale2 = ScaleTo::create(0.9f, 3.75f);

    glowSprite->runAction(RepeatForever::create((ActionInterval*)Sequence::create(scale1, scale2, NULL)));
}*/
void HelloWorld::powerTestSchedule(float dt){
    powerTestTime += dt;
    if(powerTestTime > 10){
        powerTestTime = 0;
        totalPower = 0;
    }
    powerPerSec = totalPower/powerTestTime;
    CCLOG("power/sec: %f", powerPerSec);
}
Sprite* HelloWorld::getLight(){
    Sprite* sptLight = Sprite::create("whiteBigCircle.png");
    BlendFunc f = {GL_DST_COLOR, GL_ONE};
    sptLight->setBlendFunc(f);
    sptLight->setOpacity(255);
    sptLight->setColor(Color3B(255, 180, 0));
    sptLight->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.0, 0.9), DelayTime::create(0.3), ScaleTo::create(0.0, 1), DelayTime::create(0.3), nullptr)));
    return sptLight;
}

void HelloWorld::setLayerTag(TMXTiledMap* map){
    map->getLayer("unit")->setTag(TAG_UNIT_LAYER);
    map->getLayer("stage")->setTag(TAG_STAGE_LAYER);
    map->getLayer("back")->setTag(TAG_BACK_LAYER);
    map->getLayer("fore")->setTag(TAG_FORE_LAYER);
    map->getLayer("deco")->setTag(TAG_DECO_LAYER);
}
void HelloWorld::setBossMap(int stage){
    int tilesetIndex = rand()%3;
    tilesetIndex = 2;
    
    char buf[50];
    if(stage == 0){
        Director::getInstance()->tilesetName = "dungeonTileset.png";
        sprintf( buf, "stages/ldd_boss%d.tmx", 0); // start map
    }else if(stage == 1){
        Director::getInstance()->tilesetName = "stoneTileset.png";
        sprintf( buf, "stages/ldd_boss%d.tmx", 1); // start map
    }else if(stage == 2){
        Director::getInstance()->tilesetName = "brickTileset.png";
        sprintf( buf, "stages/ldd_boss%d.tmx", 2); // start map
    }else if(stage == 3){
        Director::getInstance()->tilesetName = "dungeonTileset.png";
        sprintf( buf, "stages/ldd_boss%d.tmx", 3); // start map
    }
    
//    GameManager::getInstance()->getHudLayer()->showStageTitle();
    stageIndex = stage;
    
    Rect rect;
    dnMiniMap = DrawNode::create();
    ScrollView* scrollView = ScrollView::create();
    GameManager::getInstance()->getHudLayer()->addChild(scrollView);
    //miniMapPos = Point(size.width - 10, size.height - 10);
    scrollView->setPosition(Point(size.width - 10, size.height - 10));
    scrollView->getInnerContainer()->addChild(dnMiniMap);
    scrollView->setAnchorPoint(Point(1, 1));
    scrollView->setContentSize(miniMapSize);
    
    TMXTiledMap* map = cocos2d::TMXTiledMap::create(buf);
    
    mapArray.pushBack(map);
    setLayerTag(map);
    this->addChild(map);
    mapRectArray.push_back(map->getBoundingBox());
    rect = map->getBoundingBox();
    map->setTag(1);
    setStage(map);
    initScene();
    
    if(stage == STAGE_LOBBY){
        setLobby(map);
    }
    
    if (stageIndex == 0) {
        this->initBoss(BOSS_BAZOOKA);
    }else if (stageIndex == 1) {
        this->initBoss(BOSS_MACHINE_GUN);
    }else if (stageIndex == 2) {
        this->initBoss(BOSS_GIANT);
    }else if (stageIndex == 3) {
        this->initBoss(BOSS_GIANT_HOOD);
    }
}
void HelloWorld::onMiniMapTouch(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_BEGAN == (TouchEventType)eEventType) {
        GameManager::getInstance()->getCurrentStageLayer()->showBigMiniMap(true);
    }else if(TouchEventType::TOUCH_EVENT_ENDED == (TouchEventType)eEventType) {
        GameManager::getInstance()->getCurrentStageLayer()->showBigMiniMap(false);
    }
}

void HelloWorld::setEntireMap(int stage){
    srand(GameManager::getInstance()->getTodaysRandom() + stage);
    int tilesetIndex = rand()%3;
    tilesetIndex = 2;
    if (stage < 1) {
        Director::getInstance()->tilesetName = "dungeonTileset.png";
    }else if(stage%3 == 1){
        Director::getInstance()->tilesetName = "brickTileset.png";
    }else if(stage%3 == 2){
        Director::getInstance()->tilesetName = "stoneTileset.png";
    }
//    Director::getInstance()->tilesetName = "stoneTileset.png"; // test 
    if (stage != STAGE_ENTRANCE) {
//        GameManager::getInstance()->getHudLayer()->showStageTitle();
    }
    
    stageIndex = stage;
    int stageNumber = 0;
    int mapCount = 5;
    int fineMapCount = 0;
    int weaponRoomCount = 1;
    //int blueKeyCount = stage < 0?0:1;
    int blueKeyCount = 0;
    bool isAdditionalMapNeeded = false;
    char buf[50];
    if (stage >= 0) {
        mapCount = 5;
        fineMapCount = 1;
        weaponRoomCount = 2;
        if (stage > 0) {
            mapCount = 4;
            fineMapCount = 2;
            weaponRoomCount = 3;
        }
        sprintf( buf, "stages/ldd_start%d.tmx", stageNumber); // start map
        isAdditionalMapNeeded = true;
    }else if(stage <= TUTORIAL_STAGE_END && stage >= TUTORIAL_STAGE_START ){
        mapCount = 0;
        sprintf( buf, "stages/ldd_tutorial%d.tmx", stage - TUTORIAL_STAGE_START);
    }else if(stage == -1){
        mapCount = 0;
        sprintf( buf, "stages/ldd_stageLobby.tmx");
    }else if(stage == STAGE_NAMING){
        mapCount = 0;
        sprintf( buf, "stages/ldd_naming.tmx");
    }else if(stage == STAGE_ENTRANCE){
        mapCount = 0;
        sprintf( buf, "stages/ldd_entrance.tmx");
    }else if(stage <= STAGE_DAILY_DUNGEON_0 && stage >= STAGE_DAILY_DUNGEON_3){
        mapCount = 0;
        sprintf( buf, "stages/ldd_daily.tmx");
    }
    // test
    /*int testIndex  = 0;
    stage = 0;
    mapCount = 0;
    fineMapCount = 1;
    stageIndex = 0;
    GameManager::getInstance()->currentStageIndex = TUTORIAL_STAGE_START + testIndex;
    sprintf( buf, "stages/ldd_start%d.tmx", testIndex);
    isAdditionalMapNeeded = true;*/
    // test done
    
    Rect rect;
    dnMiniMap = DrawNode::create();
    TMXTiledMap* map = cocos2d::TMXTiledMap::create(buf);
    
    if(stage == -1){
        setLobby(map);
    }
    mapArray.pushBack(map);
    setLayerTag(map);
    this->addChild(map);
    mapRectArray.push_back(map->getBoundingBox());
    rect = map->getBoundingBox();
    map->setTag(1);
    ScrollView* scrollView = ScrollView::create();
    GameManager::getInstance()->getHudLayer()->addChild(scrollView);
    scrollView->setPosition(Point(size.width - 10, size.height - 10));
    scrollView->setScrollBarEnabled(false);
    scrollView->getInnerContainer()->addChild(dnMiniMap);
    scrollView->setAnchorPoint(Point(1, 1));
    scrollView->setContentSize(miniMapSize);
    scrollView->setBackGroundColor(Color3B::BLACK);
    scrollView->setBackGroundColorOpacity(100);
    scrollView->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    scrollView->addTouchEventListener(CC_CALLBACK_2(HelloWorld::onMiniMapTouch, this));
    log("mini map pos: %f, %f", scrollView->getPositionX(), scrollView->getPositionY());
    setStage(map);
    
    initScene();
    
    DrawNode* dnHero = DrawNode::create();  // test
    scrollView->getInnerContainer()->addChild(dnHero);
    dnHero->setName("Hero");
    dnHero->setPosition(scrollView->getContentSize()/2);
    dnHero->setPosition(dnHero->getPosition() + Point(-TILE_SIZE*0.3f*0.5f, -TILE_SIZE*0.3f*0.5f));
    dnHero->drawSolidRect(Point::ZERO, Point(TILE_SIZE*0.3f, TILE_SIZE*0.3f), Color4F(1, 0.3, 0.3, 1));
    
    int counter = 0;
    int retryCounter = 0;
    int retryOutCount = 100;
    int mapIndexToTry = 0;
    TMXTiledMap* resultMap;
    currentLevelScore = stage*100;
    bool isNormalMap = true;
    int endMapCount = 1;
    
    int shopCount = 1;
    if (isAdditionalMapNeeded) {
        while(mapCount > 0 || weaponRoomCount > 0 || endMapCount > 0 || shopCount > 0 || fineMapCount > 0){
            //        sprintf( buf, "stages/ldd_stage%d.tmx", 2 + rand()%3);
            int tag = MAP_NORMAL;
            if(shopCount > 0 && mapCount <= 3){
                sprintf( buf, "stages/ldd_shop%d.tmx", 0);
                shopCount--;
                tag = MAP_SHOP;
                isNormalMap = false;
            }else if (mapCount > 0) {
                int whichMap = rand()%8;
                if (whichMap == 0) {
                    sprintf( buf, "stages/ldd_8x10.tmx");
                }else if (whichMap == 1) {
                    sprintf( buf, "stages/ldd_8x20.tmx");
                }else if (whichMap == 2) {
                    sprintf( buf, "stages/ldd_10x8.tmx");
                }else if (whichMap == 3) {
                    sprintf( buf, "stages/ldd_20x8.tmx");
                }else if (whichMap == 4) {
                    sprintf( buf, "stages/ldd_20x20.tmx");
                }else if (whichMap == 5) {
                    sprintf( buf, "stages/ldd_22x14.tmx");
                }else if (whichMap == 6) {
                    sprintf( buf, "stages/ldd_42x20.tmx");
                }else if (whichMap == 7) {
                    sprintf( buf, "stages/ldd_42x42.tmx");
                }
                if (stageIndex == 0 && (whichMap == 0 || whichMap == 2 || whichMap == 7)) {
                    continue;
                }
                mapCount--;
                isNormalMap = true;
            }else if(weaponRoomCount > 0){
                sprintf( buf, "stages/ldd_weapon_chest%d.tmx", 0);
                weaponRoomCount--;
                tag = MAP_WEAPON;
                isNormalMap = false;
            }else if(fineMapCount > 0){
                sprintf( buf, "stages/ldd_fine%d.tmx", 0);//rand()%6); // test
                fineMapCount--;
                tag = MAP_FINE;
                isNormalMap = false;
            }else if(endMapCount > 0){
                sprintf( buf, "stages/ldd_end%d.tmx", 0);
                endMapCount--;
                tag = MAP_BOSS_GATE;
                isNormalMap = false;
            }
            
            map = cocos2d::TMXTiledMap::create(buf);
            //Director::getInstance()->tilesetName = "";
            setLayerTag(map);
            if (blueKeyCount > 0 && (rand()%100 < 30 || mapCount <= blueKeyCount)) {
                tag = MAP_BLUE_KEY;
                blueKeyCount--;
            }
            map->setTag(tag);
            map->getLayer("unit")->setVisible(false);
            if (isNormalMap) {
                setEmptyMap(map);
            }
            
            map->getLayer("deco")->setVisible(false);
            map->getChildByTag(TAG_STAGE_LAYER)->setOpacity(100);
            map->getChildByTag(TAG_BACK_LAYER)->setOpacity(100);
            map->getChildByTag(TAG_DECO_LAYER)->setOpacity(100);
            while(true){
                mapIndexToTry = rand()%mapArray.size();
                resultMap = findDirectionAndPlaceTheMap(mapArray.at(mapIndexToTry), map);
                //        fixStageLayerFourTiles(map);
                if (resultMap == nullptr) {
                    log("failed to find direction");
                }else{
                    mapArray.pushBack(resultMap);
                    break;
                }
            }
            
            retryCounter++;
            if (retryCounter >= retryOutCount) {
                MessageBox("Can't believe! Failed to make rooms!", "OMG2");
                break;
            }
        }
    }
    
    
    for(auto theMap: mapArray){
        Sprite* darkness = Sprite::create("fiftyRect.png");
        this->addChild(darkness);
        darkness->setOpacity(180);
        darkness->setScale(theMap->getContentSize().width/darkness->getContentSize().width, theMap->getContentSize().height/darkness->getContentSize().height);
        darkness->setAnchorPoint(Point::ZERO);
        darkness->setPosition(theMap->getPosition());
    }
    
    updateMiniMap();
    log("** tried %d times to make the map", retryCounter);
}
void HelloWorld::initScene(){
    
    this->schedule(schedule_selector(HelloWorld::gravityUpdate));
    this->schedule(schedule_selector(HelloWorld::gravityUpdateForCoins));
    this->schedule(schedule_selector(HelloWorld::gravityUpdateForStraight));
    this->schedule(schedule_selector(HelloWorld::gravityUpdateForCustomMoving));
    this->schedule(schedule_selector(HelloWorld::gravityUpdateForFlyingOrSwimingEnemies));
    
    this->schedule(schedule_selector(HelloWorld::bubbleUpdate));
    this->schedule(schedule_selector(HelloWorld::makeSomeBubbles), 0.1f);
    
    /*
     skyBuddy = GameManager::getInstance()->getSkyPet();
     
     if (skyBuddy != NULL) {
     skyBuddy->setPosition(player->getPosition() + Point(10, 0));
     spriteBatch->addChild(skyBuddy, 9);
     skyBuddy->setFlippedX(false);
     buddyArray.pushBack(skyBuddy);
     }*/
    groundBuddy = GameManager::getInstance()->getGroundPet();
    extraCritical = 0;
    extraPower = 0;
    extraFruit = 0;
    if (groundBuddy != NULL) {
        groundBuddy->setPosition(player->getPosition() + Point(10, 0));
        spriteBatch->addChild(groundBuddy, 9);
        buddyArray.pushBack(groundBuddy);
        
        groundBuddy->master = player;
        droppableArray.pushBack(groundBuddy);
        
        if(groundBuddy->petNumber == PET_PLUTO){
            this->scheduleOnce(schedule_selector(HelloWorld::sacrificeLater), 1);
        }else if(groundBuddy->petNumber == PET_KUMURI){
            extraPower = 10;
        }else if(groundBuddy->petNumber == PET_LEXY){
            extraFruit = 2;
        }else if(groundBuddy->petNumber == PET_NEPTUN){
            extraCritical = 10;
        }else if(groundBuddy->petNumber == PET_OXTO){
            extraPower = 20;
        }else if(groundBuddy->petNumber == PET_RIUS){
            extraPower = 20;
        }
    }
    
    if (GameManager::getInstance()->isVideoRewardCoins) {
        Droppable* coin;
        Point coinPos;
        Point pos;
        int spread = 70;
        
        for (int i = 0; i < 40; i++) {
            coinPos = player->getPosition() + Point(spread - rand()%(spread*2), spread - rand()%(spread*2));
            TMXTiledMap* map = getTileMap(coinPos);
            TMXLayer* stageLayer = map->getLayer("stage");
            pos = this->tileCoordForPosition(coinPos - map->getPosition(), map);
            if(pos.x < stageLayer->getLayerSize().width && pos.y < stageLayer->getLayerSize().height && pos.x >=0 && pos.y >=0){
                
            }else{
                i--;
                continue;
            }
            
            int tgid = stageLayer->getTileGIDAt(pos);
            if(tgid > 0){
                i--;
                continue;
            }
            coin = Droppable::createDroppable(UNIT_COIN, 1, 0, "silverCoin0.png");
            coin->runAnimation("silverCoin", true);
            
            coinArray.pushBack(coin);
            spriteBatch->addChild(coin);
            coin->setPosition(coinPos);
            coin->runAction(Sequence::create(DelayTime::create(8), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::coinWaitDone, this)), NULL));
        }
    }
    
    isVideoRewardAttack = GameManager::getInstance()->isVideoRewardAttack;
    if (isVideoRewardAttack) {
        Sprite* particle;
        GameManager::getInstance()->playSoundEffect(SOUND_MEDICINE);
        for (int i = 0; i < 20; i++) {
            particle = Sprite::createWithSpriteFrameName("particle.png");
            spriteBatch->addChild(particle, player->getLocalZOrder()+1);
            particle->setScale(1 + (rand()%10)*0.05);
            //            particle->setColor(Color3B(28, 135, 171));
            particle->setPosition(player->getPosition() + Point(rand()%30 - 15, rand()%40 - 20));
            //            particle->runAction(FadeOut::create(0));
            float dur = 0.5;
            particle->runAction(RotateBy::create(dur, 360));
            particle->runAction(MoveBy::create(dur - (rand()*10)*0.1f*dur, Point(0, 30)));
            particle->runAction(Sequence::create(FadeIn::create(dur*3/10), DelayTime::create(dur*4/10), FadeOut::create(dur*3/10), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        }
    }
    
    GameManager::getInstance()->isVideoRewardEnergy = false;
    GameManager::getInstance()->isVideoRewardAttack = false;
    GameManager::getInstance()->isVideoRewardCoins = false;
    
    GameManager::getInstance()->canJumpForever = (skyBuddy != nullptr && skyBuddy->petNumber == PET_FALCON) || GameManager::getInstance()->developer;
    
}
void HelloWorld::tournamentSchedule(float dt){
    if (enemyArray.size() > 0) {
        return;
    }
    
    if (tournamentWaveIndex == 0) {
        
    }
}
void HelloWorld::addEnemiesToMap(TMXTiledMap* map, int levelScore, bool blueKey){
    Point pos;
    int enemyCountLeft = map->getMapSize().height*map->getMapSize().width/120;
    float dur = 2;
    if (enemyCountLeft <= 0) {
        enemyCountLeft = 1;
    }
    int tryCount = 0;
    int tryCountMax = 10;
    TMXLayer* stageLayer = map->getLayer("stage");
    while (enemyCountLeft > 0) {
        while(true){
            //pos = Point(3 + rand()%(int)(map->getMapSize().width - 6), map->getMapSize().height/2 + rand()%(int)(map->getMapSize().height/2) - 3);
            pos = Point(3 + rand()%(int)(map->getMapSize().width - 6), 4 + rand()%(int)(map->getMapSize().height - 6));
            if (!isBlock(map->getLayer("stage")->getTileGIDAt(pos))) {
                break;
            }
        }
        
        if (blueKey && (rand()%100 < 20 || enemyCountLeft == 1)) {
            Droppable* coinSprite = Droppable::createDroppable(UNIT_KEY, 1, 0, "blueKey.png");
            spriteBatch->addChild(coinSprite);
            coinArray.pushBack(coinSprite);
            coinSprite->setScale(0.8);
            GameManager::getInstance()->makeItSiluk(coinSprite);
            coinSprite->setPosition(map->getPosition() + Point(pos.x*TILE_SIZE, (map->getMapSize().height - pos.y)*TILE_SIZE));
            blueKey = false;
        }
        tryCount = 0;
        tryCountMax = 10;
        while(tryCount < tryCountMax){
            tryCount++;
            if (isBlock(stageLayer->getTileGIDAt(pos))) {
                pos = Point(pos.x, pos.y + 1);
                if (pos.y >= map->getMapSize().height) {
                    break;
                }else{
                    continue;
                }
            }
            addEnemyToLoadStack(map, levelScore, pos, -1, -1, true);
            break;
        }
        
        enemyCountLeft--;
    }
    
//    this->runAction(Sequence::create(DelayTime::create(dur), CallFunc::create(CC_CALLBACK_0(HelloWorld::loadEnemies, this)), NULL));
    map->getLayer("deco")->setVisible(true);
    
    // add Destructable
    tryCount = 0;
    tryCountMax = map->getMapSize().width*map->getMapSize().height/15;
    bool found = false;
    while(tryCount < tryCountMax){
        found = false;
        while(tryCount < tryCountMax){
            tryCount++;
            pos = Point(3 + rand()%(int)(map->getMapSize().width - 6), map->getMapSize().height/2 + rand()%(int)(map->getMapSize().height/2) - 2);
            int tileIndex = map->getLayer("stage")->getTileGIDAt(Point(pos.x, pos.y + 1));
            if ((isBlock(tileIndex) || isOneWay(tileIndex)) && map->getLayer("stage")->getTileGIDAt(pos) == 0 && tileIndex != 322) {
                found = true;
                break;
            }
        }
        if (!found) {
            continue;
        }
        
        Point thePos = map->getPosition() + Point(pos.x*TILE_SIZE + rand()%TILE_SIZE, (map->getMapSize().height - pos.y - 1)*TILE_SIZE + TILE_SIZE/2 - 2);
        int destructableIndex = rand()%8;
        int coinCount = 0;
        int goldPossibility=10;
        int eng = 5;
        if (destructableIndex == 6 || (destructableIndex == 7 && goldChestCount > 1)) {
            destructableIndex = 6;
            coinCount = rand()%5;
            eng = 50;
        }else if (destructableIndex == 7) {
            goldChestCount++;
            coinCount = rand()%5 + 15;
            goldPossibility = 30;
            eng = 150;
        }
        EnemyBase* sptDestructable = EnemyBase::createEnemy(UNIT_DESTRUCTABLE, eng, __String::createWithFormat("destructable%d.png", destructableIndex)->getCString(), coinCount, goldPossibility);
        sptDestructable->secondTag = destructableIndex;
        sptDestructable->setPosition(thePos);
        destructableArray.pushBack(sptDestructable);
        spriteBatch->addChild(sptDestructable);
    }
}
void HelloWorld::setEmptyMap(TMXTiledMap* map){
    int mapSizeX = map->getMapSize().width;
    int mapSizeY = map->getMapSize().height;
    
    TMXLayer* stageLayer = map->getLayer("stage");
    TMXLayer* decoLayer = map->getLayer("deco");
    int currentX = 3;
    int currentY = 4;
    int squareWidth = 10;
    Point pos;
    
    while(currentY < mapSizeY - 3){
        while(currentX < mapSizeX - 3){
            pos = Point(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(34, pos);
            }
            pos = Point(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(34, pos);
            }
            
            pos = Point(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(130, pos);
            }
            
            pos = Point(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(130, pos);
            }
            
            /*
            if (rand()%100 < 20) {
                pos = Point(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
                if (pos.y >= mapSizeY) {
                    pos.y = mapSizeY - 1;
                }
                if (pos.x >= mapSizeX) {
                    pos.x = mapSizeX - 1;
                }
                decoLayer->setTileGID(rand()%2==0?50:51, pos);
            }*/
            
            currentX += squareWidth;
        }
        currentX = 3;
        currentY += squareWidth;
    }
}

FireableBase* HelloWorld::addEnemyToLoadStack(TMXTiledMap* map, int levelScore, Point pos, int missile, int enemyModel, bool addGround){
    int totalHeight = map->getMapSize().height;
    float dur = 2;
    int extraScore = -10;
    int enemyVar = (levelScore + extraScore)/80;
    enemyVar = 7;
    
    extraScore += 20;
    int extraSpeed = 0;
    int missileType = -1;
    
    const char* nick;
    bool hideGun = false;
    EnemyWithGun* fEnemy;
    char sptName[50];
    char aniName[50];
    bool hasAni=true;
    int moveTypeBeforeFindTarget = ENEMY_MOVE_WANDER_ON_A_PLATFORM;
    int moveTypeAfterFindTarget = ENEMY_MOVE_TO_CLIFF;
    int gunType = ENEMY_GUN_TYPE_HANDGUN;
    int bulletCount = 1;
    bool isBlue = rand()%(100) < 5 + levelScore/10;
    //isBlue = true;// test
    float reloadingTime = 3;
    int bulletSpeed = 100;
    int enemy = rand()%enemyVar;
    if(enemyModel >= 0){
        enemy = -1;
    }
    int enemyType = -1;
    std::string fireAni = "";
    int eng = 40;
    if(enemy == 0 || enemyModel == UNIT_PIERROT_NORMAL){
        enemyType = UNIT_PIERROT_NORMAL;
        missileType = MISSILE_SINGLE;
        
        nick = "";
    }else if(enemy == 1 || enemyModel == UNIT_PIERROT_SCOT){
        enemyType = UNIT_PIERROT_SCOT;
        missileType = MISSILE_CIRCLE;
        eng = 60;
        nick = "Scot";
    }else if(enemy == 2 || enemyModel == UNIT_PIERROT_CROSS){
        enemyType = UNIT_PIERROT_CROSS;
        missileType = MISSILE_ARC;
        eng = 80;
        nick = "CrossEye";
    }else if(enemy == 3 || enemyModel == UNIT_PIERROT_VERTIAL_EYE){
        enemyType = UNIT_PIERROT_VERTIAL_EYE;
        missileType = MISSILE_COLUMN;
        eng = 100;
        nick = "VerticalEye";
    }else if(enemy == 4 || enemyModel == UNIT_PIERROT_GHOST){
        enemyType = UNIT_PIERROT_GHOST;
        missileType = MISSILE_ARC;
        eng = 120;
        nick = "Ghost";
        extraSpeed = -20;
        hideGun = true;
        moveTypeBeforeFindTarget = ENEMY_MOVE_HANG;
        moveTypeAfterFindTarget = ENEMY_MOVE_FLY_TO_HERO_FOLLOW;
    }else if(enemy == 5 || enemyModel == UNIT_PIERROT_DOG){
        enemyType = UNIT_PIERROT_DOG;
        missileType = MISSILE_NONE;
        eng = 140;
        nick = "Dog";
        hideGun = true;
        extraSpeed = 50;
        moveTypeBeforeFindTarget = ENEMY_MOVE_WANDER_ON_A_PLATFORM;
        moveTypeAfterFindTarget = ENEMY_MOVE_WANDER_CROSS_PLATFORM;
    }else if(enemy == 6 || enemyModel == UNIT_PIERROT_TALL){
        enemyType = UNIT_PIERROT_TALL;
        missileType = MISSILE_SPIRAL;
        if (isBlue) {
            fireAni = "pierrotBlueTall";
        }else{
            fireAni = "pierrotTall";
        }
        eng = 200;
        hasAni = false;
        hideGun = true;
        nick = "Tall";
        moveTypeBeforeFindTarget = ENEMY_MOVE_STAND;
        moveTypeAfterFindTarget = ENEMY_MOVE_STAND;
    }
    if (missile >= 0) {
        missileType = missile;
    }
    if (missileType == MISSILE_SINGLE ||
        missileType == MISSILE_SPIRAL) {
        bulletCount = 3;
    }else if (missileType == MISSILE_ARC) {
        bulletCount = 2;
    }else if (missileType == MISSILE_COLUMN) {
        bulletCount = 1;
    }else if (missileType == MISSILE_CIRCLE ||
              missileType == MISSILE_RANDOM) {
        bulletCount = 1;
    }else if (missileType == MISSILE_FOUNTAIN) {
        bulletCount = 1;
        hasAni = false;
    }
    
    sprintf(sptName, "pierrot%s0.png", nick);
    if (hasAni) {
        sprintf(aniName, "pierrot%s", nick);
        if (isBlue) {
            sprintf(aniName, "pierrotBlue%s", nick);
        }
    }
    if (isBlue) {
        sprintf(sptName, "pierrotBlue%s0.png", nick);
    }
    if(enemyModel == UNIT_RUNNING_HUD){
        enemyType = UNIT_RUNNING_HUD;
        missileType = MISSILE_CIRCLE;
        bulletCount = 2;
        eng = 1180;
        sprintf(sptName, "runningHud0.png");
        sprintf(aniName, "runningHud");
    }
    int scale = 1;
    if (isBlue) {
        bulletCount *= 2;
        eng *= 1.6f;
    }else{
        if (rand()%100 < 3) {
            scale = 2;
            eng *= 4;
        }
    }
    int coinReward = 10;
    int goldPossibility = 10;
    fEnemy = EnemyWithGun::create(enemyType, eng, extraSpeed, sptName, coinReward, goldPossibility, moveTypeBeforeFindTarget, moveTypeAfterFindTarget, 120);
    fEnemy->setScale(scale);
    if (hasAni) {
        fEnemy->runAnimation(aniName, true);
    }
    if (isBlue) {
        fEnemy->startJumping();
    }
    fEnemy->fireAniName = fireAni;
    fEnemy->setTarget(player);
    fEnemy->missileType = missileType;
    ((EnemyWithGun*)fEnemy)->setGunInfo(bulletCount, reloadingTime, gunType, bulletSpeed);
    fEnemy->sptGun->setVisible(!hideGun);
    spriteBatch->addChild(fEnemy);
    Point enemyPos = map->getPosition() + Point(pos.x*TILE_SIZE + TILE_SIZE/2, (totalHeight-pos.y - 1)*TILE_SIZE + TILE_SIZE/2);
    TMXLayer* stageLayer = map->getLayer("stage");
    if (addGround) {
        int tryCount = 100;
        Point tempPos;
        while(tryCount > 0){
            tempPos = Point(3 + rand()%(int)(map->getMapSize().width - 6), 3 + rand()%(int)(map->getMapSize().height - 6));
            if (stageLayer->getTileGIDAt(tempPos)) {
                enemyPos = map->getPosition() + Point(tempPos.x*TILE_SIZE + TILE_SIZE/2, (totalHeight-tempPos.y)*TILE_SIZE + TILE_SIZE/2);
                if (rand()%100 < 100) {
                    int gid = stageLayer->getTileGIDAt(tempPos);
                    int length = rand()%7 + 3;
                    for(int i = 0;i< length;i++){
                        if (map->getMapSize().width - 3 > tempPos.x + i) {
                            stageLayer->setTileGID(gid, tempPos + Point(i, 0));
                        }
                    }
                }
                break;
            }
            tryCount--;
        }
    }
    enemyPointArray->addControlPoint(enemyPos);
    
    for (int i = 0; i < 4; i++) {
        Sprite* sptWarning = Sprite::createWithSpriteFrameName(__String::createWithFormat("enemyWarning%d.png", i)->getCString());
        sptWarning->setPosition(enemyPos);
        int angle = i*50;
        if (i == 2) {
            angle *= -3;
        }
        sptWarning->runAction(Sequence::create(RotateBy::create(dur, angle), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptWarning)), NULL));
        if (i == 3) {
            sptWarning->runAction(ScaleTo::create(dur, 0.2));
        }
        spriteBatch->addChild(sptWarning);
    }
    fEnemy->setVisible(false);
    fEnemy->setPosition(Point(0, 10000));
    //                enemyArray.pushBack(fEnemy);
    //                fireableArray.pushBack(fEnemy);
    //                droppableArray.pushBack(fEnemy);
    enemiesToLoad.pushBack(fEnemy);
    loadedEnemyList.push_back(fEnemy->unitType);
    return fEnemy;
}
void HelloWorld::loadEnemies(){
    for(auto enemy: enemiesToLoad){
        enemyArray.pushBack(enemy);
        enemy->setTarget(player);
        enemy->setVisible(true);
        if (enemy->getTag() == UNIT_PIERROT_DOG) {
            droppableArray.pushBack(enemy);
        }else if(enemy->getTag() == UNIT_PIERROT_GHOST){
            fireableArray.pushBack((FireableBase*)enemy);
            flyingSwimingEnemyArray.pushBack(enemy);
        }else{
            droppableArray.pushBack(enemy);
            fireableArray.pushBack((FireableBase*)enemy);
        }
        enemy->setPosition(enemyPointArray->getControlPointAtIndex(0));
        enemyPointArray->removeControlPointAtIndex(0);
        addAppearEffect(enemy);
    }
    enemyPointArray->clone();
    enemiesToLoad.clear();
}
void HelloWorld::addAppearEffect(Sprite* parent){
    Sprite* sptEffect = Sprite::createWithSpriteFrameName("bombEffect0.png");
    sptEffect->setPosition(Point(parent->getContentSize().width/2, parent->getContentSize().height/2));
    sptEffect->runAction(RotateBy::create(1, 400));
    sptEffect->setScale(0.5);
    sptEffect->runAction(Sequence::create(ScaleTo::create(0.3, 1.5), ScaleTo::create(0.2, 0.1), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptEffect)), NULL));
    parent->addChild(sptEffect);
}
void HelloWorld::updateMiniMap(){
    dnMiniMap->clear();
    dnMiniMap->removeAllChildren();
    for(auto map: revealedMapArray){
        addMapToMiniMap(map);
    }
    // set question mark
    int index = 0;
    for (auto map: revealedMapArray) {
        bool isRightChecked = false;
        bool isLeftChecked = false;
        for (int y = 0; y < map->getMapSize().height; y++) {
            if (map->getLayer("stage")->getTileGIDAt(Point(map->getMapSize().width - 1, y)) == 0 && !isRightChecked) {//check right
                for (auto nextMap: mapArray) {
                    Point pos = Point(map->getBoundingBox().getMaxX() + TILE_SIZE, map->getBoundingBox().getMinY() + (map->getMapSize().height - y)*TILE_SIZE);
                    if (nextMap->getBoundingBox().containsPoint(pos)) {
                        if (revealedMapArray.find(nextMap) == revealedMapArray.end()) {
                            Sprite* sptMark = Sprite::create("miniMapQuestionMark.png");
                            dnMiniMap->addChild(sptMark);
                            sptMark->setAnchorPoint(Point(0.5, 1));
                            sptMark->setPosition(pos * miniMapScale + Point(4, 1));
                            isRightChecked = true;
                            sptMark->setScale(TILE_SIZE/sptMark->getContentSize().width);
                        }
                    }
                }
            }
            if (map->getLayer("stage")->getTileGIDAt(Point(0, y)) == 0 && !isLeftChecked) {//check left
                for (auto nextMap: mapArray) {
                    Point pos = Point(map->getBoundingBox().getMinX() - TILE_SIZE, map->getBoundingBox().getMinY() + (map->getMapSize().height - y)*TILE_SIZE);
                    if (nextMap->getBoundingBox().containsPoint(pos)) {
                        if (revealedMapArray.find(nextMap) == revealedMapArray.end()) {
                            Sprite* sptMark = Sprite::create("miniMapQuestionMark.png");
                            dnMiniMap->addChild(sptMark);
                            sptMark->setAnchorPoint(Point(0.5, 1));
                            sptMark->setPosition(pos * miniMapScale + Point(-4, 1));
                            isLeftChecked = true;
                            sptMark->setScale(TILE_SIZE/sptMark->getContentSize().width);
                        }
                    }
                }
            }
        }
        
        bool isTopChecked = false;
        bool isBottomChecked = false;
        for (int x = 0; x < map->getMapSize().width; x++) {
            if (map->getLayer("stage")->getTileGIDAt(Point(x, 0)) == 0 && !isTopChecked) {//check top
                for (auto nextMap: mapArray) {
                    Point pos = Point(map->getBoundingBox().getMinX() + x*TILE_SIZE, map->getBoundingBox().getMaxY() + TILE_SIZE/2);
                    if (nextMap->getBoundingBox().containsPoint(pos)) {
                        if (revealedMapArray.find(nextMap) == revealedMapArray.end()) {
                            Sprite* sptMark = Sprite::create("miniMapQuestionMark.png");
                            dnMiniMap->addChild(sptMark);
                            sptMark->setAnchorPoint(Point(0.5, 0));
                            sptMark->setScale(TILE_SIZE/sptMark->getContentSize().width);
                            sptMark->setPosition(pos * miniMapScale + Point(6, 0));
                            isTopChecked = true;
                        }
                    }
                }
            }
            if (map->getLayer("stage")->getTileGIDAt(Point(x, map->getMapSize().height - 1)) == 0 && !isBottomChecked) {//check bottom
                for (auto nextMap: mapArray) {
                    Point pos = Point(map->getBoundingBox().getMinX() + x*TILE_SIZE, map->getBoundingBox().getMinY() - TILE_SIZE/2);
                    if (nextMap->getBoundingBox().containsPoint(pos)) {
                        if (revealedMapArray.find(nextMap) == revealedMapArray.end()) {
                            Sprite* sptMark = Sprite::create("miniMapQuestionMark.png");
                            dnMiniMap->addChild(sptMark);
                            sptMark->setAnchorPoint(Point(0.5, 1));
                            sptMark->setPosition(pos * miniMapScale + Point(0, 1));
                            sptMark->setScale(TILE_SIZE/sptMark->getContentSize().width);
                            isBottomChecked = true;
                        }
                    }
                }
            }
        }
    }
}
void HelloWorld::addMapToMiniMap(TMXTiledMap* map){
    Rect rect = map->getBoundingBox();
    rect = Rect(rect.origin.x*miniMapScale, rect.origin.y*miniMapScale, rect.size.width*miniMapScale, rect.size.height*miniMapScale);
    //            dnMiniMap->drawSolidRect(Point(rect.getMinX(), rect.getMinY()), Point(rect.getMaxX(), rect.getMaxY()), Color4F(1, 0, 0, 0.3));
    TMXLayer* theStageLayer = map->getLayer("stage");
    for (int x = 0; x < map->getMapSize().width; x++) {
        for (int y = 0; y < map->getMapSize().height; y++) {
            int gid = theStageLayer->getTileGIDAt(Point(x, y));
            if (gid) {
                int realY = map->getMapSize().height - y - 1;
                dnMiniMap->drawSolidRect(rect.origin + Point(x*TILE_SIZE, realY*TILE_SIZE)*miniMapScale, rect.origin + Point(x*TILE_SIZE + TILE_SIZE, realY*TILE_SIZE + TILE_SIZE)*miniMapScale, Color4F(0.3f, 0.3f, 0.3f, miniMapAlpha/255.0f));
                
            }
        }
    }
}
void HelloWorld::setNamingRoom(TMXTiledMap* map){
    
}
void HelloWorld::resetWeapons(){
    for (int i = 0; i < WEAPON_NO_MORE; i++) {
        setWeaponAtSlot(i, WEAPON_NOT_EXIST);
    }
}
void HelloWorld::setWeaponsFromEquiped(){
    resetWeapons();
    addWeapon(0, 0);
//    collectedCoinCount = 1000; // test 
    int value;
    for (int i = 0; i < 4; i++) {
        value = UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, i).c_str(), -1);
        if(value > 0){
            addWeapon(value%1000, value/1000);
        }
    }
    changeWeapon(0);
    
    for(auto shield:playerShields){
        shield->removeFromParent();
    }
    playerShields.clear();
    float extraEnergy = 0;
    for (int i = 0; i < 4; i++) {
        int value = UDGetInt(strmake(KEY_SHIELD_SLOT_FORMAT, i).c_str(), -1);
        log("add shield: %d", value);
        if(value > 0){
            Droppable* drop = Droppable::createDroppable(value, GM->getShieldHPRuneApplied(value), 0, strmake("shield/shield%d", (value%1000)%600).c_str());
            drop->setScale(0.5f);
            drop->DP = GM->getShieldDefRuneApplied(value);
            playerShields.pushBack(drop);
            this->addChild(drop, player->getLocalZOrder() + 1);
            extraEnergy += drop->energy;
        }
    }
    player->energy = GM->getHeroHP() + extraEnergy;
    player->maxEnergy = player->energy;
    HUD->setLife(player->energy);
}
void HelloWorld::setLobby(TMXTiledMap* map){
//    if (UserDefault::getInstance()->getBoolForKey(KEY_IS_TUTORIAL_CLEAR, false)) {  // naming
//        map->getLayer("stage")->removeTileAt(Point(21, 17));
//        map->getLayer("stage")->removeTileAt(Point(22, 17));
//        map->getLayer("stage")->removeTileAt(Point(23, 17));
//        map->getLayer("stage")->removeTileAt(Point(21, 18));
//        map->getLayer("stage")->removeTileAt(Point(22, 18));
//        map->getLayer("stage")->removeTileAt(Point(23, 18));
//    }
    resetWeapons();
    GameManager::getInstance()->isWeaponSet = false;
    changeWeapon(GM->lastWeaponSlot);
    GameManager::getInstance()->totalKill = 0;
    GameManager::getInstance()->totalDestroy=0;
    GameManager::getInstance()->totalCoin=0;
    GameManager::getInstance()->totalRoom=0;
    GameManager::getInstance()->totalBoss=0;
    GameManager::getInstance()->totalTime=60*60;
    GameManager::getInstance()->totalScore = 0;
    GameManager::getInstance()->jewelryCollected = false;
    
    TutorialGhost* tGhost = TutorialGhost::create();
    this->addChild(tGhost, 1000);
    tGhost->setPosition(Point(100, 100));
    destructableArray.pushBack(tGhost);
    tGhost->setName("tutorialGhost");
    Animation* animation = AnimationCache::getInstance()->getAnimation("tutorialGhost");
    Animate* animate = Animate::create(animation);
    RepeatForever* forever = RepeatForever::create(animate);
    tGhost->runAction(forever);
}
bool HelloWorld::isRoomEmpty(Rect rect){
    Rect targetRect;
    for (int i = 0; i < mapArray.size(); i++) {
        targetRect = mapArray.at(i)->getBoundingBox();
        
        if (intersectsRect(rect, targetRect)) {
            return false;
        }
        
    }
    return true;
}
TMXTiledMap* HelloWorld::findDirectionAndPlaceTheMap(TMXTiledMap* srcMap, TMXTiledMap* dstMap){
    bool found = false;
    int directionToFind = rand()%4;
    int directionCounter = 0;
    Rect nextRect;
    Rect srcRect;
    Size dstSize = dstMap->getContentSize();
    int randomData;
    while(true){   // find room for the selected map
        srcRect = srcMap->getBoundingBox();
        if (directionToFind == 0) { // check right
            randomData = (rand()%(int)dstMap->getMapSize().height);
            if (randomData <= 1) {
                randomData = 2;
            }else if(randomData >= dstMap->getMapSize().height - 2){
                randomData = dstMap->getMapSize().height - 3;
            }
            randomData *= TILE_SIZE;
            nextRect = Rect(srcRect.getMaxX(), srcRect.origin.y + ((int)srcMap->getMapSize().height/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, dstSize.width, dstSize.height);
        }else if (directionToFind == 1) { // check bottom
            randomData = (rand()%(int)dstMap->getMapSize().width);
            if (randomData <= 1) {
                randomData = 2;
            }else if(randomData >= dstMap->getMapSize().width - 2){
                randomData = dstMap->getMapSize().width - 3;
            }
            randomData *= TILE_SIZE;
            
            nextRect = Rect( srcRect.origin.x + ((int)srcMap->getMapSize().width/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, srcRect.getMinY() - dstSize.height, dstSize.width, dstSize.height);
        }else if (directionToFind == 2) { // check left
            randomData = (rand()%(int)dstMap->getMapSize().height);
            if (randomData <= 1) {
                randomData = 2;
            }else if(randomData >= dstMap->getMapSize().height - 2){
                randomData = dstMap->getMapSize().height - 3;
            }
            randomData *= TILE_SIZE;
            nextRect = Rect(srcRect.getMinX() - dstSize.width, srcRect.origin.y + ((int)srcMap->getMapSize().height/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, dstSize.width, dstSize.height);
        }else if (directionToFind == 3) { // check top
            randomData = (rand()%(int)dstMap->getMapSize().width);
            if (randomData <= 1) {
                randomData = 2;
            }else if(randomData >= dstMap->getMapSize().width - 2){
                randomData = dstMap->getMapSize().width - 3;
            }
            randomData *= TILE_SIZE;
            nextRect = Rect( srcRect.origin.x + ((int)srcMap->getMapSize().width/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, srcRect.getMaxY(), dstSize.width, dstSize.height);
        }
        
        if (isRoomEmpty(nextRect)) {
            found = true;
            this->addChild(dstMap);
            dstMap->setPosition(nextRect.origin);
            
            if (directionToFind == 0) { // connect right
                int x = srcMap->getMapSize().width - 1;
                float minY = getMax(srcRect.getMinY(), nextRect.getMinY());
                float maxY = getMin(srcRect.getMaxY(), nextRect.getMaxY());
                int midY = (minY + maxY)/2;
                
                int y = srcMap->getMapSize().height - (midY - srcRect.getMinY())/TILE_SIZE;
                int counter = 0;
                while(true){
                    if (srcMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(6, Point(x, y + 1));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(70, Point(x, y - 1));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(5, Point(x, y + 1));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(69, Point(x, y - 1));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter%2==0) {
                        y--;
                    }else{
                        x--;
                        y++;
                    }
                    
                    counter++;
                }
                x = 0;
                y = dstMap->getMapSize().height - (midY - nextRect.getMinY())/TILE_SIZE;
                counter = 0;
                while(true){
                    if (dstMap->getLayer("stage")->getTileGIDAt(Point(x, y)) && y > 0) {
                        dstMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(6, Point(x, y + 1));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(70, Point(x, y - 1));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(7, Point(x, y + 1));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(71, Point(x, y - 1));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter%2==0) {
                        y--;
                    }else{
                        x++;
                        y++;
                    }
                    
                    counter++;
                }
            }else if (directionToFind == 1) { // connect bottom
                float minX = getMax(srcRect.getMinX(), nextRect.getMinX());
                float maxX = getMin(srcRect.getMaxX(), nextRect.getMaxX());
                int midX = (minX + maxX)/2;
                int x = (midX - srcRect.getMinX())/TILE_SIZE;
                int y = srcMap->getMapSize().height - 1;
                int counter = 0;
                while(true){
                    if (srcMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(37, Point(x + 1, y));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(39, Point(x - 1, y));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(5, Point(x + 1, y));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(7, Point(x - 1, y));
                        }
                    }else{
                        break;
                    }
                    if (counter==1) {
                        srcMap->getLayer("stage")->setTileGID(130, Point(x, y));
                    }
                    
                    if (counter%2==0) {
                        x--;
                    }else{
                        y--;
                        x++;
                    }
                    
                    counter++;
                }
                x = (midX - nextRect.getMinX())/TILE_SIZE;
                y = 0;//dstMap->getMapSize().height - 1;
                counter = 0;
                while(true){
                    if (dstMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        dstMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(37, Point(x + 1, y));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(39, Point(x - 1, y));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(69, Point(x + 1, y));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(71, Point(x - 1, y));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter==1) {
                        dstMap->getLayer("stage")->setTileGID(130, Point(x, y));
                    }
                    if (counter%2==0) {
                        x--;
                    }else{
                        y++;
                        x++;
                    }
                    
                    counter++;
                }
                // ladder for top
                counter = 0;
                while(true){
                    if (dstMap->getLayer("stage")->getTileGIDAt(Point(x, y)) == 0 && dstMap->getMapSize().height > y &&
                        dstMap->getLayer("stage")->getTileGIDAt(Point(x - 1, y)) == 0) {
                        if (counter >= 3) {
                            dstMap->getLayer("stage")->setTileGID(130, Point(x, y));
                            counter = 0;
                        }
                    }else{
                        break;
                    }
                    
                    y++;
                    counter++;
                }
            }else if (directionToFind == 2) { // connect left
                int x = 0;
                float minY = getMax(srcRect.getMinY(), nextRect.getMinY());
                float maxY = getMin(srcRect.getMaxY(), nextRect.getMaxY());
                int midY = (minY + maxY)/2;
                
                int y = srcMap->getMapSize().height - (midY - srcRect.getMinY())/TILE_SIZE;
                int counter = 0;
                while(true){
                    if (srcMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(6, Point(x, y + 1));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(70, Point(x, y - 1));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(7, Point(x, y + 1));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(71, Point(x, y - 1));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter%2==0) {
                        y--;
                    }else{
                        x++;
                        y++;
                    }
                    
                    counter++;
                }
                x = dstMap->getMapSize().width - 1;
                y = dstMap->getMapSize().height - (midY - nextRect.getMinY())/TILE_SIZE;
                counter = 0;
                while(true){
                    if (dstMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        dstMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(6, Point(x, y + 1));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(70, Point(x, y - 1));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(5, Point(x, y + 1));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(69, Point(x, y - 1));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter%2==0) {
                        y--;
                    }else{
                        x--;
                        y++;
                    }
                    
                    counter++;
                }
            }else if (directionToFind == 3) { // connect top
                float minX = getMax(srcRect.getMinX(), nextRect.getMinX());
                float maxX = getMin(srcRect.getMaxX(), nextRect.getMaxX());
                int midX = (minX + maxX)/2;
                int x = (midX - srcRect.getMinX())/TILE_SIZE;
                int y = 0;//srcMap->getMapSize().height - 1;
                int counter = 0;
                while(true){
                    if (srcMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(37, Point(x + 1, y));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(39, Point(x - 1, y));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(69, Point(x + 1, y));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(71, Point(x - 1, y));
                        }
                    }else{
                        
                        break;
                    }
                    if (counter==1) {
                        srcMap->getLayer("stage")->setTileGID(130, Point(x, y));
                    }
                    
                    if (counter%2==0) {
                        x--;
                    }else{
                        y++;
                        x++;
                    }
                    
                    counter++;
                }
                // ladder for top
                counter = 0;
                while(true){
                    if (srcMap->getLayer("stage")->getTileGIDAt(Point(x, y)) == 0 && y < srcMap->getMapSize().height &&
                        srcMap->getLayer("stage")->getTileGIDAt(Point(x - 1, y)) == 0) {
                        if (counter >= 3) {
                            srcMap->getLayer("stage")->setTileGID(130, Point(x, y));
                            counter = 0;
                        }
                    }else{
                        break;
                    }
                    
                    y++;
                    counter++;
                }
                x = (midX - nextRect.getMinX())/TILE_SIZE;
                y = dstMap->getMapSize().height - 1;
                counter = 0;
                while(true){
                    if (dstMap->getLayer("stage")->getTileGIDAt(Point(x, y)) && y > 0) {
                        dstMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(37, Point(x + 1, y));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(39, Point(x - 1, y));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(5, Point(x + 1, y));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(7, Point(x - 1, y));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter==1) {
                        dstMap->getLayer("stage")->setTileGID(130, Point(x, y));
                    }
                    
                    if (counter%2==0) {
                        x--;
                    }else{
                        y--;
                        x++;
                    }
                    
                    counter++;
                }
            }
        }
        
        if (found) {
            break;
        }
        directionToFind++;
        if (directionToFind >= 4) {
            directionToFind = 0;
        }
        
        directionCounter++;
        if (directionCounter >= 4) {
            break;
        }
    }
    if (!found) {
        return nullptr;
    }
    
    return dstMap;
}
float HelloWorld::getMin(float src, float dst){
    return src < dst?src:dst;
}
float HelloWorld::getMax(float src, float dst){
    return src > dst?src:dst;
}
void HelloWorld::setStage(TMXTiledMap* tileMap)
{
//    Sprite* sptLight = getLight();
//    this->addChild(sptLight, 1);
//    sptLight->setPosition(100, 100);
    
    
    if (tileMap->getTag() == MAP_SHOP) {
//        GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("found a shop").c_str(), GameManager::getInstance()->getHudLayer());
        HUD->showInstanceMessage(LanguageManager::getInstance()->getText("found a shop"));
    }else if (tileMap->getTag() == MAP_WEAPON) {
//        GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("suitcase room").c_str(), GameManager::getInstance()->getHudLayer());
        HUD->showInstanceMessage(LanguageManager::getInstance()->getText("suitcase room"));
    }
    float laserTrapDelay=0;
    float missileTrapDelay=0;
    tileMap->getChildByTag(TAG_STAGE_LAYER)->setOpacity(255);
    tileMap->getChildByTag(TAG_BACK_LAYER)->setOpacity(255);
    tileMap->getChildByTag(TAG_DECO_LAYER)->setOpacity(255);
    
    int themeIndex = GameManager::getInstance()->theme;
    if(themeIndex > 3){
        themeIndex = 3;
    }
    
    // change coin tile into coin animation
    int totalWidth = tileMap->getMapSize().width;
    int totalHeight = tileMap->getMapSize().height;
    Point point;
    
    
    stageLayer = tileMap->getLayer("stage");
    TMXLayer* unitLayer = tileMap->getLayer("unit");
    TMXLayer* decoLayer = tileMap->getLayer("deco");
    TMXLayer* foreLayer = tileMap->getLayer("fore");
    TMXLayer* backLayer = tileMap->getLayer("back");
    PointArray* pointArrayToRemoveInUnitLayer = PointArray::create(3000);
    
    for (int i = 0; i < tileMap->getMapSize().width; i++) {
        for (int j = 0; j < tileMap->getMapSize().height; j++) {
            if (isBlock(stageLayer->getTileGIDAt(Point(i, j)))) {
                stageTileRectArray.push_back(Rect(i*TILE_SIZE + tileMap->getPositionX(), (tileMap->getMapSize().height - j - 1)*TILE_SIZE + tileMap->getPositionY(), TILE_SIZE, TILE_SIZE));
            }
        }
    }
    
    for (int i = 0; i < totalWidth; i++) {
        for (int j = 0; j < totalHeight; j++) {
            point = Point(i,j);
            int gid = unitLayer->getTileGIDAt(point);
            if (gid) {
//                if(!keyExist(map, "MovingPlatformLine") &&
//                   !keyExist(map, "MovingPlatformEnd")){
//                    unitLayer->removeTileAt(point);
//                }
                EnemyBase* enemy = NULL;
                FireableBase* fEnemy = NULL;
                if(gid == 234234){//coin
                    Droppable* coinSprite = Droppable::createDroppable(UNIT_COIN, 1, 0, "silverCoin0.png");
                    spriteBatch->addChild(coinSprite);
                    coinArray.pushBack(coinSprite);
                    coinSprite->runAnimation("silverCoin", true);
                    coinSprite->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + 9, (totalHeight-j)*TILE_SIZE - 7));
                }else if(gid == 234234){//coin part
                    Droppable* coinSprite = Droppable::createDroppable(UNIT_COIN_PART, 1, 0, "coinPart.png");
                    spriteBatch->addChild(coinSprite);
                    coinArray.pushBack(coinSprite);
                    //                        coinSprite->runAnimation("silverCoin", true);
                    coinSprite->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + 9, (totalHeight-j)*TILE_SIZE - 7));
                }else if(gid == 234234){//gold coin
                    Droppable* coinSprite = Droppable::createDroppable(UNIT_GOLD_COIN, 1, 0, "goldCoin0.png");
                    spriteBatch->addChild(coinSprite);
                    coinArray.pushBack(coinSprite);
                    coinSprite->runAnimation("goldCoin", true);
                    coinSprite->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + 9, (totalHeight-j)*TILE_SIZE - 7));
                }else if(gid == 234234){
                    theKey = Droppable::createDroppable(UNIT_KEY, 1, 0, "key.png");
                    spriteBatch->addChild(theKey);
                    coinArray.pushBack(theKey);
                    GameManager::getInstance()->makeItSiluk(theKey);
                    theKey->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + 9, (totalHeight-j)*TILE_SIZE - 7));
                    indicatedArray.pushBack(theKey);
                }else if(gid == 234234){
                    Droppable* star = Droppable::createDroppable(UNIT_STAR, 1, 0, "star.png");
                    spriteBatch->addChild(star);
                    coinArray.pushBack(star);
                    star->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + 9, (totalHeight-j)*TILE_SIZE - 7));
                    starArray.pushBack(star);
                    GameManager::getInstance()->makeItSiluk(star);
                    indicatedArray.pushBack(star);
                    
                    if(mastery > 5){
                        EnemyBase* enemy = EnemyBase::createEnemy(UNIT_ELECTRIC_BALL, 0, 0, "theme2EletricBall.png",0, 50, ENEMY_MOVE_CUSTOM, ENEMY_MOVE_CUSTOM, 0);
                        spriteBatch->addChild(enemy);
                        float distanceToMove = 80;
                        enemy->setPosition(star->getPosition() + Point(-40,0));
                        float timeToMove = (rand()%5)*0.1f + 0.7f;
                        enemy->runAction(RepeatForever::create(Sequence::create(MoveBy::create(timeToMove, Point(distanceToMove, 0)), MoveBy::create(timeToMove, Point(-distanceToMove, 0)), NULL)));
                        enemy->runAction(RepeatForever::create(RotateBy::create(0.5, 360)));
                        enemyArray.pushBack(enemy);
                        enemy->untouchable = true;
                    }
                }else if(gid == 49){
                    GameManager::getInstance()->isPlayerSet = true;
                    
                    GameManager::getInstance()->preLoadAllSoundEffect();
                    collectedCoinPart = 0;
                    GameManager::getInstance()->getHudLayer()->keyCollected(GameManager::getInstance()->collectedKeyCount);
                    enemyKilledCount = 0;
                    enemyMaxCount = 0;
                    collectedCoinCount = GameManager::getInstance()->getCoin();
                    GameManager::getInstance()->getHudLayer()->updateCoin(collectedCoinCount);
//                    NativeInterface::NativeInterface::destroyAds();
                    
                    GameManager::getInstance()->setButtonSize(GameManager::getInstance()->getButtonSize());
                    playerFireCoolTime = 0;
                    indestructableTime = 0;
                    starCount = 0;
                    
                    float spd = 80;
                    int eng = GM->getHeroHP();
                    int playerType = UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_CHARACTER, CHARACTER_DARY);
                    int weapon = UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_WEAPON, WEAPON_PISTOL);
                    if (GameManager::getInstance()->developer) {
                        //        weapon = WEAPON_GUIDED_ROCKET_LAUNCHER;
                    }
                    
                    spd = 10;
                    
                    heroPositionArray = PointArray::create(50);
                    heroPositionArray->retain();
                    
                    player = Hero::createHero(playerType, eng, spd);
                    player->weapon = weapon;
                    player->doubleJump = true;
                    player->maxEnergy = eng;
                    player->energy = eng;
                    player->setRestOfBodyPart();
                    player->runHatCheck();
                    player->oneWayCancelY = -1;
                    player->setFlippedX(player->isTowardLeft);
                    setPlayerInfo();
                    
                    float angle = player->isTowardLeft?3.14f:0;
                    int radius = 170;
                    float dx = cosf(angle) * radius;
                    float dy = sinf(angle) * radius;
                    GameManager::getInstance()->getHudLayer()->cursorLayer->cursor->setPosition(center + Point(dx, dy));
                    GameManager::getInstance()->getHudLayer()->cursorLayer->cursor->setRotation(angle*180);
                    
                    heroLight = getLight();
                    heroLight->setVisible(true);
                    heroLight->setPosition(Point(player->getContentSize().width/2, player->getContentSize().height/2));
                    heroLight->stopAllActions();
//                    heroLight->setColor(Color3B(100, 100, 200));
                    this->addChild(heroLight, 1);
                    
//                    Node* backupLight = getLight();
//                    backupLight->setColor(Color3B::WHITE);
//                    backupLight->setPosition(heroLight->getContentSize().width/2, heroLight->getContentSize().height/2);
//                    heroLight->addChild(backupLight);
                    //    if (GameManager::getInstance()->isShieldPurcahsed()) {
                    //        player->setShield();
                    //    }
                    criticalLevel = GameManager::getInstance()->getWeaponCriticalLevel(weapon);
                    
                    
                    if(playerType == CHARACTER_SUSAN){
                        player->waterAccelX = 1000;
                        player->waterMaxX = 100;
                    }
                    GameManager::getInstance()->getHudLayer()->setLife(player->energy);
                    setPlayerInfo();
                    /*setWeaponInfo(weapon);
                     totalBulletCountMax = getMaxLoadedBulletCount(weapon);
                     totalBulletCount = totalBulletCountMax;
                     bulletCount = 0;
                     bulletCountMax = getMaxBulletCount(weapon);
                     GameManager::getInstance()->getHudLayer()->setWeapon(weapon);
                     GameManager::getInstance()->getHudLayer()->setBulletCount(bulletCount,bulletCountMax);
                     GameManager::getInstance()->getHudLayer()->setTotalBulletCount(totalBulletCount, totalBulletCountMax);
                     reload();
                     */
                    if (GM->currentStageIndex == STAGE_LOBBY){
                        GameManager::getInstance()->isWeaponSet = false;
                    }
                    
                    int runeValue;
                    int effectHPAmount = 0;
                    int effectDefAmount = 0;

//                    if (!GameManager::getInstance()->isWeaponSet) {
                    if(true){
                        if (GM->currentStageIndex != STAGE_LOBBY){
                            GameManager::getInstance()->isWeaponSet = true;
                        }
                        log("stage: %d", GM->currentStageIndex);
                        if (GM->currentStageIndex == STAGE_LOBBY || (GM->currentStageIndex == 0 && !isBossMap) || (GM->currentStageIndex <= STAGE_DAILY_DUNGEON_0 && GM->currentStageIndex >= STAGE_BOSS_DUNGEON)) {
                            currentSlot = 0;
                            setWeaponAtSlot(0, -1);
                            setTotalBulletCountAtSlot(currentSlot, getMaxTotalBulletCount(WEAPON_PISTOL));
                            setLoadedBulletCountAtSlot(currentSlot, getMaxLoadedBulletCount(WEAPON_PISTOL));
                            addWeapon(WEAPON_PISTOL);
                            GM->collectedKeyCount = 0;
                            setWeaponsFromEquiped();
                        }else{
                            player->energy = GM->playerEnergy;
                            player->maxEnergy = GM->playerEnergyMax;
                            int index = 0;
                            for (auto shieldIndex : GM->playerShieldIndices) {
                                int value = shieldIndex;
                                Droppable* drop = Droppable::createDroppable(value, GM->getShieldHPRuneApplied(value), 0, strmake("shield/shield%d", (value%1000)%600).c_str());
                                drop->setScale(0.5f);
                                drop->DP = GM->getShieldDefRuneApplied(value);
                                playerShields.pushBack(drop);
                                this->addChild(drop, player->getLocalZOrder() + 1);
                                drop->energy = GM->playerShieldHps.at(index);
                                index++;
                            }
                            HUD->setLife(player->energy);
                            
                        }
//                        Shield* shield = Shield::create();
//                        shield->fromString(item);
//                        spriteBatch->addChild(shield);
//                        playerShields.pushBack(shield);
//                        //                shield->setPosition(player->getPosition());
//                        //                shield->runAction(EaseOut::create(MoveTo::create(dur, player->getPosition() + Point(itemGapX*(shieldIndex + 1), shieldIndex%2==0?-itemGapY:itemGapY)), 2));
//                        playerHealth += shield->energy;
//                        shieldIndex++;
                        
                        
//                        addWeapon(UserDefault::getInstance()->getIntegerForKey(StringUtils::format(KEY_BONUS_WEAPON_FORMAT, 0).c_str(), WEAPON_PISTOL+1));
//                        addWeapon(UserDefault::getInstance()->getIntegerForKey(StringUtils::format(KEY_BONUS_WEAPON_FORMAT, 1).c_str(), WEAPON_PISTOL+2));
//                        addWeapon(UserDefault::getInstance()->getIntegerForKey(StringUtils::format(KEY_BONUS_WEAPON_FORMAT, 2).c_str(), WEAPON_PISTOL+3));
//                        addWeapon(WEAPON_UZI);
//                        addWeapon(WEAPON_LIGHTNING_GUN);
//                        GameManager::getInstance()->collectedKeyCount = 1; // test
                    }
                    
                    
                    int slotIndex = GM->lastWeaponSlot;
                    while(true){
                        if (getWeaponAtSlot(slotIndex) < 0) {
                            break;
                        }
                        
                        if (getTotalBulletCountAtSlot(slotIndex) > 0) {
                            changeWeapon(slotIndex);
                            break;
                        }
                        
                        slotIndex++;
                    }
                    log("%d", GM->lastWeaponSlot);
                    changeWeapon(GM->lastWeaponSlot);
                    
                    GameManager::getInstance()->getHudLayer()->initializeSreen(eng);
                    
                    if(GameManager::getInstance()->isVideoRewardEnergy){
                        GameManager::getInstance()->getHudLayer()->addEnergyEffect();
                    }
                    furtherBack = NULL;
                    farBack = NULL;
                    nearBack = NULL;
                    
                    
                    if(GameManager::getInstance()->theme == 3){
                        furtherWidth = 399;//625;
                        furtherHeight = 207; //293;
                        farWidth = 437;//759;
                        farHeight = 247;//257;
                        nearWidth = 490;//838;
                        nearHeight = 290;//284;
                    }else{
                        furtherWidth = 600;//625;
                        furtherHeight = 200; //293;
                        farWidth = 800;//759;
                        farHeight = 300;//257;
                        nearWidth = 1200;//838;
                        nearHeight = 600;//284;
                    }
                    
                    
                    //    float nextX = 0;
                    //    float wallY = -40;
                    //    float wallHeight = 143;
                    //    Sprite* spt;
                    
                    char buf[80];
                    
                    Value stageMap;
                    int tutorialStages = 3;
                    
                    int theTheme = GameManager::getInstance()->theme+1;
                    sprintf( buf, "stages/ldd_stage%d", 0);
                    stageMap = (buf);
                    
                    
                    sprintf( buf, "%s.tmx", stageMap.asString().c_str());
                    
                    CCLOG("Map name: %s", buf);
                    
                    //    tileMap = cocos2d::TMXTiledMap::create(buf);
                    
                    //    mapHight = tileMap->getMapSize().height * TILE_SIZE;
                    //    mapRowCount = tileMap->getMapSize().height;
                    //    mapColumnCount = tileMap->getMapSize().width;
                    
                    /*
                     tileMap = cocos2d::experimental::TMXTiledMap::create(buf);
                     
                     #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
                     tileMap = cocos2d::experimental::TMXTiledMap::create(buf);
                     #else
                     tileMap = cocos2d::TMXTiledMap::create(buf);
                     #endif
                     */
                    spriteBatchForHero->addChild(player, 10);
                    
                    //    tileMap->retain();
                    this->setAnchorPoint(Point(0,0));
                    this->setScale(4);
                    //    this->addChild(tileMap, -2);
                    this->addChild(spriteBatchForHero, unitLayer->getLocalZOrder() + 1);
                    this->addChild(spriteBatch, unitLayer->getLocalZOrder() + 1);
                    //    blackSquare = Sprite::create("blackSquare.png");
                    //    this->addChild(blackSquare, 1);
                    
                    GameManager::getInstance()->firstPlayed = false;
                    
                    isGameOver = false;
                    keyCollected = false;
                    
                    backLayer->setLocalZOrder(-20);
                    decoLayer->setLocalZOrder(-10);
                    
                    
                    spriteBatch->setLocalZOrder(unitLayer->getLocalZOrder());
                    stageLayer->setLocalZOrder(unitLayer->getLocalZOrder() - 1);
                    
                    if (foreLayer) {
                        foreLayer->setLocalZOrder(unitLayer->getLocalZOrder() + 1);
                    }
                    
                    // Mastery setting
                    mastery = GameManager::getInstance()->getMastery(GameManager::getInstance()->theme*24 + GameManager::getInstance()->currentStageIndex);
                    mastery++;
                    if (mastery > 7) {
                        mastery = 0;
                    }
                    
                    player->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j-1)*TILE_SIZE) + Point(TILE_SIZE/2, 0));
                    playerSpawnPoint = tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE) + Point(TILE_SIZE/2, -TILE_SIZE/2 - 3);
                    player->setOpacity(0);
                    player->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.1), NULL));
                    player->face->setOpacity(0);
                    player->face->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.1), NULL));
                    player->hand->setOpacity(0);
                    player->hand->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.1), NULL));
                    
                    
//                    collectedCoinCount = 2000; // test
//                    GameManager::getInstance()->setCoin(collectedCoinCount); // test
//                    GameManager::getInstance()->getHudLayer()->updateCoin(collectedCoinCount); // test
                    if (stageIndex != STAGE_ENTRANCE) {
                        
                        showWarpAnimation();
                    }
                    
                    
                    if (GameManager::getInstance()->currentPet >= 0) {
                        Buddy* skyBuddy = GameManager::getInstance()->getPet(GameManager::getInstance()->currentPet);
                        skyBuddy->setPosition(this->player->getPosition() + Point(10, 0));
                        this->spriteBatch->addChild(skyBuddy, 9);
                        skyBuddy->setFlippedX(false);
                        this->buddyArray.pushBack(skyBuddy);
                        this->skyBuddy = skyBuddy;
                    }
                    
                    if (stageIndex == STAGE_ENTRANCE) {
                        GameManager::getInstance()->getHudLayer()->setVisible(false);
                        isCameraInCustomMoving = true;
                        player->extraSpeed = 10;
                        setViewPointCenter(player->getPosition() + Point(16*9, 0));
//                        this->runAction(Sequence::create(DelayTime::create(3), CallFunc::create(CC_CALLBACK_0(HelloWorld::releaseCustomMovingCamera, this)), nullptr));
                    }
                    
                    if (stageIndex == STAGE_LOBBY) {
                        ((TutorialGhost*)this->getChildByName("tutorialGhost"))->setTarget(player);
                    }
                }else if(gid == 17){// Walkable
                    if (true) {
                        enemy = FireableOnPlatformBase::create(UNIT_PIERROT_DUMB, 70, 50, "pierrotDog0.png", 1, 10, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_CROSS_PLATFORM, 100);
                        enemy->runAnimation("pierrotDog", true);
                    }else{
                        enemy = FireableOnPlatformBase::create(UNIT_PIERROT_DUMB, 30, 0, "pierrot0.png", 1, 1, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_CROSS_PLATFORM, 100);
                        enemy->runAnimation("pierrot", true);
                    }
                    enemy->setTarget(player);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(enemy);
                    droppableArray.pushBack(enemy);
                }else if(gid == 18){// Walkable Fireable
                    int moveTypeBeforeFindTarget = ENEMY_MOVE_WANDER_ON_A_PLATFORM;
                    int moveTypeAfterFindTarget = ENEMY_MOVE_TO_CLIFF;
                    int gunType = ENEMY_GUN_TYPE_HANDGUN;
                    int bulletCount = 1;
                    float reloadingTime = 3;
                    int bulletSpeed = 100;
                    if (true) {
                        fEnemy = EnemyWithGun::create(UNIT_PIERROT_DUMB, 40, 0, "pierrotCrossEye0.png", 5, 10, moveTypeBeforeFindTarget, moveTypeAfterFindTarget, 120);
                        fEnemy->runAnimation("pierrotCrossEye", true);
                    }else{
                        fEnemy = EnemyWithGun::create(UNIT_PIERROT_DUMB, 20, 0, "pierrot0.png", 5, 10, moveTypeBeforeFindTarget, moveTypeAfterFindTarget, 120);
                        fEnemy->runAnimation("pierrot", true);
                    }
                    fEnemy->setTarget(player);
                    ((EnemyWithGun*)fEnemy)->setGunInfo(bulletCount, reloadingTime, gunType, bulletSpeed);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                }else if(gid == 234234){
                    Droppable* door = Droppable::createDroppable(UNIT_DOOR, 1, 0, "bigDoor.png");
                    door->centerPosition = Point(0, -12);
                    door->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE + 16));
                    theDoor = door;
                    spriteBatch->addChild(door);
                    droppableCoinArray.pushBack(door);
                    indicatedArray.pushBack(door);
                }else if(gid == 234234){
                    Droppable* door = Droppable::createDroppable(UNIT_DOOR, 1, 0, "door.png");
                    door->centerPosition = Point(0, -12);
                    door->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE + 16));
                    theDoor = door;
                    spriteBatch->addChild(door);
                    droppableCoinArray.pushBack(door);
                    indicatedArray.pushBack(door);
                }else if (gid == 52){   // start enemy create
                    fEnemy = addEnemyToLoadStack(tileMap, currentLevelScore, Point(i, j), MISSILE_SINGLE, UNIT_PIERROT_NORMAL, false);
                }else if (gid == 53){   // suitcase
                    int suitCaseType = suitcaseCount;
//                    if (stageIndex > 2 && suitcaseCount > 0) {
//                        suitCaseType = 2 + rand()%2;
//                    }
                    int clearCount = UDGetInt(KEY_CLEAR_COUNT, 0);
                    if (clearCount == 0) {
                        if (stageIndex == 0) {
                            suitCaseType = 0;
                        }else if (stageIndex == 1 || stageIndex == 2) {
                            suitCaseType = 0;
                        }else{
                            suitCaseType = rand()%2;
                        }
                    }else if (clearCount == 1) {
                        if (stageIndex == 0) {
                            if (suitcaseCount == 0) {
                                suitCaseType = 0;
                            }else{
                                suitCaseType = 1;
                            }
                        }else if (stageIndex == 1 || stageIndex == 2) {
                            if (suitcaseCount == 0) {
                                suitCaseType = 0;
                            }else if (suitcaseCount == 1) {
                                suitCaseType = 1;
                            }else{
                                suitCaseType = 0;
                            }
                        }else{
                            suitCaseType = 1 + rand()%2;
                        }
                    }else if (clearCount == 2) {
                        if (stageIndex == 0) {
                            if (suitcaseCount == 0) {
                                suitCaseType = 0;
                            }else{
                                suitCaseType = 1;
                            }
                        }else if (stageIndex == 1 || stageIndex == 2) {
                            if (suitcaseCount == 0) {
                                suitCaseType = 0;
                            }else if (suitcaseCount == 1) {
                                suitCaseType = 1;
                            }else{
                                suitCaseType = 1;
                            }
                        }else{
                            if (rand()%100 < 20) {
                                suitCaseType = 3;
                            }else{
                                suitCaseType = 2;
                            }
                        }
                    }else if (clearCount == 3) {
                        if (stageIndex == 0) {
                            if (suitcaseCount == 0) {
                                suitCaseType = 0;
                            }else{
                                suitCaseType = 1;
                            }
                        }else if (stageIndex == 1 || stageIndex == 2) {
                            if (suitcaseCount == 0) {
                                suitCaseType = 0;
                            }else if (suitcaseCount == 1) {
                                suitCaseType = 1;
                            }else{
                                suitCaseType = 2;
                            }
                        }else{
                            if (rand()%100 < 40) {
                                suitCaseType = 3;
                            }else{
                                suitCaseType = 2;
                            }
                        }
                    }
                    if (suitcaseCount > 3) {
                        suitcaseCount = 3;
                    }

                    Droppable* suitcase = Droppable::createDroppable(UNIT_SUITCASE, 1, 0, __String::createWithFormat("suitCase%d_0.png", suitCaseType)->getCString());
                    suitcase->secondTag = suitCaseType;
                    spriteBatch->addChild(suitcase);
                    coinArray.pushBack(suitcase);
                    GameManager::getInstance()->makeItSiluk(suitcase);
                    suitcase->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2 + 5));
                    suitcase->runAction(RepeatForever::create(Sequence::create(EaseOut::create(MoveBy::create(1, Point(0, 3)), 2), EaseOut::create(MoveBy::create(1, Point(0, -3)), 2), NULL)));
                    suitcaseCount++;
                }else if (gid == 61){   // jewelry box
                    bool isJewelVisible = false;
                    bool isAllClear = UserDefault::getInstance()->getIntegerForKey(KEY_CLEAR_COUNT, 0) > 0;
//                    isAllClear = true; // test
                    if (isAllClear) {
                        bool jewelCollected0 = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 0)->getCString());
                        bool jewelCollected1 = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 1)->getCString());
                        bool jewelCollected2 = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 2)->getCString());
                        bool jewelCollected3 = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString());
                        if (stageIndex == 0 && !jewelCollected0) {
                            isJewelVisible = true;
                        }else if (stageIndex == 1 && jewelCollected0 && !jewelCollected1) {
                            isJewelVisible = true;
                        }else if (stageIndex == 2 && jewelCollected1 && !jewelCollected2) {
                            isJewelVisible = true;
                        }else if (stageIndex == 3 && jewelCollected2 && !jewelCollected3) {
                            isJewelVisible = true;
                        }
                    }
                    if (GameManager::getInstance()->jewelryCollected) {
                        isJewelVisible = false;
                    }
                    
                    if (isJewelVisible) {
                        EnemyBase* jewelryBox = EnemyBase::createEnemy(UNIT_JEWELRY_BOX, 1, "jewelryBox.png", 0, 0);
                        jewelryBox->secondTag = -1;
                        spriteBatch->addChild(jewelryBox);
                        coinArray.pushBack(jewelryBox);
                        GameManager::getInstance()->makeItSiluk(jewelryBox);
                        jewelryBox->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2 + 5));
                        jewelryBox->runAction(RepeatForever::create(Sequence::create(EaseOut::create(MoveBy::create(1, Point(0, 3)), 2), EaseOut::create(MoveBy::create(1, Point(0, -3)), 2), NULL)));
//                        destructableArray.pushBack(jewelryBox);
                    }
                }else if (gid == 54){
                    Droppable* coinSprite = Droppable::createDroppable(UNIT_KEY, 1, 0, "blueKey.png");
                    spriteBatch->addChild(coinSprite);
                    coinArray.pushBack(coinSprite);
                    coinSprite->setScale(0.8);
                    GameManager::getInstance()->makeItSiluk(coinSprite);
                    coinSprite->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2));
                }else if (gid == 55){
                    HoodGhost* ghost = HoodGhost::create();
                    ghost->setTarget(player);
                    spriteBatch->addChild(ghost);
                    ghost->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j-1)*TILE_SIZE) + Point(TILE_SIZE/2, TILE_SIZE/2));
                    ghost->runAnimation("hoodGhost", true);
                    
                    if (stageIndex >= 0) {
                        fEnemy = ghost;
                        enemyArray.pushBack(fEnemy);
                        flyingSwimingEnemyArray.pushBack(fEnemy);
                        fireableArray.pushBack(fEnemy);
                    }else{
                        npcArray.pushBack(ghost);
                    }
                }else if (gid == 56){
                    theKnight = TheKnight::create();
                    this->addChild(theKnight, spriteBatch->getLocalZOrder());
                    npcArray.pushBack(theKnight);
                    theKnight->setTag(UNIT_THE_KNIGHT);
                    theKnight->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j-2)*TILE_SIZE - TILE_SIZE/2 - 1));
                }else if (gid == 57){
                    Vec2 gatePos = tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE) + Point(TILE_SIZE/2, -TILE_SIZE/2 - 3);
                    int x = gatePos.x;
                    bool isGateClosed = false;
                    if(stageIndex == STAGE_LOBBY){
                        if(x == 80){
                            if(!GM->isTutorialOn || GM->tutorialType == TUTORIAL_MENU){
                                isGateClosed = true;
                            }
                        }else{
                            
                        }
                    }
//                    isGateClosed = false; // test 
                    if (!isGateClosed) {
                        NPCBase* gate = NPCBase::create("activeZone0.png");
                        spriteBatch->addChild(gate);
                        npcArray.pushBack(gate);
                        gate->runAnimation("warp", true);
                        gate->setTag(UNIT_GATE);
                        gate->setPosition(gatePos);
                        
                        gate->secondTag = stageIndex + 1;// test
                        if (stageIndex == TUTORIAL_STAGE_START) {
                            if (gate->getPosition().x < 80) {
                                gate->secondTag = TUTORIAL_STAGE_END + 1;
                                gate->setOpacity(0);
                            }
                        }
                    }
                }else if (gid == 58){
                    NPCBase* gate = NPCBase::create("activeZoneRed0.png");
                    spriteBatch->addChild(gate);
                    npcArray.pushBack(gate);
                    gate->runAnimation("warpBoss", true);
                    gate->setTag(UNIT_GATE);
                    gate->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE) + Point(TILE_SIZE/2, -TILE_SIZE/2 - 3));
                    gate->secondTag = stageIndex;
                    
                    Droppable* sptRedFace = Droppable::createDroppable(0, 0, 0, "redFace0.png");
                    sptRedFace->runAnimation("redFace", true);
                    spriteBatch->addChild(sptRedFace);
                    sptRedFace->setPosition(gate->getPosition() + Point(0, 34));
                }else if (gid == 59){
                    Seller* gate = Seller::create();
                    spriteBatch->addChild(gate);
                    npcArray.pushBack(gate);
                    gate->runAnimation("seller", true);
                    gate->setTag(UNIT_NPC);
                    gate->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j-1)*TILE_SIZE + gate->getContentSize().height/2));
                }else if (gid == 60){
                    int itemIndex = 0;
                    if (itemCount == 0) {
                        itemIndex = SELLING_ITEM_KEY;
                    }else if (itemCount == 1) {
                        itemIndex = SELLING_ITEM_HEART;
                    }else if (itemCount == 2) {
                        itemIndex = SELLING_ITEM_AMMO;
                    }else if (itemCount == 3) {
                        itemIndex = SELLING_ITEM_DRAGON_BABY - stageIndex;
                        if (itemIndex < SELLING_ITEM_DRAGON_SENIOR) {
                            itemIndex = SELLING_ITEM_DRAGON_SENIOR;
                        }
                    }else if (itemCount >= 4) { // weapon
                        int var = WEAPON_NO_MORE/4.0f;
                        
                        int start = var*stageIndex;
                        int end = start + var - 1;
                        if (start == 0) {
                            start = 1;
                        }
                        if (stageIndex == 3) {
                            end = WEAPON_NO_MORE - 1;
                        }
                        while(true){
                            itemIndex = rand()%(end - start + 1) + start;
                            bool exist = false;
                            for(int j = 0; j < shopWeaponItem.size();j++){
                                if (shopWeaponItem.at(j) == itemIndex) {
                                    exist = true;
                                }
                            }
                            if (!exist) {
                                break;
                            }
                        }
                        shopWeaponItem.push_back(itemIndex);
                        
                        log("stage %d, item: %d, start: %d, end: %d", stageIndex, itemIndex, start, end);
                    }
                    OnSale* gate = OnSale::create(itemIndex);
                    this->addChild(gate);
                    npcArray.pushBack(gate);
                    gate->setTag(UNIT_ONSALE);
                    gate->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j-1)*TILE_SIZE + gate->getContentSize().height/2));
                    itemCount++;
                }else if (gid == 88){   // cross
                    fEnemy = addEnemyToLoadStack(tileMap, currentLevelScore, Point(i, j), MISSILE_ARC, UNIT_PIERROT_CROSS, false);
                }else if (gid == 89){   // dog
                    fEnemy = addEnemyToLoadStack(tileMap, currentLevelScore, Point(i, j), -1, UNIT_PIERROT_DOG, false);
                }else if (gid == 90){   // ghost
                    fEnemy = addEnemyToLoadStack(tileMap, currentLevelScore, Point(i, j), MISSILE_ARC, UNIT_PIERROT_GHOST, false);
                }else if (gid == 91){   // scot
                    fEnemy = addEnemyToLoadStack(tileMap, currentLevelScore, Point(i, j), MISSILE_CIRCLE, UNIT_PIERROT_SCOT, false);
                }else if (gid == 113){   // running hud
                    fEnemy = addEnemyToLoadStack(tileMap, currentLevelScore, Point(i, j), MISSILE_CIRCLE, UNIT_RUNNING_HUD, false);
                }else if (gid == 92){   // vertical
                    fEnemy = addEnemyToLoadStack(tileMap, currentLevelScore, Point(i, j), MISSILE_COLUMN, UNIT_PIERROT_VERTIAL_EYE, false);
                }else if (gid == 93){   // tall
                    fEnemy = addEnemyToLoadStack(tileMap, currentLevelScore, Point(i, j), MISSILE_SPIRAL, UNIT_PIERROT_TALL, false);
                }else if (gid == 94){   // alphabet
                    std::string strChar;
                    int extraY = 0;
                    if (alphabetCount == 25) {
                        strChar = "*";
                    }else if (alphabetCount == 29) {
                        strChar = "-";
                    }else if (alphabetCount == 33) {
                        strChar = "_";
                        extraY = 3;
                    }else if (alphabetCount == 37) {
                        strChar = "<-";
                    }else if (alphabetCount%4 - 3 == 0) {
                        strChar = (char)((int)'a' + (alphabetCount - 3)/4);
                    }else if (alphabetCount%4 - 2 == 0) {
                        strChar = (char)((int)'k' + (alphabetCount - 2)/4);
                    }else if (alphabetCount%4 - 1 == 0) {
                        strChar = (char)((int)'u' + (alphabetCount - 1)/4);
                    }else if (alphabetCount%4 - 0 == 0) {
                        strChar = (char)((int)'0' + (alphabetCount - 0)/4);
                    }
                    Alphabet* alphabet = Alphabet::create(UNIT_KEYBOARD_KEY,"keyboardKey.png");
                    destructableArray.pushBack(alphabet);
                    spriteBatch->addChild(alphabet);
                    alphabet->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2));
                    alphabet->initLabel(strChar, 13, Point(0, 3 + extraY));
                    
                    alphabetCount++;
                }else if (gid == 234234){
                    fEnemy = HotDogGun::create(UNIT_HOTDOG_GUN, 70 + (stageIndex + themeIndex*20)*2, 0, "hotDogGun0.png", 4, 10, ENEMY_MOVE_JUMP_STAND, ENEMY_MOVE_JUMP_STAND, 200);
                    fEnemy->setTarget(player);
                    ((HotDogGun*)fEnemy)->aniName = "hotDogGunFire";
                    fEnemy->state = STATE_JUMPING;
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = HotDogGun::create(UNIT_OCTOPUS, 120 + (stageIndex + themeIndex*20)*5, 0, "octopus0.png", 4, 10, ENEMY_MOVE_JUMP_STAND, ENEMY_MOVE_JUMP_STAND, 200);
                    fEnemy->setTarget(player);
                    ((HotDogGun*)fEnemy)->aniName = "octopusFire";
                    fEnemy->state = STATE_JUMPING;
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    enemy = Frog::create(UNIT_FROG, 70 + stageIndex, 50 + (stageIndex + themeIndex*20)*2, "silverCoin0.png", 4, 10, ENEMY_MOVE_JUMP_STAND, ENEMY_MOVE_JUMP_STAND, 70);
                    enemy->setTarget(player);
                    enemy->state = STATE_JUMPING;
                    
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(enemy);
                    droppableArray.pushBack(enemy);
                }else if (gid == 234234){
                    RandomBox* box = RandomBox::create(UNIT_RANDOM_BOX, 70 + (stageIndex + themeIndex*20)*2, 0, "woodBox.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 100);
                    box->state = STATE_STANDING;
                    spriteBatch->addChild(box);
                    box->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    destructableArray.pushBack(box);
                    droppableArray.pushBack(box);
                    
                    Sprite* sptQuestionMark = Sprite::createWithSpriteFrameName("questionMark.png");
                    box->addChild(sptQuestionMark);
                    sptQuestionMark->setPosition(Point(box->getContentSize().width/2, 28));
                    float duration = 0.4f;
                    sptQuestionMark->runAction(RepeatForever::create(Sequence::create(MoveBy::create(duration, Point(0, -4)), MoveBy::create(duration, Point(0, 4)), NULL)));
                }else if (gid == 234234){
                    fEnemy = AppleMon::create(UNIT_APPLEMON, 120 + (stageIndex + themeIndex*20)*2, 0, "appleMon0.png", 4, 10, ENEMY_MOVE_CUSTOM, ENEMY_MOVE_JUMP_STAND, 100);
                    fEnemy->setTarget(player);
                    fEnemy->state = STATE_WALKING;
                    spriteBatch->addChild(fEnemy);
                    ((AppleMon*)fEnemy)->setPositionForWander(Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = FlyingPenguin::create(UNIT_FLYING_PENGUIN, 90 + (stageIndex + themeIndex*20)*3, 0, "flyingPenguin0.png", 0, 10, ENEMY_MOVE_CUSTOM, ENEMY_MOVE_FLY_WANDER_STOP, 100);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    ((AppleMon*)fEnemy)->setPositionForWander(Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = SquirrelStand::create(UNIT_SQUIRREL, 50 + (stageIndex + themeIndex*20)*2, 0, "squirrel0.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 300);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + 8, (totalHeight-j)*TILE_SIZE) + Point(TILE_SIZE/2, TILE_SIZE/2));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = Pumpkin::create(UNIT_PUMPKIN, 150 + (stageIndex + themeIndex*20)*5, 0, "pumpkin0.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 200);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = DungBeetle::create(UNIT_DUNG_BEETLE, 200 + (stageIndex + themeIndex*20)*7, -10, "dungBeetleRolling0.png", 4, 10, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_ON_A_PLATFORM, 100);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = Wood::create(UNIT_WOOD, 100 + (stageIndex + themeIndex*20), 0, "wood0.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 300);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    enemy = PinkRakoon::create(UNIT_PINK_RAKOON, 140 + (stageIndex + themeIndex*20)*3, 49, "pinkRakoon.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_WANDER_CROSS_PLATFORM, 100);
                    enemy->setTarget(player);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(enemy);
                    droppableArray.pushBack(enemy);
                }else if (gid == 234234){
                    fEnemy = DrumBomb::create(UNIT_DRUM_BOMB, 80 + (stageIndex + themeIndex*20)*1, 0, "drumBomb0.png", 4, 10, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_CROSS_PLATFORM, 30);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = ForkPorkStand::create(UNIT_FORKPORK_STAND, 160 + (stageIndex + themeIndex*20)*5, 0, "forkPork0.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 300);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE) + Point(TILE_SIZE/2, TILE_SIZE/2));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = SkeletonWizard::create(UNIT_SKELETON_WIZARD, 120 + (stageIndex + themeIndex*20)*6, -30, "skeletonWizard0.png", 4, 10, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_STAND, 100);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = BoardOwl::create(UNIT_BOARD_OWL, 90 + (stageIndex + themeIndex*20)*4, 0, "boardOwl0.png", 4, 10, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_ON_A_PLATFORM, 100);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = GreenWorm::create(UNIT_GREEB_WORM, 40 + (stageIndex + themeIndex*20)*2, 10, "greenWorm0.png", 4, 10, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_ON_A_PLATFORM, 100);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = BluePot::create(UNIT_POT, 1000 + (stageIndex + themeIndex*20)*5, -20, "bluePot0.png", 4, 10, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_ON_A_PLATFORM, 100);
                    fEnemy->setTarget(player);
                    fEnemy->setScale(2);
                    fEnemy->setAnchorPoint(Point(0.5, 0.25));
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    FishMon* fish = FishMon::create(UNIT_FISH_MON, 70 + (stageIndex + themeIndex*20)*3, -30, "fishMon.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 100);
                    fish-> setTarget(player);
                    fish->untouchable = true;
                    spriteBatch->addChild(fish);
                    fish->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE) + Point(TILE_SIZE/2, TILE_SIZE/2));
                    enemyArray.pushBack(fish);
                    droppableArray.pushBack(fish);
                    fireableArray.pushBack(fish);
                }else if (gid == 234234){
                    fEnemy = Ghost::create(UNIT_GHOST, 100 + (stageIndex + themeIndex*20)*5, -10, "ghost0.png", 4, 10, ENEMY_MOVE_HANG, ENEMY_MOVE_FLY_TO_HERO_FOLLOW, 100);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE) + Point(TILE_SIZE/2, TILE_SIZE/2));
                    enemyArray.pushBack(fEnemy);
                    flyingSwimingEnemyArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    enemy = Bat::create(UNIT_BAT, 25 + (stageIndex + themeIndex*20)*1, -12, "bat1.png", 5, 10, ENEMY_MOVE_HANG, ENEMY_MOVE_FLY_TO_HERO_FOLLOW, 200);
                    enemy->setTarget(player);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(enemy);
                    flyingSwimingEnemyArray.pushBack(enemy);
                }else if (gid == 234234){
                    fEnemy = OddEyeRide::create(UNIT_ODDEYERIDE, 100 + (stageIndex + themeIndex*20)*4, -20, "oddEyeRide0.png", 4, 10, ENEMY_MOVE_HANG, ENEMY_MOVE_SWIM_TO_HERO_FOLLOW, 130);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    flyingSwimingEnemyArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    enemy = OddEye::create(UNIT_ODDEYE, 50 + (stageIndex + themeIndex*20)*3, -30, "oddEye0.png", 4, 10, ENEMY_MOVE_HANG, ENEMY_MOVE_SWIM_TO_HERO_FOLLOW, 100);
                    enemy->setTarget(player);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(enemy);
                    flyingSwimingEnemyArray.pushBack(enemy);
                    enemy->runAnimation("oddEye", true);
                }else if (gid == 234234){
                    enemy = OddEye::create(UNIT_JELLYFISH, 20 + (stageIndex + themeIndex*20)*1, -35, "jellyFish0.png", 4, 10, ENEMY_MOVE_HANG, ENEMY_MOVE_SWIM_TO_HERO_FOLLOW, 100);
                    enemy->setTarget(player);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(enemy);
                    flyingSwimingEnemyArray.pushBack(enemy);
                    enemy->runAnimation("jellyFish", true);
                }else if (gid == 234234){
                    enemy = BlueWorm::create(UNIT_BLUE_WORM, 22 + (stageIndex + themeIndex*20)*1, -10, "blueWorm0.png", 4, 10, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_ON_A_PLATFORM, 100);
                    enemy->setTarget(player);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(enemy);
                    droppableArray.pushBack(enemy);
                }else if (gid == 234234){
                    enemy = LeapWorm::create(UNIT_LEAP_WORM, 42 + (stageIndex + themeIndex*20)*2, -15, "leapWorm0.png", 4, 10, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_ON_A_PLATFORM, 100);
                    enemy->setTarget(player);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(enemy);
                    droppableArray.pushBack(enemy);
                }else if (gid == 234234){
                    fEnemy = TreeMan::create(UNIT_TREE_MAN, 92 + (stageIndex + themeIndex*20)*4, -10, "treeMan0.png", 4, 8, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_ON_A_PLATFORM, 100);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = ThreeEyes::create(UNIT_THREE_EYES, 252 + (stageIndex + themeIndex*20)*7, -10, "threeEye0.png", 4, 8, ENEMY_MOVE_CUSTOM, ENEMY_MOVE_CUSTOM, 100);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    enemy = WalkingPenguin::create(UNIT_WALKING_PENGUIN, 42 + (stageIndex + themeIndex*20)*2, 5, "walkingPenguin0.png", 4, 10, ENEMY_MOVE_WANDER_ON_A_PLATFORM, ENEMY_MOVE_WANDER_ON_A_PLATFORM, 100);
                    enemy->setTarget(player);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(enemy);
                    droppableArray.pushBack(enemy);
                }else if (gid == 234234){
                    fEnemy = Squirrel::create(UNIT_SQUIRREL, 50 + (stageIndex + themeIndex*20)*2, 0, "squirrel0.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 80);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = ForkPork::create(UNIT_FORKPORK_STAND, 140 + (stageIndex + themeIndex*20)*7, 0, "forkPork0.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 100);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                    enemyArray.pushBack(fEnemy);
                    droppableArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    fEnemy = DJMonkey::create(UNIT_FORKPORK_STAND, 170 + (stageIndex + themeIndex*20)*3, 0, "djMonkey0.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 200);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE) + Point(TILE_SIZE/2, TILE_SIZE/2));
                    enemyArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                    float duration = 1.2f;
                    int x = 0;
                    int y = 5;
                    JumpBy* left0 = JumpBy::create(duration/2, Point(-x, 0), y, 1);
                    JumpBy* left1 = JumpBy::create(duration/2, Point(-x, 0), -y, 1);
                    fEnemy->runAction(RepeatForever::create(Sequence::create(left0, left1, NULL)));
                }else if (gid == 234234){
                    fEnemy = BeeHive::create(UNIT_BEE_HIVE, 70 + (stageIndex + themeIndex*20)*3, 0, "beeHive.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 300);
                    fEnemy->setTarget(player);
                    spriteBatch->addChild(fEnemy);
                    fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - 8));
                    enemyArray.pushBack(fEnemy);
                    fireableArray.pushBack(fEnemy);
                }else if (gid == 234234){
                    enemy = EnemyBase::createEnemy(UNIT_SHORT_THORN, 0, 0, "thornBottomTheme3.png",0, 50, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 0);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2));
                    enemyArray.pushBack(enemy);
                    enemy->desiredPosition = enemy->getPosition();
                    enemy->untouchable = true;
                }else if (gid == 234234){
                    enemy = EnemyBase::createEnemy(UNIT_SHORT_THORN, 0, 0, "thornBottom2Theme3.png",0, 50, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 0);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2-3));
                    enemyArray.pushBack(enemy);
                    enemy->desiredPosition = enemy->getPosition();
                    enemy->untouchable = true;
                }else if (gid == 234234){
                    enemy = EnemyBase::createEnemy(UNIT_SHORT_THORN, 0, 0, "shortThorn.png",0, 50, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 0);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2-4));
                    enemyArray.pushBack(enemy);
                    enemy->desiredPosition = enemy->getPosition();
                    enemy->untouchable = true;
                }else if (gid == 81){
                    enemy = EnemyBase::createEnemy(UNIT_RED_THORN_BOTTOM, 0, 0,"redThorn.png", 0, 50, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 0);
                    spriteBatch->addChild(enemy);
                    
                    if (stageLayer->getTileGIDAt(point + Point(0, 1))) {//bottom tile exist
                        enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2-4));
                    }else if (stageLayer->getTileGIDAt(point + Point(1, 0))) {//right tile exist
                        enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE-4, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2));
                        enemy->setRotation(-90);
                    }else if (stageLayer->getTileGIDAt(point + Point(0, -1))) {//top tile exist
                        enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - 4));
                        enemy->setRotation(180);
                    }else if (stageLayer->getTileGIDAt(point + Point(-1, 0))) {//left tile exist
                        enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2 -5, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2));
                        enemy->setRotation(90);
                    }
                    
                    enemy->secondTag = UNIT_TRAP_KIND_OF;
                    enemyArray.pushBack(enemy);
                    enemy->untouchable = true;
                }else if (gid == 82){
                    enemy = EnemyBase::createEnemy(UNIT_ELECTRIC_BALL, 0, 0, "pyochang.png",0, 50, ENEMY_MOVE_CUSTOM, ENEMY_MOVE_CUSTOM, 0);
                    spriteBatch->addChild(enemy);
                    float distanceToMove = 80;
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2 - distanceToMove/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2-4));
                    float timeToMove = (rand()%5)*0.1f + 0.7f;
                    enemy->runAction(RepeatForever::create(RotateBy::create(0.5, 360)));
                    enemy->runAction(RepeatForever::create(Sequence::create(MoveBy::create(timeToMove, Point(distanceToMove, 0)), MoveBy::create(timeToMove, Point(-distanceToMove, 0)), NULL)));
                    enemy->secondTag = UNIT_TRAP_KIND_OF;
                    enemyArray.pushBack(enemy);
                    enemy->untouchable = true;
                }else if (gid == 234234){
                    Mace* mace = Mace::createMace("maceString.png", "maceHead.png", 0, Point(0.5, 1), Point(2, 0));
                    spriteBatch->addChild(mace);
                    mace->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE));
                    maceArray.pushBack(mace);
                }else if (gid == 234234){
                    Mace* mace = Mace::createMace("axeString.png", "axeHead.png", -45, Point(1, 1), Point(0, 0));
                    spriteBatch->addChild(mace);
                    mace->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE));
                    axeArray.pushBack(mace);
                }else if (gid == 234234){
                    FireStick* stick = FireStick::createFireStick("msFireSpin.png", true, 6);
                    spriteBatch->addChild(stick);
                    stick->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2));
                    fireStickArray.pushBack(stick);
                    
                    for(auto ball: stick->fireArray){
                        spriteBatch->addChild(ball);
                    }
                }else if (gid == 234234){
                    FireStick* stick = FireStick::createFireStick("msFireSpin.png", false, 6);
                    spriteBatch->addChild(stick);
                    stick->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2));
                    fireStickArray.pushBack(stick);
                    for(auto ball: stick->fireArray){
                        spriteBatch->addChild(ball);
                    }
                }else if (gid == 234234){
                    ThornDrop* enemy;
                    if (GameManager::getInstance()->theme == 0) {
                        enemy = ThornDrop::create(UNIT_THORN_DROP, 0, 0, "theme0ThornDrop.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 60);
                        enemy->setTarget(player);
                        spriteBatch->addChild(enemy);
                        enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - 4));
                    }else{
                        enemy = ThornDrop::create(UNIT_THORN_DROP, 0, 0, "theme2DropThorn.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 60);
                        enemy->setTarget(player);
                        spriteBatch->addChild(enemy);
                        enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - 8));
                    }
                    
                    enemyArray.pushBack(enemy);
                    fireableArray.pushBack(enemy);
                }else if (gid == 234234){
                    enemy = EnemyBase::createEnemy(UNIT_SHORT_THORN, 0, 0, "theme2Thorn.png",0, 50, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 0);
                    spriteBatch->addChild(enemy);
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2-4));
                    enemyArray.pushBack(enemy);
                    enemy->desiredPosition = enemy->getPosition();
                    enemy->untouchable = true;
                }else if (gid == 234234){
                    enemy = EnemyBase::createEnemy(UNIT_ELECTRIC_BALL, 0, 0, "theme2EletricBall.png",0, 50, ENEMY_MOVE_CUSTOM, ENEMY_MOVE_CUSTOM, 0);
                    spriteBatch->addChild(enemy);
                    float distanceToMove = 80;
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2 - distanceToMove/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2-4));
                    float timeToMove = (rand()%5)*0.1f + 0.7f;
                    enemy->runAction(RepeatForever::create(Sequence::create(MoveBy::create(timeToMove, Point(distanceToMove, 0)), MoveBy::create(timeToMove, Point(-distanceToMove, 0)), NULL)));
                    enemy->runAction(RepeatForever::create(RotateBy::create(0.5, 360)));
                    
                    enemyArray.pushBack(enemy);
                    enemy->untouchable = true;
                }else if(gid >= 83 && gid <= 86){
                    Trap* trap;
                    laserTrapDelay += 0.5;
                    float duration = 1.0f;
                    if (gid == 83 || gid == 84) {
                        duration = 1000;
                    }
                    trap = Trap::createTrap("laserEmpty.png", "laserTrap", laserTrapDelay, duration, TRAP_TYPE_LASER);
                    trap->setTag(UNIT_LASER_TRAP);
                    if (gid == 84 || gid == 86) {
                        trap->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE*2, (totalHeight-j)*TILE_SIZE - TILE_SIZE*0.5f));
                        trap->setRotation(-90);
                    }else{
                        trap->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE + TILE_SIZE));
                    }
                    spriteBatch->addChild(trap);
                    trapArray.pushBack(trap);
                }else if (gid == 87){
                    enemy = EnemyBase::createEnemy(UNIT_DUMMY, 30, 0, "dummy.png",0, 60, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 0);
                    spriteBatch->addChild(enemy);
                    enemy->setAnchorPoint(Point(0.5, 0));
                    enemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2 - enemy->getContentSize().height/2));
                    enemyArray.pushBack(enemy);
                    enemy->desiredPosition = enemy->getPosition();
                    enemy->setFlippedX(true);
                    loadedEnemyList.push_back(UNIT_DUMMY);
                }else if(gid == 234234){
                    
                    if (gid == 234234){
                        Droppable* sprite = Droppable::createDroppable(UNIT_SPRING_THEME0, 1, 0, "theme0Spring0.png");
                        sprite->setTag(UNIT_SPRING_THEME0);
                        spriteBatch->addChild(sprite);
                        sprite->setAnchorPoint(Point(0.5, 0));
                        sprite->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE));
                        helperArray.pushBack(sprite);
                    }else if (gid == 234234){
                        Droppable* sprite = Droppable::createDroppable(UNIT_SPRING_THEME2, 1, 0, "theme2Spring0.png");
                        sprite->setTag(UNIT_SPRING_THEME2);
                        spriteBatch->addChild(sprite);
                        sprite->setAnchorPoint(Point(0.5, 0));
                        sprite->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE));
                        helperArray.pushBack(sprite);
                    }else if(gid == 234234){
                        addRide(UNIT_BROOM, Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE + 29));
                    }else if(gid == 234234){
                        addRide(UNIT_BALLOON, Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE));
                    }else if(gid == 234234){
                        addRide(UNIT_CART, Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE + 29));
                    }else if(gid == 234234){
                        addRide(UNIT_TOP, Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE + 29));
                    }else if(gid == 234234){
                        addRide(UNIT_BIRD, Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE - TILE_SIZE + 29));
                    }else if(gid == 234234){
                        Sprite* tele = Sprite::createWithSpriteFrameName("timeMachine.png");
                        Sprite* effect = Sprite::createWithSpriteFrameName("timeMachineCircle.png");
                        tele->addChild(effect);
                        effect->setPosition(Point(tele->getContentSize().width/2, tele->getContentSize().height/2 - 4));
                        effect->runAction(RepeatForever::create(RotateBy::create(1, 360)));
                        effect->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.25, 0.8), ScaleTo::create(0.25, 1), ScaleTo::create(0.25, 0.8), ScaleTo::create(0.25, 1), NULL)));
                        float x = i*TILE_SIZE + TILE_SIZE/2 + 8;
                        float y =  (totalHeight-j)*TILE_SIZE - TILE_SIZE + 16;
                        tele->setPosition(tileMap->getPosition() + Point(x, y));
                        if (getTeleport(TAG_TELEPORT_0_ENTRY) == NULL) {
                            tele->setTag(TAG_TELEPORT_0_ENTRY);
                        }else{
                            tele->setTag(TAG_TELEPORT_0_EXIT);
                        }
                        
                        if(GameManager::getInstance()->theme == 3){
                            tele->setVisible(false);
                        }
                        
                        spriteBatch->addChild(tele);
                        teleportArray.pushBack(tele);
                    }else if(gid == 234234){
                        Sprite* tele = Sprite::createWithSpriteFrameName("timeMachine.png");
                        Sprite* effect = Sprite::createWithSpriteFrameName("timeMachineCircle.png");
                        tele->addChild(effect);
                        effect->setPosition(Point(tele->getContentSize().width/2, tele->getContentSize().height/2 - 4));
                        effect->runAction(RepeatForever::create(RotateBy::create(1, 360)));
                        effect->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.25, 0.8), ScaleTo::create(0.25, 1), ScaleTo::create(0.25, 0.8), ScaleTo::create(0.25, 1), NULL)));
                        float x = i*TILE_SIZE + TILE_SIZE/2 + 8;
                        float y =  (totalHeight-j)*TILE_SIZE - TILE_SIZE + 16;
                        tele->setPosition(tileMap->getPosition() + Point(x, y));
                        if (getTeleport(TAG_TELEPORT_1_ENTRY) == NULL) {
                            tele->setTag(TAG_TELEPORT_1_ENTRY);
                        }else{
                            tele->setTag(TAG_TELEPORT_1_EXIT);
                        }
                        if(GameManager::getInstance()->theme == 3){
                            tele->setVisible(false);
                        }
                        spriteBatch->addChild(tele);
                        teleportArray.pushBack(tele);
                    }else if(gid == 234234){
                        Sprite* tele = Sprite::createWithSpriteFrameName("timeMachine.png");
                        Sprite* effect = Sprite::createWithSpriteFrameName("timeMachineCircle.png");
                        tele->addChild(effect);
                        effect->setPosition(Point(tele->getContentSize().width/2, tele->getContentSize().height/2 - 4));
                        effect->runAction(RepeatForever::create(RotateBy::create(1, 360)));
                        effect->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.25, 0.8), ScaleTo::create(0.25, 1), ScaleTo::create(0.25, 0.8), ScaleTo::create(0.25, 1), NULL)));
                        float x = i*TILE_SIZE + TILE_SIZE/2 + 8;
                        float y =  (totalHeight-j)*TILE_SIZE - TILE_SIZE + 16;
                        tele->setPosition(tileMap->getPosition() + Point(x, y));
                        if (getTeleport(TAG_TELEPORT_2_ENTRY) == NULL) {
                            tele->setTag(TAG_TELEPORT_2_ENTRY);
                        }else{
                            tele->setTag(TAG_TELEPORT_2_EXIT);
                        }
                        
                        if(GameManager::getInstance()->theme == 3){
                            tele->setVisible(false);
                        }
                        spriteBatch->addChild(tele);
                        teleportArray.pushBack(tele);
                    }else if(gid == 234234){
                        TMXObjectGroup* objectGroup = tileMap->getObjectGroup("Object");
                        
                        if (objectGroup != NULL) {
                            
                            ValueMap spawnPoint = objectGroup->getObject("TriggerDoor1");
                            
                            int x = spawnPoint.at("X").asInt();
                            int y = spawnPoint.at("Y").asInt() + spawnPoint.at("height").asInt();
                            
                            float convertedX = x/TILE_SIZE;
                            float convertedY = ((TILE_SIZE*totalHeight) - y)/TILE_SIZE;
                            
                            bool isOpen = spawnPoint.at("Open").asBool();
                            
                            TriggerDoorInfo* info = TriggerDoorInfo::create(DOOR_TYPE_1, spriteBatch, Point(x, y), isOpen);
                            triggerDoorArray.pushBack(info);
                            
                            //                                info->doorSprite->setPosition(Point(x*TILE_SIZE, (totalHeight-y)*TILE_SIZE - TILE_SIZE));
                            
                            for (int k = 0; k < 3; k++) {
                                info->pointArray->addControlPoint(Point(convertedX, convertedY + k));
                                if (!info->isOpen) {
                                    stageLayer->setTileGID(8, info->pointArray->getControlPointAtIndex(i));
                                }
                            }
                            
                            x = i*TILE_SIZE + TILE_SIZE/2;
                            y = (totalHeight-j)*TILE_SIZE - TILE_SIZE/2;
                            info->trigger->setPosition(tileMap->getPosition() + Point(x, y));
                            info->triggerTalk->setPosition(tileMap->getPosition() + Point(x, y + 16));
                        }
                    }else if(gid == 234234){
                        TMXObjectGroup* objectGroup = tileMap->getObjectGroup("Object");
                        
                        if (objectGroup != NULL) {
                            
                            ValueMap spawnPoint = objectGroup->getObject("TriggerDoor2");
                            
                            int x = spawnPoint.at("X").asInt();
                            int y = spawnPoint.at("Y").asInt() + spawnPoint.at("height").asInt();
                            
                            float convertedX = x/TILE_SIZE;
                            float convertedY = ((TILE_SIZE*totalHeight) - y)/TILE_SIZE;
                            
                            
                            bool isOpen = spawnPoint.at("Open").asBool();
                            
                            TriggerDoorInfo* info = TriggerDoorInfo::create(DOOR_TYPE_2, spriteBatch, Point(x,y), isOpen);
                            triggerDoorArray.pushBack(info);
                            
                            //                                info->doorSprite->setPosition(Point(x*TILE_SIZE, (totalHeight-y)*TILE_SIZE - TILE_SIZE));
                            
                            for (int k = 0; k < 3; k++) {
                                info->pointArray->addControlPoint(Point(convertedX + k, convertedY));
                                if (!info->isOpen) {
                                    stageLayer->setTileGID(8, info->pointArray->getControlPointAtIndex(i));
                                }
                            }
                            
                            x = i*TILE_SIZE + TILE_SIZE/2;
                            y = (totalHeight-j)*TILE_SIZE - TILE_SIZE/2;
                            info->trigger->setPosition(tileMap->getPosition() + Point(x, y));
                            info->triggerTalk->setPosition(tileMap->getPosition() + Point(x, y + 16));
                        }
                    }else if(gid == 234234){
                        TMXObjectGroup* objectGroup = tileMap->getObjectGroup("Object");
                        
                        if (objectGroup != NULL) {
                            
                            ValueMap spawnPoint = objectGroup->getObject("TriggerDoor3");
                            
                            int x = spawnPoint.at("X").asInt();
                            int y = spawnPoint.at("Y").asInt() + spawnPoint.at("height").asInt();
                            
                            float convertedX = x/TILE_SIZE;
                            float convertedY = ((TILE_SIZE*totalHeight) - y)/TILE_SIZE;
                            
                            bool isOpen = spawnPoint.at("Open").asBool();
                            
                            TriggerDoorInfo* info = TriggerDoorInfo::create(DOOR_TYPE_3, spriteBatch, Point(x,y), isOpen);
                            triggerDoorArray.pushBack(info);
                            
                            //                                info->doorSprite->setPosition(Point(x*TILE_SIZE, (totalHeight-y)*TILE_SIZE - TILE_SIZE));
                            for (int k = 0; k < 3; k++) {
                                for (int l = 0; l < 3; l++) {
                                    info->pointArray->addControlPoint(Point(convertedX + k, convertedY + l));
                                    if (!info->isOpen) {
                                        stageLayer->setTileGID(8, info->pointArray->getControlPointAtIndex(info->pointArray->count()-1));
                                    }
                                }
                            }
                            
                            x = i*TILE_SIZE + TILE_SIZE/2;
                            y = (totalHeight-j)*TILE_SIZE - TILE_SIZE/2;
                            info->trigger->setPosition(tileMap->getPosition() + Point(x, y));
                            info->triggerTalk->setPosition(tileMap->getPosition() + Point(x, y + 16));
                        }
                    }else if(gid == 234234){
                        Droppable* sprite = Droppable::createDroppable(UNIT_MEDI_KIT, 1, 0, "medicine.png");
                        //                        coinSpriteBatch->addChild(sprite);
                        
                        spriteBatch->addChild(sprite);
                        sprite->setAnchorPoint(Point(0,1));
                        sprite->setTag(UNIT_MEDI_KIT);
                        sprite->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                        sprite->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.3, Point(0, 3)), MoveBy::create(0.5, Point(0, -6)), MoveBy::create(0.3, Point(0, 3)), NULL)));
                        coinArray.pushBack(sprite);
                    }else if(gid == 234234){
                        DestructablePlatform* platform;
                        if(GameManager::getInstance()->theme == 2){
                            platform = DestructablePlatform::createDestructablePlatform("theme2DestructablePlatform.png", 0.5f, point, ((TMXLayer*)tileMap->getChildByTag(TAG_STAGE_LAYER)));
                        }else if(GameManager::getInstance()->theme == 3){
                            platform = DestructablePlatform::createDestructablePlatform("destructableTheme3.png", 0.5f, point, ((TMXLayer*)tileMap->getChildByTag(TAG_STAGE_LAYER)));
                        }else{
                            platform = DestructablePlatform::createDestructablePlatform("destructablePlatform.png", 0.5f, point, ((TMXLayer*)tileMap->getChildByTag(TAG_STAGE_LAYER)));
                        }
                        spriteBatch->addChild(platform);
                        platform->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j-1)*TILE_SIZE));
                        platform->setAnchorPoint(Point::ZERO);
                        platform->originalPos = platform->getPosition();
                        stageLayer->setTileGID(51, point);
                        destructablePlatformArray.pushBack(platform);
                    }
                }
                else if(gid == 234234){
                    bool trigger = false;//keyExist(map, "MovingPlatformTriggerStart");
                    pointArrayToRemoveInUnitLayer->addControlPoint(point);
                    int x = i;
                    int y = j;
                    bool solved = false;
                    bool notFoundAny = false;
                    int startX = x;
                    int startY = y;
                    int searchX, searchY;
                    
                    while (!solved) {
                        notFoundAny = true;
                        for (int t = 0; t < 9; t++) { //3
                            int c = t % 3;
                            int r = (int)(t / 3);
                            searchX = x + c - 1;
                            searchY = y + r - 1;
                            int tileGid = unitLayer->getTileGIDAt(Point(searchX, searchY));
                            if (tileGid) {
                                Value properties = tileMap->getPropertiesForGID(tileGid);
                                
                                if (!properties.isNull())
                                {
                                    ValueMap newMap = properties.asValueMap();
                                    if (keyExist(newMap, "MovingPlatformLine")) {
                                        x = searchX;
                                        y = searchY;
                                        
                                        unitLayer->removeTileAt(Point(x, y));
                                        notFoundAny = false;
                                        break;
                                    }else if(keyExist(newMap, "MovingPlatformEnd")){
                                        x = searchX;
                                        y = searchY;
                                        
                                        Point startPosition =Point(startX*TILE_SIZE + 10, (totalHeight - startY)*TILE_SIZE);
                                        Point endPosition = Point(x*TILE_SIZE + 10, (totalHeight - y)*TILE_SIZE);
                                        pointArrayToRemoveInUnitLayer->addControlPoint(Point(x,y));
                                        const char* sptName;
                                        if (GameManager::getInstance()->theme == 0) {
                                            sptName = "movingPlatform0.png";
                                        }else if(GameManager::getInstance()->theme == 1){
                                            sptName = "movingPlatform3.png";
                                        }else{
                                            sptName = "theme2MovingPlatform.png";
                                        }
                                        MovingPlatform* platform = MovingPlatform::create(sptName, startPosition, endPosition);
                                        movingPlatformArray.pushBack(platform);
                                        spriteBatch->addChild(platform);
                                        
                                        platform->setPosition(startPosition);
                                        platform->desiredRect = platform->boundingBox();
                                        
                                        platform->isTowardEnd = true;
                                        
                                        notFoundAny = false;
                                        solved = true;
                                        
                                        if (trigger) {
                                            platform->isTriggerPlatform = true;
                                            platform->isTriggered = false;
                                        }
                                        
                                        break;
                                    }
                                }
                            }
                        }
                        if (notFoundAny) {
                            CCLOG("Moving Platform Creation Failed, Platform line not complete");
                            break;
                        }
                    }
                }
                unitLayer->removeTileAt(point);
                
                if((enemy != NULL && enemy->energy > 0) || (fEnemy != NULL && fEnemy->energy > 0)){
                    enemyMaxCount++;
                    /*if((fEnemy != NULL && fEnemy->energy > 0)){
                        if(mastery >= 7){
                            fEnemy = Pumpkin::create(UNIT_PUMPKIN, 150 + (stageIndex + themeIndex*20)*5, 0, "pumpkin0.png", 4, 10, ENEMY_MOVE_STAND, ENEMY_MOVE_STAND, 200);
                            fEnemy->setTarget(player);
                            spriteBatch->addChild(fEnemy);
                            fEnemy->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE));
                            enemyArray.pushBack(fEnemy);
                            droppableArray.pushBack(fEnemy);
                            fireableArray.pushBack(fEnemy);
                        }
                    }
                    */
                    EnemyBase* theEnemy;
                    if(fEnemy != NULL){
                        theEnemy = fEnemy;
                    }else if(enemy != NULL){
                        theEnemy = enemy;
                    }
                    
                    if(theEnemy != NULL){
                        if( mastery >= 7){
                            theEnemy->energy *= 4;
                        }else{
                            theEnemy->energy = theEnemy->energy*(1 + 0.3*mastery);
                        }
                        theEnemy->maxEnergy = theEnemy->energy;
                        addAppearEffect(theEnemy);
                    }
                }
            }
            
            
            if (foreLayer) {
                gid = foreLayer->getTileGIDAt(point);
                if (gid) {
//                    TileInfo* tInfo = this->getTileAtPosition(Point(i*TILE_SIZE, (totalHeight-j)*TILE_SIZE), foreLayer);
                    if (gid == 1233)
                    {
                        Sprite* waterSprite = Sprite::createWithSpriteFrameName("goldCoin0.png");
                        spriteBatch->addChild(waterSprite, 100);
                        foreLayer->removeTileAt(point);
                        
                        waterSprite->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + 8, (totalHeight-j)*TILE_SIZE - 8));
                        Animation* animation = AnimationCache::getInstance()->getAnimation("waterSurface");
                        Animate* animate = Animate::create(animation);
                        waterSprite->runAction(RepeatForever::create(animate));
                    }
                }
            }
            
            gid = decoLayer->getTileGIDAt(point);
            if (gid) {
                if (gid == 50){
                    Sprite* sptCandle = Sprite::createWithSpriteFrameName("theme2DoubleCandle0.png");
                    spriteBatch->addChild(sptCandle, -1);
                    decoLayer->removeTileAt(point);
                    sptCandle->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + 8, (totalHeight-j)*TILE_SIZE - 8));
                    Sprite* light = getLight();
                    this->addChild(light, 1);
                    sptCandle->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + 8, (totalHeight-j)*TILE_SIZE - 8));
                    light->setPosition(sptCandle->getPosition());
                    Animation* animation = AnimationCache::getInstance()->getAnimation("doubleCandle");
                    Animate* animate = Animate::create(animation);
                    sptCandle->runAction(RepeatForever::create(animate));
                }else if (gid == 51){
                    Sprite* sptCandle = Sprite::createWithSpriteFrameName("theme2Candle0.png");
                    spriteBatch->addChild(sptCandle, -1);
                    decoLayer->removeTileAt(point);
                    Sprite* light = getLight();
                    this->addChild(light, 1);
                    sptCandle->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + 8, (totalHeight-j)*TILE_SIZE - 8));
                    light->setPosition(sptCandle->getPosition());
                    Animation* animation = AnimationCache::getInstance()->getAnimation("candle");
                    Animate* animate = Animate::create(animation);
                    sptCandle->runAction(RepeatForever::create(animate));
                }
            }
        }
    }
    CCLOG("setStage map done");
    TMXObjectGroup* objectGroup = tileMap->getObjectGroup("Object");
        CCLOG("setStage object map done");
    if (pointArrayToRemoveInUnitLayer) {
        for (int i = 0; i < pointArrayToRemoveInUnitLayer->count(); i++) {
            Point pos = pointArrayToRemoveInUnitLayer->getControlPointAtIndex(i);
            unitLayer->removeTileAt(pos);
        }
    }
    
    
    tileMap->setTag(MAP_FOUND);
    bool isThereEnemy = false;
    for (auto enemy: enemyArray){
        if(enemy->secondTag != UNIT_TRAP_KIND_OF){
            isThereEnemy = true;
        }
    }
    if (isThereEnemy || enemiesToLoad.size() > 0) {
        GameManager::getInstance()->playSoundEffect(SOUND_KOONG);
        Point pos;
        //dnMiniMap->getParent()->runAction(MoveTo::create(0.5, miniMapPos + Point(0, 500)));
        isMiniMapFadeIn = false;
        ScrollView* scrollView = (ScrollView*)dnMiniMap->getParent()->getParent();
        scrollView->setBackGroundColorOpacity(0);
        this->schedule(schedule_selector(HelloWorld::fadeMiniMap));
        // lock doors aaaaaaaaaaaa
        for (int i = 1; i < tileMap->getMapSize().height; i++) {    // left
            pos = Point(1, i);
            if(tileMap->getLayer("stage")->getTileGIDAt(pos) == 0){
                tileMap->getLayer("stage")->setTileGID(322, pos);
                addDirtToTile(tileMap, pos);
            }
        }
        for (int i = 1; i < tileMap->getMapSize().width; i++) {    // bottom
            pos = Point(i, 1);
            if(tileMap->getLayer("stage")->getTileGIDAt(pos) == 0){
                tileMap->getLayer("stage")->setTileGID(322, pos);
                addDirtToTile(tileMap, pos);
            }
        }
        for (int i = 1; i < tileMap->getMapSize().height; i++) {    // right
            pos = Point(tileMap->getMapSize().width - 2, i);
            if(tileMap->getLayer("stage")->getTileGIDAt(pos) == 0){
                tileMap->getLayer("stage")->setTileGID(322, pos);
                addDirtToTile(tileMap, pos);
            }
        }
        for (int i = 1; i < tileMap->getMapSize().width; i++) {    // top
            pos = Point(i, tileMap->getMapSize().height - 2);
            if(tileMap->getLayer("stage")->getTileGIDAt(pos) == 0){
                tileMap->getLayer("stage")->setTileGID(322, pos);
                addDirtToTile(tileMap, pos);
            }
        }
    }
    revealedMapArray.pushBack(tileMap);
    if (stageIndex >= 0) {
        GameManager::getInstance()->totalRoom++;
        GameManager::getInstance()->totalScore+=500;
        GameManager::getInstance()->getHudLayer()->setScore(GameManager::getInstance()->totalScore);
    }
    tileMap->getLayer("deco")->setVisible(true);
    float dur = 2;
    /*
    while (enemyArray.size() > 0 && false) {
        FireableBase* enemy = (FireableBase*)enemyArray.at(0);
        enemyArray.erase(0);
        Point enemyPos = enemy->getPosition();
        
        enemyPointArray->addControlPoint(enemyPos);
        
        for (int i = 0; i < 4; i++) {
            Sprite* sptWarning = Sprite::createWithSpriteFrameName(__String::createWithFormat("enemyWarning%d.png", i)->getCString());
            sptWarning->setPosition(enemyPos);
            int angle = i*50;
            if (i == 2) {
                angle *= -3;
            }
            sptWarning->runAction(Sequence::create(RotateBy::create(dur, angle), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptWarning)), NULL));
            if (i == 3) {
                sptWarning->runAction(ScaleTo::create(dur, 0.2));
            }
            spriteBatch->addChild(sptWarning);
        }
        
        enemy->setPosition(Point(0, 10000));
        //                enemyArray.pushBack(fEnemy);
        //                fireableArray.pushBack(fEnemy);
        //                droppableArray.pushBack(fEnemy);
        enemiesToLoad.pushBack(enemy);
    }
    
    */
    if (enemiesToLoad.size() > 0) {
        this->runAction(Sequence::create(DelayTime::create(dur), CallFunc::create(CC_CALLBACK_0(HelloWorld::loadEnemies, this)), NULL));
    }
    
    isSetStageDone = true;
    CCLOG("setStage done");
}
void HelloWorld::stopWoongSound(){
    readyToMove = true;
    GameManager::getInstance()->stopSoundEffect(SOUND_LASER_WOONG);
}
void HelloWorld::addDirtToTile(TMXTiledMap* map, Point pos){
    Sprite* sptDirt;
    for (int i = 0; i < 4; i++) {
        sptDirt = Sprite::createWithSpriteFrameName(rand()%2==0?"dirt1.png":"dirt0.png");
        spriteBatch->addChild(sptDirt, 100);
        float dur = 1;
        sptDirt->runAction(MoveBy::create(0.1, Point(0, -2)));
        sptDirt->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur, Point(i%2==0?-4:4, 4)), 6), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptDirt)), NULL));
        sptDirt->runAction(Sequence::create(DelayTime::create(dur/2), FadeOut::create(dur/2),NULL));
        sptDirt->setPosition(Point(pos.x*TILE_SIZE + (i%2==0?0:TILE_SIZE), (map->getMapSize().height - pos.y - 1)*TILE_SIZE + (i>1?TILE_SIZE:0)) + map->getPosition());
        //log("dirt %f, %f", sptDirt->getPosition().x, sptDirt->getPosition().y);
    }
}

void HelloWorld::fadeMiniMap(float dt){
    if (isMiniMapFadeIn) {
        miniMapAlpha += 5;
    }else{
        miniMapAlpha -= 5;
    }
    if (miniMapAlpha > 255) {
        miniMapAlpha = 255;
        this->unschedule(schedule_selector(HelloWorld::fadeMiniMap));
    }else if(miniMapAlpha <= 0){
        miniMapAlpha = 0;
        this->unschedule(schedule_selector(HelloWorld::fadeMiniMap));
    }
    
    for(auto child: dnMiniMap->getChildren()){
        child->setOpacity(miniMapAlpha);
    }
    dnMiniMap->clear();
    for(auto map: revealedMapArray){
        addMapToMiniMap(map);
    }
    
    DrawNode* dnHero = (DrawNode*)dnMiniMap->getParent()->getChildByName("Hero");
    if(dnHero){
        dnHero->clear();
        dnHero->drawSolidRect(Point::ZERO, Point(TILE_SIZE*0.3f, TILE_SIZE*0.3f), Color4F(1, 0.3, 0.3, miniMapAlpha/255.0f));
    }
}
void HelloWorld::cloudSchedule(float dt){
    for (auto cloud : cloudArray) {
        if(cloud->getPosition().x < 0){
            cloud->setPosition(Point(cloud->getParent()->getContentSize().width + cloud->getContentSize().width/2, cloud->getPosition().y));
        }
    }
}
void HelloWorld::saveCoinIfStarCollected(){
//    if (starCount > 0) {
    if (collectedCoinCount < MAX_COIN_COUNT) {
        GameManager::getInstance()->addCoin(collectedCoinCount);
    }
    GameManager::getInstance()->saveCoin();
    int totalKillCount = UserDefault::getInstance()->getIntegerForKey("KC", 0);
    totalKillCount += killCountForRecord;
    UserDefault::getInstance()->setIntegerForKey("KC", totalKillCount);
//    }
}
void HelloWorld::sacrificeLater(float dt){
    Droppable* drop = groundBuddy;
    buddyArray.eraseObject(groundBuddy);
    droppableArray.eraseObject(groundBuddy);
    groundBuddy = NULL;
    drop->runAction(Sequence::create(EaseOut::create(MoveTo::create(0.5, player->getPosition() + Point(0, 50)), 3), MoveBy::create(0.05, Point(-2,1)), MoveBy::create(0.05, Point(2,-2)), MoveBy::create(0.05, Point(-2,1)), MoveBy::create(0.05, Point(2,1)), MoveBy::create(0.05, Point(-2,-2)),MoveBy::create(0.05, Point(-2,1)), MoveBy::create(0.05, Point(2,-2)), MoveBy::create(0.05, Point(-2,1)), MoveBy::create(0.05, Point(2,1)), MoveBy::create(0.05, Point(-2,-2)), CallFuncN::create(CC_CALLBACK_1(HelloWorld::sacrificeDone, this)), NULL));
    drop->runAction(EaseIn::create(RotateBy::create(0.1, 360), 3));
    
    GameManager::getInstance()->playSoundEffect(SOUND_JUMP);
/*    Sprite* spt = Sprite::create("backLight.png");
    spt->setScale(0.1);
    this->addChild(spt, -1);
    spt->setPosition(<#const cocos2d::Vec2 &pos#>)
    spt->runAction(FadeOut::create(0));
    spt->runAction(RepeatForever::create(RotateBy::create(3, 360)));
    spt->runAction(ScaleTo::create(0.1, 0.6));
    
    spt = Sprite::create("backLight.png");
    spt->setScale(0.1);
    this->addChild(spt, -1);
    spt->runAction(RepeatForever::create(RotateBy::create(3, -360)));
    spt->runAction(ScaleTo::create(0.1, 0.6));*/
}
void HelloWorld::sacrificeDone(Ref* obj){
    Droppable* drop = (Droppable*)obj;
    runEffect(EFFECT_TWINKLE, drop->getPosition());
    GameManager::getInstance()->getHudLayer()->addHeartAnimated((drop->getPosition()*4 + this->getPosition()));
//    drop->unscheduleAllSelectors();
    drop->stopAllActions();
    drop->removeFromParentAndCleanup(true);
    player->energy++;    player->maxEnergy++;
}

void HelloWorld::addRide(int unit, Point pos){
    const char* sptName;
    if(unit == UNIT_TOP) sptName = "rideTop.png";
    else if(unit == UNIT_CART) sptName = "rideCart.png";
    else if(unit == UNIT_BROOM) sptName = "rideBroom.png";
    else if(unit == UNIT_BALLOON) sptName = "rideBalloon.png";
    else if(unit == UNIT_BIRD) sptName = "rideBird.png";
    Droppable* board = Droppable::createDroppable(unit, 1, 0, sptName);
    board->setPosition(pos);
    board->setTag(unit);
    
    /*Sprite* sprite = Sprite::createWithSpriteFrameName("silverCoin0.png");
    sprite->setPosition(Point(16, 20));
    sprite->runAction(RepeatForever::create((ActionInterval*)Sequence::create(MoveBy::create(0.3f, Point(0, 15)), MoveBy::create(0.3f, Point(0, -15)), NULL)));
    board->addChild(sprite);*/
    
    spriteBatch->addChild(board);
    helperArray.pushBack(board);
    float time = 0.6f;
    MoveTo* moveUpTop = MoveTo::create(time, pos + Point(0, 10));
    MoveTo* moveDownTop = MoveTo::create(time, pos);
    MoveTo* moveDownBottom = MoveTo::create(time, pos + Point(0, - 10));
    MoveTo* moveUpBottom = MoveTo::create(time, pos);
    float rate = 1;
    
    board->runAction(RepeatForever::create((ActionInterval*)Sequence::create(EaseIn::create(moveUpTop, rate), EaseOut::create(moveDownTop, rate), EaseIn::create(moveDownBottom, rate), EaseOut::create(moveUpBottom, rate), NULL)));
}
void HelloWorld::splashDemage(Point pos, int radius, int demage){
    Rect mRect;
    EnemyBase* drop;
    
    for (int i = 0; i<enemyArray.size(); i++) {
        drop = enemyArray.at(i);
        
        Point tPos = drop->getPosition();
        float distance = sqrtf((pos.x - tPos.x)*(pos.x - tPos.x) + (pos.y - tPos.y)*(pos.y - tPos.y));
        
        if (distance < radius){
            Point pos = drop->getPosition();
            if(!drop->untouchable){
                if (!drop->immortal) {
                    if (demage >= 10) {
                        demage = demage*0.8f + (rand()%((int)(demage*0.2f)));
                    }
                    
                    if (drop->energy < demage) {
                        demage = drop->energy;
                    }
                    drop->energy -= (int)demage;
                    drop->showEnergy();
                    
                    if (drop == theBoss) {
                        GameManager::getInstance()->getHudLayer()->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
                    }else{
//                        drop->desiredPosition = drop->desiredPosition + Point(drop->isTowardLeft?2:-2, 2);
//                        drop->setPosition(drop->getPosition() + Point(drop->isTowardLeft?2:-2, 2));
                    }
                    
                    showDamage(demage, drop->getPosition());
                    runEffect(EFFECT_EXPLODE_SMALL, drop->getPosition());
                    if (drop->moveTypeAfterFindHero != drop->currentMoveType) {
                        //                    drop->doActionAfterFindTarget();
                    }
                    
                    drop->blinkForAWhile();
                }
                
                break;
            }
            
        }
        
    }
    
    this->runEffect(EFFECT_EXPLODE_HUGE, pos);
    
}
void HelloWorld::updateIndicators(){
    Droppable* unit;
    float eX = 0;
    float eY = 0;
    float centerX = -this->getPosition().x + size.width/2;
    float centerY = -this->getPosition().y + size.height/2;
    float a;float y;float x;Rect rect;
    int outOfBoxCount = 0;
    Sprite* indicator;
    Sprite* spt;
    float scale = this->getScale();
    float offset = 15;
    for (int i = 0; i < indicatedArray.size(); i++) {
        spt = (Sprite*)indicatedArray.at(i);
        if(spt->getTag() == UNIT_STAR){
            unit = (Droppable*)starArray.at(i);
        }else if(spt->getTag() == UNIT_KEY){
            unit = theKey;
            if(keyCollected && unit->isVisible()){
                unit->setVisible(false);
            }
        }else if(spt->getTag() == UNIT_DOOR){
            unit = theDoor;
        }
        
        rect = Rect(-this->getPosition().x/scale, -this->getPosition().y/scale, size.width/scale, size.height/scale);
        if(rect.containsPoint(unit->getPosition())){
            continue;
        }else{
            
        }
        eX = unit->getPosition().x*scale;
        eY = unit->getPosition().y*scale;
        a = (eY - centerY)/(eX - centerX);
        y = centerY > eY? (-size.height/2 + offset):(size.height/2 - offset);
        x = y/a;
        if (x > size.width/2) {// right
            x = size.width/2 - offset;
            y = x*a;
        }else if(x < -size.width/2){//left
            x = -size.width/2 + offset;
            y = x*a;
        }else{
            
        }
        
        if (indicatorArray.size() > outOfBoxCount) {
//            indicator = indicatorArray.at(outOfBoxCount);
            for (auto indi: indicatorArray) {
                if(indi->getTag() == unit->getTag()){
                    indicator = indi;
                    break;
                }
            }
        }else{
            indicator = Sprite::create("blueArrow.png");\
            indicator->setTag(spt->getTag());
            if(spt->getTag() == UNIT_STAR){
                spt = Sprite::createWithSpriteFrameName("star.png");
                spt->setScale(4);
                indicator->setTag(UNIT_STAR);
            }else if(spt->getTag() == UNIT_KEY){
                spt = Sprite::createWithSpriteFrameName("key.png");
                spt->setScale(4);
                indicator->setTag(UNIT_KEY);
            }else if(spt->getTag() == UNIT_DOOR){
                spt = Sprite::createWithSpriteFrameName("door.png");
                spt->setScale(2);
            }
            
            spt->setTag(79);
            indicator->addChild(spt);

            spt->setPosition(Point(-50, indicator->getContentSize().height*indicator->getScale()/2));
            indicator->setScale(0.5);
            this->getParent()->addChild(indicator);
            GameManager::getInstance()->makeItSiluk(indicator);
            indicatorArray.pushBack(indicator);
        }

        indicator->setRotation(-atan2(y,x)*180.0f/3.14f);
        indicator->setPosition(Point(size.width/2 + x, size.height/2 + y));

        spt = (Sprite*)indicator->getChildByTag(79);
        spt->setRotation(-indicator->getRotation());
        
        outOfBoxCount++;
    }
    
    if (indicatorArray.size() > outOfBoxCount) {
//        for (int i = (int)indicatorArray.size() - 1; i >= outOfBoxCount; i--) {
//            indicator = (Sprite*)indicatorArray.at(i);
//            indicatorArray.eraseObject(indicator);
//            indicator->removeFromParentAndCleanup(true);
//        }
        for(auto indi: indicatorArray){
            indi->removeFromParentAndCleanup(true);
        }
        indicatorArray.clear();
        updateIndicators();
    }
}

void HelloWorld::removeUsedAssets(){
    CCLOG("HelloWorld removeUsedAssets");
        if(background)
        background->removeFromParentAndCleanup(true);
    unscheduleAll();
    heroPositionArray->release();
    for (auto lbl: talkLabelArray){
        lbl->release();
    }
    //    _eventDispatcher->removeEventListener(listener);
    player->removeFromParentAndCleanup(true);
    this->removeAllChildrenWithCleanup(true);
    this->unscheduleAllCallbacks();
    this->stopAllActions();
    GameManager::getInstance()->setHudLayer(NULL);
    GameManager::getInstance()->setCurrentStageLayer(NULL);
}
void HelloWorld::testSchedule(float dt){
    //    testPet->setPosition(player->getPosition() + Point(50, 0));
    //    testPet->setPosition(player->damageBoundingBox().origin);
    //    testPet->setAnchorPoint(Point::ZERO);
    //    testPet->setScale(player->damageBoundingBox().size.width, player->damageBoundingBox().size.height);
}
void HelloWorld::useBomb(){
    if(isGameOver)
        return;
    
    CCLOG("Use Bomb!");
    isPaused = true;
    for(auto spt: enemyArray)
    {
        spt->pause();
    }
    
    for(auto spt: heroMissileArray)
    {
        spt->pause();
    }
    
    for(auto spt: enemyMissileArray)
    {
        spt->pause();
    }
    
    bombTargetMissileArray.clear();
    bombTargetEnemyArray.clear();
    Point pos = player->getPosition();
    Point tPos;
    Rect rect = Rect(player->getPosition().x - size.width/8, player->getPosition().y - size.height/8, size.width/4, size.height/4);
    for (auto enemy: enemyArray) {
        if(enemy->untouchable || enemy->immortal) continue;
        tPos = enemy->getPosition();
        if (rect.containsPoint(tPos)){//distance < targetDistance) {
            bombTargetEnemyArray.pushBack(enemy);
        }
    }
    for (auto ms: enemyMissileArray) {
        tPos = ms->getPosition();
        if (rect.containsPoint(tPos)){//distance < targetDistance) {
            bombTargetMissileArray.pushBack(ms);
        }
    }
    
    Sprite* effect = Sprite::createWithSpriteFrameName("daryFace.png");
    effect->setPosition(player->getPosition());
    spriteBatch->addChild(effect);
    effect->runAction(Sequence::create(ScaleTo::create(1, 15), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
    effect->runAction(Sequence::create(DelayTime::create(0.5), FadeOut::create(0.4), NULL));
    
    this->schedule(schedule_selector(HelloWorld::doTheBombToMissiles), 0.1, bombTargetMissileArray.size(), 1);
}

void HelloWorld::doTheBombToMissiles(float dt){
    CCLOG("doTheBombToMissiles");
    if (bombTargetMissileArray.size() <= 0) {
        this->unschedule(schedule_selector(HelloWorld::doTheBombToMissiles));
        bombTargetEnemyIndex = 0;
        this->schedule(schedule_selector(HelloWorld::doTheBombToEnemies), 0.1, bombTargetEnemyArray.size(), 0.1);
        return;
    }
    
    Missile* drop = bombTargetMissileArray.at(0);
    Sprite* sprite = Sprite::createWithSpriteFrameName("daryFace.png");
    sprite->runAction(Sequence::create(ScaleTo::create(0.5, 5), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
    sprite->runAction(FadeOut::create(0.5));
    spriteBatch->addChild(sprite);
    switch (drop->getTag()) {
        case UNIT_MISSILE_CHASING:
            chasingMissileArray.eraseObject(drop);
            break;
        case UNIT_MISSILE_DROPPABLE:
            droppableMissileArray.eraseObject(drop);
            break;
        case UNIT_MISSILE_CUSTOM:
            customMovingArray.eraseObject(drop);
            break;
        case UNIT_MISSILE_STRAIGHT:
            straightMovingArray.eraseObject(drop);
            break;
        default:
            break;
    }
    sprite->setPosition(drop->getPosition());
    bombTargetMissileArray.erase(0);
    enemyMissileArray.eraseObject(drop);
    spriteBatch->removeChild(drop, true);
    
    Point screenPos = this->getPosition();
    this->runAction(Sequence::create(MoveBy::create(0.05, Point(rand()%36 - 18, rand()%36 - 18)),
                                     MoveTo::create(0.05, screenPos),NULL));
}

void HelloWorld::doTheBombToEnemies(float dt){
    float demage = 1;
    
    if (bombTargetEnemyArray.size() <= bombTargetEnemyIndex ) {
        this->unschedule(schedule_selector(HelloWorld::doTheBombToEnemies));
        isPaused = false;
        
        for(auto spt: enemyArray)
        {
            spt->resume();
        }
        
        for(auto spt: heroMissileArray)
        {
            spt->resume();
        }
        
        for(auto spt: enemyMissileArray)
        {
            spt->resume();
        }
        
        for(auto enemy: bombTargetEnemyArray){
            if(enemy->untouchable){
                
            }else{
                enemy->energy -= (int)demage;
                
                if (enemy == theBoss) {
                    GameManager::getInstance()->getHudLayer()->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
                }else{
                    enemy->desiredPosition = enemy->desiredPosition + Point(enemy->isTowardLeft?2:-2, 2);
                    enemy->setPosition(enemy->getPosition() + Point(enemy->isTowardLeft?2:-2, 2));
                }
                
                showDamage(demage, enemy->getPosition());
                
                if (enemy->energy <= 0) {
                    this->runEffect(EFFECT_EXPLODE_SMALL, enemy->getPosition());
                    this->createRewards(enemy);
                    
                    enemyArray.eraseObject(enemy);
                    droppableArray.eraseObject(enemy);
                    spriteBatch->removeChild(enemy, true);
                }else{
                    enemy->blinkForAWhile();
                }
            }
        }
        
        return;
    }
    
    EnemyBase* enemy = bombTargetEnemyArray.at(bombTargetEnemyIndex);
    Sprite* sprite = Sprite::createWithSpriteFrameName("daryFace.png");
    sprite->runAction(Sequence::create(ScaleTo::create(0.5, 5), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
    sprite->runAction(FadeOut::create(0.5));
    spriteBatch->addChild(sprite);
    sprite->setPosition(enemy->getPosition());
    
    Point screenPos = this->getPosition();
    this->runAction(Sequence::create(MoveBy::create(0.05, Point(rand()%18 - 9, rand()%18 - 9)),
                                     MoveTo::create(0.05, screenPos),NULL));
    
    bombTargetEnemyIndex++;
}
void HelloWorld::usePotion(){
    if(isGameOver)
        return;
    
    player->energy = player->maxEnergy;
    GameManager::getInstance()->getHudLayer()->setLife(player->maxEnergy);
    // heal effect
    CCLOG("Use Potion!");
}

bool HelloWorld::keyExist(ValueMap map, const char* key){
    return map.find(key) != map.end();
}

void HelloWorld::revive(){
    resumeLayer();
    player->setPosition(deadPoint);
    
    isGameOver = false;
    indestructableTime = -1;
    
    player->velocity = Point(0, 100);
    
    GameManager::getInstance()->isInMiddleOfGame = true;
    
    indestructableTime =  1.6f;
    this->schedule(schedule_selector(HelloWorld::blinkingPlayer), 0.1f);
    
    GameManager::getInstance()->getHudLayer()->enableButtons(true);
    
    player->energy = player->maxEnergy;
    GameManager::getInstance()->getHudLayer()->setLife(player->maxEnergy);
    GameManager::getInstance()->playSoundEffect(SOUND_MEDICINE);
}

void HelloWorld::enemyFireLoop(float dt)
{
    for(auto enemy: fireableArray)
    {
        while(enemy->missileArray.size() > 0){
            Missile* missile = enemy->missileArray.at(0);
            missile->energy = enemy->maxEnergy*0.1f;
            enemy->missileArray.erase(0);
            Node* light = getLight();
            light->setPosition(missile->getPosition());
            this->addChild(light);
            light->runAction(Sequence::create(DelayTime::create(0.1), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, light)), NULL));
            light->setColor(Color3B(250, 100, 100));
            if (missile->getScaleX() == missile->getScaleY()) {
                missile->runAction(ScaleTo::create(0.2f, 1));
            }
            
            if(missile->getTag() == UNIT_MISSILE_DROPPABLE){
                droppableMissileArray.pushBack(missile);
                enemyMissileArray.pushBack(missile);
                if (missile->secondTag != UNIT_APPLEMON_WORM) {
                    missile->secondTag = enemy->getTag();
                }
                missile = NULL;
            }else if(missile->getTag() == UNIT_MISSILE_STRAIGHT){
                straightMovingArray.pushBack(missile);
                enemyMissileArray.pushBack(missile);
                //missile->secondTag = enemy->getTag();
                missile = NULL;
            }else if(missile->getTag() == UNIT_MISSILE_CUSTOM){
                customMovingArray.pushBack(missile);
                enemyMissileArray.pushBack(missile);
                //missile->secondTag = enemy->getTag();
                missile = NULL;
            }else if(missile->getTag() == UNIT_MISSILE_CHASING){
                chasingMissileArray.pushBack(missile);
                
                if(missile->secondTag == UNIT_BAZOOKA_MINI){
                    missile->runAction(Sequence::create(DelayTime::create(2), CallFuncN::create(CC_CALLBACK_1(HelloWorld::removeMeFromChasing, this)), NULL));
                }
                
                if(enemy->getTag() == UNIT_BEE_HIVE ||
                   missile->secondTag == UNIT_BAZOOKA_MINI ||
                   missile->secondTag == UNIT_BAZOOKA_BIG){
                    CCLOG("bee hive");
                    enemyArray.pushBack((EnemyBase*)missile);
                }else{
                    CCLOG("** NOT bee hive");
                    //missile->secondTag = enemy->getTag();
                    enemyMissileArray.pushBack(missile);
                }
                
                missile = NULL;
            }else if(missile->getTag() == UNIT_THORN_DROP){
                droppableArray.pushBack(enemy);
                missile = NULL;
                break;
            }
        }
    }
}
void HelloWorld::removeMeFromChasing(Ref* ref){
    Droppable* drop = (Droppable*)ref;
    drop->setTag(UNIT_MISSILE_STRAIGHT);
    chasingMissileArray.eraseObject(drop);
    straightMovingArray.pushBack(drop);
}

void HelloWorld::bubbleUpdate(float dt)
{
    char buf[80];
    sprintf( buf, "theme_%d_Background_tower_%d_shadow.png", GameManager::getInstance()->theme, rand()%2);
    
    for(auto bubble: bubbleArray)
    {
        bubble->setPosition(Point(bubble->getPosition().x, bubble->getPosition().y + 50*dt));
        
        TMXTiledMap* map = getTileMap(bubble->getPosition());
        //        Point plPos = this->tileCoordForPosition(bubble->getPosition() + Point(0, - bubble->getContentSize().height), map);
        int tgid = getTileAtPosition(bubble->getPosition(), TAG_FORE_LAYER, map);
        if (tgid == 0) {
            bubbleArray.eraseObject(bubble);
            bubble->stopAllActions();
            sprintf( buf, "bubble%dPop", bubble->getTag());
            Animation* animation = AnimationCache::getInstance()->getAnimation(buf);
            Animate* animate = Animate::create(animation);
            CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this));
            bubble->runAction(Sequence::create(animate, call, NULL));
            return;
        }
    }
}

void HelloWorld::makeSomeBubbles(float dt)
{
    int index = 0;
    char buf[50];
    for(auto sprite: inWaterArray)
    {
        if (rand()%7 > 0) {
            continue;
        }
        index = rand()%2;
        
        sprintf( buf, "bubble%d.png", index);
        
        Droppable* bubble = Droppable::createDroppable(UNIT_BUBBLE, 1, 0, buf);
        //        bubble->setPosition(Point(sprite->getPosition().x + (rand()%8-4), sprite->getPosition().y + (rand()%8-4)));
        bubble->setPosition(sprite->getPosition() + Point(0, sprite->getContentSize().height + 4));
        bubbleArray.pushBack(bubble);
        spriteBatch->addChild(bubble, 20);
        bubble->setTag(index);
    }
}
void HelloWorld::setPlayerInfo()
{
    player->extraSpeed = 60;
    if (player->vehicleType == VEHICLE_GOLIATH) {
        playerWeaponType = 8;
        player->extraJump = -45;
        reloadingCoolTime = 0.1f;
    }else if(player->getTag() == CHARACTER_DARY){
        player->extraJump = 0;
    }else if(player->getTag() == CHARACTER_PAUL){
        player->extraJump = 0;
    }else if(player->getTag() == CHARACTER_GABRIEL){
        player->extraJump = 0;
        player->extraSpeed = 120;
    }else if(player->getTag() == CHARACTER_BREE){
        player->extraJump = 0;
    }else if(player->getTag() == CHARACTER_JOHN){
        player->extraJump = 0;
    }else if(player->getTag() == CHARACTER_NINJA){
        player->extraJump = 0;
    }else if(player->getTag() == CHARACTER_SUSAN){
        player->extraJump = 0;
    }
}

void HelloWorld::setWeaponInfo(int weapon, int level){
//    float attackBuff = 1;
//    float criticalPossibility = 0;
//    for (int i = 0; i < 4; i++) {
//        int rune = UDGetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, i).c_str(), -1);
//        if(rune > 0){
//            if(rune%4 == weapon%4 && GM->getRuneBuffType(rune) == RUNE_BUFF_TYPE_ATTACK){
//                attackBuff += GM->getRuneBuff(rune/1000, RUNE_BUFF_TYPE_ATTACK);
//            }
//            if(rune%4 == weapon%4 && GM->getRuneBuffType(rune) == RUNE_BUFF_TYPE_CRI){
//                criticalPossibility += GM->getRuneBuff(rune/1000, RUNE_BUFF_TYPE_CRI);
//            }
//        }
//    }
//    log("weapon buff: %f", attackBuff);
//
//    float weaponPower = GM->getWeaponPower(weapon, level)*(1 + attackBuff*0.01f)*(1 + GM->getResearchPower()*0.01f);
//    log("weapon power : %f", weaponPower);
//    player->setWeapon(weapon, weaponPower, DEFAULT_CRITICAL + criticalPossibility);
    player->setWeapon(weapon, GM->getWeaponPowerRuneResearchApplied(weapon + level*1000), GM->getWeaponCriticalPossibilityRuneResearchApplied(weapon + level*1000));
    playerWeaponType = weapon;
    loadedBulletCountMax = getMaxLoadedBulletCount(weapon);
    playerFireCoolTimeMax = getWeaponCoolTime(weapon)*(1 + GM->getResearchCoolTime()*0.01f);
    log("** max: %f, weapon: %d", playerFireCoolTimeMax, weapon);
    reloadingCoolTime = 1.0f;
    switch (weapon) {
        case WEAPON_PISTOL: // 20 p/s
            log("** max: %f, weapon: %d", playerFireCoolTimeMax, weapon);
            break;
        case WEAPON_MAGNUM: // 25
            reloadingCoolTime = 1.2f;
            break;
        case WEAPON_UZI: // 34
            reloadingCoolTime = 1.5f;
            break;
        case WEAPON_RIFLE: // 25
            reloadingCoolTime = 1.5f;
            break;
        case WEAPON_FIRE_BOTTLER: // 42
            break;
        case WEAPON_ANTI_BAT: // 60
            reloadingCoolTime = 1.5f;
            break;
        case WEAPON_SNIPER_RIFLE: // 36
            break;
        case WEAPON_RIFLE_GRENADE: // 35
            reloadingCoolTime = 1.5f;
            break;
        case WEAPON_CROSS_BOW: // 49
            reloadingCoolTime = 2;
            break;
        case WEAPON_MACHINE_GUN: // 110
            reloadingCoolTime = 3;
            break;
        case WEAPON_STRIKER: // 61
            reloadingCoolTime = 1.5f;
            break;
        case WEAPON_NIFE_THROWER: // 88
            reloadingCoolTime = 1.5f;
            break;
        case WEAPON_BALL_SHOOTER: // 111
            reloadingCoolTime = 2;
            break;
        case WEAPON_FLAME_THROWER: // 207
            reloadingCoolTime = 2;
            break;
        case WEAPON_ROCKET_LAUNCHER: // 132
            reloadingCoolTime = 1.5f;
            break;
        case WEAPON_BAZOOKA: // 44
            break;
        case WEAPON_MATT_A4: // 164
            break;
        case WEAPON_LASER_GUN: // 161
            reloadingCoolTime = 2;
            break;
        case WEAPON_ENERGY_GUN: // 85
            reloadingCoolTime = 1.5f;
            break;
        case WEAPON_GUIDED_ROCKET_LAUNCHER: // 107
            reloadingCoolTime = 3;
            break;
        case WEAPON_SLUGGER: // 171
            reloadingCoolTime = 4;
            break;
        case WEAPON_DRAGON_RIFLE: // 282
            reloadingCoolTime = 2.5f;
            break;
        case WEAPON_SLUGGER_NG: // 171
            reloadingCoolTime = 3;
            break;
        case WEAPON_LIGHTNING_GUN: // 171
            reloadingCoolTime = 3;
            break;
        case WEAPON_SHARK_GUN: // 171
            reloadingCoolTime = 4;
            break;
        case WEAPON_LASER_COIN: // 171
            reloadingCoolTime = 2;
            break;
        default:
            break;
    }
    log("** max: %f, weapon: %d", playerFireCoolTimeMax, weapon);
}
float HelloWorld::getWeaponCoolTime(int weapon){
    switch (weapon) {
        case WEAPON_PISTOL: // 20 p/s
            return 0.3f;
        case WEAPON_MAGNUM: // 25
            return 0.2f;
            break;
        case WEAPON_UZI: // 34
            return 0.1f;
            break;
        case WEAPON_RIFLE: // 25
            return 0.5f;
            break;
        case WEAPON_FIRE_BOTTLER: // 42
            return 0.7f;
            break;
        case WEAPON_ANTI_BAT: // 60
            return 0.2f;
            break;
        case WEAPON_SNIPER_RIFLE: // 36
            return 0.8f;
            break;
        case WEAPON_RIFLE_GRENADE: // 35
            return 0.6f;
            break;
        case WEAPON_CROSS_BOW: // 49
            return 0.3f;
            break;
        case WEAPON_MACHINE_GUN: // 110
            return 0.07f;
            break;
        case WEAPON_STRIKER: // 61
            return 0.3f;
            break;
        case WEAPON_NIFE_THROWER: // 88
            return 0.2f;
            break;
        case WEAPON_BALL_SHOOTER: // 111
            return 0.15f;
            break;
        case WEAPON_FLAME_THROWER: // 207
            return 0.05f;
            break;
        case WEAPON_ROCKET_LAUNCHER: // 132
            return 1.0f;
            break;
        case WEAPON_BAZOOKA: // 44
            return 3.5f;
            break;
        case WEAPON_MATT_A4: // 164
            return 0.3f;
            break;
        case WEAPON_LASER_GUN: // 161
            return 0.01f;
            break;
        case WEAPON_ENERGY_GUN: // 85
            return 0.4f;
            break;
        case WEAPON_GUIDED_ROCKET_LAUNCHER: // 107
            return 0.7f;
            break;
        case WEAPON_SLUGGER: // 171
            return 0.3f;
            break;
        case WEAPON_DRAGON_RIFLE: // 282
            return 0.3f;
            break;
        case WEAPON_SLUGGER_NG: // 171
            return 0.3f;
            break;
        case WEAPON_LIGHTNING_GUN: // 171
            return 0.2f;
            break;
        case WEAPON_SHARK_GUN: // 171
            return 1.0f;
            break;
        case WEAPON_LASER_COIN: // 171
            return 0.01f;
            break;
        default:
            break;
    }
    return 0.3f;
}
void HelloWorld::talkUpdate(float dt)
{
    Sprite* sprite;
    Rect projectileRect;
    bool talkEncountered = false;
    bool shouldReset = true;
    for (int i = 0; i<talkArray.size(); i++) {
        sprite = (Sprite*)talkArray.at(i);
        
        if (sprite->getPosition().y-player->getPosition().y > 50 || sprite->getPosition().y - player->getPosition().y < -50) {
            continue;
        }
        
        projectileRect = Rect(sprite->getPosition().x - (sprite->getContentSize().width*sprite->getScale()/2),
                              sprite->getPosition().y - (sprite->getContentSize().height*sprite->getScale()/2),
                              sprite->getContentSize().width*sprite->getScale(),
                              sprite->getContentSize().height*sprite->getScale());
        if (projectileRect.intersectsRect(player->collisionBoundingBox())) {
            shouldReset = false;
            if (currentTalkIndex != i) {
                
                currentTalkIndex = i;
                talkEncountered = true;
                GameManager::getInstance()->playSoundEffect(SOUND_COIN);
                break;
                //                GameManager::getInstance()->getHudLayer()->enableLayerAndMenus(false);
            }
        }
    }
    
    if (talkEncountered) {
        GameManager::getInstance()->getHudLayer()->showTalk(talkLabelArray.at(currentTalkIndex)->getString().c_str());
        GameManager::getInstance()->getHudLayer()->lblTouch->runAction(RepeatForever::create(Sequence::create(FadeIn::create(0.1), DelayTime::create(0.2f), FadeOut::create(0.2f), DelayTime::create(0.1f), NULL)));
        //        CCLOG("encountered: %s", talkLabelArray.at(currentTalkIndex)->getString().c_str());
    }
    if (shouldReset) {
        currentTalkIndex = -1;
    }
}
void HelloWorld::autoTargeting(){
    if (!isAutoTargetingOn) {
        return;
    }
    Droppable* enemy = findTarget(200);
    float angle;
    bool shouldChangeAngle = true;
    if(enemy == nullptr) {
        if(player->velocity.x < 0){
            angle = 3.14f;
        }else if(player->velocity.x > 0){
            angle = 0;
        }else{
//            if (player->lastAngle == 0 || player->lastAngle == -180) {
                shouldChangeAngle = false;
//            }
        }
    }else{
        angle = (enemy->damageBoundingBox().origin + Point(enemy->damageBoundingBox().size.width/2, enemy->damageBoundingBox().size.height/2) - player->getPosition()).getAngle();
    }
    
    int radius = 170;
    float dx = cosf(angle) * radius;
    float dy = sinf(angle) * radius;
    
//    Point velocity = Point(dx/joystickRadius, dy/joystickRadius);
//    float degrees = angle * SJ_RAD2DEG;
//    
//    // Update the thumb's position
//    Point stickPosition = Point(dx, dy);
//    
//    Vec2 location = Vec2(size.width/2 + dx*6, size.height/2 + dy*6);
//    
//        
//
//    log("targeting: %f, dx: %f, dy: %f", angle*180/3.14f, dx, dy);
    if(shouldChangeAngle){
        GameManager::getInstance()->getHudLayer()->cursorLayer->cursor->setPosition(center + Point(dx, dy));
        GameManager::getInstance()->getHudLayer()->cursorLayer->cursor->setRotation(angle*180);
    }
}
void HelloWorld::entranceSchedule(float dt){
    if (npcArray.size() > 0) {
        if(npcArray.at(0)->getBoundingBox().origin.x + 10 < player->getPositionX() && !player->wantsToJump && player->getPositionY() < TILE_SIZE*4){
            GameManager::getInstance()->rightPressed = false;
            GameManager::getInstance()->leftPressed = false;
            GameManager::getInstance()->jumpPressed = true;
            log("jump: %f", player->getPositionY());
        }else if(npcArray.at(0)->getBoundingBox().origin.x + 16 > player->getPositionX() && player->onGround && !GameManager::getInstance()->rightPressed){
            GameManager::getInstance()->rightPressed = true;
            player->startWalkingAnimation();
            player->hand->setVisible(false);
        }
    }
}
void HelloWorld::releaseCustomMovingCamera(){
    isCameraInCustomMoving = false;
}
void HelloWorld::gravityUpdate(float dt)
{
    if (stageIndex == STAGE_ENTRANCE) {
        entranceSchedule(dt);
    }
    if (isGameOver) {
        if (GameManager::getInstance()->getHudLayer()->cursorLayer->isMouseLeftDown) {
            GameManager::getInstance()->getHudLayer()->tryResultOK();
        }
        return;
    }
    if (isPaused) {
        return;
    }
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    autoTargeting();
//#endif
    
    if (dt > 0.03) {
        dt = 0.03;
    }
    
    //player->desiredPosition = player->getPosition();
    if (isGameOver) {
        return;
    }
    if (GameManager::getInstance()->totalTime > 0) {
        GameManager::getInstance()->totalTime -= dt;
    }
    
    if (!theBoss) {
        this->checkForDoor();
        
        setCurrentTileMap(player->getPosition());
    }
    
    shieldAngle+=2.1f;
    if(shieldAngle>360){
        shieldAngle -= 360;
    }
    int counter = 0;
    float theta = 0;
    for(auto shield : playerShields){
        theta = (shieldAngle + 360*counter/playerShields.size())*3.14f/180;
        shield->setPosition(player->getPosition() + Vec2(cos(theta)*20, sin(theta)*20));
        counter++;
    }
    
    TMXTiledMap* map = getTileMap(player->getPosition());
    if (map->getTag() < MAP_FOUND) {
        Point plPos = this->tileCoordForPosition(player->getPosition() - map->getPosition(), map); //1
        if (plPos.x >  1 && plPos.y > 1 && plPos.x < map->getMapSize().width - 2 && plPos.y < map->getMapSize().height - 2) {
            roomNumber++;
            srand(GameManager::getInstance()->getTodaysRandom() + roomNumber + stageIndex);
            if (map->getTag() == MAP_NORMAL || map->getTag() == MAP_BLUE_KEY) {
                addEnemiesToMap(map, currentLevelScore, map->getTag() == MAP_BLUE_KEY);
            }
            
            setStage(map);
            currentLevelScore += 20;
            
            updateMiniMap();
        }
    }
    
    bool left = GameManager::getInstance()->leftPressed;
    bool right = GameManager::getInstance()->rightPressed;
    bool jump = GameManager::getInstance()->jumpPressed || GameManager::getInstance()->upPressed;
    //GameManager::getInstance()->jumpPressed = false;
    if (player->vehicleType == VEHICLE_TOP ||
        player->vehicleType == VEHICLE_BROOM) {
        
    }else{
        if(player->jumped) GameManager::getInstance()->jumpPressed = false;
    }
    if (!isReloading && stageIndex != STAGE_ENTRANCE) {
        Vec2 pos = GameManager::getInstance()->getHudLayer()->cursorLayer->cursor->getPosition();
        float angle = -GameManager::getInstance()->getAngle(pos, center);
        if (isAutoTargetingOn) {
            
        }else{
            if(!HUD->cursorLayer->isMouseLeftDown && !GM->firePressed){
                bool shouldChangeCursor = false;
                if(player->velocity.x < 0){
                    angle = 180;
                    shouldChangeCursor = true;
                }else if(player->velocity.x > 0){
                    angle = 0;
                    shouldChangeCursor = true;
                }
                if (shouldChangeCursor) {
                    int radius = 170;
                    float dx = cosf(angle*3.14f/180) * radius;
                    float dy = sinf(angle*3.14f/180) * radius;
                    GameManager::getInstance()->getHudLayer()->cursorLayer->cursor->setPosition(center + Point(dx, dy));
                }
            }
        }
        player->setGunPos(angle);
    }
    
    GameManager::getInstance()->upPressed = false;
    
    if(jump && player->onGround){
        if(player->currentPlatform){
            for(auto buddy: buddyArray){
                if (buddy->moveType == PET_MOVE_WALK) {
                    buddy->wantsToJump = true;
                }
            }
        }
    }
    if (player->freezed) {
        jump = false;
        left = false;
        right = false;
    }
    player->wantsToJump = jump;
    player->wantsToGoLeft = left;
    player->wantsToGoRight = right;
    if((left || right) && player->onGround && !player->isFootStepStarted){
        player->startFootStep();
    }
    if((!left && !right && player->isFootStepStarted) || jump || !player->onGround){
        player->stopFootStep();
    }
    
    if (!left && !right) {
        player->wantsToStop = true;
    }else{
        player->wantsToStop = false;
    }
    
    if (!playerIgnoreGravity) {
        if (player->vehicleType != VEHICLE_NONE) {
            player->updatePositionForSuperBoard(dt, jump, left, right);
        }else{
            player->updatePosition(dt);
        }
    }
    
    for(auto platform: movingPlatformArray)
    {
        platform->updatePosition(dt);
    }
    
    if (!isGameOver && !playerIgnoreGravity) {
        checkForAndResolveCollisions(player);
    }
    
    if (!isGameOver) {
        for(auto drop: droppableArray){
            if (drop->ignoreGravity) {
                continue;
            }
            drop->updatePosition(dt);
            checkForAndResolveCollisions(drop);
        }
    }
    
    for(auto buddy: buddyArray){
        if (buddy->moveType == PET_MOVE_WALK) {
            if(!player->currentPlatform)
                buddy->movingSchedule(dt);
            else{
                buddy->setPosition(player->getPosition() + Point(0, buddy->getContentSize().height/2 - player->getContentSize().height/2));
            }
        }else if(buddy->moveType == PET_MOVE_FLY) {
            skyBuddyCurrentHeight += isSkyBuddyFloatingUp?1:-1;
            if (skyBuddyCurrentHeight > skyBuddyHeight + skyBuddyFloatingHeight) {
                skyBuddyCurrentHeight = skyBuddyHeight + skyBuddyFloatingHeight;
                isSkyBuddyFloatingUp = false;
            }else if(skyBuddyCurrentHeight < skyBuddyHeight - skyBuddyFloatingHeight){
                skyBuddyCurrentHeight = skyBuddyHeight - skyBuddyFloatingHeight;
                isSkyBuddyFloatingUp = true;
            }
            
            if (skyBuddyXDistance < player->getPosition().x - skyBuddyXDistanceMax) {
                skyBuddyXDistance = player->getPosition().x - skyBuddyXDistanceMax;
            }else if(skyBuddyXDistance > player->getPosition().x + skyBuddyXDistanceMax){
                skyBuddyXDistance = player->getPosition().x + skyBuddyXDistanceMax;
            }
            
            
            
            float yGap = player->getPosition().y + skyBuddyCurrentHeight - skyBuddy->getPosition().y;
            float xGap = skyBuddyXDistance - skyBuddy->getPosition().x;
            skyBuddy->setPosition(skyBuddy->getPosition() + Point(xGap/20, yGap/20));
        }
        if(player->isTowardLeft != buddy->isTowardLeft)
            buddy->setFlippedX(player->isTowardLeft);
    }
    // Buddy position
    if (groundBuddy) {
        /*heroPositionArray->addControlPoint(player->getPosition() + Point(0, -4));
         if (heroPositionArray->count() > 9) {
         if(player->isTowardLeft != groundBuddy->isTowardLeft)
         groundBuddy->setFlippedX(player->isTowardLeft);
         groundBuddy->setPositionForBoth(heroPositionArray->getControlPointAtIndex(0));
         heroPositionArray->removeControlPointAtIndex(0);
         }*/
    }
    
    
    
    //    if (GameManager::getInstance()->downPressed && player->oneWayCancelY == 0) {
    //        player->oneWayCancelY = player->collisionBoundingBox().origin.y;
    //    }
    
    
    if(GameManager::getInstance()->cPressed && player->vehicleType != VEHICLE_NONE){
        getOffVehicle();
    }
    
    if (!isCameraInCustomMoving) {
        Point targetPos = player->getPosition() + extraCameraPos;
        float cameraPos = powf(_cameraPoint.x, 2) + powf(_cameraPoint.y, 2);
        float cameraTargetPos = powf(targetPos.x, 2) + powf(targetPos.y, 2);
        if(abs(cameraPos - cameraTargetPos) > 500){
            
        }
        this->setViewPointCenter(player->getPosition());
    }
    heroLight->setPosition(player->getPosition());
    
    testingFlag = !testingFlag;
    
    // fire
    if (isReloading) {
        reloadingCoolTimeSpan -= dt;
        
        if (reloadingCoolTimeSpan <= 0) {
            reloadingCoolTimeSpan = reloadingCoolTime;
            isReloading = false;
            player->lastAngle = -90;
            int maxLoadedCount = getMaxLoadedBulletCount(player->weapon);
            //            int totalBulletCount = getTotalBulletCountAtSlot(currentSlot);
            int bulletCountToLoad = maxLoadedCount;
            if (maxLoadedCount > totalBulletCount && getMaxTotalBulletCount(player->weapon) > 0) {
                bulletCountToLoad = totalBulletCount;// - loadedBulletCount;
            }
            GameManager::getInstance()->getHudLayer()->setBulletCount(bulletCountToLoad, getMaxLoadedBulletCount(player->weapon));
//            player->hand->setAnchorPoint(Point(0, 0));
            //player->lastAngle = 0;
            player->setGunPos(-GameManager::getInstance()->getAngle(GameManager::getInstance()->getHudLayer()->cursorLayer->cursor->getPosition(), center));
            loadedBulletCount = bulletCountToLoad;
            log("loaded");
        }
    }
    
    
    GameManager::getInstance()->firePressed = GameManager::getInstance()->getHudLayer()->cursorLayer->isMouseLeftDown;
    if (playerFireCoolTime > 0) {
        playerFireCoolTime -= dt;
    }
    
    
    
    if (GameManager::getInstance()->firePressed) {
        if (playerFireCoolTime > 0) {
//            playerFireCoolTime -= dt;
        }else{
            if (playerFireCoolTimeMax < 0.001) {
                playerFireCoolTimeMax = 0.3f;
            }
            playerFireCoolTime = playerFireCoolTimeMax;
            fire();
        }
    }else if(GameManager::getInstance()->getHudLayer()->cursorLayer->isMouseRightDown){
        if (!isReloading) {
            reload();
        }
    }else{
//        playerFireCoolTime = 0;
        isFired = false;
        removeLaser();
        //        if (player->weapon == WEAPON_GUN) {
        //            playerFireCoolTime = 0;
        //        }
    }
    
    isWaterBoy = false;
    for(auto drop: droppableMissileArray){
        drop->updatePosition(dt);
        if(checkDroppableMissileCollision(drop)) break;
    }
    if (isWaterBoy) {
        Missile* missile = Missile::createMissile(UNIT_MISSILE_DROPPABLE, 1, 0, 0, "waterBoy0.png");
        missile->isFregile = false;
        missile->movingAngle = waterBoyAngle;
        missile->setFlippedX(isWaterBoyFlip);
        missile->weight = 0.5f;
        missile->extraSpeed = 50;
        missile->runAnimation("waterBoy", true);
        missile->setPosition(waterBoyPos);
        missile->velocity = Point(isWaterBoyFlip?100:-100, 10);
        missile->wantsToGoRight = !missile->isTowardLeft;
        missile->wantsToGoLeft = missile->isTowardLeft;
        missile->secondTag = UNIT_BLUE_POT_WATERBALL;
        spriteBatch->addChild(missile);
        droppableMissileArray.pushBack(missile);
        enemyMissileArray.pushBack(missile);
    }
    
    // buddy fire
    Droppable* target;
    for(auto buddy: buddyArray){
        target = findTarget(buddy->monitoringDistance);
        if(!buddy || !buddy->isReadyToFire || target == NULL) continue;
        
        buddy->fire();
        
        if(buddy->missile->getTag() == UNIT_MISSILE_DROPPABLE){
            droppableMissileArray.pushBack(buddy->missile);
            heroMissileArray.pushBack(buddy->missile);
        }else if(buddy->missile->getTag() == UNIT_MISSILE_STRAIGHT){
            straightMovingArray.pushBack(buddy->missile);
            heroMissileArray.pushBack(buddy->missile);
        }else if(buddy->missile->getTag() == UNIT_MISSILE_CUSTOM){
            customMovingArray.pushBack(buddy->missile);
            heroMissileArray.pushBack(buddy->missile);
        }else if(buddy->missile->getTag() == UNIT_MISSILE_CHASING){
            //            chasingMissileArray.pushBack(buddy->missile);
            heroMissileArray.pushBack(buddy->missile);
        }
        if(buddy->moveType == PET_MOVE_FLY){
            buddy->missile->movingAngle = GameManager::getInstance()->getAngle(target->getPosition(), buddy->getPosition());
            buddy->missile->setRotation(-buddy->missile->movingAngle);
        }
        buddy->missile = NULL;
    }
    
    //    player->armatureBody->setPosition(player->getPosition());
    //    this->updateIndicators();
    if(player->getTag() == CHARACTER_BREE){
        coinMagnet();
    }
    stickTimeLeft -= dt;
    if (stickTimeLeft < 0 && stageIndex != STAGE_ENTRANCE) {
        GameManager::getInstance()->playSoundEffect(SOUND_STICK_SOUND, (rand()%40)*0.01f, (10 - rand()%20)*0.1f);
        stickTimeLeft = 0.1f + (rand()%40)*0.1f;
    }
    updateFireStick(dt);
    this->chasingMissileUpdate(dt);
    this->enemyUpdate(dt);
    this->missileUpdate(dt);
    this->coinUpdate(dt);
    this->npcUpdate(dt);
    this->missileEffectUpdate(dt);
    this->enemyFireLoop(dt);
    this->destructableUpdate();
}
void HelloWorld::coinMagnet(){
    int range = 80;
    float x, y;
    Point pos= player->getPosition() + Point(8, 16);
    for(auto coin: coinArray){
        x = pos.x - coin->getPosition().x;
        y = pos.y - coin->getPosition().y;
        
        if(sqrt(x*x + y*y) < range && (coin->getTag() == UNIT_COIN_PART || coin->getTag() == UNIT_COIN || coin->getTag() == UNIT_GOLD_COIN)) {
            coin->setPosition(coin->getPosition() + Point(x > 0?2:-2, y > 0?2:-2));
        }
    }
}
Sprite* HelloWorld::getTeleport(int teleport){
    for(auto tele: teleportArray){
        if(tele->getTag() == teleport)
            return tele;
    }
    return NULL;
}
void HelloWorld::gravityUpdateForFlyingOrSwimingEnemies(float dt)
{
    if (isPaused) {
        return;
    }
    if (dt > 0.05) {
        dt = 0.05;
    }
    
    for(auto drop: flyingSwimingEnemyArray){
        if (drop->currentMoveType == ENEMY_MOVE_FLY_TO_HERO_FOLLOW) {
            bool flip = drop->getPosition().x > drop->target->getPosition().x;
            if (flip != drop->isFlippedX()) {
                drop->setFlippedX(flip);
            }
            drop->updatePositionToFlyToFollowTarget(dt);
            checkForAndResolveCollisions(drop);
        }else if (drop->currentMoveType == ENEMY_MOVE_SWIM_TO_HERO_FOLLOW) {
            drop->updatePositionToFlyToFollowTarget(dt);
            TMXTiledMap* map = getTileMap(drop->getPosition());
            int tileId = this->getTileAtPosition(drop->desiredPosition, TAG_FORE_LAYER, map);
            if (tileId >= 0) { // check if out of water
                drop->desiredPosition = drop->getPosition();
            }
            checkForAndResolveCollisions(drop);
            
            //            checkForAndResolveCollisionsForWater(drop);
        }
    }
}

void HelloWorld::gravityUpdateForCoins(float dt)
{
    if (isPaused) {
        return;
    }
    if (dt > 0.05) {
        dt = 0.05;
    }
    
    for(auto drop: droppableCoinArray)
    {
        drop->updatePosition(dt);
        Point vel = drop->velocity;
        bool isGround = drop->onGround;
        checkForAndResolveCollisions(drop);
        if (!isGround && drop->onGround) {
            if (vel.y < -30) {
                drop->velocity = Point(vel.x*0.8, -vel.y*0.6);
                drop->stopActionByTag(ACTION_TAG_ROTATION);
                auto rotate = RotateBy::create(1, rand()%1960);
                rotate->setTag(ACTION_TAG_ROTATION);
                drop->runAction(rotate);
            }else{
                drop->wantsToStop = true;
                drop->stopActionByTag(ACTION_TAG_ROTATION);
            }
        }
        //        checkForAndResolveCollisionsForBouncing(drop);
    }
}
void HelloWorld::gravityUpdateForStraight(float dt)
{
    if (isPaused) {
        return;
    }
    for(auto drop: straightMovingArray)
    {
        drop->updatePositionForStraight(dt);
    }
    bool allClear = false;
    do
    {
        allClear = true;
        for(auto drop: straightMovingArray)
        {
            if(checkForAndResolveCollisionsForMissile(drop)){
                allClear = false;
                break;
            }
        }
    }while(!allClear);
}
void HelloWorld::gravityUpdateForCustomMoving(float dt)
{
    if (isPaused) {
        return;
    }
    for(auto drop: customMovingArray)
    {
        drop->desiredPosition = drop->getPosition();
    }
    bool allClear = false;
    do
    {
        allClear = true;
        for(auto drop: customMovingArray)
        {
            if (drop->secondTag == UNIT_MISSILE_THROUGH) {
                continue;
            }
            if(checkForAndResolveCollisionsForMissile(drop)){
                allClear = false;
                break;
            }
        }
    }while(!allClear);
}
void HelloWorld::chasingMissileUpdate(float dt)
{
    for (auto drop: chasingMissileArray)
    {
        if (drop->getTag() == UNIT_MISSILE_CHASING && drop->target != NULL) {
            float xGap = drop->getPosition().x - drop->target->getPosition().x;
            float yGap = drop->getPosition().y - drop->target->getPosition().y;
            
            double th = atan2(yGap, xGap);
            double angle = th*180/3.14 + 180;
            float gap = angle - drop->movingAngle;
            drop->movingAngle += gap > 0?(gap > 180?-2:2):(gap < -180?2:-2);
            //            drop->setRotation(-angle);
            drop->setRotation(-drop->movingAngle);
            if(drop->movingAngle < 0)
                drop->movingAngle += 360;
            if (drop->movingAngle > 360) {
                drop->movingAngle -= 360;
            }
            drop->setFlippedY(drop->movingAngle > 90 && drop->movingAngle < 270);
        }
        drop->updatePositionForStraight(dt);
        
        checkForAndResolveCollisionsForMissile(drop);
    }
}
void HelloWorld::setViewPointCenter(Point position)
{
    float scale = getScale();
    int x = position.x*scale;
    int y = position.y*scale;
    
    dnMiniMap->setPosition(Point(currentMapSize.width/2 - position.x*miniMapScale, currentMapSize.height/2 - position.y*miniMapScale));
    
    Point actualPosition = Point(x, y);
    
    Point centerOfView = Point(size.width/2, size.height/2);
    Point viewPoint = centerOfView - actualPosition;
    float distance = GameManager::getInstance()->getHudLayer()->cursorLayer->cursor->getPosition().getDistance(center);
    float maxDistance = 300;
    if (distance > maxDistance) {
        distance = maxDistance;
    }
    float angle = GameManager::getInstance()->getAngle(GameManager::getInstance()->getHudLayer()->cursorLayer->cursor->getPosition(), center);
    Point aimingPos = Point(cos(angle*3.14/180)*distance, -sin(-angle*3.14/180)*distance);
    this->setPosition((viewPoint - aimingPos*0.7f).lerp(this->getPosition(), 0.9f));
}

void HelloWorld::showBigMiniMap(bool isBig){
    ScrollView* scrollView = (ScrollView*)dnMiniMap->getParent()->getParent();
    if (isBig) {
        float height = size.height*3/4;
        float width = miniMapSize.width*height/miniMapSize.height;
        currentMapSize = Size(width, height);
        scrollView->setContentSize(Size(width, height));
        log("mini map pos: %f, %f", scrollView->getPositionX(), scrollView->getPositionY());
        scrollView->setPosition(Point(size.width/2 + width/2, size.height/2 + height/2));
        scrollView->setBackGroundColorOpacity(200);
    }else{
        currentMapSize = miniMapSize;
        scrollView->setContentSize(miniMapSize);
        log("mini map pos: %f, %f", scrollView->getPositionX(), scrollView->getPositionY());
        scrollView->setPosition(Point(size.width - 10, size.height - 10));
        scrollView->setBackGroundColorOpacity(100);
    }
    Node* dnHero = scrollView->getInnerContainer()->getChildByName("Hero");
    if (dnHero != nullptr) {
        dnHero->setPosition(scrollView->getContentSize()/2);
        dnHero->setPosition(dnHero->getPosition() + Point(-TILE_SIZE*0.3f*0.5f, -TILE_SIZE*0.3f*0.5f));
    }
    
    scrollView->setInnerContainerSize(scrollView->getContentSize());
}
void HelloWorld::setCurrentTileMap(Point pos){
    for(auto map: mapArray){
        if (map->getBoundingBox().containsPoint(pos)) {
            //            if (currentTileMap != map) {
            //                currentTileMap = map;
            //                stageLayer = currentTileMap->getLayer("stage");
            //                foreLayer = currentTileMap->getLayer("fore");
            //            }
            break;
        }
    }
}
TMXTiledMap* HelloWorld::getTileMap(Point pos){
    for(auto map: mapArray){
        if (map->getBoundingBox().containsPoint(pos)) {
            return map;
        }
    }
    return nullptr;
}

Point HelloWorld::tileCoordForPosition(Point position, TMXTiledMap* map)
{
    int mapRowCount = map->getMapSize().height;
    int mapColumnCount = map->getMapSize().width;
    float x = floor(position.x / TILE_SIZE);
    if (x < 0) x =  0;
    if (x > mapColumnCount - 1) x = mapColumnCount - 1;
    float y = floor(((map->getMapSize().height * TILE_SIZE) - position.y) / TILE_SIZE);
    if (y < 0) y =  0;
    if (y > mapRowCount - 1) y = mapRowCount - 1;
    return Point(x, y);
}

Rect HelloWorld::tileRectFromTileCoords(Point tileCoords, TMXTiledMap* map)
{
    return Rect(tileCoords.x*TILE_SIZE + map->getPositionX(), (map->getMapSize().height - tileCoords.y - 1)*TILE_SIZE + map->getPositionY(), TILE_SIZE, TILE_SIZE);
    //    float levelHeightInPixels = tileMap->getMapSize().height * tileMap->getTileSize().height;
    //    Point origin = Point(tileCoords.x * tileMap->getTileSize().width, levelHeightInPixels - ((tileCoords.y + 1) * tileMap->getTileSize().height));
    //    return Rect(origin.x, origin.y, tileMap->getTileSize().width, tileMap->getTileSize().height);
}

/*Vector<TileInfo*> HelloWorld::getSurroundingTilesAtPosition(Point position, cocos2d::TMXLayer *layer, Droppable* p)
 {
 Point plPos = this->tileCoordForPosition(position); //1
 Vector<TileInfo*> gids;
 if(plPos.x < 1 || plPos.y < 1 || plPos.x + 2 > layer->getLayerSize().width || plPos.y + 2 > layer->getLayerSize().height){
 CCLOG("NO WAY!!!");
 return gids;
 }
 
 for (int i = 0; i < 9; i++) { //3
 int c = i % 3;
 int r = (int)(i / 3);
 Point tilePos = Point(plPos.x + (c - 1), plPos.y + (r - 1));
 if (tilePos.x < 0 || tilePos.y < 0 || tilePos.x > tileMap->getMapSize().width || tilePos.y > tileMap->getMapSize().height) {
 gids.pushBack(new TileInfo());
 continue;
 }
 int tgid = layer->getTileGIDAt(tilePos); //4
 
 Rect tileRect = this->tileRectFromTileCoords(tilePos); //5
 Value property = tileMap->getPropertiesForGID(tgid);
 TileInfo* info = TileInfo::create();
 info->gid = tgid;
 info->rect = tileRect;
 info->tileX = tilePos.x;
 info->tileY = tilePos.y;
 gids.pushBack(info); //6
 if (property.getType() != Value::Type::NONE && keyExist(property.asValueMap(), "Type")) {
 info->type = property.asValueMap().at("Type");
 }
 }
 
 gids.erase(4);
 gids.insert(6, gids.at(2));
 gids.erase(2);
 gids.swap(4 ,6);
 gids.swap(0 ,4); //7
 
 if (p->velocity.y > 0) {
 gids.swap(4, 6);
 gids.swap(5, 7);
 }
 //    CCLOG("surrounding count: %d", gids.size());
 return gids;
 }*/
int HelloWorld::getTileAtPosition(Point position, int tag, TMXTiledMap* map)
{
    float mapX = map->getPositionX();
    float mapY = map->getPositionY();
    if (position.x < mapX || position.y < mapY || mapX + map->getMapSize().width*TILE_SIZE < position.x || mapY + map->getMapSize().height*TILE_SIZE < position.y ) {
        return 5;
    }
    Point plPos = this->tileCoordForPosition(position - map->getPosition(), map); //1
    //TMXLayer* layer = map->getLayer("stage");
    //Node* parent = layer->getParent();
    return ((TMXLayer*)map->getChildByTag(tag))->getTileGIDAt(plPos); //4
}

/*Vector<TileInfo*> HelloWorld::getAllSurroundingTilesAtPosition(Point position, cocos2d::TMXLayer *layer)
 {
 Point plPos = this->tileCoordForPwwwwwwwwwosition(position); //1
 Vector<TileInfo*> gids;
 
 for (int i = 0; i < 9; i++) { //3
 int c = i % 3;
 int r = (int)(i / 3);
 Point tilePos = Point(plPos.x + (c - 1), plPos.y + (r - 1));
 
 int tgid = layer->getTileGIDAt(tilePos); //4
 
 Rect tileRect = this->tileRectFromTileCoords(tilePos); //5
 Value property = tileMap->getPropertiesForGID(tgid);
 TileInfo* info = TileInfo::create();
 info->gid = tgid;
 info->rect = tileRect;
 info->tileX = tilePos.x;
 info->tileY = tilePos.y;
 
 
 if (property.isNull()) {
 info->type = property.asValueMap().at("Type");
 }
 
 gids.pushBack(info); //6
 }
 
 //    CCLOG("surrounding count: %d", gids.size());
 return gids;
 }
 */
void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    exit(0);
#endif
}

void HelloWorld::registerScriptAccelerateHandler(int handler)
{
    //    Director::getInstance()->getEventDispatcher()->addTargetedDelegate(this, 0, true);
}


void HelloWorld::setPlayerPosition(Point position) {
    player->setPosition(position);
}
void HelloWorld::Dispose(){
    
}
void HelloWorld::addListener(){
    /*listener = cocos2d::EventListenerTouchOneByOne::create();
     listener->setSwallowTouches(false);
     
     listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
     listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
     listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
     //    listener->onTouchCancelled = CC_CALLBACK_2(MenuLayerMainMenu::onTouchCancelled, this);
     
     _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);*/
    
    
    
    /*
     listener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event)
     {
     this->onTouchBegan(touch, event);
     return false; // we did not consume this event, pass thru.
     };
     listener->onTouchMoved = [=](cocos2d::Touch* touch, cocos2d::Event* event)
     {
     this->onTouchMoved(touch, event);
     };
     listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
     {
     this->onTouchEnded(touch, event);
     };
     
     cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 30);
     */
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *unused_event){
    Point location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);
    
    /*if(location.y > size.height*3/4){
     if(true){//location.x > size.width/2){
     int nextWeapon = GameManager::getInstance()->getNextToThisWeapon(playerWeaponType);
     if (nextWeapon >= WEAPON_NO_MORE) {
     nextWeapon = WEAPON_PISTOL;
     }
     setWeaponInfo(nextWeapon);
     return false;
     }
     return false;
     }*/
    /*
     if(GameManager::getInstance()->getHudLayer()->btnLeftRight->getBoundingBox().containsPoint(location) ||
     GameManager::getInstance()->getHudLayer()->btnA->getBoundingBox().containsPoint(location) ||
     GameManager::getInstance()->getHudLayer()->btnB->getBoundingBox().containsPoint(location)){
     return true;
     }*/
    
    return true;
}

void HelloWorld::onTouchMoved(Touch *touch, Event *unused_event){
    
}
void HelloWorld::onTouchEnded(Touch *touch, Event *unused_event){
    
}
void HelloWorld::TouchesBegan(const std::vector<Touch*>& touches, Event *unused_event)
{
    Touch *touch = touches.at(0);
    Point location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);
    
    touchStart = location;
    this->unschedule(schedule_selector(HelloWorld::resetTouchStart));
    this->scheduleOnce(schedule_selector(HelloWorld::resetTouchStart), 0.5);
    
}
void HelloWorld::resetTouchStart(float dt){
    touchStart = Point(0, size.height);
}
void HelloWorld::TouchesMoved(const std::vector<Touch*>& touches, Event *unused_event)
{
    Touch *touch = touches.at(0);
    Point location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);
    
    int min = 350;
    
    int toDown = touchStart.y - location.y;
    int toLeft = touchStart.x - location.x;
    if (toLeft< 0) {
        toLeft = -toLeft;
    }
    if(toDown > toLeft && toDown > min && player->oneWayCancelY == -1){
        player->oneWayCancelY = player->collisionBoundingBox().origin.y;
    }else if(toDown < -toLeft && toDown < -min && player->vehicleType != VEHICLE_NONE){
        getOffVehicle();
    }
}

void HelloWorld::getOffVehicle(){
    int vehicle = player->vehicleType;
    player->exitingVehicle = true;
    player->getOutTheVehicle();
    setPlayerInfo();
    player->velocity = Point(player->velocity.x, player->velocity.y + 1800);
    player->onGround = false;
    player->jumped = true;
    TMXTiledMap* map = getTileMap(player->getPosition());
    float x = player->getPosition().x + 4;
    float y = player->getPosition().y - map->getTileSize().height + 20;
    Droppable* board;
    if (vehicle == VEHICLE_GOLIATH) {
        
    }else if (vehicle == VEHICLE_BROOM){
        board = Droppable::createDroppable(UNIT_BROOM, 1, 0, "rideBroom.png");
        board->setTag(UNIT_BROOM);
    }else if (vehicle == VEHICLE_CART){
        board = Droppable::createDroppable(UNIT_CART, 1, 0, "rideCart.png");
        board->setTag(UNIT_CART);
    }else if (vehicle == VEHICLE_TOP){
        board = Droppable::createDroppable(UNIT_TOP, 1, 0, "rideTop.png");
        board->setTag(UNIT_TOP);
    }else if (vehicle == VEHICLE_BIRD){
        board = Droppable::createDroppable(UNIT_BIRD, 1, 0, "rideBird.png");
        board->setTag(UNIT_BIRD);
    }else if(vehicle == VEHICLE_BALLOON){
        Sprite* spt = Sprite::createWithSpriteFrameName("rideBalloon.png");
        spriteBatch->addChild(spt);
        spt->setPosition(Point(x, y));
        float duration = 0.5f;
        spt->setAnchorPoint(Point(0, 0));
        spt->runAction(RotateBy::create(duration, 720));
        spt->runAction(MoveBy::create(duration, Point(-20, 40)));
        spt->runAction(Sequence::create(ScaleTo::create(duration, 0.1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        return;
    }
    board->setPosition(Point(x, y));
    spriteBatch->addChild(board);
    helperArray.pushBack(board);
    
    //    float time = 0.6f;
    //    MoveTo* moveUpTop = MoveTo::create(time, Point(x, y + 10));
    //    MoveTo* moveDownTop = MoveTo::create(time, Point(x, y));
    //    MoveTo* moveDownBottom = MoveTo::create(time, Point(x, y - 10));
    //    MoveTo* moveUpBottom = MoveTo::create(time, Point(x, y));
    
    //    ActionInterval* ccai = (ActionInterval*)Sequence::create(moveUpTop, moveDownTop, moveDownBottom, moveUpBottom, NULL);
    //    board->runAction(RepeatForever::create(ccai));
}

void HelloWorld::TouchesEnded(const std::vector<Touch*>& touches, Event *unused_event)
{
    Touch *touch = (Touch*)(touches.at(0));
    Point location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);

}

//void HelloWorld::TouchesCancelled(const std::vector<Touch*>& touches, Event *unused_event)
//{
//    Touch *touch = (Touch*)(touches.at(0));
//    Point location = touch->getLocationInView();
//    location = Director::getInstance()->convertToGL(location);

//}

void HelloWorld::solveCollision(Droppable* p, Point pos, Rect rect){
    
}

float HelloWorld::checkBottom(Droppable* p){
    Rect rect;
    Point pos;
    Point plPos;
    Point newPlPos;
    bool ground = false;
    bool escape = false;
    float temp = -1;
    // check bottom
    rect = p->collisionBoundingBox();
    int offset = 2;
    pos = rect.origin + Point(rect.size.width - offset, 0);
    do{
        if(pos.x <= rect.origin.x + offset){
            pos.x = rect.origin.x;
            escape = true;
        }
        TMXTiledMap* map = getTileMap(pos);
        if(map){
            plPos = this->tileCoordForPosition(pos - map->getPosition(), map);
            int tgid = ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
            if(tgid > 0){
                Rect tileRect = tileRectFromTileCoords(plPos, map);
                //            tileRect.origin.y = floorf(tileRect.origin.y);
                //            rect.origin.y = floorf(rect.origin.y);
                Rect inter = intersection(tileRect, rect);
                //            inter.size.height = floorf(inter.size.height);
                if(p->velocity.y < 0){
                    if(inter.size.width > 0 && inter.size.height > 0 && p->getCurrentY() >= inter.origin.y + inter.size.height){
                        //                    newPlPos = this->tileCoordForPosition(pos + Point(0, TILE_SIZE));
                        //                    tgid = stageLayer->getTileGIDAt(newPlPos);
                        bool isOneway = isOneWay(tgid);
                        if (isOneway || isBlock(tgid)){//tgid > 0) {
                            if(isOneway){
                                if((int)p->oneWayCancelY == inter.origin.y + inter.size.height){
                                    p->setOnGround(false);
                                    
                                    break;
                                }
                                lastClearY = rect.origin.y;
                                for(auto platform: destructablePlatformArray){
                                    if (platform->tilePos == plPos && !platform->isFallingStarted) {
                                        startFallingDestructablePlatform(platform);
                                    }
                                }
                            }
                            
                            
                            temp = inter.size.height;
                            ground = true;
                            
                            if (p->onGround) {
                                int tgid;
                                if (p->isTowardLeft) {
                                    //                                plPos = tileCoordForPosition(rect.origin + Point(0, -5), map);
                                    tgid = getTileAtPosition(rect.origin + Point(0, -5), TAG_STAGE_LAYER, map);
                                }else{
                                    //                                plPos = tileCoordForPosition(rect.origin + Point(rect.size.width, -5), map);
                                    tgid = getTileAtPosition(rect.origin + Point(rect.size.width, -5), TAG_STAGE_LAYER, map);
                                }
                                
                                if(!isBlock(tgid) && !isOneWay(tgid)){
                                    p->encounteredCliff(p->isTowardLeft?CLIFF_LEFT:CLIFF_RIGHT);
                                }
                            }
                            
                            break;
                        }
                    }
                }
                
            }
        }
        pos = pos - Point(TILE_SIZE/2, 0);
    }while(!escape);
    
    //    return ground;
    return temp;
}
void HelloWorld::checkForAndResolveCollisions(Droppable* p){
    Rect rect;
    Point pos;
    Point plPos;
    Point newPlPos;
    bool ground = false;
    bool escape = false;
    float tempTotal = 0;
    float temp = -1;
    
    //    ground = checkBottom(p);
    temp = checkBottom(p);
    if(temp > 0){
        ground = true;
        tempTotal += temp;
    }
    
    // check top
    escape = false;
    int xOffset = 2;
    rect = p->collisionBoundingBox();
    pos = rect.origin + Point(xOffset, rect.size.height);
    do{
        if(pos.x >= rect.origin.x + rect.size.width - xOffset){
            pos.x = rect.origin.x + rect.size.width - xOffset; escape = true;
        }
        
        TMXTiledMap* map = getTileMap(pos);
        if (map) {
            plPos = this->tileCoordForPosition(pos - map->getPosition(), map);
            int tgid = ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
            if(tgid > 0){
                if(!isOneWay(tgid)){
                    Rect tileRect = tileRectFromTileCoords(plPos, map);
                    Rect inter = intersection(tileRect, rect);
                    
                    if(inter.size.width > 3 && inter.size.height > 0 && inter.size.width > inter.size.height){
                        
                        if(p->velocity.y >= 0){
                            //                            plPos = this->tileCoordForPosition(pos - Point(0, TILE_SIZE));
                            //                            tgid = stageLayer->getTileGIDAt(plPos);
                            //                            property = tileMap->getPropertiesForGID(tgid);
                            if (isBlock(tgid)) {
                                p->desiredPosition.y -= inter.size.height;
                                p->velocity.y  = 0;
                                break;
                            }else if (tgid == 32432) {
                                p->desiredPosition.y -= inter.size.height;
                                p->velocity.y  = 0;
                                
                                float dur = 0.38;
                                float xDst = 20;
                                float yDst = 10;
                                int angle = 360;
                                Sprite* part = Sprite::createWithSpriteFrameName("fragilePart2.png");
                                spriteBatch->addChild(part);
                                part->setPosition(pos);
                                part->runAction(Sequence::create(JumpBy::create(dur, Point(-xDst, 0), yDst, 1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
                                ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->removeTileAt(plPos);
                                part->runAction(RotateBy::create(dur, -angle));
                                
                                part = Sprite::createWithSpriteFrameName("fragilePart0.png");
                                spriteBatch->addChild(part);
                                part->setPosition(pos);
                                part->runAction(Sequence::create(JumpBy::create(dur, Point(-xDst, yDst), yDst*2, 1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
                                ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->removeTileAt(plPos);
                                part->runAction(RotateBy::create(dur, -angle));
                                
                                part = Sprite::createWithSpriteFrameName("fragilePart3.png");
                                spriteBatch->addChild(part);
                                part->setPosition(pos);
                                part->runAction(Sequence::create(JumpBy::create(dur, Point(xDst, 0), yDst, 1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
                                ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->removeTileAt(plPos);
                                part->runAction(RotateBy::create(dur, angle));
                                
                                part = Sprite::createWithSpriteFrameName("fragilePart1.png");
                                spriteBatch->addChild(part);
                                part->setPosition(pos);
                                part->runAction(Sequence::create(JumpBy::create(dur, Point(xDst, yDst), yDst*2, 1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
                                ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->removeTileAt(plPos);
                                part->runAction(RotateBy::create(dur, angle));
                                
                                GameManager::getInstance()->playSoundEffect(SOUND_SMALL_EXPLOSION);
                                
                                GameManager::getInstance()->playSoundEffect(SOUND_COIN);
                                
                                Droppable* coin;
                                if(rand()%2 == 1){
                                    coin = Droppable::createDroppable(UNIT_COIN, 1, 0, "silverCoin0.png");
                                    coin->runAnimation("silverCoin", true);
                                }else{
                                    coin = Droppable::createDroppable(UNIT_COIN_PART, 1, 0, "coinPart.png");
                                }
                                
                                coin->velocity = Point(0, 200);
                                coinArray.pushBack(coin);
                                spriteBatch->addChild(coin);
                                droppableCoinArray.pushBack(coin);
                                coin->setPosition(pos + Point(0, 18));
                                coin->runAction(Sequence::create(DelayTime::create(10), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::coinWaitDone, this)), NULL));
                                
                                break;
                            }
                        }
                    }
                }
            }
        }
        pos = pos + Point(TILE_SIZE/2, 0);
    }while(!escape);
    
    // check left
    escape = false;
    rect = p->collisionBoundingBox();
    if(tempTotal > 0){
        rect.origin.y += tempTotal;
    }
    pos = rect.origin + Point(0, xOffset);
    do{
        if(pos.y >= rect.origin.y + rect.size.height - xOffset){
            pos.y = rect.origin.y + rect.size.height - xOffset; escape = true;
        }
        TMXTiledMap* map = getTileMap(pos);
        if (map) {
            plPos = this->tileCoordForPosition(pos - map->getPosition(), map);
            int tgid = ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
            if(isBlock(tgid)){
                Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                Rect inter = intersection(tileRect, rect);
                if(inter.size.width > 0 && inter.size.height > 0) {
                    p->desiredPosition.x += inter.size.width;
                    
                    //                        ground = checkBottom(p);
                    temp = checkBottom(p);
                    if(tempTotal > 0 && temp < 0){
                        tempTotal = 0;
                        ground = false;
                    }
                    
                    
                    if(p->isTowardLeft ){
                        p->encounteredWall();
                        if(p->getTag() == UNIT_THREE_EYES){
                            CCLOG("turned to right");
                        }
                    }
                    
                    break;
                }
            }
        }
        pos = pos + Point(0, TILE_SIZE/2);
    }while(!escape);
    
    // check right
    escape = false;
    rect = p->collisionBoundingBox();
    if(tempTotal > 0){
        rect.origin.y += tempTotal;
    }
    pos = rect.origin + Point(rect.size.width, xOffset);
    do{
        if(pos.y >= rect.origin.y + rect.size.height - xOffset){
            pos.y = rect.origin.y + rect.size.height - xOffset; escape = true;
        }
        TMXTiledMap* map = getTileMap(pos);
        if (map) {
            plPos = this->tileCoordForPosition(pos - map->getPosition(), map);
            int tgid = ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
            if(tgid){
                if (isBlock(tgid)) {
                    Rect tileRect = tileRectFromTileCoords(plPos, map);
                    Rect inter = intersection(tileRect, rect);
                    if(inter.size.width > 0 && inter.size.height > 0){
                        p->desiredPosition.x -= inter.size.width;
                        
                        //                    ground = checkBottom(p);
                        temp = checkBottom(p);
                        if(tempTotal > 0 && temp < 0){
                            tempTotal = 0;
                            ground = false;
                        }
                        
                        
                        if(!p->isTowardLeft ){
                            p->encounteredWall();
                        }
                        break;
                    }
                }
            }
        }
        
        pos = pos + Point(0, TILE_SIZE/2);
    }while(!escape);
    TMXTiledMap* map = getTileMap(pos);
    if (map) {
        if (((TMXLayer*)map->getChildByTag(TAG_FORE_LAYER))) {
            bool inWater = false;
            int tileId = getTileAtPosition(p->getPosition(), TAG_FORE_LAYER, map);
            if (tileId) {
                if(tileId == 0 || /*info->type.asString().compare("WaterSurface")*/tileId == 0){
                    inWater = true;
                }
            }
            
            if (inWater && !p->inWater) {
                spriteBatch->setLocalZOrder(-10);
                p->setInWater(true);
                //            p->setOnGround(false);
                inWaterArray.pushBack(p);
            }else if(!inWater && p->inWater){
                if(p == player)
                    spriteBatch->setLocalZOrder(3);
                p->setInWater(false);
                GameManager::getInstance()->playSoundEffect(SOUND_WATER_SPLASH);
                inWaterArray.eraseObject(p);
            }
        }
    }
    
    
    if(p == player){
        for(auto platform: movingPlatformArray)
        {
            Rect pRect = p->collisionBoundingBox();
            //        Rect pCurrentRect = player->currentBoundingBox();
            //            Rect platformRect = platform->getBoundingBox();
            Rect platformRect = platform->desiredRect;
            Rect intersectionR = intersection(pRect, platformRect);
            //        Rect intersectionWithCurrent  = intersection(pCurrentRect, platformRect);
            bool intersects = intersectionR.size.width > 0 && intersectionR.size.height > 0 && p->velocity.y < 0;
            if (intersects &&
                (platform == p->currentPlatform || (platform->desiredRect.origin.y + platform->desiredRect.size.height/2 < p->desiredPosition.y && p->velocity.y < 0))){
                p->desiredPosition = Point(p->desiredPosition.x, p->desiredPosition.y + intersectionR.size.height);
                platform->player = player;
                if (platform->isTriggerPlatform && p == player) {
                    platform->isTriggered = true;
                }
                p->currentPlatform = platform;
                p->velocity = Point(p->velocity.x, -0.0001);
                ground = true;
            }
            
            Point diff = platform->desiredRect.origin - platform->getBoundingBox().origin;
            platform->setPosition(platform->getPosition() + diff);
        }
    }
    
    if(p->onGround && !ground){
        //        if (p->getPositionY() == p->desiredPosition.y) {
        //            ground = true;
        //        }
    }
    
    if(ground){
        if(!p->onGround){
            p->onLanded();
        }
        tempTotal = floorf(tempTotal * 100) / 100;
        p->velocity.y = -0.001;
        p->desiredPosition.y += tempTotal;
    }
    
    p->setOnGround(ground);
    p->setPosition( p->desiredPosition);
}
void HelloWorld::startFallingDestructablePlatform(DestructablePlatform* platform){
    platform->isFallingStarted = true;
    platform->runAction(Sequence::create(MoveBy::create(platform->fallingStartTime/7, Point(2,2)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(-2,0)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(-2,-4)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(2,4)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(0,-2)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(-2,-2)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(2,2)),
                                         
                                         CallFuncN::create(CC_CALLBACK_1(HelloWorld::destructablePlatformShakingDone, this)),
                                         NULL));
}
void HelloWorld::destructablePlatformShakingDone(Ref* obj){
    DestructablePlatform* platform = (DestructablePlatform*)obj;
    platform->layer->removeTileAt(platform->tilePos);
    platform->runAction(FadeOut::create(0.5));
    platform->runAction(Sequence::create(MoveBy::create(0.5, Point(0, - 50)),
                                         DelayTime::create(5.0f),
                                         CallFuncN::create(CC_CALLBACK_1(HelloWorld::destructablePlatformWaitingDone, this)),
                                         NULL));
    
}
void HelloWorld::destructablePlatformWaitingDone(Ref* obj){
    DestructablePlatform* platform = (DestructablePlatform*)obj;
    platform->isFallingStarted = false;
    platform->layer->setTileGID(51, platform->tilePos);
    platform->runAction(FadeIn::create(0.5));
    platform->runAction(MoveTo::create(0.5, platform->originalPos));
}


bool HelloWorld::isFregile(int index){
    switch (index) {
        case 4:
        case 5:
        case 6:
        case 33:
        case 70:
            return true;
            
        default:
            return false;
    }
}
bool HelloWorld::isBlock(int index){
    switch (index) {
        //case 1:
        case 5:
        case 6:
        case 7:
        case 13:
        case 14:
        case 15:
        case 45:
        case 47:
        case 77:
        case 78:
        case 79:
        case 34:
        case 37:
        case 38:
        case 39:
        case 69:
        case 70:
        case 71:
        case 322:
        case 266:
        case 267:
        case 268:
        case 299:
        case 300:
        case 298:
        case 330:
        case 331:
        case 332:
            return true;
        default:
            return false;
    }
}
bool HelloWorld::isOneWay(int index){
    switch (index) {
        case 130:
        case 133:
        case 134:
        case 135:
        case 324:
        case 325:
        case 356:
        case 357:
        case 258:
        case 261:
        case 262:
        case 263:
            return true;
            
        default:
            return false;
    }
}
bool HelloWorld::checkDroppableMissileCollision(Droppable* p){
    //    Vector<TileInfo*> tiles = getSurroundingTilesAtPosition(p->getGravityPosition(), stageLayer, p); //1
    /*if (tiles.size() < 1) {
     droppableMissileArray.eraseObject(p);
     heroMissileArray.eraseObject(p);
     enemyMissileArray.eraseObject(p);
     p->removeFromParentAndCleanup(true);
     CCLOG("tileSize is wrong");
     return true;
     }*/
    //    bool ground = false;
    //    Point current = p->getPosition();
    Rect pRect = p->collisionBoundingBox(); //2
    p->setPosition( p->desiredPosition);
    int gid;
    TMXTiledMap* map = getTileMap(p->getPosition());
    Point plPos = this->tileCoordForPosition(p->getPosition(), map); //1
    Rect tileRect = tileRectFromTileCoords(plPos, map);//info->rect;
    if(p->isFregile){
        gid = getTileAtPosition(p->getPosition(), TAG_STAGE_LAYER, map);
        if (gid) {
            if (isBlock(gid)) {
                if (p->secondTag == UNIT_POT) {
                    isWaterBoy = true;
                    waterBoyPos = p->getPosition() + Point(0, 20);
                    waterBoyAngle = p->velocity.x<0?180:0;
                    isWaterBoyFlip = p->velocity.x<0;
                }
                
                if (p->secondTag == WEAPON_FIRE_BOTTLER){
                    splashDemage(p->getPosition(), 50, p->energy);
                    GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
                }
                
                droppableMissileArray.eraseObject(p);
                heroMissileArray.eraseObject((Missile*)p);
                enemyMissileArray.eraseObject((Missile*)p);
                enemyArray.eraseObject((EnemyBase*)p);
                p->removeFromParent();
                return true;
            }
            
        }
    }else{
        if(p->velocity.y < 0){
            if(isBlock(gid)){
                p->velocity.y = 0;
                Rect intersectionR = intersection(pRect, tileRect);
                if(intersectionR.size.width > 0 && intersectionR.size.height > 0){
                    p->setPosition(p->getPosition() + Point(0, intersectionR.size.height));
                }
            }
        }
        if(p->velocity.y > 0){
            p->velocity.y = -0.0021;
            Rect intersectionR = intersection(pRect, tileRect);
            if(intersectionR.size.width > 0 && intersectionR.size.height > 0){
                p->setPosition(p->getPosition() + Point(0, -intersectionR.size.height));
            }
        }
        
        
        if (p->velocity.x > 0) {
            gid = getTileAtPosition(p->getPosition() + Point(p->getContentSize().width/2, 0), TAG_STAGE_LAYER, map);
        }else if(p->velocity.x < 0){
            gid = getTileAtPosition(p->getPosition() - Point(p->getContentSize().width/2, 0), TAG_STAGE_LAYER, map);
        }
        if (isBlock(gid)) {
            droppableMissileArray.eraseObject(p);
            heroMissileArray.eraseObject((Missile*)p);
            enemyMissileArray.eraseObject((Missile*)p);
            enemyArray.eraseObject((EnemyBase*)p);
            p->removeFromParent();
            return true;
        }
    }
    return false;
}

bool HelloWorld::intersectsRect(cocos2d::Rect srcRect, cocos2d::Rect dstRect)
{
    return !(srcRect.getMaxX() <= dstRect.getMinX() ||
             dstRect.getMaxX() <= srcRect.getMinX() ||
             srcRect.getMaxY() <= dstRect.getMinY() ||
             dstRect.getMaxY() <= srcRect.getMinY());
}
Rect HelloWorld::intersection(Rect source, Rect rect)
{
    Rect intersection;
    intersection = Rect(source.getMinX()>rect.getMinX()?source.getMinX():rect.getMinX(),
                        source.getMinY()>rect.getMinY()?source.getMinY():rect.getMinY(),
                        0,
                        0);
    intersection.size.width = (source.getMaxX()<rect.getMaxX()?source.getMaxX():rect.getMaxX())-intersection.getMinX();
    intersection.size.height = (source.getMaxY()<rect.getMaxY()?source.getMaxY():rect.getMaxY())-intersection.getMinY();
    return intersection;
}
void HelloWorld::checkForAndResolveCollisionsForWater(Droppable* p){
    
    Point current = p->getGravityPosition();
    TMXTiledMap* map = getTileMap(current);
    Rect pRect = p->collisionBoundingBox();
    int gid;
    for (int i = 0; i < 8; i++) {
        if (i == 0) {
            current = current + Point(0, -TILE_SIZE);
        }else if(i == 1) {
            current = current + Point(0, TILE_SIZE);
        }else if(i == 2) {
            current = current + Point(-TILE_SIZE, 0);
        }else if(i == 3) {
            current = current + Point(TILE_SIZE, 0);
        }else if(i == 4) {
            current = current + Point(-TILE_SIZE, -TILE_SIZE);
        }else if(i == 5) {
            current = current + Point(TILE_SIZE, -TILE_SIZE);
        }else if(i == 6) {
            current = current + Point(-TILE_SIZE, TILE_SIZE);
        }else if(i == 7) {
            current = current + Point(TILE_SIZE, TILE_SIZE);
        }
        
        gid = getTileAtPosition(current, TAG_STAGE_LAYER, map);
        if (gid) {
            Point plPos = this->tileCoordForPosition(current - map->getPosition(), map); //1
            Rect tileRect = tileRectFromTileCoords(plPos, map);//info->rect;
            
            Rect intersectionR = intersection(pRect, tileRect);
            if (intersectionR.size.width > 0 && intersectionR.size.height > 0){
                
                if (isBlock(gid)) {
                    //                    int tileIndx = tiles->indexOfObject(dic);
                    int tileIndx = i;
                    
                    if (tileIndx == 0 && p->velocity.y <= 0) {
                        //tile is directly below Hero
                        p->desiredPosition = Point(p->desiredPosition.x, p->desiredPosition.y + intersectionR.size.height);
                        p->velocity = Point(p->velocity.x, 0);
                        //                        p->setOnGround(true);
                        
                    } else if (tileIndx == 1) {
                        //tile is directly above Hero
                        p->desiredPosition = Point(p->desiredPosition.x, p->desiredPosition.y - intersectionR.size.height);
                        p->velocity = Point(p->velocity.x, 0.0);
                    } else if (tileIndx == 2) {
                        //tile is left of Hero
                        p->desiredPosition = Point(p->desiredPosition.x + intersectionR.size.width, p->desiredPosition.y);
                        
                    } else if (tileIndx == 3) {
                        //tile is right of Hero
                        p->desiredPosition = Point(p->desiredPosition.x - intersectionR.size.width, p->desiredPosition.y);
                    } else {
                        if (p->velocity.y > 0) {    // rasing
                            if (tileIndx > 5) {
                                p->desiredPosition = Point(p->desiredPosition.x, p->desiredPosition.y - intersectionR.size.height);
                                p->velocity = Point(p->velocity.x, 0.0);
                            }else if (tileIndx == 4){
                                p->desiredPosition = Point(p->desiredPosition.x + intersectionR.size.width, p->desiredPosition.y);
                            }else if (tileIndx == 5){
                                p->desiredPosition = Point(p->desiredPosition.x - intersectionR.size.width, p->desiredPosition.y);
                            }
                        }else{  // falling
                            if (tileIndx > 5) {
                                p->desiredPosition = Point(p->desiredPosition.x, p->desiredPosition.y + intersectionR.size.height);
                                p->velocity = Point(p->velocity.x, 0);
                                //                                p->setOnGround(true);
                            }else if (tileIndx == 4){
                                p->desiredPosition = Point(p->desiredPosition.x + intersectionR.size.width, p->desiredPosition.y);
                            }else if (tileIndx == 5){
                                p->desiredPosition = Point(p->desiredPosition.x - intersectionR.size.width, p->desiredPosition.y);
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (getTileAtPosition(p->desiredPosition, TAG_FORE_LAYER, map) > 0) { // check if out of water
        p->desiredPosition = p->getPosition();
    }
    
    p->setPosition(p->desiredPosition);
}

void HelloWorld::checkForAndResolveCollisionsForBouncing(Droppable* p){
    Rect rect;
    Point pos;
    Point plPos;
    bool ground = false;
    
    bool escape = false;
    
    
    //    p->setOnGround(false);
    // check top
    escape = false;
    rect = p->collisionBoundingBox();
    pos = rect.origin + Point(0, rect.size.height);
    TMXTiledMap* map;
    do{
        if(pos.x >= rect.origin.x + rect.size.width){
            pos.x = rect.origin.x + rect.size.width; escape = true;
        }
        
        map = getTileMap(pos);
        plPos = this->tileCoordForPosition(pos - map->getPosition(), map);
        int tgid = ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
        if(tgid > 0){
            Value property = map->getPropertiesForGID(tgid);
            if (property.getType() != Value::Type::NONE && keyExist(property.asValueMap(), "Type")) {
                if(property.asValueMap().at("Type").asString().compare("OneWay") != 0){
                    Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                    Rect inter = intersection(tileRect, rect);
                    if(inter.size.width > 3 && inter.size.height > 0 && inter.size.width > inter.size.height){
                        if(p->velocity.y > 0){
                            p->desiredPosition.y -= inter.size.height;
                            p->velocity.y  = 0;
                            break;
                        }
                    }
                }
            }
        }
        pos = pos + Point(TILE_SIZE/2, 0);
    }while(!escape);
    
    // check left
    escape = false;
    rect = p->collisionBoundingBox();
    pos = rect.origin + Point(0, rect.size.height);
    do{
        if(pos.y <= rect.origin.y){
            break;//pos.y = rect.origin.y + rect.size.height; escape = true;
        }
        map = getTileMap(pos);
        plPos = this->tileCoordForPosition(pos - map->getPosition(), map);
        int tgid = ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
        if(tgid > 0 && p->velocity.x <= 0){
            Value property = map->getPropertiesForGID(tgid);
            if(property.getType() != Value::Type::NONE && property.asValueMap().at("Type").asString().compare("OneWay") != 0){
                if (property.getType() != Value::Type::NONE && keyExist(property.asValueMap(), "Type")) {
                    Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                    Rect inter = intersection(tileRect, rect);
                    if(inter.size.width > 0 && inter.size.height > 0) {
                        p->desiredPosition.x += inter.size.width;
                        
                        if(p->isTowardLeft ){
                            p->encounteredWall();
                            if(p->getTag() == UNIT_THREE_EYES){
                                CCLOG("turned to right");
                            }
                        }
                        
                        break;
                    }
                }
            }
        }
        pos = pos - Point(0, TILE_SIZE/2);
    }while(!escape);
    
    // check right
    escape = false;
    rect = p->collisionBoundingBox();
    pos = rect.origin + rect.size;
    do{
        if(pos.y <= rect.origin.y){
            break;//pos.y = rect.origin.y; escape = true;
        }
        map = getTileMap(pos);
        plPos = this->tileCoordForPosition(pos - map->getPosition(), map);
        int tgid = ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
        if(tgid > 0 && p->velocity.x >= 0){
            Value property = map->getPropertiesForGID(tgid);
            if (property.getType() != Value::Type::NONE && keyExist(property.asValueMap(), "Type")) {
                if(property.asValueMap().at("Type").asString().compare("OneWay") != 0){
                    Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                    Rect inter = intersection(tileRect, rect);
                    if(inter.size.width > 0 && inter.size.height > 0){
                        p->desiredPosition.x -= inter.size.width;
                        
                        if(!p->isTowardLeft ){
                            p->encounteredWall();
                            if(p->getTag() == UNIT_THREE_EYES){
                                CCLOG("turned to left");
                            }
                        }
                        break;
                    }
                }
            }
        }
        
        pos = pos - Point(0, TILE_SIZE/2);
    }while(!escape);
    
    // check bottom
    rect = p->collisionBoundingBox();
    int offset = 2;
    pos = rect.origin + Point(rect.size.width - offset, 0);
    do{
        if(pos.x <= rect.origin.x + offset){
            pos.x = rect.origin.x;
            escape = true;
        }
        map = getTileMap(pos);
        plPos = this->tileCoordForPosition(pos - map->getPosition(), map);
        int tgid = ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
        if(tgid > 0){
            Rect tileRect = this->tileRectFromTileCoords(plPos, map);
            Rect inter = intersection(tileRect, rect);
            
            if(p->velocity.y < 0){
                if(inter.size.width > 0 && inter.size.height > 0 && p->getCurrentY() >= inter.origin.y + inter.size.height){
                    if (tgid > 0) {
                        if(isOneWay(tgid)){
                            if((int)p->oneWayCancelY == inter.origin.y + inter.size.height){
                                p->setOnGround(false);
                                
                                break;
                            }
                            lastClearY = rect.origin.y;
                            for(auto platform: destructablePlatformArray){
                                if (platform->tilePos == plPos && !platform->isFallingStarted) {
                                    startFallingDestructablePlatform(platform);
                                }
                            }
                        }
                        
                        //                            p->desiredPosition.y += inter.size.height;
                        //                        temp = inter.size.height;
                        p->desiredPosition.y += inter.size.height;
                        //                        p->setOnGround(true);
                        //                        p->velocity.y = 0;
                        p->velocity = Point(p->velocity.x*0.8, -p->velocity.y*0.6);
                        
                        
                        break;
                    }
                }
            }
            
        }
        pos = pos - Point(TILE_SIZE/2, 0);
    }while(!escape);
    
    p->setPosition( p->desiredPosition);
}

bool HelloWorld::checkForAndResolveCollisionsForMissile(Droppable* p){
    p->setPosition( p->desiredPosition); //7
    
    TMXTiledMap* map = getTileMap(p->getPosition());
    int gid = 0;
    if (map != nullptr) {
        gid = getTileAtPosition(p->getPosition(), TAG_STAGE_LAYER, map);
    }
    
    if(map == nullptr || isBlock(gid)){
        /*if (p->velocity.x > 0){
         runEffect(EFFECT_EXPLODE_SMALL,//EFFECT_BULLET_AGAINST_WALL,
         p->getPosition());//Point(intersectionR.origin.x, intersectionR.origin.y + intersectionR.size.height/2));//p->getPosition());
         }else{
         runEffect(EFFECT_EXPLODE_SMALL,//EFFECT_BULLET_AGAINST_WALL_LEFT,
         p->getPosition());//Point(intersectionR.origin.x + intersectionR.size.width,intersectionR.origin.y + intersectionR.size.height/2));//p->getPosition());
         }*/
        runEffect(EFFECT_BULLET_WALL_SMALL, p->getPosition(), p->getRotation());
        if(p->secondTag != SECOND_TAG_NULL){
            if(p->secondTag == WEAPON_FIRE_BOTTLER ||
               p->secondTag == WEAPON_RIFLE_GRENADE ||
               p->secondTag == WEAPON_BAZOOKA ||
               p->secondTag == WEAPON_ROCKET_LAUNCHER ||
               p->secondTag == WEAPON_GUIDED_ROCKET_LAUNCHER){
                splashDemage(p->getPosition(), 40, p->energy/2);
                CCLOG("in missile updat");
            }
            if (p->secondTag == UNIT_BAZOOKA_BIG) {
                runEffect(EFFECT_EXPLODE_HUGE, p->getPosition(), p->getRotation());
            }
            
            if (p->secondTag == UNIT_MACHINEGUN_BIG) {
                double angle = -GameManager::getInstance()->getAngle(p->getPosition(), player->getPosition() + Point(0, TILE_SIZE/2)) + 180;
                
                Point gunTipPos = p->getPosition();
                Droppable* missile = Droppable::createDroppable(UNIT_MISSILE_STRAIGHT, 1, 0, "bombSmallEffect0.png");
                missile->isFregile = true;
                missile->movingAngle = -angle;
                missile->extraSpeed = 200;
                Point vlct = Point(missile->extraSpeed*0.1*cos(missile->movingAngle*3.14/180),
                                   missile->extraSpeed*0.1*sin(missile->movingAngle*3.14/180));
                
                missile->setPosition(gunTipPos + vlct);
                missile->desiredPosition = gunTipPos + vlct;
                missile->weight = 0.1f;
                missile->secondTag = UNIT_MACHINEGUN_MINI;
                missile->setScale(1.5, 0.5);
                missile->setRotation(angle);
                //missile->setScale(0.1);
                spriteBatch->addChild(missile);
                straightMovingArray.pushBack(missile);
                enemyMissileArray.pushBack((Missile*)missile);
            }
            if (p->secondTag == UNIT_MACHINEGUN_MINI) {
                double angle = -GameManager::getInstance()->getAngle(p->getPosition(), player->getPosition() + Point(0, TILE_SIZE/2)) + 180;
                
                Point gunTipPos = p->getPosition();
                Droppable* missile = Droppable::createDroppable(UNIT_MISSILE_STRAIGHT, 1, 0, "bombSmallEffect0.png");
                missile->isFregile = true;
                missile->movingAngle = -angle;
                missile->extraSpeed = 100;
                Point vlct = Point(missile->extraSpeed*0.1*cos(missile->movingAngle*3.14/180),
                                   missile->extraSpeed*0.1*sin(missile->movingAngle*3.14/180));
                
                missile->setPosition(gunTipPos + vlct);
                missile->desiredPosition = gunTipPos + vlct;
                missile->weight = 0.1f;
                missile->secondTag = UNIT_MISSILE_STRAIGHT;
                //missile->setScale(0.1);
                spriteBatch->addChild(missile);
                straightMovingArray.pushBack(missile);
                enemyMissileArray.pushBack((Missile*)missile);
            }
            
            if(p->secondTag == WEAPON_FIRE_BOTTLER ||
               p->secondTag == WEAPON_BAZOOKA){
                splashDemage(p->getPosition(), 50, p->energy);
            }
        }
        
        bulletWasted = true;
        
        if(heroMissileArray.find((Missile*)p) != heroMissileArray.end()){
            heroMissileArray.eraseObject((Missile*)p);
            if(p->secondTag == WEAPON_ENERGY_GUN){
                ((Missile*)p)->victimArray.clear();
            }
        }
        if (enemyMissileArray.find((Missile*)p) != enemyMissileArray.end()) {
            enemyMissileArray.eraseObject((Missile*)p);
        }
        
        if(p->getTag() == UNIT_MISSILE_STRAIGHT) straightMovingArray.eraseObject(p);
        if(p->getTag() == UNIT_MISSILE_CHASING){
            chasingMissileArray.eraseObject(p);
            if(p->secondTag == UNIT_BEE){
                enemyArray.eraseObject((EnemyBase*)p);
            }
        }
        if(p->getTag() == UNIT_MISSILE_CUSTOM){
            //                            customMovingArray.eraseObject(p);
            missileMoveDone(p);
        }
        if(p->getTag() == UNIT_MISSILE_DROPPABLE) droppableMissileArray.eraseObject(p);
        
        enemyArray.eraseObject((EnemyBase*)p);
        if(spriteBatch->getChildren().find(p) != spriteBatch->getChildren().end()){
            spriteBatch->removeChild(p, true);
        }
        return true;
    }
    
    return false;
}
void HelloWorld::checkForDoor()
{
    
}
void HelloWorld::unscheduleAll(){
    this->unscheduleAllCallbacks();
}

void HelloWorld::showSuccessLayer(float dt)
{
    if (collectedCoinCount < MAX_COIN_COUNT) {
        GameManager::getInstance()->addCoin(collectedCoinCount);
    }
    //    GameManager::getInstance()->showInterstitialAds();
    
    isGameOver = true;
    CCLOG("star count: %d", starCount);
    player->stopFootStep();
    char buf[50];
    const char* totalStarCountKey = "Tt_SC";
    int totalStarCount = UserDefault::getInstance()->getIntegerForKey(totalStarCountKey, 0);
    
    sprintf( buf, "T%dS%dC", GameManager::getInstance()->theme, GameManager::getInstance()->currentStageIndex);
    int previousStar = UserDefault::getInstance()->getIntegerForKey(buf, 0);
    if (starCount > previousStar) {
        int starToAdd = starCount - previousStar;
        
        sprintf( buf, "T%dS%dC", GameManager::getInstance()->theme, GameManager::getInstance()->currentStageIndex);
        UserDefault::getInstance()->setIntegerForKey(buf, starCount);
        
        totalStarCount += starToAdd;
        UserDefault::getInstance()->setIntegerForKey(totalStarCountKey, totalStarCount);
    }
    
    totalStarCount = GameManager::getInstance()->getTotalStarCount();
    
    if(starCount >= 3){
        GameManager::getInstance()->addMastery(GameManager::getInstance()->theme*24 + GameManager::getInstance()->currentStageIndex);
    }
    
    GameManager::getInstance()->getHudLayer()->setVisible(true);
    activityIndicator = Sprite::create("sandglass.png");
    GameManager::getInstance()->getHudLayer()->addChild(activityIndicator);
    activityIndicator->setPosition(Point(size.width/2, size.height/2));
    activityIndicator->runAction(RepeatForever::create(RotateBy::create(1, 360)));
    GameManager::getInstance()->saveCoin();
    if (starCount >= 3) {
        showStageClearLayer(true);
    }else{
        showStageClearLayer(false);
    }
}

void HelloWorld::showStageClearLayer(bool threeStars){
    activityIndicator->setVisible(false);
    activityIndicator->stopAllActions();
    activityIndicator->removeFromParentAndCleanup(true);
    
    if (GameManager::getInstance()->life >= 0 || UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET, false)) {
        if (!UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET, false)) {
            GameManager::getInstance()->life++;
            UserDefault::getInstance()->setIntegerForKey(KEY_LIFE, GameManager::getInstance()->life);
        }
    }
    
    if (enemyKilledCount > enemyMaxCount) {
        enemyKilledCount = enemyMaxCount;
    }
    GameManager::getInstance()->getHudLayer()->showStageClearLayer(collectedCoinCount, enemyKilledCount, enemyMaxCount);
}
void HelloWorld::rateResult(Node* node){
    if (node->getTag() == MESSAGE_BOX_RESULT_OK) {
        if (GameManager::getInstance()->market == MARKET_PLAYSTORE_PAID) {
            Application::getInstance()->openURL("itms-apps://itunes.apple.com/app/id838183037");
        }else if (GameManager::getInstance()->market == MARKET_PLAYSTORE_PAID) {
            Application::getInstance()->openURL("http://play.google.com/store/apps/details?id=com.magmon.LegendDaryTheTower");
        }else if(GameManager::getInstance()->market == MARKET_TSTORE_PAID){
            Application::getInstance()->openURL("http://m.tstore.co.kr/mobilepoc/apps/appsDetail.omp?prodId=0000661603");
        }else if(GameManager::getInstance()->market == MARKET_NAVER_PAID){
            Application::getInstance()->openURL("http://m.tstore.co.kr/mobilepoc/apps/appsDetail.omp?prodId=0000661603");
        }else if(GameManager::getInstance()->market == MARKET_MAC){
            Application::getInstance()->openURL("macappstore://itunes.apple.com/app/id886115032?mt=12");
        }
        CCUserDefault::getInstance()->setBoolForKey("Rated", true);
    }
}
void HelloWorld::onUpdateResultErrorComplete(char const *status, char const *error) {
    CCLOG("Update result failed. Status: %s, Error: %s", status, error);
    activityIndicator->setVisible(false);
    activityIndicator->stopAllActions();
    activityIndicator->removeFromParentAndCleanup(true);
    
}

void HelloWorld::gameClearAnimationDone(Ref* obj)
{
    //    CCLOG("Show stage clear popup");
}

Droppable* HelloWorld::findTarget(int distance){
    Droppable* targetEnemy = NULL;
    
    float minDistance = distance;
    float x = 0;
    float y = 0;
    for(auto drop: enemyArray){
        if (drop->untouchable) {
            continue;
        }
        
        x = player->getPositionX() - drop->getPositionX();
        y = player->getPositionY() - drop->getPositionY();
        if (x < 0) x = -x;
        if (y < 0) y = -y;
        
        if (x + y < minDistance) {
            minDistance = x + y;
            targetEnemy = drop;
        }
    }
    return targetEnemy;
}
int HelloWorld::getMaxLoadedBulletCount(int weapon){
    switch (weapon) {
        case WEAPON_PISTOL: // 20 p/s
            return 10;
        case WEAPON_MAGNUM: // 25
            return 8;
        case WEAPON_UZI: // 34
            return 20;
        case WEAPON_RIFLE: // 25
            return 5;
        case WEAPON_FIRE_BOTTLER: // 42
            return 5;
        case WEAPON_ANTI_BAT: // 60
            return 10;
        case WEAPON_SNIPER_RIFLE: // 36
            return 2;
        case WEAPON_RIFLE_GRENADE: // 35
            return 5;
        case WEAPON_CROSS_BOW: // 49
            return 8;
        case WEAPON_MACHINE_GUN: // 110
            return 30;
        case WEAPON_STRIKER: // 61
            return 4;
        case WEAPON_NIFE_THROWER: // 88
            return 10;
        case WEAPON_BALL_SHOOTER: // 111
            return 15;
        case WEAPON_FLAME_THROWER: // 207
            return 100;
        case WEAPON_ROCKET_LAUNCHER: // 132
            return 4;
        case WEAPON_BAZOOKA: // 44
            return 2;
        case WEAPON_MATT_A4: // 164
            return 7;
        case WEAPON_LASER_GUN: // 161
            return 150;
        case WEAPON_ENERGY_GUN: // 85
            return 4;
        case WEAPON_GUIDED_ROCKET_LAUNCHER: // 107
            return 3;
        case WEAPON_SLUGGER: // 171
            return 10;
        case WEAPON_DRAGON_RIFLE: // 282
            return 9;
        case WEAPON_SLUGGER_NG: // 171
            return 9;
        case WEAPON_LIGHTNING_GUN:
            return 5;
        case WEAPON_SHARK_GUN:
            return 4;
        case WEAPON_LASER_COIN:
            return 150;
        default:
            return 8;
    }
}
int HelloWorld::getMaxTotalBulletCount(int weapon){
    return GM->getMaxTotalBulletCount(weapon);
}
void HelloWorld::playReloadingSoundLater(float dt){
    GameManager::getInstance()->playSoundEffect(SOUND_RELOAD);
}
//int testWeaponIndex = WEAPON_LASER_GUN - 1;
void HelloWorld::reload(){
    //    testWeaponIndex++;
    //    if (testWeaponIndex == WEAPON_NO_MORE) {
    //        testWeaponIndex = WEAPON_PISTOL;
    //    }
    //    addWeapon(testWeaponIndex);
    //    return;
    if (laser!=nullptr) {
        laser->removeFromParent();
        laser = nullptr;
        GameManager::getInstance()->stopSoundEffect(SOUND_LASER_BUZZ);
    }
    
    if (totalBulletCount <= loadedBulletCount && getMaxTotalBulletCount(player->weapon) >= 0) {
        // auto weapon change
        int lastSlot = 0;
        while(true){
            if (getWeaponAtSlot(lastSlot) < 0) {
                break;
            }
            lastSlot++;
        }
        lastSlot--;
        
        while (true) {
            if (getTotalBulletCountAtSlot(lastSlot) > 0) {
                changeWeapon(lastSlot);
                break;
            }
            lastSlot--;
            if (lastSlot <= 0) {
                changeWeapon(0);
                break;
            }
        }
        return;
    }
    isReloading = true;
    reloadingCoolTimeSpan = reloadingCoolTime;
    float reloadingTime = reloadingCoolTime;
    if(reloadingTime > 1.5f){
        GameManager::getInstance()->playSoundEffect(SOUND_RELOAD_SHORT);
        this->scheduleOnce(schedule_selector(HelloWorld::playReloadingSoundLater), reloadingTime - 1);
    }else if (reloadingTime > 0.9f) {
        GameManager::getInstance()->playSoundEffect(SOUND_RELOAD);
    }else{
        GameManager::getInstance()->playSoundEffect(SOUND_RELOAD_SHORT);
    }
    
    if (reloadingTime <= 0.5) {
        player->gun->runAction(RotateTo::create(reloadingTime*8/10, player->face->isFlippedY()?860:770));
        player->gun->runAction(EaseOut::create(MoveTo::create(reloadingTime*8/10, player->center + Point(0, -4)), 3));
    }else{
        player->gun->runAction(RotateTo::create(0.5, player->face->isFlippedY()?860:770));
        player->gun->runAction(EaseOut::create(MoveTo::create(0.5, player->center + Point(0, -4)), 3));
    }
    player->gun->setAnchorPoint(Point(0.5, 0.5));
//    player->face->setRotation(player->face->isFlippedY()?130:40);
    
    if (reloadingTime > 0.3) {
        Sprite* shining = Sprite::createWithSpriteFrameName("shining0.png");
        Animation* animation = AnimationCache::getInstance()->getAnimation("shining");
        Animate* animate = Animate::create(animation);
        shining->setRotation(60);
        player->addChild(shining, 30);
        shining->setPosition(player->center + Point(-3, -3));
        shining->runAction(Sequence::create(DelayTime::create(reloadingTime - 0.7), MoveBy::create(0.7, Point(6, 6)), nullptr));
        shining->runAction(Sequence::create(DelayTime::create(reloadingTime - 0.7), RotateBy::create(0.7, 250), nullptr));
        shining->runAction(Sequence::create(DelayTime::create(reloadingTime - 0.4), animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, shining)), NULL));
    }
    float dur = 0.15f;
    float timeSpan = 0;
    Sprite* hand = Sprite::createWithSpriteFrameName("dary_hand.png");
    player->addChild(hand, 25);
    hand->setPosition(player->center + Point( 3, 1));
    Vector<FiniteTimeAction*> actionArray;
    actionArray.pushBack(MoveBy::create(dur, Point(-6, -2)));
    timeSpan = dur;
    while(true){
        actionArray.pushBack(MoveBy::create(dur, Point(6, 0)));
        timeSpan += dur;
        if (timeSpan + dur >= reloadingTime) {
            break;
        }
        actionArray.pushBack(MoveBy::create(dur, Point(-6, 0)));
        timeSpan += dur;
        if (timeSpan + dur >= reloadingTime) {
            break;
        }
    }
    actionArray.pushBack(CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, hand)));
    hand->runAction(Sequence::create(actionArray));
    
    Sprite* whiteRect = Sprite::createWithSpriteFrameName("whiteRect.png");
    player->addChild(whiteRect);
    whiteRect->setPosition(Point(0, player->getContentSize().height + 4));
    whiteRect->setAnchorPoint(Point(0, 0.5));
    whiteRect->setScaleX(player->getContentSize().width/whiteRect->getContentSize().width);
    whiteRect->setOpacity(100);
    whiteRect->runAction(Sequence::create(DelayTime::create(reloadingTime), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, whiteRect)), NULL));
    
    Sprite* whiteGuage = Sprite::createWithSpriteFrameName("whiteRect.png");
    player->addChild(whiteGuage);
    whiteGuage->setPosition(Point(0, player->getContentSize().height + 4));
    whiteGuage->setAnchorPoint(Point(0, 0.5));
    whiteGuage->setScaleX(0.01);
    whiteGuage->runAction(Sequence::create(ScaleTo::create(reloadingTime, player->getContentSize().width/whiteGuage->getContentSize().width, 1), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, whiteGuage)), NULL));
}
void HelloWorld::fire()
{
    if (GameManager::getInstance()->getHudLayer()->dialogBox) {
        return;
    }
    
    if (!readyToMove || !player->gun->isVisible() || isReloading || GameManager::getInstance()->getCurrentStageLayer()->indexToWarp != STAGE_NONE) {
        return;
    }
    
//    if (loadedBulletCountMax < 15 && isFired) { // continues fire
//        return;
//    }
    if (loadedBulletCount < 1) {
        if (!isReloading) {
            reload();
        }
        return;
    }
    
    shakeScreen(1);
    
    loadedBulletCount--;
    GameManager::getInstance()->getHudLayer()->reduceBulletCount();
    if (totalBulletCountMax > 0) {
        totalBulletCount--;
        GameManager::getInstance()->getHudLayer()->setTotalBulletCount(totalBulletCount, totalBulletCountMax);
    }
    
    isFired = true;
    int angle = player->lastAngle;
    
    float scaleY = 0.5f;
    float gaugeY = 7;
    Sprite* whiteRect = Sprite::createWithSpriteFrameName("whiteRect.png");
    player->addChild(whiteRect);
    whiteRect->setPosition(Point(0, player->getContentSize().height + gaugeY));
    whiteRect->setAnchorPoint(Point(0, 0.5));
    whiteRect->setScale(player->getContentSize().width/whiteRect->getContentSize().width, scaleY);
    whiteRect->setOpacity(100);
    whiteRect->runAction(Sequence::create(DelayTime::create(playerFireCoolTime), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, whiteRect)), NULL));
    
    Sprite* whiteGuage = Sprite::createWithSpriteFrameName("whiteRect.png");
    player->addChild(whiteGuage);
    whiteGuage->setColor(Color3B(0, 149, 233));
    whiteGuage->setPosition(Point(0, player->getContentSize().height + gaugeY));
    whiteGuage->setAnchorPoint(Point(0, 0.5));
    whiteGuage->setScale(0.01f, 1);
    whiteGuage->runAction(Sequence::create(ScaleTo::create(playerFireCoolTime, player->getContentSize().width/whiteGuage->getContentSize().width, scaleY), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, whiteGuage)), NULL));
    
    float gunWidth = player->gun->getContentSize().width;
    float x = cos(angle*3.14/180)*gunWidth;
    float y = -sin(angle*3.14/180)*gunWidth;
    Point gunLengthPos = Point(x, y);
    Vec2 msStartPos = player->getPosition() + player->msStartPos;
    if (player->weapon == WEAPON_LASER_GUN ||
        player->weapon == WEAPON_LASER_COIN) {
        if (laser == nullptr) {
            TMXTiledMap* tileMap = getTileMap(player->getPosition());
            if (player->weapon == WEAPON_LASER_GUN) {
                GameManager::getInstance()->playSoundEffect(SOUND_LASER_BUZZ);
                laser = Laser::create("laserThinBlue.png", "laserThinBlueGathering.png", "laserThinBlueHit", tileMap);
            }else{
                GameManager::getInstance()->playSoundEffect(SOUND_LASER_WOONG);
                laser = Laser::create("bigLaserLine.png", "bigLaserCircle.png", "", tileMap);
                laser->setScaleY(0.4);
                laser->sptBeam->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
                laser->sptHit->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
                laser->sptHit->addChild(getLight());
                laser->sptBeam->addChild(getLight());
                laser->sptHit->setScale(2, 1.6);
            }
            
            laser->enemyArray = enemyArray;
            laser->rectArray = stageTileRectArray;
            this->addChild(laser);
        }
        
//        laser->setLaser(player->getPosition() + player->gun->getPosition() - player->center + gunLengthPos, player->lastAngle);
        laser->setLaser(msStartPos, player->lastAngle);
        if (laser->hitEnemy != nullptr && laser->hitEnemy->energy > 0)
        {
            int damage = player->weaponPower;//GameManager::getInstance()->getWeaponPower(player->weapon);
            log("weapon power: %d", damage);
            laser->hitEnemy->energy -= (int)damage;
            laser->hitEnemy->showEnergy();
            
            showDamage(damage, laser->hitEnemy->getPosition());
            if (laser->hitEnemy->energy <= 0 && !enemiesToRemove.contains(laser->hitEnemy)) {
                enemiesToRemove.pushBack(laser->hitEnemy);
//                laser->enemyArray = enemyArray;
//                laser->enemyArray.eraseObject(laser->hitEnemy);
                
                this->runEffect(EFFECT_EXPLODE_BIG, laser->hitEnemy->getPosition());
                this->createRewards(laser->hitEnemy);
                if (enemyArray.contains(laser->hitEnemy)) {
                    enemyKilledCount++;
                }
                laser->hitEnemy = nullptr;
//                enemyArray.eraseObject(laser->hitEnemy);
            }else
            {
                laser->hitEnemy->blinkForAWhile();
            }
        }
        if (loadedBulletCount <= 0) {
            removeLaser();
        }
        return;
    }else if (player->weapon == WEAPON_LIGHTNING_GUN) {
        Point startPos =msStartPos;// player->getPosition() + player->gun->getPosition() - player->center + gunLengthPos;
        GameManager::getInstance()->playSoundEffect(SOUND_LIGHTNING);
        Sprite* sptSpark = Sprite::create("bigLaserCircle.png");
        sptSpark->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
        this->addChild(sptSpark, 1);
        sptSpark->setPosition(startPos);
        sptSpark->setScale(0.5);
        sptSpark->runAction(Sequence::create(FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptSpark)), NULL));
        sptSpark = Sprite::create("bigLaserCircle.png");
        sptSpark->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
        this->addChild(sptSpark, 3);
        sptSpark->setPosition(startPos);
        sptSpark->setScale(0.5);
        sptSpark->runAction(Sequence::create(FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptSpark)), NULL));
        
        Lightning* lightning = Lightning::create("line.png", "lineTip.png", startPos, player->lastAngle, enemyArray, stageTileRectArray);
        this->addChild(lightning);
        int damage = GameManager::getInstance()->getWeaponPower(WEAPON_LIGHTNING_GUN);
        for (int i = 0; i < lightning->hitEnemyArray.size(); i++) {
            EnemyBase* drop = lightning->hitEnemyArray.at(i);
            drop->energy -= (int)damage;
            drop->showEnergy();
            totalPower += damage;
            if (drop == theBoss) {
                GameManager::getInstance()->getHudLayer()->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
            }else{
                if(droppableArray.find(drop) != droppableArray.end()){
                    drop->desiredPosition = drop->desiredPosition + Point(0, 2);
                    drop->setPosition(drop->getPosition() + Point(0, 2));
                }
            }
            
            showDamage(damage, drop->getPosition());
            
            if (drop->energy <= 0) {
                enemiesToRemove.pushBack(drop);
                this->runEffect(EFFECT_EXPLODE_BIG, drop->getPosition());
                
                this->createRewards(drop);
                if (drop->rewards > 0) {
                    enemyKilledCount++;
                }
            }
            else
            {
                drop->blinkForAWhile();
            }
            
        }
        lightning->hitEnemyArray.clear();
        return;
    }
    //    player->fireGuage -= (9.5f - /*playerMissileRate*/9*0.9f)*0.1f;
    //    CCLOG("Fire: %f, rate: %d", player->fireGuage, playerMissileRate);
    
    Point point = player->getPosition();
    Missile* ms;
    int extraY = -9;
    Point msPos = Point(player->getPosition().x + (player->isTowardLeft?2:14), player->getPosition().y+ extraY + 8 );
    if (player->weapon == WEAPON_PISTOL) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_PISTOL, GameManager::getInstance()->getWeaponPower(WEAPON_PISTOL));
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_UZI) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_UZI, GameManager::getInstance()->getWeaponPower(WEAPON_UZI));
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_MAGNUM) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_MAGNUM, GameManager::getInstance()->getWeaponPower(WEAPON_MAGNUM));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_RIFLE) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_RIFLE, GameManager::getInstance()->getWeaponPower(WEAPON_RIFLE));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*14, player->getPosition().y+ extraY + 8);
        straightMovingArray.pushBack((Droppable*)ms);
        ms->extraSpeed = 150;
    }else if (player->weapon == WEAPON_BALL_SHOOTER) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_BALL_SHOOTER, GameManager::getInstance()->getWeaponPower(WEAPON_BALL_SHOOTER));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*14, player->getPosition().y+ extraY + 8);
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_FLAME_THROWER) {
        GameManager::getInstance()->playSoundEffect(SOUND_NOISE_37);
        ms = GameManager::getInstance()->getBullet(WEAPON_FLAME_THROWER, GameManager::getInstance()->getWeaponPower(WEAPON_FLAME_THROWER));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*14, player->getPosition().y+ extraY + 6);
        customMovingArray.pushBack((Droppable*)ms);
        float duration = 0.5f;
        ms->setScale(1.5, 0.2);
        ms->setAnchorPoint(Point(0.5, 1));
        ms->setTag(UNIT_MISSILE_CUSTOM);
        ms->runAction(MoveBy::create(duration, gunLengthPos*4));
        ms->runAction(EaseIn::create(MoveBy::create(duration, Point(0, 20)), 5));
        ms->runAction(EaseIn::create(ScaleTo::create(duration, 1, 1.5), 2));
        ms->runAction(Sequence::create(DelayTime::create(duration), CallFuncN::create(CC_CALLBACK_1(HelloWorld::missileMoveDone, this)), NULL));
    }else if (player->weapon == WEAPON_NIFE_THROWER) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_NIFE_THROWER, GameManager::getInstance()->getWeaponPower(WEAPON_NIFE_THROWER));
        ms->runAction(RepeatForever::create(RotateBy::create(0.3, 360)));
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_ANTI_BAT) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_PISTOL, GameManager::getInstance()->getWeaponPower(WEAPON_ANTI_BAT));
        ms->setPosition(msStartPos);
        straightMovingArray.pushBack((Droppable*)ms);
        if (angle < -90 || angle > 90) {
            ms->movingAngle = -angle - 45;
        }else{
            ms->movingAngle = -angle + 45;
        }
        spriteBatch->addChild(ms);
        heroMissileArray.pushBack((Missile*)ms);
        ms->setRotation(-ms->movingAngle);
        
        ms = GameManager::getInstance()->getBullet(WEAPON_PISTOL, 20);
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_CROSS_BOW) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_CROSS_BOW, GameManager::getInstance()->getWeaponPower(WEAPON_CROSS_BOW));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_MACHINE_GUN) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_MACHINE_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_MACHINE_GUN));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 9);
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_LASER_GUN) {
        GameManager::getInstance()->playSoundEffect(SOUND_FA);
        ms = GameManager::getInstance()->getBullet(WEAPON_LASER_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_LASER_GUN));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack((Droppable*)ms);
        ms->extraSpeed = 250;
    }else if (player->weapon == WEAPON_FIRE_BOTTLER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_FIRE_BOTTLER, GameManager::getInstance()->getWeaponPower(WEAPON_FIRE_BOTTLER));
        ms->setPosition(Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 2));
        droppableMissileArray.pushBack((Droppable*)ms);
        ms->velocity = Point(cos(-angle*3.14/180)*200, sin(-angle*3.14/180)*200);
        
        ms->weight = 0.5f;
        ms->secondTag = WEAPON_FIRE_BOTTLER;
        ms->setTag(UNIT_MISSILE_DROPPABLE);
        ms->isFregile = true;
        ms->runAction(RepeatForever::create(RotateBy::create(0.5, 360)));
    }else if (player->weapon == WEAPON_RIFLE_GRENADE) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_RIFLE_GRENADE, GameManager::getInstance()->getWeaponPower(WEAPON_RIFLE_GRENADE));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y + extraY+ 7);
        ms->secondTag = WEAPON_RIFLE_GRENADE;
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_SNIPER_RIFLE) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_SNIPER_RIFLE, GameManager::getInstance()->getWeaponPower(WEAPON_SNIPER_RIFLE));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*16, player->getPosition().y + extraY+ 8);
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_ROCKET_LAUNCHER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_ROCKET_LAUNCHER, GameManager::getInstance()->getWeaponPower(WEAPON_ROCKET_LAUNCHER));
        ms->effectType = MISSILE_EFFECT_SMOKE;
        ms->secondTag = WEAPON_ROCKET_LAUNCHER;
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y + extraY+ 9);
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_STRIKER) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_STRIKER, GameManager::getInstance()->getWeaponPower(WEAPON_STRIKER));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*14, player->getPosition().y + extraY+ 8);
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_ENERGY_GUN) {
        GameManager::getInstance()->playSoundEffect(SOUND_LASER);
        ms = GameManager::getInstance()->getBullet(WEAPON_ENERGY_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_ENERGY_GUN));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*13, player->getPosition().y + extraY+ 8);
        straightMovingArray.pushBack((Droppable*)ms);
        ms->secondTag = WEAPON_ENERGY_GUN;
        ms->setScale(2);
        ms->isPiercing = true;
        GameManager::getInstance()->makeItSiluk(ms);
    }else if (player->weapon == WEAPON_BAZOOKA) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_BAZOOKA, GameManager::getInstance()->getWeaponPower(WEAPON_BAZOOKA));
        ms->secondTag = WEAPON_BAZOOKA;
        ms->effectType = MISSILE_EFFECT_RED_SMOKE;
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 6);
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_DRAGON_RIFLE) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_DRAGON_RIFLE, GameManager::getInstance()->getWeaponPower(WEAPON_DRAGON_RIFLE));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 6);
        ms->effectType = MISSILE_EFFECT_RED_BALL;
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_GUIDED_ROCKET_LAUNCHER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_GUIDED_ROCKET_LAUNCHER, GameManager::getInstance()->getWeaponPower(WEAPON_GUIDED_ROCKET_LAUNCHER));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*14, player->getPosition().y+ extraY + 6);
        ms->target = findTarget(500);
        ms->setRotation(player->isTowardLeft?180:0);
        ms->setTag(UNIT_MISSILE_CHASING);
        ms->effectType = MISSILE_EFFECT_SMOKE;
        ms->secondTag = WEAPON_GUIDED_ROCKET_LAUNCHER;
        chasingMissileArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_MATT_A4) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_MATT_A4, GameManager::getInstance()->getWeaponPower(WEAPON_MATT_A4));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_SLUGGER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*13, player->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_SLUGGER_NG) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->setPosition(player->getPosition() + player->gun->getPosition() - player->center + gunLengthPos);
        straightMovingArray.pushBack((Droppable*)ms);
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        
        ms->movingAngle =  -angle + 45;
        spriteBatch->addChild(ms);
        heroMissileArray.pushBack(ms);
        
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->setPosition(player->getPosition() + player->gun->getPosition() - player->center + gunLengthPos);
        straightMovingArray.pushBack((Droppable*)ms);
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        
        ms->movingAngle =  -angle - 45;
        spriteBatch->addChild(ms);
        heroMissileArray.pushBack(ms);
        
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        
        straightMovingArray.pushBack((Droppable*)ms);
    }else if (player->weapon == WEAPON_SHARK_GUN) {
        GameManager::getInstance()->playSoundEffect(SOUND_TOONG);
        ms = GameManager::getInstance()->getBullet(WEAPON_SHARK_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_SHARK_GUN));
        ms->effectType = MISSILE_EFFECT_SHARK_GUN;
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*13, player->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack((Droppable*)ms);
        
    }
    
//    ms->setPosition(player->getPosition() + player->gun->getPosition() - player->center + gunLengthPos);
    ms->setPosition(msStartPos);
    if (player->weapon != WEAPON_FLAME_THROWER) {
        Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(ms->getPosition());
        explode->setAnchorPoint(Point(1, 0.5));
        Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        explode->setRotation(angle + 180);
        if (rand()%2 == 0) {
            explode->setFlippedY(true);
        }
        player->runFireAnimation();
    }
    
    
    if (player->weapon != WEAPON_FIRE_BOTTLER &&
        player->weapon != WEAPON_NIFE_THROWER &&
        player->weapon != WEAPON_BALL_SHOOTER &&
        player->weapon != WEAPON_FLAME_THROWER&&
        player->weapon != WEAPON_CROSS_BOW) {
        Droppable* tanpi = Droppable::createDroppable(UNIT_COIN, 0, 0, "bulletPi.png");
        droppableCoinArray.pushBack(tanpi);
        tanpi->weight = 1;
        tanpi->setScale((ms->getContentSize().height)/tanpi->getContentSize().height);
        auto rotate = RotateBy::create(1, rand()%2 == 0?300:-300);
        rotate->setTag(ACTION_TAG_ROTATION);
        tanpi->runAction(rotate);
        tanpi->runAction(Sequence::create(DelayTime::create(3), CallFuncN::create(CC_CALLBACK_1(HelloWorld::coinWaitDone, this)), NULL));
        spriteBatch->addChild(tanpi);
        
        float middleX = cos(angle*3.14/180)*(gunWidth/2);
        float middleY = -sin(angle*3.14/180)*(gunWidth/2);
        
        tanpi->setPosition(ms->getPosition() - Point(middleX, middleY));
        tanpi->velocity = Point(rand()%1000 - 500, 500);
    }
    
    
    ms->movingAngle = -angle;
    ms->setRotation(angle);
    if (player->weapon == WEAPON_MACHINE_GUN ||
        player->weapon == WEAPON_UZI){
        ms->movingAngle += rand()%10 - 5;
    }
//    if (isVideoRewardAttack) {
//        ms->energy = ms->energy*1.2;
//    }
    ms->energy = player->weaponPower;
    ((Missile*)ms)->criticalPossibility = player->weaponCriticalPossibility;
    
    log("weapon power for ms: %f", ms->energy);
    
    if (GameManager::getInstance()->developer) {
        //        ms->energy = 2000;
    }
    spriteBatch->addChild(ms);
    heroMissileArray.pushBack(ms);
    
    Node* light = getLight();
    light->setPosition(ms->getPosition());
    this->addChild(light);
    light->runAction(Sequence::create(DelayTime::create(0.1), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, light)), NULL));
    light->setColor(Color3B(250, 100, 100));
    
    
    /*Sprite* fireFlash = Sprite::createWithSpriteFrameName(CCString::createWithFormat("fireFlash%d.png", fireIndex)->getCString());
     fireFlash->setPosition(Point(point.x + (player->isTowardLeft?-5:5), point.y));
     fireFlash->setFlippedX(player->isTowardLeft);
     spriteBatch->addChild(fireFlash, 10);
     DelayTime* delay = DelayTime::create(0.07f);
     CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone));
     fireFlash->runAction(Sequence::create(delay, call, NULL));
     
     Sprite* fireFlashLight = Sprite::createWithSpriteFrameName("fireLight.png");
     fireFlashLight->setPosition(Point(point.x + (player->isTowardLeft?-5:5), point.y));
     fireFlashLight->setFlippedX(player->isTowardLeft);
     spriteBatch->addChild(fireFlashLight, 10);
     DelayTime* delayForLight = DelayTime::create(0.07f);
     CallFuncN* callForLight = CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone));
     fireFlashLight->runAction(Sequence::create(delayForLight, callForLight, NULL));
     */
}
void HelloWorld::missileEffectUpdate(float dt){
    missileEffectCollapsedTime += dt;
    if(missileEffectCollapsedTime < 0.053){
        return;
    }
    missileEffectCollapsedTime = 0;
    Vector<Droppable*> doneArray;
    bool checkChange = false;
    do
    {
        checkChange = false;
        for(auto drop: heroMissileArray){
            if(drop->effectType == MISSILE_EFFECT_SMOKE)
                runEffect(EFFECT_SMOKE, drop->getPosition());
            if(drop->effectType == MISSILE_EFFECT_RED_BALL)
                runEffect(EFFECT_EXPLODE_SMALL, drop->getPosition());
            else if(drop->effectType == MISSILE_EFFECT_GREEN_SMOKE)
                runEffect(EFFECT_GREEN_SMOKE, drop->getPosition());
            else if(drop->effectType == MISSILE_EFFECT_RED_SMOKE)
                runEffect(EFFECT_RED_SMOKE, drop->getPosition());
            else if(drop->effectType == MISSILE_EFFECT_SHARK_GUN){
                if (doneArray.find(drop) != doneArray.end()) {
                    continue;
                }
                doneArray.pushBack(drop);
                for (int i = 0; i < 2; i ++) {
                    float angle = drop->movingAngle;
                    Droppable* ms = GameManager::getInstance()->getBullet(WEAPON_SHARK_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_SHARK_GUN)*4);
                    ms->setSpriteFrame("bombSmallEffect0.png");
                    ms->effectType = MISSILE_EFFECT_NONE;
                    ms->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
                    ms->setScale(0.5);
                    Point msPos = drop->getPosition();
                    //                straightMovingArray.pushBack(ms);
                    customMovingArray.pushBack(ms);
                    ms->setTag(UNIT_MISSILE_CUSTOM);
                    ms->setPosition(msPos);
                    if (i == 0) {
                        Node* light = getLight();
                        light->setPosition(ms->getPosition());
                        this->addChild(light);
                        light->runAction(Sequence::create(DelayTime::create(0.1), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, light)), NULL));
                        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
                    }
                    
                    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
                    spriteBatch->addChild(explode);
                    explode->setPosition(ms->getPosition());
                    explode->setAnchorPoint(Point(1, 0.5));
                    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
                    Animate* animate = Animate::create(animation);
                    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
                    explode->setRotation(angle + 180);
                    if (rand()%2 == 0) {
                        explode->setFlippedY(true);
                    }
                    
                    ms->movingAngle = angle + (i%2 == 0? 90:-90);
                    int movingDistance = 30;
                    float dur= 0.5;
                    //                ms->addChild(getLight());
                    ms->runAction(ScaleTo::create(dur, 0.3));
                    ms->runAction(FadeOut::create(dur));
                    ms->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur, Point(cos(ms->movingAngle*3.14/180)*movingDistance, sin(ms->movingAngle*3.14/180)*movingDistance)), 5), CallFuncN::create(CC_CALLBACK_1(HelloWorld::missileMoveDone, this)), nullptr));
                    //                ms->setRotation(angle);
                    if (player->weapon == WEAPON_MACHINE_GUN ||
                        player->weapon == WEAPON_UZI){
                        ms->movingAngle += rand()%10 - 5;
                    }
                    if (isVideoRewardAttack) {
                        ms->energy = ms->energy*1.2;
                    }
                    if (GameManager::getInstance()->developer) {
                        //        ms->energy = 2000;
                    }
                    spriteBatch->addChild(ms);
                    heroMissileArray.pushBack((Missile*)ms);
                }
                checkChange = true;
                break;
            }
        }
    }while(checkChange);
    doneArray.clear();
}
void HelloWorld::missileMoveDone(Ref* obj)
{
    EnemyBase* missile = (EnemyBase*)obj;
    heroMissileArray.eraseObject((Missile*)missile);
    enemyMissileArray.eraseObject((Missile*)missile);
    customMovingArray.eraseObject(missile);
    missile->stopAllActions();
    missile->removeFromParentAndCleanup(true);
}
void HelloWorld::fireBulletMoveDone(Ref* obj)
{
    Droppable* missile = (Droppable*)obj;
    heroMissileArray.eraseObject((Missile*)missile);
    customMovingArray.eraseObject((Missile*)missile);
    missile->stopAllActions();
    missile->removeFromParentAndCleanup(true);
}
void HelloWorld::enemyBaseMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    EnemyBase* sprite = (EnemyBase*)obj;
    sprite->stopAllActions();
    enemyArray.eraseObject(sprite);
    sprite->removeFromParentAndCleanup(true);
}
void HelloWorld::coinWaitDone(Ref* obj){
    Droppable* drop = (Droppable*)obj;
    coinArray.eraseObject(drop);
    droppableCoinArray.eraseObject(drop);
    drop->runAction(Sequence::create(FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::removeFromParentAndCleanup, drop)), NULL));
}
void HelloWorld::spriteMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Sprite* sprite = (Sprite*)obj;
    sprite->stopAllActions();
    sprite->removeFromParentAndCleanup(true);
}
void HelloWorld::nodeMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Label* lbl = (Label*)obj;
    
    lbl->stopAllActions();
    lbl->removeFromParentAndCleanup(true);
}

/*void HelloWorld::fireLoop(float dt)
 {
 if (player->fireGuage < 0) {
 player->fireGuage += dt;
 }
 
 }*/

void HelloWorld::blinkingPlayer(float dt)
{
    blinkingTime -= dt;
    
    if (player->getColor().g == 115) {
        player->setColor(Color3B(255, 255, 255));
        player->face->setColor(Color3B(255, 255, 255));
    }else{
        player->setColor(Color3B(255, 115, 115));
        player->face->setColor(Color3B(255, 115, 115));
    }
    
    if (blinkingTime <= 0) {
        player->setColor(Color3B(255, 255, 255));
        
        this->unschedule(schedule_selector(HelloWorld::blinkingPlayer));
    }
}

void HelloWorld::pauseLayer()
{
    for(auto spt: enemyArray)
    {
        spt->pause();
    }
    
    for(auto spt: heroMissileArray)
    {
        spt->pause();
    }
    
    for(auto spt: enemyMissileArray)
    {
        spt->pause();
    }
    
    for(auto trap: trapArray){
        trap->pause();
    }
    
    for(auto buddy: buddyArray){
        buddy->pause();
    }
    player->pause();
    this->pause();
}

void HelloWorld::resumeLayer()
{
    for(auto spt: enemyArray)
    {
        spt->resume();
    }
    
    for(auto spt: heroMissileArray)
    {
        spt->resume();
    }
    
    for(auto spt: enemyMissileArray)
    {
        spt->resume();
    }
    for(auto trap: trapArray){
        trap->resume();
    }
    
    for(auto buddy: buddyArray){
        buddy->resume();
    }
    player->resume();
    this->resume();
}

void HelloWorld::refreshPlayerInfo(int costum, int weapon)
{
    setPlayerInfo();
}

/*
 void HelloWorld::draw()
 {
 ccDrawColor4F(0.0f, 0.0f, 0.0f, 1.0f);
 
 glLineWidth(3.0f);
 Ref* obj;
 Droppable* drop;
 CCARRAY_FOREACH(spiderArray, obj){
 drop = (Droppable*)obj;
 ccDrawLine(Point(drop->getPosition().x + 1, drop->getPosition().y + 4),
 Point(drop->wanderStart.x + 1, drop->wanderStart.y + 4));
 }
 //    ccDrawColor4F(1.0f, 0.0f, 0.0f, 1.0f);
 
 glLineWidth(12.0f);
 Rect rect;
 CCARRAY_FOREACH(enemyArray, obj)
 {
 drop = (Droppable*)obj;
 if (drop->getTag() != -2 && drop->blinkingTime > 0) {
 rect = drop->getBoundingBox();
 ccDrawColor4F(0.96f, 0.9f, 0.2f, 1.0f);
 
 ccDrawLine(Point(rect.origin.x + rect.size.width/2 - 8, rect.origin.y + rect.size.height + 10), Point(rect.origin.x + rect.size.width/2 + 8, rect.origin.y + rect.size.height + 10));
 ccDrawColor4F(0.74f, 0.02f, 0.01f, 1.0f);
 
 ccDrawLine(Point(rect.origin.x + rect.size.width/2 - 8 + 16*drop->energy/drop->maxEnergy, rect.origin.y + rect.size.height + 10), Point(rect.origin.x + rect.size.width/2 + 8, rect.origin.y + rect.size.height + 10));
 }
 }
 
 if (playerInvisible) {
 rect = player->getBoundingBox();
 
 ccDrawColor4F(0.2f, 0.7f, 0.99f, 0.7f);
 ccDrawLine(Point(rect.origin.x + rect.size.width/2 - 8, rect.origin.y - 3), Point(rect.origin.x + rect.size.width/2 + 8, rect.origin.y - 3));
 
 ccDrawColor4F(0.2f, 0.2f, 0.2f, 0.7f);
 ccDrawLine(Point(rect.origin.x + rect.size.width/2 - 8 + 16*invisibleRemainTime/invisibleMaxTime, rect.origin.y - 3), Point(rect.origin.x + rect.size.width/2 + 8, rect.origin.y - 3));
 }
 
 if (playerImmortal) {
 rect = player->getBoundingBox();
 
 ccDrawColor4F(0.99f, 0.7f, 0.2f, 0.7f);
 ccDrawLine(Point(rect.origin.x + rect.size.width/2 - 8, rect.origin.y - 6), Point(rect.origin.x + rect.size.width/2 + 8, rect.origin.y - 6));
 
 ccDrawColor4F(0.2f, 0.2f, 0.2f, 0.7f);
 ccDrawLine(Point(rect.origin.x + rect.size.width/2 - 8 + 16*immortalRemainTime/immortalMaxTime, rect.origin.y - 6), Point(rect.origin.x + rect.size.width/2 + 8, rect.origin.y - 6));
 }
 
 }*/


/*
 ccDrawColor4F(1.0f, 0.0f, 0.0f, 1.0f);
 
 glLineWidth(1.0f);
 Sprite* sprite;
 Rect projectileRect;
 //    CCLOG("enemy count: %d", enemyArray.size());
 float width;
 for (int i = 0; i<enemyArray.size(); i++) {
 sprite = (Sprite*)enemyArray.at(i);
 
 if (sprite->getPosition().y-player->getPosition().y > 50 || sprite->getPosition().y - player->getPosition().y < -50) {
 continue;
 }
 
 width = sprite->getContentSize().width;
 
 projectileRect = Rect(sprite->getPosition().x - (width*sprite->getScaleX()/2) + 2,
 sprite->getPosition().y - (sprite->getContentSize().height*sprite->getScaleY()/2) + 2,
 width*sprite->getScaleX() - 4,
 sprite->getContentSize().height*sprite->getScaleY() - 4);
 
 projectileRect = RectInset(projectileRect, 3, 3);
 ccDrawRect(projectileRect.origin, Point(projectileRect.origin.x + projectileRect.size.width, projectileRect.origin.y + projectileRect.size.height));
 }
 Rect pRect = player->damageBoundingBox();
 ccDrawRect(pRect.origin, Point(pRect.origin.x + pRect.size.width, pRect.origin.y + pRect.size.height));
 
 ccDrawColor4F(0.0f, 0.0f, 1.0f, 1.0f);
 pRect = player->collisionBoundingBox();
 ccDrawRect(pRect.origin, Point(pRect.origin.x + pRect.size.width, pRect.origin.y + pRect.size.height));
 
 //    ccDrawColor4F(0.0f, 1.0f, 0.0f, 1.0f);
 //    pRect = player->currentBoundingBox();
 //    ccDrawRect(pRect.origin, Point(pRect.origin.x + pRect.size.width, pRect.origin.y + pRect.size.height));
 */

