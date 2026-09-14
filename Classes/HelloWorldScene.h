#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Droppable.h"
#include "Hero.h"
#include "Enemies.h"
#include "AwesomeDialogBox.h"
#include "MovingPlatform.h"
#include "Trap.h"
#include "TriggerDoorInfo.h"
#include "Buddies.h"
#include "Laser.h"
#include "../cocos2d/cocos/editor-support/cocostudio/CocoStudio.h"
#include "../cocos2d/cocos/ui/UILayout.h"
#include "../cocos2d/cocos/ui/UIHelper.h"
#define TAG_TELEPORT_0_ENTRY 0
#define TAG_TELEPORT_0_EXIT 1
#define TAG_TELEPORT_1_ENTRY 2
#define TAG_TELEPORT_1_EXIT 3
#define TAG_TELEPORT_2_ENTRY 4
#define TAG_TELEPORT_2_EXIT 5

#define MAX_DROP_SPEED -10
#define MAX_COIN_COUNT 691

#define MAP_FOUND 0
#define MAP_NORMAL -1
#define MAP_BLUE_KEY -2
#define MAP_WEAPON -3
#define MAP_BOSS_GATE -4
#define MAP_SHOP -5
#define MAP_FINE -6

//#include "../cocos2d/cocos/ui/UILayout.h"
//#include "../cocos2d/cocos/ui/UIButton.h"
//#include "../cocos2d/cocos/ui/UIText.h"
//#include "../cocos2d/cocos/ui/UIImageView.h"
//#include "../cocos2d/cocos/ui/UITextField.h"
//#include "../cocos2d/cocos/ui/UIHelper.h"

//#include "../cocos2d/cocos/2d/CCFastTMXTiledMap.h"
//#include "../cocos2d/cocos/2d/CCFastTMXLayer.h"
using namespace cocos2d;
//using namespace cocostudio;
//using namespace cocos2d::ui;
class HelloWorld : public cocos2d::Layer
{
protected:
    float stickTimeLeft=0;
    void updateIndicators();
    Vector<Sprite*> indicatorArray;
    Vector<Droppable*> indicatedArray;
    std::vector<int> shopWeaponItem;
    float missileEffectCollapsedTime;
    Vector<Missile*> enemyMissileArray;
    Vector<EnemyBase*> flyingSwimingEnemyArray;
    Vector<Droppable*> droppableArray;
    Vector<Droppable*> straightMovingArray;
    Vector<Droppable*> customMovingArray;
    Vector<Droppable*> chasingMissileArray;
    Vector<Droppable*> droppableMissileArray;
    Vector<Droppable*> droppableCoinArray;
    Vector<Mace*> maceArray;
    Vector<Mace*> axeArray;
    Vector<FireStick*> fireStickArray;
    Vector<Sprite*> teleportArray;
    Vector<DestructablePlatform*> destructablePlatformArray;
    void startFallingDestructablePlatform(DestructablePlatform* platform);
    void destructablePlatformShakingDone(Ref* obj);
    void destructablePlatformWaitingDone(Ref* obj);
    Vector<EnemyBase*> enemiesToRemove;
    int skyBuddyHeight;
    int skyBuddyCurrentHeight;
    bool isSkyBuddyFloatingUp;
    int skyBuddyFloatingHeight;
    int skyBuddyXDistanceMax;
    int skyBuddyXDistance;
    Buddy* groundBuddy;
    cocos2d::PointArray* heroPositionArray;
    int extraCritical;
    int extraPower;
    int extraFruit;
    void sacrificeLater(float dt);
    void sacrificeDone(Ref* obj);
    void splashDemage(cocos2d::Point pos, int radius, int demage);

    void testSchedule(float dt);
    Sprite* testDragon;
    Sprite* testPet;
    Sprite* testPet2;

