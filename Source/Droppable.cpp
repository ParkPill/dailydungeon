//
//  Droppable.cpp
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 26..
//
//

#include "Droppable.h"
#include "GameManager.h"

//bool Droppable::init(){}
Droppable::Droppable()
{
    ignoreGravity = false;
    waterAccelX = 100;
    waterMaxX = 50;
    waterAccelYN = 100;
    waterAccelYP = 90;
    waterMinY = 60;
    inWater = false;
    isTowardLeft = false;
    isFregile = false;
    exitingVehicle = false;
    state = STATE_STANDING;
    xSpeed = 0;
    ySpeed = 0;
    velocity = Point(0,-100);
    jumped = false;
    blinkingTime = 0;
    freezed = false;
    immortal = false;
    centerPosition = Point(0,0);
    currentPlatform = NULL;
    untouchable = false;
    onGround = false;
    wantsToStop = false;
    wantsToGoLeft = false;
    wantsToGoRight = false;
    wantsToJump = false;
    weight = 1;
    target = NULL;
    effectType = 0;
    iWasInWater = false;
    buttonReleased = false;
//    legAniName = NULL;
//    isTowardLeft = true;
}
void Droppable::init(int unit, int eng, float extraSpd, const char* sptName)
{
//    this->autorelease();
    this->Sprite::init();
    unitType = unit;
    SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(sptName);
    this->setDisplayFrame(frame);
    this->getTexture()->setAliasTexParameters();
    this->energy = eng;
    this->maxEnergy = eng;
    this->setTag(unit);
    this->extraSpeed = extraSpd;
    this->weight = 1;
    this->jumpAllowed = false;
    this->secondTag = -1;
}
void Droppable::onDead(){
    
}
void Droppable::onHit(){
    
}
void Droppable::showTalk(std::string text, float time, float y, float delay){
    talkBalloonY = y;
    if (imgTalkBalloon != nullptr) {
        imgTalkBalloon->removeFromParentAndCleanup(true);
    }
    imgTalkBalloon = ImageView::create("talkBalloonEmpty.png");
    imgTalkBalloon->setAnchorPoint(Point(0.5, 0));
    imgTalkBalloon->setScale9Enabled(true);
    imgTalkBalloon->setCapInsets(Rect(12, 4, 1, 4));
    imgTalkBalloon->setOpacity(0);
    imgTalkBalloon->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0), NULL));
    
    lblTalk = Label::createWithTTF(text, GameManager::getInstance()->getFont(FONT_VISITOR), 36);
    imgTalkBalloon->addChild(lblTalk);
    lblTalk->setColor(Color3B(104, 85, 84));
    lblTalk->setScale(0.3);
    lblTalk->setOpacity(0);
    lblTalk->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0), NULL));
    float gap = 3;
    lblTalk->setPosition(Point(lblTalk->getBoundingBox().size.width/2 + gap, imgTalkBalloon->getBoundingBox().size.height/2 + 1.5));
    float width = lblTalk->getBoundingBox().size.width + gap*2;
    if (width < imgTalkBalloon->getContentSize().width) {
        width = imgTalkBalloon->getContentSize().width;
    }
    imgTalkBalloon->setContentSize(Size(width, imgTalkBalloon->getContentSize().height));
    
    GameManager::getInstance()->getCurrentStageLayer()->addChild(imgTalkBalloon, 10000);
    talkVisibleTime = time + delay;
    talkVisibleTimeElapsed = time + delay;
    this->schedule(schedule_selector(Droppable::updateTalkBalloon), 0.07f, CC_REPEAT_FOREVER, delay);
}
void Droppable::updateTalkBalloon(float dt){
    talkVisibleTimeElapsed -= dt;
    if (talkVisibleTimeElapsed < 0) {
        int opacity = imgTalkBalloon->getOpacity();
        opacity -= 10;
        imgTalkBalloon->setOpacity(opacity);
        lblTalk->setOpacity(opacity);
        if (opacity <= 0) {
            imgTalkBalloon->removeFromParentAndCleanup(true);
            imgTalkBalloon = nullptr;
            this->unschedule(schedule_selector(Droppable::updateTalkBalloon));
            return;
        }
    }
    imgTalkBalloon->setPosition(this->getPosition() + Point(0, talkBalloonY));
}
void Droppable::dontHurt(){
    isHurting = false;
}
float Droppable::getCurrentY(){
    return getPositionY();
}
Droppable* Droppable::createDroppable(int unit, int eng, float extraSpd, const char* sptName)
{
    Droppable* drop = new Droppable();
    drop->init(unit, eng, extraSpd, sptName);
    return drop;
}

