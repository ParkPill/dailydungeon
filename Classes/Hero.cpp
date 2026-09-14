//
//  Hero.cpp
//  LegendDaryTwo
//
//  Created by SUNG PILL PACK on 14. 7. 9..
//
//

#include "Hero.h"
#include "GameManager.h"
Hero* Hero::createHero(int unit, int eng, float extraSpd){
    Hero* hero = new Hero();
    hero->autorelease();
    hero->Sprite::initWithSpriteFrameName("avatar0_run0body.png");
    hero->setScale(0.5f);
//    Sprite* sprite = Sprite::create("blackSquare.png");
//    sprite->setAnchorPoint(Point(0, 0));
//    sprite->setScale(hero->damageBoundingBox().size.width, hero->damageBoundingBox().size.height);
//    hero->addChild(sprite, 100);
    hero->getTexture()->setAliasTexParameters();
//    hero->setContentSize(Size(1, 20));
    hero->center = Point(17, 15);
    hero->energy = eng;
    hero->maxEnergy = eng;
    hero->setTag(unit);
    hero->extraSpeed = extraSpd;
    hero->weight = 1;
    hero->isFootStepStarted = false;
    hero->sptDropEffect = Sprite::createWithSpriteFrameName("dropEffect0.png");
    hero->sptDropEffect->setScaleX(0.5);
    hero->addChild(hero->sptDropEffect, 10);
    Animation* animation = AnimationCache::getInstance()->getAnimation("dropEffect");
    Animate* animate = Animate::create(animation);
    RepeatForever* forever =RepeatForever::create(animate);
    hero->sptDropEffect->runAction(forever);
    hero->sptDropEffect->setVisible(false);
    hero->sptDropEffect->setPosition(Point(7, 10));
    return hero;
}
void Hero::onDropping(){
    Droppable::onDropping();
    sptDropEffect->setVisible(true);
}
void Hero::updatePosition(float dt)
{
    Droppable::updatePosition(dt);
    if(GM->firePressed){
        if (isFlippedX()) {
            if (lastAngle > -90 && lastAngle < 90) {
//                setFlippedX(false);
            }
        }else{
            if (lastAngle < -90 || lastAngle > 90) {
//                setFlippedX(true);
            }
        }
    }
    setFlippedX(!(lastAngle > -90 && lastAngle < 90));
    
    if (onGround) {
        sptDropEffect->setVisible(false);
    }
    
}
void Hero::onJumped(){
    GameManager::getInstance()->playSoundEffect(SOUND_FOOT_STEP);
    GameManager::getInstance()->playSoundEffect(SOUND_FOOT_STEP);

    isHatUp = false;
//    this->face->setSpriteFrame(Sprite::createWithSpriteFrameName(GameManager::getInstance()->getPlayerInfoFileName(this->getTag(), PLAYER_INFO_FACE))->getSpriteFrame());
    
    stopAndStand();
//    addJumpingDirt();
//    if (heroType == HERO_DARY) {
//        this->runAnimation("daryJump", false);
//    }else if (heroType == HERO_AMOR) {
//        this->runAnimation("amorJump", false);
//    }
    this->runAnimation(strmake("avatarJump%d", heroType).c_str(), false);
    
//    leftLeg->runAction(MoveTo::create(0.1, Point(leftLeg->getPosition() + Point(-leftLeg->getContentSize().height + 1, leftLeg->getContentSize().height + leftLeg->getContentSize().width-1))));
//    leftLeg->runAction(RotateTo::create(0.1, 90));

//    rightLeg->runAction(MoveTo::create(0.1, Point(rightLeg->getPosition() + Point(rightLeg->getContentSize().height + rightLeg->getContentSize().width - 1, rightLeg->getContentSize().height-1))));
//    rightLeg->runAction(RotateTo::create(0.1, -90));
}
void Hero::startFootStep(){
    if (!isFootStepStarted) {
        isFootStepStarted = true;
        this->schedule(schedule_selector(Hero::stepSound), 0.2, kRepeatForever, 0.08);
    }
}
void Hero::stopFootStep(){
    if(isFootStepStarted){
        isFootStepStarted = false;
        this->unschedule(schedule_selector(Hero::stepSound));
    }
}
void Hero::addDirt(){
    if (inWater) return;
    int index = rand()%3;
    char buf[20];
    sprintf(buf, "dirt%d.png", index);
    Sprite* sptDirt = Sprite::createWithSpriteFrameName(buf);
    this->getParent()->addChild(sptDirt, 100);
    float dur = 0.5;
    int diff = 20;
    sptDirt->runAction(Sequence::create(EaseIn::create(MoveBy::create(dur, Point(rand()%diff - diff/2, diff/2)), 2), CallFuncN::create(CC_CALLBACK_1(Hero::spriteMoveDone, this)), NULL));
    sptDirt->runAction(Sequence::create(DelayTime::create(dur/2), FadeOut::create(dur/2),NULL));
    sptDirt->setPosition(this->getPosition() + Point(rand()%17 - 8, -this->getContentSize().height/2 - 1));
}
void Hero::addLandingDirt(){
    if (inWater) return;
    Sprite* sptDirt;
    for (int i = -20; i <= 20; i += 5) {
        if(i <= 10 && i >= -10) continue;
        if(i == -20 || i == 20) sptDirt = Sprite::createWithSpriteFrameName("dirt0.png");
        if(i == -15 || i == 15) sptDirt = Sprite::createWithSpriteFrameName("dirt1.png");
        this->getParent()->addChild(sptDirt, 100);
        float dur = 1;
        if(i == 20 || i == -20){
            sptDirt->runAction(EaseIn::create(MoveBy::create(dur, Point(0, 8)), 2));
        }else{
            sptDirt->runAction(EaseIn::create(MoveBy::create(dur, Point(0, 5)), 4));
        }
        
        sptDirt->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur, Point(i, 0)), 4), CallFuncN::create(CC_CALLBACK_1(Hero::spriteMoveDone, this)), NULL));
        sptDirt->runAction(Sequence::create(DelayTime::create(dur/2), FadeOut::create(dur/2),NULL));
        sptDirt->setPosition(this->getPosition() + Point(0, -this->getContentSize().height/2 - 6));
    }
}
void Hero::addJumpingDirt(){
    if (inWater) return;
    Sprite* sptDirt = Sprite::createWithSpriteFrameName("jumpEffect.png");
    int xDiff = 0;
    float dur = 0.5f;
    int angle = 0;
//    if(velocity.x > 0){
//        xDiff = -12;
//        angle = 30;
//    }else if(velocity.x == 0){
//        xDiff = 0;
//    }else{
//        xDiff = 12;
//        angle = -30;
//    }
    this->getParent()->addChild(sptDirt);
    sptDirt->setRotation(angle);
    sptDirt->runAction(EaseOut::create(MoveBy::create(dur, Point(0, -12)), 3));
    sptDirt->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur, Point(xDiff, 0)), 1.5), CallFuncN::create(CC_CALLBACK_1(Hero::spriteMoveDone, this)), NULL));
    sptDirt->runAction(Sequence::create(DelayTime::create(dur/2), FadeOut::create(dur/2),NULL));
    sptDirt->setPosition(this->getPosition() + Point(0, -this->getContentSize().height/2 + 5));
}

