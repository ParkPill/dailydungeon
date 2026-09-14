
#include "GameManager.h"
#include "LanguageManager.h"
#include "ZOrderChangleSprite.h"
#include "SettingLayer.h"
#include "ServerManager.h"
#include "GameSharing.h"
//#include "StageClearLayer.h"
//#include "StageSelectScene.h"
//#include "ThemeSelectScene.h"
//#include "GameStartLayer.h"
#include "Title.h"

using namespace cocos2d;
using namespace CocosDenshion;

GameManager* GameManager::m_mySingleton = NULL;

GameManager::GameManager()
{
    life = UserDefault::getInstance()->getIntegerForKey(KEY_LIFE, 5);
    
    isInMiddleOfGame = false;
    isGoogleSigned = false;
    isPaidGame = false;
    gameStarted = false;
    isUsingController = false;
    
    appAlreadyLaunched = false;
//    pushedLayer = NULL;
    initComplete = false;
    
    
    totalThemeCount = 7;
    pauseLayer = NULL;
    

    isGuestPlay = false;

    optionLayer = NULL;
    gameOverLayer = NULL;
    gameOverScene = NULL;
    currentStageLayer = NULL;
//    achievementLayer = NULL;
    titleLayer = NULL;
    
    jumpPressed = false;
    cPressed = false;
    firePressed = false;
    leftPressed = false;
    rightPressed = false;
    downPressed = false;
    upPressed = false;
    paidUser = UserDefault::getInstance()->getBoolForKey(KEY_IAP_USER, false);
    settingLayer = NULL;
    
    isVideoRewardAttack = false;
    isVideoRewardEnergy = false;
    isVideoRewardCoins = false;
//    originallyDisabledMenusWhenLayerPushed = NULL;
    
    myPhoto = NULL;
    
    hudLayer = NULL;
    
    market = MARKET_PLAYSTORE_PAID;
//    priceArray = __Dictionary::create();
//    priceArray->retain();
    CCLOG("GameManager almost created");

    CCLOG("GameManager created");
}
void GameManager::initGameManager(){
    
    float musicVolumn = UserDefault::getInstance()->getFloatForKey(KEY_MUSIC_VOLUMN, 1);
    setMusicVolumn(musicVolumn);
    float soundVolumn = UserDefault::getInstance()->getFloatForKey(KEY_SOUND_VOLUMN, 1);
    setSoundVolumn(soundVolumn);

    for(int i = 0; i<12;i++){
        dailyMissionComplete[i] = MISSION_STATE_NOT_CLEAR;
    }
    
//    initAchievement();
    loadBagContent();
}
int GameManager::getGemForCoin(int coinCount){
    if(coinCount/100 == coinCount/100.0f){
        return coinCount/100;
    }else{
        return coinCount/100 + 1;
    }
}
void GameManager::setFontName(Label* lbl, const char* name, float fontSize){
    if(FileUtils::getInstance()->isFileExist(name))
    {
        TTFConfig config = lbl->getTTFConfig();
        config.fontFilePath = name;
        config.fontSize = fontSize;
        lbl->setTTFConfig(config);
    }
    else{
        lbl->setSystemFontName(name);
    }
}
ValueVector GameManager::split(const std::string &str, const std::string &delim){
    ValueVector res;
    size_t current = 0, found, quote;
    bool lastQuoteShouldBeRemoved = false;
    while((found = str.find_first_of(delim, current)) != std::string::npos){
        if (delim.compare(",") == 0) {
            if ((quote = str.find_first_of("\"", current)) != std::string::npos && current >= quote) {
                if (current == quote) {
                    current++;
                }
                if ((quote = str.find_first_of("\"", quote + 1)) != std::string::npos) {
                    found = str.find_first_of(delim, quote);
                    if (found == quote + 1) {
                        lastQuoteShouldBeRemoved = true;
                    }
                }
            }
        }
        if (lastQuoteShouldBeRemoved) {
            res.push_back(Value(std::string(str, current, found - current - 1)));
            current = found + delim.size();
        }else{
            res.push_back(Value(std::string(str, current, found - current)));
            current = found + delim.size();
        }
        
        bool isLast = found == std::string::npos;
        if (isLast) {
            return res;
        }
    }
    res.push_back(Value(std::string(str, current, str.size() - current)));
    return res;
}

bool GameManager::endsWith(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
bool GameManager::isVideoReady(){
    return GameSharing::isVideoAvailable();
}
const char* GameManager::getFont(int font){
    if(font == FONT_DEFAULT){
        if(LanguageManager::getInstance()->getLanguageType() == LanguageType::KOREAN){
            return "BMDOHYEON.ttf";
        }else if(LanguageManager::getInstance()->getLanguageType() == LanguageType::ENGLISH){
            return "visitor1.ttf";
        }else{
            return "Helvetica";
        }
    }else if(font == FONT_ARIAL){
        return "Arial";
    }else if(font == FONT_BITDUST_ONE){
        return "bitdust1.ttf";
    }else if(font == FONT_BITDUST_TWO){
        return "bitdust2.ttf";
    }else if(font == FONT_VISITOR){
        return "visitor1.ttf";
    }
    return "Helvetica";
}
int GameManager::getBombCount(){
    return UserDefault::getInstance()->getIntegerForKey("Bomb_Count", 3);
}
void GameManager::setBombCount(int bombCount){
    UserDefault::getInstance()->setIntegerForKey("Bomb_Count", bombCount);
}
int GameManager::getPotionCount(){
    return UserDefault::getInstance()->getIntegerForKey("Potion_Count", 3);
}
void GameManager::setPotionCount(int potionCount){
    UserDefault::getInstance()->setIntegerForKey("Potion_Count", potionCount);
}
GameManager* GameManager::getInstance()
{
    if(NULL == m_mySingleton)
    {
        m_mySingleton = new GameManager();
    }
    
    return m_mySingleton;
}
void GameManager::exitGame(){
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

const char* GameManager::getShortenedKoreanString(std::string str, int length){
    const char* nick = "";
    if (str.length() > length) {
        CCLOG("original: %s", str.c_str());
        int normalIndex = -1;
        for (int k = 0; k < length; k++) {
            if (str[k] >= 0 && (int)str[k] < 128) {
                normalIndex = k;
            }
        }
        
        CCLOG("normal: %d", normalIndex);
        if (normalIndex < 0 || normalIndex == length) {
            nick = __String::createWithFormat("%s...",str.substr(0, length).c_str())->getCString();
            CCLOG("all korean: normalIndex: %d", normalIndex);
        }else{
            int last = normalIndex;
            for (int i = last; i < length - 3; i += 3) {
                last += 3;
            }
            CCLOG("ugly/ last: %d, normalIndex: %d", last, normalIndex);
            nick = __String::createWithFormat("%s...",str.substr(0, (last + 1)).c_str())->getCString();
        }
    }
    return nick;
}

const char* GameManager::getWeaponImageFileName(int infoType){
    switch (infoType) {
        case WEAPON_PISTOL:
            return __String::createWithFormat("pistol.png")->getCString();
        case WEAPON_MAGNUM:
            return __String::createWithFormat("magnum.png")->getCString();
        case WEAPON_UZI:
            return __String::createWithFormat("uzi.png")->getCString();
        case WEAPON_SNIPER_RIFLE:
            return __String::createWithFormat("sniperRifle.png")->getCString();
        case WEAPON_SLUGGER:
            return __String::createWithFormat("slugger.png")->getCString();
        case WEAPON_SLUGGER_NG:
            return __String::createWithFormat("sluggerNG.png")->getCString();
        case WEAPON_ROCKET_LAUNCHER:
            return __String::createWithFormat("rocketLauncher.png")->getCString();
        case WEAPON_RIFLE_GRENADE:
            return __String::createWithFormat("rifleGrenade.png")->getCString();
        case WEAPON_RIFLE:
            return __String::createWithFormat("rifle.png")->getCString();
        case WEAPON_MATT_A4:
            return __String::createWithFormat("mattA4.png")->getCString();
        case WEAPON_NIFE_THROWER:
            return __String::createWithFormat("nifeThrower.png")->getCString();
        case WEAPON_MACHINE_GUN:
            return __String::createWithFormat("machinegun.png")->getCString();
        case WEAPON_LASER_GUN:
            return __String::createWithFormat("lasergun.png")->getCString();
        case WEAPON_GUIDED_ROCKET_LAUNCHER:
            return __String::createWithFormat("guidedRocketLauncher.png")->getCString();
        case WEAPON_FIRE_BOTTLER:
            return __String::createWithFormat("fireBottler.png")->getCString();
        case WEAPON_ENERGY_GUN:
            return __String::createWithFormat("energygun.png")->getCString();
        case WEAPON_DRAGON_RIFLE:
            return __String::createWithFormat("dragonRifle.png")->getCString();
        case WEAPON_CROSS_BOW:
            return __String::createWithFormat("CrossBow.png")->getCString();
        case WEAPON_BAZOOKA:
            return __String::createWithFormat("bazooka.png")->getCString();
        case WEAPON_BALL_SHOOTER:
            return __String::createWithFormat("ballShooter.png")->getCString();
        case WEAPON_FLAME_THROWER:
            return __String::createWithFormat("flameThrower.png")->getCString();
        case WEAPON_ANTI_BAT:
            return __String::createWithFormat("antiBat.png")->getCString();
        case WEAPON_STRIKER:
            return __String::createWithFormat("striker.png")->getCString();
        case WEAPON_LIGHTNING_GUN:
            return __String::createWithFormat("lightningGun.png")->getCString();
        case WEAPON_SHARK_GUN:
            return __String::createWithFormat("sharkGun.png")->getCString();
        case WEAPON_LASER_COIN:
            return __String::createWithFormat("laserCoin.png")->getCString();
        default:
            return __String::createWithFormat("pistol.png")->getCString();
            break;
    }
}
const char* GameManager::getShieldImageFileName(int item){
    switch (item) {
        case SHIELD_WOOD:
            return __String::createWithFormat("shield0.png")->getCString();
        case SHIELD_INDIAN:
            return __String::createWithFormat("shield1.png")->getCString();
        case SHIELD_STEEL:
            return __String::createWithFormat("shield2.png")->getCString();
        case SHIELD_RED:
            return __String::createWithFormat("shield5.png")->getCString();
        case SHIELD_BLUE:
            return __String::createWithFormat("shield4.png")->getCString();
        case SHIELD_GREEN:
            return __String::createWithFormat("shield6.png")->getCString();
        case SHIELD_GOLD:
            return __String::createWithFormat("shield3.png")->getCString();
        case SHIELD_PALADIN:
            return __String::createWithFormat("shield7.png")->getCString();
        
        default:
            return __String::createWithFormat("shield0.png")->getCString();
            break;
    }
}
double GameManager::getAngle(Point pos1, Point pos2){
    float xGap = pos1.x - pos2.x;
    float yGap = pos1.y - pos2.y;
    return atan2(yGap, xGap)*180/3.14;
}
void GameManager::makeItSiluk(Node* node){
    float originalScaleX = node->getScaleX();
    float originalScaleY = node->getScaleY();
    
    float duration = 0.3f;
    float scaleBit;
    if(node->getContentSize().width > node->getContentSize().height){
        scaleBit = node->getContentSize().height*0.05;
    }else{
        scaleBit = node->getContentSize().width*0.05;
    }
    node->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(duration, (1+scaleBit/node->getContentSize().width)*originalScaleX, (1-scaleBit/node->getContentSize().width)*originalScaleY), ScaleTo::create(duration, (1-scaleBit/node->getContentSize().width)*originalScaleX, (1+scaleBit/node->getContentSize().width)*originalScaleY), NULL)));
}
bool GameManager::DoesWeaponExist(int weaponType){
    switch (weaponType) {
        case WEAPON_PISTOL:
            break;
        case WEAPON_MAGNUM:
            break;
        case WEAPON_UZI:
            break;
        case WEAPON_SNIPER_RIFLE:
            break;
        case WEAPON_SLUGGER:
        case WEAPON_SLUGGER_NG:
            break;
        case WEAPON_ROCKET_LAUNCHER:
            break;
        case WEAPON_RIFLE_GRENADE:
            break;
        case WEAPON_RIFLE:
            break;
        case WEAPON_MATT_A4:
            break;
        case WEAPON_NIFE_THROWER:
            break;
        case WEAPON_MACHINE_GUN:
            break;
        case WEAPON_LASER_GUN:
            break;
        case WEAPON_GUIDED_ROCKET_LAUNCHER:
            break;
        case WEAPON_FIRE_BOTTLER:
            break;
        case WEAPON_ENERGY_GUN:
            break;
        case WEAPON_DRAGON_RIFLE:
            break;
        case WEAPON_CROSS_BOW:
            break;
        case WEAPON_BAZOOKA:
            break;
        case WEAPON_BALL_SHOOTER:
            break;
        case WEAPON_FLAME_THROWER:
            break;
        case WEAPON_ANTI_BAT:
            break;
        case WEAPON_STRIKER:
            break;
        default:
            return false;
            break;
    }
    return true;
}
int GameManager::getNextToThisWeapon(int weaponType){
    for (int i = weaponType + 1; i < WEAPON_NO_MORE; i++) {
        if (DoesWeaponExist(i)) {
            return i;
        }
    }
    return WEAPON_NO_MORE;
}
int GameManager::getNextWeaponToUnlock(int weaponType){
    for (int i = WEAPON_NOT_EXIST + 1; i < WEAPON_NO_MORE; i++) {
        if (DoesWeaponExist(i) && !getWeaponUnlocked(i)) {
            return i;
        }
    }
    return WEAPON_NO_MORE;
}
//void GameManager::setStageLayer(Layer* layer)
//{
//    stageLayer = layer;
//    stageLayer->retain();
//}

//HelloWorld* GameManager::getStageLayer()
//{
//    if (!stageLayer) {
//        return NULL;
//    }
//    return stageLayer;

//}

void GameManager::setHudLayer(HudLayer* layer)
{
    hudLayer = layer;
//    hudLayer->retain();
}
void GameManager::setDualHudLayer(DualHudLayer* layer)
{
    dualHudLayer = layer;
}
HudLayer* GameManager::getHudLayer()
{
    if(!hudLayer){
        return NULL;
    }
    return hudLayer;
}
DualHudLayer* GameManager::getDualHudLayer()
{
    if(!dualHudLayer){
        return NULL;
    }
    return dualHudLayer;
}

void GameManager::setStageScene(Scene* scene)
{
    isStageSetOnce = true;
//    stageScene->retain();
}

void GameManager::nativeControllerButtonEvent(int controller, int button, bool isPressed, float value, bool isAnalog)
{
//    EventKeyboard::KeyCode keyCode = (EventKeyboard::KeyCode)button;
    if(isPressed){
        if(button == 29 ||
           button == 54 ||
           button == 96 ||
           button == 99
           ){
            GameManager::getInstance()->buttonDown(BUTTON_A);
        }else if(button == 30 ||
                 button == 52 ||
                 button == 58 ||
                 button == 23 ||
                 button == 97 ||
                 button == 100
                 ){
            if (downPressed) {
                if (getHudLayer()) {
                    getCurrentStageLayer()->getDown();
                }
            }else{
                GameManager::getInstance()->buttonDown(BUTTON_B);
            }
        }else if(button == 31){
            GameManager::getInstance()->cPressed = true;
        }else if(button == 19){
            GameManager::getInstance()->buttonDown(BUTTON_B);
        }else if (button == 20){
            GameManager::getInstance()->downPressed = true;
        }else if(button == 21){
            GameManager::getInstance()->buttonDown(BUTTON_LEFT);
        }else if(button == 22){
            GameManager::getInstance()->buttonDown(BUTTON_RIGHT);
        }
//        CCLOG("Key with keycode %d pressed/ And left: %d", keyCode, (int)EventKeyboard::KeyCode::KEY_DPAD_LEFT);
    }else{
        if(button == 29 ||
           button == 54 ||
           button == 96 ||
           button == 99
           ){
            GameManager::getInstance()->buttonUp(BUTTON_A);
        }else if(button == 30 ||
                 button == 52 ||
                 button == 58 ||
                 button == 23 ||
                 button == 97 ||
                 button == 100
                 ){
            GameManager::getInstance()->buttonUp(BUTTON_B);
        }else if(button == 31){
            GameManager::getInstance()->cPressed = false;
        }else if(button == 19){
            GameManager::getInstance()->buttonUp(BUTTON_B);
        }else if (button == 20){
            GameManager::getInstance()->downPressed = false;
        }else if(button == 21){
            GameManager::getInstance()->buttonUp(BUTTON_LEFT);
        }else if(button == 22){
            GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
        }else if(button == 108 ||
                 button == 109){
            if(GameManager::getInstance()->isInMiddleOfGame){
                GameManager::getInstance()->getHudLayer()->showPauseLayer();
            }
        }
//        CCLOG("Key with keycode %d released", keyCode);
    }
}

void GameManager::nativeControllerLAxisXEvent(int controller, float value){
//    CCLOG("Analog Key with X keycode: %d, %f", controller, value);
    if (value < -0.5) {
//        GameManager::getInstance()->buttonDown(BUTTON_LEFT);
    }else if(value > 0.5){
//        GameManager::getInstance()->buttonDown(BUTTON_RIGHT);
    }else{
//        GameManager::getInstance()->buttonUp(BUTTON_LEFT);
//        GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
    }
}
void GameManager::nativeControllerLAxisYEvent(int controller, float value){
//    CCLOG("Analog Key with Y keycode: %d, %f", controller, value);
    if (value < -0.9) {
//        GameManager::getInstance()->cPressed = true;
    }else if(value > 0.5){
//        GameManager::getInstance()->buttonDown(BUTTON_B);
    }else{
//        GameManager::getInstance()->buttonUp(BUTTON_B);
//        GameManager::getInstance()->cPressed = false;
    }
}

void GameManager::buttonDown(int buttonType)
{
    if (getCurrentStageLayer() != nullptr && getCurrentStageLayer()->blockKeys) {
        return;
    }
    switch (buttonType) {
        case BUTTON_LEFT:
            leftPressed = true;
            if (this->isInMiddleOfGame && this->getCurrentStageLayer()->player->onGround && this->getCurrentStageLayer()->player->state != STATE_WALKING) {
                this->getCurrentStageLayer()->player->startWalkingAnimation();
            }
            break;
        case BUTTON_RIGHT:
            rightPressed = true;
            if (this->isInMiddleOfGame && this->getCurrentStageLayer()->player->onGround && this->getCurrentStageLayer()->player->state != STATE_WALKING) {
                this->getCurrentStageLayer()->player->startWalkingAnimation();
            }
            break;
        case BUTTON_A:
            if (this->isInMiddleOfGame) {
                this->getCurrentStageLayer()->playerFireCoolTime = 0;
            }
            
            firePressed = true;
            break;
        case BUTTON_B:
            jumpPressed = true;
            break;
        default:
            break;
    }
}

void GameManager::buttonUp(int buttonType)
{
    switch (buttonType) {
        case BUTTON_LEFT:
            leftPressed = false;
            break;
        case BUTTON_RIGHT:
            rightPressed = false;
            break;
        case BUTTON_A:
            firePressed = false;
            break;
        case BUTTON_B:
            jumpPressed = false;
            break;
        default:
            break;
    }
    if(!leftPressed && !rightPressed){
        HelloWorld* world = this->getCurrentStageLayer();
        if(!world){
            return;
        }
        Droppable* pl = world->player;
        bool ground = pl->onGround;
        if (this->isInMiddleOfGame && ground) {
            this->getCurrentStageLayer()->player->stopAndStand();
        }
    }
}
void GameManager::shakeIt(Node* node){
    //node->stopAllActionsByTag(ACTION_TAG_ANIMATION);
    //node->setPosition(Point::ZERO);
    Sequence* seq = Sequence::create(MoveBy::create(0.05, Point(0.5,0.5)), MoveBy::create(0.05, Point(-1, -0.5)), MoveBy::create(0.03, Point(0.5, 0)), NULL);
    
    seq->setTag(ACTION_TAG_ANIMATION);
    node->runAction(seq);
}
//const char* GameManager::getAchievementId(int achievementIndex){
//    if (GameManager::getInstance()->market == MARKET_APPSTORE_PAID ||
//        GameManager::getInstance()->market == MARKET_APPSTORE_FREE) {
//
//        switch (achievementIndex) {
//            case ACHIEVEMENT_COMPLETE_TUTORIAL:
//                return "starcollecter";
//            case ACHIEVEMENT_BEAT_BOSS:
//                return "brkeeper";
//            case ACHIEVEMENT_OPEN_GOLD_SUITCASE:
//                return "brfirstfruit";
//            case ACHIEVEMENT_PERFECT_SCORE:
//                return "brrichman";
//            case ACHIEVEMENT_WHO_IS_THE_HOOD :
//                return "brkiller";
//            case ACHIEVEMENT_WEAPON_MASTER:
//                return "brangrykettel";
//            case ACHIEVEMENT_DUNGEON_MASTER:
//                return "brwhosboss";
//            case ACHIEVEMENT_WHERE_IS_THE_PRINCESS:
//                return "brweaponmaster";
//            case ACHIEVEMENT_OPEN_JEWELRY_BOX:
//                return "brleadership";
//            case ACHIEVEMENT_COLLECT_RELICS:
//                return "belegendary";
//            default:
//                break;
//        }
//    }else{
//        switch (achievementIndex) {
//            case ACHIEVEMENT_COMPLETE_TUTORIAL:
//                return "CgkIjOjFydEdEAIQAQ";
//            case ACHIEVEMENT_BEAT_BOSS:
//                return "CgkIjOjFydEdEAIQAg";
//            case ACHIEVEMENT_OPEN_GOLD_SUITCASE:
//                return "CgkIjOjFydEdEAIQAw";
//            case ACHIEVEMENT_PERFECT_SCORE:
//                return "CgkIjOjFydEdEAIQBA";
//            case ACHIEVEMENT_WHO_IS_THE_HOOD :
//                return "CgkIjOjFydEdEAIQBQ";
//            case ACHIEVEMENT_WEAPON_MASTER:
//                return "CgkIjOjFydEdEAIQBg";
//            case ACHIEVEMENT_DUNGEON_MASTER:
//                return "CgkIjOjFydEdEAIQBw";
//            case ACHIEVEMENT_WHERE_IS_THE_PRINCESS:
//                return "CgkIjOjFydEdEAIQCA";
//            case ACHIEVEMENT_OPEN_JEWELRY_BOX:
//                return "CgkIjOjFydEdEAIQCQ";
//            case ACHIEVEMENT_COLLECT_RELICS:
//                return "CgkIjOjFydEdEAIQCg";
//            default:
//                break;
//        }
//    }
//    return "";
//}
void GameManager::saveCoin()
{
    CCLOG("save coin");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//    checkDataSecure();
#endif
    UserDefault::getInstance()->flush();
}

