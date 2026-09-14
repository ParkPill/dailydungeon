#ifndef AlmostLegendary_DualHudLayer_h
#define AlmostLegendary_DualHudLayer_h

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
//#include "ShopLayer.h"

using namespace cocos2d;

using namespace cocos2d::ui;
using namespace cocostudio;
class DualHudLayer : public Layer
{
private:
    cocos2d::Point btnJumpPos;
    cocos2d::Point btnReloadPos;
    cocos2d::Point btnWeaponPos;
    cocos2d::Point joystickLRPos;
    cocos2d::Point joystickAimPos;
    SneakyJoystickSkinnedBase* joystickLR;
    SneakyJoystickSkinnedBase* joystickAim;
    SneakyJoystick* js2;
    SneakyJoystick* js;
    VirtualPadButton* btnJump;
    VirtualPadButton* btnReload;
    VirtualPadButton* btnWeapon;
    VirtualPadButton* btnAction;
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
    int readySetBattleState = 0;
    Label* lblReady;
    void readySetBattle();
    void setInfo(std::string playerName, std::string enemyName, int playerHealth, int playerAttack, int enemyHealth, int enemyAttack, int playerTrophy, int enemyTrophy);
    void setPlayerHealth(int percent);
    void setEnemyHealth(int percent);
//    void showResult(bool win);
    void onResultOk();
    
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
    CREATE_FUNC(DualHudLayer);
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
    
    void closePopupLayer();
    Label* lblCoin;
    void updateCoin(int howMuch);
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
//    ShopLayer* shopLayer;
    float shopLayerMoveYDistance;
    
//    virtual void keyBackClicked();
//    virtual void draw();
    virtual void keyDown(int key);
    virtual void keyUp(int key);
    
    void onBack();
    void onNext();
    void onTapToContinue();
    
    int getNextMatchPrice();
    
    float waitingTime = 0;
    void onWaitingMatching(float dt);
    void showMessageBox(int mBoxType, std::string message, std::string ok, bool showClose);
    void onMessageBoxOk();
    void onMessageBoxCancel();
    
    bool isMultiSearchDone = false;
    bool isMultiSearchSuccess = false;
};
#endif
