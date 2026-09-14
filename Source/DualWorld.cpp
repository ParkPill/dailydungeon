#include "DualWorld.h"
#include "SimpleAudioEngine.h"
#include "LegendDaryButton.h"
#include "DualHudLayer.h"
#include "Title.h"
#include "SceneChanger.h"
#include "GameManager.h"
#include "MyMessageBox.h"
#include "NativeInterface.h"
#include "Buddies.h"
#include "AwesomeDialogBox.h"
#include "LanguageManager.h"
#include "ServerManager.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
using namespace cocos2d;
using namespace CocosDenshion;

Scene* DualWorld::scene(int stage)
{
    GameManager::getInstance()->playSoundEffect(SOUND_DUNGEON_NOISE);
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
    GameManager::getInstance()->currentStageIndex = stage;
    
//    GameManager::getInstance()->showInterstitialAds();
    DualHudLayer* dualHudLayer = DualHudLayer::create();
    scene->addChild(dualHudLayer, 10);
    GameManager::getInstance()->setDualHudLayer(dualHudLayer);
    dualHudLayer->addListener();
    
    DualWorld *layer = DualWorld::create();
    scene->addChild(layer);
    
    GameManager::getInstance()->setCurrentDualLayer(layer);
    layer->addListener();
    int tag = rand()%50;
    layer->setTag(tag);
//    dualHudLayer->setTag(tag);
    CCLOG("** New Tag: %d, theme: %d, stage: %d", tag, GameManager::getInstance()->theme, stage);
    
    layer->setDualMap();
    
    return scene;
}

// on "init" you need to initialize your instance
bool DualWorld::init()
{
    size = Director::getInstance()->getWinSize();
    //////////////////////////////
    // 1. super init first
    theBoss = NULL;
    isGameOver = false;
    GameManager::getInstance()->isInMiddleOfGame = true;
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
    this->addChild(spriteBatch);
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
    
    // init done
    return true;
}

void DualWorld::updateGame(float dt){
    setViewPointCenter((player->getPosition() + enemy->getPosition())/2 +extraCameraPos);
    
    if (readySetBattleState < 3) {
        
        return;
    }
    
    if (!isGameOver) {
        for(Weapon* weapon: playerWeapons){
//            if (weapon->value%1000 == WEAPON_LASER_COIN || weapon->value%1000 == WEAPON_LASER_GUN) {
//                continue;
//            }
            fire(weapon, dt);
        }
        for(Weapon* weapon: enemyWeapons){
//            if (weapon->value%1000 == WEAPON_LASER_COIN || weapon->value%1000 == WEAPON_LASER_GUN) {
//                continue;
//            }
            fire(weapon, dt);
        }
    }
//    Vector<Droppable*> laserEnemiesToRemove;
//    for(auto laser: laserArray){
//
//    }
//    laserEnemiesToRemove.clear();
    
    this->chasingMissileUpdate(dt);
    this->enemyUpdate(dt);
    this->missileUpdate(dt);
    this->coinUpdate(dt);
    this->npcUpdate(dt);
    this->missileEffectUpdate(dt);
    this->enemyFireLoop(dt);
    
    for(auto drop: droppableArray){
        if (drop->ignoreGravity) {
            continue;
        }
        drop->updatePosition(dt);
        checkForAndResolveCollisions(drop);
    }
    
    for(auto drop: droppableMissileArray){
        drop->updatePosition(dt);
        if(checkDroppableMissileCollision(drop)) break;
    }
    if (player->energy != lastPlayerHealth || enemy->energy != lastEnemyHealth) {
        lastPlayerHealth = player->energy;
        lastEnemyHealth = enemy->energy;
        DUAL_HUD->setPlayerHealth(player->energy*100/player->maxEnergy);
        DUAL_HUD->setEnemyHealth(enemy->energy*100/enemy->maxEnergy);
        if ((player->energy <= 0 || enemy->energy <= 0) && !isGameOver) {
            isGameOver = true;
            runEffect(EFFECT_METAL_SLUG_EXPLOSION, player->getPosition());
            for(auto weapon: playerWeapons){
                weapon->stopShooting();
            }
            for(auto weapon: enemyWeapons){
                weapon->stopShooting();
            }
            Sprite* sptBurntHero = Sprite::createWithSpriteFrameName("burntHero.png");
            spriteBatch->addChild(sptBurntHero);
            int direction = 1;
            if (player->energy <= 0) {
                sptBurntHero->setPosition(player->getPosition());
                player->setVisible(false);
            }else{
                sptBurntHero->setPosition(enemy->getPosition());
                enemy->setVisible(false);
                direction = -1;
            }
            float durBeforeSlow = 1;
            float durSlow = 1;
            float durAfterSlow = 1;
            float moveX = 300*direction;
            float moveY = 100;
            float totalRate = 10;
            float slowPoint = 4.5f;
            float slowAmount = 1;
            DUAL_HUD->runAction(EaseOut::create(MoveBy::create(0.2f, Point(0, size.height/2)), 2));
            sptBurntHero->runAction(Sequence::create(RotateBy::create(durBeforeSlow, (360 + 50)*direction), RotateBy::create(durSlow, 50*direction), RotateBy::create(durAfterSlow, 360*direction), CallFunc::create(CC_CALLBACK_0(DualWorld::showResult, this)), NULL));
            sptBurntHero->setScaleX(direction);
            sptBurntHero->runAction(Sequence::create(MoveBy::create(durBeforeSlow, Point(moveX*slowPoint/totalRate, moveY*slowPoint/totalRate)), MoveBy::create(durBeforeSlow, Point(moveX*slowAmount/totalRate, moveY*slowAmount/totalRate)), MoveBy::create(durAfterSlow, Point(moveX*(totalRate - slowPoint - slowAmount)/totalRate, moveY*(totalRate - slowPoint - slowAmount)/totalRate)), NULL));
            
//            removeLaser();
            for(auto weapon: playerWeapons){
                weapon->stopShooting();
            }
            
        }
        
        Vector<Shield*> shieldToRemove;
        for (int i = 0; i < playerShields.size(); i++) {
            Shield* shield = playerShields.at(i);
            if (shield->energy <= 0) {
                shieldToRemove.pushBack(shield);
                this->runEffect(EFFECT_EXPLODE_BIG, shield->getPosition());
            }
        }
        for (auto shield: shieldToRemove) {
            playerShields.eraseObject(shield);
            shield->removeFromParent();
        }
        shieldToRemove.clear();
        
        for (int i = 0; i < enemyShields.size(); i++) {
            Shield* shield = enemyShields.at(i);
            if (shield->energy <= 0) {
                shieldToRemove.pushBack(shield);
                this->runEffect(EFFECT_EXPLODE_BIG, shield->getPosition());
            }
        }
        for (auto shield: shieldToRemove) {
            enemyShields.eraseObject(shield);
            shield->removeFromParent();
        }
        shieldToRemove.clear();
    }
}
DualWorld::DualWorld(){
    
}
void DualWorld::getDown(){
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
                this->schedule(schedule_selector(DualWorld::immortalForAWhile), 0.1);
                
                playerIgnoreGravity = true;
                if (player->currentPlatform) {
                    player->currentPlatform->player= NULL;
                    player->currentPlatform = NULL;
                }
                player->setPosition(Point(tele->getPosition().x - 3, player->getPosition().y));
                player->runAction(MoveBy::create(duration, Point(0, -30)));
                
                spriteBatch->setLocalZOrder(3);
                
                this->scheduleOnce(schedule_selector(DualWorld::teleportLater), duration);
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
void DualWorld::dialogFinished(Ref* obj){
    
}
void DualWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* unused_event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE){
//        NativeInterface::NativeInterface::destroyAds();
//        Director::getInstance()->replaceScene(StageSelectScene::scene(GameManager::getInstance()->theme));
    }
}
void DualWorld::showWarpAnimation(){
    Sprite* sptBeam = Sprite::createWithSpriteFrameName("blueBeamVertical0.png");
    sptBeam->setAnchorPoint(Point(0.5, 0));
    sptBeam->setScaleY(10);
    sptBeam->setScaleX(2);
//    spriteBatchForHero->addChild(sptBeam, 11);
    spriteBatch->addChild(sptBeam, 11); // dual
    const char* beamName = "blueBeam";
    if (stageIndex == indexToWarp || (isBossMap && !isSetStageDone)) {
        beamName = "redBeam";
    }
    beamName = "redBeam"; // dual
    Animation* animation = AnimationCache::getInstance()->getAnimation(beamName);
    Animate* animate = Animate::create(animation);
    sptBeam->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptBeam)), NULL));
    Point pos;
    if (player->encounteredNpc == nullptr) {
        pos = player->getPosition();
    }else{
        pos = player->encounteredNpc->getPosition() + Point(0, -player->encounteredNpc->getContentSize().height/2);
    }
    
    pos = Point(Point(21.0f*TILE_SIZE, (42-19)*TILE_SIZE)); // dual
    sptBeam->setPosition(pos + Point(0, 100));
    sptBeam->runAction(MoveTo::create(0.05, pos));
    CCLOG("Player pos: %f, %f", player->getPositionX(), player->getPositionY());
    GameManager::getInstance()->playSoundEffect(SOUND_LASER_WOONG);
    this->runAction(Sequence::create(DelayTime::create(18*0.04), CallFunc::create(CC_CALLBACK_0(DualWorld::stopWoongSound, this)), nullptr));
    shakeScreen(6);
    
    // check if it's time to show ending
    bool lastJewelCollected = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString());
    bool gameCleared = GameManager::getInstance()->isGameClear;
    if (lastJewelCollected && gameCleared) {
        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 0)->getCString(), false);
        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 1)->getCString(), false);
        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 2)->getCString(), false);
        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString(), false);
        
        UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_DOES_IT_TALKED_FORMAT, "story4")->getCString(), false);
        
        HoodGhost* son;
        for(auto knight : npcArray){
            if(knight->getTag() == UNIT_HOOD_GHOST){
                son = (HoodGhost*)knight;
            }
        }
        
        DUAL_HUD->showDialog(son->getTalk());
    }
}
void DualWorld::takeAction(){
    GameManager::getInstance()->life = player->energy;
    if (player->encounteredNpc != nullptr) {
        if (player->encounteredNpc->getTag() == UNIT_GATE) {
            log("npc x, y: %d, %d", (int)player->encounteredNpc->getPosition().x, (int)player->encounteredNpc->getPosition().y);
            int slot = 0;
            while (getWeaponAtSlot(slot) >= 0){
                changeWeapon(slot);
                slot++;
            }
            if (stageIndex == STAGE_LOBBY) {
                if (player->encounteredNpc->getPosition().x == 528 ) {
                    if (UserDefault::getInstance()->getBoolForKey(KEY_IS_TUTORIAL_CLEAR, false)) {
                        indexToWarp = 0;
                    }else{
                        DUAL_HUD->showDialog(theKnight->getTalk());
                        return;
                    }
                }else if(player->encounteredNpc->getPosition().x == 624){
                    indexToWarp = STAGE_NAMING;
                }else{
                    indexToWarp = TUTORIAL_STAGE_START;
                }
            }else if(stageIndex == STAGE_NAMING){
                UserDefault::getInstance()->setStringForKey(KEY_NAME, DUAL_HUD->originalString);
                indexToWarp = STAGE_LOBBY;
            }else{
                indexToWarp = player->encounteredNpc->secondTag;
                
                if (indexToWarp == TUTORIAL_STAGE_END + 1) {
                    indexToWarp = -1;
                    UserDefault::getInstance()->setBoolForKey(KEY_IS_TUTORIAL_CLEAR, true);
                }
            }
            
            warpOut();
        }else if (player->encounteredNpc->getTag() == UNIT_NPC ||
                  player->encounteredNpc->getTag() == UNIT_HOOD_GHOST ||
                  player->encounteredNpc->getTag() == UNIT_THE_KNIGHT) {
            DUAL_HUD->showDialog(player->encounteredNpc->getTalk());
        }else if(player->encounteredNpc->getTag() == UNIT_ONSALE){
            OnSale* onSale = (OnSale*)player->encounteredNpc;
            onSale->buyItem();
            if (!onSale->isExplained) {
                DUAL_HUD->showDialog(player->encounteredNpc->getTalk());
            }
        }
    }
}
void DualWorld::warpOut(){
    player->runAction(MoveTo::create(0.1, player->encounteredNpc->getPosition()));
    player->runAction(Sequence::create(DelayTime::create(0.3), FadeOut::create(0.1), NULL));
    player->face->runAction(Sequence::create(DelayTime::create(0.3), FadeOut::create(0.1), NULL));
    player->gun->runAction(Sequence::create(DelayTime::create(0.3), FadeOut::create(0.1), NULL));
    player->sptTalkBalloon->runAction(Sequence::create(DelayTime::create(0.3), FadeOut::create(0.1), NULL));
    player->wantsToStop = true;
    player->wantsToGoLeft = false;
    player->wantsToGoRight = false;
    GameManager::getInstance()->leftPressed = false;
    GameManager::getInstance()->rightPressed = false;
    this->runAction(Sequence::create(DelayTime::create(0.1), CallFunc::create(CC_CALLBACK_0(DualWorld::showWarpAnimation, this)), DelayTime::create(2), CallFunc::create(CC_CALLBACK_0(DualWorld::warp, this)), NULL));
}
void DualWorld::warp(){
    DUAL_HUD->removeListener();
    Scene* scene = Scene::create();
    bool isBoss = false;
    if (stageIndex >= 0 && !isBossMap) {
        isBoss = true;
    }
    scene->addChild(DualWorld::scene(indexToWarp));
    Director::getInstance()->replaceScene(scene);
}
void DualWorld::enemyUpdate(float dt)
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
                this->schedule(schedule_selector(DualWorld::immortalForAWhile), 0.1f);
