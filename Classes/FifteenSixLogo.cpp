#include "FifteenSixLogo.h"
#include "GameManager.h"
//#include "LanguageManager.h"
#include "Title.h"
#include "SimpleAudioEngine.h"
#include "HelloWorldScene.h"
// on "init" you need to initialize your instance
bool FifteenSixLogo::init()
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(! LayerColor::initWithColor(Color4B(27, 26, 25, 255)));
        size = Director::getInstance()->getWinSize();
        
        isLab = true;
        if(isLab){
            
        }else{
            fadeInEnd = 0.4f;
            blinkDur = 0.1f;
            blinkStart = 0.7f;
            dataWarningStart = 2.5f;
            dataWarningEnd = 4.0f;
            
            int splashTime = 0;
            if(false){
                showSplash();
                splashTime = 2;
            }else{
                showLogo();
            }
        }
        
        
        //        Label* lblDataWarn = Label::createWithTTF(LanguageManager::getInstance()->getStringForKey(STR_data_warn), LanguageManager::getInstance()->getStringForKey(STR_font), 30);
        //        this->addChild(lblDataWarn);
        //        lblDataWarn->setWidth(size.width);
        //        lblDataWarn->setPosition(Point(size.width/2, lblDataWarn->getContentSize().height));
        
        bRet = true;
    } while (0);
    
    return bRet;
}
void FifteenSixLogo::showSplash(){
    this->removeAllChildren();
    cocos2d::Size size = Director::getInstance()->getWinSize();
    
    Sprite* background = Sprite::create("kakaoSplash.png");
    this->addChild(background);
    background->setPosition(cocos2d::Point(size.width/2, size.height/2));
    background->setScale(size.height/background->getContentSize().height);
    
    background->runAction(Sequence::create(DelayTime::create(1), FadeOut::create(1), CallFunc::create(CC_CALLBACK_0(FifteenSixLogo::showLogo, this)), NULL));
}
void FifteenSixLogo::showLabLogo(){
    Layout* layout = Layout::create();
    this->addChild(layout);
    layout->setContentSize(size);
    
    logo = Sprite::create("1506_logo.png");
    this->addChild(logo);
    logo->getTexture()->setAntiAliasTexParameters();
    logo->setPosition(cocos2d::Point(size.width/2, size.height/2 - 5));
    logo->setOpacity(0);
//    logo->runAction(MoveBy::create(0, cocos2d::Point(0, size.height)));
    
    logo->runAction(Sequence::create(FadeIn::create(0.8f), DelayTime::create(1.6f), FadeOut::create(0.8f), CallFunc::create(CC_CALLBACK_0(FifteenSixLogo::goMain, this)),NULL));
}
void FifteenSixLogo::showLogo(){
    this->removeAllChildren();
    Layout* layout = Layout::create();
    this->addChild(layout);
    layout->setContentSize(size);
    layout->setTouchEnabled(true);
    
    cocos2d::Size size = Director::getInstance()->getWinSize();
    {
        Sprite* background = Sprite::create("1506_logoBackground.png");
        this->addChild(background);
        background->setPosition(cocos2d::Point(size.width/2, size.height/2));
        background->setScaleX(size.width/background->getContentSize().width);
        background->setScaleY(size.height/background->getContentSize().height);
        
        logo = Sprite::create("1506_logo.png");
        this->addChild(logo);
        logo->getTexture()->setAntiAliasTexParameters();
        logo->setPosition(cocos2d::Point(size.width/2, size.height/2 - 59 - size.height));
        logo->runAction(FadeOut::create(0));
        logo->runAction(MoveBy::create(0, cocos2d::Point(0, size.height)));
        
        float shortDur = 0.01f;
        logoDragon = Sprite::create("1506_logoDragon1.png");
        this->addChild(logoDragon);
        logoDragon->getTexture()->setAntiAliasTexParameters();
        logoDragon->setPosition(cocos2d::Point(size.width/2 - 1, size.height/2 + 107 + size.height));
        logoDragon->runAction(FadeOut::create(shortDur));
        logoDragon->runAction(MoveBy::create(shortDur, cocos2d::Point(0, -size.height)));
        
        Sprite* logoDragon2 = Sprite::create("1506_logoDragon2.png");
        this->addChild(logoDragon2);
        logoDragon2->getTexture()->setAntiAliasTexParameters();
        logoDragon2->setPosition(logoDragon->getPosition());
        logoDragon2->runAction(FadeOut::create(shortDur));
        logoDragon2->runAction(MoveBy::create(shortDur, cocos2d::Point(0, -size.height)));
        
        logoDragon->runAction(Sequence::create(DelayTime::create(shortDur), FadeIn::create(fadeInEnd),NULL));
        logo->runAction(Sequence::create(DelayTime::create(shortDur), FadeIn::create(fadeInEnd), NULL));
        this->runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create(CC_CALLBACK_0(FifteenSixLogo::ringMyBell, this)), NULL));
        logoDragon2->runAction(Sequence::create(DelayTime::create(blinkStart), FadeIn::create(0.1f), DelayTime::create(0.25f), FadeOut::create(0.1f), CallFunc::create(CC_CALLBACK_0(FifteenSixLogo::goMain, this)),NULL));
    }
    
}
void FifteenSixLogo::ringMyBell(){
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("doorBell.mp3");
}
void FifteenSixLogo::dataWarning(float dt){
    Layer* layer = Layer::create();
    this->addChild(layer);
    float dur = 0.5f;
    logo->runAction(FadeOut::create(dur));
    logoDragon->runAction(FadeOut::create(dur));
    Sprite* spt = Sprite::create("flyingDary.png");
    cocos2d::Point sptPos = cocos2d::Point(size.width/2, size.height/2 + 100);
    spt->setPosition(cocos2d::Point(-100,-1000));
    layer->addChild(spt);
    spt->runAction(Sequence::create(FadeOut::create(0),MoveTo::create(0, sptPos), DelayTime::create(dur),FadeIn::create(dur), NULL));
    
    //    this->scheduleOnce(schedule_selector(FifteenSixLogo::goMain), dataWarningEnd - dataWarningStart);
}   
void FifteenSixLogo::goMain(){
    this->removeFromParent();
//    titleLayer = TitleLayer::create();
    Scene* sceneTitle = Scene::create();
//    sceneTitle->addChild(titleLayer);
    sceneTitle->addChild(HelloWorld::scene(STAGE_LOBBY, false));
    
//    Director::getInstance()->replaceScene(TransitionFade::create(0.8, sceneTitle, Color3B::BLACK));
    Director::getInstance()->replaceScene(sceneTitle);
}
