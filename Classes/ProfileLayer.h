//
//  ProfileLayer.hpp
//  LegendDaryDungeon
//
//  Created by PACKSUNG PILL on 03/01/2019.
//

#ifndef ProfileLayer_hpp
#define ProfileLayer_hpp

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "ui/UIScrollView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"
#include "ui/UIPageView.h"
#include "base/CCGameController.h"

using namespace cocos2d;
using namespace cocos2d::ui;
class ProfileLayer : public Layer
{
private:
public:
    cocos2d::Size size;
    virtual bool init();
    CREATE_FUNC(ProfileLayer);
    
};





#endif /* ProfileLayer_hpp */