void Hero::spriteMoveDone(Ref* obj){
    Sprite* spt = (Sprite*)obj;
    spt->removeFromParentAndCleanup(true);
}
void Hero::stepSoundOnce(float dt){
    GameManager::getInstance()->playSoundEffect(SOUND_FOOT_STEP);
}
void Hero::stepSound(float dt){
    GameManager::getInstance()->playSoundEffect(SOUND_FOOT_STEP);
    addDirt();
}
void Hero::hatCheck(float dt){
    if (!isHatUp && velocity.y < -1.5) {
//        this->face->setSpriteFrame(Sprite::createWithSpriteFrameName(GameManager::getInstance()->getPlayerInfoFileName(this->getTag(), PLAYER_INFO_JUMP_FACE))->getSpriteFrame());
        isHatUp = true;
    }
}
void Hero::onLanded(){
    if (immortalByTumble) {
        immortalByTumble = false;
        immortal = false;
    }
    
    hand->setVisible(true);
//    face->setVisible(true);
    stopAnimationAndStand();
    oneWayCancelY = -1;
    Droppable::onLanded();
    
    isHatUp = false;
//    this->face->setSpriteFrame(Sprite::createWithSpriteFrameName(GameManager::getInstance()->getPlayerInfoFileName(this->getTag(), PLAYER_INFO_FACE))->getSpriteFrame());
    
    addLandingDirt();
    if(velocity.x != 0){
        startWalkingAnimation();
    }
    GameManager::getInstance()->playSoundEffect(SOUND_FOOT_STEP);
    GameManager::getInstance()->playSoundEffect(SOUND_FOOT_STEP);
    this->scheduleOnce(schedule_selector(Hero::stepSoundOnce), 0.04);
}
void Hero::getInTheVehicle(int vType, Sprite* vhc)
{
    //    Size size = Director::getInstance()->getWinSize();
    if (isTowardLeft) {
        vhc->setFlippedX(isTowardLeft);
    }
    sptDropEffect->setVisible(false);
    vehicle = vhc;
    vehicleType = vType;
}
void Hero::getOutTheVehicle()
{
    exitingVehicle = true;
    if (vehicleType) {
        vehicle->retain();
        this->removeChild(vehicle, true);
        vehicle = NULL;
        vehicleType = VEHICLE_NONE;
    }
}

