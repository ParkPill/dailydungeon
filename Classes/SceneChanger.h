//
//  SceneChanger.hpp
//  DaryDual
//
//  Created by PACKSUNG PILL on 31/05/2017.
//
//

#ifndef SceneChanger_hpp
#define SceneChanger_hpp

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "ui/UIScrollView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"
#include "ui/UIPageView.h"
#include "base/CCGameController.h"
#include "editor-support/cocostudio/CocoStudio.h"
using namespace cocos2d;
using namespace cocos2d::ui;
class SceneChanger : public Layer
{
private:
public:
    cocos2d::Size size;
    Node* layer = nullptr;
    virtual bool init();
    CREATE_FUNC(SceneChanger);
    void setLayer(bool isClosing, std::string title, std::string tip, Scene* scene);
    Scene* sceneToChange;
    
    void changeScene(float dt);
    void removeThis(float dt);
};



#endif /* SceneChanger_hpp */
