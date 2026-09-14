#include "cocos2d.h"
#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "DualWorld.h"
#include "ServerManager.h"
#include "MobileTitle.h"
#ifdef SDKBOX_ENABLED
#include "PluginIAP/PluginIAP.h"
#endif
#ifdef SDKBOX_ENABLED
//#include "PluginFacebook/PluginFacebook.h"
#endif
#ifdef SDKBOX_ENABLED
#include "Sdkbox/Sdkbox.h"
#endif
#include "GameManager.h"
#include "LanguageManager.h"
#include "Title.h"
#include "FifteenSixLogo.h"
#include "GameSharing.h"
//#include "NativeInterface.h"
#include "Title.h"
#include "MobileTitle.h"
#include "ServerManager.h"
USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(1136, 640);
static cocos2d::Size smallResolutionSize = cocos2d::Size(1136, 640);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1136, 640);//1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(1136, 640);//2048, 1536);

AppDelegate::AppDelegate() {
    
}

AppDelegate::~AppDelegate()
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    
    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    log("dailydungeon 1");
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_ANDROID
    
#ifdef SDKBOX_ENABLED
    sdkbox::IAP::init();
#endif
    
#ifdef SDKBOX_ENABLED
//    sdkbox::PluginFacebook::init();
#endif

#endif
    log("dailydungeon 2");
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
//        glview = GLViewImpl::createWithRect("Daily Dungeon", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
        glview = GLViewImpl::createWithRect("Daily Dungeon", Rect(0, 0, designResolutionSize.width*1.2f, designResolutionSize.height));
//        glview = GLViewImpl::createWithFullScreen("Daily Dungeon: Pierrot side story");
//        glview = GLViewImpl::create("Brave Rascal");
#else
        glview = GLViewImpl::create("Brave Rascal");
#endif
        director->setOpenGLView(glview);
    }
    log("dailydungeon 3");
    // turn on display FPS
    director->setDisplayStats(false);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 50);
    
    // Set the design resolution
//    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
    Size frameSize = glview->getFrameSize();
    if (frameSize.height/frameSize.width > designResolutionSize.height/designResolutionSize.width){
        glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_WIDTH);
    }else{
        glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_HEIGHT);
    }
    
    /*Size frameSize = glview->getFrameSize();
     // if the frame's height is larger than the height of medium size.
     if (frameSize.height > mediumResolutionSize.height)
     {
     director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
     }
     // if the frame's height is larger than the height of small size.
     else if (frameSize.height > smallResolutionSize.height)
     {
     director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
     }
     // if the frame's height is smaller than the height of medium size.
     else
     {
     director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
     }*/
    log("dailydungeon 4");
    SpriteFrameCache* cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile("legendDaryDungeon.plist");
    cache->addSpriteFramesWithFile("lddSpriteSheet.plist");
    for (int i = 0; i < 6; i++) {
        cache->addSpriteFrame(Sprite::create(__String::createWithFormat("laserThinBlueHit%d.png", i)->getCString())->getSpriteFrame(), __String::createWithFormat("laserThinBlueHit%d.png", i)->getCString());
    }log("dailydungeon 5");
    
    //    CCFileUtils::getInstance()->addSearchPath("animations");
    FileUtils::getInstance()->addSearchPath("UI");
    FileUtils::getInstance()->addSearchPath("Language");
    FileUtils::getInstance()->addSearchPath("fonts");
    FileUtils::getInstance()->addSearchPath("res");
    FileUtils::getInstance()->addSearchPath("sounds");
    FileUtils::getInstance()->addSearchPath("Logo");
    GameManager::getInstance()->preLoadAllSoundEffect();
    
    if(!UDGetBool(KEY_SOUND_ONOFF, true)){
        CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(0);
    }   
    AnimationCache::getInstance()->addAnimationsWithFile("animations.plist");
    
    GameManager::getInstance()->originalSize = director->getWinSize();
    GameManager::getInstance()->market = MARKET_PLAYSTORE_FREE;
    GameManager::getInstance()->showVPad = false;
    GameManager::getInstance()->totalThemeCount = 4;
    GameManager::getInstance()->totalStage = 24*4;
    LanguageManager::getInstance()->loadLanguageSheet();
    GameManager::getInstance()->setPetInfos();
    
    if (UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET, false)) {
        UserDefault::getInstance()->setBoolForKey(KEY_DOUBLE_EXP_GET, true);
        UserDefault::getInstance()->setBoolForKey(KEY_REMOVE_ADS_GET, true);
    }
    
    Director::getInstance()->setProjection(Director::Projection::_2D);
    
    if (GameManager::getInstance()->market == MARKET_MAC ||
        GameManager::getInstance()->market == MARKET_WINDOWS ||
        GameManager::getInstance()->market == MARKET_FUNBOX ||
        GameManager::getInstance()->market == MARKET_OUYA) {
        GameManager::getInstance()->isUsingController = true;
    }
    //    NativeInterface::NativeInterfacedestroyAds();
    //    NativeInterface::NativeInterfacedestroyRectAds();
    Director::getInstance()->setDepthTest(false);
    GameManager::getInstance()->firstPlayed = true;
    GameSharing::init();
    srand((uint)time(NULL));
    
    log("about to init GameManager");
    GameManager::getInstance()->initGameManager();
    /*
     log("init GameManagerInit done");
     log("time request ");*/
    
    register_all_packages();
    
    log("creating scene");
    