void Hero::updatePositionForSuperBoard(float dt, bool jump, bool left, bool right)
{
    Point position = this->getPosition();
    if (left && !isTowardLeft) {
        isTowardLeft = true;
//        this->setFlippedX(isTowardLeft);
    }else if(right && isTowardLeft){
        isTowardLeft = false;
//        this->setFlippedX(isTowardLeft);
    }
    
    Point gravity = Point(0.0, inWater?-waterAccelYN:-850.0);
    if(vehicleType == VEHICLE_BALLOON) gravity.y = -gravity.y/2;
    if (onGround) {
//        gravity = Point(0.0, 0);
        exitingVehicle = false;
//        if (state == STATE_JUMPING) {
//            state = STATE_STANDING;
//            stopAnimationAndStand();
//            landed();
//        }

        jumped = false;
    }
    
    Point gravityStep = gravity * dt;
    
    Point vlct = this->velocity + gravityStep;
    
    if ((!left && !right && vehicleType != VEHICLE_CART) || (vehicleType == VEHICLE_BROOM && onGround)) {
        vlct = Point(inWater?vlct.x*0.9:vlct.x * 0.30, vlct.y); //2
    }
    
    float jumpCutoff = 150.0;
    
    if (jump) {
        if(vehicleType == VEHICLE_BROOM || vehicleType == VEHICLE_TOP){
            Point jumpForce = Point(0.0, onGround?80:0 + (inWater?waterAccelYP:20.0));
            if (vlct.y < 0) {
                vlct.y = 0;
            }
            vlct = (vlct + jumpForce);
        }else{
            if (wantsToJump && !jumped && (onGround || inWater || vehicleType == VEHICLE_BIRD)) {
                Point jumpForce = Point(0.0, inWater?(onGround?waterAccelYP+10:waterAccelYP):1850.0);
                vlct = vlct + jumpForce;
                
                wantsToJump = false;
                if (currentPlatform) {
                    currentPlatform->player = NULL;
                    currentPlatform = NULL;
                }
            }
        }
        onGround = false;
        jumped = true;
        if (currentPlatform) {
            currentPlatform->player = NULL;
            currentPlatform = NULL;
        }
    } else if (!jump && vlct.y > jumpCutoff) {
        
        // sppark: This allows you to jump to different height depending how long a player hold the jump button.
        //this->velocity = Point(this->velocity.x, jumpCutoff);
    }
//    if(jumped && !jump) jumped = false;
    
    if(vehicleType == VEHICLE_BROOM && this->onGround){
        
    }else{
        if (left || (vehicleType == VEHICLE_CART && isTowardLeft)) {
            Point backwardMove = Point(inWater?-waterAccelX:-2800.0, 0.0);
            Point backwardStep = backwardMove * dt; //1
            vlct = (vlct + backwardStep);
        }else if(right || (vehicleType == VEHICLE_CART && !isTowardLeft)){
            Point forwardMove = Point(inWater?waterAccelX:2800.0, 0.0);
            Point forwardStep = forwardMove * dt; //1
            vlct = vlct + forwardStep;
        }
    }
    
    
    Point minMovement;
    Point maxMovement;
    if(vehicleType == VEHICLE_CART){
        minMovement = Point(inWater?-waterMaxX:-150.0-extraSpeed, inWater?-waterMinY:-280.0 - extraJump);
        maxMovement = Point(inWater?waterMaxX:150.0+extraSpeed, 280.0 + extraJump);
    }else{
        minMovement = Point(inWater?-waterMaxX:-50.0-extraSpeed, inWater?-waterMinY:-180.0 - extraJump);
        maxMovement = Point(inWater?waterMaxX:50.0+extraSpeed, vehicleType == VEHICLE_BALLOON?80:180.0 + extraJump);
    }
    
    // Clamp allows you limit a hero's speed while Moving
    vlct = vlct.getClampPoint(minMovement, maxMovement);
    Point stepVelocity = vlct * dt;
    
    //this->setPosition(ccpAdd(getPosition(), stepVelocity));
    desiredPosition = getPosition() + stepVelocity;
    this->velocity = vlct;
}
void Hero::setInWater(bool inOrOut){
    Droppable::setInWater(inOrOut);
    if (inOrOut) {
        sptDropEffect->setVisible(false);
    }
}
void Hero::setRestOfBodyPart(){
    leftArmPos = Point(-2, 5);
    leftArmFlipPos = Point(-2, 5);
    rightArmPos = Point(5, 5);
    rightArmFlipPos = Point(5, 5);
    leftLegPos = Point(0, 0);
    rightLegPos = Point(6, 0);
//    bodyPos = Point(0, 1);
//    bodyFlipPos = Point(0, 1);
//    facePos = Point(3, 8);
//    faceFlipPos = Point(7, 8);

//    SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(GameManager::getInstance()->getPlayerInfoFileName(this->getTag(), PLAYER_INFO_BODY));
//    this->setDisplayFrame(frame);
    
//    body = Sprite::createWithSpriteFrameName(GameManager::getInstance()->getPlayerInfoFileName(this->getTag(), PLAYER_INFO_BODY));
//    this->addChild(body, 1);
//    body->setAnchorPoint(Point(0,0));
//    body->setPosition(bodyPos);
//    this->setSpriteFrame("dary_run0body.png");
    
    leftArmFlipPos = rightArmPos;
    rightArmFlipPos = leftArmPos;
    
//    leftLeg = Sprite::createWithSpriteFrameName(GameManager::getInstance()->getPlayerInfoFileName(this->getTag(), PLAYER_INFO_LEG));
//    this->addChild(leftLeg, 1);
//    leftLeg->setAnchorPoint(Point(0,0));
//    leftLeg->setPosition(leftLegPos);
    
//    rightLeg = Sprite::createWithSpriteFrameName(GameManager::getInstance()->getPlayerInfoFileName(this->getTag(), PLAYER_INFO_LEG));
//    this->addChild(rightLeg, 11);
//    rightLeg->setAnchorPoint(Point(0,0));
//    rightLeg->setPosition(rightLegPos);
    
//    face = Sprite::createWithSpriteFrameName(GameManager::getInstance()->getPlayerInfoFileName(this->getTag(), PLAYER_INFO_FACE));
    
    if (heroType == HERO_DARY) {
        face = Sprite::createWithSpriteFrameName("dary_head.png");
    }else if (heroType == HERO_AMOR) {
        face = Sprite::createWithSpriteFrameName("amor_head.png");
        
        Animation* animation = AnimationCache::getInstance()->getAnimation("amorHead");
        Animate* animate = Animate::create(animation);
        RepeatForever* forever = RepeatForever::create(animate);
        face->runAction(forever);
    }
    this->addChild(face, 20);
    face->setAnchorPoint(Point(0.38, 0.0));
    face->setVisible(false);
    float faceY = 20;
    facePos = Point(this->getContentSize().width*0.4f, faceY);
    faceFlipPos = Point(this->getContentSize().width*0.48f, faceY);
    face->setPosition(facePos);
    
    gun = Sprite::createWithSpriteFrameName(GameManager::getInstance()->getWeaponImageFileName(weapon));
//    gun->setScale(0.5f);
    gun->setAnchorPoint(Point(0, 0));
//    this->addChild(gun, 20);
    hand = Sprite::createWithSpriteFrameName("amor_hand.png");
    this->addChild(hand, -1);
    hand->setAnchorPoint(Vec2(0.2f, 0.5f));
    hand->setPosition(center);
    hand->addChild(gun);
    gun->setPosition(gunPos);
    velocity = Point(0, 0);
    gunPos = Vec2(hand->getContentSize().width - 10, hand->getContentSize().height - 10);
    gunFlipPos = Vec2(hand->getContentSize().width - 10, hand->getContentSize().height + 3);
    
    heroType = UDGetInt(KEY_AVATAR, 0);
    stopAnimationAndStand();
}
void Hero::setFeelingVisible(bool visible, int feeling){
    if (sptTalkBalloon == nullptr) {
        sptTalkBalloon = Sprite::createWithSpriteFrameName("talkBalloon.png");
        this->addChild(sptTalkBalloon);
        sptTalkBalloon->setAnchorPoint(Point(0.5, 0));
        sptTalkBalloon->setPosition(Point(this->getContentSize().width/2, this->getContentSize().height + 4));
    }
    if (feeling == FEELING_TALK) {
        sptTalkBalloon->setSpriteFrame("talkBalloon.png");
    }else if(feeling == FEELING_EXCLAIM){
        sptTalkBalloon->setSpriteFrame("exclaim.png");
    }
    sptTalkBalloon->setVisible(visible);
}
void Hero::setGunPos(int angle){
    if (lastAngle == angle) {
        return;
    }
    lastAngle = angle;
    float radius = 13.6f + gun->getContentSize().width/2;
    radius /= 2;
//    face->setRotation(angle);
    face->setRotation(0);
    gun->setRotation(angle);
    gun->setRotation(0);
    float x = cos(angle*3.14/180)*radius;
    float y = -sin(angle*3.14/180)*radius;
//    gunPos = center + Point(x, y);
//    gun->setPosition(gunPos);
    msStartPos = Vec2(1, 1) + Vec2(x, y);
//    handPos = center;// + Point(x, y);
//    flippedHandPos = Vec2(8, 16);
    hand->setPosition(isFlippedX()?flippedHandPos:handPos);
    hand->setRotation(angle);
    gun->setPosition(gunPos);
    
    if (angle < -90 || angle > 90) {
        gun->setFlippedY(true);
        gun->setAnchorPoint(Point(0, 1));
        gun->setPosition(gunFlipPos);
//        this->setFlippedX(true);
//        face->setFlippedY(true);
//        face->setAnchorPoint(Point(0.6, 0.8));
    }else{
        gun->setAnchorPoint(Point(0, 0));
        gun->setFlippedY(false);
        gun->setPosition(gunPos);
//        this->setFlippedX(false);
//        face->setFlippedY(false);
//        face->setAnchorPoint(Point(0.6, 0.2));
    }
}
void Hero::runHatCheck(){
    this->schedule(schedule_selector(Hero::hatCheck), 0.1);
}
void Hero::setWeapon(int weaponType, float power, int criPossibility){
    weapon = weaponType;
    weaponPower = power;
    this->weaponCriticalPossibility = criPossibility;
    const char* name = GameManager::getInstance()->getWeaponImageFileName(weapon);
    SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
    if (frame) {
        gun->setSpriteFrame(frame);
    }else{
        CCLOG("NULL!");
    }
}

