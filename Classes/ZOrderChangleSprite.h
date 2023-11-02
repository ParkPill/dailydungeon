//
//  ZOrderChangleSprite.h
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 12/14/14.
//
//

#ifndef __LegendDaryTwo__ZOrderChangleSprite__
#define __LegendDaryTwo__ZOrderChangleSprite__
#include "cocos2d.h"
USING_NS_CC;
class ZOrderChangableSprite : public Sprite{
public:
    void changeZOrderUp();
    void changeZOrderDown();
    void initWithSpriteFrameNameYo(const char* name);
};
#endif /* defined(__LegendDaryTwo__ZOrderChangleSprite__) */