void GameManager::setFacebookReady(bool ready){
    isFacebookReady = ready;
    if(ready){
        CCLOG("setFacebookReaday true");
    }else{
        CCLOG("setFacebookReaday false");
    }
}
void GameManager::addCoin(int howMuch){
    int coin = getCoin();
    coin += howMuch;
    UDSetInt(KEY_COIN_COUNT, coin);
//    int one = (char)c0;
//    int ten = (char)c1;
//    int hun = (char)c2;
//    int tho = (char)c3;
//    int tth = (char)c4;
//    int hth = (char)c5;
//    int mil = (char)c6;
//    int total = one + ten*10 + hun*100 + tho*1000 + tth*10000 + hth*100000 + mil*1000000;
//    total += howMuch;
//    
//    c0 = (int)(total%10);
//    c1 = (int)((total/10)%10);
//    c2 = (int)((total/100)%10);
//    c3 = (int)((total/1000)%10);
//    c4 = (int)((total/10000)%10);
//    c5 = (int)((total/100000)%10);
//    c6 = (int)((total/1000000)%10);
}
void GameManager::setCoin(int howMuch){
//    int one = (char)c0;
//    int ten = (char)c1;
//    int hun = (char)c2;
//    int tho = (char)c3;
//    int tth = (char)c4;
//    int hth = (char)c5;
//    int mil = (char)c6;
//    int total = one + ten*10 + hun*100 + tho*1000 + tth*10000 + hth*100000 + mil*1000000;
//    total = howMuch;
//    
//    c0 = (int)(total%10);
//    c1 = (int)((total/10)%10);
//    c2 = (int)((total/100)%10);
//    c3 = (int)((total/1000)%10);
//    c4 = (int)((total/10000)%10);
//    c5 = (int)((total/100000)%10);
//    c6 = (int)((total/1000000)%10);
    UDSetInt(KEY_COIN_COUNT, howMuch);
}

int GameManager::getCoin(){
//    int one = (char)c0;
//    int ten = (char)c1;
//    int hun = (char)c2;
//    int tho = (char)c3;
//    int tth = (char)c4;
//    int hth = (char)c5;
//    int mil = (char)c6;
//    return one + ten*10 + hun*100 + tho*1000 + tth*10000 + hth*100000 + mil*1000000;
    return UDGetInt(KEY_COIN_COUNT, 0);
}

void GameManager::addSoul(int howMuch){
    int soulCount = getSoul();
    soulCount += howMuch;
    UDSetInt(KEY_SOUL_COUNT, soulCount);
}

int GameManager::getSoul(){
    return UDGetInt(KEY_SOUL_COUNT, 0);
}
void GameManager::addGem(int howMuch){
    int gemCount = getGem();
    gemCount += howMuch;
    setGem(gemCount);
}
void GameManager::setGem(int howMuch){
    UDSetInt(KEY_GEM, howMuch);
    UD->flush();
}

int GameManager::getGem(){
    return UDGetInt(KEY_GEM, 0);
}

void GameManager::addCurrency(int howMuch, int currencyType){
    int count = getCurrency(currencyType);
    count += howMuch;
    setCurrency(count, currencyType);
}
void GameManager::setCurrency(int howMuch, int currencyType){
    UDSetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, currencyType).c_str(), howMuch);
    UD->flush();
}

int GameManager::getCurrency(int currencyType, int defaultValue){
    return UDGetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, currencyType).c_str(), defaultValue);
}

void GameManager::unscheduleAllNotifications(){
//    NativeInterface::NativeInterfacecancelAllLocalNotification();
}
void GameManager::scheduleLocalNotification(const char* title, const char* msg, int time){
//    NativeInterface::NativeInterfaceregisterLocalNotification(time, msg, title);
}
void GameManager::scheduleLocalNotification(){
//    NativeInterface::NativeInterfacecancelAllLocalNotification();
    if (!this->getNotificationOn()) {
        return;
    }
    
    CCLOG("schedule");
    
    if(LanguageManager::getInstance()->getLanguageType() == LanguageType::KOREAN){
        /*NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*1, "공주가 기다려요! 어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*2, "공주가 기다려요! 대리님 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*3, "깃발이 가득 찼어요! 맘껏 대리의 전설을 즐겨볼까요?", "깃발이 가득 찼어요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*4, "공주가 기다려요!어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*5, "공주가 기다려요! 어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*6, "공주가 기다려요! 대리님 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*7, "깃발이 가득 찼어요! 맘껏 대리의 전설을 즐겨볼까요?", "깃발이 가득 찼어요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*8, "공주가 기다려요!어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*9, "공주가 기다려요! 어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*10, "공주가 기다려요! 대리님 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*11, "깃발이 가득 찼어요! 맘껏 대리의 전설을 즐겨볼까요?", "깃발이 가득 찼어요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*12, "공주가 기다려요!어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*13, "공주가 기다려요! 어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*14, "공주가 기다려요! 대리님 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*15, "깃발이 가득 찼어요! 맘껏 대리의 전설을 즐겨볼까요?", "깃발이 가득 찼어요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*16, "공주가 기다려요!어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*17, "공주가 기다려요! 어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*18, "공주가 기다려요! 대리님 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*19, "깃발이 가득 찼어요! 맘껏 대리의 전설을 즐겨볼까요?", "깃발이 가득 찼어요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*20, "공주가 기다려요!어서 공주를 구해주세요!", "공주가 기다려요!");
    }else{
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*1, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*3, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*5, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*7, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*9, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*11, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*13, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*15, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*17, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*19, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*21, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*30, "Come and save the princess!", "The princess is waiting for you");*/
    }
}
void GameManager::setMusicVolumn(float vol)
{
    float value = 0;
    float percent = vol * 100;
    if(percent > 30){
        value += 0.1;
        value += (percent - 30)*0.9f/70;
    }else{
        value = percent/300.0f;
    }
    
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(vol);
    UserDefault::getInstance()->setFloatForKey(KEY_MUSIC_VOLUMN, vol);
//    GameManager::getInstance()->saveCoin();
}
void GameManager::setSoundVolumn(float vol)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(vol);
    UserDefault::getInstance()->setFloatForKey(KEY_SOUND_VOLUMN, vol);
//    GameManager::getInstance()->saveCoin();
}
void GameManager::setNotificationOn(bool onOff)
{
    UserDefault::getInstance()->setBoolForKey(KEY_ALERT_ONOFF, onOff);
//    GameManager::getInstance()->saveCoin();
}
void GameManager::setButtonSize(float siz)
{
    if(hudLayer && isInMiddleOfGame){
        hudLayer->setButtonSize(siz);
    }
    UserDefault::getInstance()->setFloatForKey(KEY_BUTTON_SIZE, siz);
    GameManager::getInstance()->saveCoin();
}
float GameManager::getButtonSize()
{
    if (Director::getInstance()->getWinSize().width < 960) {
        return UserDefault::getInstance()->getFloatForKey(KEY_BUTTON_SIZE, 0.4);
    }else{
        return UserDefault::getInstance()->getFloatForKey(KEY_BUTTON_SIZE, 0.9);
    }
}
float GameManager::getMusicVolumn()
{
    float value = 0;
    float percent = SimpleAudioEngine::getInstance()->getBackgroundMusicVolume() * 100;
    if(percent > 30){
        value += 0.1;
        value += (percent - 30)*0.9f/70;
    }else{
        value = percent/300.0f;
    }
    
    return value;
}
float GameManager::getSoundVolumn(){
    return SimpleAudioEngine::getInstance()->getEffectsVolume();
}
bool GameManager::getNotificationOn(){
    return UserDefault::getInstance()->getBoolForKey(KEY_ALERT_ONOFF, true);
}
void GameManager::preLoadAllSoundEffect(){
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("Coin.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("Medicine.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("Spring.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("jump_sound.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("FireSmall.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("laser.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("MachineGun1.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("MachineGun2.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("DdalKack.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("powerUp45.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("fa.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("misol.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("rocketLaunch.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("noise37.wav");
    
    for (int i = 0; i < 9; i++) {
        int index = rand()%9;
        char buf[20];
        sprintf(buf, "footStep%d.wav", index);
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(buf);
    }
    
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("waterSplash.wav");
}
void GameManager::stopSoundEffect(int sound){
    CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(sustainSoundTag);
    sustainSoundTag = -1;
    /*switch(sound)
    {
        case SOUND_LASER_BUZZ:
        case SOUND_LASER_WOONG:
            CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(sustainSoundTag);
            break;
    }*/
}
long GameManager::getTodaysRandom(){
    int seed = SM->getCurrentTime()/(60*60*24);
    log("today's seed: %d", seed);
    return 1234 + seed;
}
float GameManager::getSoundVolumnByDistance(Node* target, Node* source){
    float distance = getDistance(target, this);
    float minDistance = 300;
    float maxDistance = 1100;
    float volumn = 0;
    if (distance < minDistance) {
        volumn = 1;
    }else if(distance > maxDistance){
        volumn = 0.4f;
    }else{
        float range = maxDistance - minDistance;
        volumn = (range - distance)/range;
    }
    return volumn;
}

void GameManager::loadFlagImage(ImageView* flag, int family){
    if (family == FAMILY_LIONSTAR) {
        flag->loadTexture("lionFlagSmall.png");
    }else if (family == FAMILY_STARK) {
        flag->loadTexture("wolfFlagSmall.png");
    }
}
void GameManager::runAnimation(Sprite* spt, std::string name, bool repeat){
    spt->stopActionByTag(ACTION_TAG_ANIMATION);
    Animation* animation = AnimationCache::getInstance()->getAnimation(name);
    Animate* animate = Animate::create(animation);
    if(repeat)
    {
        RepeatForever* forever = RepeatForever::create(animate);
        forever->setTag(ACTION_TAG_ANIMATION);
        spt->runAction(forever);
    }
    else
    {
        animate->setTag(ACTION_TAG_ANIMATION);
        spt->runAction(animate);
    }
}
void GameManager::splitUp(Node* layer){
    float dur = 0.7f;
    //    float delayDur = 0.3f;
    Point pos;
    for (int i = 0; i < layer->getChildren().size();i++){
        auto child = layer->getChildren().at(layer->getChildren().size() - 1 - i);
        pos = child->getPosition();
        if (rand()%2==0) {
            child->runAction(EaseIn::create(MoveTo::create(dur, Point(rand()%2==0?size.width*1.5f:-size.width/2, pos.y)), 2));
        }else{
            child->runAction(EaseIn::create(MoveTo::create(dur, Point(pos.x, rand()%2==0?size.height*1.5f:-size.height/2)), 2));
        }
    }
    layer->runAction(Sequence::create(DelayTime::create(dur + 0.1f), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, layer)), NULL));
}
void GameManager::gatherUp(Node* layer){
    float dur = 0.7f;
    float delayDur = 0.3f;
    Point pos;
    for (int i = 0; i < layer->getChildren().size();i++){
        auto child = layer->getChildren().at(layer->getChildren().size() - 1 - i);
        pos = child->getPosition();
        if (rand()%2==0) {
            child->setPosition(Point(rand()%2==0?size.width*1.5f:-size.width/2, pos.y));
        }else{
            child->setPosition(Point(pos.x, rand()%2==0?size.height*1.5f:-size.height/2));
        }
        
        child->runAction(Sequence::create(DelayTime::create(delayDur*(rand()%4)/4), EaseElasticOut::create(MoveTo::create(dur, pos), 2), NULL));
    }
}
float GameManager::getDistance(Node* target, Node* source){
    return sqrtf(powf(target->getPositionX() - getPositionX(), 2) + powf(target->getPositionY() - getPositionY(), 2));
}
void GameManager::stopBGM(){
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}
void GameManager::resumeBGM(){
    playBGM(lastBGM);
}
void GameManager::playBGM(int bgm){
//    return; // test
    lastBGM = bgm;
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    if(!UDGetBool(KEY_MUSIC_ONOFF, true)){
        return;
    }
    switch(bgm)
    {
        case BGM_GAMEOVER:
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("GameOver_Music.mp3", false);
            break;
        case BGM_REVIVE:
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("Revive_Music.mp3", false);
            break;
        case BGM_WIN:
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("Win_Music.mp3", false);
            break;
        case BGM_BRIGHT:
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("LegendDary2Bright.mp3", true);
            break;
        case BGM_NOVEMBER:
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("bensound-november.mp3", true);
            break;
        case BGM_BRIGHT_NO_EFFECT:
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("LegendDary2Bright_NoEffect.mp3", true);
            break;
    }
}
void GameManager::playSoundEffect(int sound, float gain, float pan){
//    return; // test
    if(!UDGetBool(KEY_SOUND_ONOFF, true)){
        return;
    }
    switch(sound)
    {
        case SOUND_GUN_FIRE_0:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("weapon-sound9_.wav", false, 1, 0, gain);
            break;
        case SOUND_GUN_FIRE_1:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("weapon-sound5_.wav", false, 1, 0, gain);
            break;
        case SOUND_GUN_FIRE_2:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("weapon-sound10_.wav", false, 1, 0, gain);
            break;
        case SOUND_RELOAD:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("gun_reload_long.mp3", false, 1, 0, gain);
            break;
        case SOUND_RELOAD_SHORT:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("gun_reload.wav", false, 1, 0, gain);
            break;
        case SOUND_COIN:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Coin.wav", false, 1, 0, gain);
            break;
        case SOUND_MEDICINE:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Medicine.wav", false, 1, 0, gain);
            break;
        case SOUND_SPRING:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Spring.wav", false, 1, 0, gain);
            break;
        case SOUND_JUMP:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("jump_sound.wav", false, 1, 0, gain);
            break;
        case SOUND_FIRE_SMALL:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("FireSmall.wav", false, 1, 0, gain);
            break;
        case SOUND_LASER:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("laser.wav", false, 1, 0, gain);
            break;
        case SOUND_MACHINE_GUN_1:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("MachineGun1.wav", false, 1, 0, gain);
            break;
        case SOUND_MACHINE_GUN_2:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("MachineGun2.wav", false, 1, 0, gain);
            break;
        case SOUND_DDALKACK:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("DdalKack.wav", false, 1, 0, gain);
            break;
        case SOUND_POWER_UP_45:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("powerUp45.wav", false, 1, 0, gain);
            break;
        case SOUND_ROCKET_LAUNCH:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("rocketLaunch.wav", false, 1, 0, gain);
            break;
        case SOUND_NOISE_37:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("noise37.wav", false, 1, 0, gain);
            break;
        case SOUND_FA:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("fa.wav", false, 1, 0, gain);
            break;
        case SOUND_MISOL:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("misol.wav", false, 1, 0, gain);
            break;
        case SOUND_LASER_BUZZ:
            if(sustainSoundTag >= 0){
                CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(sustainSoundTag);
            }
            sustainSoundTag = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("laserBuzz.mp3", true, 1, 0, gain);
            break;
        case SOUND_FOOT_STEP:
        {
            int index = time(0)%9;
            char buf[20];
            sprintf(buf, "footStep%d.wav", index);
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(buf, false, 1, 0, 0.3f);
        }
            break;
        case SOUND_WATER_SPLASH:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("waterSplash.wav", false, 1, 0, gain);
            break;
        case SOUND_SMALL_EXPLOSION:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("smallExplosion.wav", false, 1, 0, gain);
            break;
        case SOUND_LIGHTNING:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("lightningMagic2.mp3", false, 1, 0, gain);
            break;
        case SOUND_LASER_WOONG:
            if(sustainSoundTag >= 0){
                CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(sustainSoundTag);
            }
            sustainSoundTag = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("LaserWoong.mp3", true, 1, 0, gain);
            break;
        case SOUND_TOONG:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("toong.wav", false, 1, 0, gain);
            break;
        case SOUND_KOONG:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("koong.mp3", false, 1, 0, gain);
            break;
        case SOUND_STONE_ROLL:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("stone_roll.wav", false, 1, 0, gain);
            break;
        case SOUND_STICK_SOUND:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("stickSoundInCave.mp3", false, 1, pan, gain);
            break;
        case SOUND_WOOD_DRAP:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("wood_drop.wav", false, 1, pan, gain);
            break;
        case SOUND_WOOD_CRASH:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("wood_crash.wav", false, 1, pan, gain);
            break;
        case SOUND_GLASS_BREAK:
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("glassBreak.mp3", false, 1, pan, gain);
            break;
        case SOUND_GIGGLING:
            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("dungeonNoise.mp3", true);
            break;
        case SOUND_THUNDER:
            sustainSoundTag = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("lightning.mp3", false, 1, pan, gain);
            break;
        case SOUND_DUNGEON_NOISE:
            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("dungeonNoise.mp3", true);
            break;
	}
}
void GameManager::makeLabelEllipsis(Label* lbl, float width){
    std::string originalStr = lbl->getString();
    int index = originalStr.size();
    while(lbl->getContentSize().width > width){
        index--;
        lbl->setString(originalStr.substr(0, index) + "...");
    }
}
Point GameManager::getGemCountPosition(){
    Size size = Director::getInstance()->getWinSize();
    return Point(size.width/2 + 190, size.height - 30);
}


Layer* GameManager::getShopLayer()
{
//    if (!shopLayer) {
//        shopLayer = ShopLayer::create();
//        shopLayer->retain();
//    }
    return NULL;//shopLayer;
}
void GameManager::replaceAll(std::string& source, const std::string& from, const std::string& to)
{
    std::string newString;
    newString.reserve(source.length());  // avoids a few memory allocations
    
    std::string::size_type lastPos = 0;
    std::string::size_type findPos;
    
    while(std::string::npos != (findPos = source.find(from, lastPos)))
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }
    
    // Care for the rest after last occurrence
    newString += source.substr(lastPos);
    
    source.swap(newString);
}

HelloWorld* GameManager::getCurrentStageLayer()
{
    return currentStageLayer;
}
void GameManager::setCurrentStageLayer(HelloWorld* layer)
{
    currentStageLayer = layer;
}
DualWorld* GameManager::getCurrentDualLayer()
{
    return currentDualLayer;
}
void GameManager::setCurrentDualLayer(DualWorld* layer)
{
    currentDualLayer = layer;
}
float GameManager::getWeaponPowerRuneResearchApplied(int value){
    float attackBuff = 1;
    for (int i = 0; i < 4; i++) {
        int rune = UDGetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, i).c_str(), -1);
        if(rune > 0){
            if(rune%4 == value%4 && GM->getRuneBuffType(rune) == RUNE_BUFF_TYPE_ATTACK){
                attackBuff += GM->getRuneBuff(rune/1000, RUNE_BUFF_TYPE_ATTACK);
            }
        }
    }
    return GM->getWeaponPower(value%1000, value/1000)*(1 + attackBuff*0.01f)*(1 + GM->getResearchPower()*0.01f);
}
float GameManager::getWeaponCriticalPossibilityRuneResearchApplied(int value){
    float criticalPossibility = 0;
    for (int i = 0; i < 4; i++) {
        int rune = UDGetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, i).c_str(), -1);
        if(rune > 0){
            if(rune%4 == value%4 && GM->getRuneBuffType(rune) == RUNE_BUFF_TYPE_CRI){
                criticalPossibility += GM->getRuneBuff(rune/1000, RUNE_BUFF_TYPE_CRI);
            }
        }
    }
    return DEFAULT_CRITICAL + criticalPossibility ;//+ getResearchCriticalPossibility();
}
float GameManager::getWeaponPower(int index, int rank){
    index = index%1000;
    return GM->getWeaponPower(index)*(1 + rank*0.1f);
}
int GameManager::getWeaponPower(int weaponType){
    int damage = 10;
    switch (weaponType) {
        case WEAPON_PISTOL:
            damage = 11;
            break;
        case WEAPON_MAGNUM:
            damage = 8;
            break;
        case WEAPON_UZI:
            damage = 5;
            break;
        case WEAPON_RIFLE:
            damage = 22;
            break;
        case WEAPON_FIRE_BOTTLER:
            damage = 36;
            break;
        case WEAPON_ANTI_BAT:
            damage = 11;
            break;
        case WEAPON_SNIPER_RIFLE:
            damage = 45;
            break;
        case WEAPON_RIFLE_GRENADE:
            damage= 32;
            break;
        case WEAPON_CROSS_BOW:
            damage = 17;
            break;
        case WEAPON_MACHINE_GUN:
            damage = 4;
            break;
        case WEAPON_STRIKER:
            damage = 18;
            break;
        case WEAPON_NIFE_THROWER:
            damage = 15;
            break;
        case WEAPON_BALL_SHOOTER:
            damage= 12;
            break;
        case WEAPON_FLAME_THROWER:
            damage = 5;
            break;
        case WEAPON_ROCKET_LAUNCHER:
            damage = 110;
            break;
        case WEAPON_BAZOOKA:
            damage = 255;
            break;
        case WEAPON_MATT_A4:
            damage = 34;
            break;
        case WEAPON_LASER_GUN:
            damage = 3;
            break;
        case WEAPON_ENERGY_GUN:
            damage= 50;
            break;
        case WEAPON_GUIDED_ROCKET_LAUNCHER:
            damage = 70;
            break;
        case WEAPON_SLUGGER:
            damage = 45;
            break;
        case WEAPON_DRAGON_RIFLE:
            damage = 55;
            break;
        case WEAPON_SHARK_GUN:
            damage = 160;
            break;
        case WEAPON_SLUGGER_NG:
            damage = 60;
            break;
        case WEAPON_LIGHTNING_GUN:
            damage = 35;
            break;
        case WEAPON_LASER_COIN:
            damage = 3;
            break;
        default:
            break;
    }
    int level = GameManager::getInstance()->getWeaponPowerLevel(weaponType);
    if (damage > 10) {
        damage = damage + damage*(0.2*(level));// + rand()%(int)(damage*0.2f);
    }
    
    return damage;
}
/*
bool GameManager::isShieldPurcahsed(){
    const time_t rawtime = (const time_t)TimeManager::getInstance()->webTime;
    const time_t shieldTime = (const time_t)GameManager::getInstance()->getShieldPurchasedTime();
    int time = (int)difftime(shieldTime, rawtime);
    
    return UserDefault::getInstance()->getBoolForKey(KEY_SHIELD, false) && TimeManager::getInstance()->timeEstablished && time > 0;
}
void GameManager::setShield(long time){
    UserDefault::getInstance()->setBoolForKey(KEY_SHIELD, true);
    UserDefault::getInstance()->setDoubleForKey(KEY_SHIELD_TIME, time + 60*60*24);
    UserDefault::getInstance()->flush();
}
long GameManager::getShieldPurchasedTime(){
    return UserDefault::getInstance()->getDoubleForKey(KEY_SHIELD_TIME, 0);
}*/
Missile* GameManager::getBullet(int weaponType, int playerMissileDemage){
    const char* name;
    int unitType = UNIT_MISSILE_STRAIGHT;
    int extraSpeed = 180;
    
    switch (weaponType) {
        case WEAPON_PISTOL:
            name = "msPistol.png";
            break;
        case WEAPON_MAGNUM:
            name = "msMagnum.png";
            extraSpeed = 220;
            break;
        case WEAPON_UZI:
            name = "msUzi.png";
            break;
        case WEAPON_SNIPER_RIFLE:
            name = "msSniperRifle.png";
            extraSpeed = 340;
            break;
        case WEAPON_SLUGGER:
        case WEAPON_SLUGGER_NG:
            name = "msSlugger.png";
            break;
        case WEAPON_ROCKET_LAUNCHER:
            name = "msRocketLauncher.png";
            extraSpeed = 90;
            break;
        case WEAPON_RIFLE_GRENADE:
            name = "msRifleGrenade.png";
            extraSpeed = 140;
            break;
        case WEAPON_RIFLE:
            name = "msRifle.png";
            extraSpeed = 280;
            break;
        case WEAPON_MATT_A4:
            name = "msMatt-A4.png";
            break;
        case WEAPON_NIFE_THROWER:
            name = "nifeThrower.png";
            break;
        case WEAPON_MACHINE_GUN:
            name = "msMachineGun.png";
            break;
        case WEAPON_LASER_GUN:
            name = "msLaserGun.png";
            extraSpeed = 280;
            break;
        case WEAPON_GUIDED_ROCKET_LAUNCHER:
            name = "msGuidedRocketLauncher.png";
            extraSpeed = 90;
            break;
        case WEAPON_FIRE_BOTTLER:
            name = "msBottler.png";
            break;
        case WEAPON_ENERGY_GUN:
            name = "msEnergyGun.png";
            extraSpeed = 90;
            break;
        case WEAPON_DRAGON_RIFLE:
            name = "msDragonRifle0.png";
            extraSpeed = 110;
            break;
        case WEAPON_CROSS_BOW:
            name = "msCrossBow.png";
            break;
        case WEAPON_BAZOOKA:
            name = "msBazooka.png";
            extraSpeed = 100;
            break;
        case WEAPON_BALL_SHOOTER:
            name = "msBallShooter.png";
            break;
        case WEAPON_FLAME_THROWER:
            name = "msFlameThrower.png";
            break;
        case WEAPON_ANTI_BAT:
            name = "msFire1.png";
            break;
        case WEAPON_STRIKER:
            name = "msStriker.png";
            break;
        case WEAPON_SHARK_GUN:
            name = "msShark.png";
            break;
        default:
            break;
    }
    
    Missile* ms = Missile::createMissile(unitType,
                                             playerMissileDemage,
                                             0,
                                             extraSpeed,
                                             name);
    if(weaponType == WEAPON_ENERGY_GUN){
        ms->isPiercing = true;
    }
//    RepeatForever* repeat = RepeatForever::create(RotateBy::create(0.1, 360));
//    ((Sprite*)ms)->runAction(repeat);
    
    switch (weaponType) {
        case WEAPON_PISTOL:
            break;
        case WEAPON_MAGNUM:
            break;
        case WEAPON_UZI:
            break;
        case WEAPON_SNIPER_RIFLE:
            break;
        case WEAPON_SLUGGER:
        case WEAPON_SLUGGER_NG:
            break;
        case WEAPON_ROCKET_LAUNCHER:
            break;
        case WEAPON_RIFLE_GRENADE:
            break;
        case WEAPON_RIFLE:
            break;
        case WEAPON_MATT_A4:
            break;
        case WEAPON_NIFE_THROWER:
            ms->runAction(RepeatForever::create(RotateBy::create(0.1, 360)));
            break;
        case WEAPON_MACHINE_GUN:
            break;
        case WEAPON_LASER_GUN:
            break;
        case WEAPON_GUIDED_ROCKET_LAUNCHER:
            break;
        case WEAPON_FIRE_BOTTLER:
            ms->runAction(RepeatForever::create(RotateBy::create(0.7, 360)));
            break;
        case WEAPON_ENERGY_GUN:
            makeItSiluk(ms);
            break;
        case WEAPON_DRAGON_RIFLE:
            ms->runAnimation("msDragonRifle", true);
            break;
        case WEAPON_CROSS_BOW:
            break;
        case WEAPON_BAZOOKA:
            break;
        case WEAPON_BALL_SHOOTER:
            ms->runAction(RepeatForever::create(RotateBy::create(0.5, 360)));
            break;
        case WEAPON_ANTI_BAT:
            break;
        case WEAPON_STRIKER:
            break;
        default:
            break;
    }
    
    return ms;
}
bool GameManager::getWeaponUnlocked(int weaponType){
    bool unlocked = false;

    switch (weaponType) {
        case WEAPON_PISTOL:
            return true;
        case WEAPON_MAGNUM:
            return UserDefault::getInstance()->getBoolForKey(ldMagnumUnlockedKey, false);
        case WEAPON_UZI:
            return UserDefault::getInstance()->getBoolForKey(ldMagnumUnlockedKey, false);
        case WEAPON_SNIPER_RIFLE:
            return UserDefault::getInstance()->getBoolForKey(ldUziUnlockedKey, false);
        case WEAPON_SLUGGER:
            return UserDefault::getInstance()->getBoolForKey(ldSluggerUnlockedKey, false);
        case WEAPON_SLUGGER_NG:
            return UserDefault::getInstance()->getBoolForKey(ldSluggerNGUnlockedKey, false);
        case WEAPON_ROCKET_LAUNCHER:
            return UserDefault::getInstance()->getBoolForKey(ldRocketLauncherUnlockedKey, false);
        case WEAPON_RIFLE_GRENADE:
            return UserDefault::getInstance()->getBoolForKey(ldRifleGrenadeUnlockedKey, false);
        case WEAPON_RIFLE:
            return UserDefault::getInstance()->getBoolForKey(ldRifleUnlockedKey, false);
        case WEAPON_MATT_A4:
            return UserDefault::getInstance()->getBoolForKey(ldMattA4UnlockedKey, false);
        case WEAPON_NIFE_THROWER:
            return UserDefault::getInstance()->getBoolForKey(ldNifeThrowerUnlockedKey, false);
        case WEAPON_MACHINE_GUN:
            return UserDefault::getInstance()->getBoolForKey(ldMachineGunUnlockedKey, false);
        case WEAPON_LASER_GUN:
            return UserDefault::getInstance()->getBoolForKey(ldLaserGunUnlockedKey, false);
        case WEAPON_GUIDED_ROCKET_LAUNCHER:
            return UserDefault::getInstance()->getBoolForKey(ldGuidedRocketLauncherUnlockedKey, false);
        case WEAPON_FIRE_BOTTLER:
            return UserDefault::getInstance()->getBoolForKey(ldFireBottlerUnlockedKey, false);
        case WEAPON_ENERGY_GUN:
            return UserDefault::getInstance()->getBoolForKey(ldEnergyGunUnlockedKey, false);
        case WEAPON_DRAGON_RIFLE:
            return UserDefault::getInstance()->getBoolForKey(ldDragonRifleUnlockedKey, false);
        case WEAPON_CROSS_BOW:
            return UserDefault::getInstance()->getBoolForKey(ldCrossBowUnlockedKey, false);
        case WEAPON_BAZOOKA:
            return UserDefault::getInstance()->getBoolForKey(ldBazookaUnlockedKey, false);
        case WEAPON_BALL_SHOOTER:
            return UserDefault::getInstance()->getBoolForKey(ldBallShooterUnlockedKey, false);
        case WEAPON_ANTI_BAT:
            return UserDefault::getInstance()->getBoolForKey(ldAntiBatUnlockedKey, false);
        case WEAPON_STRIKER:
            return UserDefault::getInstance()->getBoolForKey(ldStrikerUnlockedKey, false);
        default:
            break;
    }

    
    return unlocked;
}

