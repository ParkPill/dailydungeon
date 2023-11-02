//
//  EndingCredit.cpp
//  Pigwing
//
//  Created by PACKSUNG PILL on 12/7/15.
//
//

#include "EndingCredit.h"
#include "GameManager.h"
#include "Title.h"


bool EndingCredit::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    size = Director::getInstance()->getWinSize();
    
    Label* lbl;
    Point startPos = Point(size.width/2, -50);
    Point endPos = Point(size.width/2, size.height + 50);
    float delay = 1.5f;
    float totalDelay = 0;
    float dur = 20;
    for (int i = 0; i < 500; i++) {
        if (i == 0) {
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "Brave Rascal");
        }else if(i == 1){
            totalDelay += delay*2;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "1506 game studio");
        }else if(i == 2){
            totalDelay += delay;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "");
        }else if(i == 3){
            totalDelay += delay*2;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "Programing");
        }else if(i == 4){
            totalDelay += delay;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "Park Pill");
        }else if(i == 5){
            totalDelay += delay*2;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "Art");
        }else if(i == 6){
            totalDelay += delay;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "Shinae Choi");
        }else if(i == 7){
            totalDelay += delay*2;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "Music & Sound");
        }else if(i == 8){
            totalDelay += delay;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "Park Pill");
        }else if(i == 9){
            totalDelay += delay;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "");
        }else if(i == 10){
            totalDelay += delay*2;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "Special Thanks to");
        }else if(i == 11){
            totalDelay += delay;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "Global Top Round");
        }else if(i == 12){
            totalDelay += delay;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "And of course you");
        }else if(i == 13){
            totalDelay += delay*2;
            lbl = Label::createWithBMFont("boldPixelFont.fnt", "Thanks for playing");
        }else if(i == 14){
            break;
        }
        lbl->setScale(4);
        lbl->setPosition(startPos);
        lbl->runAction(Sequence::create(DelayTime::create(totalDelay), MoveTo::create(dur, endPos), NULL));
        this->addChild(lbl);
    }
    
    this->runAction(Sequence::create(DelayTime::create(totalDelay + dur), CallFunc::create(CC_CALLBACK_0(EndingCredit::goToMain, this)), NULL));
    
    backgroundLayer = Layer::create();
    this->addChild(backgroundLayer, -5);
    backgroundLayer->setRotation(15);
    backgroundLayer->setPosition(Point(-150, -50));
    
    float x = 0;
    float y = 0;
    tileWidth = 0;
    speedX = 50;
    speedY = -50;
    tileSpeedy = false;
    float time;
    float yGap = 0;
    while(true){
        for (int i = 0; i < size.height*3;) {
            Sprite* spt = Sprite::create("bottomTile.png");
            spt->setScale(4);
            spt->setAnchorPoint(Point::ZERO);
            if(tileWidth == 0){
                tileWidth = spt->getContentSize().width*spt->getScale();
            }
            backgroundLayer->addChild(spt);
            spt->setPosition(Point(x, y + yGap));
            time = (x + tileWidth)/speedX;
            spt->runAction(Sequence::create(MoveBy::create(time, Point(-x-tileWidth, time*speedY*(tileSpeedy?1:2))), CallFuncN::create(CC_CALLBACK_1(EndingCredit::spriteMoveDone, this)), NULL));
            i += spt->getContentSize().height*spt->getScale();
            y = i;
        }
        tileSpeedy = !tileSpeedy;
        if (x > size.width*1.4f) {
            break;
        }
        x += tileWidth;
        yGap -= 50;
        y = 0;
    }
    this->schedule(schedule_selector(EndingCredit::backgroundSchedule), tileWidth/(speedX*2));
    backTileStartX = x;
    
    GameManager::getInstance()->playBGM(BGM_BRIGHT);
    return true;
}
void EndingCredit::spriteMoveDone(Node* node){
    node->removeFromParentAndCleanup(true);
}
void EndingCredit::backgroundSchedule(float dt){
    float y = 0;
    float time;
    for (int i = 0; i < size.height*3;) {
        Sprite* spt = Sprite::create("bottomTile.png");
        spt->setScale(4);
        spt->setAnchorPoint(Point::ZERO);
        
        backgroundLayer->addChild(spt);
        spt->setPosition(Point(backTileStartX, y));
        time = (backTileStartX + tileWidth)/speedX;
        spt->runAction(Sequence::create(MoveBy::create(time, Point(-backTileStartX-tileWidth, time*speedY*(tileSpeedy?1:2))), CallFuncN::create(CC_CALLBACK_1(EndingCredit::spriteMoveDone, this)), NULL));
        i += spt->getContentSize().height*spt->getScale();
        y = i;
    }
    tileSpeedy = !tileSpeedy;
}
void EndingCredit::goToMain(){
    GameManager::getInstance()->page = PAGE_TITLE;
    
    Director::getInstance()->popScene();
    //        ((TitleLayer*)GameManager::getInstance()->titleLayer)->topBar->updateLabels();
    
}