    int totalCoinCount;
//    int getEpisodeIndex();
//    void episodeActionNeeded();
//    void episodeActionFinished();
//    int episodeActionIndex;
//    void showEpisode();
//    EpisodeLayer* episodeLayer;
//    void episodeDone();
//    Droppable* episodeNpc;
    bool testingFlag = false;
    Laser* laser=nullptr;
    std::vector<cocos2d::Rect> stageTileRectArray;
    Sprite* blackSquare;
    cocos2d::Size size;
    Vector<Droppable*> coinArray;
    Vector<Droppable*> starArray;
    Vector<Trap*> trapArray;
    Vector<Missile*> heroMissileArray;
    Vector<MovingPlatform*> movingPlatformArray;
    Vector<Droppable*> helperArray;
    Vector<Droppable*> inWaterArray;
    Vector<Droppable*> bubbleArray;
    Vector<FireableBase*> fireableArray;
    Vector<Droppable*> spiderArray;
    Vector<TriggerDoorInfo*> triggerDoorArray;
    Vector<Droppable*> coinToRemove;
    Sprite* nearBack;
    Sprite* farBack;
    Sprite* furtherBack;
	AwesomeDialogBox* _dialogBox;
    Droppable* theDoor;
    Droppable* theKey;
    Vector<Droppable*>talkArray;
    Vector<Label*> talkLabelArray;
    float indestructableTime;
    int playerWeaponType;
//    int playerMissileDemage;
    void destroyDestructable(Droppable* drop);
    int roomNumber=0;
    int currentTalkIndex;
    int starCount;
    float coinDelay;
    float otherDelay;
    bool bulletWasted;
    cocos2d::Point deadPoint;
    bool guidedMissile;
    Droppable* findTarget(int distance);
    Sprite* activityIndicator;
    int criticalLevel;
    
    int furtherWidth;
    int furtherHeight;
    int farWidth;
    int farHeight;
    int nearWidth;
    int nearHeight;
    void solveCollision(Droppable* p, cocos2d::Point pos, cocos2d::Rect rect);
    void removeLaser();
    Vector<TMXTiledMap*> mapArray;
    Vector<TMXTiledMap*> revealedMapArray;
    std::vector<cocos2d::Rect> mapRectArray;
    DrawNode* dnMiniMap;
    cocos2d::Size miniMapSize = cocos2d::Size(200,160);
    cocos2d::Size currentMapSize = miniMapSize;
    int miniMapAlpha=255;
    bool isMiniMapFadeIn=false;
    void fadeMiniMap(float dt);
    bool isRoomEmpty(cocos2d::Rect rect);
    TMXTiledMap* findDirectionAndPlaceTheMap(TMXTiledMap* srcMap, TMXTiledMap* dstMap);
    void setEntireMap(int stage);
    void setBossMap(int stage);
    bool intersectsRect(cocos2d::Rect srcRect, cocos2d::Rect dstRect);
    void addMapToMiniMap(TMXTiledMap* map);
    void updateMiniMap();
    TheKnight* theKnight;
    int suitcaseCount = 0;
    
public:
    Vector<EnemyBase*> destructableArray;
    Vector<EnemyBase*> enemyArray;
    Vector<NPCBase*> npcArray;
    void onjewelryBoxOpen(Ref* sender);
    void showBigMiniMap(bool isBig);
    Buddy* skyBuddy=nullptr;
    Vector<Buddy*> buddyArray;
    float getMin(float src, float dst);
    float getMax(float src, float dst);
    cocos2d::Point touchStart;
    bool isTouchStarted;
    float playerFireCoolTime = 0.3f;
    float playerFireCoolTimeMax = 0.3f;
    float reloadingCoolTime;
    float reloadingCoolTimeSpan;
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object cocos2d::Pointer)
    virtual bool init();
    Hero* player = nullptr;
    Sprite* heroLight;
    void showDamage(int damage, cocos2d::Point pos);