bool GameManager::getCharacterUnlocked(int costume){
    bool unlocked = false;
    switch (costume) {
        case UNIT_DARY:
            return true;
            break;
//        case COSTUME_MITHRIL:
//            return UserDefault::getInstance()->getBoolForKey(getCharacterUnlockedKey(CHARACTER_MITHRIL), false);
//            break;
            
        default:
            break;
    }
    return unlocked;
}

const char* GameManager::getCharacterUnlockedKey(int costume){
    switch (costume) {
        case UNIT_DARY:
            return ldCharacterDaryUnlockKey;
            break;
            
        default:
            break;
    }
    return ldCharacterDaryUnlockKey;
}


const char* GameManager::getCharacterName(int costume){
    
    return "Dary";
}

const char* GameManager::getCharacterDesc(int costume){
   
    
    return "Dary";
}




void GameManager::showSpriteExplosion(Node* parent, const char* sptName, Vec2 pos, int sptCount, float radius, float dur, bool isInPlist){
    Sprite* spt;
    for (int i = 0; i < sptCount; i++) {
        if(isInPlist){
            spt = Sprite::createWithSpriteFrameName(sptName);
        }else{
            spt = Sprite::create(sptName);
        }
        parent->addChild(spt);
        spt->setPosition(pos);
        int angle = rand()%360;
        spt->setRotation(angle);
        float radian = angle*3.14f/180;
        //        dur = dur + dur*(rand()%10)*0.1f;
        spt->runAction(EaseOut::create(MoveBy::create(dur, Vec2(cos(radian)*radius, sin(radian)*radius)), 2));
//        spt->runAction(RotateBy::create(dur, -180 + rand()%360));
        spt->runAction(Sequence::create(DelayTime::create(dur*2/3), FadeOut::create(dur/3), SPT_REMOVE_FUNC, NULL));
        spt->runAction(ScaleTo::create(dur, 0.5f));
    }
}
void GameManager::showParticleExplosion(Node* prt, const char* sptName, Point pos, float scale){
    ParticleExplosion* particle = ParticleExplosion::create();
    particle->setPosition(pos);        // 위치
    particle->setLife(0.4*scale);                          // 지속시간
    particle->setLifeVar(0.3*scale);
    particle->setSpeed(70);                      // 속도
    particle->setStartSize(scale);
    particle->setTotalParticles(15);
    particle->setStartColor(Color4F(205, 205, 0, 155));
    particle->setEndColor(Color4F(205, 205, 0, 0));
    particle->setTexture(Director::getInstance()->getTextureCache()->addImage(sptName));
    prt->addChild(particle);
}
void GameManager::pushLayer(Layer* parent, Layer* layer){
//    disableLayer(parent);
//    parent->addChild(layer, 1000);
    animateFadeIn(layer, parent);
}

void GameManager::pushLayerWithoutDisable(Layer* parent, Layer* layer){
    //    parent->addChild(layer, 1000);
    animateFadeIn(layer, parent);
}
void GameManager::googleSignInOrOut(bool sign){
    if(sign){
        //        MessageBox("Signed in", "Google Play Service");
//        GameSharing::signIn();
        CCLOG("** google Signed in");
    }else{
        //        MessageBox("Signed out", "Google Play Service");
        CCLOG("** google not signed in");
    }
    if (settingLayer != NULL) {
        ((SettingLayer*)settingLayer)->changeLoginState(sign);
    }

//    if(titleLayer != NULL && ((TitleLayer*)titleLayer)->titleMain != NULL){
//        ((TitleLayer*)titleLayer)->removeSignInButton();
//    }
    isGoogleSigned = sign;
    
}
void GameManager::animateFadeIn(Node* layer, Node* parent){
    parent->addChild(layer, 1000);
    layer->setScale(0.1f);
    layer->runAction(Sequence::create(ScaleTo::create(0.2, 1.1), ScaleTo::create(0.05, 0.95),ScaleTo::create(0.05, 1), NULL));
    
    Size size = Director::getInstance()->getWinSize();
    Sprite* blackBack = Sprite::create("blackSquare.png");
    parent->addChild(blackBack, 999);
    blackBack->setTag(7899);
    blackBack->setScaleX(size.width);
    blackBack->setScaleY(size.height);
    blackBack->setPosition(Point(size.width/2, size.height/2));
}
void GameManager::animateFadeOut(Node* layer){
//    layer->setScale(0.1f);
    layer->runAction(Sequence::create(ScaleTo::create(0.06, 0.85),ScaleTo::create(0.1, 1.2), ScaleTo::create(0.1, 0.1), CallFuncN::create(CC_CALLBACK_1(GameManager::animationFadeOutDone, this)), NULL));
}

void GameManager::animationFadeInDone(Node* layer){
//    CCLOG("fade in done");
}
void GameManager::animationFadeOutDone(Node* layer){
    Layer* parent = (Layer*)layer->getParent();
    parent->removeChildByTag(7899);
    layer->removeFromParentAndCleanup(true);
    CCLOG("layer removed");
//    enableLayer(parent);
}
/*
void GameManager::disableLayer(Layer* layer){
    layer->setTouchEnabled(false);
//    CCLOG("layer retain count: %d", layer->retainCount());
    CCMenu* menu;
    Layer* childLayer;
//    CCLOG("disable layer children count: %d", layer->getChildren().size());
    for (int i = 0; i < layer->getChildren().size(); i++) {
        menu = dynamic_cast<CCMenu*>(layer->getChildren().at(i));
        if (menu) {
            if (!menu->isEnabled()) {
                Vector<Droppable*>disabledMenusArray = dynamic_cast<CCArray*>((CCArray*)layer->getUserData());
                if (disabledMenusArray == NULL) {
                    disabledMenusArray = CCArray::create();
                    disabledMenusArray->retain();
                    layer->setUserData((void*)disabledMenusArray);
                }
                disabledMenusArray.pushBack(menu);
            }else{
                menu->setEnabled(false);
            }
        }else{
            childLayer = dynamic_cast<Layer*>(layer->getChildren().at(i));
            if (childLayer != NULL) {
                disableLayer(childLayer);
            }
        }
    }
}

void GameManager::popLayer(Layer* layer){
    animateFadeOut(layer);
}

void GameManager::popLayerWithoutAnimation(Layer* layer){
    animationFadeOutDone(layer);
}

void GameManager::enableLayer(Layer* layer){
    layer->setTouchEnabled(true);
    CCMenu* menu;
    Layer* childLayer;
    Vector<Droppable*>disabledMenusArray = NULL;
    for (int i = 0; i < layer->getChildren().size(); i++) {
        menu = dynamic_cast<CCMenu*>(layer->getChildren().at(i));
        if (menu) {
            disabledMenusArray = dynamic_cast<CCArray*>((CCArray*)layer->getUserData());
            
            if (disabledMenusArray != NULL && disabledMenusArray->containsObject(menu)) {
                
            }else{
                menu->setEnabled(true);
            }
        }
        childLayer = dynamic_cast<Layer*>(layer->getChildren().at(i));
        if (childLayer != NULL) {
            enableLayer(childLayer);
        }
    }

    if (disabledMenusArray != NULL) {
        disabledMenusArray->release();
        layer->setUserData(NULL);
    }
}*/


long GameManager::strToLong(const char* str){
    std::stringstream strValue;
    strValue << str;
    
    long longValue;
    strValue >> longValue;
    
    return longValue;
}
Sprite* GameManager::getGrayScaleImage(const char* src){
    Image *img=  new Image();
    img->initWithImageFile(src);

//    II) From Sprite :
    
//    II.1) Sprite -> RenderTexture2D
    
//    II.2) RenderTexture2D -> Image (Image *testImage = RenderText2D->newImage();)
    
//    2.Then You can do what you need :
    
//        Image *img= ... // make Image from Sprite
        int x=3;
    if(img->hasAlpha())
        x=4;
    
    unsigned char *data = new unsigned char[img->getDataLen()*x];
    data = img->getData();
    // [0][0] => Left-Top Pixel !
    // But cocos2d Location Y-axis is Bottom(0) to Top(max)
    
//    unsigned char *newData = new unsigned char[img->getDataLen()*x];
    
    for(int i=0;i<img->getWidth();i++)
    {
        for(int j=0;j<img->getHeight();j++)
        {
            unsigned char *pixel = data + (i + j * img->getWidth()) * x;
            
            // You can see/change pixels' RGBA value(0-255) here !
            unsigned char r = *pixel;
            unsigned char g = *(pixel + 1);
            unsigned char b = *(pixel + 2);
//            unsigned char a = *(pixel + 3);
            
            *pixel = (r+g+b)/3;
            *(pixel + 1) = (r+g+b)/3;
            *(pixel + 2) = (r+g+b)/3;
        }
    }
//    3.Then, convert it to texture_2D
    //Image -> Texture2d
    Texture2D* texture_2D= new Texture2D();
    texture_2D->initWithImage(img);
//    4.And Finally back to Sprite
    return Sprite::createWithTexture(texture_2D);
}

