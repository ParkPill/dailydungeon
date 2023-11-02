//
//  Firework.cpp
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 29..
//
//

#include "Firework.h"
#include "GameManager.h"
#include "HelloWorldScene.h"
#include "MyMessageBox.h"
//#include "GoogleBilling.h"
#include "LanguageManager.h"
#include "SimpleAudioEngine.h"
using namespace cocos2d;

using namespace cocos2d::ui;
using namespace cocostudio;
bool Firework::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("firework.plist");

    isRandomColor = true;
    fireScale = 1;
	nextFire = 0;
	size = Director::getInstance()->getVisibleSize();
	startPoint = Point(size.width/2, 0);
	interval = 1;
	intervalVarious = 0.5f;
	totalTime = 0;
	power = 50;
	powerVarious = 15;
	spriteBatch = SpriteBatchNode::create("firework.png");
	this->addChild(spriteBatch, 1);
    sptName = "whiteCircle.png";
	this->schedule(schedule_selector(Firework::fireworkSchedule), 0.1f);

	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("firework0.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("firework1.mp3");

    return true;
}

Firework* Firework::create(const char* spriteName, float fireScale, bool isRandomColor){
    Firework* firework = Firework::create();
    firework->Layer::init();
    
    firework->nextFire = 0;
    firework->size = Director::getInstance()->getVisibleSize();
    firework->startPoint = Point(firework->size.width/2, 0);
    firework->interval = 1;
    firework->intervalVarious = 0.5f;
    firework->totalTime = 0;
    firework->power = 50;
    firework->powerVarious = 15;
    firework->spriteBatch = SpriteBatchNode::create("firework.png");
    firework->addChild(firework->spriteBatch, 1);
    firework->isRandomColor = isRandomColor;
    firework->fireScale = fireScale;
    
    firework->schedule(schedule_selector(Firework::fireworkSchedule), 0.1f);
    firework->sptName = spriteName;
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("firework0.mp3");
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("firework1.mp3");

    return firework;
}
Firework* Firework::createOnce(int fireworkCount){
    Firework* firework = Firework::create();
    for(int i = 0; i < fireworkCount; i++){
        firework->totalTime = 0;
        firework->fireworkSchedule(0);
    }
    firework->unschedule(schedule_selector(Firework::fireworkSchedule));
    return firework;
}

void Firework::fireworkSchedule(float dt){
	totalTime -= dt;
	if (totalTime > 0)
	{
		return;
	}
	totalTime = interval + (cocos2d::random() % 11)*0.1f * 2 * intervalVarious - intervalVarious;

	//fire
	FireRocket* rocket = FireRocket::create(sptName);
	rocket->setPosition(startPoint);
	rocket->setTimer(0.9f);
    if (isRandomColor) {
        rocket->setColor(GM->getRandomColor());
    }
    rocket->setScale(fireScale);
	int degree = 80 + cocos2d::random() % 20;
	double angle = degree*3.14f / 180;
	float shootPower = power + cocos2d::random() % (powerVarious * 2) - powerVarious;
	rocket->velocity = Point(cos(angle)*shootPower, sin(angle)*shootPower);
	spriteBatch->addChild(rocket);
}

void Firework::spriteMoveDone(Node* node){
	node->removeFromParentAndCleanup(true);
}
FireParticle::FireParticle(){
	velocity = Point::ZERO;
	weight = 0.03f;
}
FireParticle* FireParticle::create(const char* name){
	FireParticle* fire = new FireParticle();
	fire->initFire(name);
	return fire;
}
void FireParticle::initFire(const char* name){
    sptName = name;
	Sprite::initWithSpriteFrameName(name);
//    setBlendFunc({GL_ONE, GL_DST_ALPHA});
    setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
    setOpacity(200);
	schedule(schedule_selector(FireParticle::updatePosition));
}
void FireParticle::spriteMoveDone(Node* node){
	node->removeFromParentAndCleanup(true);
}

