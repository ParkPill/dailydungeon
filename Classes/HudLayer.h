#ifndef AlmostLegendary_HudLayer_h
#define AlmostLegendary_HudLayer_h

#include "SneakyJoystickSkinnedBase.h"
#include "SneakyJoystick.h"
#include "VirtualPadButton.h"
#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "ui/UIScrollView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"
#include "ui/UILoadingBar.h"
#include "cocos2d.h"
#include "LegendDaryButton.h"
#include "Droppable.h"
#include "base/CCEventListenerController.h"
#include "base/CCEventController.h"
#include "base/CCController.h"
#include "ui/UIHelper.h"
#include "ui/UIButton.h"
#include "ui/UIListView.h"
#include "ui/UIText.h"
#include "ui/UITextField.h"
#include "ui/UITextBMFont.h"
#include "ui/UIImageView.h"
#include "ui/UILoadingBar.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "CursorLayer.h"
#include "AwesomeDialogBox.h"
#define POST_TYPE_GEM 0
#define POST_TYPE_SEARCH_TICKET 1
#define POST_TYPE_LOCAL_TICKET 2
#define POST_TYPE_NETWORK_TICKET 3
#define POST_TYPE_MONTHLY_INSTANT_GEM 4
#define POST_TYPE_MONTHLY_INSTANT_TICKETS 5
#define POST_TYPE_MONTHLY_DAILY_GEM 6
#define POST_TYPE_MONTHLY_DAILY_TICKETS 7

typedef struct PostBoxItemStruct
{
    int postType;
    int value;
    std::string text;
}PostBoxItem;
#define SEARCH_STATE_NOT_TRY 0
#define SEARCH_STATE_TRIED 1
#define SEARCH_STATE_GET 2

#define MENU_MERGE 0
#define MENU_BAG 1
#define MENU_COLLECTION 2
#define MENU_SEARCH 3
#define MENU_DUNGEON 4
#define MENU_BOSS 5
#define MENU_GIFT 6
#define MENU_ARMOR 7
#define MENU_RESEARCH 8
#define MENU_ACHIEVE 9
#define MENU_ATTEND 10
#define MENU_RUNE 11
#define MENU_POST 12
#define MENU_CHEST 13
#define MENU_BATTLE 14
#define MENU_SHOP 15

#define NEW_ITEM_FOUND 0
#define NEW_UI_FOUND 1


#define BAG_JOB_NONE -1
#define BAG_JOB_REGISTER_FOR_PROMOTE_0 0
#define BAG_JOB_REGISTER_FOR_PROMOTE_1 1
#define BAG_JOB_REGISTER_FOR_PROMOTE_2 2
#define BAG_JOB_REGISTER_FOR_PROMOTE_3 3
#define BAG_JOB_REGISTER_FOR_PROMOTE_4 4
#define BAG_JOB_REGISTER_FOR_WEAPON_EQUIP 10
#define BAG_JOB_REGISTER_FOR_WEAPON_RANK_UP 11
#define BAG_JOB_REGISTER_FOR_SHIELD_EQUIP 12
#define BAG_JOB_REGISTER_FOR_SHIELD_RANK_UP 13
#define BAG_JOB_REGISTER_FOR_RUNE_EQUIP 20
#define BAG_JOB_REGISTER_FOR_RUNE_RANK_UP 21


typedef void (*voidCallBack)();

using namespace cocos2d;

using namespace cocos2d::ui;
using namespace cocostudio;
class HudLayer : public Layer
{
private:
    int weaponIndex;
    bool toChar;
    //Constructor
    Vector<LegendDaryButton*> buttonArray;
    Vector<Sprite*> lifeArray;
//    Sprite* coin;
//    Label* lblStage;
    int starCount;
    Label* talkLabel;
    Layer* talkLayer;
    MenuItemImage* pauseItem;
    MenuItemImage* recordItem;
    MenuItemImage* bombItem;
    MenuItemImage* potionItem;
    void closeWaitMessageBox(float dt);
    void closeWaitMessageBoxAndShowSuccess(float dt);
    void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
public:
    