Droppable* Droppable::createDroppable(int unit, int eng, float extraSpd, const char* sptName, const char* animationName)
{
    return Droppable::createDroppable(unit, eng, extraSpd, sptName, animationName, false);
}

Droppable* Droppable::createDroppable(int unit, int eng, float extraSpd, const char* sptName, const char* animationName, bool repeat)
{
    Droppable* drop = Droppable::createDroppable(unit, eng, extraSpd, sptName);
    
    drop->runAnimation(animationName, repeat);
    return drop;
}

void Droppable::setFlippedX(bool flip){
    Sprite::setFlippedX(flip);
    isTowardLeft = flip;
}
void Droppable::encounteredCliff(int direction){}
void Droppable::encounteredWall(){}
void Droppable::onLanded(){
    onGround = true;
    isDropping = false;
    tumbled = false;
    jumped = false;
    this->stopActionByTag(ACTION_TAG_ROTATION);
    setRotation(0);
    if (!inWater) {
        iWasInWater = false;
    }
}
void Droppable::updatePosition(float dt)
{
    Point gravity = Point(0.0, inWater?-waterAccelYN:(-850.0 * weight));
    bool toLeft = wantsToStop?false:wantsToGoLeft;
    bool toRight = wantsToStop?false:wantsToGoRight;
    
    if (onGround) {
        exitingVehicle = false;
        jumped = false;
        if (state == STATE_JUMPING) {
            state = STATE_STANDING;
            onLanded();
            return;
        }
    }
    
    if (wantsToGoLeft && !isTowardLeft && !wantsToStop) {
        setFlippedX(true);
    }else if(wantsToGoRight && isTowardLeft && !wantsToStop){
        setFlippedX(false);
    }
    
    Point gravityStep = gravity * dt;
    
    if (!wantsToJump) {
        buttonReleased = true;
    }
    
    Point vlct = this->velocity + gravityStep;
    
    if (this->freezed) {
        vlct = Point(inWater?vlct.x*0.9:vlct.x * (freezed?0.95:0.30), vlct.y);
    }else{
        if (wantsToJump && buttonReleased){
            if(inWater || (!jumped && (onGround || jumpAllowed))) {
                Point jumpForce = Point(0.0, inWater?(onGround?waterAccelYP+10:waterAccelYP):1850.0);
                vlct = vlct + jumpForce;
                jumped = true;
                onGround = false;
                wantsToJump = false;
                buttonReleased = false;
                onJumped();
                if (currentPlatform) {
                    currentPlatform->player = NULL;
                    currentPlatform = NULL;
                }
            }else if (doubleJump && !tumbled && (jumped || !onGround) && !iWasInWater) {
                Point jumpForce = Point(0.0, inWater?(onGround?waterAccelYP+60:waterAccelYP):1850.0);
                vlct = Point(vlct.x, 230);
                tumbled = true;
                wantsToJump = false;
                buttonReleased = false;
                if (currentPlatform) {
                    currentPlatform->player = NULL;
                    currentPlatform = NULL;
                }
                tumbleAnimation();
            }/*else if(tumbled && !inWater && !isDropping){
                Point dropForce = Point(0.0, -2850.0);
                vlct = vlct += dropForce;
                buttonReleased = false;
                this->stopAllActions();
                this->setRotation(0);
                onDropping();
            }*/
        }

        if (wantsToStop){// && !wantsToGoLeft && !wantsToGoRight) {
            vlct = Point(0, vlct.y);//Point(inWater?vlct.x*0.9:vlct.x * (freezed?0.95:0.30), vlct.y); //2
        }else{
            if (toLeft) {
                Point backwardMove = Point(inWater?-waterAccelX:-2800.0, 0.0);
                Point backwardStep = backwardMove * dt; //1
                vlct = vlct + backwardStep;
            }else if(toRight){
                Point forwardMove = Point(inWater?waterAccelX:2800.0, 0.0);
                Point forwardStep = forwardMove * dt; //1
                vlct = vlct + forwardStep;
            }
        }
    }
//    float jumpCutoff = 150.0;
//    CCLOG("On ground: %d", onGround);
    
    Point minMovement = Point(inWater?-waterMaxX:-50.0-extraSpeed, inWater?-waterMinY:-280.0 - extraJump);
    Point maxMovement = Point(inWater?waterMaxX:50.0+extraSpeed, 280.0 + extraJump);

    // Clamp allows you limit a hero's speed while Moving
    vlct = vlct.getClampPoint(minMovement, maxMovement);
    Point stepVelocity = vlct * dt;
    //this->setPosition(ccpAdd(getPosition(), stepVelocity));
    desiredPosition = getPosition() + stepVelocity;
    this->velocity = vlct;
}
void Droppable::freezeForAWhile(float dur){
    freezed = true;
    this->runAction(Sequence::create(DelayTime::create(dur), CallFunc::create(CC_CALLBACK_0(Droppable::releaseFreezing, this)), nullptr));
}
void Droppable::releaseFreezing(){
    freezed = false;
    if (fixAngleToTargetWhenReleaseFreezing) {
        movingAngle = -(-GameManager::getInstance()->getAngle(this->getPosition(), this->target->getPosition() + Point(0, TILE_SIZE/2)) + 180);
    }
}
void Droppable::onDropping(){
    isDropping = true;
}
void Droppable::banJump(float dt){
    jumpAllowed = false;
}
void Droppable::setOnGround(bool ground){
    if (onGround && !ground) {
        jumpAllowed = true;
        this->scheduleOnce(schedule_selector(Droppable::banJump), 0.06f);
    }
    onGround = ground;
    
    
}
void Droppable::tumbleAnimation()
{
    state = STATE_JUMPING;
//    Action* action = RotateBy::create(0.5, isTowardLeft?-360:360);
//    action->setTag(ACTION_TAG_ROTATION);
//    this->runAction(action);
}