Sprite* GameManager::getSpriteShapeSolidColorSprite(const char* src, Color3B color){
    Image *img=  new Image();
    img->initWithImageFile(src);

    int x=3;
    if(img->hasAlpha())
        x=4;
    
    unsigned char *data = new unsigned char[img->getDataLen()*x];
    data = img->getData();
    
    for(int i=0;i<img->getWidth();i++)
    {
        for(int j=0;j<img->getHeight();j++)
        {
            unsigned char *pixel = data + (i + j * img->getWidth()) * x;
            
            if(img->hasAlpha())
            {
                if(*(pixel + 3) == 0){
                    continue;
                }
            }
            *pixel = color.r;
            *(pixel + 1) = color.g;
            *(pixel + 2) = color.b;
        }
    }
    Texture2D* texture_2D= new Texture2D();
    texture_2D->initWithImage(img);

    return Sprite::createWithTexture(texture_2D);
}
//void GameManager::SendAchievement(const char* text, int count, int goal){
//    const char* theText;
//    if (GameManager::getInstance()->market == MARKET_MAC) {
//        theText = __String::createWithFormat("%s_Mac", text)->getCString();
//    }else{
//        theText = text;
//    }
//
//}
int GameManager::getWeaponPrice(int index){
    if(index == 0) return 200;
    else if(index == 1) return 1000;
    else if(index == 2) return 1500;
    else if(index == 3) return 2000;
    else if(index == 4) return 2500;
    else if(index == 5) return 2900;
    else if(index == 6) return 3500;
    else if(index == 7) return 4000;
    else if(index == 8) return 5000;
    else if(index == 9) return 5800;
    else if(index == 10) return 6500;
    else if(index == 11) return 7500;
    else if(index == 12) return 8000;
    else if(index == 13) return 9300;
    else if(index == 14) return 10900;
    else if(index == 15) return 12500;
    else if(index == 16) return 13000;
    else if(index == 17) return 14500;
    else if(index == 18) return 15800;
    else if(index == 19) return 17000;
    else if(index == 20) return 55000;
    else if(index == 21) return 99000;
    else if(index == 22) return 92000;
    
    int priceUnit = 500;
    return index*priceUnit;
}
std::string GameManager::getItemName(int value){
    int index = value%1000;
    if(index < SHIELD_BEGIN){
        return getWeaponName(index);
    }else if(index >= SHIELD_BEGIN && index < RUNE_BEGIN){
        return getShieldName(index-SHIELD_BEGIN);
    }else{
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
        return strmake("%s (%s)", LM->getText("rune").c_str(), LM->getText(color.c_str()).c_str());
    }
    return "undefined";
}
std::string GameManager::getShieldName(int index){
    return LanguageManager::getInstance()->getText(StringUtils::format("shieldName%d", index).c_str());
}
std::string GameManager::getWeaponName(int index){
    return LanguageManager::getInstance()->getText(StringUtils::format("weaponName%d", index).c_str());
//    if (index == 0) {
//        return "PISTOL";
//    }else if(index == 1) {
//        return "MAGNUM";
//    }else if(index == 2) {
//        return "UZI";
//    }else if(index == 3) {
//        return "RIFLE";
//    }else if(index == 4) {
//        return "FIRE BOTTLER";
//    }else if(index == 5) {
//        return "ANTI BAT";
//    }else if(index == 6) {
//        return "SNIPER RIFLE";
//    }else if(index == 7) {
//        return "GRENADE RIFLE";
//    }else if(index == 8) {
//        return "CROSS BOW";
//    }else if(index == 9) {
//        return "MACHINE GUN";
//    }else if(index == 10) {
//        return "STRIKER";
//    }else if(index == 11) {
//        return "NIFE THROWER";
//    }else if(index == 12) {
//        return "BALL SHOOTER";
//    }else if(index == 13) {
//        return "FLAME THROWER";
//    }else if(index == 14) {
//        return "ROCKET LAUNCHER";
//    }else if(index == 15) {
//        return "BAZOOKA";
//    }else if(index == 16) {
//        return "MATT-A4";
//    }else if(index == 17) {
//        return "LASER GUN";
//    }else if(index == 18) {
//        return "ENERGY GUN";
//    }else if(index == 19) {
//        return "G.R.L";
//    }else if(index == 20) {
//        return "SLUGGER";
//    }else if(index == 21) {
//        return "R.DRAGON";
//    }else if(index == 22) {
//        return "SLUGGER NG";
//    }else if(index == 22) {
//        return "SLUGGER NG";
//    }
//    return "";
}
void GameManager::showInterstitialAds(){
    if (UserDefault::getInstance()->getBoolForKey(KEY_IAP_USER, false)) {
//        return;
    }
    if (UserDefault::getInstance()->getBoolForKey(KEY_REMOVE_ADS_GET, false)) {
        return;
    }
    int adsShowCount = 1;
    int count = UserDefault::getInstance()->getIntegerForKey(KEY_ADS_COUNTER, adsShowCount);
    count++;
    if (count >= adsShowCount) {
        count = 0;
//        NativeInterface::NativeInterfaceshowInterstitial();
    }
    UserDefault::getInstance()->setIntegerForKey(KEY_ADS_COUNTER, count);
}
void GameManager::showVideo(int whichVideo){
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    videoIndex = whichVideo;
    GameSharing::showRewardedVideoAds();
}
void GameManager::showVideoDone(){
    log("video done");
    if (videoIndex == VIDEO_GAME_OVER) {
        getHudLayer()->videoDone();
    }else if(videoIndex == VIDEO_WEAPON_RENT){
        
    }else if(videoIndex == VIDEO_GAME_CONTINUE){
        hudLayer->reviveFromVideo();
        hudLayer->showPauseLayer();
    }else{
        getHudLayer()->videoDone();
    }
    
//    ((TitleLayer*)titleLayer)->showVideoDone();
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
bool GameManager::checkDataSecure(){
    int total = 0;
    total += (int)UserDefault::getInstance()->getBoolForKey(KEY_STARTER_PACK_PURCHASED, false);
    
    // Stage
    for (int th = 0; th < totalThemeCount; th++) {
        for (int st = 0; st < 24; st++) {
            // star
            char buf[50];
            sprintf(buf, KEY_STAR_COUNT_FORMAT, th, st);
            total += UserDefault::getInstance()->getIntegerForKey(buf, 0)*7;

            // mastery
            char masterBuf[50];
            sprintf( masterBuf, KEY_STAGE_MASTER_FORMAT, th*24 + st);
            total += UserDefault::getInstance()->getIntegerForKey(masterBuf, 0)*7;
        }
    }
    
    // coin
    total += getCoin();
    total += 479;
    total *= 571;
    
    // laset clear
    total += (UserDefault::getInstance()->getIntegerForKey(KEY_LAST_CLEAR_STAGE, 0) + 7)*17;
    
    // Exp
    total += UserDefault::getInstance()->getIntegerForKey("GExp", 0)*3;
    
    // Weapon
    char wbuf[25];
    for (int i = 0; i < TOTAL_WEAPON_COUNT; i++) {
        sprintf(wbuf, KEY_WEAPON_PURCHASED_FORMAT, i);
        total += UserDefault::getInstance()->getBoolForKey(wbuf, false)*3;
        
        // Weapon Power
        total += UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), 0)*5;
        
        // Weapon Critical
        total += UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), 0)*2;
    }
    
    // Pet
    for (int i = 0; i < 100; i++) {
        char pbuf[50];
        sprintf( pbuf, KEY_PET_FORMAT, i);
        total += UserDefault::getInstance()->getIntegerForKey(pbuf, PET_NONE)*9;
        
        char plbuf[50];
        sprintf( plbuf, KEY_PET_LEVEL_FORMAT, i);
        total += UserDefault::getInstance()->getIntegerForKey(plbuf, 1)*11;
    }
    
    // Character
    for (int i = 0; i < TOTAL_CHARACTER_COUNT; i++) {
        char buf[25];
        sprintf(buf, KEY_CHARACTER_PURCHASED_FORMAT, i);
        total += UserDefault::getInstance()->getBoolForKey(buf, false)*19;
    }
    
//    CCLOG("Data total: %d", total);
    
    int tn = 797;
    int tnt = 9;
    int shn = total += DATA_SECURE_OFFSET;
//    CCLOG("Data shn: %d", total);
    int data = (shn/tn)*331 + (shn%tnt)*619;
    
//    CCLOG("Data log: %d", data);
    
    
    return true;
}
void GameManager::showVideoFailed(){
    if (getHudLayer()) {
        getHudLayer()->videoFailed();
    }
//    ((TitleLayer*)titleLayer)->showVideoFailed();
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
void GameManager::clickAdsFailed(){
    
}
void GameManager::clickAdsDone(){
    
}
void GameManager::buyItem(int index){
    iapFlag = IAP_FLAG_INIT;
    std::string strItem = "";
    if (index >= IAP_STARTER_0 && index <= IAP_STARTER_2) {
        strItem = strmake("mg.starter0%d", index - IAP_STARTER_0 + 1).c_str();
    }else if (index >= IAP_PACKAGE_0 && index <= IAP_PACKAGE_1) {
        strItem = strmake("mg.package0%d", index - IAP_PACKAGE_0 + 1).c_str();
    }else if (index >= IAP_GEM_0 && index <= IAP_GEM_4) {
        strItem = strmake("mg.gem0%d", index - IAP_GEM_0 + 1).c_str();
    }else if (index >= IAP_TICKET_0 && index <= IAP_TICKET_0) {
        strItem = strmake("mg.ticket0%d", index - IAP_TICKET_0 + 1).c_str();
    }
    iapDetail = strItem;
    if (developer) {
        HUD->giveAwayIAPForTest();
        return;
    }
    
//    iapFlag = IAP_STATE_IN_PROCESS;
    GameSharing::buyItem(strItem);
    
}
void GameManager::addConsumedItem(std::string strSkuName){
    log("consumed name: %s", strSkuName.c_str());
    consumeSkuNameList.push_back(strSkuName);
}
void GameManager::onRestored(std::string strSkuID){
//    if(strSkuID.compare(IAP_DETAIL_CHAPTER2) == 0){
//        UDSetBool(KEY_CHAPTER_2_PURCHASED, true);
//    }else if(strSkuID.compare(IAP_DETAIL_PREMIUM_RETRY) == 0){
//        UDSetBool(KEY_PREMIUM_START, true);
//    }
    GM->iapFlag = IAP_FLAG_RESTORED;
}
/*
RenderTexture* GameManager::createAdditiveBorder( Sprite* label, int size, Color3B color, GLubyte opacity )
{
    RenderTexture* rt = RenderTexture::create(label->getTexture()->getContentSize().width + size * 2,
                                                  label->getTexture()->getContentSize().height+size * 2
                                                  );
    
    Point originalPos = label->getPosition();
    
    Color3B originalColor = label->getColor();
    
    GLubyte originalOpacity = label->getOpacity();
    
    bool originalVisibility = label->isVisible();
    
    label->setColor(color);
    
    label->setOpacity(opacity);
    
    label->setVisible(true);
    
    ccBlendFunc originalBlend = label->getBlendFunc();
    
    ccBlendFunc bf = {GL_SRC_ALPHA, GL_ONE}; // GL_SRC_ALPHA for glow effect
    label->setBlendFunc(bf);
    
    Point bottomLeft = Point(
                             label->getTexture()->getContentSize().width * label->getAnchorPoint().x + size,
                             label->getTexture()->getContentSize().height * label->getAnchorPoint().y + size);
    
    Point positionOffset= Point(
                                label->getTexture()->getContentSize().width  * label->getAnchorPoint().x - label->getTexture()->getContentSize().width / 2,
                                label->getTexture()->getContentSize().height * label->getAnchorPoint().y - label->getTexture()->getContentSize().height / 2);
    
    
    Point position = ccpSub(originalPos, positionOffset);
    
    //rt->getSprite()->getTexture()->setAntiAliasTexParameters();
    rt->begin();
    
    for (int i=0; i<360; i+= 15) // you should optimize that for your needs
    {
        label->setPosition(Point(bottomLeft.x + sin(CC_DEGREES_TO_RADIANS(i))*size, bottomLeft.y + cos(CC_DEGREES_TO_RADIANS(i))*size)
                           );
        label->visit();
    }
    rt->end();
    
    label->setPosition(originalPos);
    label->setColor(originalColor);
    label->setBlendFunc(originalBlend);
    label->setVisible(originalVisibility);
    label->setOpacity(originalOpacity);
    
    rt->setPosition(position);
    
    return rt;
}


RenderTexture* GameManager::createAdditive( Sprite* label, Color3B color, GLubyte opacity, int additiveCount)
{
    RenderTexture* rt = RenderTexture::create(label->getTexture()->getContentSize().width ,
                                                  label->getTexture()->getContentSize().height);
    
    Point originalPos = label->getPosition();
    
    Color3B originalColor = label->getColor();
    
    GLubyte originalOpacity = label->getOpacity();
    
    bool originalVisibility = label->isVisible();
    
    label->setColor(color);
    
    label->setOpacity(opacity);
    
    label->setVisible(true);
    
    ccBlendFunc originalBlend = label->getBlendFunc();
    
    ccBlendFunc bf = {GL_SRC_ALPHA, GL_ONE}; // GL_SRC_ALPHA for glow effect
    label->setBlendFunc(bf);
    
    Point bottomLeft = Point(
                             label->getTexture()->getContentSize().width * label->getAnchorPoint().x,
                             label->getTexture()->getContentSize().height * label->getAnchorPoint().y);
    
    Point positionOffset= Point(
                                label->getTexture()->getContentSize().width  * label->getAnchorPoint().x - label->getTexture()->getContentSize().width / 2,
                                label->getTexture()->getContentSize().height * label->getAnchorPoint().y - label->getTexture()->getContentSize().height / 2);
    
    
    Point position = originalPos - positionOffset;
    
    //rt->getSprite()->getTexture()->setAntiAliasTexParameters();
    rt->begin();
    
    label->setPosition(Point(bottomLeft.x, bottomLeft.y));
    for(int i = 0;i < additiveCount;i++){
        label->visit();
    }
    
    rt->end();
    
    label->setPosition(originalPos);
    label->setColor(originalColor);
    label->setBlendFunc(originalBlend);
    label->setVisible(originalVisibility);
    label->setOpacity(originalOpacity);
    
    rt->setPosition(position);
    
    return rt;
}
*/

bool GameManager::isBoss(int theme, int stage){
    if(theme == 0 && stage == 23) return true;
    if(theme == 1 && stage == 23) return true;
    if(theme == 2 && stage == 23) return true;
    if(theme == 3 && stage == 23) return true;
    return false;
}

void GameManager::setPetInfos(){
    petInfoArray.clear();
    PET* pet = createPetInfo(PET_BOOGI, 7, 18, 3, 1, "boogi", "good");
    petInfoArray.push_back(pet);
    petInfoArray.push_back(createPetInfo(PET_BURI, 9, 19, 3, 1, "STR_PET_NAME_1_BURI",
                                         "STR_PET_DESCRIPTION_1_BURI"));
    petInfoArray.push_back(createPetInfo(PET_CHUS, 9, 20, 5, 1, "STR_PET_NAME_2_CHUS",
                                         "STR_PET_DESCRIPTION_2_CHUS"));
    petInfoArray.push_back(createPetInfo(PET_DORA_0, 13, 32, 3, 1, "STR_PET_NAME_3_DORA_0",
                                         "STR_PET_DESCRIPTION_3_DORA_0"));
    petInfoArray.push_back(createPetInfo(PET_DORA_1, 15, 44, 7, 2, "STR_PET_NAME_3_DORA_0",
                                         "STR_PET_DESCRIPTION_3_DORA_0"));
    petInfoArray.push_back(createPetInfo(PET_DORA_2, 17, 58, 11, 3, "STR_PET_NAME_3_DORA_0",
                                         "STR_PET_DESCRIPTION_3_DORA_0"));
    petInfoArray.push_back(createPetInfo(PET_DORA_3, 19, 70, 16, 4, "STR_PET_NAME_3_DORA_0",
                                         "STR_PET_DESCRIPTION_3_DORA_0"));
    petInfoArray.push_back(createPetInfo(PET_DORIS_0, 13, 35, 5, 1, "STR_PET_NAME_4_DORIS_0",
                                         "STR_PET_DESCRIPTION_4_DORIS_0"));
    petInfoArray.push_back(createPetInfo(PET_DORIS_1, 15, 46, 9, 2, "STR_PET_NAME_4_DORIS_0",
                                         "STR_PET_DESCRIPTION_4_DORIS_0"));
    petInfoArray.push_back(createPetInfo(PET_DORIS_2, 17, 60, 13, 3, "STR_PET_NAME_4_DORIS_0",
                                         "STR_PET_DESCRIPTION_4_DORIS_0"));
    
    petInfoArray.push_back(createPetInfo(PET_DORIS_3, 19, 74, 17, 4, "STR_PET_NAME_4_DORIS_0",
                                         "STR_PET_DESCRIPTION_4_DORIS_0"));
    petInfoArray.push_back(createPetInfo(PET_FRITH_0, 13, 37, 5, 1, "STR_PET_NAME_5_FRITH_0",
                                         "STR_PET_DESCRIPTION_5_FRITH_0"));
    petInfoArray.push_back(createPetInfo(PET_FRITH_1, 15, 48, 9, 2, "STR_PET_NAME_5_FRITH_0",
                                         "STR_PET_DESCRIPTION_5_FRITH_0"));
    petInfoArray.push_back(createPetInfo(PET_FRITH_2, 17, 62, 13, 3, "STR_PET_NAME_5_FRITH_0",
                                         "STR_PET_DESCRIPTION_5_FRITH_0"));
    petInfoArray.push_back(createPetInfo(PET_FRITH_3, 19, 76, 18, 4, "STR_PET_NAME_5_FRITH_0",
                                         "STR_PET_DESCRIPTION_5_FRITH_0"));
    petInfoArray.push_back(createPetInfo(PET_KIWI, 11, 24, 7, 1, "STR_PET_NAME_6_KIWI",
                                         "STR_PET_DESCRIPTION_6_KIWI"));
    petInfoArray.push_back(createPetInfo(PET_KUMURI, 15, 22, 10, 2, "STR_PET_NAME_7_KUMURI",
                                         "STR_PET_DESCRIPTION_7_KUMURI"));
    petInfoArray.push_back(createPetInfo(PET_LEXY, 9, 24, 10, 2, "STR_PET_NAME_8_LEXY",
                                         "STR_PET_DESCRIPTION_8_LEXY"));
    petInfoArray.push_back(createPetInfo(PET_LINDIS_0, 13, 39, 7, 1, "STR_PET_NAME_9_LINDIS_0",
                                         "STR_PET_DESCRIPTION_9_LINDIS_0"));
    petInfoArray.push_back(createPetInfo(PET_LINDIS_1, 15, 50, 11, 2, "STR_PET_NAME_9_LINDIS_0",
                                         "STR_PET_DESCRIPTION_9_LINDIS_0"));
    
    petInfoArray.push_back(createPetInfo(PET_LINDIS_2, 17, 64, 16, 3, "STR_PET_NAME_9_LINDIS_0",
                                         "STR_PET_DESCRIPTION_9_LINDIS_0"));
    petInfoArray.push_back(createPetInfo(PET_LINDIS_3, 19, 78, 20, 4, "STR_PET_NAME_9_LINDIS_0",
                                         "STR_PET_DESCRIPTION_9_LINDIS_0"));
    petInfoArray.push_back(createPetInfo(PET_NEPTUN, 11, 27, 12, 2, "STR_PET_NAME_10_NEPTUN",
                                         "STR_PET_DESCRIPTION_10_NEPTUN"));
    petInfoArray.push_back(createPetInfo(PET_OXTO, 19, 50, 17, 4, "STR_PET_NAME_11_OXTO",
                                         "STR_PET_DESCRIPTION_11_OXTO"));
    petInfoArray.push_back(createPetInfo(PET_PLUTO, 1, 0, 20, 5, "STR_PET_NAME_12_PLUTO",
                                         "STR_PET_DESCRIPTION_12_PLUTO"));
    petInfoArray.push_back(createPetInfo(PET_RIUS, 19, 55, 17, 4, "STR_PET_NAME_13_RIUS",
                                         "STR_PET_DESCRIPTION_13_RIUS"));
    petInfoArray.push_back(createPetInfo(PET_ROLO_0, 15, 44, 7, 2, "STR_PET_NAME_14_ROLO_0",
                                         "STR_PET_DESCRIPTION_14_ROLO_0"));
    petInfoArray.push_back(createPetInfo(PET_ROLO_1, 17, 55, 11, 3, "STR_PET_NAME_14_ROLO_0",
                                         "STR_PET_DESCRIPTION_14_ROLO_0"));
    petInfoArray.push_back(createPetInfo(PET_ROLO_2, 19, 69, 17, 4, "STR_PET_NAME_14_ROLO_0",
                                         "STR_PET_DESCRIPTION_14_ROLO_0"));
    petInfoArray.push_back(createPetInfo(PET_ROLO_3, 21, 83, 21, 5, "STR_PET_NAME_14_ROLO_0",
                                         "STR_PET_DESCRIPTION_14_ROLO_0"));
    
    petInfoArray.push_back(createPetInfo(PET_RURIK_0, 15, 46, 8, 2, "STR_PET_NAME_15_RURIK_0",
                                         "STR_PET_DESCRIPTION_15_RURIK_0"));
    petInfoArray.push_back(createPetInfo(PET_RURIK_1, 17, 57, 12, 3, "STR_PET_NAME_15_RURIK_0",
                                         "STR_PET_DESCRIPTION_15_RURIK_0"));
    petInfoArray.push_back(createPetInfo(PET_RURIK_2, 19, 71, 18, 4, "STR_PET_NAME_15_RURIK_0",
                                         "STR_PET_DESCRIPTION_15_RURIK_0"));
    petInfoArray.push_back(createPetInfo(PET_RURIK_3, 21, 85, 22, 5, "STR_PET_NAME_15_RURIK_0",
                                         "STR_PET_DESCRIPTION_15_RURIK_0"));
    petInfoArray.push_back(createPetInfo(PET_TERRY, 9, 32, 10, 2, "STR_PET_NAME_16_TERRY",
                                         "STR_PET_DESCRIPTION_16_TERRY"));
    petInfoArray.push_back(createPetInfo(PET_TORI, 11, 36, 10, 2, "STR_PET_NAME_17_TORI",
                                         "STR_PET_DESCRIPTION_17_TORI"));
    petInfoArray.push_back(createPetInfo(PET_TORORI, 13, 48, 13, 3, "STR_PET_NAME_18_TORORI",
                                         "STR_PET_DESCRIPTION_18_TORORI"));
    petInfoArray.push_back(createPetInfo(PET_UNO, 13, 60, 13, 4, "STR_PET_NAME_19_UNO",
                                         "STR_PET_DESCRIPTION_19_UNO"));
    petInfoArray.push_back(createPetInfo(PET_VETH, 13, 50, 15, 3, "STR_PET_NAME_20_VETH",
                                         "STR_PET_DESCRIPTION_20_VETH"));
    petInfoArray.push_back(createPetInfo(PET_WALOONG_0, 15, 50, 10, 2, "STR_PET_NAME_21_WALOONG_0",
                                         "STR_PET_DESCRIPTION_21_WALOONG_0"));
    
    petInfoArray.push_back(createPetInfo(PET_WALOONG_1, 17, 61, 14, 3, "STR_PET_NAME_21_WALOONG_0",
                                         "STR_PET_DESCRIPTION_21_WALOONG_0"));
    petInfoArray.push_back(createPetInfo(PET_WALOONG_2, 19, 75, 18, 4, "STR_PET_NAME_21_WALOONG_0",
                                         "STR_PET_DESCRIPTION_21_WALOONG_0"));
    petInfoArray.push_back(createPetInfo(PET_WALOONG_3, 21, 90, 25, 5, "STR_PET_NAME_21_WALOONG_0",
                                         "STR_PET_DESCRIPTION_21_WALOONG_0"));
    
    
    petInfoArray.push_back(createPetInfo(PET_ANGEL_DRAGON, 1, 100, 1, 6, "STR_ANGEL_DRAGON",
                                         "STR_PET_DESCRIPTION_22_ANGEL_DRAGON"));
    
    petInfoArray.push_back(createPetInfo(PET_IRON_DRAGON, 1, 100, 1, 6, "STR_IRON_DRAGON",
                                         "STR_PET_DESCRIPTION_23_IRON_DRAGON"));
    
    petInfoArray.push_back(createPetInfo(PET_WOOD_DRAGON, 1, 100, 1, 6, "STR_WOOD_DRAGON",
                                         "STR_PET_DESCRIPTION_24_WOOD_DRAGON"));
    
    petInfoArray.push_back(createPetInfo(PET_STONE_DRAGON, 1, 100, 1, 6, "STR_STONE_DRAGON",
                                         "STR_PET_DESCRIPTION_25_STONE_DRAGON"));
    
    petInfoArray.push_back(createPetInfo(PET_FALCON, 1, 100, 1, 6, "STR_FALCON",
                                         "STR_PET_DESCRIPTION_26_FALCON"));
     
    
}
bool GameManager::setPetCollected(int index){
    char buf[50];
    sprintf(buf, KEY_COLLECTION_FORMAT, index);
    bool isNew = !UserDefault::getInstance()->getBoolForKey(buf, false);;
    UserDefault::getInstance()->setBoolForKey(buf, true);
    saveCoin();
    return isNew;
}
bool GameManager::getPetCollected(int index){
    char buf[50];
    sprintf(buf, KEY_COLLECTION_FORMAT, index);
    return UserDefault::getInstance()->getBoolForKey(buf, false);
}
PET* GameManager::createPetInfo(int petNumber, int maxLevel, int maxAttack, int levelRequired, int starCount, const char* name, const char* description){
    CCLOG("Pet info creating");
    PET* pet = (PET *)malloc(sizeof(PET));
    CCLOG("Pet allocated");
    pet->petNumber = petNumber;
    pet->maxLevel = maxLevel;
    pet->maxAttack = maxAttack;
    pet->levelRequired = levelRequired;
    pet->name = name;
    pet->description = description;
    pet->starCount = starCount;
    CCLOG("returnging pet info");
    return pet;
}
//void GameManager::initAchievement(){
//    setAchievementGoal(ACHIEVEMENT_COMPLETE_TUTORIAL, 1);
//    setAchievementGoal(ACHIEVEMENT_BEAT_BOSS, 1);
//    setAchievementGoal(ACHIEVEMENT_OPEN_GOLD_SUITCASE, 1);
//    setAchievementGoal(ACHIEVEMENT_PERFECT_SCORE, 1);
//    setAchievementGoal(ACHIEVEMENT_WHO_IS_THE_HOOD, 1);
//    setAchievementGoal(ACHIEVEMENT_WEAPON_MASTER, WEAPON_NO_MORE);
//    setAchievementGoal(ACHIEVEMENT_DUNGEON_MASTER, 10);
//    setAchievementGoal(ACHIEVEMENT_WHERE_IS_THE_PRINCESS, 1);
//    setAchievementGoal(ACHIEVEMENT_OPEN_JEWELRY_BOX, 1);
//    setAchievementGoal(ACHIEVEMENT_COLLECT_RELICS, 4);
//}
//
////__String* GameManager::getInAppItemPrice(const char* name){
////    CCObject* obj = priceArray->objectForKey(name);
////    return (__String*)obj;
////}
//
////void GameManager::setInAppItemPrice(const char* name, const char* price){
////    GameManager::getInstance()->priceArray->setObject(__String::create(price), name);
////    CCLOG("cpp price: %s", ((__String*)GameManager::getInstance()->priceArray->objectForKey(name))->getCString());
////}
//void GameManager::setAchievementGoal(int achievementIndex, int goalCount){
//    char buf[50];
//    sprintf(buf, KEY_ACHIEVEMENT_GOAL, achievementIndex);
//    UserDefault::getInstance()->setIntegerForKey(buf, goalCount);
//    saveCoin();
//}
//bool GameManager::setAchievement(int achievementIndex, int countToAdd){
//    char buf[50];
//    sprintf(buf, KEY_ACHIEVEMENT_GOAL, achievementIndex);
//    int goalCount = UserDefault::getInstance()->getIntegerForKey(buf, 0);
//
//    char buff[50];
//    sprintf(buff, KEY_ACHIEVEMENT_CURRENT, achievementIndex);
//    int original = UserDefault::getInstance()->getIntegerForKey(buff, 0);
//    int currentCount = countToAdd;//
//    
//    if (currentCount > goalCount) {
//        currentCount = goalCount;
//    }
//    UserDefault::getInstance()->setIntegerForKey(buff, currentCount);
//    return goalCount <= currentCount && original < currentCount;
//}
//int GameManager::getAchievementGoalCount(int achievementIndex){
//    char buf[50];
//    sprintf(buf, KEY_ACHIEVEMENT_GOAL, achievementIndex);
//    return UserDefault::getInstance()->getIntegerForKey(buf, 0);
//}
//int GameManager::getAchievementCurrentCount(int achievementIndex){
//    char buf[50];
//    sprintf(buf, KEY_ACHIEVEMENT_CURRENT, achievementIndex);
//    return UserDefault::getInstance()->getIntegerForKey(buf, 0);
//}
void GameManager::arrangeInvetory(){
    int petNumber = PET_NONE;
    int nextPetNumber = PET_NONE;
    int nextPetLevel = 0;

    char buf[50];
    
    for (int i = 0; i < INVENTORY_MAX_COUNT-1; i++) {
        sprintf( buf, KEY_PET_FORMAT, i);
        petNumber = UserDefault::getInstance()->getIntegerForKey(buf, PET_NONE);
        if (petNumber == PET_NONE) {
            
            bool isSomethingThere = false;
            for (int j = i; j < INVENTORY_MAX_COUNT; j++) {
                sprintf( buf, KEY_PET_FORMAT, j);
                if (UserDefault::getInstance()->getIntegerForKey(buf, PET_NONE) != PET_NONE) {
                    isSomethingThere = true;
                }
            }
            if (!isSomethingThere) {
                break;
            }
            sprintf( buf, KEY_PET_FORMAT, i+1);
            nextPetNumber = UserDefault::getInstance()->getIntegerForKey(buf, PET_NONE);
            sprintf( buf, KEY_PET_LEVEL_FORMAT, i+1);
            nextPetLevel = UserDefault::getInstance()->getIntegerForKey(buf, 0);
            
            sprintf( buf, KEY_PET_FORMAT, i);
            UserDefault::getInstance()->setIntegerForKey(buf, nextPetNumber);
            sprintf( buf, KEY_PET_LEVEL_FORMAT, i);
            UserDefault::getInstance()->setIntegerForKey(buf, nextPetLevel);
            
            sprintf( buf, KEY_PET_FORMAT, i+1);
            UserDefault::getInstance()->setIntegerForKey(buf, PET_NONE);
            sprintf( buf, KEY_PET_LEVEL_FORMAT, i+1);
            UserDefault::getInstance()->setIntegerForKey(buf, 0);
            i--;
        }
    }
    saveCoin();
}
int GameManager::getInventoryCount(){
//    arrangeInvetory();
    int petIndex = PET_NONE;
    int count = 0;
    for (int i = 0; i < INVENTORY_MAX_COUNT; i++) {
        petIndex = getPetNumber(i);
//        CCLOG("get inventory");
        if (petIndex == PET_NONE) {
            count = i;
            break;
        }
        count++;
    }
    return count;
}

int GameManager::getTotalStarCount(){
    int starCount = 0;
    char buf[50];
    for (int i = 0; i < GameManager::getInstance()->totalThemeCount; i++) {
        for (int j = 0; j < 24; j++) {
            sprintf(buf, KEY_STAR_COUNT_FORMAT, i, j);
            starCount += UserDefault::getInstance()->getIntegerForKey(buf, 0);
            
            int mastery = GameManager::getInstance()->getMastery(i*24 + j);
            if(mastery >= 7){
                starCount++;
            }
        }
    }
    
    return starCount;
}

void GameManager::addPet(int petNumber){
    int index = getInventoryCount();
    CCLOG("addPet index: %d, petNumber: %d", index, petNumber);
    
    char buf[50];
    sprintf( buf, KEY_PET_FORMAT, index);
    UserDefault::getInstance()->setIntegerForKey(buf, petNumber);
    sprintf( buf, KEY_PET_LEVEL_FORMAT, index);
    UserDefault::getInstance()->setIntegerForKey(buf, 1);
    
    setPetCollected(petNumber);
    saveCoin();
}
void GameManager::removePet(int index){
    char buf[50];
    if(index < getSkyPetIndex()){
        setSkyPet(getSkyPetIndex()-1);
    }
    if(index < getGroundPetIndex()){
        setGroundPet(getGroundPetIndex() - 1);
    }
    sprintf( buf, KEY_PET_FORMAT, index);
    UserDefault::getInstance()->setIntegerForKey(buf, PET_NONE);
    sprintf( buf, KEY_PET_LEVEL_FORMAT, index);
    UserDefault::getInstance()->setIntegerForKey(buf, 0);
    saveCoin();
    arrangeInvetory();
}
//void GameManager::getPetAttack(int petNumber, int petExp, int& level, int& attack, int& expLeft){

//}
int GameManager::getPetAttack(int petNumber, int level){
    PET* petInfo = this->getPetInfo(petNumber);
    float attackUnit = petInfo->maxAttack/(petInfo->maxLevel*3.0f);
    return attackUnit*petInfo->maxLevel + attackUnit*level*2;
}
int GameManager::getPetAttack(int index){
    int petNumber = this->getPetNumber(index);
    int level = this->getPetLevel(index);
    return getPetAttack(petNumber, level);
}
int GameManager::getCurrentExp(){
    return UDGetInt(KEY_EXP, 0);
//    int expUnit = GAME_EXP_UNIT;
//
//    int originalExp = UserDefault::getInstance()->getIntegerForKey("GExp", 0);
//    int expLeft = originalExp;
//    int level = 0;
//    for (int i = 0; i < 100; i++) {
//        expLeft -= expUnit*i;
//        level++;
//        if (expLeft < 0) {
//            level--;
//            expLeft += expUnit*i;
//            break;
//        }else if(expLeft == 0){
//            break;
//        }
//    }
//    if (getGameLevel() >= MAX_LEVEL) {
//        UserDefault::getInstance()->setIntegerForKey("GExp", originalExp - (expLeft - getCurrentExpMax()));
//        saveCoin();
//        expLeft = getCurrentExpMax();
//    }
//    return expLeft;
}
int GameManager::getCurrentExpMax(){
    return GAME_EXP_UNIT*(getGameLevel() + 1);
}

int GameManager::getGameLevel(){
    return UDGetInt(KEY_LEVEL, 0);
//    int expUnit = GAME_EXP_UNIT;
//    int expLeft = UserDefault::getInstance()->getIntegerForKey("GExp", 0);
//    int level = 0;
//
//    for (int i = 0; i < MAX_LEVEL; i++) {
//        expLeft -= expUnit*i;
//        level++;
//        if (expLeft < 0) {
//            level--;
//            break;
//        }else if(expLeft == 0){
//            break;
//        }
//    }
//    return level;
}
void GameManager::setGameLevel(int level){
    UDSetInt(KEY_LEVEL, level);
}
void GameManager::addGameExp(int exp){
    int expLeft = UDGetInt(KEY_EXP, 0);
    expLeft += exp;
    int maxExp = getCurrentExpMax();
    if (maxExp <= expLeft) {
        expLeft -= maxExp;
        addGameLevel();
    }
    
    UDSetInt(KEY_EXP, expLeft);
    saveCoin();
    if (HUD) {
        HUD->updateProfile();
    }
}
void GameManager::addGameLevel(){
    int level = getGameLevel();
    level++;
    setGameLevel(level);
}
int GameManager::getPetLevel(int index){
    char buf[50];
    sprintf( buf, KEY_PET_LEVEL_FORMAT, index);
    return UserDefault::getInstance()->getIntegerForKey(buf, 1);
}
void GameManager::setPetLevel(int index, int level){
    char buf[50];
    sprintf( buf, KEY_PET_LEVEL_FORMAT, index);
    UserDefault::getInstance()->setIntegerForKey(buf, level);
    saveCoin();
}
/*int GameManager::getPetLevel(int petNumber, int exp){
    PET* pet = getPetInfo(petNumber);
    int totalCount = 1;
    for (int i = 0; i < pet->maxLevel; i++) {
        totalCount += i;
    }
    int expUnit = pet->maxExp/totalCount;
    
    int expLeft = exp;
    int level = 0;
    for (int i = 0; i < pet->maxLevel; i++) {
        expLeft -= expUnit*i;
        level++;
        if (expLeft <= 0) {
            level--;
            break;
        }
    }
    return level + 1;
}*/
int GameManager::getPetNumber(int index){
    char buf[50];
    sprintf( buf, KEY_PET_FORMAT, index);
    return UserDefault::getInstance()->getIntegerForKey(buf, PET_NONE);
}
/*int GameManager::getIndexOf(std::vector<int> vec, int index){
    for (int i = 0; i < vec.size(); i++) {
        if(vec.at(i) == index){
            return i;
        }
    }
    return -1;
}*/
const char* GameManager::getPetSpriteName(int petNumber){
    switch (petNumber) {
        case PET_NONE:
            return NULL;
        case PET_BOOGI:
            return "boogi0.png";
        case PET_BURI:
            return "buri0.png";
        case PET_CHUS:
            return "chus0.png";
        case PET_DORA_0:
            return "dora_0_0.png";
        case PET_DORA_1:
            return "dora_1_0.png";
        case PET_DORA_2:
            return "dora_2_0.png";
        case PET_DORA_3:
            return "dora_3_0.png";
        case PET_DORIS_0:
            return "doris_0_0.png";
        case PET_DORIS_1:
            return "doris_1_0.png";
        case PET_DORIS_2:
            return "doris_2_0.png";
            
        case PET_DORIS_3:
            return "doris_3_0.png";
        case PET_FRITH_0:
            return "frith_0_0.png";
        case PET_FRITH_1:
            return "frith_1_0.png";
        case PET_FRITH_2:
            return "frith_2_0.png";
        case PET_FRITH_3:
            return "frith_3_0.png";
        case PET_KIWI:
            return "kiwi0.png";
        case PET_KUMURI:
            return "kumuri0.png";
        case PET_LEXY:
            return "lexy0.png";
        case PET_LINDIS_0:
            return "lindis_0_0.png";
        case PET_LINDIS_1:
            return "lindis_1_0.png";
            
        case PET_LINDIS_2:
            return "lindis_2_0.png";
        case PET_LINDIS_3:
            return "lindis_3_0.png";
        case PET_NEPTUN:
            return "neptun0.png";
        case PET_OXTO:
            return "oxto0.png";
        case PET_PLUTO:
            return "pluto0.png";
        case PET_RIUS:
            return "rius0.png";
        case PET_ROLO_0:
            return "rolo_0_0.png";
        case PET_ROLO_1:
            return "rolo_1_0.png";
        case PET_ROLO_2:
            return "rolo_2_0.png";
        case PET_ROLO_3:
            return "rolo_3_0.png";
            
        case PET_RURIK_0:
            return "rorik_0_0.png";
        case PET_RURIK_1:
            return "rorik_1_0.png";
        case PET_RURIK_2:
            return "rorik_2_0.png";
        case PET_RURIK_3:
            return "rorik_3_0.png";
        case PET_TERRY:
            return "terry0.png";
        case PET_TORI:
            return "tori0.png";
        case PET_TORORI:
            return "torori0.png";
        case PET_UNO:
            return "uno0.png";
        case PET_VETH:
            return "veth0.png";
        case PET_WALOONG_0:
            return "waloong_0_0.png";
        case PET_WALOONG_1:
            return "waloong_1_0.png";
        case PET_WALOONG_2:
            return "waloong_2_0.png";
        case PET_WALOONG_3:
            return "waloong_3_0.png";
        case PET_ANGEL_DRAGON:
            return "angelDragon.png";
        case PET_IRON_DRAGON:
            return "ironDragon.png";
        case PET_STONE_DRAGON:
            return "stoneDragon.png";
        case PET_WOOD_DRAGON:
            return "woodDragon.png";
        case PET_FALCON:
            return "falcon.png";
        default:
            return NULL;
            break;
    }
}
int GameManager::getWeaponPowerLevel(int weaponType){
    int level = UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, weaponType)->getCString(), 0);
    return level;
}