    cocos2d::Point btnJumpPos;
    cocos2d::Point btnReloadPos;
    cocos2d::Point btnWeaponPos;
    cocos2d::Point joystickLRPos;
    cocos2d::Point joystickAimPos;
    SneakyJoystickSkinnedBase* joystickLR;
    SneakyJoystickSkinnedBase* joystickAim;
    SneakyJoystick* js2;
    SneakyJoystick* js;
    Vector<VirtualPadButton*> btns;
    VirtualPadButton* btnToggle;
    VirtualPadButton* btnShoot;
    VirtualPadButton* btnJump;
    VirtualPadButton* btnReload;
    VirtualPadButton* btnWeapon;
    VirtualPadButton* btnAction;
    
    AwesomeDialogBox* dialogBox = nullptr;
    void onDialogDone();
    void showNextTalk();
    void showDialog(std::string str);
    void facebookSafe(float dt);
    EventListenerKeyboard* listener;
//    EventListenerTouchOneByOne* touchListener;
    EventListenerTouchAllAtOnce* touchListener;
    void tryResultOK();
    LegendDaryButton* btnA;
    LegendDaryButton* btnB;
    LegendDaryButton* btnLeftRight;
    Sprite* bossEnergyBackground;
    Label* bossName;
    DrawNode* bossEnergyBar;
    DrawNode* bossEnergyBarBack;
    int bossEnergyBarWidth;
    bool isRecording;
    void addEnergyEffect();
    void addEnergyDone(float dt);
    Node* hudLayer;
    TextBMFont* lblKeyCollected;
//    Label* coinLabel;
    cocos2d::Size size;
    Layout* pauseLayer;
    Layout* reviveLayer;
    Layout* gameOverLayer;
    Layout* gameClearLayer;
    void showStageTitle();
    void stageTitleLineUpdate(float dt);
    DrawNode* dnTitleLine;
    float lineLength;
    float currentLineLength;
    void whiteGunFired();
    Sprite* sptWhiteGun;
    