void Droppable::onJumped(){
    
}
void Droppable::updatePositionForStraight(float dt)
{
    Point vlct;
    float baseSpeed = 50;
    
    vlct = Point((baseSpeed+extraSpeed)*cos(movingAngle*3.14/180),
                 (baseSpeed+extraSpeed)*sin(movingAngle*3.14/180));
    
    Point minMovement = Point(-baseSpeed-extraSpeed, -280.0);
    Point maxMovement = Point(baseSpeed+extraSpeed, 280.0);
    
    // Clamp allows you limit a hero's speed while Moving
    vlct = vlct.getClampPoint(minMovement, maxMovement);
    Point stepVelocity = vlct * dt;

    //this->setPosition(ccpAdd(getPosition(), stepVelocity));
    if (freezed) {
        stepVelocity = Point::ZERO;
    }
    desiredPosition = getPosition() + stepVelocity;
    this->velocity = vlct;
}

void Droppable::updatePositionToFlyToFollowTarget(float dt)
{
//    baseSpeed+extraSpeed
    Point tPos = target->getPosition();
    Point pos = getPosition();
    float diff = sqrtf((tPos.x - pos.x)*(tPos.x-pos.x)+(tPos.y - pos.y)*(tPos.y - pos.y));
    float toMove = dt*50 + dt*extraSpeed;
    
    float toX = (tPos.x - pos.x)*(toMove/diff);
    float toY = (tPos.y - pos.y)*(toMove/diff);
    desiredPosition = Point(pos.x + toX, pos.y + toY);
    velocity = Point(0, this->getPositionY() >= desiredPosition.y?-1:1);
}