void GameManager::addWeaponPowerLevel(int weapon){
    int level = UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, weapon)->getCString(), 0);
    level++;
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, weapon)->getCString(), level);
    GameManager::getInstance()->saveCoin();
}

int GameManager::getWeaponCriticalLevel(int weaponType){
    int level = UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, weaponType)->getCString(), 0);
    return level;
}

void GameManager::addWeaponCriticalLevel(int weapon){
    int level = UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, weapon)->getCString(), 0);
    level++;
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, weapon)->getCString(), level);
    GameManager::getInstance()->saveCoin();
}

void GameManager::addMastery(int stageIndex){
    char masterBuf[50];
    sprintf( masterBuf, KEY_STAGE_MASTER_FORMAT, stageIndex);
    int mastery = UserDefault::getInstance()->getIntegerForKey(masterBuf, 0);
    mastery++;
    if(mastery == 6){
        
    }
    if (mastery > 7) {
        mastery = 7;
    }
    UserDefault::getInstance()->setIntegerForKey(masterBuf, mastery);
    saveCoin();
}
int GameManager::getMastery(int stageIndex){
    char masterBuf[50];
    sprintf( masterBuf, KEY_STAGE_MASTER_FORMAT, stageIndex);
    int mastery = UserDefault::getInstance()->getIntegerForKey(masterBuf, 0);
    return mastery;
}
PET* GameManager::getPetInfo(int petNumber){
    return petInfoArray.at(petNumber-1);
}

const char* GameManager::getPetName(int petNumber){
    return getPetInfo(petNumber)->name;
}