//                player->tumbleAnimation();
                player->sptDropEffect->setVisible(false);
                if (!drop->immortal) {
                    attackEnemy(drop, 10);
                }
            }else{
                if (hitDary()) {
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
            if(!(trap->trapType == TRAP_TYPE_LASER && skyBuddy && skyBuddy->petNumber == PET_IRON_DRAGON)){
                demagingUnit = trap->getTag();
                if(hitDary()){
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
                }
            }
        }
    }
    Vector<Missile*> msToRemove;
    
    for (auto missile: enemyMissileArray) {
        projectileRect = Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
                              missile->getPosition().y - (missile->getContentSize().height*missile->getScaleY()/2),
                              missile->getContentSize().width*missile->getScaleX(),
                              missile->getContentSize().height*missile->getScaleY());
        projectileRect = RectInset(projectileRect, 3, 3);
        if (projectileRect.intersectsRect(player->getBoundingBox()) && missile->isHurting) {
            if(missile->secondTag == UNIT_APPLEMON_WORM){
                player->setAppleWorm();
            }else{
                if(!player->immortal){
                    demagingUnit = missile->secondTag;
                    //                        hitDary();
                    player->energy -= missile->energy;
                    
                    if (missile->secondTag == WEAPON_FIRE_BOTTLER) {
                        runEffect(EFFECT_EXPLODE_HUGE, missile->getPosition());
                    }
                    runEffect(EFFECT_EXPLODE_SMALL, player->getPosition() + Point(rand()%16-8, rand()%16-8));
                    enemyMissileArray.eraseObject((Missile*)missile);
                    if (missile->secondTag != WEAPON_ENERGY_GUN) {
                        msToRemove.pushBack(missile);
                    }else{
                        ((Missile*)missile)->victimArray.pushBack(player);
                    }
                }
            }
            enemyMissileArray.eraseObject((Missile*)missile);
            
            break;
        }
        for (int i = 0; i<playerShields.size(); i++) {
            Shield* shield = playerShields.at(i);
            
            if(missile->secondTag == WEAPON_ENERGY_GUN){
                if(((Missile*)missile)->victimArray.contains(shield)){
                    continue;
                }
            }
            
            if (shield->getPosition().y-missile->getPosition().y > 50 ||
                shield->getPosition().y - missile->getPosition().y < -50) {
                continue;
            }
            
            if (shield->getBoundingBox().intersectsRect(projectileRect)) {
                Point pos = shield->getPosition();
                float demage = missile->energy + missile->energy*extraPower/100.0f;
                if (rand()%100 > 100 - DEFAULT_CRITICAL - extraCritical - criticalLevel) {//critical
                    demage*=2;
                    Sprite* critical = Sprite::createWithSpriteFrameName("critical.png");
                    spriteBatch->addChild(critical, 100);
                    critical->getTexture()->setAliasTexParameters();
                    //                            critical->setColor(aColor3B(0,0,0));
                    critical->setScale(0.5f);
                    critical->setPosition(shield->getPosition() + Point(0, 15));
                    critical->runAction(EaseIn::create(MoveBy::create(1, Point(0, 25)), 0.3));
                    critical->runAction(Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
                    
                    shakeScreen(1);
                }
                if (demage >= 10) {
                    demage = demage*0.8f + (rand()%((int)(demage*0.4f)));
                }
                if (missile->parentWeaponIndex%4 == shield->value%4){
                    demage *= 0.5f;
                }
                shield->energy -= demage;
                
                totalPower += demage;
                
                showDamage(demage, shield->getPosition());
                
                this->runEffect(EFFECT_EXPLODE_SMALL, shield->getPosition());
                
                
                if(missile->secondTag == WEAPON_FIRE_BOTTLER ||
                   missile->secondTag == WEAPON_RIFLE_GRENADE ||
                   missile->secondTag == WEAPON_BAZOOKA ||
                   missile->secondTag == WEAPON_ROCKET_LAUNCHER ||
                   missile->secondTag == WEAPON_GUIDED_ROCKET_LAUNCHER){
                    splashDemage(missile->getPosition(), 40, demage/2);
                    CCLOG("in missile updat");
                }
                if(missile->secondTag != WEAPON_ENERGY_GUN){
                    msToRemove.pushBack(missile);
                }else{
                    ((Missile*)missile)->victimArray.pushBack(shield);
                }
                this->runEffect(EFFECT_EXPLODE_SMALL, pos + Point(rand()%16-8, rand()%16-8));
                GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
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
            hitDary();
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
            hitDary();
            demagingUnit = UNIT_MACE_TRAP;
            break;
        }
    }
}
void DualWorld::updateFireStick(float dt){
    
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
                hitDary();
                demagingUnit = UNIT_FIRE_STICK;
            }
            stickR += stick->getContentSize().width;
        }
        if (stick->boundingBox().intersectsRect(player->damageBoundingBox())) {
            hitDary();
            demagingUnit = UNIT_FIRE_STICK;
        }
    }
}
void DualWorld::destructableUpdate()
{
    if (isGameOver) {
        return;
    }
    
    Rect projectileRect, mRect;
    Vector<Droppable*> missileToRemove;
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
                    float demage = missile->energy;
                    
                    if (demage >= 10) {
                        demage = demage*0.8f + (rand()%((int)(demage*0.2f)));
                    }
                    
                    drop->energy -= demage;
                    if (drop->energy <= 0) {
                        destroyDestructable(drop);
                        currentScore += drop->secondTag*10;
                        DUAL_HUD->setScore(currentScore);
                        createRewards(drop);
                        destructableArray.eraseObject(drop);
                        drop->removeFromParentAndCleanup(true);
                        
                        GameManager::getInstance()->totalDestroy++;
                        GameManager::getInstance()->totalScore += 10;
                        DUAL_HUD->setScore(GameManager::getInstance()->totalScore);
                    }
                    
                    if (drop->getTag() == UNIT_KEYBOARD_KEY) {
                        ((Alphabet*)drop)->keyDownTime = 0.1f;
                        DUAL_HUD->typing(((Alphabet*)drop)->lblChar->getString());
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
        heroMissileArray.eraseObject((Missile*)spt);
        enemyMissileArray.eraseObject((Missile*)spt);
        droppableMissileArray.eraseObject((Missile*)spt);
        enemyArray.eraseObject((EnemyBase*)spt);
        if(spt->getTag() == UNIT_MISSILE_CHASING){
            chasingMissileArray.eraseObject((Missile*)spt);
        }else if(spt->getTag() == UNIT_MISSILE_DROPPABLE){
            droppableMissileArray.eraseObject((Missile*)spt);
        }else if(spt->getTag() == UNIT_MISSILE_CUSTOM){
            customMovingArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_STRAIGHT){
            straightMovingArray.eraseObject(spt);
        }
        spt->removeFromParentAndCleanup(true);
    }
//    enemiesToRemove.clear();
}
void DualWorld::destroyDestructable(Droppable* drop){
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
        part->runAction(Sequence::create(DelayTime::create(3), CallFuncN::create(CC_CALLBACK_1(DualWorld::coinWaitDone, this)), NULL));
        droppableCoinArray.pushBack(part);
    }
    runEffect(EFFECT_EXPLODE_BIG, drop->getPosition());
}
void DualWorld::showDamage(int damage, Point pos){
    return;
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
void DualWorld::attackEnemy(EnemyBase* drop, int demage){
    drop->energy -= demage;
    
    showDamage(demage, drop->getPosition());
    
    if (drop->energy <= 0) {
        this->runEffect(EFFECT_EXPLODE_SMALL, drop->getPosition());
        this->createRewards(drop);
        
        killCountForRecord++;
        GameManager::getInstance()->totalKill++;
        GameManager::getInstance()->totalScore += 100;
        DUAL_HUD->setScore(GameManager::getInstance()->totalScore);
        
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
void DualWorld::immortalForAWhile(float dt){
    indestructableTime -= dt;
    if (indestructableTime < 0) {
        this->unschedule(schedule_selector(DualWorld::immortalForAWhile));
    }
}
bool DualWorld::hitDary(){
    if (indestructableTime <= 0 && !player->immortal) {
        indestructableTime =  1.6f;
        this->schedule(schedule_selector(DualWorld::immortalForAWhile), 0.1f);
        
        if (!GameManager::getInstance()->developer) {
            player->energy--;
            DUAL_HUD->setLife(player->energy);
        }
        
//        player->face->setVisible(true);
        player->freezed = true;
        player->stopAndStand();
        this->scheduleOnce(schedule_selector(DualWorld::stepBackForHit), 0.02f);
        this->scheduleOnce(schedule_selector(DualWorld::unfreezePlayer), 0.4f);
        
        blinkingTime = 1.6f;
        this->schedule(schedule_selector(DualWorld::blinkingPlayer), 0.1f);
        
        if (player->energy <= 0) {
            gameOver();
        }
        return true;
    }
    return false;
}
void DualWorld::stepBackForHit(float dt)
{
    player->velocity = Point(player->isTowardLeft?1000:-1000, player->inWater?50:220);
}
void DualWorld::unfreezePlayer(float dt)
{
    player->freezed = false;
}

Rect DualWorld::RectInset(Rect rect, float x, float y)
{
    return Rect(rect.origin.x + x, rect.origin.y + y, rect.size.width - x*2, rect.size.height - y*2);
}

void DualWorld::angelReviveLater(float dt){
    revive();
    buddyArray.eraseObject(skyBuddy);
    runEffect(EFFECT_TWINKLE, skyBuddy->getPosition());
    skyBuddy->removeFromParentAndCleanup(true);
    skyBuddy = NULL;
}
void DualWorld::endGame(){
    //this->unschedule(schedule_selector(DualWorld::gravityUpdate));
    this->unschedule(schedule_selector(DualWorld::gravityUpdateForCoins));
    this->unschedule(schedule_selector(DualWorld::gravityUpdateForStraight));
    this->unschedule(schedule_selector(DualWorld::gravityUpdateForCustomMoving));
    this->unschedule(schedule_selector(DualWorld::gravityUpdateForFlyingOrSwimingEnemies));
    showResult();
}
void DualWorld::onResultOk(){
    this->unschedule(schedule_selector(DualWorld::updateResult));
    GM->isBossFight = false;
    DUAL_HUD->removeListener();
//    Title* title = Title::create();
//    scene->addChild(title);
    Scene* scene = Scene::create();
    scene->addChild(HelloWorld::scene(STAGE_LOBBY, false));
    Director::getInstance()->replaceScene(TransitionCrossFade::create(0.5f, scene));
//    SceneChanger* changer = SceneChanger::create();
//    changer->setLayer(true, "battle", "tip0", HelloWorld::scene(STAGE_LOBBY, false));
//    DUAL_HUD->addChild(changer, TOP_MOST_ZORDER + 1);
}
void DualWorld::showResult(){
    bool playerWon = player->energy > 0;
    if (GM->isBossFight) {
        resultLayer = CSLoader::createNode("BossFightResult.csb");
        resultLayer->setPosition(0, -size.height/2);
        DUAL_HUD->addChild(resultLayer, TOP_MOST_ZORDER);
        
        Button* btn = (Button*)resultLayer->getChildByName("btnClose");
        btn->addClickEventListener(CC_CALLBACK_0(DualWorld::onResultOk, this));
        Text* lbl = (Text*)resultLayer->getChildByName("titleBack")->getChildByName("lblTitle");
        LM->setLocalizedString(lbl, "result");
        
        Node* background = resultLayer->getChildByName("imgBackground");
        lbl = (Text*)background->getChildByName("lblHp");
        LM->setLocalizedString(lbl, "hp");
        
        lbl = (Text*)background->getChildByName("lblScore");
        LM->setLocalizedStringNotKey(lbl, strmake("%s:0", LM->getText("score").c_str()));
        
        Node* nd = background->getChildByName("spt");
        Sprite* spt = Sprite::createWithSpriteFrameName("gunHead.png");
        background->addChild(spt);
        spt->setPosition(nd->getPosition());
        nd->removeFromParent();
        
        int score = 10000 - enemy->energy;
        int rewardCount = 0;
        if (score >= 10000){
            rewardCount = 100;
        }else if (score >= 6000){
            rewardCount = 80;
        }else if (score >= 2000){
            rewardCount = 60;
        }else if (score >= 700){
            rewardCount = 40;
        }else if (score >= 400){
            rewardCount = 30;
        }else if (score >= 200){
            rewardCount = 25;
        }else if (score >= 100){
            rewardCount = 15;
        }
        
        lbl = (Text*)background->getChildByName("lblCrystal");
        lbl->setString(Value(rewardCount).asString());
        
        int crystal = UDGetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, CURRENCY_TYPE_GEM_PURPLE).c_str(), 0);
        crystal += rewardCount;
        SM->saveUserData(strmake("crystal_purple=%d", crystal));
        
        this->schedule(schedule_selector(DualWorld::updateResult), 0.1f);
        return;
    }
    
    resultLayer = CSLoader::createNode("ResultPopupNew.csb");
    resultLayer->setPosition(size.width/2, 0);
    DUAL_HUD->addChild(resultLayer, TOP_MOST_ZORDER);
    cocostudio::timeline::ActionTimeline* action = CSLoader::createTimeline("ResultPopup.csb");
    resultLayer->runAction(action);
    action->gotoFrameAndPlay(0, true);
    
    Button* btn = (Button*)resultLayer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(DualWorld::onResultOk, this));
    Text* lbl = (Text*)resultLayer->getChildByName("lblOk");
    LM->setLocalizedString(lbl, "ok");
    lbl = (Text*)resultLayer->getChildByName("lblResult");
    LM->setLocalizedString(lbl, "result");
    lbl = (Text*)resultLayer->getChildByName("lblDescription");
    
    std::string resultDescription = "player won result description";
    int exp = 10;
    int coin = 150 + rand()%151;
    
    int enemyTrophy = GM->dualEnemyTrophy;
    int playerTrophy = UDGetInt(KEY_TROPHY, 1000);
    int enemyFamily = 1;// test  GameManager::getInstance()->enemyDoc["family"].GetInt();
    int playerFamily = 1; // test  UserDefault::getInstance()->getIntegerForKey(KEY_FAMILY, FAMILY_LIONSTAR);
    bool isRivalMatch = enemyFamily != playerFamily;
//    ImageView* playerFlag = (ImageView*)resultLayer->getChildByName("imgPlayerFlag");
//    GameManager::getInstance()->loadFlagImage(playerFlag, playerFamily); // test  uncomment
//    ImageView* enemyFlag = (ImageView*)resultLayer->getChildByName("imgEnemyFlag");
//    GameManager::getInstance()->loadFlagImage(enemyFlag, enemyFamily); // test  uncomment
//    ImageView* flagToFall = playerFlag;
    
    
    if (playerWon) {
        GM->isBattleWon = true;
//        flagToFall = enemyFlag;
        if (playerTrophy > enemyTrophy) {
            playerTrophy += 5;
        }else{
            playerTrophy += 35;
        }
    }else{
        resultDescription = "enemy won result description";
        exp = 3;
        coin = 150 + rand()%151;
        if (playerTrophy > enemyTrophy) {
            playerTrophy += -35;
        }else{
            playerTrophy += -5;
        }
    }
    
    if (isRivalMatch) {
        exp *= 2;
        coin *= 2;

//        GameManager::getInstance()->shakeIt(flagToFall, 20, 0.07f, 10); // test  uncomment
//        flagToFall->runAction(Sequence::create(DelayTime::create(20*0.07f), FadeOut::create(0.3f), NULL));
//        flagToFall->runAction(Sequence::create(DelayTime::create(20*0.07f), MoveBy::create(0.3f, Point(0, -50)), NULL));
    }else{
//        resultLayer->getChildByName("imgPlayerFlag")->setVisible(false);
//        resultLayer->getChildByName("imgEnemyFlag")->setVisible(false);
    }
    
    LM->setLocalizedString(lbl, resultDescription);
//    Text* lblTrophy = (Text*)resultLayer->getChildByName("lblTrophy");
//    lblTrophy->setString(Value(trophy).asString());
//    Text* lblCoin = (Text*)resultLayer->getChildByName("lblCoin");
//    lblCoin->setString(Value(coin).asString());
//    Text* lblExp = (Text*)resultLayer->getChildByName("lblExp");
//    lblExp->setString(StringUtils::format("EXP: %d", exp));
//    if (isRivalMatch) {
//        lblTrophy->setString(StringUtils::format("%s (x2)", lblTrophy->getString().c_str()));
//        lblExp->setString(StringUtils::format("%s (x2)", lblExp->getString().c_str()));
//        lblCoin->setString(StringUtils::format("%s (x2)", lblCoin->getString().c_str()));
//    }
    
//    int winCountForSuitcase = UserDefault::getInstance()->getIntegerForKey(KEY_WIN_COUNT_FOR_SUITCASE, 0);
//    winCountForSuitcase++;
//    UserDefault::getInstance()->setIntegerForKey(KEY_WIN_COUNT_FOR_SUITCASE, winCountForSuitcase);
    
//    DUAL_HUD->shopLayer->coinTo += coin;
    
//    GameManager::getInstance()->alignToCenter(resultLayer->getChildByName("sptTrohpy"), lblTrophy, 50, 0, 0);
//    GameManager::getInstance()->alignToCenter(resultLayer->getChildByName("sptCoin"), lblCoin, 50, 0, 0);
    
    
    if (GM->isLocalGamePlay) {
        if (playerWon) {
            int unlockedLocalEnemyIndex = UDGetInt(KEY_UNLOCKED_LOCAL_ENEMY, 0);
            if(GM->localPlayIndex >= unlockedLocalEnemyIndex){
                UDSetInt(KEY_UNLOCKED_LOCAL_ENEMY, GM->localPlayIndex+1);
                SM->saveUserData(strmake("avatar_unlock=%d", GM->localPlayIndex+1));
            }
            HUD->setDailyMissionState(DAILY_MISSION_WIN_AVATAR, MISSION_STATE_CLEAR);
        }
        
        Widget* clone;
        int gem = 0;
        int exp = 0;
        for (int i = 0 ; i < 4; i++) {
            int value = rand()%4;
            if (!playerWon) {
                value = 3;
            }
            if (value == 0) { // fruit
                clone = ((Widget*)resultLayer->getChildByName("reward0"))->clone();
                Sprite* spt = Sprite::createWithSpriteFrameName(GM->getFoodSpriteName(FOOD_APPLE + GM->localPlayIndex));
                clone->addChild(spt);
                spt->setScale(0.5f);
                spt->setPosition(clone->getChildByName("img")->getPosition());
                clone->getChildByName("img")->removeFromParent();
                
                bool isMissionItem = false;
                for (int j = 0; j < 4; j++) {
                    if (UDGetInt(strmake(KEY_COLLECT_MISSION_TYPE_FORMAT, j).c_str(), 0) == GM->dualEnemyChar) {
                        isMissionItem = true;
                        break;
                    }
                }
                if (isMissionItem) {
                    bool isAllCollected = true;
                    for (int j = 0; j < 4; j++) {
                        if (UDGetInt(strmake(KEY_COLLECT_MISSION_TYPE_FORMAT, j).c_str(), 0) == GM->dualEnemyChar) {
                            int maxCount = UDGetInt(strmake(KEY_COLLECT_MISSION_MAX_COUNT_FORMAT, j).c_str(), 2);
                            int collectedCount = UDGetInt(strmake(KEY_COLLECT_MISSION_COLLECTED_COUNT_FORMAT, j).c_str(), 0);
                            
                            if (collectedCount < maxCount) {
                                collectedCount++;
                                UDSetInt(strmake(KEY_COLLECT_MISSION_COLLECTED_COUNT_FORMAT, j).c_str(), collectedCount);
                                saveCollectMission();
                                isAllCollected = collectedCount >= maxCount;
                                break;
                            }
                        }
                    }
                    if (isAllCollected) {
                        isMissionItem = false;
                    }
                }
                if(!isMissionItem){
                    spt->runAction(Sequence::create(DelayTime::create(1), JumpBy::create(0.4f, Vec2::ZERO, 20, 1), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
                    Sprite* sptGem = Sprite::create("crystalUI.png");
                    clone->addChild(sptGem);
                    sptGem->setScale(0.5f);
                    sptGem->setPosition(spt->getPosition());
                    sptGem->setOpacity(0);
                    sptGem->runAction(Sequence::create(DelayTime::create(1.4f), FadeIn::create(0), NULL));
                    
                    gem += 1;
                }
            }else if (value == 1) { // gem
                clone = ((Widget*)resultLayer->getChildByName("reward2"))->clone();
                gem += 5;
            }else if (value == 2) { // exp
                clone = ((Widget*)resultLayer->getChildByName("reward3"))->clone();
                exp += 10;
            }else if (value == 3) { // fail
                clone = ((Widget*)resultLayer->getChildByName("reward1"))->clone();
                Sprite* spt = Sprite::createWithSpriteFrameName("pierrotSmileFace0.png");
                GM->runAnimation(spt, "pierrotSmileFace", true);
                spt->runAction(Sequence::create(DelayTime::create(2), FadeOut::create(3), NULL));
                clone->addChild(spt);
                spt->setPosition(clone->getChildByName("img")->getPosition());
                clone->getChildByName("img")->removeFromParent();
            }
            resultLayer->addChild(clone);
            clone->setPosition(resultLayer->getChildByName(strmake("reward%d", i))->getPosition());
        }
        std::string str = "";
        if (gem > 0) {
            GM->addGem(gem);
            str = strmake("gem=%d", GM->getGem());
        }
        if (exp > 0) {
            if (str.length() > 0) {
                str += "&";
            }
            int level = GM->getGameLevel();
            GM->addGameExp(exp);
            str = strmake("%sexp=%d", str.c_str(), GM->getCurrentExp());
            if (GM->getGameLevel() > level) {
                str = strmake("%s&level=%d", str.c_str(), GM->getGameLevel());
            }
        }
        SM->saveUserData(str);
    }else{
        if (playerWon) {
            int clearState = UDGetInt(strmake(KEY_NETWORK_BATTLE_RESULT_FORMAT, GM->networkPlayIndex).c_str(), 0);
            if(clearState == 0){
                UDSetInt(strmake(KEY_NETWORK_BATTLE_RESULT_FORMAT, GM->networkPlayIndex).c_str(), 1);
            }
            
            std::string strData = "network_matches_defeat=";
            for (int i = 0; i < 10; i++) {
                strData = strmake("%s%d", strData.c_str(), UDGetInt(strmake(KEY_NETWORK_BATTLE_RESULT_FORMAT, i).c_str(), 0));
            }
            strData = strmake("%s&trophy=%d", strData.c_str(), playerTrophy);
            SM->saveUserData(strData);
            HUD->setDailyMissionState(DAILY_MISSION_WIN_NETWORK, MISSION_STATE_CLEAR);
            UDSetInt(KEY_TROPHY, playerTrophy);
        }else{
            SM->saveUserData(strmake("trophy=%d", playerTrophy));
        }
    }
    for (int i = 0; i < 4; i++) {
        resultLayer->getChildByName(strmake("reward%d", i))->setVisible(false);
    }
    GameManager::getInstance()->gatherUp(resultLayer);
//    UserDefault::getInstance()->setIntegerForKey(KEY_CURRENT_TROPHY, playerTrophy + trophy);
//    int playerExp = UserDefault::getInstance()->getIntegerForKey(KEY_EXP, 0);
//    UserDefault::getInstance()->setIntegerForKey(KEY_EXP, playerExp + exp);
    GM->addGameExp(exp);
    GameManager::getInstance()->addCoin(coin);
    SM->saveUserData(strmake("level=%d&exp=%d", GM->getGameLevel(), GM->getCurrentExp()));
    /*resultLayer = CSLoader::createNode("Result.csb");
    resultLayer->setPositionX(size.width/2 - resultLayer->getContentSize().width/2);
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
    this->resumeLayer();
    this->schedule(schedule_selector(DualWorld::updateResult), 0.1, CC_REPEAT_FOREVER, 1);*/
}
void DualWorld::saveCollectMission(){
    std::string str = "";
    bool isAllCollected = true;
    for(int i = 0; i < 4; i++){
        int num = UDGetInt(strmake(KEY_COLLECT_MISSION_COLLECTED_COUNT_FORMAT, i).c_str());
        if(num < 2){
            isAllCollected = false;
        }else if(num > 2){
            num = 2;
        }
        str += Value(num).asString();
    }
    
    if (isAllCollected) {
        if (GM->dailyMissionComplete[11] == MISSION_STATE_NOT_CLEAR) {
            GM->dailyMissionComplete[11] = MISSION_STATE_CLEAR;
            std::string strState = "";
            for (int i = 0; i < 12; i++) {
                strState += Value(GM->dailyMissionComplete[i]).asString();
            }
            SM->saveUserData(strmake("mission=%s&dailymission=%s", str.c_str(), strState.c_str()));
        }
    }else{
        SM->saveUserData(strmake("mission=%s", str.c_str()));
    }
}
void DualWorld::updateResult(float dt){
    if (GM->isBossFight) {
        if(enemy->energy < bossEnergyForResult){
            if(bossEnergyForResult - enemy->energy >= 100000){
                bossEnergyForResult -= 100000;
            }else if(bossEnergyForResult - enemy->energy >= 10000){
                bossEnergyForResult -= 10000;
            }else if(bossEnergyForResult - enemy->energy >= 1000){
                bossEnergyForResult -= 1000;
            }else if(bossEnergyForResult - enemy->energy >= 100){
                bossEnergyForResult -= 100;
            }else if(bossEnergyForResult - enemy->energy >= 10){
                bossEnergyForResult -= 10;
            }else if(bossEnergyForResult - enemy->energy >= 1){
                bossEnergyForResult -= 1;
            }
            if( bossEnergyForResult < 0){
                bossEnergyForResult = 0;
            }
            Text* lbl = (Text*)resultLayer->getChildByName("imgBackground")->getChildByName("lblEnergy");
            lbl->setString(strmake("%d/10000", bossEnergyForResult));
            
            lbl = (Text*)resultLayer->getChildByName("imgBackground")->getChildByName("lblScore");
            lbl->setString(strmake("%s: %d", LM->getText("score").c_str(), 10000 - bossEnergyForResult));
        }
        
        return;
    }
    if (resultDone) {
        this->unschedule(schedule_selector(DualWorld::updateResult));
        return;
    }
    
    Text* lbllblTotalScoreValue = (Text*)resultLayer->getChildByName("lblTotalScoreValue");
    Text* lblKill = (Text*)resultLayer->getChildByName("lblKill");
    Text* lblKillValue = (Text*)resultLayer->getChildByName("lblKillValue");
    lblKill->setVisible(true);
    lblKillValue->setVisible(true);
    if (kill < GameManager::getInstance()->totalKill) {
        kill++;
        lblKillValue->setString(Value(kill).asString());
        GameManager::getInstance()->playSoundEffect(SOUND_MISOL);
        totalScore += 100;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
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
        totalScore += 10;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblCoin = (Text*)resultLayer->getChildByName("lblCoin");
    Text* lblCoinValue = (Text*)resultLayer->getChildByName("lblCoinValue");
    lblCoin->setVisible(true);
    lblCoinValue->setVisible(true);
    if (coin < GameManager::getInstance()->totalCoin) {
        coin++;
        lblCoinValue->setString(Value(coin).asString());
        GameManager::getInstance()->playSoundEffect(SOUND_MISOL);
        totalScore += 10;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
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
        totalScore += 500;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
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
        totalScore += 10000;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
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
        time+=10;
        if (time > GameManager::getInstance()->totalTime) {
            time = GameManager::getInstance()->totalTime;
        }
        lblTimeValue->setString(String::createWithFormat("%2d:%2d", (60*60 - time)/60, (60*60 - time)%60)->getCString());
        GameManager::getInstance()->playSoundEffect(SOUND_MISOL);
        totalScore += 100;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
        return;
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
void DualWorld::gameOver()
{
    pauseLayer();
    endGame();
    char stageBuf[20];
    sprintf(stageBuf, "%d-%d", GameManager::getInstance()->theme, GameManager::getInstance()->currentStageIndex);
    
    this->unschedule(schedule_selector(DualWorld::showSuccessLayer));
    
    
    deadPoint = player->getPosition();
    isGameOver = true;
//    GameManager::getInstance()->showInterstitialAds();
    
    if (skyBuddy && skyBuddy->petNumber == PET_ANGEL_DRAGON) {
        MoveBy* moveUp = MoveBy::create(0.25f, Point(0, 40));
        player->runAction(Sequence::create(EaseIn::create(moveUp, 0.3f), NULL));
        
        Sprite* spt = Sprite::create("backLight.png");
        spt->setScale(0.1);
        spt->runAction(ScaleTo::create(0.4, 0.5));
        spt->runAction(Sequence::create(RotateBy::create(1.5, 300), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
        spt->setPosition(skyBuddy->getPosition());
        this->addChild(spt, spriteBatch->getLocalZOrder()-1);

        spt = Sprite::create("backLight.png");
        spt->setScale(0.1);
        spt->runAction(ScaleTo::create(0.4, 0.5));
        spt->runAction(Sequence::create(RotateBy::create(1.5, -300), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
        spt->setPosition(skyBuddy->getPosition());
        this->addChild(spt, spriteBatch->getLocalZOrder()-1);
        
        spt =  Sprite::createWithSpriteFrameName("angelDragon.png");
        spriteBatch->addChild(spt);
        spt->runAction(FadeOut::create(0.5));
        spt->runAction(ScaleTo::create(0.5, 10));
        spt->setPosition(skyBuddy->getPosition());
        
        this->resume();
        this->scheduleOnce(schedule_selector(DualWorld::angelReviveLater), 1.5);
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
    
    return;
    CallFunc* call = CallFunc::create(CC_CALLBACK_0(DualWorld::showGameOverLayer, this));
    DelayTime* time = DelayTime::create(1.6f);
    this->runAction(Sequence::create(time, call, NULL));
}

void DualWorld::showGameOverLayer()
{
    DUAL_HUD->showGameContinueLayer();
}

void DualWorld::missileUpdate(float dt)
{
    if (isGameOver) {
        return;
    }

    Rect mRect;
//    enemiesToRemove.clear();
    Vector<Droppable*> missileToRemove;
    EnemyBase* drop;
    for (auto missile: heroMissileArray) {
        mRect = Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
                            missile->getPosition().y - (missile->getContentSize().height*missile->getScaleY()/2),
                            missile->getContentSize().width*missile->getScaleX(),
                            missile->getContentSize().height*missile->getScaleY());
        for (int i = 0; i<enemyArray.size(); i++) {
            drop = enemyArray.at(i);
            
            if(missile->secondTag == WEAPON_ENERGY_GUN){
                if(((Missile*)missile)->victimArray.contains(drop)){
                    continue;
                }
            }
            
            if (drop->untouchable ||
                drop->getPosition().y-missile->getPosition().y > 50 ||
                drop->getPosition().y - missile->getPosition().y < -50) {
                continue;
            }
            
            if (drop->getBoundingBox().intersectsRect(mRect)) {
                Point pos = drop->getPosition();
                if(!drop->untouchable){
                    if (!drop->immortal) {
                        float demage = missile->energy + missile->energy*extraPower/100.0f;
                        if (rand()%100 > 100 - DEFAULT_CRITICAL - extraCritical - criticalLevel) {//critical
                            demage*=2;
                            Sprite* critical = Sprite::createWithSpriteFrameName("critical.png");
                             spriteBatch->addChild(critical, 100);
                            critical->getTexture()->setAliasTexParameters();
//                            critical->setColor(aColor3B(0,0,0));
                             critical->setScale(0.5f);
                             critical->setPosition(drop->getPosition() + Point(0, 15));
                             critical->runAction(EaseIn::create(MoveBy::create(1, Point(0, 25)), 0.3));
                             critical->runAction(Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
                            
                            shakeScreen(1);
                        }
                        if(player->getTag() == CHARACTER_PAUL){
                            demage += 10;
                            
                        }
                        if (demage >= 10) {
                            demage = demage*0.8f + (rand()%((int)(demage*0.4f)));
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
                            coin->runAction(Sequence::create(DelayTime::create(10), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(DualWorld::coinWaitDone, this)), NULL));
                        }
                        drop->energy -= demage;
                        drop->onHit();
                        if(drop->getTag() != UNIT_DRUM_BOMB){
                            drop->isTargetFound = true;
                            if(player->getTag() == CHARACTER_NINJA){
                                drop->removeChildByTag(CHARACTER_NINJA);
                            }
                        }
                        if (drop->getTag() != UNIT_MISSILE_STRAIGHT &&
                            drop->getTag() != UNIT_MISSILE_CHASING) {
                            drop->showEnergy();
                        }
                        
                        totalPower += demage;
                        if (drop == theBoss) {
                            DUAL_HUD->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
                        }else{
                            if(droppableArray.find(drop) != droppableArray.end()){
                                drop->desiredPosition = drop->desiredPosition + Point(0, 2);
                                drop->setPosition(drop->getPosition() + Point(0, 2));
                            }
                        }
                        
                        showDamage(demage, drop->getPosition());
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
                            lblPlusDemage->runAction(Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(DualWorld::enemyBaseMoveDone, this)), NULL));
                        }
                        if (drop->moveTypeAfterFindHero != drop->currentMoveType) {
                            //                    drop->doActionAfterFindTarget();
                        }
                        
                        if (drop->energy <= 0) {
//                            enemiesToRemove.pushBack(drop);
                            this->runEffect(EFFECT_EXPLODE_BIG, drop->getPosition());
                            drop->onDead();
                            this->createRewards(drop);
                            if (drop->rewards > 0) {
                                enemyKilledCount++;
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
                            splashDemage(missile->getPosition(), 40, demage/2);
                            CCLOG("in missile updat");
                        }
                    }
                    if(missile->secondTag != WEAPON_ENERGY_GUN){
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
        if (mRect.intersectsRect(enemy->getBoundingBox()) && missile->isHurting) {
            if(!enemy->immortal){
                demagingUnit = missile->secondTag;
                
                enemy->energy -= missile->energy;
                if (enemy->energy < 0) {
                    log("enemy dead");
                }
                
                if (missile->secondTag == WEAPON_FIRE_BOTTLER) {
                    runEffect(EFFECT_EXPLODE_HUGE, missile->getPosition());
                }
                runEffect(EFFECT_EXPLODE_SMALL, enemy->getPosition() + Point(rand()%16-8, rand()%16-8));
                heroMissileArray.eraseObject((Missile*)missile);
                if (missile->secondTag != WEAPON_ENERGY_GUN) {
                    missileToRemove.pushBack(missile);
                }else{
                    ((Missile*)missile)->victimArray.pushBack(player);
                }
            }
            heroMissileArray.eraseObject((Missile*)missile);
            
            break;
        }
        
        for (int i = 0; i<enemyShields.size(); i++) {
            Shield* shield = enemyShields.at(i);
            
            if(missile->secondTag == WEAPON_ENERGY_GUN){
                if(((Missile*)missile)->victimArray.contains(shield)){
                    continue;
                }
            }
            
            if (shield->getPosition().y-missile->getPosition().y > 50 ||
                shield->getPosition().y - missile->getPosition().y < -50) {
                continue;
            }
            
            if (shield->getBoundingBox().intersectsRect(mRect)) {
                Point pos = shield->getPosition();
                float demage = missile->energy + missile->energy*extraPower/100.0f;
                if (rand()%100 > 100 - DEFAULT_CRITICAL - extraCritical - criticalLevel) {//critical
                    demage*=2;
                    Sprite* critical = Sprite::createWithSpriteFrameName("critical.png");
                    spriteBatch->addChild(critical, 100);
                    critical->getTexture()->setAliasTexParameters();
                    //                            critical->setColor(aColor3B(0,0,0));
                    critical->setScale(0.5f);
                    critical->setPosition(shield->getPosition() + Point(0, 15));
                    critical->runAction(EaseIn::create(MoveBy::create(1, Point(0, 25)), 0.3));
                    critical->runAction(Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
                    
                    shakeScreen(1);
                }
                if(player->getTag() == CHARACTER_PAUL){
                    demage += 10;
                    
                }
                if (demage >= 10) {
                    demage = demage*0.8f + (rand()%((int)(demage*0.4f)));
                }
                
                if (missile->parentWeaponIndex%4 == shield->value%4) {
                    demage *= 0.5f;
                }
                shield->energy -= demage;
                
                totalPower += demage;
                
                showDamage(demage, shield->getPosition());
                
                this->runEffect(EFFECT_EXPLODE_SMALL, shield->getPosition());
                
                if(missile->secondTag == WEAPON_FIRE_BOTTLER ||
                   missile->secondTag == WEAPON_RIFLE_GRENADE ||
                   missile->secondTag == WEAPON_BAZOOKA ||
                   missile->secondTag == WEAPON_ROCKET_LAUNCHER ||
                   missile->secondTag == WEAPON_GUIDED_ROCKET_LAUNCHER){
                    splashDemage(missile->getPosition(), 40, demage/2);
                    CCLOG("in missile updat");
                }
                if(missile->secondTag != WEAPON_ENERGY_GUN){
                    missileToRemove.pushBack(missile);
                }else{
                    ((Missile*)missile)->victimArray.pushBack(shield);
                }
                this->runEffect(EFFECT_EXPLODE_SMALL, pos + Point(rand()%16-8, rand()%16-8));
                GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
                break;
            }
        }
    }

    for (auto spt: enemiesToRemove){
        if (spt == theBoss) {
            keyCollected = true;
//            createBossRewards();
            
            this->schedule(schedule_selector(DualWorld::bossExplode), 0.3, 20, 1);
            enemyArray.eraseObject(spt);
            theBoss->unscheduleAllCallbacks();
            theBoss->stopAllActions();
            droppableArray.eraseObject(theBoss);
            theBoss->hideEnergy();
            theBoss->setColor(Color3B::RED);
            theBoss->runAction(Sequence::create(DelayTime::create(3), FadeOut::create(2), NULL));
            this->scheduleOnce(schedule_selector(DualWorld::makeGate), 6);
            GameManager::getInstance()->totalBoss++;
            GameManager::getInstance()->totalScore+=10000;
            DUAL_HUD->setScore(GameManager::getInstance()->totalScore);
            continue;
        }
        removeEnemy(spt);
    }
    
    for (auto spt: missileToRemove){
        spt->stopAllActions();
        heroMissileArray.eraseObject((Missile*)spt);
        if(spt->getTag() == UNIT_MISSILE_CHASING){
            chasingMissileArray.eraseObject((Missile*)spt);
        }else if(spt->getTag() == UNIT_MISSILE_DROPPABLE){
            droppableMissileArray.eraseObject((Missile*)spt);
        }else if(spt->getTag() == UNIT_MISSILE_CUSTOM){
            customMovingArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_STRAIGHT){
            straightMovingArray.eraseObject(spt);
        }
        spt->removeFromParentAndCleanup(true);
    }
    enemiesToRemove.clear();
}
void DualWorld::makeGate(float dt){
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
        
        if (GameManager::getInstance()->collectedJewelIndex >= 0) {
            UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, GameManager::getInstance()->collectedJewelIndex)->getCString(), true);
        }
        showResult();
    }else{
        gate->secondTag = stageIndex + 1;
    }
}
void DualWorld::bossExplode(float dt){
    if (rand()%2 == 0) {
        runEffect(EFFECT_EXPLODE_HUGE, theBoss->getPosition() + Point(rand()%20 - 10, rand()%20 - 30));
    }else{
        runEffect(EFFECT_BOMB_WALL, theBoss->getPosition() + Point(rand()%20 - 10, rand()%30 - 30));
    }
}
void DualWorld::shakeScreenOnce(){
    shakeScreen(2);
}
void DualWorld::shakeScreen(int count){
    Vector<FiniteTimeAction*> array;
    for (int i = 0; i < count; i++) {
        array.pushBack(CallFunc::create(CC_CALLBACK_0(DualWorld::shakeScreenFirst, this)));
        array.pushBack(DelayTime::create(0.03));
        array.pushBack(CallFunc::create(CC_CALLBACK_0(DualWorld::shakeScreenSecond, this)));
        array.pushBack(DelayTime::create(0.05));
        array.pushBack(CallFunc::create(CC_CALLBACK_0(DualWorld::shakeScreenEnd, this)));
        array.pushBack(DelayTime::create(0.03));
    }
    GameManager::getInstance()->shakeIt(DUAL_HUD);
    this->runAction(Sequence::create(array));
}
void DualWorld::shakeScreenFirst(){
    extraCameraPos = Point((rand()%10)*0.1, (rand()%10)*0.1);
}
void DualWorld::shakeScreenSecond(){
    extraCameraPos = Point(-(rand()%10)*0.1, -(rand()%10)*0.1);
}
void DualWorld::shakeScreenEnd(){
    extraCameraPos = Point::ZERO;
}
void DualWorld::removeEnemy(EnemyBase* spt){
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
    
    spt->stopAllActions();
    chasingMissileArray.eraseObject((Missile*)spt);
    straightMovingArray.eraseObject(spt);
    enemyArray.eraseObject(spt);
    droppableArray.eraseObject(spt);
    flyingSwimingEnemyArray.eraseObject(spt);
    chasingMissileArray.eraseObject((Missile*)spt);
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
    if (isAllClear && false) {
        TMXTiledMap* tileMap = getTileMap(player->getPosition());
        GameManager::getInstance()->playSoundEffect(SOUND_STONE_ROLL);
        //dnMiniMap->getParent()->runAction(MoveBy::create(0.5, miniMapPos));
        isMiniMapFadeIn = true;
        ScrollView* scrollView = (ScrollView*)dnMiniMap->getParent()->getParent();
        scrollView->setBackGroundColorOpacity(100);
        this->schedule(schedule_selector(DualWorld::fadeMiniMap));
        
        // unlock  doors
        for (int i = 1; i < tileMap->getMapSize().height; i++) {    // left
            if(tileMap->getLayer("stage")->getTileGIDAt(Point(1, i)) == 162){
                tileMap->getLayer("stage")->setTileGID(0, Point(1, i));
                addDirtToTile(tileMap, Point(1, i));
            }
        }
        for (int i = 1; i < tileMap->getMapSize().width; i++) {    // bottom
            if(tileMap->getLayer("stage")->getTileGIDAt(Point(i, 1)) == 162){
                tileMap->getLayer("stage")->setTileGID(0, Point(i, 1));
                addDirtToTile(tileMap, Point(i, 1));
            }
        }
        for (int i = 1; i < tileMap->getMapSize().height; i++) {    // right
            if(tileMap->getLayer("stage")->getTileGIDAt(Point(tileMap->getMapSize().width - 2, i)) == 162){
                tileMap->getLayer("stage")->setTileGID(0, Point(tileMap->getMapSize().width - 2, i));
                addDirtToTile(tileMap, Point(tileMap->getMapSize().width - 2, i));
            }
        }
        for (int i = 1; i < tileMap->getMapSize().width; i++) {    // top
            if(tileMap->getLayer("stage")->getTileGIDAt(Point(i, tileMap->getMapSize().height - 2)) == 162){
                tileMap->getLayer("stage")->setTileGID(0, Point(i, tileMap->getMapSize().height - 2));
                addDirtToTile(tileMap, Point(i, tileMap->getMapSize().height - 2));
            }
        }
        updateMiniMap();
    }
}

void DualWorld::fixStageLayerFourTiles(TMXTiledMap* map){
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

bool DualWorld::compareFourTiles(int t,int l, int r, int b, int x, int y, TMXLayer* stageLayer){
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

void DualWorld::fixStageLayerTiles(TMXTiledMap* map){
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
bool DualWorld::compareNineTiles(int lt, int t, int rt, int l, int r, int lb, int b, int rb, int x, int y, TMXLayer* stageLayer){
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
void DualWorld::createRewards(EnemyBase* enemy){
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
        coin->runAction(Sequence::create(DelayTime::create(8), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(DualWorld::coinWaitDone, this)), NULL));
    }
}
Sprite* DualWorld::getLightSpin(float persistTime){
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
int DualWorld::getSlotForWeapon(int weapon){
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

Sprite* DualWorld::createNumberSprite(int number)
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

void DualWorld::onjewelryBoxOpen(Ref* sender){
    GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("jewel collected").c_str(), DUAL_HUD);
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
void DualWorld::openWeapon(Ref* sender){
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
    }else{
        recharge(getSlotForWeapon(weapon));
    }
}
void DualWorld::recharge(int slot){
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
void DualWorld::addWeapon(int weapon){
    int index = 0;
    while(getWeaponAtSlot(index) >= 0){
        index++;
    }
    setWeaponAtSlot(index, weapon);
    setWeaponAtSlot(index+1, -1);
    setLoadedBulletCountAtSlot(index, getMaxLoadedBulletCount(weapon));
    setTotalBulletCountAtSlot(index, getMaxTotalBulletCount(weapon));
    changeWeapon(index);
}
int DualWorld::getTotalBulletCountAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), 0);
}
void DualWorld::setTotalBulletCountAtSlot(int slot, int count){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), count);
}
int DualWorld::getLoadedBulletCountAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), 0);
}
void DualWorld::setLoadedBulletCountAtSlot(int slot, int count){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), count);
}

void DualWorld::changeWeapon(int slot){
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
    
    setWeaponInfo(weapon);
    totalBulletCountMax = getMaxTotalBulletCount(weapon);
    totalBulletCount = getTotalBulletCountAtSlot(slot);
    loadedBulletCount = getLoadedBulletCountAtSlot(slot);
    loadedBulletCountMax = getMaxLoadedBulletCount(weapon);
    DUAL_HUD->setWeapon(weapon);
    DUAL_HUD->setBulletCount(loadedBulletCount,loadedBulletCountMax);
    DUAL_HUD->setTotalBulletCount(totalBulletCount, totalBulletCountMax);
}
//void DualWorld::removeLaser(){
//    if (laser != nullptr) {
//        laser->removeFromParent();
//        laser = nullptr;
//        GameManager::getInstance()->stopSoundEffect(SOUND_LASER_BUZZ);
//    }
//    for(auto laser: laserArray){
//        laser->removeFromParent();
//    }
//    laserArray.clear();
//}
int DualWorld::getWeaponAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat("WeaponInSlot%d", slot)->getCString(), -1);
}
void DualWorld::setWeaponAtSlot(int slot, int weapon){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat("WeaponInSlot%d", slot)->getCString(), weapon);
}
Sprite* DualWorld::getShining(float delayTime){
    Sprite* shining = Sprite::createWithSpriteFrameName("shining0.png");
    Animation* animation = AnimationCache::getInstance()->getAnimation("shining");
    Animate* animate = Animate::create(animation);
    shining->runAction(Sequence::create(DelayTime::create(delayTime), MoveBy::create(0.7, Point(6, 6)), nullptr));
    shining->runAction(Sequence::create(DelayTime::create(delayTime), RotateBy::create(0.7, 250), nullptr));
    shining->runAction(Sequence::create(DelayTime::create(delayTime), animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, shining)), NULL));
    
    return shining;
}
void DualWorld::npcUpdate(float dt){
    if (!player->onGround) {
        if (player->sptTalkBalloon != nullptr) {
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
                break;
            }
            Rect rect = intersection(npc->getBoundingBox(), player->getBoundingBox());
            if (npc->getTag() == UNIT_NPC || npc->getTag() == UNIT_THE_KNIGHT || npc->getTag() == UNIT_HOOD_GHOST) {
                player->setFeelingVisible(true, FEELING_TALK);
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
            player->encounteredNpc = npc;
            break;
        }
    }
    if (hideTalk && player->encounteredNpc != nullptr) {
        player->setFeelingVisible(false, FEELING_EXCLAIM);
        if (player->encounteredNpc != nullptr) {
            if (player->encounteredNpc->getTag() == UNIT_GATE && stageIndex == -1 && player->encounteredNpc->getPosition().x == 528 ) {
                for(auto knight : npcArray){
                    if (knight->getTag() == UNIT_THE_KNIGHT && UserDefault::getInstance()->getBoolForKey(KEY_IS_TUTORIAL_CLEAR, false)) {
                        if (((TheKnight*)knight)->state == KNIGHT_SWING_LEFT_BOTTOM) {
                            ((TheKnight*)knight)->doKnightAction(KNIGHT_SWING_BACK_FROM_LEFT_BOTTOM);
                        }
                    }
                }
            }
        }
        player->encounteredNpc = nullptr;
    }
}
void DualWorld::coinUpdate(float dt){
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
                    if(droppableCoinArray.getIndex(drop) >= 0)
                        droppableCoinArray.eraseObject(drop);
                    showCoinCount(drop->getPosition(), coin);
                    GameManager::getInstance()->setCoin(collectedCoinCount);
                    DUAL_HUD->updateCoin(collectedCoinCount);
                    GameManager::getInstance()->totalCoin += coin;
                    GameManager::getInstance()->totalScore+=coin*10;
                    DUAL_HUD->setScore(GameManager::getInstance()->totalScore);
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
                    DUAL_HUD->updateCoin(collectedCoinCount);
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
                    DUAL_HUD->updateCoin(collectedCoinCount);
                    GameManager::getInstance()->totalCoin += coin;
                    GameManager::getInstance()->totalScore+=coin*10;
                    DUAL_HUD->setScore(GameManager::getInstance()->totalScore);
                    break;
                case UNIT_FOOD:
                    GameManager::getInstance()->playSoundEffect(SOUND_COIN);
                    if(droppableCoinArray.getIndex(drop) >= 0)
                        droppableCoinArray.eraseObject(drop);
                    GameManager::getInstance()->addFoodCount((drop)->secondTag, 1);
                    break;
                case UNIT_MEDI_KIT:
                    player->energy = player->maxEnergy;
                    DUAL_HUD->setLife(player->maxEnergy);
                    // heal effect
                    GameManager::getInstance()->playSoundEffect(SOUND_MEDICINE);
                    break;
                case UNIT_KEY:
                    GameManager::getInstance()->collectedKeyCount++;
                    DUAL_HUD->keyCollected(GameManager::getInstance()->collectedKeyCount);
                    GameManager::getInstance()->playSoundEffect(SOUND_MEDICINE);
                    break;
                case UNIT_SUITCASE:
                case UNIT_JEWELRY_BOX:
                    if (GameManager::getInstance()->collectedKeyCount > 0) {
                        GameManager::getInstance()->collectedKeyCount--;
                        DUAL_HUD->keyCollected(GameManager::getInstance()->collectedKeyCount);
                        drop->stopAllActions();
                        player->stopAndStand();
//                        player->face->setVisible(true);
                        GameManager::getInstance()->playSoundEffect(SOUND_POWER_UP_45);
                        if (drop->getTag() == UNIT_SUITCASE) {
                            player->freezed = true;
                            Animation* animation = AnimationCache::getInstance()->getAnimation(__String::createWithFormat("suitcaseOpen%d", drop->secondTag)->getCString());
                            Animate* animate = Animate::create(animation);
                            drop->runAction(Sequence::create(DelayTime::create(1), animate, CallFuncN::create(CC_CALLBACK_1(DualWorld::openWeapon, this)),NULL));
                            drop->setSpriteFrame(__String::createWithFormat("suitCaseUnlocked%d.png", drop->secondTag)->getCString());
                        }else if(drop->getTag() == UNIT_JEWELRY_BOX) {
                            Animation* animation = AnimationCache::getInstance()->getAnimation("jewelryBoxOpen");
                            Animate* animate = Animate::create(animation);
                            drop->runAction(Sequence::create(DelayTime::create(1), animate, CallFuncN::create(CC_CALLBACK_1(DualWorld::onjewelryBoxOpen, this)),NULL));
                            drop->secondTag = stageIndex;
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
                    
                    DUAL_HUD->starCollected(starCount);
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
                    this->schedule(schedule_selector(DualWorld::jumpBySpring), 0.05f, 4, 0);
                    Animation* animation = AnimationCache::getInstance()->getAnimation("theme0Spring");
                    Animate* animate = Animate::create(animation);
                    drop->runAction(animate);
                    GameManager::getInstance()->playSoundEffect(SOUND_SPRING);
                    break;
                }
                case UNIT_SPRING_THEME2:
                {
                    this->schedule(schedule_selector(DualWorld::jumpBySpring), 0.05f, 4, 0);
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
                    this->schedule(schedule_selector(DualWorld::immortalForAWhile), 0.1);
                    
                    playerIgnoreGravity = true;
                    if (player->currentPlatform) {
                        player->currentPlatform->player= NULL;
                        player->currentPlatform = NULL;
                    }
                    
                    player->runAction(EaseIn::create(MoveTo::create(duration, tele->getPosition()), 0.3));
                    player->runAction(RepeatForever::create(RotateBy::create(0.5, 360)));
                    player->runAction(ScaleTo::create(duration, 0.1));
                    this->scheduleOnce(schedule_selector(DualWorld::teleportLater), duration);
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

void DualWorld::teleportLaterForThemeThree(float dt){
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
void DualWorld::teleportLater(float dt){
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
        this->scheduleOnce(schedule_selector(DualWorld::teleportLaterForThemeThree), dur);
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
void DualWorld::showCoinCount(Point pos, int count){
    return;
    lblCoinGot->setPosition(pos);
    lblCoinGot->setOpacity(255);
    lblCoinGot->setString(Value(count).asString());
    Action* action = Sequence::create(EaseOut::create(MoveBy::create(1, Point(0, 20)), 8), FadeOut::create(1), nullptr);
    action->setTag(ACTION_TAG_ANIMATION);
    lblCoinGot->stopActionByTag(ACTION_TAG_ANIMATION);
    lblCoinGot->runAction(action);
//    lblCoinGot->runAction(Sequence::create(DelayTime::create(1), CallFuncN::create(CC_CALLBACK_1(DualWorld::nodeMoveDone, this)), NULL));
//    lbl->getTexture()->setAliasTexParameters();
}
void DualWorld::jumpBySpring(float dt)
{
    player->velocity.y = 500;
}
void DualWorld::runEffect(int effect, Point point, float angle){
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
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
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
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
        GameManager::getInstance()->playSoundEffect(SOUND_FIRE_SMALL);
    }else if(effect == EFFECT_BOMB_WALL){
        Sprite* explode = Sprite::createWithSpriteFrameName("bombEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bombEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
    }else if(effect == EFFECT_EXPLODE_SMALL){
//        float scale = 0.5f;
        duration = 0.15;
        Sprite* explode = Sprite::createWithSpriteFrameName("bombSmallEffect0.png");
        spriteBatch->addChild(explode);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bombSmallEffect");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
        explode->setPosition(point);
        explode->setScale(1.5f);
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
        explode->runAction(Sequence::create(DelayTime::create(duration*2), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
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
            particle->runAction(Sequence::create(EaseOut::create(MoveBy::create(baseDuration, Point(x, y)), easeRate), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
        }
    }else if(effect == EFFECT_EXPLODE_HUGE){
        Sprite* explode = Sprite::createWithSpriteFrameName("bombEffect0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        explode->setScale(1.5f);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bombEffect");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
    }else if(effect == EFFECT_METAL_SLUG_EXPLOSION){
        Sprite* explode = Sprite::createWithSpriteFrameName("explosion0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        explode->setScale(1.5f);
        Animation* animation = AnimationCache::getInstance()->getAnimation("explosion");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
    }else if(effect == EFFECT_SMOKE){
        float duration = 0.5;
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteExplode.png");
        spriteBatch->addChild(spt, -1);
        spt->setPosition(point);
        spt->setScale(0.5);
        spt->setRotation(rand()%360);
        spt->setColor(Color3B(200,200,200));
        spt->runAction(Sequence::create(EaseIn::create(MoveBy::create(duration, Point(0, 7)), 2), CallFuncN::create(CC_CALLBACK_1(DualWorld::enemyBaseMoveDone, this)), NULL));
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
        spt->runAction(Sequence::create(EaseIn::create(ScaleTo::create(duration, 0.05), 2), CallFuncN::create(CC_CALLBACK_1(DualWorld::enemyBaseMoveDone, this)), NULL));
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
        spt->runAction(Sequence::create(EaseIn::create(MoveBy::create(duration, Point(0, 7)), 2), CallFuncN::create(CC_CALLBACK_1(DualWorld::enemyBaseMoveDone, this)), NULL));
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
            particle->runAction(Sequence::create(EaseOut::create(MoveBy::create(duration, Point(x, y)), 1.6f), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
        }
    }
}
void DualWorld::runEffect(int effect, Point point)
{
    if (effect == EFFECT_EXPLODE_SMALL && rand()%2==0) {
        effect = EFFECT_EXPLODE_HUGE;
    }
    if (effect == EFFECT_EXPLODE_BIG){
        effect = EFFECT_EXPLODE_HUGE;
    }
    runEffect(effect, point, 0);
}
/*
void DualWorld::addGlowEffect(Sprite* sprite,const Color3B& colour, const Size& size)
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
void DualWorld::powerTestSchedule(float dt){
    powerTestTime += dt;
    if(powerTestTime > 10){
        powerTestTime = 0;
        totalPower = 0;
    }
    powerPerSec = totalPower/powerTestTime;
    CCLOG("power/sec: %f", powerPerSec);
}
Sprite* DualWorld::getLight(){
    Sprite* sptLight = Sprite::create("whiteBigCircle.png");
    BlendFunc f = {GL_DST_COLOR, GL_ONE};
    sptLight->setBlendFunc(f);
    sptLight->setOpacity(255);
    sptLight->setColor(Color3B(255, 180, 0));
    sptLight->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.0, 0.9), DelayTime::create(0.1), ScaleTo::create(0.0, 1), DelayTime::create(0.3), nullptr)));
    return sptLight;
}

void DualWorld::setLayerTag(TMXTiledMap* map){
    map->getLayer("unit")->setTag(TAG_UNIT_LAYER);
    map->getLayer("stage")->setTag(TAG_STAGE_LAYER);
    map->getLayer("back")->setTag(TAG_BACK_LAYER);
    map->getLayer("fore")->setTag(TAG_FORE_LAYER);
    map->getLayer("deco")->setTag(TAG_DECO_LAYER);
}
void DualWorld::setBossMap(int stage){
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
    
    DUAL_HUD->showStageTitle();
    stageIndex = stage;
    
    Rect rect;
    dnMiniMap = DrawNode::create();
    ScrollView* scrollView = ScrollView::create();
    DUAL_HUD->addChild(scrollView);
    //miniMapPos = Point(size.width - 10, size.height - 10);
    scrollView->setPosition(Point(size.width - 10, size.height - 10));
    scrollView->getInnerContainer()->addChild(dnMiniMap);
    scrollView->setAnchorPoint(Point(1, 1));
    scrollView->setContentSize(miniMapSize);
    
    TMXTiledMap* map = cocos2d::TMXTiledMap::create(buf);
    
    if(stage == STAGE_LOBBY){
        setLobby(map);
    }
    mapArray.pushBack(map);
    setLayerTag(map);
    this->addChild(map);
    mapRectArray.push_back(map->getBoundingBox());
    rect = map->getBoundingBox();
    map->setTag(1);
    setStage(map);
}
void DualWorld::setDualMap(){
    TMXTiledMap* map = cocos2d::TMXTiledMap::create("stages/darydual.tmx");
    theMap = map;
    setLayerTag(map);
    this->addChild(map);
    map->setTag(1);
    setStage(map);
    int playerAvatar = UDGetInt(KEY_AVATAR, 0);
    player = Hero::createHero(UNIT_DARY, 100, 21);
    player->initWithSpriteFrameName(strmake("hero%d_0.png", playerAvatar));
    switch (playerAvatar) {
        case 22:
            player->setRestOfBodyPart();
            player->gun->setVisible(false);
            break;
        default:
//            player->face->setVisible(false);
            break;
    }
    player->runAnimation(strmake("avatarIdle%d", playerAvatar).c_str(), true);
    spriteBatch->addChild(player);
    player->setPosition(map->getPosition() + Point(16*TILE_SIZE + 9, (42-16)*TILE_SIZE - 7));
    Sprite* flyingPlatform = Sprite::createWithSpriteFrameName("flying_platform0.png");
    player->addChild(flyingPlatform);
    flyingPlatform->setAnchorPoint(Point(0.5f, 1));
    flyingPlatform->setPosition(player->getContentSize().width/2, 0);
    GameManager::getInstance()->runAnimation(flyingPlatform, "flying_platform", true);
    
    
    enemy = Hero::createHero(UNIT_DARY, 100, 0);
    if(GM->isBossFight){
        enemy->initWithSpriteFrameName("gunHead.png");
    }else{
        enemy->initWithSpriteFrameName(strmake("hero%d_0.png", GM->dualEnemyChar));
        enemy->runAnimation(strmake("avatarIdle%d", GM->dualEnemyChar).c_str(), true);
    }
    
    spriteBatch->addChild(enemy);
    enemy->setFlippedX(true);
    log("enemy: %d", GM->dualEnemyChar);
    
    switch (GM->dualEnemyChar) {
        case 22:
            enemy->setRestOfBodyPart();
            enemy->gun->setVisible(false);
            break;
        default:
//            enemy->face->setVisible(false);
            break;
    }
    enemy->setPosition(map->getPosition() + Point(25*TILE_SIZE + 9, (42-16)*TILE_SIZE - 7));
    flyingPlatform = Sprite::createWithSpriteFrameName("flying_platform0.png");
    enemy->addChild(flyingPlatform);
    flyingPlatform->setAnchorPoint(Point(0.5f, 1));
    flyingPlatform->setPosition(enemy->getContentSize().width/2, 0);
    GameManager::getInstance()->runAnimation(flyingPlatform, "flying_platform", true);
    
    this->setAnchorPoint(Point(0,0));
//    this->setScale(4);
    Action* action = EaseInOut::create(ScaleTo::create(3, 4), 2);
    action->setTag(94);
    this->runAction(action);
    loadCharData();
    
    Sprite* spt = Sprite::create("blur_tile_bg.jpg");
    this->addChild(spt, -100);
    spt->setPosition((player->getPosition() + enemy->getPosition())/2);
    
    setViewPointCenter((player->getPosition() + enemy->getPosition())/2);
}
void DualWorld::startBattle(){
    this->stopActionByTag(94);
    this->runAction(EaseInOut::create(ScaleTo::create(2, 3), 2));
    
    this->runAction(Sequence::create(DelayTime::create(0.1f), CallFunc::create(CC_CALLBACK_0(DualWorld::showWarpAnimation, this)), NULL));
    
    int enemyFamily = 1;// test  GameManager::getInstance()->enemyDoc["family"].GetInt();
    int playerFamily = 1;// test  UserDefault::getInstance()->getIntegerForKey(KEY_FAMILY, FAMILY_LIONSTAR);
    bool isRivalMatch = enemyFamily != playerFamily;
    if (isRivalMatch) {
        rivalMatchLayer = CSLoader::createNode("RivalMatch.csb");
        rivalMatchLayer->setPositionY(size.height/2 - rivalMatchLayer->getContentSize().height/2);
        DUAL_HUD->addChild(rivalMatchLayer, 99003);
        GameManager::getInstance()->gatherUp(rivalMatchLayer);
        this->runAction(Sequence::create(DelayTime::create(0.7f), CallFunc::create(CC_CALLBACK_0(DualWorld::runRivalMatchAnimation, this)), nullptr));
        this->runAction(Sequence::create(DelayTime::create(1.7f), CallFunc::create(CC_CALLBACK_0(DualWorld::splitUp, this)), nullptr));
        
//        int enemyFamily = GameManager::getInstance()->enemyDoc["family"].GetInt();
//        int playerFamily = UserDefault::getInstance()->getIntegerForKey(KEY_FAMILY, FAMILY_LIONSTAR);
        ImageView* img = (ImageView*)rivalMatchLayer->getChildByName("imgPlayerFlag");
        GameManager::getInstance()->loadFlagImage(img, playerFamily);
        img = (ImageView*)rivalMatchLayer->getChildByName("imgEnemyFlag");
        GameManager::getInstance()->loadFlagImage(img, enemyFamily);
        
        Text* lbl = (Text*)rivalMatchLayer->getChildByName("lblTitle");
        LM->setLocalizedString(lbl, "rival match");
        lbl = (Text*)rivalMatchLayer->getChildByName("lblDescription");
        LM->setLocalizedString(lbl, "rival match description");
    }else{
        this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(DualWorld::readySetBattle, this)), DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(DualWorld::readySetBattle, this)), DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(DualWorld::readySetBattle, this)), NULL));
    }
    
    sptRedFace->runAction(Sequence::create(DelayTime::create(0.9f), ScaleTo::create(0.1f, 1), nullptr));
    sptRedFace->runAction(Sequence::create(DelayTime::create(0.9f), FadeIn::create(0.1f), nullptr));
}
void DualWorld::runRivalMatchAnimation(){
    cocostudio::timeline::ActionTimeline* action = CSLoader::createTimeline("RivalMatch.csb");
    rivalMatchLayer->runAction(action);
    action->gotoFrameAndPlay(0, true);
}
void DualWorld::splitUp(){
    rivalMatchLayer->stopAllActions();
    GameManager::getInstance()->splitUp(rivalMatchLayer);
    
    this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(DualWorld::readySetBattle, this)), DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(DualWorld::readySetBattle, this)), DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(DualWorld::readySetBattle, this)), NULL));
}
void DualWorld::loadCharData(){
    int index = 0;
    // player weapon
    int weaponIndex = 0;
    int shieldIndex = 0;
    int itemGapX = 20;
    int itemGapY = 15;
    float itemMoveDur = 1;
 
    float dur = 0.4f;
//    // test
//    UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 0).c_str(), -1);
//    UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 1).c_str(), -1);
//    UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 2).c_str(), -1);
//    UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, 3).c_str(), 17);
    
    std::string playerName = UserDefault::getInstance()->getStringForKey(KEY_NAME, "Guest");
    int playerHealth = 0;
    int playerAttack = 0;
    int runeCountArray[4] = {0,0,0,0};
    for (int i = 0; i < 4; i++) {
        int value = UDGetInt(strmake(KEY_WEAPON_SLOT_FORMAT, i).c_str(), -1);
        if(value < 0){
            continue;
        }
        Weapon* wp = Weapon::create();
        wp->setWeapon(value);
        runeCountArray[value%4]++;
        spriteBatch->addChild(wp, player->getLocalZOrder() + 1);
        playerWeapons.pushBack(wp);
//        wp->runAction(RepeatForever::create(RotateBy::create(1, 10)));// test
        wp->isEnemy = false;
        playerAttack += wp->powerPerAttack/wp->shootInterval;
        weaponIndex++;
    }
    for (int i = 0; i < 4; i++) {
        int value = UDGetInt(strmake(KEY_SHIELD_SLOT_FORMAT, i).c_str(), -1);
        if(value < 0){
            continue;
        }
        runeCountArray[value%4]++;
        Shield* shield = Shield::create();
        shield->setShield(value);
        this->addChild(shield);
        playerShields.pushBack(shield);
        playerHealth += shield->energy;
        shieldIndex++;
    }
    Text* lbl;
    int totalRuneCount = 0;
    for (int i = 0; i < 4; i++) {
        totalRuneCount += runeCountArray[i];
    }
    int totalPercent = 0;
    int percent;
    for (int i = 0; i < 4; i++) {
        lbl = (Text*)DUAL_HUD->hudLayer->getChildByName(strmake("lblPlayerRune%d", i));
        percent = runeCountArray[i]*100/totalRuneCount;
        if(i == 3){
            percent = 100 - totalPercent;
        }
        lbl->setString(strmake("%d%%", percent));
        totalPercent += percent;
    }
    
    ImageView* img = (ImageView*)DUAL_HUD->hudLayer->getChildByName("imgPlayerFlag");
    img->loadTexture("wolfFlagSmall.png");
    
    
//    while(true){
//        std::string item = UserDefault::getInstance()->getStringForKey(StringUtils::format(KEY_ITEM_EQUIPPED_FORMAT, index).c_str(), "");
//        if (item.size() > 0) {
//            ValueVector rows = GameManager::getInstance()->split(item, ",");
//            if (rows.at(0).asInt() == ITEM_TYPE_WEAPON) {
//                Weapon* wp = Weapon::create();
//                wp->fromString(item);
//                spriteBatch->addChild(wp, player->getLocalZOrder() + 1);
//                playerWeapons.pushBack(wp);                wp->isEnemy = false;
//                playerAttack += wp->totalPower;
//                weaponIndex++;
//            }else if (rows.at(0).asInt() == ITEM_TYPE_SHIELD) {
//                Shield* shield = Shield::create();
//                shield->fromString(item);
//                spriteBatch->addChild(shield);
//                playerShields.pushBack(shield);
//                playerHealth += shield->energy;
//                shieldIndex++;
//            }
//        }else{
//            break;
//        }
//        index++;
//    }
    player->maxEnergy += playerHealth;
    playerHealth = player->maxEnergy;
    player->energy = player->maxEnergy;
    
    std::string enemyName = GM->dualEnemyName;// test  GameManager::getInstance()->enemyDoc["name"].GetString();
//    rapidjson::Value& array = GameManager::getInstance()->enemyDoc["public_info"];
    std::string enemyEquip = GM->dualEnemyEquip;
    ValueVector enemyEquipValueArray = GM->split(enemyEquip, ",");
    std::string enemyRune = GM->dualEnemyRune;
    ValueVector enemyRuneValueArray = GM->split(enemyRune, ",");
    std::string enemyArmor = GM->dualEnemyArmor;
    ValueVector enemyArmorValueArray = GM->split(enemyArmor, ",");
    std::string enemyResearch = GM->dualEnemyResearch;
    ValueVector enemyResearchValueArray = GM->split(enemyResearch, ",");
    
    img = (ImageView*)DUAL_HUD->hudLayer->getChildByName("imgEnemyFlag");
    img->loadTexture("lionFlagSmall.png");
    
    for(int i = 0; i < 4; i++){
        runeCountArray[i] = 0;
    }
    weaponIndex = 0;
    shieldIndex = 0;
    int enemyHealth = 0;
    if(GM->isBossFight){
        enemyHealth = 10000;
    }
    int enemyAttack = 0;
    for (int i = 0; i < 4; i++) {
        int value = enemyEquipValueArray.at(i).asInt();
        if(value < 0){
            continue;
        }
        Weapon* wp = Weapon::create();
        wp->setWeapon(value);
        runeCountArray[value%4]++;
        
        wp->shootInterval = GM->getWeaponCoolTime(value%1000)*(1 + enemyResearchValueArray.at(0).asInt()*-1*0.01f);
//        wp->shootInterval /= 2;
        
        float attackBuff = 1;
        for (int i = 0; i < enemyRuneValueArray.size(); i++) {
            int rune = enemyRuneValueArray.at(i).asInt();
            if(rune > 0){
                if(rune%4 == value%4 && GM->getRuneBuffType(rune) == RUNE_BUFF_TYPE_ATTACK){
                    attackBuff += GM->getRuneBuff(rune/1000, RUNE_BUFF_TYPE_ATTACK);
                }
            }
        }
        wp->powerPerAttack = GM->getWeaponPower(value%1000, value/1000)*(1 + attackBuff*0.01f)*(1 + enemyResearchValueArray.at(1).asInt()*0.01f);
        
        float criticalPossibility = 0;
        for (int i = 0; i < 4; i++) {
            int rune = enemyRuneValueArray.at(i).asInt();//UDGetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, i).c_str(), -1);
            if(rune > 0){
                if(rune%4 == value%4 && GM->getRuneBuffType(rune) == RUNE_BUFF_TYPE_CRI){
                    criticalPossibility += GM->getRuneBuff(rune/1000, RUNE_BUFF_TYPE_CRI);
                }
            }
        }
        wp->criticalPossibility = DEFAULT_CRITICAL + criticalPossibility + enemyResearchValueArray.at(2).asInt()*0.1f;
        wp->criticalPower = (200 + enemyResearchValueArray.at(3).asInt()*10)*0.01f;
        spriteBatch->addChild(wp);
        enemyWeapons.pushBack(wp);
        wp->setFlippedY(true);
        wp->setRotation(180);
        wp->isEnemy = true;
        enemyAttack += wp->powerPerAttack/wp->shootInterval;
        weaponIndex++;
    }
    for (int i = 4; i < 8; i++) {
        if(enemyEquipValueArray.size() <= i){
            continue;
        }
        int value = enemyEquipValueArray.at(i).asInt();
        if(value < 0){
            continue;
        }
        Shield* shield = Shield::create();
        shield->setShield(value);
        
        runeCountArray[value%4]++;
        float hp = GM->getShieldHp(value%1000, value/1000);
        float effectHPAmount = 0;
        for (int j = 0; j < 4; j++) {
            int rune = enemyRuneValueArray.at(j).asInt();//UDGetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, j).c_str(), -1);
            if(rune%4 == value%4 && GM->getRuneBuffType(rune) == RUNE_BUFF_TYPE_HP){
                effectHPAmount += GM->getRuneBuff(rune/1000, RUNE_BUFF_TYPE_HP);
            }
        }
        shield->energy = hp*(1 + effectHPAmount*0.01f);
        
        int effectDefAmount = 0;
        for (int j = 0; j < 4; j++) {
            int rune = enemyRuneValueArray.at(j).asInt();//UDGetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, j).c_str(), -1);
            if(rune%4 == value%4 && GM->getRuneBuffType(rune) == RUNE_BUFF_TYPE_DEF){
                effectDefAmount += GM->getRuneBuff(rune/1000, RUNE_BUFF_TYPE_DEF);
            }
        }
        shield->DP = effectDefAmount;
        
        this->addChild(shield);
        enemyShields.pushBack(shield);
        
        shield->setFlippedX(true);
        enemyHealth += shield->energy;
        shieldIndex++;
    }
    
    totalRuneCount = 0;
    for (int i = 0; i < 4; i++) {
        totalRuneCount += runeCountArray[i];
    }
    totalPercent = 0;
    if(totalRuneCount){
        for (int i = 0; i < 4; i++) {
            lbl = (Text*)DUAL_HUD->hudLayer->getChildByName(strmake("lblEnemyRune%d", i));
            percent = runeCountArray[i]*100/totalRuneCount;
            if(i == 3){
                percent = 100 - totalPercent;
            }
            lbl->setString(strmake("%d%%", percent));
            totalPercent += percent;
        }
    }
    
//    enemy->maxEnergy = 20000;// test
    enemy->maxEnergy += enemyHealth;
    enemyHealth = enemy->maxEnergy;
    enemy->energy = enemy->maxEnergy;
    log("name: %s", enemyName.c_str());
    int enemyTrophy = GM->dualEnemyTrophy;
    int playerTrophy = UDGetInt(KEY_TROPHY, 1000);
    DUAL_HUD->setInfo(playerName, enemyName, playerHealth, playerAttack, enemyHealth, enemyAttack, playerTrophy, enemyTrophy);
    enemyArray.pushBack(enemy);
    for(auto weapon : playerWeapons){
        if(weapon->weaponIndex == WEAPON_LASER_GUN || weapon->weaponIndex == WEAPON_LASER_COIN){
            for(auto shield : enemyShields){
                weapon->targetList.pushBack(shield);
            }
            weapon->targetList.pushBack(enemy);
        }
    }
    for(auto weapon : enemyWeapons){
        if(weapon->weaponIndex == WEAPON_LASER_GUN || weapon->weaponIndex == WEAPON_LASER_COIN){
            for(auto shield : playerShields){
                weapon->targetList.pushBack(shield);
            }
            weapon->targetList.pushBack(player);
        }
    }
}
void DualWorld::readySetBattle(){
    DUAL_HUD->readySetBattle();
    
    int index = 0;
    // player weapon
    int weaponIndex = 0;
    int shieldIndex = 0;
    int itemGapX = 20;
    int itemGapY = 15;
    float itemMoveDur = 1;
    
    if (readySetBattleState == 0) {
        float dur = 0.8f;
        int angle = -10;
        float xToMove = -20;
        float yToMove = 60;
        player->runAction(Sequence::create(EaseInOut::create(RotateBy::create(dur/3, angle), 2), DelayTime::create(dur/3), EaseInOut::create(RotateBy::create(dur/3, -angle), 2), NULL));
        player->runAction(EaseInOut::create(MoveBy::create(dur, Point(xToMove, yToMove)), 2));
        enemy->runAction(Sequence::create(EaseInOut::create(RotateBy::create(dur/3, -angle), 2), DelayTime::create(dur/3), EaseInOut::create(RotateBy::create(dur/3, angle), 2), NULL));
        enemy->runAction(EaseInOut::create(MoveBy::create(dur, Point(-xToMove, yToMove)), 2));
//        this->runAction(ScaleTo::create(dur, 4));
    }else if (readySetBattleState == 1) {
        float dur = 0.4f;
        std::string playerName = UserDefault::getInstance()->getStringForKey(KEY_NAME, "Guest");
        int playerHealth = 0;
        int playerAttack = 0;
        weaponIndex = 0;
        for(auto wp: playerWeapons){
            wp->setPosition(player->getPosition());
            wp->runAction(EaseOut::create(MoveTo::create(dur, player->getPosition() + Point(-itemGapX*(weaponIndex + 1), weaponIndex%2==0?itemGapY:-itemGapY)), 2));
            weaponIndex++;
        }
        for (auto shield: playerShields) {
            shield->setPosition(player->getPosition());
            shield->runAction(EaseOut::create(MoveTo::create(dur, player->getPosition() + Point(itemGapX*(shieldIndex + 1), shieldIndex%2==0?-itemGapY:itemGapY)), 2));
            shieldIndex++;
        }
        
//        std::string enemyName = "enemy name";// test  GameManager::getInstance()->enemyDoc["name"].GetString();
//        rapidjson::Value& array = GameManager::getInstance()->enemyDoc["public_info"];
        weaponIndex = 0;
        shieldIndex = 0;
        int enemyHealth = 0;
        int enemyAttack = 0;
        for(auto wp : enemyWeapons){
            wp->setPosition(enemy->getPosition());
            wp->runAction(EaseOut::create(MoveTo::create(dur, enemy->getPosition() + Point(itemGapX*(weaponIndex + 1), weaponIndex%2==0?itemGapY:-itemGapY)), 2));
            weaponIndex++;
        }
        for(auto shield: enemyShields){
            shield->setPosition(enemy->getPosition());
            shield->runAction(EaseOut::create(MoveTo::create(dur, enemy->getPosition() + Point(-itemGapX*(shieldIndex + 1), shieldIndex%2==0?-itemGapY:itemGapY)), 2));
            shieldIndex++;
        }
//        for (int i = 0; i < array.Size(); i++){
//            log("name: %s, info: %s", enemyName.c_str(), array[i].GetString());
//            std::string item = array[i].GetString();
//            if (item.size() > 0) {
//                ValueVector rows = GameManager::getInstance()->split(item, ",");
//                if (rows.at(0).asInt() == ITEM_TYPE_WEAPON) {
//                    Weapon* wp = enemyWeapons.at(weaponIndex);
//                    wp->setPosition(enemy->getPosition());
//                    wp->runAction(EaseOut::create(MoveTo::create(dur, enemy->getPosition() + Point(itemGapX*(weaponIndex + 1), weaponIndex%2==0?itemGapY:-itemGapY)), 2));
//                    weaponIndex++;
//                }else if (rows.at(0).asInt() == ITEM_TYPE_SHIELD) {
//                    Shield* shield = enemyShields.at(shieldIndex);
//                    shield->setPosition(enemy->getPosition());
//                    shield->runAction(EaseOut::create(MoveTo::create(dur, enemy->getPosition() + Point(-itemGapX*(shieldIndex + 1), shieldIndex%2==0?-itemGapY:itemGapY)), 2));
//                    shieldIndex++;
//                }
//            }
//        }
    }else if (readySetBattleState == 2) {
        weaponIndex = 0;
        shieldIndex = 0;
        for(auto wp: playerWeapons){
            if (weaponIndex%2==0) {
                wp->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), NULL)));
            }else{
                wp->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), NULL)));
            }
            
            wp->startShooting();
            weaponIndex++;
        }
        for (auto shield: playerShields) {
            if (shieldIndex%2==1) {
                shield->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), NULL)));
            }else{
                shield->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), NULL)));
            }
            shieldIndex++;
        }
        
//        while(true){
//            std::string item = UserDefault::getInstance()->getStringForKey(StringUtils::format(KEY_ITEM_EQUIPPED_FORMAT, index).c_str(), "");
//            if (item.size() > 0) {
//                ValueVector rows = GameManager::getInstance()->split(item, ",");
//                if (rows.at(0).asInt() == ITEM_TYPE_WEAPON) {
//                    Weapon* wp = playerWeapons.at(weaponIndex);
//                    if (weaponIndex%2==0) {
//                        wp->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), NULL)));
//                    }else{
//                        wp->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), NULL)));
//                    }
//                    if (wp->value%1000 == WEAPON_LASER_GUN || wp->value%1000 == WEAPON_LASER_COIN) {
//                        Laser* laser;
//                        if (wp->value%1000 == WEAPON_LASER_GUN) {
//                            GameManager::getInstance()->playSoundEffect(SOUND_LASER_BUZZ);
//                            laser = Laser::create("laserThinBlue.png", "laserThinBlueGathering.png", "laserThinBlueHit");
//                        }else{
//                            GameManager::getInstance()->playSoundEffect(SOUND_LASER_WOONG);
//                            laser = Laser::create("bigLaserLine.png", "bigLaserCircle.png", "");
//                            laser->setScaleY(0.4);
//                            laser->sptBeam->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
//                            laser->sptHit->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
//                            laser->sptHit->addChild(getLight());
//                            laser->sptBeam->addChild(getLight());
//                            laser->sptHit->setScale(2, 1.6);
//                        }
//
//
//                        for(auto shield:enemyShields){
//                            laser->enemyArray.pushBack(shield);
//                        }
//                        laser->enemyArray.pushBack(enemy);
//                        laser->rectArray = stageTileRectArray;
//                        this->addChild(laser);
//                        laser->setTag((int)laserArray.size() + 793);
//                        wp->setTag((int)laserArray.size() + 793);
//                        laserArray.pushBack(laser);
//                    }
//
//
//                    wp->startShooting();
//                    weaponIndex++;
//                }else if (rows.at(0).asInt() == ITEM_TYPE_SHIELD) {
//                    Shield* shield = playerShields.at(shieldIndex);
//                    if (shieldIndex%2==1) {
//                        shield->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), NULL)));
//                    }else{
//                        shield->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), NULL)));
//                    }
//                    shieldIndex++;
//                }
//
//            }else{
//                break;
//            }
//            index++;
//        }
        
//        std::string enemyName = "enemy name";// test  GameManager::getInstance()->enemyDoc["name"].GetString();
//        rapidjson::Value& array = GameManager::getInstance()->enemyDoc["public_info"];
        weaponIndex = 0;
        shieldIndex = 0;
        
        for(auto wp : enemyWeapons){
            if (weaponIndex%2==0) {
                wp->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), NULL)));
            }else{
                wp->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), NULL)));
            }
            
            if (wp->value%1000 == WEAPON_LASER_GUN || wp->value%1000 == WEAPON_LASER_COIN) {
//                Weapon* wp = getWeaponByTag(laser->getTag());
//                laser->setPositionY(wp->getPositionY());
//                
//                float gunWidth = wp->getContentSize().width*(wp->getScaleX() < 0?-1:1);
//                float x = cos(0*3.14/180)*gunWidth;
//                float y = -sin(0*3.14/180)*gunWidth;
//                Point gunLengthPos = Point(x, y);
//                laser->setLaser(wp->getPosition() + gunLengthPos, wp->getScaleX() < 0?180:0);
//                
//                if (laser->hitEnemy != nullptr)
//                {
//                    //            if (laserEnemiesToRemove.find(laser->hitEnemy) == std::vector) {
//                    if (std::find(laserEnemiesToRemove.begin(), laserEnemiesToRemove.end(), laser->hitEnemy) != laserEnemiesToRemove.end()){
//                        laser->hitEnemy = nullptr;
//                    }else{
//                        float demage = wp->powerPerAttack;//wp->totalPower*dt;
//                        laser->hitEnemy->energy -= demage;
//                        
//                        showDamage(demage, laser->hitEnemy->getPosition());
//                        if (laser->hitEnemy->energy <= 0) {
//                            //                enemiesToRemove.pushBack(laser->hitEnemy);
//                            for(auto en:enemyArray){
//                                laser->enemyArray.pushBack(en);
//                            }
//                            laser->enemyArray.eraseObject(laser->hitEnemy);
//                            laserEnemiesToRemove.pushBack(laser->hitEnemy);
//                            this->runEffect(EFFECT_EXPLODE_BIG, laser->hitEnemy->getPosition());
//                            //                this->createRewards(laser->hitEnemy);
//                            //                if (laser->hitEnemy->rewards > 0) {
//                            //                    enemyKilledCount++;
//                            //                }
//                            laser->hitEnemy = nullptr;
//                        }else
//                        {
//                            laser->hitEnemy->blinkForAWhile();
//                        }
//                    }
//                }
//                
            }
            
            wp->startShooting();
            weaponIndex++;
        }
        for(auto shield: enemyShields){
            if (shieldIndex%2==1) {
                shield->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), NULL)));
            }else{
                shield->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), NULL)));
            }
            shieldIndex++;
        }
//        for (int i = 0; i < array.Size(); i++){
//            std::string item = array[i].GetString();
//            if (item.size() > 0) {
//                ValueVector rows = GameManager::getInstance()->split(item, ",");
//                if (rows.at(0).asInt() == ITEM_TYPE_WEAPON) {
//                    Weapon* wp = enemyWeapons.at(weaponIndex);
//                    if (weaponIndex%2==0) {
//                        wp->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), NULL)));
//                    }else{
//                        wp->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), NULL)));
//                    }
//
//                    if (wp->value%1000 == WEAPON_LASER_GUN || wp->value%1000 == WEAPON_LASER_COIN) {
//                        Laser* laser;
//                        if (wp->value%1000 == WEAPON_LASER_GUN) {
//                            GameManager::getInstance()->playSoundEffect(SOUND_LASER_BUZZ);
//                            laser = Laser::create("laserThinBlue.png", "laserThinBlueGathering.png", "laserThinBlueHit");
//                        }else{
//                            GameManager::getInstance()->playSoundEffect(SOUND_LASER_WOONG);
//                            laser = Laser::create("bigLaserLine.png", "bigLaserCircle.png", "");
//                            laser->setScaleY(0.4);
//                            laser->sptBeam->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
//                            laser->sptHit->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
//                            laser->sptHit->addChild(getLight());
//                            laser->sptBeam->addChild(getLight());
//                            laser->sptHit->setScale(2, 1.6);
//                        }
//                        for(auto shield:playerShields){
//                            laser->enemyArray.pushBack(shield);
//                        }
//                        laser->enemyArray.pushBack(player);
//                        laser->rectArray = stageTileRectArray;
//                        this->addChild(laser);
//                        laser->setTag((int)laserArray.size() + 793);
//                        wp->setTag((int)laserArray.size() + 793);
//                        laserArray.pushBack(laser);
//                    }
//
//                    wp->startShooting();
//                    weaponIndex++;
//                }else if (rows.at(0).asInt() == ITEM_TYPE_SHIELD) {
//                    Shield* shield = enemyShields.at(shieldIndex);
//                    if (shieldIndex%2==1) {
//                        shield->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), NULL)));
//                    }else{
//                        shield->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, itemGapY*2)), 2), EaseInOut::create(MoveBy::create(itemMoveDur, Point(0, -itemGapY*2)), 2), NULL)));
//                    }
//                    shieldIndex++;
//                }
//            }
//        }
    }
    // Betting - Buff or lose
    
    // Factory - quest->25 win, 38 gameplay, 5 suitcases -> reward: golden wrench
    // Golden wrench - 20 percent to upgarde
    // Tower - PVE: Continous battle. Rewards every 10 or 20 floors.
    
    
    // key - time, restore
    
    readySetBattleState++;
}
Sprite* DualWorld::getWeapon(int weaponType){
    if (weaponType == WEAPON_PISTOL) {
        Sprite* sptWeapon = Sprite::createWithSpriteFrameName(GameManager::getInstance()->getWeaponImageFileName(weaponType));
        sptWeapon->setScale(0.5f);
        return sptWeapon;
    }
    return nullptr;
}
Weapon* DualWorld::getWeaponByTag(int tag){
    for(auto wp : playerWeapons){
        if(wp->getTag() == tag){
            return wp;
        }
    }
    for(auto wp : enemyWeapons){
        if(wp->getTag() == tag){
            return wp;
        }
    }
    return nullptr;
}
void DualWorld::setEntireMap(int stage){
    srand(GameManager::getInstance()->getTodaysRandom());
    int tilesetIndex = rand()%3;
    tilesetIndex = 2;
    if (stage < 1) {
        Director::getInstance()->tilesetName = "dungeonTileset.png";
    }else if(stage%3 == 1){
        Director::getInstance()->tilesetName = "brickTileset.png";
    }else if(stage%3 == 2){
        Director::getInstance()->tilesetName = "stoneTileset.png";
    }
    
    DUAL_HUD->showStageTitle();
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
    DUAL_HUD->addChild(scrollView);
    scrollView->setPosition(Point(size.width - 10, size.height - 10));
    scrollView->setScrollBarEnabled(false);
    scrollView->getInnerContainer()->addChild(dnMiniMap);
    scrollView->setAnchorPoint(Point(1, 1));
    scrollView->setContentSize(miniMapSize);
    scrollView->setBackGroundColor(Color3B::BLACK);
    scrollView->setBackGroundColorOpacity(100);
    scrollView->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    log("mini map pos: %f, %f", scrollView->getPositionX(), scrollView->getPositionY());
    setStage(map);
    
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
                mapCount--;
                isNormalMap = true;
            }else if(weaponRoomCount > 0){
                sprintf( buf, "stages/ldd_weapon_chest%d.tmx", 0);
                weaponRoomCount--;
                tag = MAP_WEAPON;
                isNormalMap = false;
            }else if(fineMapCount > 0){
                sprintf( buf, "stages/ldd_fine%d.tmx", 1);//rand()%6);
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
void DualWorld::tournamentSchedule(float dt){
    if (enemyArray.size() > 0) {
        return;
    }
    
    if (tournamentWaveIndex == 0) {
        
    }
}
void DualWorld::addEnemiesToMap(TMXTiledMap* map, int levelScore, bool blueKey){
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
    
//    this->runAction(Sequence::create(DelayTime::create(dur), CallFunc::create(CC_CALLBACK_0(DualWorld::loadEnemies, this)), NULL));
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
            if ((isBlock(tileIndex) || isOneWay(tileIndex)) && map->getLayer("stage")->getTileGIDAt(pos) == 0 && tileIndex != 162) {
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
void DualWorld::setEmptyMap(TMXTiledMap* map){
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

FireableBase* DualWorld::addEnemyToLoadStack(TMXTiledMap* map, int levelScore, Point pos, int missile, int enemyModel, bool addGround){
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
    
    fEnemy->setPosition(Point(0, 10000));
    //                enemyArray.pushBack(fEnemy);
    //                fireableArray.pushBack(fEnemy);
    //                droppableArray.pushBack(fEnemy);
    enemiesToLoad.pushBack(fEnemy);
    
    return fEnemy;
}
void DualWorld::loadEnemies(){
    for(auto enemy: enemiesToLoad){
        enemyArray.pushBack(enemy);
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
void DualWorld::addAppearEffect(Sprite* parent){
    Sprite* sptEffect = Sprite::createWithSpriteFrameName("bombEffect0.png");
    sptEffect->setPosition(Point(parent->getContentSize().width/2, parent->getContentSize().height/2));
    sptEffect->runAction(RotateBy::create(1, 400));
    sptEffect->setScale(0.5);
    sptEffect->runAction(Sequence::create(ScaleTo::create(0.3, 1.5), ScaleTo::create(0.2, 0.1), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptEffect)), NULL));
    parent->addChild(sptEffect);
}
void DualWorld::updateMiniMap(){
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
void DualWorld::addMapToMiniMap(TMXTiledMap* map){
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
void DualWorld::setNamingRoom(TMXTiledMap* map){
    
}
void DualWorld::setLobby(TMXTiledMap* map){
    if (UserDefault::getInstance()->getBoolForKey(KEY_IS_TUTORIAL_CLEAR, false)) {  // naming
        map->getLayer("stage")->removeTileAt(Point(21, 17));
        map->getLayer("stage")->removeTileAt(Point(22, 17));
        map->getLayer("stage")->removeTileAt(Point(23, 17));
        map->getLayer("stage")->removeTileAt(Point(21, 18));
        map->getLayer("stage")->removeTileAt(Point(22, 18));
        map->getLayer("stage")->removeTileAt(Point(23, 18));
    }
    for (int i = 0; i < WEAPON_NO_MORE; i++) {
        setWeaponAtSlot(i, WEAPON_NOT_EXIST);
    }
    GameManager::getInstance()->isWeaponSet = false;
    changeWeapon(0);
    GameManager::getInstance()->totalKill = 0;
    GameManager::getInstance()->totalDestroy=0;
    GameManager::getInstance()->totalCoin=0;
    GameManager::getInstance()->totalRoom=0;
    GameManager::getInstance()->totalBoss=0;
    GameManager::getInstance()->totalTime=60*60;
    GameManager::getInstance()->totalScore = 0;
}
bool DualWorld::isRoomEmpty(Rect rect){
    Rect targetRect;
    for (int i = 0; i < mapArray.size(); i++) {
        targetRect = mapArray.at(i)->getBoundingBox();
        
        if (intersectsRect(rect, targetRect)) {
            return false;
        }
        
    }
    return true;
}
TMXTiledMap* DualWorld::findDirectionAndPlaceTheMap(TMXTiledMap* srcMap, TMXTiledMap* dstMap){
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
float DualWorld::getMin(float src, float dst){
    return src < dst?src:dst;
}
float DualWorld::getMax(float src, float dst){
    return src > dst?src:dst;
}
void DualWorld::setStage(TMXTiledMap* tileMap)
{
//    Sprite* sptLight = getLight();
//    this->addChild(sptLight, 1);
//    sptLight->setPosition(100, 100);
    
    if (tileMap->getTag() == MAP_SHOP) {
        GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("found a shop").c_str(), DUAL_HUD);
    }else if (tileMap->getTag() == MAP_WEAPON) {
        GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("suitcase room").c_str(), DUAL_HUD);
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
    int talkCount = 0;
    
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
                    DUAL_HUD->keyCollected(GameManager::getInstance()->collectedKeyCount);
                    enemyKilledCount = 0;
                    enemyMaxCount = 0;
                    collectedCoinCount = 0;
//                    NativeInterface::NativeInterface::destroyAds();
                    
                    GameManager::getInstance()->setButtonSize(GameManager::getInstance()->getButtonSize());
                    playerFireCoolTime = 0;
                    indestructableTime = 0;
                    starCount = 0;
                    
                    float spd = 80;
                    int eng = 10; // test
                    if (GameManager::getInstance()->isWeaponSet) {
                        eng = GameManager::getInstance()->life;
                    }
                    int playerType = UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_CHARACTER, CHARACTER_DARY);
                    int weapon = UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_WEAPON, WEAPON_PISTOL);
                    if (GameManager::getInstance()->developer) {
                        //        weapon = WEAPON_GUIDED_ROCKET_LAUNCHER;
                    }
                    
                    spd = 10;
                    
                    heroPositionArray = PointArray::create(50);
                    heroPositionArray->retain();
                    
                    
                    
                    player= Hero::createHero(playerType, eng, spd);
                    player->weapon = weapon;
                    player->doubleJump = true;
                    player->maxEnergy = eng;
                    player->energy = eng;
                    player->setRestOfBodyPart();
                    player->runHatCheck();
                    player->oneWayCancelY = -1;
                    player->setFlippedX(player->isTowardLeft);
                    setPlayerInfo();
                    
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
                    DUAL_HUD->setLife(player->energy);
                    setPlayerInfo();
                    /*setWeaponInfo(weapon);
                     totalBulletCountMax = getMaxLoadedBulletCount(weapon);
                     totalBulletCount = totalBulletCountMax;
                     bulletCount = 0;
                     bulletCountMax = getMaxBulletCount(weapon);
                     DUAL_HUD->setWeapon(weapon);
                     DUAL_HUD->setBulletCount(bulletCount,bulletCountMax);
                     DUAL_HUD->setTotalBulletCount(totalBulletCount, totalBulletCountMax);
                     reload();
                     */
                    
                    if (!GameManager::getInstance()->isWeaponSet) {
                        GameManager::getInstance()->isWeaponSet = true;
                        
                        currentSlot = 0;
                        setWeaponAtSlot(0, -1);
                        setTotalBulletCountAtSlot(currentSlot, getMaxTotalBulletCount(WEAPON_PISTOL));
                        setLoadedBulletCountAtSlot(currentSlot, getMaxLoadedBulletCount(WEAPON_PISTOL));
                        addWeapon(WEAPON_PISTOL);
                        addWeapon(WEAPON_UZI);
                        addWeapon(WEAPON_LIGHTNING_GUN);
                        GameManager::getInstance()->collectedKeyCount = 0; // test
                    }
                    
                    changeWeapon(0);
                    
                    DUAL_HUD->initializeSreen(eng);
                    
                    if(GameManager::getInstance()->isVideoRewardEnergy){
                        DUAL_HUD->addEnergyEffect();
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
                    
                    //    this->addChild(tileMap, -2);
                    this->addChild(spriteBatchForHero, unitLayer->getLocalZOrder() + 1);
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
                        foreLayer->setLocalZOrder(25);
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
                    player->gun->setOpacity(0);
                    player->gun->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.1), NULL));
                    
//                    collectedCoinCount = 2000; // test
//                    GameManager::getInstance()->setCoin(collectedCoinCount); // test
//                    DUAL_HUD->updateCoin(collectedCoinCount); // test
                    showWarpAnimation();
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
                    if (stageIndex > 2 && suitcaseCount > 0) {
                        suitCaseType = 2 + rand()%2;
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
                    
                    if (isJewelVisible) {
                        EnemyBase* jewelryBox = EnemyBase::createEnemy(UNIT_JEWELRY_BOX, 1, "jewelryBox.png", 0, 0);
                        jewelryBox->secondTag = -1;
                        spriteBatch->addChild(jewelryBox);
                        coinArray.pushBack(jewelryBox);
                        GameManager::getInstance()->makeItSiluk(jewelryBox);
                        jewelryBox->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE, (totalHeight-j)*TILE_SIZE - TILE_SIZE/2 + 5));
                        jewelryBox->runAction(RepeatForever::create(Sequence::create(EaseOut::create(MoveBy::create(1, Point(0, 3)), 2), EaseOut::create(MoveBy::create(1, Point(0, -3)), 2), NULL)));
                        destructableArray.pushBack(jewelryBox);
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
                    theKnight->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE, (totalHeight-j-2)*TILE_SIZE - TILE_SIZE/2 + 5));
                }else if (gid == 57){
                    NPCBase* gate = NPCBase::create("activeZone0.png");
                    spriteBatch->addChild(gate);
                    npcArray.pushBack(gate);
                    gate->runAnimation("warp", true);
                    gate->setTag(UNIT_GATE);
                    gate->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE) + Point(TILE_SIZE/2, -TILE_SIZE/2 - 3));
                    
                    gate->secondTag = stageIndex + 1;
                }else if (gid == 58){
                    NPCBase* gate = NPCBase::create("activeZoneRed0.png");
                    spriteBatch->addChild(gate);
                    npcArray.pushBack(gate);
                    gate->runAnimation("warpBoss", true);
                    gate->setTag(UNIT_GATE);
                    gate->setPosition(tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j)*TILE_SIZE) + Point(TILE_SIZE/2, -TILE_SIZE/2 - 3));
                    gate->secondTag = stageIndex;
                    
                    sptRedFace = Droppable::createDroppable(0, 0, 0, "redFace0.png");
                    sptRedFace->runAnimation("redFace", true);
                    spriteBatch->addChild(sptRedFace);
                    sptRedFace->setPosition(gate->getPosition() + Point(0, 34));
                    sptRedFace->setOpacity(0);
                    sptRedFace->setScale(0.2f);
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
    
//    this->schedule(schedule_selector(DualWorld::gravityUpdate));
    this->schedule(schedule_selector(DualWorld::gravityUpdateForCoins));
    this->schedule(schedule_selector(DualWorld::gravityUpdateForStraight));
    this->schedule(schedule_selector(DualWorld::gravityUpdateForCustomMoving));
    this->schedule(schedule_selector(DualWorld::gravityUpdateForFlyingOrSwimingEnemies));
    this->schedule(schedule_selector(DualWorld::updateGame));
    
    if (talkCount > 0) {
        this->schedule(schedule_selector(DualWorld::talkUpdate), 0.1f);
    }

    this->schedule(schedule_selector(DualWorld::bubbleUpdate));
    this->schedule(schedule_selector(DualWorld::makeSomeBubbles), 0.1f);

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
            this->scheduleOnce(schedule_selector(DualWorld::sacrificeLater), 1);
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
            coin->runAction(Sequence::create(DelayTime::create(8), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(DualWorld::coinWaitDone, this)), NULL));
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
            particle->runAction(Sequence::create(FadeIn::create(dur*3/10), DelayTime::create(dur*4/10), FadeOut::create(dur*3/10), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
        }
    }
    
    GameManager::getInstance()->isVideoRewardEnergy = false;
    GameManager::getInstance()->isVideoRewardAttack = false;
    GameManager::getInstance()->isVideoRewardCoins = false;
    
    GameManager::getInstance()->canJumpForever = (skyBuddy != nullptr && skyBuddy->petNumber == PET_FALCON) || GameManager::getInstance()->developer;
    
    
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
        this->schedule(schedule_selector(DualWorld::fadeMiniMap));
        // lock  doors
        for (int i = 1; i < tileMap->getMapSize().height; i++) {    // left
            pos = Point(1, i);
            if(tileMap->getLayer("stage")->getTileGIDAt(pos) == 0){
                tileMap->getLayer("stage")->setTileGID(162, pos);
                addDirtToTile(tileMap, pos);
            }
        }
        for (int i = 1; i < tileMap->getMapSize().width; i++) {    // bottom
            pos = Point(i, 1);
            if(tileMap->getLayer("stage")->getTileGIDAt(pos) == 0){
                tileMap->getLayer("stage")->setTileGID(162, pos);
                addDirtToTile(tileMap, pos);
            }
        }
        for (int i = 1; i < tileMap->getMapSize().height; i++) {    // right
            pos = Point(tileMap->getMapSize().width - 2, i);
            if(tileMap->getLayer("stage")->getTileGIDAt(pos) == 0){
                tileMap->getLayer("stage")->setTileGID(162, pos);
                addDirtToTile(tileMap, pos);
            }
        }
        for (int i = 1; i < tileMap->getMapSize().width; i++) {    // top
            pos = Point(i, tileMap->getMapSize().height - 2);
            if(tileMap->getLayer("stage")->getTileGIDAt(pos) == 0){
                tileMap->getLayer("stage")->setTileGID(162, pos);
                addDirtToTile(tileMap, pos);
            }
        }
    }
    revealedMapArray.pushBack(tileMap);
    if (stageIndex >= 0) {
        GameManager::getInstance()->totalRoom++;
        GameManager::getInstance()->totalScore+=500;
//        DUAL_HUD->setScore(GameManager::getInstance()->totalScore);
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
        this->runAction(Sequence::create(DelayTime::create(dur), CallFunc::create(CC_CALLBACK_0(DualWorld::loadEnemies, this)), NULL));
    }
    
    isSetStageDone = true;
    CCLOG("setStage done");
}
void DualWorld::stopWoongSound(){
    readyToMove = true;
    GameManager::getInstance()->stopSoundEffect(SOUND_LASER_WOONG);
}
void DualWorld::addDirtToTile(TMXTiledMap* map, Point pos){
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

void DualWorld::fadeMiniMap(float dt){
    if (isMiniMapFadeIn) {
        miniMapAlpha += 5;
    }else{
        miniMapAlpha -= 5;
    }
    if (miniMapAlpha > 255) {
        miniMapAlpha = 255;
        this->unschedule(schedule_selector(DualWorld::fadeMiniMap));
    }else if(miniMapAlpha <= 0){
        miniMapAlpha = 0;
        this->unschedule(schedule_selector(DualWorld::fadeMiniMap));
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
void DualWorld::cloudSchedule(float dt){
    for (auto cloud : cloudArray) {
        if(cloud->getPosition().x < 0){
            cloud->setPosition(Point(cloud->getParent()->getContentSize().width + cloud->getContentSize().width/2, cloud->getPosition().y));
        }
    }
}
void DualWorld::saveCoinIfStarCollected(){
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
void DualWorld::sacrificeLater(float dt){
    Droppable* drop = groundBuddy;
    buddyArray.eraseObject(groundBuddy);
    droppableArray.eraseObject(groundBuddy);
    groundBuddy = NULL;
    drop->runAction(Sequence::create(EaseOut::create(MoveTo::create(0.5, player->getPosition() + Point(0, 50)), 3), MoveBy::create(0.05, Point(-2,1)), MoveBy::create(0.05, Point(2,-2)), MoveBy::create(0.05, Point(-2,1)), MoveBy::create(0.05, Point(2,1)), MoveBy::create(0.05, Point(-2,-2)),MoveBy::create(0.05, Point(-2,1)), MoveBy::create(0.05, Point(2,-2)), MoveBy::create(0.05, Point(-2,1)), MoveBy::create(0.05, Point(2,1)), MoveBy::create(0.05, Point(-2,-2)), CallFuncN::create(CC_CALLBACK_1(DualWorld::sacrificeDone, this)), NULL));
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
void DualWorld::sacrificeDone(Ref* obj){
    Droppable* drop = (Droppable*)obj;
    runEffect(EFFECT_TWINKLE, drop->getPosition());
    DUAL_HUD->addHeartAnimated((drop->getPosition()*4 + this->getPosition()));
//    drop->unscheduleAllSelectors();
    drop->stopAllActions();
    drop->removeFromParentAndCleanup(true);
    player->energy++;    player->maxEnergy++;
}

void DualWorld::addRide(int unit, Point pos){
    const char* sptName;
    if(unit == UNIT_TOP) sptName = "rideTop.png";
    else if(unit == UNIT_CART) sptName = "rideCart.png";
    else if(unit == UNIT_BROOM) sptName = "rideBroom.png";
    else if(unit == UNIT_BALLOON) sptName = "rideBalloon.png";
    else if(unit == UNIT_BIRD) sptName = "rideBird.png";
    Droppable* board = Droppable::createDroppable(unit, 1, 0, sptName);
    board->setPosition(pos);
    board->setTag(unit);
    
    
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
void DualWorld::splashDemage(Point pos, int radius, int demage){
    Rect mRect;
    EnemyBase* drop;
    
    shakeScreenOnce();
    
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
                    drop->energy -= demage;
                    drop->showEnergy();
                    
                    if (drop == theBoss) {
                        DUAL_HUD->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
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
void DualWorld::updateIndicators(){
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

void DualWorld::removeUsedAssets(){
    CCLOG("DualWorld removeUsedAssets");
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
    GameManager::getInstance()->setDualHudLayer(NULL);
    GameManager::getInstance()->setCurrentStageLayer(NULL);
}
void DualWorld::testSchedule(float dt){
    //    testPet->setPosition(player->getPosition() + Point(50, 0));
    //    testPet->setPosition(player->damageBoundingBox().origin);
    //    testPet->setAnchorPoint(Point::ZERO);
    //    testPet->setScale(player->damageBoundingBox().size.width, player->damageBoundingBox().size.height);
}
void DualWorld::useBomb(){
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
    effect->runAction(Sequence::create(ScaleTo::create(1, 15), CallFuncN::create(CC_CALLBACK_1(DualWorld::enemyBaseMoveDone, this)), NULL));
    effect->runAction(Sequence::create(DelayTime::create(0.5), FadeOut::create(0.4), NULL));
    
    this->schedule(schedule_selector(DualWorld::doTheBombToMissiles), 0.1, (int)bombTargetMissileArray.size(), 1);
}

void DualWorld::doTheBombToMissiles(float dt){
    CCLOG("doTheBombToMissiles");
    if (bombTargetMissileArray.size() <= 0) {
        this->unschedule(schedule_selector(DualWorld::doTheBombToMissiles));
        bombTargetEnemyIndex = 0;
        this->schedule(schedule_selector(DualWorld::doTheBombToEnemies), 0.1, (int)bombTargetEnemyArray.size(), 0.1);
        return;
    }
    
    Droppable* drop = bombTargetMissileArray.at(0);
    Sprite* sprite = Sprite::createWithSpriteFrameName("daryFace.png");
    sprite->runAction(Sequence::create(ScaleTo::create(0.5, 5), CallFuncN::create(CC_CALLBACK_1(DualWorld::enemyBaseMoveDone, this)), NULL));
    sprite->runAction(FadeOut::create(0.5));
    spriteBatch->addChild(sprite);
    switch (drop->getTag()) {
        case UNIT_MISSILE_CHASING:
            chasingMissileArray.eraseObject((Missile*)drop);
            break;
        case UNIT_MISSILE_DROPPABLE:
            droppableMissileArray.eraseObject((Missile*)drop);
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
    enemyMissileArray.eraseObject((Missile*)drop);
    spriteBatch->removeChild(drop, true);
    
    Point screenPos = this->getPosition();
    this->runAction(Sequence::create(MoveBy::create(0.05, Point(rand()%36 - 18, rand()%36 - 18)),
                                     MoveTo::create(0.05, screenPos),NULL));
}

void DualWorld::doTheBombToEnemies(float dt){
    float demage = 1;
    
    if (bombTargetEnemyArray.size() <= bombTargetEnemyIndex ) {
        this->unschedule(schedule_selector(DualWorld::doTheBombToEnemies));
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
                enemy->energy -= demage;
                
                if (enemy == theBoss) {
                    DUAL_HUD->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
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
    sprite->runAction(Sequence::create(ScaleTo::create(0.5, 5), CallFuncN::create(CC_CALLBACK_1(DualWorld::enemyBaseMoveDone, this)), NULL));
    sprite->runAction(FadeOut::create(0.5));
    spriteBatch->addChild(sprite);
    sprite->setPosition(enemy->getPosition());
    
    Point screenPos = this->getPosition();
    this->runAction(Sequence::create(MoveBy::create(0.05, Point(rand()%18 - 9, rand()%18 - 9)),
                                     MoveTo::create(0.05, screenPos),NULL));
    
    bombTargetEnemyIndex++;
}
void DualWorld::usePotion(){
    if(isGameOver)
        return;
    
    player->energy = player->maxEnergy;
    DUAL_HUD->setLife(player->maxEnergy);
    // heal effect
    CCLOG("Use Potion!");
}

bool DualWorld::keyExist(ValueMap map, const char* key){
    return map.find(key) != map.end();
}

void DualWorld::revive(){
    resumeLayer();
    player->setPosition(deadPoint);
    
    isGameOver = false;
    indestructableTime = -1;
    
    player->velocity = Point(0, 100);
    
    GameManager::getInstance()->isInMiddleOfGame = true;
    
    indestructableTime =  1.6f;
    this->schedule(schedule_selector(DualWorld::blinkingPlayer), 0.1f);
    
    DUAL_HUD->enableButtons(true);
    
    player->energy = player->maxEnergy;
    DUAL_HUD->setLife(player->maxEnergy);
    GameManager::getInstance()->playSoundEffect(SOUND_MEDICINE);
}

void DualWorld::enemyFireLoop(float dt)
{
    for(auto enemy: fireableArray)
    {
        while(enemy->missileArray.size() > 0){
            Missile* missile = enemy->missileArray.at(0);
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
                    missile->runAction(Sequence::create(DelayTime::create(2), CallFuncN::create(CC_CALLBACK_1(DualWorld::removeMeFromChasing, this)), NULL));
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
void DualWorld::removeMeFromChasing(Ref* ref){
    Droppable* drop = (Droppable*)ref;
    drop->setTag(UNIT_MISSILE_STRAIGHT);
    chasingMissileArray.eraseObject((Missile*)drop);
    straightMovingArray.pushBack(drop);
}

void DualWorld::bubbleUpdate(float dt)
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
            CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(DualWorld::enemyBaseMoveDone, this));
            bubble->runAction(Sequence::create(animate, call, NULL));
            return;
        }
    }
}

void DualWorld::makeSomeBubbles(float dt)
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

void DualWorld::setPlayerInfo()
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

void DualWorld::setWeaponInfo(int weapon){
//    player->setWeapon(weapon); // not need right? if not uncomment and fix this
    playerWeaponType = weapon;
    loadedBulletCountMax = getMaxLoadedBulletCount(weapon);
    playerFireCoolTimeMax = 0.5f;
    log("** max: %f, weapon: %d", playerFireCoolTimeMax, weapon);
    reloadingCoolTime = 1.0f;
    switch (weapon) {
        case WEAPON_PISTOL: // 20 p/s
            playerFireCoolTimeMax = 0.3f;
            log("** max: %f, weapon: %d", playerFireCoolTimeMax, weapon);
            break;
        case WEAPON_MAGNUM: // 25
            reloadingCoolTime = 1.2f;
            playerFireCoolTimeMax = 0.2f;
            break;
        case WEAPON_UZI: // 34
            playerFireCoolTimeMax = 0.1f;
            reloadingCoolTime = 1.5f;
            break;
        case WEAPON_RIFLE: // 25
            reloadingCoolTime = 1.5f;
            playerFireCoolTimeMax = 0.5f;
            break;
        case WEAPON_FIRE_BOTTLER: // 42
            playerFireCoolTimeMax = 0.7f;
            break;
        case WEAPON_ANTI_BAT: // 60
            reloadingCoolTime = 1.5f;
            playerFireCoolTimeMax = 0.2f;
            break;
        case WEAPON_SNIPER_RIFLE: // 36
            playerFireCoolTimeMax = 0.8f;
            break;
        case WEAPON_RIFLE_GRENADE: // 35
            reloadingCoolTime = 1.5f;
            playerFireCoolTimeMax = 0.6f;
            break;
        case WEAPON_CROSS_BOW: // 49
            reloadingCoolTime = 2;
            playerFireCoolTimeMax = 0.3f;
            break;
        case WEAPON_MACHINE_GUN: // 110
            reloadingCoolTime = 3;
            playerFireCoolTimeMax = 0.07f;
            break;
        case WEAPON_STRIKER: // 61
            reloadingCoolTime = 1.5f;
            playerFireCoolTimeMax = 0.3f;
            break;
        case WEAPON_NIFE_THROWER: // 88
            reloadingCoolTime = 1.5f;
            playerFireCoolTimeMax = 0.2f;
            break;
        case WEAPON_BALL_SHOOTER: // 111
            reloadingCoolTime = 2;
            playerFireCoolTimeMax = 0.15f;
            break;
        case WEAPON_FLAME_THROWER: // 207
            reloadingCoolTime = 2;
            playerFireCoolTimeMax = 0.05f;
            break;
        case WEAPON_ROCKET_LAUNCHER: // 132
            reloadingCoolTime = 1.5f;
            playerFireCoolTimeMax = 1.0f;
            break;
        case WEAPON_BAZOOKA: // 44
            playerFireCoolTimeMax = 0.8f;
            break;
        case WEAPON_MATT_A4: // 164
            playerFireCoolTimeMax = 0.3f;
            break;
        case WEAPON_LASER_GUN: // 161
            reloadingCoolTime = 2;
            playerFireCoolTimeMax = 0.01f;
            break;
        case WEAPON_ENERGY_GUN: // 85
            reloadingCoolTime = 1.5f;
            playerFireCoolTimeMax = 0.4f;
            break;
        case WEAPON_GUIDED_ROCKET_LAUNCHER: // 107
            reloadingCoolTime = 3;
            playerFireCoolTimeMax = 0.7f;
            break;
        case WEAPON_SLUGGER: // 171
            reloadingCoolTime = 4;
            playerFireCoolTimeMax = 0.3f;
            break;
        case WEAPON_DRAGON_RIFLE: // 282
            reloadingCoolTime = 2.5f;
            playerFireCoolTimeMax = 0.3f;
            break;
        case WEAPON_SLUGGER_NG: // 171
            reloadingCoolTime = 3;
            playerFireCoolTimeMax = 0.3f;
            break;
        case WEAPON_LIGHTNING_GUN: // 171
            reloadingCoolTime = 3;
            playerFireCoolTimeMax = 0.2f;
            break;
        case WEAPON_SHARK_GUN: // 171
            reloadingCoolTime = 4;
            playerFireCoolTimeMax = 1.0f;
            break;
        case WEAPON_LASER_COIN: // 171
            reloadingCoolTime = 2;
            playerFireCoolTimeMax = 0.01f;
            break;
        default:
            break;
    }
    log("** max: %f, weapon: %d", playerFireCoolTimeMax, weapon);
}
void DualWorld::talkUpdate(float dt)
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
                //                DUAL_HUD->enableLayerAndMenus(false);
            }
        }
    }
    
    if (talkEncountered) {
        DUAL_HUD->showTalk(talkLabelArray.at(currentTalkIndex)->getString().c_str());
        //        CCLOG("encountered: %s", talkLabelArray.at(currentTalkIndex)->getString().c_str());
    }
    if (shouldReset) {
        currentTalkIndex = -1;
    }
}

void DualWorld::gravityUpdate(float dt)
{
    if (isGameOver) {
        if (DUAL_HUD->cursorLayer->isMouseLeftDown) {
            DUAL_HUD->tryResultOK();
        }
        return;
    }
    if (isPaused) {
        return;
    }
    
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
    
    TMXTiledMap* map = getTileMap(player->getPosition());
    if (map->getTag() < MAP_FOUND) {
        Point plPos = this->tileCoordForPosition(player->getPosition() - map->getPosition(), map); //1
        if (plPos.x >  1 && plPos.y > 1 && plPos.x < map->getMapSize().width - 2 && plPos.y < map->getMapSize().height - 2) {
            roomNumber++;
            srand(GameManager::getInstance()->getTodaysRandom() + roomNumber);
            if (map->getTag() == MAP_NORMAL || map->getTag() == MAP_BLUE_KEY) {
                addEnemiesToMap(map, currentLevelScore, map->getTag() == MAP_BLUE_KEY);
            }
            
            setStage(map);
            currentLevelScore += 20;
            
            updateMiniMap();
        }
    }
    
    //    CCLOG("%d, %d, %d", (int)spriteBatch->getChildren().size(), (int)this->getChildren().size(), (int)tileMap->getChildren().size());
    //    CCLOG("left: %d, right: %d", GameManager::getInstance()->leftPressed, GameManager::getInstance()->rightPressed);
    bool left = GameManager::getInstance()->leftPressed;
    bool right = GameManager::getInstance()->rightPressed;
    bool jump = GameManager::getInstance()->jumpPressed || GameManager::getInstance()->upPressed;
    //GameManager::getInstance()->jumpPressed = false;
    if (player->vehicleType == VEHICLE_TOP ||
        player->vehicleType == VEHICLE_BROOM) {
        
    }else{
        if(player->jumped) GameManager::getInstance()->jumpPressed = false;
    }
    if (!isReloading) {
        player->setGunPos(-GameManager::getInstance()->getAngle(DUAL_HUD->cursorLayer->cursor->getPosition(), center));
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
//        this->setViewPointCenter(player->getPosition() + extraCameraPos);
    }
    heroLight->setPosition(player->getPosition());
    
    testingFlag = !testingFlag;
    
    // fire
    if (isReloading) {
        reloadingCoolTimeSpan -= dt;
        
        if (reloadingCoolTimeSpan <= 0) {
            reloadingCoolTimeSpan = reloadingCoolTime;
            isReloading = false;
            int maxLoadedCount = getMaxLoadedBulletCount(player->weapon);
            //            int totalBulletCount = getTotalBulletCountAtSlot(currentSlot);
            int bulletCountToLoad = maxLoadedCount;
            if (maxLoadedCount > totalBulletCount && getMaxTotalBulletCount(player->weapon) > 0) {
                bulletCountToLoad = totalBulletCount;// - loadedBulletCount;
            }
            DUAL_HUD->setBulletCount(bulletCountToLoad, getMaxLoadedBulletCount(player->weapon));
            player->gun->setAnchorPoint(Point(0, 0.5));
            //player->lastAngle = 0;
            player->setGunPos(-GameManager::getInstance()->getAngle(DUAL_HUD->cursorLayer->cursor->getPosition(), center));
            loadedBulletCount = bulletCountToLoad;
            log("loaded");
        }
    }
    
    
    GameManager::getInstance()->firePressed = DUAL_HUD->cursorLayer->isMouseLeftDown;
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
    }else if(DUAL_HUD->cursorLayer->isMouseRightDown){
        if (!isReloading) {
            reload();
        }
    }else{
//        playerFireCoolTime = 0;
        isFired = false;
//        removeLaser();
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
    if (stickTimeLeft < 0) {
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
void DualWorld::coinMagnet(){
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
Sprite* DualWorld::getTeleport(int teleport){
    for(auto tele: teleportArray){
        if(tele->getTag() == teleport)
            return tele;
    }
    return NULL;
}
void DualWorld::gravityUpdateForFlyingOrSwimingEnemies(float dt)
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

void DualWorld::gravityUpdateForCoins(float dt)
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
void DualWorld::gravityUpdateForStraight(float dt)
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
            if(checkForAndResolveCollisionsForMissile((Missile*)drop)){
                allClear = false;
                break;
            }
        }
    }while(!allClear);
    
}
void DualWorld::gravityUpdateForCustomMoving(float dt)
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
            if(checkForAndResolveCollisionsForMissile((Missile*)drop)){
                allClear = false;
                break;
            }
        }
    }while(!allClear);
}
void DualWorld::chasingMissileUpdate(float dt)
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
        
        checkForAndResolveCollisionsForMissile((Missile*)drop);
    }
}
void DualWorld::setViewPointCenter(Point position)
{
    float scale = getScale();
    int x = position.x*scale;
    int y = position.y*scale;
    
//    dnMiniMap->setPosition(Point(currentMapSize.width/2 - position.x*miniMapScale, currentMapSize.height/2 - position.y*miniMapScale));
    
    Point actualPosition = Point(x, y);
    
    Point centerOfView = Point(size.width/2, size.height/2);
    Point viewPoint = centerOfView - actualPosition;
//    float distance = DUAL_HUD->cursorLayer->cursor->getPosition().getDistance(center);
    float maxDistance = 300;
//    if (distance > maxDistance) {
//        distance = maxDistance;
//    }
//    float angle = GameManager::getInstance()->getAngle(DUAL_HUD->cursorLayer->cursor->getPosition(), center);
//    Point aimingPos = Point(cos(angle*3.14/180)*distance, -sin(-angle*3.14/180)*distance);
    this->setPosition(viewPoint);
}
void DualWorld::showBigMiniMap(bool isBig){
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
    dnHero->setPosition(scrollView->getContentSize()/2);
    dnHero->setPosition(dnHero->getPosition() + Point(-TILE_SIZE*0.3f*0.5f, -TILE_SIZE*0.3f*0.5f));
    
    scrollView->setInnerContainerSize(scrollView->getContentSize());
}
void DualWorld::setCurrentTileMap(Point pos){
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
TMXTiledMap* DualWorld::getTileMap(Point pos){
    return theMap;
    for(auto map: mapArray){
        if (map->getBoundingBox().containsPoint(pos)) {
            return map;
        }
    }
    return nullptr;
}

Point DualWorld::tileCoordForPosition(Point position, TMXTiledMap* map)
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

Rect DualWorld::tileRectFromTileCoords(Point tileCoords, TMXTiledMap* map)
{
    return Rect(tileCoords.x*TILE_SIZE + map->getPositionX(), (map->getMapSize().height - tileCoords.y - 1)*TILE_SIZE + map->getPositionY(), TILE_SIZE, TILE_SIZE);
    //    float levelHeightInPixels = tileMap->getMapSize().height * tileMap->getTileSize().height;
    //    Point origin = Point(tileCoords.x * tileMap->getTileSize().width, levelHeightInPixels - ((tileCoords.y + 1) * tileMap->getTileSize().height));
    //    return Rect(origin.x, origin.y, tileMap->getTileSize().width, tileMap->getTileSize().height);
}

/*Vector<TileInfo*> DualWorld::getSurroundingTilesAtPosition(Point position, cocos2d::TMXLayer *layer, Droppable* p)
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
int DualWorld::getTileAtPosition(Point position, int tag, TMXTiledMap* map)
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

/*Vector<TileInfo*> DualWorld::getAllSurroundingTilesAtPosition(Point position, cocos2d::TMXLayer *layer)
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
void DualWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    exit(0);
#endif
}

void DualWorld::registerScriptAccelerateHandler(int handler)
{
    //    Director::getInstance()->getEventDispatcher()->addTargetedDelegate(this, 0, true);
}


void DualWorld::setPlayerPosition(Point position) {
    player->setPosition(position);
}
void DualWorld::Dispose(){
    
}
void DualWorld::addListener(){
    /*listener = cocos2d::EventListenerTouchOneByOne::create();
     listener->setSwallowTouches(false);
     
     listener->onTouchBegan = CC_CALLBACK_2(DualWorld::onTouchBegan, this);
     listener->onTouchMoved = CC_CALLBACK_2(DualWorld::onTouchMoved, this);
     listener->onTouchEnded = CC_CALLBACK_2(DualWorld::onTouchEnded, this);
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

bool DualWorld::onTouchBegan(Touch *touch, Event *unused_event){
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
     if(DUAL_HUD->btnLeftRight->getBoundingBox().containsPoint(location) ||
     DUAL_HUD->btnA->getBoundingBox().containsPoint(location) ||
     DUAL_HUD->btnB->getBoundingBox().containsPoint(location)){
     return true;
     }*/
    
    return true;
}

void DualWorld::onTouchMoved(Touch *touch, Event *unused_event){
    
}
void DualWorld::onTouchEnded(Touch *touch, Event *unused_event){
    
}
void DualWorld::TouchesBegan(const std::vector<Touch*>& touches, Event *unused_event)
{
    Touch *touch = touches.at(0);
    Point location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);
    
    touchStart = location;
    this->unschedule(schedule_selector(DualWorld::resetTouchStart));
    this->scheduleOnce(schedule_selector(DualWorld::resetTouchStart), 0.5);
    
}
void DualWorld::resetTouchStart(float dt){
    touchStart = Point(0, size.height);
}
void DualWorld::TouchesMoved(const std::vector<Touch*>& touches, Event *unused_event)
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

void DualWorld::getOffVehicle(){
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
        spt->runAction(Sequence::create(ScaleTo::create(duration, 0.1), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
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

void DualWorld::TouchesEnded(const std::vector<Touch*>& touches, Event *unused_event)
{
    Touch *touch = (Touch*)(touches.at(0));
    Point location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);

}

//void DualWorld::TouchesCancelled(const std::vector<Touch*>& touches, Event *unused_event)
//{
//    Touch *touch = (Touch*)(touches.at(0));
//    Point location = touch->getLocationInView();
//    location = Director::getInstance()->convertToGL(location);

//}

void DualWorld::solveCollision(Droppable* p, Point pos, Rect rect){
    
}

float DualWorld::checkBottom(Droppable* p){
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
void DualWorld::checkForAndResolveCollisions(Droppable* p){
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
                                part->runAction(Sequence::create(JumpBy::create(dur, Point(-xDst, 0), yDst, 1), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
                                ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->removeTileAt(plPos);
                                part->runAction(RotateBy::create(dur, -angle));
                                
                                part = Sprite::createWithSpriteFrameName("fragilePart0.png");
                                spriteBatch->addChild(part);
                                part->setPosition(pos);
                                part->runAction(Sequence::create(JumpBy::create(dur, Point(-xDst, yDst), yDst*2, 1), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
                                ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->removeTileAt(plPos);
                                part->runAction(RotateBy::create(dur, -angle));
                                
                                part = Sprite::createWithSpriteFrameName("fragilePart3.png");
                                spriteBatch->addChild(part);
                                part->setPosition(pos);
                                part->runAction(Sequence::create(JumpBy::create(dur, Point(xDst, 0), yDst, 1), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
                                ((TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->removeTileAt(plPos);
                                part->runAction(RotateBy::create(dur, angle));
                                
                                part = Sprite::createWithSpriteFrameName("fragilePart1.png");
                                spriteBatch->addChild(part);
                                part->setPosition(pos);
                                part->runAction(Sequence::create(JumpBy::create(dur, Point(xDst, yDst), yDst*2, 1), CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
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
                                coin->runAction(Sequence::create(DelayTime::create(10), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(DualWorld::coinWaitDone, this)), NULL));
                                
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
                platform->player= player;
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
void DualWorld::startFallingDestructablePlatform(DestructablePlatform* platform){
    platform->isFallingStarted = true;
    platform->runAction(Sequence::create(MoveBy::create(platform->fallingStartTime/7, Point(2,2)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(-2,0)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(-2,-4)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(2,4)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(0,-2)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(-2,-2)),
                                         MoveBy::create(platform->fallingStartTime/7, Point(2,2)),
                                         
                                         CallFuncN::create(CC_CALLBACK_1(DualWorld::destructablePlatformShakingDone, this)),
                                         NULL));
}
void DualWorld::destructablePlatformShakingDone(Ref* obj){
    DestructablePlatform* platform = (DestructablePlatform*)obj;
    platform->layer->removeTileAt(platform->tilePos);
    platform->runAction(FadeOut::create(0.5));
    platform->runAction(Sequence::create(MoveBy::create(0.5, Point(0, - 50)),
                                         DelayTime::create(5.0f),
                                         CallFuncN::create(CC_CALLBACK_1(DualWorld::destructablePlatformWaitingDone, this)),
                                         NULL));
    
}
void DualWorld::destructablePlatformWaitingDone(Ref* obj){
    DestructablePlatform* platform = (DestructablePlatform*)obj;
    platform->isFallingStarted = false;
    platform->layer->setTileGID(51, platform->tilePos);
    platform->runAction(FadeIn::create(0.5));
    platform->runAction(MoveTo::create(0.5, platform->originalPos));
}


bool DualWorld::isFregile(int index){
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
bool DualWorld::isBlock(int index){
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
        case 162:
            return true;
        default:
            return false;
    }
}
bool DualWorld::isOneWay(int index){
    switch (index) {
        case 130:
        case 133:
        case 134:
        case 135:
        case 164:
        case 165:
        case 196:
        case 197:
        case 258:
        case 260:
        case 261:
        case 262:
            return true;
            
        default:
            return false;
    }
}
bool DualWorld::checkDroppableMissileCollision(Droppable* p){
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
    TMXTiledMap* map = theMap;//getTileMap(p->getPosition());
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
                
                droppableMissileArray.eraseObject((Missile*)p);
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
            droppableMissileArray.eraseObject((Missile*)p);
            heroMissileArray.eraseObject((Missile*)p);
            enemyMissileArray.eraseObject((Missile*)p);
            enemyArray.eraseObject((EnemyBase*)p);
            p->removeFromParent();
            return true;
        }
    }
    return false;
}

bool DualWorld::intersectsRect(cocos2d::Rect srcRect, cocos2d::Rect dstRect)
{
    return !(srcRect.getMaxX() <= dstRect.getMinX() ||
             dstRect.getMaxX() <= srcRect.getMinX() ||
             srcRect.getMaxY() <= dstRect.getMinY() ||
             dstRect.getMaxY() <= srcRect.getMinY());
}
Rect DualWorld::intersection(Rect source, Rect rect)
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
void DualWorld::checkForAndResolveCollisionsForWater(Droppable* p){
    
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

void DualWorld::checkForAndResolveCollisionsForBouncing(Droppable* p){
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

bool DualWorld::checkForAndResolveCollisionsForMissile(Missile* p){
    p->setPosition( p->desiredPosition); //7
    
    TMXTiledMap* map = theMap;//getTileMap(p->getPosition());
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
                Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "bombSmallEffect0.png");
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
                enemyMissileArray.pushBack(missile);
            }
            if (p->secondTag == UNIT_MACHINEGUN_MINI) {
                double angle = -GameManager::getInstance()->getAngle(p->getPosition(), player->getPosition() + Point(0, TILE_SIZE/2)) + 180;
                
                Point gunTipPos = p->getPosition();
                Missile* missile = Missile::createMissile(UNIT_MISSILE_STRAIGHT, 1, 0, 0, "bombSmallEffect0.png");
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
                enemyMissileArray.pushBack(missile);
            }
            
            if(p->secondTag == WEAPON_FIRE_BOTTLER ||
               p->secondTag == WEAPON_BAZOOKA){
                splashDemage(p->getPosition(), 50, p->energy);
            }
        }
        
        bulletWasted = true;
        
        if(heroMissileArray.find(p) != heroMissileArray.end()){
            heroMissileArray.eraseObject(p);
            if(p->secondTag == WEAPON_ENERGY_GUN){
                ((Missile*)p)->victimArray.clear();
            }
        }
        if (enemyMissileArray.find(p) != enemyMissileArray.end()) {
            enemyMissileArray.eraseObject(p);
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
void DualWorld::checkForDoor()
{
    
}
void DualWorld::unscheduleAll(){
    this->unscheduleAllCallbacks();
}

void DualWorld::showSuccessLayer(float dt)
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
    
    DUAL_HUD->setVisible(true);
    activityIndicator = Sprite::create("sandglass.png");
    DUAL_HUD->addChild(activityIndicator);
    activityIndicator->setPosition(Point(size.width/2, size.height/2));
    activityIndicator->runAction(RepeatForever::create(RotateBy::create(1, 360)));
    GameManager::getInstance()->saveCoin();
    if (starCount >= 3) {
        showStageClearLayer(true);
    }else{
        showStageClearLayer(false);
    }
}

void DualWorld::showStageClearLayer(bool threeStars){
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
    DUAL_HUD->showStageClearLayer(collectedCoinCount, enemyKilledCount, enemyMaxCount);
}
void DualWorld::rateResult(Node* node){
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
void DualWorld::gameClearAnimationDone(Ref* obj)
{
    //    CCLOG("Show stage clear popup");
}

Droppable* DualWorld::findTarget(int distance){
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
int DualWorld::getMaxLoadedBulletCount(int weapon){
    switch (weapon) {
        case WEAPON_PISTOL: // 20 p/s
            return 8;
        case WEAPON_MAGNUM: // 25
            return 6;
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
int DualWorld::getMaxTotalBulletCount(int weapon){
    switch (weapon) {
        case WEAPON_PISTOL: // 20 p/s
            return -1;
        case WEAPON_MAGNUM: // 25
            return 24;
        case WEAPON_UZI: // 34
            return 100;
        case WEAPON_RIFLE: // 25
            return 25;
        case WEAPON_FIRE_BOTTLER: // 42
            return 30;
        case WEAPON_ANTI_BAT: // 60
            return 80;
        case WEAPON_SNIPER_RIFLE: // 36
            return 10;
        case WEAPON_RIFLE_GRENADE: // 35
            return 25;
        case WEAPON_CROSS_BOW: // 49
            return 40;
        case WEAPON_MACHINE_GUN: // 110
            return 150;
        case WEAPON_STRIKER: // 61
            return 20;
        case WEAPON_NIFE_THROWER: // 88
            return 50;
        case WEAPON_BALL_SHOOTER: // 111
            return 75;
        case WEAPON_FLAME_THROWER: // 207
            return 1000;
        case WEAPON_ROCKET_LAUNCHER: // 132
            return 10;
        case WEAPON_BAZOOKA: // 44
            return 5;
        case WEAPON_MATT_A4: // 164
            return 35;
        case WEAPON_LASER_GUN: // 161
            return 150;
        case WEAPON_ENERGY_GUN: // 85
            return 20;
        case WEAPON_GUIDED_ROCKET_LAUNCHER: // 107
            return 15;
        case WEAPON_SLUGGER: // 171
            return 50;
        case WEAPON_DRAGON_RIFLE: // 282
            return 45;
        case WEAPON_SLUGGER_NG: // 171
            return 45;
        case WEAPON_LIGHTNING_GUN:
            return 25;
        case WEAPON_SHARK_GUN:
            return 12;
        case WEAPON_LASER_COIN:
            return 150;
        default:
            return 40;
    }
}
void DualWorld::playReloadingSoundLater(float dt){
    GameManager::getInstance()->playSoundEffect(SOUND_RELOAD);
}
//int testWeaponIndex = WEAPON_LASER_GUN - 1;
void DualWorld::reload(){
    //    testWeaponIndex++;
    //    if (testWeaponIndex == WEAPON_NO_MORE) {
    //        testWeaponIndex = WEAPON_PISTOL;
    //    }
    //    addWeapon(testWeaponIndex);
    //    return;
//    if (laser!=nullptr) {
//        laser->removeFromParent();
//        laser = nullptr;
//        GameManager::getInstance()->stopSoundEffect(SOUND_LASER_BUZZ);
//    }
    
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
        this->scheduleOnce(schedule_selector(DualWorld::playReloadingSoundLater), reloadingTime - 1);
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
    player->face->setRotation(player->face->isFlippedY()?130:40);
    
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
void DualWorld::fire(float dt)
{
    if (DUAL_HUD->dialogBox) {
        return;
    }
    
    if (!readyToMove || !player->gun->isVisible() || isReloading || indexToWarp != STAGE_NONE) {
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
    
//    shakeScreen(1);
    
    loadedBulletCount--;
    DUAL_HUD->reduceBulletCount();
    if (totalBulletCountMax > 0) {
        totalBulletCount--;
        DUAL_HUD->setTotalBulletCount(totalBulletCount, totalBulletCountMax);
    }
    
    isFired = true;
    int angle = player->lastAngle;
    
    float gunWidth = player->gun->getContentSize().width;
    float x = cos(angle*3.14/180)*gunWidth;
    float y = -sin(angle*3.14/180)*gunWidth;
    Point gunLengthPos = Point(x, y);
    if (player->weapon == WEAPON_LASER_GUN ||
        player->weapon == WEAPON_LASER_COIN) {
//        if (laser == nullptr) {
//            if (player->weapon == WEAPON_LASER_GUN) {
//                GameManager::getInstance()->playSoundEffect(SOUND_LASER_BUZZ);
//                laser = Laser::create("laserThinBlue.png", "laserThinBlueGathering.png", "laserThinBlueHit");
//            }else{
//                GameManager::getInstance()->playSoundEffect(SOUND_LASER_WOONG);
//                laser = Laser::create("bigLaserLine.png", "bigLaserCircle.png", "");
//                laser->setScaleY(0.4);
//                laser->sptBeam->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
//                laser->sptHit->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
//                laser->sptHit->addChild(getLight());
//                laser->sptBeam->addChild(getLight());
//                laser->sptHit->setScale(2, 1.6);
//            }
//            for(auto en:enemyArray){
//                laser->enemyArray.pushBack(en);
//            }
//            laser->rectArray = stageTileRectArray;
//            this->addChild(laser);
//        }
        
//        laser->setLaser(player->getPosition() + player->gun->getPosition() - player->center + gunLengthPos, player->lastAngle);
//        if (laser->hitEnemy != nullptr)
//        {
//            int demage = GameManager::getInstance()->getWeaponPower(player->weapon);
//            laser->hitEnemy->energy -= demage;
//            
////            laser->hitEnemy->showEnergy();
//            
//            showDamage(demage, laser->hitEnemy->getPosition());
//            if (laser->hitEnemy->energy <= 0) {
////                enemiesToRemove.pushBack(laser->hitEnemy);
//                for(auto en:enemyArray){
//                    laser->enemyArray.pushBack(en);
//                }
//                laser->enemyArray.eraseObject(laser->hitEnemy);
//                
//                this->runEffect(EFFECT_EXPLODE_BIG, laser->hitEnemy->getPosition());
////                this->createRewards(laser->hitEnemy);
////                if (laser->hitEnemy->rewards > 0) {
////                    enemyKilledCount++;
////                }
//            }else
//            {
//                laser->hitEnemy->blinkForAWhile();
//            }
//        }
        if (loadedBulletCount <= 0) {
//            removeLaser();
        }
        return;
    }else if (player->weapon == WEAPON_LIGHTNING_GUN) {
        Point startPos = player->getPosition() + player->gun->getPosition() - player->center + gunLengthPos;
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
                DUAL_HUD->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
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
    Droppable* ms;
    int extraY = -9;
    Point msPos = Point(player->getPosition().x + (player->isTowardLeft?2:14), player->getPosition().y+ extraY + 8 );
    if (player->weapon == WEAPON_PISTOL) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_PISTOL, GameManager::getInstance()->getWeaponPower(WEAPON_PISTOL));
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_UZI) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_UZI, GameManager::getInstance()->getWeaponPower(WEAPON_UZI));
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_MAGNUM) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_MAGNUM, GameManager::getInstance()->getWeaponPower(WEAPON_MAGNUM));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_RIFLE) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_RIFLE, GameManager::getInstance()->getWeaponPower(WEAPON_RIFLE));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*14, player->getPosition().y+ extraY + 8);
        straightMovingArray.pushBack(ms);
        ms->extraSpeed = 150;
    }else if (player->weapon == WEAPON_BALL_SHOOTER) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_BALL_SHOOTER, GameManager::getInstance()->getWeaponPower(WEAPON_BALL_SHOOTER));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*14, player->getPosition().y+ extraY + 8);
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_FLAME_THROWER) {
        GameManager::getInstance()->playSoundEffect(SOUND_NOISE_37);
        ms = GameManager::getInstance()->getBullet(WEAPON_FLAME_THROWER, GameManager::getInstance()->getWeaponPower(WEAPON_FLAME_THROWER));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*14, player->getPosition().y+ extraY + 6);
        customMovingArray.pushBack(ms);
        float duration = 0.5f;
        ms->setScale(1.5, 0.2);
        ms->setAnchorPoint(Point(0.5, 1));
        ms->setTag(UNIT_MISSILE_CUSTOM);
        ms->runAction(MoveBy::create(duration, gunLengthPos*4));
        ms->runAction(EaseIn::create(MoveBy::create(duration, Point(0, 20)), 5));
        ms->runAction(EaseIn::create(ScaleTo::create(duration, 1, 1.5), 2));
        ms->runAction(Sequence::create(DelayTime::create(duration), CallFuncN::create(CC_CALLBACK_1(DualWorld::missileMoveDone, this)), NULL));
    }else if (player->weapon == WEAPON_NIFE_THROWER) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_NIFE_THROWER, GameManager::getInstance()->getWeaponPower(WEAPON_NIFE_THROWER));
        ms->runAction(RepeatForever::create(RotateBy::create(0.3, 360)));
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_ANTI_BAT) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_PISTOL, GameManager::getInstance()->getWeaponPower(WEAPON_ANTI_BAT));
        ms->setPosition(player->getPosition() + player->gun->getPosition() - player->center);
        straightMovingArray.pushBack(ms);
        if (angle < -90 || angle > 90) {
            ms->movingAngle = -angle - 45;
        }else{
            ms->movingAngle = -angle + 45;
        }
        spriteBatch->addChild(ms);
        heroMissileArray.pushBack((Missile*)ms);
        ms->setRotation(-ms->movingAngle);
        
        ms = GameManager::getInstance()->getBullet(WEAPON_PISTOL, 20);
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_CROSS_BOW) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_CROSS_BOW, GameManager::getInstance()->getWeaponPower(WEAPON_CROSS_BOW));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_MACHINE_GUN) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_MACHINE_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_MACHINE_GUN));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 9);
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_LASER_GUN) {
//        GameManager::getInstance()->playSoundEffect(SOUND_FA);
//        ms = GameManager::getInstance()->getBullet(WEAPON_LASER_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_LASER_GUN));
//        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 7);
//        straightMovingArray.pushBack(ms);
//        ms->extraSpeed = 250;
    }else if (player->weapon == WEAPON_FIRE_BOTTLER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_FIRE_BOTTLER, GameManager::getInstance()->getWeaponPower(WEAPON_FIRE_BOTTLER));
        ms->setPosition(Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 2));
        droppableMissileArray.pushBack((Missile*)ms);
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
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_SNIPER_RIFLE) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_SNIPER_RIFLE, GameManager::getInstance()->getWeaponPower(WEAPON_SNIPER_RIFLE));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*16, player->getPosition().y + extraY+ 8);
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_ROCKET_LAUNCHER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_ROCKET_LAUNCHER, GameManager::getInstance()->getWeaponPower(WEAPON_ROCKET_LAUNCHER));
        ms->effectType = MISSILE_EFFECT_SMOKE;
        ms->secondTag = WEAPON_ROCKET_LAUNCHER;
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y + extraY+ 9);
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_STRIKER) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_STRIKER, GameManager::getInstance()->getWeaponPower(WEAPON_STRIKER));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*14, player->getPosition().y + extraY+ 8);
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_ENERGY_GUN) {
        GameManager::getInstance()->playSoundEffect(SOUND_LASER);
        ms = GameManager::getInstance()->getBullet(WEAPON_ENERGY_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_ENERGY_GUN));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*13, player->getPosition().y + extraY+ 8);
        straightMovingArray.pushBack(ms);
        ms->secondTag = WEAPON_ENERGY_GUN;
        ms->setScale(2);
        GameManager::getInstance()->makeItSiluk(ms);
    }else if (player->weapon == WEAPON_BAZOOKA) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_BAZOOKA, GameManager::getInstance()->getWeaponPower(WEAPON_BAZOOKA));
        ms->secondTag = WEAPON_BAZOOKA;
        ms->effectType = MISSILE_EFFECT_RED_SMOKE;
        ms->secondTag = WEAPON_BAZOOKA;
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 6);
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_DRAGON_RIFLE) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_DRAGON_RIFLE, GameManager::getInstance()->getWeaponPower(WEAPON_DRAGON_RIFLE));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 6);
        ms->effectType = MISSILE_EFFECT_RED_BALL;
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_GUIDED_ROCKET_LAUNCHER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_GUIDED_ROCKET_LAUNCHER, GameManager::getInstance()->getWeaponPower(WEAPON_GUIDED_ROCKET_LAUNCHER));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*14, player->getPosition().y+ extraY + 6);
        ms->target = findTarget(500);
        ms->setRotation(player->isTowardLeft?180:0);
        ms->setTag(UNIT_MISSILE_CHASING);
        ms->effectType = MISSILE_EFFECT_SMOKE;
        ms->secondTag = WEAPON_GUIDED_ROCKET_LAUNCHER;
        chasingMissileArray.pushBack((Missile*)ms);
    }else if (player->weapon == WEAPON_MATT_A4) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_MATT_A4, GameManager::getInstance()->getWeaponPower(WEAPON_MATT_A4));
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*10, player->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_SLUGGER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*13, player->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_SLUGGER_NG) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->setPosition(player->getPosition() + player->gun->getPosition() - player->center + gunLengthPos);
        straightMovingArray.pushBack(ms);
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        
        ms->movingAngle =  -angle + 45;
        spriteBatch->addChild(ms);
        heroMissileArray.pushBack((Missile*)ms);
        
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->setPosition(player->getPosition() + player->gun->getPosition() - player->center + gunLengthPos);
        straightMovingArray.pushBack(ms);
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        
        ms->movingAngle =  -angle - 45;
        spriteBatch->addChild(ms);
        heroMissileArray.pushBack((Missile*)ms);
        
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        
        straightMovingArray.pushBack(ms);
    }else if (player->weapon == WEAPON_SHARK_GUN) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_SHARK_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_SHARK_GUN));
        ms->effectType = MISSILE_EFFECT_SHARK_GUN;
        msPos = Point(player->getPosition().x + (player->isTowardLeft?-1:1)*13, player->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack(ms);
        
    }
    
    ms->setPosition(player->getPosition() + player->gun->getPosition() - player->center + gunLengthPos);
    
    if (player->weapon != WEAPON_FLAME_THROWER) {
        Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(ms->getPosition());
        explode->setAnchorPoint(Point(1, 0.5));
        Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
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
        tanpi->runAction(Sequence::create(DelayTime::create(3), CallFuncN::create(CC_CALLBACK_1(DualWorld::coinWaitDone, this)), NULL));
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
    if (isVideoRewardAttack) {
        ms->energy = ms->energy*1.2;
    }
    if (GameManager::getInstance()->developer) {
        //        ms->energy = 2000;
    }
    spriteBatch->addChild(ms);
    heroMissileArray.pushBack((Missile*)ms);
    
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
     CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(DualWorld::enemyBaseMoveDone));
     fireFlash->runAction(Sequence::create(delay, call, NULL));
     
     Sprite* fireFlashLight = Sprite::createWithSpriteFrameName("fireLight.png");
     fireFlashLight->setPosition(Point(point.x + (player->isTowardLeft?-5:5), point.y));
     fireFlashLight->setFlippedX(player->isTowardLeft);
     spriteBatch->addChild(fireFlashLight, 10);
     DelayTime* delayForLight = DelayTime::create(0.07f);
     CallFuncN* callForLight = CallFuncN::create(CC_CALLBACK_1(DualWorld::enemyBaseMoveDone));
     fireFlashLight->runAction(Sequence::create(delayForLight, callForLight, NULL));
     */
}

