//
//  EpisodeLayer.cpp
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 14. 8. 21..
//
//

#include "EpisodeLayer.h"
#include "LanguageManager.h"
EpisodeLayer::EpisodeLayer()
{
    
}


bool EpisodeLayer::init(){
    bool bRet = false;
    do
    {
        CC_BREAK_IF(! Layer::init());

        size = CCDirector::getInstance()->getWinSize();
        dialogBox = AwesomeDialogBox::create();
        
        addChild(dialogBox, 100);

        bRet = true;
    } while (0);
	
    return bRet;
}

EpisodeLayer* EpisodeLayer::getEpisodeLayer(int index, CallFunc* finishCallback, CallFunc* actionCallback){
    EpisodeLayer* episode = EpisodeLayer::create();
    episode->retain();
    episode->finishCallback = finishCallback;
    episode->actionCallback = actionCallback;
    episode->setMessageForEpisode(index);
    episode->schedule(schedule_selector(EpisodeLayer::messageSchedule), 0.1);
    episode->isWaitingForActionFinished = false;
    episode->isTalkingDone = true;
    episode->messageIndex = 0;
    return episode;
}
void EpisodeLayer::setMessageForEpisode(int index){
    /*if(index == 0){
        for (int i = STR_EPISODE_0_0; i <= STR_EPISODE_0_12; i++) {
            messageArray.push_back(LanguageManager::getInstance()->getText(i));
        }
    }else if(index == 1){
        for (int i = STR_EPISODE_1_0; i <= STR_EPISODE_1_8; i++) {
            messageArray.push_back(LanguageManager::getInstance()->getText(i));
        }
    }else if(index == 2){
        for (int i = STR_EPISODE_2_0; i <= STR_EPISODE_2_8; i++) {
            messageArray.push_back(LanguageManager::getInstance()->getText(i));
        }
    }else if(index == 3){
        for (int i = STR_EPISODE_3_0; i <= STR_EPISODE_3_14; i++) {
            messageArray.push_back(LanguageManager::getInstance()->getText(i));
        }
    }*/
}
void EpisodeLayer::talkingDone(){
    isTalkingDone = true;
}
void EpisodeLayer::actionNeeded(){
    dialogBox->isWaiting = true;
    isWaitingForActionFinished = true;
    this->runAction(actionCallback);
}
void EpisodeLayer::actionFinished(){
    dialogBox->isWaiting = false;
    isWaitingForActionFinished = false;
    dialogBox->schedule(schedule_selector(AwesomeDialogBox::blink), 0.5);
}
void EpisodeLayer::messageSchedule(float dt){
    if(!isTalkingDone || isWaitingForActionFinished){
        return;
    }
    
    isTalkingDone = false;
    if (messageIndex >= messageArray.size()) {
        this->runAction(finishCallback);
        this->unschedule(schedule_selector(EpisodeLayer::messageSchedule));
        return;
    }
    this->showDialog((messageArray.at(messageIndex)).c_str());
    messageIndex++;
}
void EpisodeLayer::showDialog(const char* message){
    CallFunc* action = CallFunc::create(CC_CALLBACK_0(EpisodeLayer::talkingDone, this));
	action->retain();
	dialogBox->setVisible(true);
    if (false/*std::string(message).compare(STR_ACTION) == 0*/) {
        this->actionNeeded();
    }else{
        dialogBox->ShowDialog(message, this, action);
    }
}
void EpisodeLayer::removeUsedAssets(){
    dialogBox->removeUsedAssets();
    this->removeFromParent();
}