void Droppable::updatePositionForBouncing(float dt)
{
    Point vlct;
    float baseSpeed = 50;
    
    Point gravity = Point(0.0, -850.0);
    if (onGround) {
        gravity = Point(0.0, 0);
        if (state == STATE_JUMPING) {
            state = STATE_STANDING;
        }
    }
    Point gravityStep = gravity * dt;
    
    vlct = this->velocity + gravityStep;
    
    Point minMovement = Point(-baseSpeed-extraSpeed, -280.0);
    Point maxMovement = Point(baseSpeed+extraSpeed, 280.0);
    
    vlct = vlct.getClampPoint(minMovement, maxMovement);
    Point stepVelocity = vlct * dt;
    
    desiredPosition = getPosition() + stepVelocity;
    this->velocity = vlct;
}

Rect Droppable::collectBoundingBox()
{
    return Rect(desiredPosition.x - TILE_SIZE/2, desiredPosition.y - getContentSize().height/2, TILE_SIZE, TILE_SIZE*2);
}
Rect Droppable::collisionBoundingBox()
{
    float width = getContentSize().width/2;
    float height = getContentSize().height*getScaleY();
    if (width <= TILE_SIZE/2) {
        width = TILE_SIZE/2;
    }
    if (height <= 0) {
        height = TILE_SIZE/2;
    }
//    return Rect(desiredPosition.x - TILE_SIZE/2, desiredPosition.y - getContentSize().height/2, width, height);//TILE_SIZE, TILE_SIZE);
    return Rect(desiredPosition.x - width/2, desiredPosition.y - height/2, width, height);
    Rect bounding = boundingBox();
   
    if (bounding.size.width < TILE_SIZE*2 && bounding.size.height < TILE_SIZE*2) {
        Rect collisionBox = RectInset(bounding, 1, 0);
        Point diff = desiredPosition - getPosition();
        Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
        return returnBoundingBox;
    }else{
        Point pos = getPosition();
        Rect rect = Rect(pos.x - TILE_SIZE/2, pos.y - getContentSize().height/2, TILE_SIZE, getContentSize().height);
        Rect collisionBox = RectInset(rect, 0, 0);
        Point diff = desiredPosition - getPosition();
        Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
        return returnBoundingBox;
    }
}
Rect Droppable::damageBoundingBox()
{
    Rect bounding = getBoundingBox();
    Rect collisionBox = RectInset(bounding, 3, 3);
//    Point diff = 0;//desiredPosition - getPosition();
//    Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
//    return returnBoundingBox;
    return collisionBox;
}
void Droppable::goLeft(){
    wantsToGoLeft = true;
    wantsToGoRight = false;
    wantsToStop = false;
}
void Droppable::stop(){
    wantsToGoLeft = false;
    wantsToGoRight = false;
    wantsToStop = true;
}
void Droppable::goRight(){
    wantsToGoLeft = false;
    wantsToGoRight = true;
    wantsToStop = false;
}
Point Droppable::getGravityPosition()
{
    return Point(getPosition().x, getPosition().y-getContentSize().height/2 + TILE_SIZE/2);
}

/*Rect Droppable::currentBoundingBox()
{
    Rect bounding = boundingBox();
    
    if (vehicleType == VEHICLE_GOLIATH) {
        
        Point pos = getPosition();
        Rect collisionBox = Rect(pos.x - TILE_SIZE, pos.y - 28, TILE_SIZE*2, TILE_SIZE*2);
        return collisionBox;
    }
    
    if (bounding.size.width < TILE_SIZE*2 && bounding.size.height < TILE_SIZE*2) {
        Rect collisionBox = RectInset(bounding, 0, 0);
//        Point diff = ccpSub(desiredPosition, getPosition());
//        Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
        return collisionBox;
    }else{
        Point pos = getPosition();
        Rect rect = Rect(pos.x - TILE_SIZE/2, pos.y - TILE_SIZE + 1, TILE_SIZE, TILE_SIZE*2 - 2);
        Rect collisionBox = RectInset(rect, 0, 0);
//        Point diff = ccpSub(desiredPosition, getPosition());
//        Rect returnBoundingBox = RectOffset(collisionBox, diff.x, diff.y);
        return collisionBox;
    }
}*/