void DualWorld::fire(Weapon* weapon, float dt)
{
//    shakeScreen(1);
    
    weapon->shooting(dt);
    if (!weapon->shouldFire) {
        return;
    }
    weapon->onFire();
    Hero* owner;
    if (weapon->isEnemy) {
        owner = enemy;
    }else{
        owner = player;
    }
    
    int angle = weapon->isEnemy?180:0;
    
    float gunWidth = weapon->getContentSize().width;
    float x = cos(angle*3.14/180)*gunWidth;
    float y = -sin(angle*3.14/180)*gunWidth;
    Point gunLengthPos = Point(x, y);
    /*if (weapon->value%1000 == WEAPON_LASER_GUN ||
        weapon->value%1000 == WEAPON_LASER_COIN) {
        if (laser == nullptr) {
            if (weapon->value%1000 == WEAPON_LASER_GUN) {
                GameManager::getInstance()->playSoundEffect(SOUND_LASER_BUZZ);
                laser = Laser::create("laserThinBlue.png", "laserThinBlueGathering.png", "laserThinBlueHit");
            }else{
                GameManager::getInstance()->playSoundEffect(SOUND_LASER_WOONG);
                laser = Laser::create("bigLaserLine.png", "bigLaserCircle.png", "");
                laser->setScaleY(0.4);
                laser->sptBeam->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
                laser->sptHit->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
                laser->sptHit->addChild(getLight());
                laser->sptBeam->addChild(getLight());
                laser->sptHit->setScale(2, 1.6);
            }
            
            for(auto en:enemyArray){
                laser->enemyArray.pushBack(en);
            }
            laser->rectArray = stageTileRectArray;
            this->addChild(laser);
        }
        
        laser->setLaser(weapon->getPosition() - owner->center + gunLengthPos, owner->lastAngle);
        if (laser->hitEnemy != nullptr)
        {
            int demage = GameManager::getInstance()->getWeaponPower(weapon->value%1000);
            laser->hitEnemy->energy -= demage;
            
            laser->hitEnemy->showEnergy();
            
            showDamage(demage, laser->hitEnemy->getPosition());
            if (laser->hitEnemy->energy <= 0) {
                enemiesToRemove.pushBack(laser->hitEnemy);
                for(auto en:enemyArray){
                    laser->enemyArray.pushBack(en);
                }
                laser->enemyArray.eraseObject(laser->hitEnemy);
                
                this->runEffect(EFFECT_EXPLODE_BIG, laser->hitEnemy->getPosition());
                this->createRewards(laser->hitEnemy);
                if (laser->hitEnemy->rewards > 0) {
                    enemyKilledCount++;
                }
            }else
            {
                laser->hitEnemy->blinkForAWhile();
            }
        }
        if (loadedBulletCount <= 0) {
//            removeLaser();
        }
        return;
    }else*/ if (weapon->value%1000 == WEAPON_LIGHTNING_GUN) {
        Point startPos = weapon->getPosition() - owner->center + gunLengthPos;
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
        log("%f", owner->lastAngle);
        Lightning* lightning = Lightning::create("line.png", "lineTip.png", startPos, owner->isFlippedX()?180:0, enemyArray, stageTileRectArray);
        this->addChild(lightning);
        int damage = GameManager::getInstance()->getWeaponPower(WEAPON_LIGHTNING_GUN);
        for (int i = 0; i < lightning->hitEnemyArray.size(); i++) {
            EnemyBase* drop = lightning->hitEnemyArray.at(i);
            drop->energy -= (int)damage;
            drop->showEnergy();
            totalPower += damage;
            if (drop == theBoss) {
                DUAL_HUD->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
            }else{
                if(droppableArray.find(drop) != droppableArray.end()){
                    drop->desiredPosition = drop->desiredPosition + Point(0, 2);
                    drop->setPosition(drop->getPosition() + Point(0, 2));
                }
            }
            
            showDamage(damage, drop->getPosition());
            
            if (drop->energy <= 0) {
//                enemiesToRemove.pushBack(drop);
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
    //    owner->fireGuage -= (9.5f - /*playerMissileRate*/9*0.9f)*0.1f;
    //    CCLOG("Fire: %f, rate: %d", owner->fireGuage, playerMissileRate);
    
    Point point = owner->getPosition();
    Droppable* ms;
    int extraY = -9;
    Point msPos = Point(owner->getPosition().x + (owner->isTowardLeft?2:14), owner->getPosition().y+ extraY + 8 );
    if (weapon->value%1000 == WEAPON_PISTOL) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_PISTOL, GameManager::getInstance()->getWeaponPower(WEAPON_PISTOL));
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_UZI) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_UZI, GameManager::getInstance()->getWeaponPower(WEAPON_UZI));
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_MAGNUM) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_MAGNUM, GameManager::getInstance()->getWeaponPower(WEAPON_MAGNUM));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*10, owner->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_RIFLE) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_RIFLE, GameManager::getInstance()->getWeaponPower(WEAPON_RIFLE));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*14, owner->getPosition().y+ extraY + 8);
        straightMovingArray.pushBack(ms);
        ms->extraSpeed = 150;
    }else if (weapon->value%1000 == WEAPON_BALL_SHOOTER) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_BALL_SHOOTER, GameManager::getInstance()->getWeaponPower(WEAPON_BALL_SHOOTER));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*14, owner->getPosition().y+ extraY + 8);
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_FLAME_THROWER) {
        GameManager::getInstance()->playSoundEffect(SOUND_NOISE_37);
        ms = GameManager::getInstance()->getBullet(WEAPON_FLAME_THROWER, GameManager::getInstance()->getWeaponPower(WEAPON_FLAME_THROWER));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*14, owner->getPosition().y+ extraY + 6);
        customMovingArray.pushBack(ms);
        float duration = 0.5f;
        ms->setScale(1.5, 0.2);
        ms->setAnchorPoint(Point(0.5, 1));
        ms->setTag(UNIT_MISSILE_CUSTOM);
        ms->runAction(MoveBy::create(duration, gunLengthPos*13));
        ms->runAction(EaseIn::create(MoveBy::create(duration, Point(0, 20)), 5));
        ms->runAction(EaseIn::create(ScaleTo::create(duration, 1, 1.5), 2));
        ms->runAction(Sequence::create(DelayTime::create(duration), CallFuncN::create(CC_CALLBACK_1(DualWorld::missileMoveDone, this)), NULL));
    }else if (weapon->value%1000 == WEAPON_NIFE_THROWER) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_NIFE_THROWER, GameManager::getInstance()->getWeaponPower(WEAPON_NIFE_THROWER));
        ms->runAction(RepeatForever::create(RotateBy::create(0.3, 360)));
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_ANTI_BAT) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_PISTOL, GameManager::getInstance()->getWeaponPower(WEAPON_ANTI_BAT));
        ms->setPosition(weapon->getPosition() - owner->center);
        straightMovingArray.pushBack(ms);
        if (angle < -90 || angle > 90) {
            ms->movingAngle = -angle - 10;
        }else{
            ms->movingAngle = -angle + 10;
        }
        spriteBatch->addChild(ms);
        if (weapon->isEnemy) {
            enemyMissileArray.pushBack((Missile*)ms);
        }else{
            heroMissileArray.pushBack((Missile*)ms);
        }
        
        ms->setRotation(-ms->movingAngle);
        
        ms = GameManager::getInstance()->getBullet(WEAPON_PISTOL, 20);
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_CROSS_BOW) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_CROSS_BOW, GameManager::getInstance()->getWeaponPower(WEAPON_CROSS_BOW));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*10, owner->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_MACHINE_GUN) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_MACHINE_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_MACHINE_GUN));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*10, owner->getPosition().y+ extraY + 9);
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_LASER_GUN) {
        return;
//        GameManager::getInstance()->playSoundEffect(SOUND_FA);
//        ms = GameManager::getInstance()->getBullet(WEAPON_LASER_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_LASER_GUN));
//        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*10, owner->getPosition().y+ extraY + 7);
//        straightMovingArray.pushBack(ms);
//        ms->extraSpeed = 250;
    }else if (weapon->value%1000 == WEAPON_FIRE_BOTTLER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_FIRE_BOTTLER, GameManager::getInstance()->getWeaponPower(WEAPON_FIRE_BOTTLER));
        ms->setPosition(Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*10, owner->getPosition().y+ extraY + 2));
        customMovingArray.pushBack(ms);
        float dur = 2;
        ms->runAction(Sequence::create(JumpTo::create(dur, weapon->isEnemy?(player->getPosition()):(enemy->getPosition()), 50, 1), CallFuncN::create(CC_CALLBACK_1(DualWorld::fireBottleMoveDone, this)), nullptr));
        ms->velocity = Point(cos(-angle*3.14/180)*200, sin(-angle*3.14/180)*200);
        if (weapon->isEnemy) {
            ms->target = player;
        }else{
            ms->target = enemy;
        }
        ms->weight = 0.5f;
        ms->secondTag = WEAPON_FIRE_BOTTLER;
        ms->setTag(UNIT_MISSILE_DROPPABLE);
        ms->isFregile = true;
        ms->runAction(RepeatForever::create(RotateBy::create(0.5, 360)));
    }else if (weapon->value%1000 == WEAPON_RIFLE_GRENADE) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_RIFLE_GRENADE, GameManager::getInstance()->getWeaponPower(WEAPON_RIFLE_GRENADE));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*10, owner->getPosition().y + extraY+ 7);
        ms->secondTag = WEAPON_RIFLE_GRENADE;
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_SNIPER_RIFLE) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_SNIPER_RIFLE, GameManager::getInstance()->getWeaponPower(WEAPON_SNIPER_RIFLE));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*16, owner->getPosition().y + extraY+ 8);
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_ROCKET_LAUNCHER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_ROCKET_LAUNCHER, GameManager::getInstance()->getWeaponPower(WEAPON_ROCKET_LAUNCHER));
        ms->effectType = MISSILE_EFFECT_SMOKE;
        ms->secondTag = WEAPON_ROCKET_LAUNCHER;
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*10, owner->getPosition().y + extraY+ 9);
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_STRIKER) {
        GameManager::getInstance()->playSoundEffect(SOUND_GUN_FIRE_0);
        ms = GameManager::getInstance()->getBullet(WEAPON_STRIKER, GameManager::getInstance()->getWeaponPower(WEAPON_STRIKER));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*14, owner->getPosition().y + extraY+ 8);
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_ENERGY_GUN) {
        GameManager::getInstance()->playSoundEffect(SOUND_LASER);
        ms = GameManager::getInstance()->getBullet(WEAPON_ENERGY_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_ENERGY_GUN));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*13, owner->getPosition().y + extraY+ 8);
        straightMovingArray.pushBack(ms);
        ms->secondTag = WEAPON_ENERGY_GUN;
        ms->setScale(2);
        GameManager::getInstance()->makeItSiluk(ms);
    }else if (weapon->value%1000 == WEAPON_BAZOOKA) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_BAZOOKA, GameManager::getInstance()->getWeaponPower(WEAPON_BAZOOKA));
        ms->secondTag = WEAPON_BAZOOKA;
        ms->effectType = MISSILE_EFFECT_RED_SMOKE;
        ms->secondTag = WEAPON_BAZOOKA;
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*10, owner->getPosition().y+ extraY + 6);
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_DRAGON_RIFLE) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_DRAGON_RIFLE, GameManager::getInstance()->getWeaponPower(WEAPON_DRAGON_RIFLE));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*10, owner->getPosition().y+ extraY + 6);
        ms->effectType = MISSILE_EFFECT_RED_BALL;
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_GUIDED_ROCKET_LAUNCHER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_2);
        ms = GameManager::getInstance()->getBullet(WEAPON_GUIDED_ROCKET_LAUNCHER, GameManager::getInstance()->getWeaponPower(WEAPON_GUIDED_ROCKET_LAUNCHER));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*14, owner->getPosition().y+ extraY + 6);
        ms->target = findTarget(500);
        ms->setRotation(owner->isTowardLeft?180:0);
        ms->setTag(UNIT_MISSILE_CHASING);
        ms->effectType = MISSILE_EFFECT_SMOKE;
        ms->secondTag = WEAPON_GUIDED_ROCKET_LAUNCHER;
        chasingMissileArray.pushBack((Missile*)ms);
    }else if (weapon->value%1000 == WEAPON_MATT_A4) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_MATT_A4, GameManager::getInstance()->getWeaponPower(WEAPON_MATT_A4));
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*10, owner->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_SLUGGER) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*13, owner->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_SLUGGER_NG) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->setPosition(weapon->getPosition() - owner->center + gunLengthPos);
        straightMovingArray.pushBack(ms);
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        
        ms->movingAngle =  -angle + 45;
        spriteBatch->addChild(ms);
        heroMissileArray.pushBack((Missile*)ms);
        
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->setPosition(weapon->getPosition() - owner->center + gunLengthPos);
        straightMovingArray.pushBack(ms);
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        
        ms->movingAngle =  -angle - 45;
        spriteBatch->addChild(ms);
        heroMissileArray.pushBack((Missile*)ms);
        
        ms = GameManager::getInstance()->getBullet(WEAPON_SLUGGER, GameManager::getInstance()->getWeaponPower(WEAPON_SLUGGER));
        ms->effectType = MISSILE_EFFECT_GREEN_SMOKE;
        
        straightMovingArray.pushBack(ms);
    }else if (weapon->value%1000 == WEAPON_SHARK_GUN) {
        GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        ms = GameManager::getInstance()->getBullet(WEAPON_SHARK_GUN, GameManager::getInstance()->getWeaponPower(WEAPON_SHARK_GUN));
        ms->effectType = MISSILE_EFFECT_SHARK_GUN;
        msPos = Point(owner->getPosition().x + (owner->isTowardLeft?-1:1)*13, owner->getPosition().y+ extraY + 7);
        straightMovingArray.pushBack(ms);
        
    }else if (weapon->value%1000 == WEAPON_LASER_COIN) {
        return;
    }