//    bool leftPressed;
//    bool rightPressed;
//    bool firePressed;
//    bool jumpPressed;
    int collectedCoinCount;
    bool isGameOver;
    bool keyCollected;
    bool isBlock(int index);
    bool isOneWay(int index);
    bool isFregile(int index);
    int mastery=0;
    int alphabetCount=0;
    void setNamingRoom(TMXTiledMap* map);
    void setLobby(TMXTiledMap* map);
    void setEmptyMap(TMXTiledMap* map);
    int itemCount=0;
    PointArray* enemyPointArray;
    Vector<EnemyBase*> enemiesToLoad;
    void loadEnemies();
    void addAppearEffect(Sprite* parent);
    FireableBase* addEnemyToLoadStack(TMXTiledMap* map, int levelScore, cocos2d::Point pos, int missile, int enemyModel, bool addGround);
    void addEnemiesToMap(TMXTiledMap* map, int levelScore, bool blueKey);
    int currentLevelScore;
    long currentScore=0;
    void tournamentSchedule(float dt);
    int tournamentWaveIndex=0;
    FireableBase* theBoss;
    void removeMeFromChasing(Ref* ref);
    void initBoss(int boss);
    void bossExplode(float dt);
    void makeGate(float dt);
    cocos2d::Point playerSpawnPoint;
    void createBossRewards();
    void angelReviveLater(float dt);
    void bossScheduleBazooka(float dt);
    void bossScheduleGiantPenguin(float dt);
    void bossScheduleGiantWood(float dt);
    void bossScheduleGiantFish(float dt);
    void cloudSchedule(float dt);
    Vector<Sprite*> cloudArray;
    float bossScheduleTime;
    int bossStep;
    int bossInnerStep;
    void bossClear();
    float lastClearY;
    int enemyKilledCount;
    int enemyMaxCount;
    void rateResult(Node* node);
    void revive();
    float checkBottom(Droppable* p);
    bool isSetStageDone;
    int stageIndex;
    void addWeapon(int weapon, int level = 0);
    int getWeaponAtSlot(int slot);
    void setWeaponAtSlot(int slot, int weapon);
    void changeWeapon(int slot);
    int getTotalBulletCountAtSlot(int slot);
    void setTotalBulletCountAtSlot(int slot, int count);
    int getLoadedBulletCountAtSlot(int slot);
    void setLoadedBulletCountAtSlot(int slot, int count);
    int currentSlot;
    cocos2d::Point positionToTeleport;
    void teleportLater(float dt);
    void teleportLaterForThemeThree(float dt);
    bool isTeleporting;
    bool playerIgnoreGravity;
    Sprite* getTeleport(int teleport);
    // there's no 'id' in cpp, so we recommend to return the class instance cocos2d::Pointer
    static cocos2d::Scene* scene(int stage, bool boss = false);
    SpriteBatchNode* spriteBatch;
    SpriteBatchNode* spriteBatchForHero;
//    SpriteBatchNode* coinSpriteBatch;
    SpriteBatchNode* backgroundSpriteBatch;
    // a selector callback
    void menuCloseCallback(Ref* pSender);

    void removeUsedAssets();
    EventListenerTouchOneByOne* listener;
    HelloWorld();
//    ~HelloWorld();
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(HelloWorld);
    void unscheduleAll();
    int killCountForRecord;
    int collectedCoinPart;
    Label* lblCoinGot = nullptr;
    int goldChestCount = 0;
    bool readyToMove = false;
    void attackEnemy(EnemyBase* drop, int damage);
    void setViewPointCenter(cocos2d::Point position);
    void registerScriptAccelerateHandler(int handler);
    double getAngle(cocos2d::Point pos1, cocos2d::Point pos2);
    void setPlayerPosition(cocos2d::Point position);
    void bubbleUpdate(float dt);
    void gravityUpdate(float dt);
    void gravityUpdateForCoins(float dt);
    void gravityUpdateForStraight(float dt);
    void gravityUpdateForCustomMoving(float dt);
    bool hitDary(float damage);
    void useBomb();
    void usePotion();
    int demagingUnit;
    void doTheBombToMissiles(float dt);
    void doTheBombToEnemies(float dt);
    Vector<Missile*> bombTargetMissileArray;
    int bombTargetEnemyIndex;
    Vector<EnemyBase*> bombTargetEnemyArray;
//    void gravityUpdateForOthers(float dt);
//    void gravityUpdateForOnlyCheckCollision(float dt);
    void gravityUpdateForFlyingOrSwimingEnemies(float dt);
    void gravityUpdateForWaterEnemies(float dt);
    void chasingMissileUpdate(float dt);
    void missileEffectUpdate(float dt);
    void coinUpdate(float dt);
    void npcUpdate(float dt);
    void enemyUpdate(float dt);
    void destructableUpdate();
    void missileUpdate(float dt);
    void talkUpdate(float dt);
    void updateFireStick(float dt);
    void helperAndTrapUpdate(float dt);
    void createRewards(EnemyBase* enemy);
    bool keyExist(ValueMap map, const char* key);
    void takeAction();
    float powerTestTime;
    void powerTestSchedule(float dt);
    float powerPerSec;
    float totalPower;
    void coinMagnet();
    bool isPaused;
    bool isVideoRewardAttack;
    bool everySecond;
    void Dispose();
    void playReloadingSoundLater(float dt);
//    int mapHight;
//    int mapRowCount;
//    int mapColumnCount;
    TMXLayer* stageLayer;
//    TMXLayer* foreLayer;
    void setCurrentTileMap(cocos2d::Point pos);
    TMXTiledMap* getTileMap(cocos2d::Point pos);
    void setLayerTag(TMXTiledMap* map);
    
