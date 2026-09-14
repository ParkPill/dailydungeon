//
//  EpisodeLayer.h
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 14. 8. 21..
//
//

#ifndef __LegendDaryTwo__EpisodeLayer__
#define __LegendDaryTwo__EpisodeLayer__


#include "cocos2d.h"
#include "AwesomeDialogBox.h"
using namespace cocos2d;
class EpisodeLayer : public Layer
{
public:
    Size size;
    AwesomeDialogBox* dialogBox;
    bool isTalkingDone;
    int messageIndex;
    std::vector<std::string> messageArray;
    CallFunc* finishCallback;
    CallFunc* actionCallback;
    bool isWaitingForActionFinished;
	EpisodeLayer();
	virtual bool init();
    CREATE_FUNC(EpisodeLayer);
    static EpisodeLayer* getEpisodeLayer(int index, CallFunc* finishCallback, CallFunc* actionCallback);
    void setMessageForEpisode(int index);
    void actionNeeded();
    void actionFinished();
    void messageSchedule(float dt);
    void showDialog(const char* message);
    void talkingDone();
    void removeUsedAssets();
};
#endif /* defined(__LegendDaryTwo__EpisodeLayer__) */
