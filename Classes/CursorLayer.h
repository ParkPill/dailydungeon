//
//  CursorLayer.h
//  LegendDaryTheTower
//
//  Created by SUNG PILL PACK on 14. 5. 19..
//
//
#ifndef __LegendDaryTheTower__CursorLayer__
#define __LegendDaryTheTower__CursorLayer__

//
//  HudLayer.h
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 29..
//
//

#include "cocos2d.h"

// Before HelloWorldLayer class declaration after #import "cocos2d.h"
using namespace cocos2d;

class CursorLayer : public Layer
{
private:
    cocos2d::Size size;
public:
    Sprite* cursor;
//    CallFuncN* onMouseDownCallback;
//    CallFuncN* onMouseUpCallback;
//    CallFuncN* onMouseDownCallback;
    CursorLayer(){};
//    ~CursorLayer(){};
    virtual bool init();
    CREATE_FUNC(CursorLayer);
    EventListenerMouse* mouseListner = nullptr;
    bool isMouseLeftDown = false;
    bool isMouseRightDown = false;
    void onMouseDown(cocos2d::Event* event);
    void onMouseUp(cocos2d::Event* event);
    void onMouseMove(cocos2d::Event* event);
    void onMouseScroll(cocos2d::Event* event);
    void Dispose();
};


#endif /* defined(__LegendDaryTheTower__CursorLayer__) */
