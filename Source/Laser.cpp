//
//  Laser.cpp
//  LegendDaryDungeon
//
//  Created by PACKSUNG PILL on 8/4/16.
//
//

#include "Laser.h"
#include "GameManager.h"
Laser* Laser::create(std::string laserName, std::string gatheringName, std::string hitAniName, TMXTiledMap* theMap){
    Laser* laser = new Laser();
    laser->init();
    laser->map = theMap;
    laser->stageLayer = theMap->getLayer("stage");
    Sprite* beam = Sprite::create(laserName);
    laser->sptBeam = beam;
    laser->addChild(laser->sptBeam);
    laser->sptHit = Sprite::create(gatheringName);
    Animation* animation = AnimationCache::getInstance()->getAnimation(hitAniName);
    if (animation != nullptr) {
        Animate* animate = Animate::create(animation);
        laser->sptHit->runAction(RepeatForever::create(animate));
    }else{
        laser->sptHit->runAction(RepeatForever::create(RotateBy::create(1, 460)));
    }
    laser->addChild(laser->sptHit);
    
    laser->setAnchorPoint(Point(0, 0.5));
    laser->sptBeam->setAnchorPoint(Point(0, 0.5));
    laser->schedule(schedule_selector(Laser::updateLaser), 0.07f);
    laser->gatheringImageName = gatheringName;
    
    return laser;
}
void Laser::changeLook(std::string laserName, std::string gatheringName, std::string hitAniName){
    sptHit->stopAllActions();
    Animation* animation = AnimationCache::getInstance()->getAnimation(hitAniName);
    Animate* animate = Animate::create(animation);
    sptHit->runAction(RepeatForever::create(animate));
}
Laser::Laser(){
    maxLength = 200;
    gatheringIntervalMax = 0.2;
    gatheringInterval = gatheringIntervalMax;
}
void Laser::setLaser(cocos2d::Point startPos, float angle){
    this->setPosition(startPos);
    laserAngle = angle;
    slope = -tan(laserAngle*3.14f/180);
}
void Laser::setLaserOn(bool isOn){
    this->setVisible(true);
}
void Laser::updateLaser(float dt){
    if (!isVisible()) {
        return;
    }
    
    Vector<EnemyBase*> enemies;
    Vector<EnemyBase*> destructables;
//    https://nolongertooyoung2.tumblr.com/post/182653842073#notes
//    https://supersciencebiologycroissant.tumblr.com/following
    if(GM->currentStageIndex == STAGE_DUAL){
        enemies = enemyArray;//DUAL_WORLD->enemyArray;
//        enemies.pushBack(DUAL_WORLD->destructableArray);
//        destructables = DUAL_WORLD->destructableArray;
    }else{
        enemies = WORLD->enemyArray;
//        enemies.pushBack(WORLD->destructableArray);
        destructables = WORLD->destructableArray;
    }
    int mapTileHeight = map->getMapSize().height;
    int mapTileWidth = map->getMapSize().width;
    int mapWidth = mapTileWidth*TILE_SIZE;
    int mapHeight = mapTileHeight*TILE_SIZE;
    bool isUp = false;
    bool isDown = false;
    
    while(laserAngle < 0){
        laserAngle += 360;
    }
    if(laserAngle > 360){
        laserAngle = (int)laserAngle%360;
    }
    if(laserAngle > 180){
        isUp = true;
    }else if(laserAngle < 180){
        isDown = true;
    }
    bool isLeft = false;
    bool isRight = false;
    if(laserAngle > 90 && laserAngle < 270){
        isLeft = true;
    }else if(laserAngle < 90 || laserAngle > 270){
        isRight = true;
    }
    float blockLength = 10000;
    float enemyLength = 10000;
    log("laser angle: %f", laserAngle);
    int mapX = map->getPositionX();
    int mapY = map->getPositionY();
    int x = this->getPositionX() - mapX;
    int y = this->getPositionY() - mapY;
    int worldX = this->getPositionX();
    int worldY = this->getPositionY();
    if(getPosition() == Point::ZERO){
        return;
    }
    float diff = 0;
//    int tileY = y/TILE_SIZE;
//    int tileX = x/TILE_SIZE;
    int tempX;int tempY;float tempLength;
    if (isUp) {
        tempX = x;
        diff = TILE_SIZE - y%TILE_SIZE;
        tempX += diff/slope;
        for (int i = y+diff; i < mapHeight; i+=TILE_SIZE) { // detect block
            if(tempX < 0 || tempX >= mapWidth || i < 0 || i >= mapHeight){
                break;
            }
            int gid = stageLayer->getTileGIDAt(Vec2(tempX/TILE_SIZE, mapTileHeight - (i + 2)/TILE_SIZE - 1));
            if(gid > 0 && !isOneWay(gid)){
                blockLength = sqrt((i - y)*(i - y) + (tempX - x)*(tempX - x));
                break;
            }
            tempX += TILE_SIZE/slope;
        }
        
        for(auto enemy: enemies){   // detect enemy
            if(enemy->getBoundingBox().getMaxY() < worldY || enemy->energy <= 0){
                continue;
            }
            tempY = enemy->getPositionY();
            tempX = worldX + (tempY - worldY)/slope;
            if(enemy->getBoundingBox().containsPoint(Vec2(tempX, tempY + 1))){
                int tempLength = sqrt((tempY - worldY)*(tempY - worldY) + (tempX - worldX)*(tempX - worldX));
                if(tempLength < blockLength && tempLength < enemyLength){
                    enemyLength = tempLength;
                    hitEnemy = enemy;
                    break;
                }
            }
        }
        for(auto enemy: destructables){   // detect enemy
            if(enemy->getBoundingBox().getMaxY() < worldY || enemy->energy <= 0){
                continue;
            }
            tempY = enemy->getPositionY();
            tempX = worldX + (tempY - worldY)/slope;
            if(enemy->getBoundingBox().containsPoint(Vec2(tempX, tempY + 1))){
                int tempLength = sqrt((tempY - worldY)*(tempY - worldY) + (tempX - worldX)*(tempX - worldX));
                if(tempLength < blockLength && tempLength < enemyLength){
                    enemyLength = tempLength;
                    hitEnemy = enemy;
                    break;
                }
            }
        }
    }
    if (isDown) {
        tempX = x;
        diff = y%TILE_SIZE;
        log("y: %f", map->getPositionY());
        tempX -= diff/slope;
        for (int i = y-diff; i > 0; i-=TILE_SIZE) {
            if(tempX <= 0 || tempX >= mapWidth || i <= 0 || i >= mapHeight){
                break;
            }
            int gid = stageLayer->getTileGIDAt(Vec2(tempX/TILE_SIZE, mapTileHeight - (i-2)/TILE_SIZE - 1));
            if(gid > 0 && !isOneWay(gid)){
                blockLength = sqrt((i - y)*(i - y) + (tempX - x)*(tempX - x));
                break;
            }
            tempX -= TILE_SIZE/slope;
        }
        for(auto enemy: enemies){   // detect enemy
            if(enemy->getBoundingBox().getMinY() > worldY  || enemy->energy <= 0){
                continue;
            }
            tempY = enemy->getBoundingBox().getMaxY();
            tempX = worldX + (tempY - worldY)/slope;
            if(enemy->getBoundingBox().containsPoint(Vec2(tempX, tempY - 1))){
                int tempLength = sqrt((tempY - worldY)*(tempY - worldY) + (tempX - worldX)*(tempX - worldX));
                if(tempLength < blockLength && tempLength < enemyLength){
                    enemyLength = tempLength;
                    hitEnemy = enemy;
                    break;
                }
            }
        }
        for(auto enemy: destructables){   // detect enemy
            if(enemy->getBoundingBox().getMinY() > worldY || enemy->energy <= 0){
                continue;
            }
            tempY = enemy->getBoundingBox().getMaxY();
            tempX = worldX + (tempY - worldY)/slope;
            if(enemy->getBoundingBox().containsPoint(Vec2(tempX, tempY - 1))){
                int tempLength = sqrt((tempY - worldY)*(tempY - worldY) + (tempX - worldX)*(tempX - worldX));
                if(tempLength < blockLength && tempLength < enemyLength){
                    enemyLength = tempLength;
                    hitEnemy = enemy;
                    break;
                }
            }
        }
    }
    if (isLeft) {
        tempY = y;
        diff = x%TILE_SIZE;
        tempY -= diff*slope;
        for (int i = x-diff; i > 0; i-=TILE_SIZE) {
            if(i <= 0 || i >= mapWidth || tempY <= 0 || tempY >= mapHeight){
                break;
            }
            int gid = stageLayer->getTileGIDAt(Vec2((i-2)/TILE_SIZE, mapTileHeight - tempY/TILE_SIZE - 1));
            if(gid > 0 && !isOneWay(gid)){
                tempLength = sqrt((tempY - y)*(tempY - y) + (i - x)*(i - x));
                if(tempLength < blockLength){
                    blockLength = tempLength;
                }
                break;
            }
            tempY -= TILE_SIZE*slope;
        }
        for(auto enemy: enemies){   // detect enemy
            if(enemy->getBoundingBox().getMinX() > worldX  || enemy->energy <= 0){
                continue;
            }
            tempX = enemy->getBoundingBox().getMaxX();
            tempY = worldY + (tempX - worldX)*slope;
            if(enemy->getBoundingBox().containsPoint(Vec2(tempX - 1, tempY))){
                int tempLength = sqrt((tempY - worldY)*(tempY - worldY) + (tempX - worldX)*(tempX - worldX));
                if(tempLength < blockLength && tempLength < enemyLength){
                    enemyLength = tempLength;
                    hitEnemy = enemy;
                    break;
                }
            }
        }
        for(auto enemy: destructables){   // detect enemy
            if(enemy->getBoundingBox().getMinX() > worldX || enemy->energy <= 0){
                continue;
            }
            tempX = enemy->getBoundingBox().getMaxX();
            tempY = worldY + (tempX - worldX)*slope;
            if(enemy->getBoundingBox().containsPoint(Vec2(tempX - 1, tempY))){
                int tempLength = sqrt((tempY - worldY)*(tempY - worldY) + (tempX - worldX)*(tempX - worldX));
                if(tempLength < blockLength && tempLength < enemyLength){
                    enemyLength = tempLength;
                    hitEnemy = enemy;
                    break;
                }
            }
        }
    }
    if (isRight) {
        tempY = y;
        diff = TILE_SIZE-x%TILE_SIZE;
        tempY += diff*slope;
        for (int i = x+diff; i < mapWidth; i+=TILE_SIZE) {
            if(i <= 0 || i >= mapWidth || tempY <= 0 || tempY >= mapHeight){
                break;
            }
//            log("width: %f", stageLayer->getLayerSize().width);
            int gid = stageLayer->getTileGIDAt(Vec2((i+2)/TILE_SIZE, mapTileHeight - tempY/TILE_SIZE - 1));
            if(gid > 0 && !isOneWay(gid)){
                tempLength = sqrt((tempY - y)*(tempY - y) + (i - x)*(i - x));
                if(tempLength < blockLength){
                    blockLength = tempLength;
                }
                break;
            }
            tempY += TILE_SIZE*slope;
        }
        for(auto enemy: enemies){   // detect enemy
            if(enemy->getBoundingBox().getMaxX() < worldX || enemy->energy <= 0){
                continue;
            }
            tempX = enemy->getBoundingBox().getMinX();
            tempY = worldY + (tempX - worldX)*slope;
            if(enemy->getBoundingBox().containsPoint(Vec2(tempX + 1, tempY))){
                int tempLength = sqrt((tempY - worldY)*(tempY - worldY) + (tempX - worldX)*(tempX - worldX));
                if(tempLength < blockLength && tempLength < enemyLength){
                    enemyLength = tempLength;
                    hitEnemy = enemy;
                    break;
                }
            }
        }
        for(auto enemy: destructables){   // detect enemy
            if(enemy->getBoundingBox().getMaxX() < worldX || enemy->energy <= 0){
                continue;
            }
            tempX = enemy->getBoundingBox().getMinX();
            tempY = worldY + (tempX - worldX)*slope;
            if(enemy->getBoundingBox().containsPoint(Vec2(tempX + 1, tempY))){
                int tempLength = sqrt((tempY - worldY)*(tempY - worldY) + (tempX - worldX)*(tempX - worldX));
                if(tempLength < blockLength && tempLength < enemyLength){
                    enemyLength = tempLength;
                    hitEnemy = enemy;
                    break;
                }
            }
        }
    }
    if (blockLength > enemyLength) {
        blockLength = enemyLength;
    }
//    log("blockLength: %f", blockLength);
    this->setRotation(laserAngle);
    timer+=dt;
    if (timer > 0.05f) {
        timer = 0;
        gointUp = !gointUp;
        if (!gointUp) {
            scaleY = 1;
        }
    }
    if (gointUp) {
        scaleY += 0.1f;
    }else{
        scaleY -= 0.1f;
    }
    Sprite* sptGathering = Sprite::create(gatheringImageName);
    this->addChild(sptGathering);
    int gatheringRadius = 8;
    sptGathering->runAction(RotateBy::create(0.5, 180));
    sptGathering->setPosition(Point(cos((rand()*90 - 90)*3.14/180)*gatheringRadius, -sin((rand()*90 - 90)*3.14/180)*gatheringRadius));
    sptGathering->runAction(Sequence::create(EaseIn::create(MoveTo::create(0.3, Point(0, this->getContentSize().height/2)), 2), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptGathering)), NULL));
    sptBeam->setScale(blockLength/sptBeam->getContentSize().width, scaleY);
    sptHit->setPosition(Point(blockLength, 0));