void Hero::setFlippedX(bool flip){
//    this->armatureBody->setScaleX(flip?-1:1);
    Droppable::setFlippedX(flip);
//    armatureBody->setPosition(Point(flip?5:6, 5));
//    gun->setAnchorPoint(Point(flip?1:0, 0.5));
    if(appleWorm) appleWorm->setFlippedX(flip);
    if(this->getTag() == CHARACTER_DARY){
//        leftArm->setFlippedX(flip);
//        rightArm->setFlippedX(flip);
    }
    if(this->vehicleType != VEHICLE_NONE){
        this->vehicle->setFlippedX(flip);
    }
//    leftLeg->setFlippedX(!flip);
//    rightLeg->setFlippedX(!flip);
    if(face){
        face->setFlippedX(flip);
        face->setPosition(flip?faceFlipPos:facePos);
    }
//    body->setFlippedX(flip);
//    gun->setFlippedX(flip);
    if(hand){
//        hand->setPosition(flip?Vec2(9, 15):center);
        hand->setPosition(flip?flippedHandPos:handPos);
        hand->setFlippedY(flip);
    }
    
    if(flip){
        if(this->getTag() == CHARACTER_DARY){
//            leftArm->setPosition(leftArmFlipPos);
//            rightArm->setPosition(rightArmFlipPos);
        }
        
//        face->setPosition(faceFlipPos);
//        gun->setPosition(gunFlipPos);
//        body->setPosition(bodyFlipPos);
    }else{
        if(this->getTag() == CHARACTER_DARY){
//            leftArm->setPosition(leftArmPos);
//            rightArm->setPosition(rightArmPos);
        }
        
//        face->setPosition(facePos);
//        gun->setPosition(gunPos);
//        body->setPosition(bodyPos);
    }
}
void Hero::setGun(int gunType)
{
    
}

