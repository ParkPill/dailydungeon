//
//  SceneChanger.cpp
//  DaryDual
//
//  Created by PACKSUNG PILL on 31/05/2017.
//
//

#include "SceneChanger.h"
#include "GameManager.h"
bool SceneChanger::init()
{
    Layer::init();
    size = Director::getInstance()->getWinSize();

    return true;
}

void SceneChanger::setLayer(bool isClosing, std::string title, std::string tip, Scene* scene){
    Node* layer = CSLoader::createNode("SceneChange.csb");
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setContentSize(size);
    this->addChild(layer, 2003);
    sceneToChange = scene;
    
    Text* lblTitle = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lblTitle, title);
    
    Text* lblTip = (Text*)layer->getChildByName("lblTip");
    LM->setLocalizedString(lblTip, tip);
    
    Node* leftDoor = layer->getChildByName("backLeft");
    Node* rightDoor = layer->getChildByName("backRight");
    Node* imgTitleBack = layer->getChildByName("imgTitleBack");
    Node* imgTipBack = layer->getChildByName("imgTipBack");
    
    float moveDur = 1.0f;
    if (isClosing) {
        sceneToChange->retain();
        Point pos = leftDoor->getPosition();
        leftDoor->setPositionX(pos.x - size.width/2);
        leftDoor->runAction(EaseBounceOut::create(MoveTo::create(moveDur, pos)));
        pos = lblTitle->getPosition();
        lblTitle->setPositionX(pos.x - size.width/2);
        lblTitle->runAction(EaseBounceOut::create(MoveTo::create(moveDur, pos)));
        pos = imgTitleBack->getPosition();
        imgTitleBack->setPositionX(pos.x - size.width/2);
        imgTitleBack->runAction(EaseBounceOut::create(MoveTo::create(moveDur, pos)));
        
        pos = rightDoor->getPosition();
        rightDoor->setPositionX(pos.x + size.width/2);
        rightDoor->runAction(EaseBounceOut::create(MoveTo::create(moveDur, pos)));
        pos = lblTip->getPosition();
        lblTip->setPositionX(pos.x + size.width/2);
        lblTip->runAction(EaseBounceOut::create(MoveTo::create(moveDur, pos)));
        pos = imgTipBack->getPosition();
        imgTipBack->setPositionX(pos.x + size.width/2);
        imgTipBack->runAction(EaseBounceOut::create(MoveTo::create(moveDur, pos)));
        
        this->scheduleOnce(schedule_selector(SceneChanger::changeScene), moveDur);
    }else{
        Point pos = leftDoor->getPosition();
        int rate = 5;
        float delayTime = 1;
        leftDoor->runAction(Sequence::create(DelayTime::create(delayTime), EaseIn::create(MoveTo::create(moveDur, Point(pos.x - size.width, pos.y)), rate), nullptr));
        pos = lblTitle->getPosition();
        lblTitle->runAction(Sequence::create(DelayTime::create(delayTime), EaseIn::create(MoveTo::create(moveDur, Point(pos.x - size.width, pos.y)), rate), nullptr));
        pos = imgTitleBack->getPosition();
        imgTitleBack->runAction(Sequence::create(DelayTime::create(delayTime), EaseIn::create(MoveTo::create(moveDur, Point(pos.x - size.width, pos.y)), rate), nullptr));
        
        pos = rightDoor->getPosition();
        rightDoor->runAction(Sequence::create(DelayTime::create(delayTime), EaseIn::create(MoveTo::create(moveDur, Point(pos.x + size.width, pos.y)), rate), nullptr));
        pos = lblTip->getPosition();
        lblTip->runAction(Sequence::create(DelayTime::create(delayTime), EaseIn::create(MoveTo::create(moveDur, Point(pos.x + size.width, pos.y)), rate), nullptr));
        pos = imgTipBack->getPosition();
        imgTipBack->runAction(Sequence::create(DelayTime::create(delayTime), EaseIn::create(MoveTo::create(moveDur, Point(pos.x + size.width, pos.y)), rate), nullptr));
        this->scheduleOnce(schedule_selector(SceneChanger::removeThis), moveDur + delayTime);
    }
}

void SceneChanger::changeScene(float dt){
    Director::getInstance()->replaceScene(sceneToChange);
    sceneToChange->release();
}
void SceneChanger::removeThis(float dt){
    this->removeFromParent();
}