//    cocos2d::TMXTiledMap *currentTileMap;
//    Vector<TileInfo*>getSurroundingTilesAtPosition(cocos2d::Point position, cocos2d::TMXLayer *layer, Droppable* p);
//    Vector<TileInfo*>getAllSurroundingTilesAtPosition(cocos2d::Point position, cocos2d::TMXLayer *layer);
    int getTileAtPosition(cocos2d::Point position, int tag, TMXTiledMap* map);
/*
 
 cocos2d::experimental::TMXTiledMap *tileMap;
 cocos2d::experimental::TMXLayer *stageLayer;
 cocos2d::experimental::TMXLayer *unitLayer;
 cocos2d::experimental::TMXLayer *foreLayer;
 cocos2d::experimental::TMXLayer *backLayer;
 Vector<TileInfo*>getSurroundingTilesAtPosition(cocos2d::Point position, cocos2d::experimental::TMXLayer *layer, Droppable* p);
 Vector<TileInfo*>getAllSurroundingTilesAtPosition(cocos2d::Point position, cocos2d::experimental::TMXLayer *layer);
 TileInfo* getTileAtPosition(cocos2d::Point position, cocos2d::experimental::TMXLayer *layer);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    cocos2d::experimental::TMXTiledMap *tileMap;
    cocos2d::experimental::TMXLayer *stageLayer;
    cocos2d::experimental::TMXLayer *unitLayer;
    cocos2d::experimental::TMXLayer *foreLayer;
    cocos2d::experimental::TMXLayer *backLayer;
    Vector<TileInfo*>getSurroundingTilesAtPosition(cocos2d::Point position, cocos2d::experimental::TMXLayer *layer, Droppable* p);
    Vector<TileInfo*>getAllSurroundingTilesAtPosition(cocos2d::Point position, cocos2d::experimental::TMXLayer *layer);
    TileInfo* getTileAtPosition(cocos2d::Point position, cocos2d::experimental::TMXLayer *layer);
#else
    cocos2d::TMXTiledMap *tileMap;
    cocos2d::TMXLayer *stageLayer;
    cocos2d::TMXLayer *unitLayer;
    cocos2d::TMXLayer *foreLayer;
    cocos2d::TMXLayer *backLayer;
    Vector<TileInfo*>getSurroundingTilesAtPosition(cocos2d::Point position, cocos2d::TMXLayer *layer, Droppable* p);
    Vector<TileInfo*>getAllSurroundingTilesAtPosition(cocos2d::Point position, cocos2d::TMXLayer *layer);
    TileInfo* getTileAtPosition(cocos2d::Point position, cocos2d::TMXLayer *layer);
#endif*/
    //bool isPlayerSet=false;
    cocos2d::Point tileCoordForPosition(cocos2d::Point position, TMXTiledMap* map);
    cocos2d::Rect tileRectFromTileCoords(cocos2d::Point tileCoords, TMXTiledMap* map);
    void getDown();
    void checkForDoor();
    void addDirtToTile(TMXTiledMap* map, cocos2d::Point pos);
    virtual void TouchesBegan(const std::vector<Touch*>& touches, Event *unused_event);
    virtual void TouchesMoved(const std::vector<Touch*>& touches, Event *unused_event);
    virtual void TouchesEnded(const std::vector<Touch*>& touches, Event *unused_event);
    
    void checkForAndResolveCollisions(Droppable* p);
    void checkForAndResolveCollisionsForOthers(Droppable* p);
    void checkForAndResolveCollisionsForWater(Droppable* p);
    void checkForAndResolveCollisionsForBouncing(Droppable* p);
    bool checkForAndResolveCollisionsForMissile(Droppable* p);
    bool checkDroppableMissileCollision(Droppable* p);
    void changePlayerState(int state);
    void saveCoinIfStarCollected();
    void setStage(TMXTiledMap* tileMap);
    void fixStageLayerTiles(TMXTiledMap* map);
    bool compareFourTiles(int t,int l, int r, int b, int x, int y, TMXLayer* stageLayer);
    void fixStageLayerFourTiles(TMXTiledMap* map);
    void stopWoongSound();
    bool compareNineTiles(int lt, int t, int rt, int l, int r, int lb, int b, int rb, int x, int y, TMXLayer* stageLayer);
    void getOffVehicle();
    bool isCameraInCustomMoving = false;
    void addRide(int unit, cocos2d::Point pos);
    void makeSomeBubbles(float dt);
    void fire();
    void enemyFireLoop(float dt);
    //int collectedKeyCount=0;
    void openWeapon(Ref* sender);
    float miniMapScale = 0.4f;
    Sprite* createNumberSprite(int number);
    Sprite* getLightSpin(float persistTime);
    Sprite* getShining(float delayTime);
