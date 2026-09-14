

#ifndef __AlmostLegendary__FifteenSixLogo__
#define __AlmostLegendary__FifteenSixLogo__

#include "cocos2d.h"
#include <iostream>
using namespace cocos2d;
class FifteenSixLogo : public cocos2d::LayerColor
{
private:
    
public:
    Size size;
    bool isLab;
    Scene* sceneTitle;
    virtual bool init();
    CREATE_FUNC(FifteenSixLogo);
    float fadeInEnd;
    float blinkDur;
    float blinkStart;
    float dataWarningStart;
    float dataWarningEnd;
    float replaceSceneStart;
    float replaceSceneEnd;
    void dataWarning(float dt);
    void goMain();
    Layer* titleLayer;
    void ringMyBell();
    void showLogo();
    void showLabLogo();
    void showSplash();
    Sprite* logoDragon;
    Sprite* logo;
};
#endif /* defined(__AlmostLegendary__FifteenSixLogo__) */