    void closeReviveLayer(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void tryGameContinue(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    void toMain(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void toStageSelect(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    void nextStage(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void retryStage(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onReplayVideo(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    void buyGemClosed(Ref* obj);
    void reviveLater(float dt);
    void reviveFromVideo();
    virtual bool init();
    CREATE_FUNC(HudLayer);
    void showTalk(const char* talk);

    void hideTalk();
    Layer* bombLayer;
    void useBomb();
    void closeWaitMessageBoxLater();
    
    void closeWaitMessageBoxLaterAndShowSucess();
        
    void suggestBombShop();
    void buyBomb();
    Layer* potionLayer;
    void usePotion();
    void suggestPotionShop();
    void buyPotion();
    void buyGems();
    void notEnoughGemClosed(Ref* obj);
    void resumeGame(Ref* pSender, ui::Widget::TouchEventType eEventType);
//    void hideItems();
    Sprite* blackScreen;
    
    
    void showIndicator();
    void indicatorTimeOver(float dt);
    Node* indicator = nullptr;
    Node* currentPopup = nullptr;
    void hideIndicator();
    Vector<Node*> popupArray;
    void closePopup();
    void setAsPopup(Node* node);
    Node* getPopup(std::string name);
    
    Label* lblCoin;
    void updateCoin(int howMuch);
    void updateGem(int howMuch);
    void enableJoystick(bool enable);
    void initializeSreen(int lifeCount);
    void reduceLife();
    void setLife(int eng);
    void setWeapon(int weapon);
    Vector<Sprite*> bulletArray;
    void setTotalBulletCount(int count, int maxCount);
    void reduceBulletCount();
    void setBulletCount(int count, int maxCount);
    void keyCollected(int count);
    void starCollectedFancy(int count, cocos2d::Point collectedPos);
    void starCollected(int count);
    void starChange(float dt);
    void showPauseLayer();
    void showShopLayer();
    void showCoinShopLayer();
    void showGameOverLayer(float dt);
    void showGameContinueLayer();
    void showStageClearLayer(int coin, int enemyKilled, int enemyMax);
    Text* lblCoinCount;
    Text* lblEnemyCount;
    Text* lblExpCount;
    LoadingBar* pbExp;
    ImageView* sptMaxTalkBalloon;
    Text* lblPerfect;
    Text* lblNext;
    ImageView* sptBackLight0;
    ImageView* sptBackLight1;
    ImageView* sptBackLight2;
    Button* btnNext;
    Button* btnRetry;
    Button* btnHome;
    int increasingCoinCount;
    float increasingEnemyCount;
    int increasingExpCount;
    int increasingCounter;
    int increasingCounterMax;
    float countingInterval;
    int expCount;
    int maxExpCount;
    EventListenerController* _listener;
    void registerControllerListener();
    //Controller is the obejects of the Controller，keyCode means the keycode of the controller you click down
    void onKeyDown(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event);
    void onKeyUp(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event);
    
    //The axis includes X-axis and Y-axis and its range is from -1 to 1. X-axis is start from left to right and Y-axis is bottom to top.
    void onAxisEvent(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event);
    void onConnectController(Controller* controller, Event* event);
    void onDisconnectedController(Controller* controller, Event* event_);
                                            
    void coinLabelScheduler(float dt);
    void enemyLabelScheduler(float dt);
    void expLabelScheduler(float dt);
    void playCoinSoundLater();
    void removeUsedAssets();
    void onFreeCoin(Ref* pSender, ui::Widget::TouchEventType eEventType);
    Button* btnFreeCoin;
    void videoDone();
    void videoFailed();
    void closeVideoDone(float dt);
    void closeVideoFailed(float dt);
    bool videoContinued = false;
    int reviveCount = 0;
    Layout* messageBox;
    
    const char* getWeaponName(int index);
    int coinCount;
    int enemyKilledCount;
    int enemyMaxCount;
    CursorLayer* cursorLayer;
    
    ui::Layout* rankingLayer;
    cocos2d::Label* lblScore;
    void setScore(int score);
    Label* lblTyping;
    std::string originalString="";
    void showTyping();
    void typingCursorBlinking(float dt);
    void typing(std::string str);
    bool isInScene=false;
    void showEnding();
    void showCredit(float dt);
    void spriteMoveDone(Node* sprite);
    void coinWaitDone(Node* sprite);
    void displayBossEnergy(const char* name);
    void setBossEnergy(int percent);
    void setButtonSize(float siz);
    void enableButtons(bool enb);
    void showAchievementComplete(const char* text);
    void achievementEffect(Node* node);
    void nodeMoveDone(Ref* obj);
    void addHeartAnimated(cocos2d::Point pos);
    void addListener();
    void removeListener();
    void addHeartDone(Ref* obj);
    void playBackgroundMusicLater(float dt);
    int whereToGo;
    void closeSchedule(float dt);
    void shareFacebook();
    void showDialog(const char* message, const char* btn1, const char* btn2);
    void messageBoxClosed(Node* sender);
    void recommandCoinShopClosed(Node* sender);
    
//    virtual void keyBackClicked();
//    virtual void draw();
    virtual void keyDown(int key);
    virtual void keyUp(int key);

    Label* lblTouch;
    void toggleAuto(bool showMsg = true);
    void onEndCredit();
    void hideBtns();
    void showBtns();
    void showInstanceMessage(std::string msg, float stayTime = 3);
    
    bool isForceTouchAvailable = false;
    
    void updateAvatarInMenu();
    void onAvatarSelect(Ref* ref);
    void onAvatarClick();
    void onMissionClick();
    void onFreeGemClick();
    void onMenuClick();
    void onBattleClick();
    void onRefreshNetworkClick(Ref* ref);
    void onCloseBattleLayer();
    
    void onYourWeaponClick(Ref* ref);
    void onYourShieldClick(Ref* ref);
    void onMenuInMenuClick(Ref* ref);
    void onGiftClick();
    void onChestClick();
    void onPromoteClick();
    void onBagClick();
    void onCollectClick(int itemType);
    void onCollectTabClick(Ref* ref);
    void onSearchClick();
    void saveSearchState();
    void retreiveSearchState();
    void onDungeonClick();
    void onBossClick();
    void onArmorClick();
    void onUpgradeArmorClick(Ref* ref);
    void updateArmorLayer();
    void onResearchClick();
    void onAchieveClick();
    void onAttendClick();
    void onRuneClick();
    void onPostClick();
    void onFAQClick();

    void updateUI(float dt);
    
    void showAttend();
    void onAttendRewardClick(Ref* ref);
    int getUnlockLevel(int index);
    void checkForLockedMenu();
    
    Node* menuLayer;
    void updatePromoteLayer();
    void onPromoteTabClick(Ref* ref);
//    void onRegisterForBestClick(Ref* ref);
//    void onAutoRegisterForBestClick();
//    void onPromoteForBestClick(Ref* ref);
    void onRegisterForPromoteClick(Ref* ref);
    void onAutoRegisterForPromoteClick();
    void onPromoteToNextClick(Ref* ref);
    void onStarInPromoteClick(Ref* ref);
    int promoteStarIndex = 0;
    int promoteTypeIndex = 0;
    
    void showBag(int filter, int job);
    
    void onItemInBagClick(Ref* ref);
    
    void showItemGetAndAddToBag(int item);
    void showItemGetAndAddToBag(std::vector<int> array);
    void onTest();
    
    std::vector<int> registeredIndexArrayForRankUp;
    std::vector<int> registeredIndexArrayForPromote;
//    std::vector<int> registeredIndexArrayForBetter;
    
    Node* giftLayer = nullptr;
    void onGetGiftClick();
    void addGlowBack(Node* parent, float scale = 0.3f);
    bool isVideoDone = false;
    void handleVideoDone();
    
    void onPickOneClick(Ref* ref);
    void onPickTenClick(Ref* ref);
    int getRandomWeapon();
    int getRandomWeapon(int star);
    int getRandomShield();
    int getRandomShield(int star);
    int getRandomRune();
    int getRandomRune(int star);
    
    int itemDetailValue = -1;
    void showItemDetail(int index, bool showNewStar = false);
    void showItemInfo(int value, bool isFromInventory = true, bool showNewStar = false);
    void updateItemDetail();
    void onSlotForRankUpClick(Ref* ref);
    void onAutoRegisterForRankUpClick();
    void onPromoteForRankUpClick(Ref* ref);
    int selectedItemForRankUp = -1;
    Vector<Node*> addStarsToIcon(Node* btn, int starCount, int purpleStarCount = 0);
    
    void onItemInCollectionClick(Ref* ref);
    void showReward(int popupType);
    void updateSearchItems();
    void onForceFixClick(Ref* ref);
    void onTryFixClick(Ref* ref);
    void showSearchResult(int index, bool success);
    int selectedItemIndexInBag = -1;
    
    void addGlowToSearchResult();
    void onEnterDailyDungeonClick(Ref* ref);
    void setButtonTexture(Button* btn, int rareness);
    void setImageTexture(ImageView* img, int rareness);
    std::string getCardNameForRareness(int rareness);
    cocos2d::Size getCardSize(int rareness);
    void setTitle(Node* titleBack, std::string menuKey);
    void updateSearchClick();
    void onEnterBossDungeon(Ref* ref);
    void updateResearchLayer();
    void onUpgradeResearchClick(Ref* ref);
    void onRuneInfoClick();
    void updateRunePopup();
    
    int runeSlotIndexToEquip = -1;
    int runeSlotIndexToRankUp = -1;
    void onRuneSlotForEquipClick(Ref* ref);
    void onRuneSlotForRankUpClick(Ref* ref);
    void onRuneRankUpClick(Ref* ref);
    void onRuneAutoRegisterClick();
    
    int weaponSlotIndexForEquip = -1;
    int shieldSlotIndexForEquip = -1;
    void updateMenuLayer();
    void setSizeForRareness(Node* img, int rareness);

    std::vector<PostBoxItem*> postBoxArray;
    void updatePostBox();
    void getPostBoxItemFromServer();
    PostBoxItem* getPostBoxItem(int postType, int value, std::string text);
    void onPostReceivedFromServer();
    void onReceivePostItemClick(Ref* ref);
    std::string postBoxTextFromServer;
    void onServerFailed();
    void restartGame();
    
    
    
    int networkStateGetData = 0;
    int networkStateSendData = 0;
    void onGetAllPostClick(Ref* ref);
    
    bool reopenPostPopup = false;
    bool updateSearchPopupRequired = false;
    void onOkSearchResultClick();
    void confirmGift();
    bool confirmGiftRequired = false;
    
    bool updateArmorRequired = false;
    bool updateResearchRequired = false;
    
    bool showAttendRequired = false;
    Node* gemBar;
    void onGemShopClick();
    
    int itemTypeForCollection = -1;
    bool showCollectionRequired = false;
    void showCollection();
    
    void showMenu();
    bool showMenuRequired = false;
    bool isGettingTopPlayers = false;
    bool isTopPlayerForFieldRequired = false;
    bool isTopPlayerForFieldReceived = false;
    void getTopPlayersForLobby();
    void getTopPlayersOnField();
    
    void updateEquipedWeapons();
    std::string getWeaponEquipData();
    std::string getRuneEquipData();
    
    bool isWeaponEquipChanged = false;
    
    std::vector<std::string> topNameArray;
    std::vector<int> topAvatarArray;
    std::vector<std::string> topEquipArray;
    
    void onPlusTicketClick(Ref* ref);
    void updateBattleLayer();
    void onLocalPlayClick(Ref* ref);
    void onNetworkPlayClick(Ref* ref);
    void onNetworkRewardClick(Ref* ref);
    void onSelectCharClick(Ref* ref);
    
    bool isFindMatchesRequested = false;
    void generateMatches();
    int networkMatchIndex=0;
    bool isRequestingNetworkMatchUserData = false;
    std::vector<int> networkMatchIDs;
    std::vector<int> networkMatchTrophys;
    std::vector<int> networkMatchAvatars;
    std::vector<std::string> networkMatchNames;
    std::vector<std::string> networkMatchEquips;
    std::vector<std::string> networkMatchRuneEquips;
    std::vector<std::string> networkMatchResearchs;
    std::vector<std::string> networkMatchArmors;
    void finishGenerateMatches();
    void createMatchesUIFromNetworkData();
    bool isNetworkMatchDataSet = false;
    std::string getRandomName();
    
    void onTopPlayerClick(Ref* ref);
    bool isGettingAllUser = false;
    std::string playerName;
    int rankType = 0;
    
    void onMissionItemClick(Ref* ref);
    bool isGiftShouldBeShown = false;
    bool isSearchShouldBeShown = false;
    bool isPromoteShouldBeShown = false;
    
    void onSettingClick();
    void updateSettingPopup();
    void onSoundClick();
    void onMusicClick();
    void onAlertClick();
    void onMailToClick();
    void onLanguageClick();
    void onLanguageButtonClick(Ref* ref);
    int selectedLanguageWhenOpenLanguagePopup = -1;
    void onLanguageOkClick();
    void updateLanguagePopup();
    
    void onProfileClick();
    void updateProfile();
    void onGiftFromDevInProfileClick();
    void onChangeAccountClick();
    void onOkChangeAccountClick(Ref* ref);
    void onVisitForum();
    void onReceiveVisitForumOk();
    Button* showMessageBox(std::string title, std::string msg, int gemReward);
    
    void setCloseButtonAndBlock(Node* layer);
    bool isChangingAccount = false;
    bool isChangingAccountSuccess = false;
    
    void onChangeNameClick();
    void onOkChangeNameClick(Ref* ref);
    bool isChangingName = false;
    bool isChangingNameSuccess = false;
    
    int myRank = -1;
    bool isLoadingRank = false;
    bool isLoadingRankSuccess = false;
    
    void onGemShopTabClick(Ref* ref);
    void onStarterPackClick(Ref* ref);
    void onPackageClick(Ref* ref);
    void onGemPackClick(Ref* ref);
    void onTicketPackClick(Ref* ref);
    void onVideoAdsClick(Ref* ref);
    void updateGemShopButtonPositions(int index);
    std::string getAccessRewardStringForServer();
    bool isNoAdsPurchased = false;
    
    void handleRedDots();
    void giveAwayIAPForTest();
    void setPriceInfo(Node* btn, std::string before, std::string sale);

    void showTutorialDialog();
    void onTutorialDialogDone();
    Sprite* sptArrowToTutorialDungeon = nullptr;
    void attachArrowForTutorial(Node* btn);
    
    void showPrincessSmallTalk(std::string str);
    void onPrincessSmallTalkDone();
    int getTotalEquippedWeaponCount();
    void showDailyDungeonResult();
    void onVideoStoreClick();
    void onVideoStoreTabClick(Ref* ref);
    void onVideoStoreItemClick(Ref* ref);
    int selectedVideoStoreItemCount = 0;
    int selectedVideoStoreItemIndex = 0;
    int selectedVideoStoreTabIndex = 0;
    void closePopupAndSaveGem();
    int lastTimeLeftToMidnight = 0;
    int videoStoreWaitForNextVideoTime = 60*5;
    void update(float dt);
    
    
    void askExit();
    void onExitYesClick();
    
    int getAchievementCurrent(int index);
    int getAchievementMax(int index);
    int getAchievementLevel(int index);
    int getAchievementRewardGemCount(int index, int level);
    void onReceiveAchievementRewardClick(Ref* ref);
    float hudTimer = 0;
    
    void onTestLeftClick();
    void onTestRightClick();
    void onTestUpClick();
    void onTestDownClick();
    void adjustHandPos(Vec2 pos);
    
    void setDailyMissionState(int index, int state);
};
#endif