//    SM->getHttpTime();
    SM->checkServer();
//    if (GM->developer) {
    if (true) { // test
        GM->priceList.push_back("$2.99");// IAP_STARTER_0 0
        
        GM->priceList.push_back("$3.99");// IAP_STARTER_1 1
        GM->priceList.push_back("$2.99");// IAP_STARTER_2 2
        GM->priceList.push_back("$17.99");// IAP_PACKAGE_0 3
        GM->priceList.push_back("$89.99");// IAP_PACKAGE_1 4
        GM->priceList.push_back("$8.99");// IAP_GEM_0 5
        GM->priceList.push_back("$1.99");// IAP_GEM_1 6
        GM->priceList.push_back("$5.99");// IAP_GEM_2 7
        GM->priceList.push_back("$19.99");// IAP_GEM_3 8
        GM->priceList.push_back("$59.99");// IAP_GEM_4 9
        GM->priceList.push_back("$14.99");// IAP_TICKET_0 10
        GM->priceList.push_back("$19.99");// IAP_STARTER_1_ORIGINAL 11
        GM->priceList.push_back("$104.99");// IAP_STARTER_2_ORIGINAL 12
        GM->priceList.push_back("$59.99");// IAP_PACKAGE_0_ORIGINAL 13
        GM->priceList.push_back("$134.99");// IAP_PACKAGE_1_ORIGINAL 14
        GM->priceList.push_back("$99.99");// IAP_GEM_0_ORIGINAL 15
        GM->priceList.push_back("$159.99");// IAP_TICKET_0_ORIGINAL 16
        GM->priceList.push_back("$0.99");// IAP_DUNGEON_ATM 17
//        GM->iapFlag = IAP_STATE_READY;
    }
    
    GM->size = Director::getInstance()->getWinSize();
//     create a scene. it's an autorelease object
    
    GM->version = "0.57";
    GM->versionCode = 48;
    GM->developer = false; // test
//    scene->addChild(FifteenSixLogo::create());
//    scene->addChild(WorldMap::create());
//    scene->addChild(PanZoomLayer::create());
//    scene->addChild(Title::create());
//    scene->addChild(HelloWorld::scene(0, false));
//    scene->addChild(HelloWorld::scene(STAGE_LOBBY, false));
    log("about to create scene");
    auto scene = Scene::create();
    scene->addChild(MobileTitle::create()); // normal
//    auto scene = HelloWorld::scene(STAGE_DAILY_DUNGEON_3, false); // test 
//    scene->addChild(DualWorld::scene(STAGE_DUAL));
    log("scene created");
    // run
    director->runWithScene(scene);
    
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
#ifdef SDKBOX_ENABLED
    sdkbox::sessionEnd();
#endif
    Director::getInstance()->stopAnimation();
    
    
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
#ifdef SDKBOX_ENABLED
    sdkbox::sessionStart();
#endif
    Director::getInstance()->startAnimation();
    
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