Sprite* GameManager::getCard(int index){
    PET* pet = getPetInfo(getPetNumber(index));
    
    Sprite* card = Sprite::createWithSpriteFrameName("cardBackground.png");
    
    Label* lblName = Label::createWithTTF(pet->name, FONT_DEFAULT, 20);
    card->addChild(lblName);
    lblName->setPosition(Point(0,0));
    
    return card;
}
void GameManager::setLoadedData(const char* key, const char* value){
    if(std::strcmp(value, "true") == 0 || std::strcmp(value, "false") == 0){
        UserDefault::getInstance()->setBoolForKey(key, Value(value).asBool());
    }else{
        UserDefault::getInstance()->setIntegerForKey(key, Value(value).asInt());
    }
    
    // Money
    /*if(std::strcmp(key, "GemCount") == 0){
        GameManager::getInstance()->setGem(Value(value).asInt());
        return;
    }else if(std::strcmp(key, "CoinCount") == 0){
        GameManager::getInstance()->setCoin(Value(value).asInt());
        return;
    }*/
    // laset clear
    /*if(std::strcmp(key, KEY_LAST_CLEAR_STAGE) == 0){
        UserDefault::getInstance()->setIntegerForKey(KEY_LAST_CLEAR_STAGE, Value(value).asInt());
        return;
    }
    
    // Exp
    if(std::strcmp(key, "GExp") == 0){
        UserDefault::getInstance()->setIntegerForKey("GExp", Value(value).asInt());
        return;
    }
    
    // Stage
    for (int th = 0; th < totalThemeCount; th++) {
        for (int st = 0; st < 24; st++) {
            // star
            char buf[50];
            sprintf(buf, KEY_STAR_COUNT_FORMAT, th, st);
            if (std::strcmp(key, buf) == 0) {
                UserDefault::getInstance()->setIntegerForKey(buf, Value(value).asInt());
                return;
            }
            
            // mastery
            char masterBuf[50];
            sprintf( masterBuf, KEY_STAGE_MASTER_FORMAT, th*24 + st);
            if (std::strcmp(key, masterBuf) == 0) {
                UserDefault::getInstance()->setIntegerForKey(masterBuf, Value(value).asInt());
                return;
            }
        }
    }
    // Pet
    for (int i = 0; i < 100; i++) {
        char pbuf[50];
        sprintf( pbuf, KEY_PET_FORMAT, i);
        if (std::strcmp(key, pbuf)) {
            UserDefault::getInstance()->setIntegerForKey(pbuf, Value(value).asInt());
            return;
        }
        
        
        char plbuf[50];
        sprintf( plbuf, KEY_PET_LEVEL_FORMAT, i);
        if (std::strcmp(key, plbuf)) {
            UserDefault::getInstance()->setIntegerForKey(plbuf, Value(value).asInt());
            return;
        }
    }
    
    // Character
    for (int i = 0; i < TOTAL_CHARACTER_COUNT; i++) {
        char cbuf[25];
        sprintf(cbuf, KEY_CHARACTER_PURCHASED_FORMAT, i);
        if (std::strcmp(key, cbuf)) {
            UserDefault::getInstance()->setBoolForKey(cbuf, Value(value).asBool());
            return;
        }
    }
    
    // Weapon
    char wbuf[25];
    for (int i = 0; i < TOTAL_WEAPON_COUNT; i++) {
        sprintf(wbuf, KEY_WEAPON_PURCHASED_FORMAT, i);
        if (std::strcmp(key, wbuf) == 0) {
            UserDefault::getInstance()->setBoolForKey(wbuf, Value(value).asBool());
            return;
        }
        
        // Weapon Power
        if (std::strcmp(key, __String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString()) == 0) {
            UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), Value(value).asInt());
            return;
        }
        
        // Weapon Critical
        if (std::strcmp(key, __String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString()) == 0) {
            UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), Value(value).asInt());
            return;
        }
    }
    */
}
void GameManager::reset(){
    UserDefault::getInstance()->setBoolForKey(KEY_STARTER_PACK_PURCHASED, false);

    // Money
//    GameManager::getInstance()->setGem(0);
//    GameManager::getInstance()->setCoin(0);
    
    // Weapon & Character
    UserDefault::getInstance()->setIntegerForKey(KEY_SELECTED_CHARACTER, CHARACTER_DARY);
    UserDefault::getInstance()->setIntegerForKey(KEY_SELECTED_WEAPON, WEAPON_PISTOL);
    
    
    // Collection
    for (int i = PET_NONE + 1; i < PET_NO_MORE; i++) {
        char petBuf[50];
        sprintf(petBuf, KEY_COLLECTION_FORMAT, i);
        UserDefault::getInstance()->setBoolForKey(petBuf, false);
    }
    
    
    // Stage
    for (int th = 0; th < totalThemeCount; th++) {
        for (int st = 0; st < 24; st++) {
            // star
            char buf[50];
            sprintf(buf, KEY_STAR_COUNT_FORMAT, th, st);
            UserDefault::getInstance()->setIntegerForKey(buf, 0);
            
            // mastery
            char masterBuf[50];
            sprintf( masterBuf, KEY_STAGE_MASTER_FORMAT, th*24 + st);
            UserDefault::getInstance()->setIntegerForKey(masterBuf, 0);
        }
    }
    
    // laset clear
    UserDefault::getInstance()->setIntegerForKey(KEY_LAST_CLEAR_STAGE, 0);
    
    // Exp
    UserDefault::getInstance()->setIntegerForKey("GExp", 0);
    
    // Weapon
    char wbuf[25];
    for (int i = 0; i < TOTAL_WEAPON_COUNT; i++) {
        sprintf(wbuf, KEY_WEAPON_PURCHASED_FORMAT, i);
        UserDefault::getInstance()->setBoolForKey(wbuf, false);
        
        // Weapon Power
        UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), 0);
        
        // Weapon Critical
        UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), 0);
    }
    
    // Pet
    for (int i = 0; i < 100; i++) {
        char pbuf[50];
        sprintf( pbuf, KEY_PET_FORMAT, i);
        UserDefault::getInstance()->setIntegerForKey(pbuf, PET_NONE);
        
        char plbuf[50];
        sprintf( plbuf, KEY_PET_LEVEL_FORMAT, i);
        UserDefault::getInstance()->setIntegerForKey(plbuf, 1);
    }
    
    // Character
    for (int i = 0; i < TOTAL_CHARACTER_COUNT; i++) {
        char buf[25];
        sprintf(buf, KEY_CHARACTER_PURCHASED_FORMAT, i);
        UserDefault::getInstance()->setBoolForKey(buf, false);
    }
    saveCoin();
}
void GameManager::saveGameData(){
    // Money
    /*if (getGem() != 0) {
        NativeInterface::NativeInterfacesetIntForSavedGame("GemCount", getGem());
    }
    if (getCoin() != 0) {
        NativeInterface::NativeInterfacesetIntForSavedGame("CoinCount", getCoin());
    }*/
    
    // Weapon & Character
    if (UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_CHARACTER, CHARACTER_DARY) != CHARACTER_DARY) {
//        NativeInterface::NativeInterfacesetIntForSavedGame(KEY_SELECTED_CHARACTER, UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_CHARACTER));
    }
    
    if (UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_WEAPON, WEAPON_PISTOL) != WEAPON_PISTOL) {
//        NativeInterface::NativeInterfacesetIntForSavedGame(KEY_SELECTED_WEAPON, UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_WEAPON));
    }

    // Collection
    for (int i = PET_NONE + 1; i < PET_NO_MORE; i++) {
        if (getPetCollected(i)) {
            char petBuf[50];
            sprintf(petBuf, KEY_COLLECTION_FORMAT, i);
//            NativeInterface::NativeInterfacesetIntForSavedGame(petBuf, true);
        }
    }
    
    // Stage
    for (int th = 0; th < totalThemeCount; th++) {
        for (int st = 0; st < 24; st++) {
            // star
            char buf[50];
            sprintf(buf, KEY_STAR_COUNT_FORMAT, th, st);
//            if (UserDefault::getInstance()->getIntegerForKey(buf, 0) != 0) {
//                NativeInterface::NativeInterfacesetIntForSavedGame(buf, UserDefault::getInstance()->getIntegerForKey(buf, 0));
//            }
            
            // mastery
            char masterBuf[50];
            sprintf( masterBuf, KEY_STAGE_MASTER_FORMAT, th*24 + st);
//            if (UserDefault::getInstance()->getIntegerForKey(masterBuf) != 0) {
//                NativeInterface::NativeInterfacesetIntForSavedGame(masterBuf, UserDefault::getInstance()->getIntegerForKey(masterBuf));
//            }
        }
    }

    // laset clear
    if (UserDefault::getInstance()->getIntegerForKey(KEY_LAST_CLEAR_STAGE, -1) != 0) {
//        NativeInterface::NativeInterfacesetIntForSavedGame(KEY_LAST_CLEAR_STAGE, UserDefault::getInstance()->getIntegerForKey(KEY_LAST_CLEAR_STAGE, 0));
    }
    
    // Exp
    if (UserDefault::getInstance()->getIntegerForKey("GExp", 0) != 0) {
//        NativeInterface::NativeInterfacesetStringForSavedGame("GExp", Value(UserDefault::getInstance()->getIntegerForKey("GExp", 0)).asString().c_str());
//        NativeInterface::NativeInterfacesetIntForSavedGame("GExp", UserDefault::getInstance()->getIntegerForKey("GExp", 0));
    }
    
    
    // Pet
    for (int i = 0; i < 100; i++) {
        char pbuf[50];
        sprintf( pbuf, KEY_PET_FORMAT, i);
        if (UserDefault::getInstance()->getIntegerForKey(pbuf, PET_NONE) != PET_NONE) {
//            NativeInterface::NativeInterfacesetIntForSavedGame(pbuf, UserDefault::getInstance()->getIntegerForKey(pbuf, PET_NONE));
        }
        
        char plbuf[50];
        sprintf( plbuf, KEY_PET_LEVEL_FORMAT, i);
        if (UserDefault::getInstance()->getIntegerForKey(plbuf, 1) != 1) {
//            NativeInterface::NativeInterfacesetIntForSavedGame(plbuf, UserDefault::getInstance()->getIntegerForKey(plbuf, 1));
        }
    }
    
    // Character
    for (int i = 0; i < TOTAL_CHARACTER_COUNT; i++) {
        char buf[25];
        sprintf(buf, KEY_CHARACTER_PURCHASED_FORMAT, i);
        if (UserDefault::getInstance()->getBoolForKey(buf, false)) {
//            NativeInterface::NativeInterfacesetIntForSavedGame(buf, (int)UserDefault::getInstance()->getBoolForKey(buf, false));
//            NativeInterface::NativeInterfacesetStringForSavedGame(buf, UserDefault::getInstance()->getBoolForKey(buf, false)?"true":"false");
        }
    }
    
    // Weapon
    char wbuf[25];
    for (int i = 0; i < TOTAL_WEAPON_COUNT; i++) {
        sprintf(wbuf, KEY_WEAPON_PURCHASED_FORMAT, i);
        if (UserDefault::getInstance()->getBoolForKey(wbuf, false)) {
//            NativeInterface::NativeInterfacesetIntForSavedGame(wbuf, (int)UserDefault::getInstance()->getBoolForKey(wbuf, false));
//            NativeInterface::NativeInterfacesetStringForSavedGame(wbuf, UserDefault::getInstance()->getBoolForKey(wbuf, false)?"true":"false");
        }
        
        
        // Weapon Power
        if (UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), 0) != 0) {
//            NativeInterface::NativeInterfacesetIntForSavedGame(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), 0));
        }
        
        
        // Weapon Critical
        if (UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), 0) != 0) {
//            NativeInterface::NativeInterfacesetIntForSavedGame(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), 0));
        }
    }
//    NativeInterface::NativeInterfacesaveGame();
}

void GameManager::setCharacterPurchased(int character){
    char buf[25];
    sprintf(buf, KEY_CHARACTER_PURCHASED_FORMAT, character);
    UserDefault::getInstance()->setBoolForKey(buf, true);
    saveCoin();
}
void GameManager::showDisposableMessage(const char* msg, Node* parent){
    if (parent->getChildByTag(TAG_DISPOSABLE_MESSAGE) != nullptr){
        parent->getChildByTag(TAG_DISPOSABLE_MESSAGE)->removeFromParent();
    }
    Label* disposableLabel = LM->getLocalizedLabel(msg, Color4B::WHITE);//Label::createWithTTF(msg, GameManager::getInstance()->getFont(FONT_VISITOR), 26);
    parent->addChild(disposableLabel, 2002);
    disposableLabel->enableOutline(Color4B::BLACK, 8);
    //    disposableLabel->enableShadow();
    disposableLabel->setTextColor(Color4B::WHITE);
    disposableLabel->setTag(TAG_DISPOSABLE_MESSAGE);
    
    disposableLabel->setString(msg);
    float labelWidth = disposableLabel->getBoundingBox().size.width;
    Size size = Director::getInstance()->getWinSize();
    if (labelWidth > size.width) {
        disposableLabel->setWidth(size.width/disposableLabel->getScale());
    }
    float dur = 3;
    disposableLabel->setPosition(size.width/2 - parent->getPositionX(), size.height/4);
    disposableLabel->runAction(Sequence::create(MoveBy::create(0.5, Point(0, 10)), DelayTime::create(dur), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_0(Node::removeFromParent, disposableLabel)), NULL));
    lastDisposableLabel = disposableLabel;
    
//    Sprite* sptDisposableBack = Sprite::create("whiteBigCircle.png");
//    parent->addChild(sptDisposableBack, 2001);
//    
//    sptDisposableBack->setColor(Color3B::BLACK);
//    sptDisposableBack->setOpacity(100);
//    sptDisposableBack->setScale(0.8, 0.2);
//    sptDisposableBack->stopAllActions();
//    sptDisposableBack->setPosition(disposableLabel->getPosition());
//    sptDisposableBack->runAction(Sequence::create(MoveBy::create(0.5, Point(0, 10)), DelayTime::create(dur), FadeOut::create(0.3), NULL));
//    lastDisposableLabelBack = sptDisposableBack;
}
bool GameManager::isCharacterPurchased(int character){
    if (character == CHARACTER_DARY) {
        return true;
    }
    char buf[25];
    sprintf(buf, KEY_CHARACTER_PURCHASED_FORMAT, character);
    bool isPurchased = UserDefault::getInstance()->getBoolForKey(buf, false);
    return isPurchased;
}
void GameManager::setWeaponPurchased(int weapon){
    char buf[25];
    sprintf(buf, KEY_WEAPON_PURCHASED_FORMAT, weapon);
    UserDefault::getInstance()->setBoolForKey(buf, true);
    saveCoin();
}
bool GameManager::isWeaponPurchased(int weapon){
    if (weapon == WEAPON_PISTOL) {
        return true;
    }
    char buf[25];
    sprintf(buf, KEY_WEAPON_PURCHASED_FORMAT, weapon);
    bool isPurchased = UserDefault::getInstance()->getBoolForKey(buf, false);
    return isPurchased;
}


/*void GameManager::refreshTopBar(Widget* topBar){
    Text* lblGem = dynamic_cast<Text*>(Helper::seekWidgetByName(topBar, "lblGem"));
    lblGem->setString(Value(getGem()).asString());
    Text* lblCoin = dynamic_cast<Text*>(Helper::seekWidgetByName(topBar, "lblCoin"));
    lblCoin->setString(Value(getCoin()).asString());
    
    Text* lblLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(topBar, "lblLevel"));
    lblLevel->setString(Value(getGameLevel()).asString());
    
    int expUnit = GAME_EXP_UNIT;
    
    int expLeft = UserDefault::getInstance()->getIntegerForKey("GExp", 0);
    int level = 0;
    int currentMaxExp = 0;
    for (int i = 0; i < 100; i++) {
        currentMaxExp = expUnit*(i+1);
        expLeft -= currentMaxExp;
        level++;
        if (expLeft <= 0) {
            level--;
            expLeft += currentMaxExp;
            break;
        }
    }

    Text* lblExpPercent = dynamic_cast<Text*>(Helper::seekWidgetByName(topBar, "lblExpPercent"));
    char buf[50];
    CCLOG("cExp:%d, mExp: %d", expLeft, currentMaxExp);
    float percent = expLeft*100.0f/currentMaxExp;
    sprintf( buf, "%.1f%%", percent);
    LoadingBar* sptPercent = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(topBar, "ProgressBar_15"));
    sptPercent->setPercent(percent);
    lblExpPercent->setString(buf);
    lblExpPercent->setLocalZOrder(10);
}*/
int GameManager::getRandomFood(){
    return rand()%24;
}
int GameManager::getFoodCount(int food){
    char buf[50];
    sprintf( buf, KEY_FOOD_FORMAT, food);
    const char* key = buf;
    return UserDefault::getInstance()->getIntegerForKey(key, 0);
}
void GameManager::addFoodCount(int food, int count){
    char buf[50];
    sprintf( buf, KEY_FOOD_FORMAT, food);
    const char* key = buf;
    int foodCount = getFoodCount(food);
    foodCount += count;
    UserDefault::getInstance()->setIntegerForKey(key, foodCount);
    saveCoin();
}

const char* GameManager::getFoodSpriteName(int food){
    switch (food) {
        case FOOD_APPLE:
            return "apple.png";
            break;
        case FOOD_BANANA:
            return "banana.png";
            break;
        case FOOD_BISCKET:
            return "orange.png";
            break;
        case FOOD_BREAD:
            return "bread.png";
            break;
        case FOOD_CANDY:
            return "candy.png";
            break;
        case FOOD_CARROT:
            return "carrot.png";
            break;
        case FOOD_CHERRY:
            return "cherry.png";
            break;
        case FOOD_CHICKEN:
            return "barbeque.png";
            break;
        case FOOD_CORN:
            return "corn.png";
            break;
        case FOOD_DONUT:
            return "donut.png";
            break;
        case FOOD_FISH:
            return "fish.png";
            break;
        case FOOD_GREEN_APPLE:
            return "greemApple.png";
            break;
        case FOOD_HAMBURGER:
            return "hamburger.png";
            break;
        case FOOD_HOTDOG:
            return "hotDog.png";
            break;
        case FOOD_ICECREAM:
            return "iceCream.png";
            break;
        case FOOD_LOLLYPOP:
            return "lollypop.png";
            break;
        case FOOD_MUSHROOM:
            return "mushroom.png";
            break;
        case FOOD_ONIGIRI:
            return "onigiri.png";
            break;
        case FOOD_PEAR:
            return "pear.png";
            break;
        case FOOD_PINE_APPLE:
            return "pineApple.png";
            break;
        case FOOD_POTATO_CHIPS:
            return "potatoChips.png";
            break;
        case FOOD_STRAWBERRY:
            return "strawberry.png";
            break;
        case FOOD_TOAST:
            return "sausage.png";
            break;
        case FOOD_WATERMELON:
            return "waterMelon.png";
            break;
        default:
            return "apple.png";
            break;
    }
}
int GameManager::getSkyPetIndex(){
    return UserDefault::getInstance()->getIntegerForKey(KEY_SKY_PET_INDEX, -1);
}
int GameManager::getGroundPetIndex(){
    return UserDefault::getInstance()->getIntegerForKey(KEY_GROUND_PET_INDEX, -1);
}
Buddy* GameManager::getSkyPet(){
    int buddyIndex = getSkyPetIndex();
    Buddy* buddy = getPetFromInventory(buddyIndex);
    if(buddy)
        buddy->retain();
    return buddy;
}

Buddy* GameManager::getGroundPet(){
    int buddyIndex = getGroundPetIndex();
    Buddy* buddy = getPetFromInventory(buddyIndex);
    if(buddy)
        buddy->retain();
    return buddy;
}

void GameManager::setSkyPet(int index){
    UserDefault::getInstance()->setIntegerForKey(KEY_SKY_PET_INDEX, index);
    saveCoin();
}

void GameManager::setGroundPet(int index){
    UserDefault::getInstance()->setIntegerForKey(KEY_GROUND_PET_INDEX, index);
    saveCoin();
}

Buddy* GameManager::getPetFromInventory(int petIndex){
    int petNumber = getPetNumber(petIndex);
    return getPet(petNumber);
}

Buddy* GameManager::getPet(int petNumber){
    Buddy* buddy = NULL;
    switch (petNumber)
    {
        case PET_NONE:
            return NULL;
        case PET_BOOGI:
        case PET_BURI:
        case PET_CHUS:
        case PET_KIWI:
        case PET_KUMURI:
        case PET_LEXY:
        case PET_NEPTUN:
        case PET_OXTO:
        case PET_PLUTO:
        case PET_RIUS:
        case PET_TERRY:
        case PET_TORI:
        case PET_TORORI:
        case PET_UNO:
        case PET_VETH:
            buddy = GroundPet::create(UNIT_BUDDY, 10, petNumber, getPetSpriteName(petNumber), 150);
            break;
        case PET_LINDIS_0:
        case PET_LINDIS_1:
        case PET_LINDIS_2:
        case PET_LINDIS_3:
        case PET_DORA_0:
        case PET_DORA_1:
        case PET_DORA_2:
        case PET_DORA_3:
        case PET_DORIS_0:
        case PET_DORIS_1:
        case PET_DORIS_2:
        case PET_DORIS_3:
        case PET_FRITH_0:
        case PET_FRITH_1:
        case PET_FRITH_2:
        case PET_FRITH_3:
        case PET_ROLO_0:
        case PET_ROLO_1:
        case PET_ROLO_2:
        case PET_ROLO_3:
        case PET_RURIK_0:
        case PET_RURIK_1:
        case PET_RURIK_2:
        case PET_RURIK_3:
        case PET_WALOONG_0:
        case PET_WALOONG_1:
        case PET_WALOONG_2:
        case PET_WALOONG_3:
        case PET_ANGEL_DRAGON:
        case PET_IRON_DRAGON:
        case PET_STONE_DRAGON:
        case PET_WOOD_DRAGON:
        case PET_FALCON:
            buddy = SkyPet::create(UNIT_BUDDY, 10, petNumber, getPetSpriteName(petNumber), 300);
            break;
        default:
            return NULL;
            break;
    }
    /*if(petNumber == PET_BOOGI ||
     petNumber == PET_BURI ||
     petNumber == PET_CHUS ||
     petNumber == PET_KIWI||
     petNumber == PET_KUMURI||
     petNumber == PET_LEXY||
     petNumber == PET_NEPTUN||
     petNumber == PET_OXTO||
     petNumber == PET_PLUTO||
     petNumber == PET_RIUS||
     petNumber == PET_TERRY||
     petNumber == PET_TORI||
     petNumber == PET_TORORI||
     petNumber == PET_UNO||
     petNumber == PET_VETH){
     buddy = GroundPet::create(UNIT_BUDDY, 10, PET_LEXY, getPetSpriteName(petNumber), 100);
     }else{
     buddy = SkyPet::create(UNIT_BUDDY, 10, petNumber, getPetSpriteName(petNumber), 100);
     }*/
    if(petNumber == PET_ANGEL_DRAGON){
        Point start = Point(10, 24);
        Point end = Point(30, 5);
        Sprite* sptRingBeyond = Sprite::createWithSpriteFrameName("angelRingBeyond.png");
        buddy->addChild(sptRingBeyond, -1);
        sptRingBeyond->setPosition(end);
        sptRingBeyond->runAction(RepeatForever::create(Sequence::create(MoveTo::create(1, start), MoveTo::create(1, end), NULL)));
        sptRingBeyond->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.4, 0.8), ScaleTo::create(0.4, 1), NULL)));
        
        Sprite* sptRingFore = Sprite::createWithSpriteFrameName("angelRingFore.PNG");
        buddy->addChild(sptRingFore);
        sptRingFore->setPosition(end);
        sptRingFore->runAction(RepeatForever::create(Sequence::create(MoveTo::create(1, start), MoveTo::create(1, end), NULL)));
        sptRingFore->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.4, 0.8), ScaleTo::create(0.4, 1), NULL)));
    }else if(petNumber == PET_IRON_DRAGON){
        Sprite* wing = Sprite::createWithSpriteFrameName("ironDragonWing.png");
        buddy->addChild(wing);
        wing->setAnchorPoint(Point(0, 0));
        wing->setPosition(Point(23, 10));
        wing->runAction(RepeatForever::create(Sequence::create(RotateTo::create(0.2, 40), RotateTo::create(0.5, 0), NULL)));
    }else if(petNumber == PET_WOOD_DRAGON){
        Point center = Point(buddy->getContentSize().width/2, buddy->getContentSize().height/2);//Point::ZERO;
        for(int i = 0;i<20;i++){
            ZOrderChangableSprite* leaf = new ZOrderChangableSprite();
            leaf->initWithSpriteFrameNameYo("woodDragonLeaf.png");
            int r = 25;
            int angle = rand()%180;
            Point start = Point(r*cos(angle*3.14/180), r*sin(angle*3.14/180));
            Point end = Point(-start.x, -start.y);
            
            buddy->addChild(leaf);
            float dur = (rand()%100)*0.01 + 0.2f;
            leaf->runAction(RepeatForever::create(Sequence::create(EaseIn::create(MoveTo::create(dur, center), 2), EaseOut::create(MoveTo::create(dur, start + center), 2), CallFunc::create(CC_CALLBACK_0(ZOrderChangableSprite::changeZOrderDown, leaf)), EaseIn::create(MoveTo::create(dur, center), 2), EaseOut::create(MoveTo::create(dur, end + center), 2), CallFunc::create(CC_CALLBACK_0(ZOrderChangableSprite::changeZOrderUp, leaf)), NULL)));
            float scaleUp = 2;
            float scaleDown = 0.3;
            leaf->runAction(RepeatForever::create(Sequence::create(EaseIn::create(ScaleTo::create(dur, scaleUp), 2), EaseOut::create(ScaleTo::create(dur, 1), 2), CallFunc::create(CC_CALLBACK_0(ZOrderChangableSprite::changeZOrderDown, leaf)), EaseIn::create(ScaleTo::create(dur, scaleDown), 2), EaseOut::create(ScaleTo::create(dur, 1), 2), CallFunc::create(CC_CALLBACK_0(ZOrderChangableSprite::changeZOrderUp, leaf)), NULL)));
            leaf->runAction(RepeatForever::create(RotateBy::create((rand()%100)*0.01 + 1, 360)));
        }
    }else if(petNumber == PET_STONE_DRAGON){
        
        float dur = (rand()%100)*0.01 + 0.5;
        Sprite* tail0 = Sprite::createWithSpriteFrameName("stoneDragonTail3.png");
        buddy->addChild(tail0);
        tail0->setPosition(Point(15, 0));
        tail0->runAction(RepeatForever::create(Sequence::create(MoveBy::create(dur, Point(0, 9)), MoveBy::create(dur, Point(0, -9)), NULL)));
        
        dur = (rand()%100)*0.01 + 0.5;
        tail0 = Sprite::createWithSpriteFrameName("stoneDragonTail0.png");
        buddy->addChild(tail0);
        tail0->setPosition(Point(26, 5));
        tail0->runAction(RepeatForever::create(Sequence::create(MoveBy::create(dur, Point(0, 8)), MoveBy::create(dur, Point(0, -8)), NULL)));
        
        dur = (rand()%100)*0.01 + 0.5;
        tail0 = Sprite::createWithSpriteFrameName("stoneDragonTail2.png");
        buddy->addChild(tail0);
        tail0->setPosition(Point(33, 10));
        tail0->runAction(RepeatForever::create(Sequence::create(MoveBy::create(dur, Point(0, 6)), MoveBy::create(dur, Point(0, -6)), NULL)));
        
        dur = (rand()%100)*0.01 + 0.5;
        tail0 = Sprite::createWithSpriteFrameName("stoneDragonTail1.png");
        buddy->addChild(tail0);
        tail0->setPosition(Point(40, 10));
        tail0->runAction(RepeatForever::create(Sequence::create(MoveBy::create(dur, Point(0, 10)), MoveBy::create(dur, Point(0, -10)), NULL)));
    }else if(petNumber == PET_FALCON){
        Sprite* wing = Sprite::createWithSpriteFrameName("falconWing.png");
        buddy->addChild(wing);
        wing->setAnchorPoint(Point(0, 0));
        wing->setPosition(Point(33, 15));
        wing->runAction(RepeatForever::create(Sequence::create(RotateTo::create(0.3, 40), RotateTo::create(0.6, 0), DelayTime::create(3),NULL)));
    }else if (buddy != NULL) {
        buddy->runAnimation(getPetSpriteName(petNumber), true);
    }
    return buddy;
}
/*
 switch (this->getTag()) {
 case PET_NONE:
 
 case PET_BOOGI:
 
 case PET_BURI:
 
 case PET_CHUS:
 
 case PET_DORA_0:
 
 case PET_DORA_1:
 
 case PET_DORA_2:
 
 case PET_DORA_3:
 
 case PET_DORIS_0:
 
 case PET_DORIS_1:
 
 case PET_DORIS_2:
 
 case PET_DORIS_3:
 
 case PET_FRITH_0:
 
 case PET_FRITH_1:
 
 case PET_FRITH_2:
 
 case PET_FRITH_3:
 
 case PET_KIWI:
 
 case PET_KUMURI:
 
 case PET_LEXY:
 
 case PET_LINDIS_0:
 
 case PET_LINDIS_1:
 
 case PET_LINDIS_2:
 
 case PET_LINDIS_3:
 
 case PET_NEPTUN:
 
 case PET_OXTO:
 
 case PET_PLUTO:
 
 case PET_RIUS:
 
 case PET_ROLO_0:
 
 case PET_ROLO_1:
 
 case PET_ROLO_2:
 
 case PET_ROLO_3:
 
 case PET_RURIK_0:
 
 case PET_RURIK_1:
 
 case PET_RURIK_2:
 
 case PET_RURIK_3:
 
 case PET_TERRY:
 
 case PET_TORI:
 
 case PET_TORORI:
 
 case PET_UNO:
 
 case PET_VETH:
 
 case PET_WALOONG_0:
 
 
 case PET_WALOONG_1:
 
 case PET_WALOONG_2:
 
 case PET_WALOONG_3:
 
 default:
 
 break;
 }
 */