void Hero::tumbleAnimation()
{
    immortalByTumble = true;
    immortal = true;
    state = STATE_JUMPING;
    this->stopAnimationAndStand();
//    if (this->getTag() == CHARACTER_DARY) {
//        if (heroType == HERO_DARY) {
//            this->setSpriteFrame("dary_tumble1.png");
//            this->runAnimation("daryTumble", true);
//        }else if (heroType == HERO_AMOR) {
//            this->setSpriteFrame("amor_tumble0.png");
//            this->runAnimation("amorTumble", false);
//            addJumpingDirt();
//        }
//
//        hand->setVisible(false);
//        face->setVisible(false);
//    }else{
//        this->setSpriteFrame("dary_tumble1.png");
//        this->runAnimation("daryTumble", true);
//    }
    this->setSpriteFrame(strmake("avatar%d_tumble0.png", heroType));
    this->runAnimation(strmake("avatarTumble - %d", heroType).c_str(), false);
    /*
    switch (this->getTag()) {
        case CHARACTER_DARY:
            body->stopActionByTag(1);
            body->setPosition(isTowardLeft?bodyPointFlip:bodyPoint);
            //            body->setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("astronautSpin.png"));
            this->runAction(RotateBy::create(0.5, isTowardLeft?-360:360));
            GameManager::getInstance()->playSoundEffect(SOUND_SPRING);
            break;
    }*/
}
Point Hero::getGravityPosition()
{
    return getPosition();
    if(vehicleType == VEHICLE_TOP){
        if(velocity.y < 0){
//            return Point(getPosition().x, getPosition().y-2-TILE_SIZE);
//            return Point(getPosition().x, getPosition().y-getContentSize().height/2 + TILE_SIZE/2);
        }else if(velocity.y == 0){
//            return this->Droppable::getGravityPosition();
        }else{
            //Point(getPosition().x, getPosition().y-getContentSize().height/2 + TILE_SIZE/2);
//            return Point(getPosition().x, getPosition().y - 4 - TILE_SIZE + TILE_SIZE*2);
//            return Point(getPosition().x, getPosition().y-getContentSize().height/2 - TILE_SIZE/2);
        }
        return this->Droppable::getGravityPosition() + Point(0, -6);
    }
    return this->Droppable::getGravityPosition();
    Point pos = getPosition();
    if (vehicleType == VEHICLE_GOLIATH) {
        return Point(pos.x, pos.y - TILE_SIZE);
    }
    return pos;
}
Rect Hero::damageBoundingBox()
{
    return Rect(getPosition().x-3, getPosition().y -8, 6, 14);
    //    Rect bounding = boundingBox();
    Point pos = getPosition();
    if (vehicleType == VEHICLE_GOLIATH) {
        Rect collisionBox = Rect(pos.x - TILE_SIZE, pos.y - 6, TILE_SIZE*2, TILE_SIZE);
        Point diff = desiredPosition - getPosition();
        Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
        return returnBoundingBox;
    }
    
    Rect bounding = Rect(getPosition().x - 5, getPosition().y - 5, 10, 25);
//    Rect collisionBox = RectInset(Rect(bounding.origin.x + 2, bounding.origin.y + 4, 12, 18), 1, 0);
//    Point diff = desiredPosition - getPosition();
//    Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
//    return returnBoundingBox;
    return bounding;
}
void Hero::setAppleWorm(){
    if(appleWorm){
        appleWormDone(0);
        this->unschedule(schedule_selector(Hero::appleWormDone));
        this->unschedule(schedule_selector(Hero::appleWormBlink));
    }
    appleWorm = Sprite::createWithSpriteFrameName("appleWorm.png");
    this->addChild(appleWorm);
    appleWorm->setPosition(Point(5, 30));
    originalExtraSpeed = this->extraSpeed;
    this->extraSpeed = 0;
    this->scheduleOnce(schedule_selector(Hero::appleWormBlink), 5);
}
void Hero::appleWormBlink(float dt){
    Sequence* seq = Sequence::create(FadeOut::create(0.1), DelayTime::create(0.2), FadeIn::create(0.1), DelayTime::create(0.2), NULL);
    RepeatForever* repeat = RepeatForever::create(seq);
    appleWorm->runAction(repeat);
    this->scheduleOnce(schedule_selector(Hero::appleWormDone), 2);
}
void Hero::appleWormDone(float dt){
    appleWorm->removeFromParentAndCleanup(true);
    appleWorm = NULL;
    this->extraSpeed = originalExtraSpeed;
}
float Hero::getCurrentY(){
    if(this->vehicleType == VEHICLE_TOP) return getPositionY()-2-12;
    return getPositionY();
}
Rect Hero::collisionBoundingBox()
{
//    if(this->vehicleType == VEHICLE_TOP) return Rect(desiredPosition.x, desiredPosition.y-2-12 , 10, TILE_SIZE*2.5);
    Rect rect = getBoundingBox();
//    return Rect(desiredPosition.x - rect.size.width/2 + 2, desiredPosition.y - rect.size.height/2, rect.size.width - 5, rect.size.height - 4);
    return Rect(desiredPosition.x - rect.size.width/2 + 2, desiredPosition.y - rect.size.height/2, rect.size.width - 5, 14);
}
void Hero::runFireAnimation()
{
    hand->stopAllActions();
    hand->setRotation(lastAngle);
//    gun->setPosition(gunPos);
//    setGunPos(lastAngle);
    if (lastAngle < -90 || lastAngle > 90) {
        hand->runAction(Sequence::create(RotateBy::create(0.03, 30), RotateBy::create(0.1, -30), CallFunc::create(CC_CALLBACK_0(Hero::setGunPosHandler, this)),nullptr));
//        hand->runAction(Sequence::create(MoveBy::create(0.02, Point(3, 0)), MoveBy::create(0.1, Point(-3, 0)), NULL));
    }else{
        hand->runAction(Sequence::create(RotateBy::create(0.03, -30), RotateBy::create(0.1, 30), CallFunc::create(CC_CALLBACK_0(Hero::setGunPosHandler, this)), nullptr));
//        hand->runAction(Sequence::create(MoveBy::create(0.02, Point(-3, 0)), MoveBy::create(0.1, Point(3, 0)), NULL));
    }
}
void Hero::setGunPosHandler(){
    setGunPos(lastAngle+1);
}