//    void fireLoop(float dt);
    void missileMoveDone(Ref* obj);
    void spriteMoveDone(Ref* obj);
    void coinWaitDone(Ref* obj);
    void enemyBaseMoveDone(Ref* obj);
    void fireBulletMoveDone(Ref* drop);
    void nodeMoveDone(Ref* obj);
    void runEffect(int effect, cocos2d::Point point);
    void runEffect(int effect, cocos2d::Point point, float angle);
    void addGlowEffect(Sprite* sprite,const Color3B& colour, const cocos2d::Size& size);
    void gameClearAnimationDone(Ref* obj);
    void gameOver();
    void shakeScreen(int count);
    void shakeScreenOnce();
    cocos2d::Point extraCameraPos=cocos2d::Point::ZERO;
    void shakeScreenFirst();
    void shakeScreenSecond();
    void shakeScreenEnd();
    void showGameOverLayer();
    void setPlayerInfo();
    void setWeaponInfo(int weapon, int level);
    float getWeaponCoolTime(int weapon);
    void jumpBySpring(float dt);
    float blinkingTime;
    void blinkingPlayer(float dt);
    void immortalForAWhile(float dt);
    cocos2d::Rect RectInset(cocos2d::Rect rect, float x, float y);
    void removeEnemy(EnemyBase* spt);
    void showSuccessLayer(float dt);
    void unfreezePlayer(float dt);
    void stepBackForHit(float dt);
    void pauseLayer();
    void resumeLayer();
    void refreshPlayerInfo(int costum, int weapon);
    void smokeForVehicle();
    cocos2d::Rect intersection(cocos2d::Rect source, cocos2d::Rect rect);
//    virtual void draw();
    void dialogFinished(Ref* obj);
    virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* unused_event);
    cocos2d::Point waterBoyPos;
    bool isReloading = false;
    void releaseCustomMovingCamera();
    void entranceSchedule(float dt);
    void reload();
    int loadedBulletCount=-1;
    int loadedBulletCountMax;
    int totalBulletCount=-1;
    int totalBulletCountMax;
    int getSlotForWeapon(int weapon);
    void recharge(int slot);
    bool isFired;
    int getMaxLoadedBulletCount(int weapon);
    int getMaxTotalBulletCount(int weapon);
    cocos2d::Point center;
    Sprite* cursor;
    Sprite* getLight();
    bool isWaterBoy;
    bool isWaterBoyFlip;
    float waterBoyAngle;
    Sprite* background;
    void showCoinCount(cocos2d::Point pos, int count);
    void showStageClearLayer(bool threeStars);
    void onUpdateResultComplete();
    void onUpdateResultErrorComplete(char const *status, char const *error);
    void onUpdateUserComplete();
    void onUpdateUserErrorComplete(char const *status, char const *error);
    bool containsTouchLocation(Touch* touch);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    void resetTouchStart(float dt);
    void addListener();
    void warp();
    void goToNextScene();
    int indexToWarp = -9999;
    bool isBossMap;
    void showWarpAnimation();
    void endGame();
    Node* resultLayer = nullptr;
    void autoTargeting();
    void showResult();
    void updateResult(float dt);
    int kill=0;
    int destroy=0;
    int coin=0;
    int room=0;
    int boss=0;
    int time=0;
    bool resultDone = false;
    bool blockKeys = false;
    int totalScore=0;

    cocos2d::Point _cameraPoint;
    void warpOut();
    //DrawNode* dnDamageBoxes;
    //DrawNode* dnCollisionBoxes;
    
    Node* lastTalkedNpc = nullptr;
    bool isAutoTargetingOn = true;
    void testFunction();
    void checkEnding();
    void initScene();
    void onMiniMapTouch(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    void resetWeapons();
    void setWeaponsFromEquiped();
    Vector<Droppable*> playerShields;
    int shieldAngle = 0;
    
    std::vector<int> removedEnemyList;
    std::vector<int> loadedEnemyList;
    
    Node* ndBuyButton = nullptr;
    void onBuyShopItem();
};

#endif // __HELLOWORLD_SCENE_H__