void GameManager::scrollTheLayer(ui::ScrollView* scrollLayer, bool isLeft, bool isHorizontal, int howMuch){
    Point pos;
    if(isHorizontal){
        float x = scrollLayer->getInnerContainer()->getPosition().x + scrollLayer->getContentSize().width*(isLeft?1:-1)*howMuch;
        if(x > 0){
            x = 0;
        }else if(x < -(scrollLayer->getInnerContainerSize().width - scrollLayer->getContentSize().width)){
            x = -(scrollLayer->getInnerContainerSize().width - scrollLayer->getContentSize().width);
        }
        scrollLayer->getInnerContainer()->runAction(MoveTo::create(0.3, Point(x, scrollLayer->getInnerContainer()->getPosition().y)));
    }else{
        
    }
    /*
    return;
    int currentX = scrollLayer->getInnerContainer()->getPosition().x;
    int width = scrollLayer->getContentSize().width;
    int totalWidth = scrollLayer->getInnerContainerSize().width - width;
    if (isLeft){
        currentX += width;
        if(currentX > 0){
            currentX = 0;
        }
        if(isHorizontal)
            scrollLayer->scrollToPercentHorizontal(-currentX*100/totalWidth, 0.1, true);
        else
            scrollLayer->scrollToPercentVertical(-currentX*100/totalWidth, 0.1, true);
    }else{
        float rightEndX = -currentX + width;
        if(rightEndX > totalWidth){
            rightEndX = totalWidth;
        }
        int percent = rightEndX*100/totalWidth;
        if(isHorizontal)
            scrollLayer->scrollToPercentHorizontal(percent, 0.1, true);
        else
            scrollLayer->scrollToPercentVertical(percent, 0.1, true);
    }*/
}
Layer* GameManager::getTitleLayer(){
    return titleLayer;
}
void GameManager::setFontSize(Label* lbl, float size){
    TTFConfig config = lbl->getTTFConfig();
    config.fontSize = size;
    lbl->setTTFConfig(config);
}

void GameManager::showWaiting(Node* parent, Size size){
    Layout* intersectionLayer = Layout::create();
    intersectionLayer->setTouchEnabled(true);
    intersectionLayer->setContentSize(size);
    Button* background = Button::create("1506_logoBackground.png");
    background->setPosition(Point(size.width/2, size.height/2));
    background->setScaleX(size.width/background->getContentSize().width);
    background->setScaleY(size.height/background->getContentSize().height);
    intersectionLayer->addChild(background);
    background->setOpacity(180);
    
    Sprite* spt = Sprite::create("flyingDary.png");
    Point sptPos = Point(size.width/2, size.height/2 + 100);
    spt->setPosition(sptPos);
    intersectionLayer->addChild(spt);
    float dur = 0.3f;
    spt->runAction(RepeatForever::create(Sequence::create(MoveBy::create(dur, Point(0, 10)), MoveBy::create(dur, Point(0, -10)), NULL)));
    
    Label* lblTip = Label::createWithTTF("Please, wait...", GameManager::getInstance()->getFont(FONT_DEFAULT), 25);
    intersectionLayer->addChild(lblTip);
    Point lblPos = Point(size.width/2, size.height/2 - 60);
    lblTip->setPosition(lblPos);
    lblTip->setAlignment(TextHAlignment::CENTER, TextVAlignment::TOP);
    
    parent->addChild(intersectionLayer, 9000);
    waitingLayer = intersectionLayer;
}
void GameManager::closeWaiting(){
    if (waitingLayer) {
        waitingLayer->removeFromParentAndCleanup(true);
        waitingLayer= NULL;
    }
}

// stated as 00000000000
void GameManager::setWeaponCollected(int index){
    std::string data = UDGetStr(KEY_COLLECTION_WEAPON_FOUND, "");
    for (int i = 0; i <= index; i++) {
        if(data.length() <= i){
            data += Value(COLLECTION_NOT_FOUND).asString();
        }
        if(index == i){
            if(Value(data.substr(i, 1)).asInt() < COLLECTION_FOUND){
                data.erase(data.begin() + i);
                data.insert(data.begin() + i, Value(COLLECTION_FOUND).asString().at(0));
            }
            break;
        }
    }
}

Node* GameManager::findUp(Node* currentNode){
    Point pos = currentNode->getPosition();
    int unit = 10;
    Node* nodeFound = nullptr;
    while(pos.y < size.height){
        pos.y += unit;
        nodeFound = findWithDirect(pos, DIRECTION_LEFT, currentNode);
        if (nodeFound != nullptr) {
            break;
        }
        nodeFound = findWithDirect(pos, DIRECTION_RIGHT, currentNode);
        if (nodeFound != nullptr) {
            break;
        }
    }
    return nodeFound;
}
Node* GameManager::findDown(Node* currentNode){
    Point pos = currentNode->getPosition();
    int unit = 10;
    Node* nodeFound = nullptr;
    while(pos.y > 0){
        pos.y -= unit;
        nodeFound = findWithDirect(pos, DIRECTION_LEFT, currentNode);
        if (nodeFound != nullptr) {
            break;
        }
        nodeFound = findWithDirect(pos, DIRECTION_RIGHT, currentNode);
        if (nodeFound != nullptr) {
            break;
        }
    }
    return nodeFound;
}
Node* GameManager::findLeft(Node* currentNode){
    Point pos = currentNode->getPosition();
    int unit = 10;
    Node* nodeFound = nullptr;
    while(pos.x > 0){
        pos.x -= unit;
        nodeFound = findWithDirect(pos, DIRECTION_UP, currentNode);
        if (nodeFound != nullptr) {
            break;
        }
        nodeFound = findWithDirect(pos, DIRECTION_DOWN, currentNode);
        if (nodeFound != nullptr) {
            break;
        }
    }
    return nodeFound;
}
Node* GameManager::findRight(Node* currentNode){
    Point pos = currentNode->getPosition();
    int unit = 10;
    Node* nodeFound = nullptr;
    while(pos.x < size.width){
        pos.x += unit;
        nodeFound = findWithDirect(pos, DIRECTION_UP, currentNode);
        if (nodeFound != nullptr) {
            break;
        }
        nodeFound = findWithDirect(pos, DIRECTION_DOWN, currentNode);
        if (nodeFound != nullptr) {
            break;
        }
    }
    return nodeFound;
}

Node* GameManager::findWithDirect(Point pos, int direction, Node* currentNode){
    Node* currentPanel = currentNode->getParent();
    Point findingPos = Point(pos.x, pos.y);
    int unit = 10;
    Node* nodeFound = nullptr;
    while(true){
        if (direction == DIRECTION_UP && findingPos.y > size.height) break;
        else if (direction == DIRECTION_RIGHT && findingPos.x > size.width) break;
        else if (direction == DIRECTION_DOWN && findingPos.y < 0) break;
        else if (direction == DIRECTION_LEFT && findingPos.x < 0) break;
        
        if (direction == DIRECTION_UP) findingPos.y += unit;
        else if (direction == DIRECTION_RIGHT) findingPos.x += unit;
        else if (direction == DIRECTION_DOWN) findingPos.y -= unit;
        else if (direction == DIRECTION_LEFT) findingPos.x -= unit;
        nodeFound = getNodeAtThisPoint(findingPos, currentNode);
        if (nodeFound != nullptr) {
            break;
        }
    }
    return nodeFound;
}
Node* GameManager::getNodeAtThisPoint(Point pos, Node* currentNode){
    Node* currentPanel = currentNode->getParent();
    int unitTagLimit = 101;
    for (auto child: currentPanel->getChildren()){
        if (child->getTag() < unitTagLimit && child->getBoundingBox().containsPoint(pos) && child != currentNode && child->isVisible()) {
            return child;
        }
    }
    return nullptr;
}
DualWorld* GameManager::getDualWorld(){
    return currentDualLayer;
}
HelloWorld* GameManager::getWorld(){
    return currentStageLayer;
}
void GameManager::alignToCenter(Vector<Node*> array, float gap, float centerX){
//    float itemWidth = array.at(0)->getContentSize().width;
    float totalWidthInThisRow = 0;
    for(auto node: array){
        totalWidthInThisRow += node->getContentSize().width + gap;
    }
    totalWidthInThisRow -= gap;
//    int columnCount = array.size();
    float extraX = 0;
    for(auto node: array){
        node->setPositionX(centerX - totalWidthInThisRow/2 + array.at(0)->getContentSize().width/2 + extraX);
        extraX += node->getContentSize().width + gap;
    }
}
void GameManager::alignToCenter(Node* node0, Node* node1, float gap, float centerX, float offsetX){
    float totalWidth = node0->getContentSize().width*node0->getScaleX() + node1->getContentSize().width*node1->getScaleX() + gap;
    //    float centerX = centerNode->getPosition().x;
    node0->setPositionX(centerX - totalWidth/2 + node0->getContentSize().width*node0->getScaleX()/2 + offsetX);
    node1->setPositionX(centerX - totalWidth/2 + node0->getContentSize().width*node0->getScaleX() + gap + node1->getContentSize().width*node1->getScaleX()/2 + offsetX);
}
void GameManager::setTimeLeft(Label* lbl, int time){
    lbl->setString(getTimeLeftInString(time));
}
void GameManager::setTimeLeft(Text* lbl, int time){
    lbl->setString(getTimeLeftInString(time));
}
void GameManager::setTimeLeft(TextBMFont* lbl, int time){
    lbl->setString(getTimeLeftInString(time));
}
std::string GameManager::getTimeLeftInString(int time){
    int oneDay = 60*60*24;
    int oneHour = 60*60;
    int oneMin = 60;
    if (time >= oneDay*2) {
        return __String::createWithFormat("%dD", (int)(time/oneDay))->getCString();
    }else if(time >= oneDay) {
        int hour = (int)((int)time%oneDay)/oneHour;
        if (hour == 0) {
            return __String::createWithFormat("1D")->getCString();
        }else{
            return __String::createWithFormat("1D %dH", hour)->getCString();
        }
    }else {
        int sec = (int)((time%oneHour)%oneMin);
        if((int)(time/oneHour) > 0){
            return __String::createWithFormat("%02d:%02d:%02d", (int)(time/oneHour), (int)((time%oneHour)/oneMin), sec)->getCString();
        }else{
            return __String::createWithFormat("%02d:%02d", (int)((time%oneHour)/oneMin), sec)->getCString();
        }
    }
}
std::string GameManager::getTimeLeftInStringHMS(int time){
    int oneDay = 60*60*24;
    int oneHour = 60*60;
    int oneMin = 60;
    int sec = (int)((time%oneHour)%oneMin);
    if((int)(time/oneHour) > 0){
        return __String::createWithFormat("%02d:%02d:%02d", (int)(time/oneHour), (int)((time%oneHour)/oneMin), sec)->getCString();
    }else{
        return __String::createWithFormat("%02d:%02d", (int)((time%oneHour)/oneMin), sec)->getCString();
    }
}

int GameManager::getMaxTotalBulletCount(int weapon){
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
            return 100;
        case WEAPON_STRIKER: // 61
            return 20;
        case WEAPON_NIFE_THROWER: // 88
            return 50;
        case WEAPON_BALL_SHOOTER: // 111
            return 75;
        case WEAPON_FLAME_THROWER: // 207
            return 400;
        case WEAPON_ROCKET_LAUNCHER: // 132
            return 12;
        case WEAPON_BAZOOKA: // 44
            return 8;
        case WEAPON_MATT_A4: // 164
            return 35;
        case WEAPON_LASER_GUN: // 161
            return 250;
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
            return 200;
        default:
            return 40;
    }
}
float GameManager::getWeaponCoolTime(int weapon){
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
            return 2.5f;
            break;
        case WEAPON_MATT_A4: // 164
            return 0.3f;
            break;
        case WEAPON_LASER_GUN: // 161
            return 0.02f;
            break;
        case WEAPON_ENERGY_GUN: // 85
            return 0.4f;
            break;
        case WEAPON_GUIDED_ROCKET_LAUNCHER: // 107
            return 0.5f;
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

void GameManager::loadBagContent(){
    std::string bagContent = UDGetStr(KEY_BAG_CONTENT);
    ValueVector values = GM->split(bagContent, ",");
    bagContentArray.clear();
    for (int i = 0; i < values.size(); i++) {
        if(values.at(i).asString().length() <= 0) continue;
        BagItem* item = new BagItem();
        item->index = i;
        item->value = values.at(i).asInt();
        bagContentArray.push_back(item);
    }
}
void GameManager::saveBagContent(){
    std::string bagContent = "";
    for (int i = 0; i < bagContentArray.size(); i++) {
        bagContent.append(Value(bagContentArray.at(i)->value).asString());
        bagContent.append(",");
    }
    log("saved inventory: %s", bagContent.c_str());
    UDSetStr(KEY_BAG_CONTENT, bagContent);
//    std::string str = UDGetStr(KEY_BAG_CONTENT, ",");
//    log("after saved inventory: %s", str.c_str());
    SM->saveUserData(strmake("inventory=%s", UDGetStr(KEY_BAG_CONTENT).c_str()));
}
int GameManager::getItemCountInBag(int index){
    int count = 0;
    for (int i = 0; i < bagContentArray.size(); i++) {
        if(bagContentArray.at(i)->value%1000 == index){
            count++;
        }
    }
    return count;
}
void GameManager::setItemCollectionFound(int itemIndex, int state){
    int itemCount = 0;
    std::string key;
    int itemType = getItemType(itemIndex);
    int initNumber = 0;
    if(itemType == ITEM_TYPE_WEAPON){
        itemCount = TOTAL_WEAPON_COUNT;
        key = KEY_COLLECTION_WEAPON_FOUND;
        initNumber = WEAPON_BEGIN;
    }else if(itemType == ITEM_TYPE_SHIELD){
        itemCount = TOTAL_SHIELD_COUNT - SHIELD_BEGIN;
        key = KEY_COLLECTION_SHIELD_FOUND;
        initNumber = SHIELD_BEGIN;
    }else if(itemType == ITEM_TYPE_RUNE){
        itemCount = RUNE_END - RUNE_BEGIN;
        key = KEY_COLLECTION_RUNE_FOUND;
        initNumber = RUNE_BEGIN;
    }
    
    std::string data = UDGetStr(key.c_str(), "");
    int array[itemCount];
    int newState = 0;
    bool somethingChanged = false;
    for (int i = 0; i < itemCount; i++) {
        if(data.size() > i){
            newState = Value(data.substr(i, 1)).asInt();
        }else{
            newState = COLLECTION_NOT_FOUND;
            somethingChanged = true;
        }
        if(initNumber + i == itemIndex){
            if(newState < state){
                newState = state;
                somethingChanged = true;
            }
        }
        array[i] = newState;
    }
    if(somethingChanged){
        std::string newData = "";
        for(int i = 0; i < itemCount;i++){
            newData.append(Value(array[i]).asString());
        }
        UDSetStr(key.c_str(), newData);
        if(itemType == ITEM_TYPE_WEAPON){
            SM->saveUserData("weapon_collect=" + newData);
        }else if(itemType == ITEM_TYPE_SHIELD){
            SM->saveUserData("shield_collect=" + newData);
        }else if(itemType == ITEM_TYPE_RUNE){
            SM->saveUserData("rune_collect=" + newData);
        }
    }
}
bool GameManager::isCollectionHasNew(){
    return isCollectionHasNew(ITEM_TYPE_WEAPON) || isCollectionHasNew(ITEM_TYPE_SHIELD) ||isCollectionHasNew(ITEM_TYPE_RUNE);
}
bool GameManager::isCollectionHasNew(int itemType){
    int itemCount = 0;
    std::string key;
    int initNumber = 0;
    if(itemType == ITEM_TYPE_WEAPON){
        itemCount = TOTAL_WEAPON_COUNT;
        key = KEY_COLLECTION_WEAPON_FOUND;
        initNumber = WEAPON_BEGIN;
    }else if(itemType == ITEM_TYPE_SHIELD){
        itemCount = TOTAL_SHIELD_COUNT - SHIELD_BEGIN;
        key = KEY_COLLECTION_SHIELD_FOUND;
        initNumber = SHIELD_BEGIN;
    }else if(itemType == ITEM_TYPE_RUNE){
        itemCount = RUNE_END - RUNE_BEGIN;
        key = KEY_COLLECTION_RUNE_FOUND;
        initNumber = RUNE_BEGIN;
    }
    
    std::string data = UDGetStr(key.c_str(), "");
    int newState = 0;
    bool somethingNew = false;
    for (int i = 0; i < itemCount; i++) {
        if(data.size() > i){
            newState = Value(data.substr(i, 1)).asInt();
            
            if(newState == COLLECTION_FOUND){
                somethingNew = true;
                break;
            }
        }else{
            newState = COLLECTION_NOT_FOUND;
            somethingNew = true;
            break;
        }
    }
    return somethingNew;
}
    
void GameManager::addBagContent(int itemIndex, int level){
    BagItem* item = new BagItem();
    item->index = 0;
    item->value = itemIndex+level*1000;
    bagContentArray.insert(bagContentArray.begin(), item);
    updateBagItemIndices();
}
void GameManager::updateBagItemIndices(){
    BagItem* item;
    for (int i = 0; i < bagContentArray.size(); i++) {
        item = bagContentArray.at(i);
        item->index = i;
    }
}
BagItem* GameManager::getBagItem(int index){
    for(auto item : bagContentArray){
        if(item->index == index){
            return item;
        }
    }
    return nullptr;
}
int GameManager::getBagContentItem(int index){
    for(auto item : bagContentArray){
        if(item->index == index){
            return item->value%1000;
        }
    }
    return -1;
}
int GameManager::getBagContentLevel(int index){
    for(auto item : bagContentArray){
        if(item->index == index){
            return item->value/1000;
        }
    }
    return -1;
}
std::vector<BagItem*> GameManager::getBagContents(int filter){
    std::vector<BagItem*> array;
    int itemIndex = 0;
    int rareness;int itemType;
    for (int i = 0; i < bagContentArray.size(); i++) {
        itemIndex = bagContentArray.at(i)->value%1000;
        itemType = getItemType(itemIndex);
        rareness = getItemRareness(itemIndex);
        if (filter == BAG_FILTER_NONE ||
            (filter == BAG_FILTER_WEAPON && itemType == ITEM_TYPE_WEAPON) ||
            (filter == BAG_FILTER_SHIELD && itemType == ITEM_TYPE_SHIELD) ||
            (filter == BAG_FILTER_RUNE && itemType == ITEM_TYPE_RUNE) ||
            (filter == BAG_FILTER_WEAPON_1STAR && itemType == ITEM_TYPE_WEAPON && rareness == ITEM_RARENESS_1STAR) ||
            (filter == BAG_FILTER_WEAPON_2STAR && itemType == ITEM_TYPE_WEAPON && rareness == ITEM_RARENESS_2STAR) ||
            (filter == BAG_FILTER_WEAPON_3STAR && itemType == ITEM_TYPE_WEAPON && rareness == ITEM_RARENESS_3STAR) ||
            (filter == BAG_FILTER_WEAPON_4STAR && itemType == ITEM_TYPE_WEAPON && rareness == ITEM_RARENESS_4STAR) ||
            (filter == BAG_FILTER_WEAPON_5STAR && itemType == ITEM_TYPE_WEAPON && rareness == ITEM_RARENESS_5STAR) ||
            (filter == BAG_FILTER_SHIELD_1STAR && itemType == ITEM_TYPE_SHIELD && rareness == ITEM_RARENESS_1STAR) ||
            (filter == BAG_FILTER_SHIELD_2STAR && itemType == ITEM_TYPE_SHIELD && rareness == ITEM_RARENESS_2STAR) ||
            (filter == BAG_FILTER_SHIELD_3STAR && itemType == ITEM_TYPE_SHIELD && rareness == ITEM_RARENESS_3STAR) ||
            (filter == BAG_FILTER_SHIELD_4STAR && itemType == ITEM_TYPE_SHIELD && rareness == ITEM_RARENESS_4STAR) ||
            (filter == BAG_FILTER_SHIELD_5STAR && itemType == ITEM_TYPE_SHIELD && rareness == ITEM_RARENESS_5STAR)) {
            array.push_back(bagContentArray.at(i));
        }
    }
    return array;
}
int GameManager::getItemType(int value){
    value = value%1000;
    if (value >= RUNE_BEGIN) {
        return ITEM_TYPE_RUNE;
    }else if (value >= SHIELD_BEGIN && value < RUNE_BEGIN) {
        return ITEM_TYPE_SHIELD;
    }else if (value < SHIELD_BEGIN) {
        return ITEM_TYPE_WEAPON;
    }
    return ITEM_TYPE_RUNE;
}
Sprite* GameManager::getItemSprite(int index){
    if(getItemType(index) == ITEM_TYPE_WEAPON){
        Sprite* sptWeapon = Sprite::createWithSpriteFrameName(getWeaponImageFileName(index%1000));
        sptWeapon->setScale(0.5f);
        return sptWeapon;
    }else if(getItemType(index) == ITEM_TYPE_SHIELD){
        return Sprite::createWithSpriteFrameName(strmake("shield/shield%d", (index%1000)%600));
    }else if(getItemType(index) == ITEM_TYPE_RUNE){
        int runeIndex = (index%100)/4;
        Sprite* spt = Sprite::create(strmake("runeStone%d.png", runeIndex));
        Sprite* symbol = Sprite::create(strmake("runeSymbol%d.png", runeIndex));
        int symbolIndex = (index%100)%4;
        if(symbolIndex == 0){
            symbol->setColor(Color3B::RED);
        }else if(symbolIndex == 1){
            symbol->setColor(Color3B::YELLOW);//Color3B(180, 170, 50));
        }else if(symbolIndex == 2){
            symbol->setColor(Color3B::GREEN);//(40, 180, 190));
        }else if(symbolIndex == 3){
            symbol->setColor(Color3B(50, 215, 255));
        }
        symbol->setAnchorPoint(Vec2::ZERO);
        spt->addChild(symbol);
        return spt;
    }
    return nullptr;
}
int GameManager::getItemRareness(int value){
    if(getItemType(value) == ITEM_TYPE_WEAPON){
        return getWeaponRareness(value);
    }else if(getItemType(value) == ITEM_TYPE_SHIELD){
        return getShieldRareness(value);
    }
    return ITEM_RARENESS_NORMAL;
}
int GameManager::getWeaponRareness(int index){
    index = index%1000;
    if(index < WEAPON_2STAR_START){
        return ITEM_RARENESS_1STAR;
    }else if(index < WEAPON_3STAR_START){
        return ITEM_RARENESS_2STAR;
    }else if(index < WEAPON_4STAR_START){
        return ITEM_RARENESS_3STAR;
    }else if(index < WEAPON_5STAR_START){
        return ITEM_RARENESS_4STAR;
    }else if(index < TOTAL_WEAPON_COUNT){
        return ITEM_RARENESS_5STAR;
    }
    return ITEM_RARENESS_NORMAL;
}
int GameManager::getShieldRareness(int index){
    index = index%1000;
    if(index >= SHIELD_BEGIN && index < SHIELD_2STAR_START){
        return ITEM_RARENESS_1STAR;
    }else if(index >= SHIELD_2STAR_START && index < SHIELD_3STAR_START){
        return ITEM_RARENESS_2STAR;
    }else if(index >= SHIELD_3STAR_START && index < SHIELD_4STAR_START){
        return ITEM_RARENESS_3STAR;
    }else if(index >= SHIELD_4STAR_START && index < SHIELD_5STAR_START){
        return ITEM_RARENESS_4STAR;
    }else if(index >= SHIELD_5STAR_START && index <= TOTAL_SHIELD_COUNT){
        return ITEM_RARENESS_5STAR;
    }
    return ITEM_RARENESS_NORMAL;
}
void GameManager::removeBagContentWithoutUpdateOrder(int index){
    BagItem* item;
    for(int i = 0; i < bagContentArray.size(); i++){
        item = bagContentArray.at(i);
        if(item->index == index){
            bagContentArray.erase(bagContentArray.begin() + i);
            return;
        }
    }
}
void GameManager::removeBagContent(int index){
    removeBagContentWithoutUpdateOrder(index);
    updateBagItemIndices();
}
float GameManager::getShieldDef(int index, int rank){
    return getShieldDef(index)*(1 + rank*0.1f);
}
float GameManager::getShieldHp(int index, int rank){
    return getShieldHp(index)*(1 + rank*0.1f);
}
float GameManager::getShieldHPRuneApplied(int value){
    float hp = GM->getShieldHp(value%1000, value/1000);
    float effectHPAmount = 0;
    for (int j = 0; j < 4; j++) {
        int rune = UDGetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, j).c_str(), -1);
        if(rune%4 == value%4 && GM->getRuneBuffType(rune) == RUNE_BUFF_TYPE_HP){
            effectHPAmount += GM->getRuneBuff(rune/1000, RUNE_BUFF_TYPE_HP);
        }
    }
    return hp*(1 + effectHPAmount*0.01f);
}