void Hero::startWalkingAnimation()
{
    log("start walking");
    if(this->vehicleType != VEHICLE_NONE) return;
    this->stopAnimationAndStand();
    face->stopActionByTag(TAG_NODDING);
//    face->setFlippedX(!isTowardLeft);
//    face->setPosition(isTowardLeft?faceFlipPos:facePos);
    state = STATE_WALKING;
    
//    if (this->getTag() == CHARACTER_DARY) {
//        if (heroType == HERO_DARY) {
//            this->runAnimation("daryRun", true);
//        }else if (heroType == HERO_AMOR) {
//            this->runAnimation("amorRun", true);
//        }
//    }else{
//        this->runAnimation("daryRun", true);
//    }
    this->runAnimation(strmake("avatarRun%d", heroType).c_str(), true);
//    leftLeg->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.1, Point(1,0)), MoveBy::create(0.1, Point(-1,0)), NULL)));
//    rightLeg->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.1, Point(-1,0)), MoveBy::create(0.1, Point(1,0)), NULL)));
}
void Hero::stopAnimationAndStand()
{
    log("stop walking");
//    leftLeg->stopAllActions();
//    leftLeg->setPosition(leftLegPos);
//    leftLeg->setRotation(0);
    
//    rightLeg->stopAllActions();
//    rightLeg->setPosition(rightLegPos);
//    rightLeg->setRotation(0);
    this->stopActionByTag(ACTION_TAG_ANIMATION);
//    face->setPosition(isTowardLeft?faceFlipPos:facePos);
    
//    if (this->getTag() == CHARACTER_DARY) {
//        if (heroType == HERO_DARY) {
//            this->setSpriteFrame("dary_run0body.png");
//        }else if (heroType == HERO_AMOR) {
////            this->setSpriteFrame("amor_run0body.png");
//            this->runAnimation("amorIdle", true);
//        }
//    }else{
//        this->setSpriteFrame("dary_run0body.png");
//    }
    this->setSpriteFrame(strmake("avatar%d_run0body.png", heroType));
    if(heroType < 2){
        handPos = Point(17, 15);
        flippedHandPos = Vec2(9, 15);
    }else if(heroType == 2){
        handPos = Point(22, 17);
        flippedHandPos = Vec2(8, 16);
    }else if(heroType == 3){
        handPos = Point(18, 17);
        flippedHandPos = Vec2(8, 16);
    }else if(heroType == 4){
        handPos = Point(22, 17);
        flippedHandPos = Vec2(8, 16);
    }else if(heroType == 5){
        handPos = Point(19, 17);
        flippedHandPos = Vec2(9, 16);
    }else if(heroType == 6){
        handPos = Point(22, 18);
        flippedHandPos = Vec2(14, 17);
    }else if(heroType == 7){
        handPos = Point(20, 18);
        flippedHandPos = Vec2(8, 17);
    }else if(heroType == 8){
        handPos = Point(21, 18);
        flippedHandPos = Vec2(12, 18);
    }else if(heroType == 9){
        handPos = Point(25, 17);
        flippedHandPos = Vec2(16, 17);
    }else if(heroType == 10){
        handPos = Point(28, 19);
        flippedHandPos = Vec2(16, 19);
    }else if(heroType == 11){
        handPos = Point(20, 17);
        flippedHandPos = Vec2(12, 16);
    }else if(heroType == 12){
        handPos = Point(23, 18);
        flippedHandPos = Vec2(9, 18);
    }else if(heroType == 13){
        handPos = Point(24, 18);
        flippedHandPos = Vec2(11, 18);
    }else if(heroType == 14){
        handPos = Point(20, 18);
        flippedHandPos = Vec2(8, 18);
    }else if(heroType == 15){
        handPos = Point(18, 17);
        flippedHandPos = Vec2(9, 18);
    }else if(heroType == 16){
        handPos = Point(23, 16);
        flippedHandPos = Vec2(13, 17);
    }else if(heroType == 17){
        handPos = Point(25, 18);
        flippedHandPos = Vec2(11, 17);
    }else if(heroType == 18){
        handPos = Point(25, 17);
        flippedHandPos = Vec2(11, 16);
    }else if(heroType == 19){
        handPos = Point(22, 18);
        flippedHandPos = Vec2(7, 17);
    }else if(heroType == 20){
        handPos = Point(22, 18);
        flippedHandPos = Vec2(13, 16);
    }else if(heroType == 21){
        handPos = Point(25, 15);
        flippedHandPos = Vec2(14, 15);
    }
    hand->setPosition(isFlippedX()?flippedHandPos:handPos);
    this->runAnimation(strmake("avatarIdle%d", heroType).c_str(), true);
    this->hand->setSpriteFrame(strmake("avatar%d_hand.png", heroType));
    state = STATE_STANDING;
}