//    ms->value%4 = weapon->value%4;
    ms->setPosition(weapon->getPosition() + msPos - owner->getPosition());
    
    if (weapon->value%1000 != WEAPON_FLAME_THROWER) {
        Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(ms->getPosition());
        explode->setAnchorPoint(Point(1, 0.5));
        Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
        explode->setRotation(angle + 180);
        if (rand()%2 == 0) {
            explode->setFlippedY(true);
        }
//        owner->runFireAnimation();
    }
    
    
    if (weapon->value%1000 != WEAPON_FIRE_BOTTLER &&
        weapon->value%1000 != WEAPON_NIFE_THROWER &&
        weapon->value%1000 != WEAPON_BALL_SHOOTER &&
        weapon->value%1000 != WEAPON_FLAME_THROWER&&
        weapon->value%1000 != WEAPON_CROSS_BOW) {
        Droppable* tanpi = Droppable::createDroppable(UNIT_COIN, 0, 0, "bulletPi.png");
        droppableCoinArray.pushBack(tanpi);
        tanpi->weight = 1;
        tanpi->setScale((ms->getContentSize().height)/tanpi->getContentSize().height);
        auto rotate = RotateBy::create(1, rand()%2 == 0?300:-300);
        rotate->setTag(ACTION_TAG_ROTATION);
        tanpi->runAction(rotate);
        tanpi->runAction(Sequence::create(DelayTime::create(3), CallFuncN::create(CC_CALLBACK_1(DualWorld::coinWaitDone, this)), NULL));
        spriteBatch->addChild(tanpi);
        
        float middleX = cos(angle*3.14/180)*(gunWidth/2);
        float middleY = -sin(angle*3.14/180)*(gunWidth/2);
        
        tanpi->setPosition(ms->getPosition() - Point(middleX, middleY));
        tanpi->velocity = Point(rand()%1000 - 500, 500);
    }
    
    ms->energy = weapon->powerPerAttack;//weapon->totalPower*weapon->shootInterval;
    ms->movingAngle = -angle;
    
    ms->setRotation(angle);
    if (weapon->value%1000 == WEAPON_MACHINE_GUN ||
        weapon->value%1000 == WEAPON_UZI){
        ms->movingAngle += rand()%10 - 5;
    }
    if (GameManager::getInstance()->developer) {
        //        ms->energy = 2000;
    }
    spriteBatch->addChild(ms);
    if (weapon->isEnemy) {
        enemyMissileArray.pushBack((Missile*)ms);
//        ms->movingAngle += 180;
        ms->setRotation(-ms->movingAngle);
    }else{
        heroMissileArray.pushBack((Missile*)ms);
    }
    