float GameManager::getShieldDefRuneApplied(int value){
    int effectDefAmount = getShieldDef(value);
    for (int j = 0; j < 4; j++) {
        int rune = UDGetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, j).c_str(), -1);
        if(rune%4 == value%4 && GM->getRuneBuffType(rune) == RUNE_BUFF_TYPE_DEF){
            effectDefAmount += GM->getRuneBuff(rune/1000, RUNE_BUFF_TYPE_DEF);
        }
    }
    return effectDefAmount;
}
float GameManager::getShieldHp(int index){
    index = index%1000;
    switch (index) {
        case SHIELD_WOOD: return 30;
        case SHIELD_WOOD_ROUND: return 35;
        case SHIELD_WOOD_CROSS: return 40;
        case SHIELD_INDIAN: return 45;
        case SHIELD_WOOD_GUARD : return 65;
        case SHIELD_STEEL : return 75;
        case SHIELD_IRON : return 85;
        case SHIELD_METAL : return 95;
        case SHIELD_RED : return 125;
        case SHIELD_GOLD : return 140;
        case SHIELD_GREEN : return 155;
        case SHIELD_BLUE : return 170;
        case SHIELD_KNIGHT : return 220;
        case SHIELD_PALADIN : return 250;
        case SHIELD_LANCER : return 280;
        case SHIELD_TANKER : return 310;
        case SHIELD_VEGAS : return 400;
        case SHIELD_PHONIX : return 440;
        case SHIELD_EYE : return 480;
        case SHIELD_ATHENA : return 520;
        default:
            break;
    }
    return 50;
}
float GameManager::getShieldDef(int index){
    index = index%1000;
    switch (index) {
        case SHIELD_WOOD:
            return 0;
        case SHIELD_INDIAN:
            return 1;
        case SHIELD_WOOD_GUARD:
            return 1;
        case SHIELD_STEEL:
            return 1.2f;
        case SHIELD_IRON:
            return 1.2f;
        case SHIELD_METAL:
            return 1.6f;
        case SHIELD_RED:
            return 1.6f;
        case SHIELD_GOLD:
            return 1.7f;
        case SHIELD_GREEN:
            return 1.8f;
        case SHIELD_BLUE:
            return 1.9f;
        case SHIELD_KNIGHT:
            return 2.2f;
        case SHIELD_PALADIN:
            return 2.3f;
        case SHIELD_LANCER:
            return 2.4f;
        case SHIELD_TANKER:
            return 2.5f;
        case SHIELD_VEGAS:
            return 2.6f;
        case SHIELD_PHONIX:
            return 3.0f;
        case SHIELD_EYE:
            return 3.2f;
        case SHIELD_ATHENA:
            return 3.4f;
        default:
            break;
    }
    return 0;
}
int GameManager::getRuneBuffForEffectee(int runeValue, int effecteeValue){
    int runeBuffType =  getRuneBuffType(runeValue);
    if(getItemType(effecteeValue) == ITEM_TYPE_WEAPON && runeBuffType == RUNE_BUFF_TYPE_ATTACK){
        return getRuneBuff(runeValue, RUNE_BUFF_TYPE_ATTACK);
    }else if(getItemType(effecteeValue) == ITEM_TYPE_WEAPON && runeBuffType == RUNE_BUFF_TYPE_CRI){
        return getRuneBuff(runeValue, RUNE_BUFF_TYPE_CRI);
    }else if(getItemType(effecteeValue) == ITEM_TYPE_SHIELD && runeBuffType == RUNE_BUFF_TYPE_HP){
        return getRuneBuff(runeValue, RUNE_BUFF_TYPE_HP);
    }else if(getItemType(effecteeValue) == ITEM_TYPE_SHIELD && runeBuffType == RUNE_BUFF_TYPE_DEF){
        return getRuneBuff(runeValue, RUNE_BUFF_TYPE_DEF);
    }
    return 0;
}
int GameManager::getRuneBuffType(int runeValue){
    if(runeValue >= RUNE_A_RED && runeValue <= RUNE_A_BLUE){
        return RUNE_BUFF_TYPE_ATTACK;
    }else if(runeValue >= RUNE_D_RED && runeValue <= RUNE_D_BLUE){
        return RUNE_BUFF_TYPE_DEF;
    }else if(runeValue >= RUNE_S_RED && runeValue <= RUNE_S_BLUE){
        return RUNE_BUFF_TYPE_HP;
    }else{// if(runeValue >= RUNE_Q_RED && runeValue <= RUNE_Q_BLUE){
        return RUNE_BUFF_TYPE_CRI;
    }
}
int GameManager::getRuneBuff(int rank, int runeBuffType){
    switch(runeBuffType){
        case RUNE_BUFF_TYPE_ATTACK:
            return (rank + 1)*10;
        case RUNE_BUFF_TYPE_DEF:
            return (rank + 1);
        case RUNE_BUFF_TYPE_HP:
            return (rank + 1)*10;
        case RUNE_BUFF_TYPE_CRI:
            return (rank + 1)*2;
    }
    return 1;
}
void GameManager::showPopup(Node* popup){
    popup->setScale(0.1f);
    if(popup->getAnchorPoint() == Vec2::ZERO){
        popup->setAnchorPoint(Vec2(0.5, 0.5));
        popup->setPosition(Vec2(size.width/2, size.height/2));
    }
    popup->runAction(EaseBackOut::create(ScaleTo::create(0.3f, 1)));
}
Color3B GameManager::getRandomColor(){
    int ranNum = cocos2d::random()%6;
    switch (ranNum) {
        case 0:
            return Color3B(255, cocos2d::random()%255, cocos2d::random()%255);
        case 1:
            return Color3B(cocos2d::random()%255, 255, cocos2d::random()%255);
        case 2:
            return Color3B(cocos2d::random()%255, cocos2d::random()%255, 255);
        case 3:
            return Color3B(255, 255, cocos2d::random()%255);
        case 4:
            return Color3B(cocos2d::random()%255, 255, 255);
        case 5:
            return Color3B(255, cocos2d::random()%255, 255);
        default:
            return Color3B::WHITE;
    }
}
void GameManager::enableButton(Ref* ref){
    BTN_FROM_REF
    btn->setEnabled(true);
}
int GameManager::getHeroHP(){
    int level = UDGetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, 0).c_str(), 0);
    return 100 + level*10;
}
int GameManager::getHeroDefence(){
    int level = UDGetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, 1).c_str(), 0);
    return 5 + level*1;
}
int GameManager::getHeroDefenceBuff(){
    int level = UDGetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, 2).c_str(), 0);
    return 0 + level*1;
}
float GameManager::getHeroDodge(){
    int level = UDGetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, 3).c_str(), 0);
    return level*0.2f;
}

int GameManager::getHeroNextHP(){
    int level = UDGetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, 0).c_str(), 0) + 1;
    return 100 + level*10;
}
int GameManager::getHeroNextDefence(){
    int level = UDGetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, 1).c_str(), 0) + 1;
    return 5 + level*1;
}
int GameManager::getHeroNextDefenceBuff(){
    int level = UDGetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, 2).c_str(), 0) + 1;
    return 0 + level*1;
}
float GameManager::getHeroNextDodge(){
    int level = UDGetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, 3).c_str(), 0) + 1;
    return 0 + level*0.1f;
}
    

float GameManager::getResearchCoolTime(){
    int level = UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 0).c_str(), 0);
    return getResearchCoolTime(level);
}
float GameManager::getResearchCoolTime(int level){
    return level*-1;
}
int GameManager::getResearchPower(){
    int level = UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 1).c_str(), 0);
    return getResearchPower(level);
}
int GameManager::getResearchPower(int level){
    return level;
}
float GameManager::getResearchCriticalPossibility(){
    int level = UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 2).c_str(), 0);
    return getResearchCriticalPossibility(level);
}
float GameManager::getResearchCriticalPossibility(int level){
    return level*0.1f;
}
int GameManager::getResearchCriticalPower(){
    int level = UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 3).c_str(), 0);
    return getResearchCriticalPower(level);
}
int GameManager::getResearchCriticalPower(int level){
    return 200 + level*10;
}
float GameManager::getResearchNextCoolTime(){
    int level = UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 0).c_str(), 0) + 1;
    return getResearchCoolTime(level);
}
int GameManager::getResearchNextPower(){
    int level = UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 1).c_str(), 0) + 1;
    return getResearchPower(level);
}
float GameManager::getResearchNextCriticalPossibility(){
    int level = UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 2).c_str(), 0) + 1;
    return getResearchCriticalPossibility(level);
}
int GameManager::getResearchNextCriticalPower(){
    int level = UDGetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, 3).c_str(), 0) + 1;
    return getResearchCriticalPower(level);
}
/*
 * Erase First Occurrence of given  substring from main string.
 */
void GameManager::eraseSubStr(std::string & mainStr, const std::string & toErase)
{
    // Search for the substring in string
    size_t pos = mainStr.find(toErase);
    
    if (pos != std::string::npos)
    {
        // If found then erase it from string
        mainStr.erase(pos, toErase.length());
    }
}
std::string GameManager::getLocalBattleEnemyEquipInString(int index){
    std::string str = strmake("%d,%d,%d,%d", index+1, index+2, index+3, index+4);
//    if(index == 0){
//       str = strmake("1,3,-1,-1,%d,%d,%d", -1, -1, -1);
//    }else if(index == 1){
//        str = strmake("%s,%d,%d,%d", str.c_str(), 600, -1, -1);
//    }else if(index == 2){
//        str = strmake("%s,%d,%d,%d", str.c_str(), 601, 602, -1);
//    }else if(index == 3){
//        str = strmake("%s,%d,%d,%d", str.c_str(), 601, 602, 600);
//    }else if(index == 4){
//        str = strmake("%s,%d,%d,%d", str.c_str(), 602, 603, 601);
//    }else if(index == 5){
//        str = strmake("%s,%d,%d,%d", str.c_str(), 601, 604, 601);
//    }else if(index == 6){
//        str = strmake("%s,%d,%d,%d", str.c_str(), 602, 604, 605);
//    }else{
//        str = strmake("%s,%d,%d,%d", str.c_str(), 600 + index-4, 600 + index - 3, 600 + index - 2);
//    }
    if(index == 0){
        str = strmake("%d,%d,%d,%d,%d,%d,%d,%d", WEAPON_MAGNUM, WEAPON_UZI,-1, -1, 600, -1, -1, -1);
    }else if(index == 1){
        str = strmake("%d,%d,%d,%d,%d,%d,%d,%d", WEAPON_UZI, WEAPON_RIFLE, WEAPON_FIRE_BOTTLER, -1, 601, 602, -1, -1);
    }else if(index == 2){
        str = strmake("%d,%d,%d,%d,%d,%d,%d,%d", WEAPON_RIFLE, WEAPON_FIRE_BOTTLER, WEAPON_ANTI_BAT, WEAPON_SNIPER_RIFLE, 601, 602, 603, -1);
    }else if(index >= 3 && index <= 18){
        str = strmake("%d,%d,%d,%d,%d,%d,%d,%d", WEAPON_FIRE_BOTTLER + index - 3, WEAPON_ANTI_BAT + index - 3, WEAPON_SNIPER_RIFLE + index - 3, WEAPON_RIFLE_GRENADE + index - 3, 601 + index - 3, 602 + index - 3, 603 + index - 3, 604 + index - 3);
    }else if(index == 19){
        str = strmake("%d,%d,%d,%d,%d,%d,%d,%d", WEAPON_SLUGGER, WEAPON_DRAGON_RIFLE, WEAPON_SHARK_GUN, WEAPON_SLUGGER_NG, 617, 618, 618, 619);
    }else if(index == 20){
        str = strmake("%d,%d,%d,%d,%d,%d,%d,%d", WEAPON_DRAGON_RIFLE, WEAPON_SHARK_GUN, WEAPON_SLUGGER_NG, WEAPON_LIGHTNING_GUN, 618, 618, 619, 619);
    }else if(index == 21){
        str = strmake("%d,%d,%d,%d,%d,%d,%d,%d", WEAPON_SHARK_GUN, WEAPON_SLUGGER_NG, WEAPON_LIGHTNING_GUN, WEAPON_LASER_COIN, 619, 619, 619, 619);
    }
    return str;
}
std::vector<int> GameManager::getRandomNumbers(int minInclusive, int maxExclusive, int count){
    std::vector<int> list;
    if(maxExclusive-minInclusive < count){
        log("getRandomNumbers error count is bigger than max - min");
        return list;
    }
    int number;
    for (int i = 0; i < count; i++) {
        do{
            number = minInclusive + rand()%(maxExclusive-minInclusive+1);
        }while(std::find(list.begin(), list.end(), number) != list.end());
        list.push_back(number);
    }
    return list;
}
    
void GameManager::setCollectMission(){
    int enemyIndex = UDGetInt(KEY_UNLOCKED_LOCAL_ENEMY, 0);
    if(enemyIndex < 3){
        enemyIndex = 3;
    }
    for (int i = 0; i < 4; i++) {
        UDSetInt(strmake(KEY_COLLECT_MISSION_TYPE_FORMAT, i).c_str(), enemyIndex);
        UDSetInt(strmake(KEY_COLLECT_MISSION_MAX_COUNT_FORMAT, i).c_str(), 2);
        UDSetInt(strmake(KEY_COLLECT_MISSION_COLLECTED_COUNT_FORMAT, i).c_str(), 0);
        enemyIndex--;
        if(enemyIndex < 0){
            enemyIndex = 0;
        }
    }
    SM->saveUserData("mission=0000");
}
// Make a tm structure representing this date
std::tm make_tm(int year, int month, int day)
{
    std::tm tm = {0};
    tm.tm_year = year - 1900; // years count from 1900
    tm.tm_mon = month - 1;    // months count from January=0
    tm.tm_mday = day;         // days count from 1
    return tm;
}

int GameManager::getDayDiff(int fromY, int fromM, int fromD, int toY, int toM, int toD){
    
    // Structures representing the two dates
    std::tm tm1 = make_tm(toY,toM,toD);    // April 2nd, 2012
    std::tm tm2 = make_tm(fromY,fromM,fromD);    // February 2nd, 2003
    
    // Arithmetic time values.
    // On a posix system, these are seconds since 1970-01-01 00:00:00 UTC
    std::time_t time1 = std::mktime(&tm1);
    std::time_t time2 = std::mktime(&tm2);
    
    // Divide by the number of seconds in a day
    const int seconds_per_day = 60*60*24;
    std::time_t difference = (time1 - time2) / seconds_per_day;
    
    // To be fully portable, we shouldn't assume that these are Unix time;
    // instead, we should use "difftime" to give the difference in seconds:
    double portable_difference = std::difftime(time1, time2) / seconds_per_day;
    return portable_difference;
}
Sprite* GameManager::getAvatarIcon(int index){
    Sprite* spt = nullptr;
//    if(index == 0){
//        spt = Sprite::createWithSpriteFrameName("amor_stand0body.png");
//    }else if(false){
//        spt = Sprite::createWithSpriteFrameName(strmake("avatar%d_stand0.png", index));
//    }else{
//        spt = Sprite::createWithSpriteFrameName(strmake("avatar%d_stand0.png", index));
//    }
    spt = Sprite::createWithSpriteFrameName(strmake("avatar%d_stand0.png", index));
    return spt;
}
