//
//  Episode.cpp
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 2015. 1. 14..
//
//

#include "Episode.h"
#include "GameManager.h"
bool Episode::init()
{
    Layer::init();
    size = Director::getInstance()->getWinSize();
    Label* lbl = Label::createWithTTF("SKIP", GameManager::getInstance()->getFont(FONT_BITDUST_ONE), 35);
    lbl->setColor(Color3B(180,180,180));
    MenuItemLabel* item = MenuItemLabel::create(lbl,  CC_CALLBACK_1(Episode::onSkip, this));
    Menu* menu = Menu::create(item, nullptr);
    this->addChild(menu, 10);
    menu->setPosition(Point(size.width/2 + 282, size.height/2 + 75));
    return true;
}
void Episode::onSkip(Ref* obj){
    episodeDone();
}
int Episode::getEpisodeIndex(){
    int index = 0;
    if (GameManager::getInstance()->theme == 0 && GameManager::getInstance()->currentStageIndex == 0) {
        index = 0;
    }else if (GameManager::getInstance()->theme == 1 && GameManager::getInstance()->currentStageIndex == 0) {
        index = 1;
    }else if (GameManager::getInstance()->theme == 2 && GameManager::getInstance()->currentStageIndex == 0) {
        index = 2;
    }else if (GameManager::getInstance()->theme == 3 && GameManager::getInstance()->currentStageIndex == 0) {
        index = 3;
    }
    
    return index;
}
void Episode::showEpisode(){
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    episodeIndex = getEpisodeIndex();
    if(episodeIndex == 0 ||
       episodeIndex == 1 ||
       episodeIndex == 2 ||
       episodeIndex == 3){
        Sprite* background = Sprite::create("openningBackground.png");
        this->addChild(background);
        background->setPosition(Point(size.width/2, size.height/2));
        
        sptCharLeft = Hero::createHero(UNIT_DARY, 1, 1);
        ((Hero*)sptCharLeft)->setRestOfBodyPart();
        sptCharLeft->setScale(4, 4);
        sptCharLeft->setAnchorPoint(Point(0.5, 0));
        sptCharLeft->setPosition(Point(size.width/2 - 250, size.height/2 - 126));
        this->addChild(sptCharLeft);
        
        sptCharRight = Sprite::create("girl.png");
        sptCharRight->setPosition(Point(size.width/2 + 250, sptCharLeft->getPosition().y));
        sptCharRight->setAnchorPoint(Point(0.5, 0));
        this->addChild(sptCharRight);
    }
    CCLOG("show episode: %d", getEpisodeIndex());
    episodeActionIndex = 0;
    CallFunc* finishCall = CallFunc::create(CC_CALLBACK_0(Episode::episodeDone, this));
    finishCall->retain();
    CallFunc* actionCall = CallFunc::create(CC_CALLBACK_0(Episode::episodeActionNeeded, this));
    actionCall->retain();
    episodeLayer = EpisodeLayer::getEpisodeLayer(getEpisodeIndex(), finishCall, actionCall);
    this->addChild(episodeLayer);
}
void Episode::episodeDone(){
    // move to stage or main for ending
    CCLOG("Edpisode done theme: %d", GameManager::getInstance()->theme);
    //Director::getInstance()->replaceScene(GameManager::getInstance()->getStageScene(0));
    episodeLayer->removeUsedAssets();
}
void Episode::episodeActionFinished(){
    episodeLayer->actionFinished();
    
    if (episodeIndex == 0) {
        CCLOG("episode action finished");
        Sprite* dragon = Sprite::createWithSpriteFrameName("dora_3_0.png");
        this->addChild(dragon, sptCharRight->getLocalZOrder());
        Animation* animation = AnimationCache::getInstance()->getAnimation("dora_3_0.png");
        Animate* animate = Animate::create(animation);
        dragon->runAction(RepeatForever::create(animate));
        
        dragon->setScale(-6, 6);
        dragon->setPosition(sptCharRight->getPosition() + Point(0, 100));
        //        dragon->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.4, Point(0, 10)), MoveBy::create(0.3, Point(0, -10)), NULL)));
        sptCharRight->removeFromParentAndCleanup(true);
    }
}
void Episode::episodeActionNeeded(){
    if (episodeIndex == 0) {
        // change to dragon and callback
        CCLOG("Dora to change into the dragon! ");
        sptCharRight->runAction(Sequence::create(DelayTime::create(2), CallFunc::create(CC_CALLBACK_0(Episode::episodeActionFinished, this)), NULL));
        Sprite* whiteCircle = Sprite::create("whiteCircle.png");
        this->addChild(whiteCircle, 100);
        whiteCircle->setPosition(sptCharRight->getPosition() + Point(0, 36));
        whiteCircle->setScale(0.2);
        whiteCircle->runAction(Sequence::create(ScaleTo::create(1, 12), DelayTime::create(0.3), ScaleTo::create(0.3, 200), DelayTime::create(1), FadeOut::create(0.8), CallFuncN::create(CC_CALLBACK_1(Episode::spriteMoveDone, this)), NULL));
    }
    episodeActionIndex++;
}

void Episode::spriteMoveDone(Ref* obj){
    Sprite* spt = (Sprite*)obj;
    spt->removeFromParentAndCleanup(true);
}
 