void Hero::stopAndStand(){
//    armatureBody->getAnimation()->play("standing");
//    return;
//    leftLeg->runAction(RotateTo::create(0.2, 0));
//    rightLeg->runAction(RotateTo::create(0.2, 0));
//    leftLeg->setPosition(leftLegPos);
//    rightLeg->setPosition(rightLegPos);
    wantsToGoLeft = false;
    wantsToGoRight = false;
    stopAnimationAndStand();
    face->stopActionByTag(TAG_NODDING);
//    face->setPosition(isTowardLeft?faceFlipPos:facePos);
    Action* nodding = RepeatForever::create(Sequence::create(MoveBy::create(0.28, Point(0, -0.5)), MoveBy::create(0.28, Point(0, 0.5)), NULL));
    nodding->setTag(TAG_NODDING);
    face->runAction(nodding);
}
void Hero::goLeft(){
    wantsToGoLeft = true;
}
void Hero::goRight(){
    wantsToGoRight = true;
}
void Hero::jump(){
    wantsToJump = true;
}
void Hero::setColorTo(Color3B color){
//    this->body->setColor(color);
    this->face->setColor(color);
    if (this->getTag() == CHARACTER_DARY) {
        this->leftArm->setColor(color);
        this->rightArm->setColor(color);
    }
    this->leftLeg->setColor(color);
    this->rightLeg->setColor(color);
}