void FireParticle::updatePosition(float dt){
	Point gravity = Point(0.0, -850.0 * weight);
	Point gravityStep = gravity * dt;

	this->velocity += gravityStep;
	this->velocity *= 0.96f;
	Point minMovement = Point(-50.0, -280.0);
	Point maxMovement = Point(50.0, 280.0);

	this->velocity = this->velocity.getClampPoint(minMovement, maxMovement);

	this->setPosition(this->getPosition() + this->velocity);
}
void FireMass::initFireMass(){
	//schedule(schedule_selector(FireMass::creatingParticle), 0.1f, 5, 0.01f);
	schedule(schedule_selector(FireMass::creatingParticle), 0.1f);
}
FireMass* FireMass::create(const char* name){
	FireMass* rocket = new FireMass();
	rocket->initFire(name);
	rocket->initFireMass();
	return rocket;
}
void FireMass::creatingParticle(float dt){
	FireParticle* sptParticle = FireParticle::create(sptName);
	this->getParent()->addChild(sptParticle);
	sptParticle->setColor(this->getColor());
	sptParticle->setPosition(this->getPosition());//getContentSize() / 2);
	sptParticle->setScale(this->getScale()*0.7f);
	sptParticle->runAction(Sequence::create(DelayTime::create(0.2f), FadeOut::create(0.2f), NULL));
	double angle = atan2(-velocity.y, -velocity.x);
	double degree = angle * 180 / 3.14;
	degree += cocos2d::random() % 40 - 20;
	float power = sqrt(velocity.x*velocity.x + velocity.y*velocity.y)*8;
	double newAngle = degree*3.14 / 180;
	Point target = Point(cos(newAngle)*power, sin(newAngle)*power);
	sptParticle->runAction(ScaleBy::create(0.5f, 0.2f));
	sptParticle->runAction(Sequence::create(MoveBy::create(0.5f, target), CallFuncN::create(CC_CALLBACK_1(FireMass::spriteMoveDone, this)), NULL));
}
void FireMass::spriteMoveDone(Node* node){
	node->removeFromParentAndCleanup(true);
}

FireRocket::FireRocket(){
	explodePower = 12;
	explodePowerVarious = 6;
	particleCount = 20;
	particleCountVarious = 5;
	minColor = 220;
}

FireRocket* FireRocket::create(const char* name){
	FireRocket* rocket = new FireRocket();
	rocket->initFire(name);
	rocket->initFireMass();
	return rocket;
}

void FireRocket::setTimer(float time){
	this->runAction(Sequence::create(DelayTime::create(time*0.7f), FadeOut::create(time*0.3f), NULL));
	this->scheduleOnce(schedule_selector(FireRocket::explodeFire), time);
}
void FireRocket::explodeFire(float dt){
	int fireCount = particleCount + cocos2d::random() % (particleCountVarious * 2) - particleCountVarious;
	FireMass* fire;
	float power;
	bool isSphere = false;// cocos2d::random() % 2 == 0;
	if (isSphere)
		fireCount *= 3;
	const char* soundName;
	if (cocos2d::random() % 2 == 0){
		soundName = "firework0.mp3";
	}
	else{
		soundName = "firework1.mp3";
	}
	power = explodePower + cocos2d::random() % (explodePowerVarious * 2) - explodePowerVarious;
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(soundName);
	for (int i = 0; i < fireCount; i++){
		fire = FireMass::create(sptName);
		this->getParent()->addChild(fire);
		fire->setScale(this->getScale() *2/ 3);
		fire->setColor(this->getColor());
		fire->setPosition(this->getPosition());
		fire->weight = 0.01f;
		int degree = 360 * i / fireCount;
		double angle = degree*3.14f / 180;
		if (isSphere){
			power = explodePower + cocos2d::random() % (explodePowerVarious * 2) - explodePowerVarious;
			fire->velocity = Point(cos(angle)*(cocos2d::random() % (int)power), sin(angle)*(cocos2d::random() % (int)power));
		}
		else{
			fire->velocity = Point(cos(angle)*power, sin(angle)*power);
		}
		fire->runAction(Sequence::create(DelayTime::create(0.8f), FadeOut::create(0.3f), CallFuncN::create(CC_CALLBACK_1(FireMass::spriteMoveDone, fire)), NULL));
	}

	this->removeFromParentAndCleanup(true);
}