//    log("x: %f, y: %f", ms->getPosition().x, ms->getPosition().y);
//    log("player x: %f,player  y: %f", owner->getPosition().x, owner->getPosition().y);
    if (weapon->shootInterval < 0.2f && rand()%2==0) {
        return;
    }
    Node* light = getLight();
    light->setPosition(ms->getPosition());
    this->addChild(light);
    light->runAction(Sequence::create(DelayTime::create(0.1), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, light)), NULL));
    light->setColor(Color3B(250, 100, 100));
}
void DualWorld::fireBottleMoveDone(Ref* ref){
    Droppable* ms = (Droppable*)ref;
    customMovingArray.eraseObject(ms);
    runEffect(EFFECT_EXPLODE_HUGE, ms->getPosition());
    splashDemage(ms->getPosition(), 10, ms->energy);
    ms->removeFromParent();
}
void DualWorld::missileEffectUpdate(float dt){
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
                        GameManager::getInstance()->playSoundEffect(SOUND_TOONG);
                    }
                    
                    Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
                    spriteBatch->addChild(explode);
                    explode->setPosition(ms->getPosition());
                    explode->setAnchorPoint(Point(1, 0.5));
                    Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
                    Animate* animate = Animate::create(animation);
                    explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(DualWorld::spriteMoveDone, this)), NULL));
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
                    ms->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur, Point(cos(ms->movingAngle*3.14/180)*movingDistance, sin(ms->movingAngle*3.14/180)*movingDistance)), 5), CallFuncN::create(CC_CALLBACK_1(DualWorld::missileMoveDone, this)), nullptr));
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
void DualWorld::missileMoveDone(Ref* obj)
{
    EnemyBase* missile = (EnemyBase*)obj;
    heroMissileArray.eraseObject((Missile*)missile);
    enemyMissileArray.eraseObject((Missile*)missile);
    customMovingArray.eraseObject(missile);
    missile->stopAllActions();
    missile->removeFromParentAndCleanup(true);
}
void DualWorld::fireBulletMoveDone(Ref* obj)
{
    Droppable* missile = (Droppable*)obj;
    heroMissileArray.eraseObject((Missile*)missile);
    customMovingArray.eraseObject(missile);
    missile->stopAllActions();
    missile->removeFromParentAndCleanup(true);
}
void DualWorld::enemyBaseMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    EnemyBase* sprite = (EnemyBase*)obj;
    sprite->stopAllActions();
    enemyArray.eraseObject(sprite);
    sprite->removeFromParentAndCleanup(true);
}
void DualWorld::coinWaitDone(Ref* obj){
    Droppable* drop = (Droppable*)obj;
    coinArray.eraseObject(drop);
    droppableCoinArray.eraseObject(drop);
    drop->runAction(Sequence::create(FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(DualWorld::removeFromParentAndCleanup, drop)), NULL));
}
void DualWorld::spriteMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Sprite* sprite = (Sprite*)obj;
    sprite->stopAllActions();
    sprite->removeFromParentAndCleanup(true);
}
void DualWorld::nodeMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Label* lbl = (Label*)obj;
    
    lbl->stopAllActions();
    lbl->removeFromParentAndCleanup(true);
}

/*void DualWorld::fireLoop(float dt)
 {
 if (player->fireGuage < 0) {
 player->fireGuage += dt;
 }
 
 }*/

void DualWorld::blinkingPlayer(float dt)
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
        
        this->unschedule(schedule_selector(DualWorld::blinkingPlayer));
    }
}

void DualWorld::pauseLayer()
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

void DualWorld::resumeLayer()
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

void DualWorld::refreshPlayerInfo(int costum, int weapon)
{
    setPlayerInfo();
}

/*
 void DualWorld::draw()
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