Rect Droppable::RectOffset(Rect rect, float x, float y)
{
    return Rect(rect.origin.x + x, rect.origin.y + y, rect.size.width, rect.size.height);
}

Rect Droppable::RectInset(Rect rect, float x, float y)
{
    return Rect(rect.origin.x + x, rect.origin.y + y, rect.size.width - x*2, rect.size.height - y*2);
}

/*void Droppable::setOrder(int order)
{
    orderAction = order;
    if (currentAction != orderAction) {
        if (orderAction == ENEMY_ACTION_MOVE_TO_LEFT) {
            isTowardLeft = true;
            this->setFlippedX(isTowardLeft);
            
            if(unitType == UNIT_SNAIL_BALL){
                this->stopAllActions();
                this->runAction(RepeatForever::create(RotateBy::create(1, isTowardLeft?-360:360)));
            }
        }else if(orderAction == ENEMY_ACTION_MOVE_TO_RIGHT){
            isTowardLeft = false;
            this->setFlippedX(isTowardLeft);
            
            if(unitType == UNIT_SNAIL_BALL){
                this->stopAllActions();
                this->runAction(RepeatForever::create(RotateBy::create(1, isTowardLeft?-360:360)));
            }
        }
        
        switch (unitType) {
            case UNIT_LIZARD_MAN:
                //this->runAnimation("lizardMan");
                break;
                
            default:
                break;
        }
        currentAction = orderAction;
    }
}
*/
void Droppable::setTarget(Droppable* t)
{
    target = t;
}
float Droppable::findTarget(Droppable* tar, float dist)
{
    Point pos = this->getPosition();
    Point tPos = tar->getPosition();
    float distance = sqrtf((pos.x - tPos.x)*(pos.x - tPos.x) + (pos.y - tPos.y)*(pos.y - tPos.y));
    
    if (distance < dist){
        if(!isTargetFound)
            isTargetFound = true;
    }
//    else{
//        if(isTargetFound)
//            isTargetFound = false;
//    }
    return distance;
}

void Droppable::runAnimationLater(float dt){
    runAnimation(animationName, isAnimationRepeat);
}
void Droppable::runAnimation(const char* name, bool repeat, float delay){
    animationName = name;
    isAnimationRepeat = repeat;
    this->scheduleOnce(schedule_selector(Droppable::runAnimationLater), delay);
}
void Droppable::runAnimation(const char* name, bool repeat)
{
    this->stopActionByTag(ACTION_TAG_ANIMATION);
    Animation* animation = AnimationCache::getInstance()->getAnimation(name);
    Animate* animate = Animate::create(animation);
    if(repeat)
    {
        RepeatForever* forever = RepeatForever::create(animate);
        forever->setTag(ACTION_TAG_ANIMATION);
        this->runAction(forever);
    }
    else
    {
        animate->setTag(ACTION_TAG_ANIMATION);
        this->runAction(animate);
    }
}

void Droppable::fireAnimationDone(Node* nd)
{
    
}

void Droppable::jumpAnimation()
{
//    isStopped = false;
    state = STATE_JUMPING;
    
}

void Droppable::blinkForAWhile()
{
    if (blinkingTime <= 0) {
        blinkingTime = 0.6f;
        this->schedule(schedule_selector(Droppable::blinking), 0.1f);
    }else{
        blinkingTime = 0.6f;
    }
}

void Droppable::blinking(float dt)
{
    if (this->getColor().g == 115) {
        this->setColor(Color3B(255, 255, 255));
    }else{
        this->setColor(Color3B(255, 115, 115));
    }
    
    blinkingTime -= dt;
    if (blinkingTime < 0) {
        this->unschedule(schedule_selector(Droppable::blinking));
        this->setColor(Color3B(255, 255, 255));
    }
}

void Droppable::setInWater(bool inOrOut)
{
    inWater = inOrOut;
    if (inWater) {
        iWasInWater = true;
    }
}

void Droppable::runSchedule(){
//    CCLOG("Droppable runSchedule");
}

