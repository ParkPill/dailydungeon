//
//  Episode.h
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 2015. 1. 14..
//
//

#ifndef __LegendDaryTwo__Episode__
#define __LegendDaryTwo__Episode__


#include "cocos2d.h"
#include "EpisodeLayer.h"

using namespace cocos2d;
class Episode : public Layer
{
    
private:
    
public:
    virtual bool init();
    CREATE_FUNC(Episode);
    static Episode* getEpisode();
    Size size;
    int episodeIndex;
    EpisodeLayer* episodeLayer;
    Sprite* sptCharLeft;
    Sprite* sptCharRight;
    
    void onSkip(Ref* obj);
    int episodeActionIndex;
    int getEpisodeIndex();
    void showEpisode();
    void episodeDone();
    void episodeActionFinished();
    void episodeActionNeeded();
    void spriteMoveDone(Ref* obj);
};


#endif /* defined(__LegendDaryTwo__Episode__) */