//    sptHit->setRotation(-laserAngle);
}

bool Laser::isOneWay(int index){
    switch (index) {
        case 130:
        case 133:
        case 134:
        case 135:
        case 324:
        case 325:
        case 356:
        case 357:
        case 258:
        case 261:
        case 262:
        case 263:
            return true;
            
        default:
            return false;
    }
}
void LightningLine::createLine(std::string line, std::string tip, Point startPos, Point endPos, Node* parent){
    LightningLine* sptStartTip = new LightningLine();
    sptStartTip->initWithFile(tip);
    sptStartTip->getTexture()->setAntiAliasTexParameters();
    sptStartTip->setAnchorPoint(Point(1, 0.5));
    sptStartTip->setPosition(startPos);
    float angle = -atan2((endPos.y - startPos.y), (endPos.x - startPos.x))*180/3.14f;
    sptStartTip->setRotation(angle);
    //sptStartTip->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
    parent->addChild(sptStartTip);
    float delay = 0.01f;
    float fade = 0.6f;
    sptStartTip->runAction(Sequence::create(DelayTime::create(delay), EaseOut::create(FadeOut::create(fade), 2), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptStartTip)), NULL));
    float length = sqrtf((endPos.y - startPos.y)*(endPos.y - startPos.y) + (endPos.x - startPos.x)*(endPos.x - startPos.x));
    
    Sprite* sptLine = Sprite::create(line);
    sptLine->setAnchorPoint(Point(0, 0.5));
    sptLine->getTexture()->setAntiAliasTexParameters();
    sptLine->setPosition(startPos);
    sptLine->setRotation(angle);
    sptLine->setScaleX(length/sptLine->getContentSize().width);
    //sptLine->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
    parent->addChild(sptLine);
    sptLine->runAction(Sequence::create(DelayTime::create(delay), EaseOut::create(FadeOut::create(fade), 2), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptStartTip)), NULL));
    
    Sprite* sptEndTip = Sprite::create(tip);
    sptEndTip->getTexture()->setAntiAliasTexParameters();
    sptEndTip->setAnchorPoint(Point(1, 0.5));
    sptEndTip->setRotation(180 + angle);
    sptEndTip->setPosition(endPos);
    //sptEndTip->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
    parent->addChild(sptEndTip);
    sptEndTip->runAction(Sequence::create(DelayTime::create(delay), EaseOut::create(FadeOut::create(fade), 2), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptStartTip)), NULL));
}
Lightning* Lightning::create(std::string lineName, std::string tipName, cocos2d::Point startPos, float angle, Vector<EnemyBase*> enemyArray, std::vector<cocos2d::Rect> rectArray){
    Lightning* lightning = new Lightning();
    lightning->init();
//    lightning->setSpriteFrame(lineName);
    
    Point finalPos;
    float length = 10;
    bool checked = false;
    while (true) {
        float x= cos(-angle*3.14/180)*length;
        float y = sin(-angle*3.14/180)*length;
        
        for (int i = 0; i < rectArray.size(); i++) {// tile
            finalPos = startPos + Point(x, y);
            if (rectArray.at(i).containsPoint(finalPos)) {
                checked = true;
                break;
            }
        }
        
        if (checked) {
            break;
        }
        
        length += 1;
        if (length > lightning->maxLength) {
            break;
        }
    }
//    angle += 90;
    float finalLength = length;
    int nextLength = 0;
    int lengthUnit;
    Point nextPos;
    Point previousPos = startPos;
    int extraLength = 5;
    int minLengthUnit = 5;
    int verticalGap = 5;
    int verticalGapMade;
    lightning->hitPointArray = PointArray::create(200);
    lightning->hitPointArray->addControlPoint(startPos);
    angle = -angle;
    Point zagPos;
    int counter = 0;
    Node* light;
    while(true){
        lengthUnit = minLengthUnit + rand()%extraLength;
        nextLength += lengthUnit;
        verticalGapMade = rand()%(verticalGap*2) - verticalGap;
        if (finalLength < nextLength) {
            nextLength = finalLength;
            zagPos = Point::ZERO;
        }else{
            zagPos = Point(cos((angle+90)*3.14/180)*verticalGapMade, sin((angle+90)*3.14/180)*verticalGapMade);
        }
        
        nextPos = Point(cos(angle*3.14/180)*nextLength, sin(angle*3.14/180)*nextLength) + startPos + zagPos;
        for (int i = 0; i < enemyArray.size(); i++) {// tile
             if (enemyArray.at(i)->getBoundingBox().containsPoint(nextPos)) {
                 if (!lightning->hitEnemyArray.contains(enemyArray.at(i))) {
                     lightning->hitEnemyArray.pushBack(enemyArray.at(i));
                 }
             }
        }
        LightningLine::createLine(lineName, tipName, previousPos, nextPos, lightning);
        if (counter%4 == 0) {
            light = GameManager::getInstance()->getCurrentStageLayer()->getLight();
            light->runAction(Sequence::create(DelayTime::create(0.05), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, light)), NULL));
            light->setColor(Color3B(50, 86, 250));
            lightning->addChild(light);
            light->setPosition(nextPos);
        }
        previousPos = nextPos;
        counter++;
        if (finalLength == nextLength) {
            break;
        }
    }
    float dur = 0.5;
    for (int i = 0; i < 8; i++) {
        Sprite* sptSpark = Sprite::create("smallLight.png");
        sptSpark->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
        lightning->addChild(sptSpark);
        sptSpark->setPosition(nextPos);
        sptSpark->setScale(2);
        sptSpark->runAction(FadeOut::create(dur));
        sptSpark->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur, Point(rand()%30 - 15, rand()%10 - 5)), 3), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptSpark)), NULL));
    }
    light = GameManager::getInstance()->getCurrentStageLayer()->getLight();
    light->runAction(Sequence::create(DelayTime::create(0.05), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, light)), NULL));
    light->setColor(Color3B(50, 86, 250));
    lightning->addChild(light);
    light->setPosition(startPos);
    
    light = GameManager::getInstance()->getCurrentStageLayer()->getLight();
    light->runAction(Sequence::create(DelayTime::create(0.05), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, light)), NULL));
    light->setColor(Color3B(50, 86, 250));
    lightning->addChild(light);
    light->setPosition(finalPos);
    return lightning;
}
