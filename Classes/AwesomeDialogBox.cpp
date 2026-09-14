#include "AwesomeDialogBox.h"
//#include "../ExtensionsTest.h"
//#include "CustomTableViewCell.h"
//#include "ExtensionMacros.h"
#include "GameManager.h"
#include "LanguageManager.h"
using namespace cocos2d;


// on "init" you need to initialize your instance
bool AwesomeDialogBox::init()
{
    bool bRet = false;
    do 
    {
        CC_BREAK_IF(! Layer::init());
		// in init()
		// Initialize arrays
		isWaiting = false;
        isInitialized = false;
        size = Director::getInstance()->getWinSize();
        touchCount = 0;
        _background = DrawNode::create();
        _background->setContentSize(Size(size.width, size.height/4));
        _background->drawSolidRect(Point(0, 0), Point(size.width, size.height/4), Color4F(1, 1, 1, 0.2));
        _background->setPosition(Point::ZERO);
		addChild(_background);
        
        Button* btn = Button::create("whiteRect.png");
        this->addChild(btn);
        btn->setPosition(size/2);
        btn->setScale(size.width/btn->getContentSize().width, size.height/btn->getContentSize().height);
        btn->addClickEventListener(CC_CALLBACK_0(AwesomeDialogBox::onClick, this));
        btn->setOpacity(0);
        
//        labelPoint  = Point(-_background->getContentSize().width*_background->getScale()/2 + 60, _background->getContentSize().height*_background->getScale()/4);

        _labelTop = this->getLabel("        ");
        _labelTop->enableOutline(Color4B::BLACK, 3);
        this->addChild(_labelTop, 1);

        _blinkLabel = getLabel(LanguageManager::getInstance()->getText("continue").c_str());
        _blinkLabel->setWidth(-1);
        _blinkLabel->setAnchorPoint(Point(1, 0));
        _blinkLabel->enableOutline(Color4B::BLACK, 3);
        this->addChild(_blinkLabel, 1);
        
        Label* lblSkip = getLabel("SKIP (Y)");
        lblSkip->setAnchorPoint(Point(1, 0));
        lblSkip->setWidth(-1);
        lblSkip->setPosition(Point(size.width, 0) + Point(-2, 2));
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        lblSkip->setVisible(false);
//#endif
        this->addChild(lblSkip, 1);

        _leftSide = NULL;
        _rightSide = NULL;

//        listener = cocos2d::EventListenerTouchOneByOne::create();
//        listener->setSwallowTouches(false);
        
//        listener->onTouchBegan = CC_CALLBACK_2(AwesomeDialogBox::onTouchBegan, this);
//        listener->onTouchMoved = CC_CALLBACK_2(AwesomeDialogBox::onTouchMoved, this);
//        listener->onTouchEnded = CC_CALLBACK_2(AwesomeDialogBox::onTouchEnded, this);
        
//        _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
        bRet = true;
    } while (0);
	
		
    return bRet;
}
void AwesomeDialogBox::removeUsedAssets(){
//    this->removeFromParent();
//    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
}
Label* AwesomeDialogBox::getLabel(const char* txt)
{
    Label* label = LanguageManager::getInstance()->getLocalizedLabel(txt, Color4B(235, 235, 235, 255));//Label::createWithTTF(txt, GameManager::getInstance()->getFont(FONT_DEFAULT), 30);
    label->setAnchorPoint(Point(0, 0.5));
    label->setWidth(size.width - 140);
    return label;
}

// cpp with cocos2d-x
void AwesomeDialogBox::update(float dt)
{
//	_charIndex++;
	if(_currentTalk.size() <= _charIndex)
	{
        _labelTop->setString(_currentTalk.c_str());
        _isShowing = false;
        _blinkLabel->setVisible(true);
		this->schedule(schedule_selector(AwesomeDialogBox::blink), 0.5);
		this->unschedule(schedule_selector(AwesomeDialogBox::update));
	}
	else
	{
//        char currentCh = _currentTalk[_charIndex];
        char nextCh = _currentTalk[_charIndex];
        int nextIndex = _charIndex;
        if ((int)nextCh > 0 && (int)nextCh < 128) {
            _charIndex = _charIndex + 1;
        }else{
            _charIndex = _charIndex + 3;
        }
        if (_charIndex > _currentTalk.length()) {
            _charIndex = _currentTalk.length();
        }
//        CCLOG("nextIndex: %d, text: %s", nextIndex, _currentTalk.substr(0, nextIndex).c_str());
        
//        Label* lbl = this->getLabel(_currentTalk.substr(0, nextIndex).c_str());
//        this->addChild(lbl, 1);
//        this->removeChild(_label, true);
//        _label = lbl;
//        _label->retain();
        _labelTop->setString(_currentTalk.substr(0, nextIndex).c_str());
		
//		CCLOG(_currentTalk.substr(0,_charIndex + 1).c_str());
        if (_currentTalk.substr(_charIndex,1) != " ") {
            GameManager::getInstance()->playSoundEffect(SOUND_MACHINE_GUN_1);
        }
		
	}
}

void AwesomeDialogBox::blink(float dt)
{
	_blinkLabel->setVisible(!_blinkLabel->isVisible());
}

void AwesomeDialogBox::ShowDialog(const char* talks, Layer* layer, FiniteTimeAction* call, Sprite* spt, float scale)
{
	_talkIndex = -1;
	_charIndex = -1;
	_isShowing = false;
	_talkVec = split(talks, '|');
//    _talkVec.push_back(talks);
	_callingLayer = layer;
	_callback = call;
    isInitialized = true;
//	this->setTouchEnabled(true);
    _leftSide = spt;
    if(_leftSide){
        this->addChild(_leftSide);
        _leftSide->setAnchorPoint(Vec2::ZERO);
        _leftSide->setPosition(Vec2::ZERO);
        _leftSide->setScale(scale);
    }
	this->showNextTalk();
}

void AwesomeDialogBox::showNextTalk()
{
	if(_isShowing)
	{
		_charIndex = _currentTalk.size() - 1;
	}
	else
	{
		_talkIndex++;

		if(_talkVec.size() <= _talkIndex)
		{
//			_label->setString("");
			_blinkLabel->setVisible(false);
//			this->setTouchEnabled(false);
			this->unschedule(schedule_selector(AwesomeDialogBox::blink));
            if (_callback != nullptr) {
                removeUsedAssets();
                _callingLayer->runAction(_callback);
            }
		}
		else
		{
			_isShowing = true;
            
			_charIndex = 0;
			_currentTalk = _talkVec[_talkIndex];
            if(_rightSide)
            {
                //					removeChild(_rightSide, false);
            }
//            if(_leftSide)
//            {
//                removeChild(_leftSide, false);
//            }
            
//            _leftSide->setScaleX(-1);
            
            
//            if (_label) {
//                _label->removeFromParentAndCleanup(true);
//            }
            float extraX = 0;
            if (_leftSide) {
//                _leftSide->setPosition(Point(-_background->getContentSize().width/2 + 100,0));
                extraX = _leftSide->getContentSize().width*_leftSide->getScaleX() + 10;
            }
            _labelTop->setPosition(Point(20 + extraX, size.height/8));
            _labelTop->setWidth(size.width - _labelTop->getPositionX() - 40);
            //				addChild(_leftSide);
            
            _blinkLabel->setPosition(Point(size.width - 20, 20));
//            _talkIndex++;
            _currentTalk = _talkVec[_talkIndex];
            _blinkLabel->setVisible(false);
			this->unschedule(schedule_selector(AwesomeDialogBox::blink));
			this->schedule(schedule_selector(AwesomeDialogBox::update), 0.07f);
		}
	}
}

std::vector<std::string> &AwesomeDialogBox::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> AwesomeDialogBox::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}
//bool AwesomeDialogBox::onTouchBegan(Touch *touch, Event *unused_event){
//    Point location = touch->getLocationInView();
//    location = Director::getInstance()->convertToGL(location);
//  
//    touchCount++;
//    return true;
//}
//
//void AwesomeDialogBox::onTouchMoved(Touch *touch, Event *unused_event){
//    Point location = touch->getLocationInView();
//    location = Director::getInstance()->convertToGL(location);
//   
//}
//void AwesomeDialogBox::onTouchEnded(Touch *touch, Event *unused_event){
//    touchCount--;
//    if(!isWaiting && isInitialized && touchCount <= 0){
//        this->showNextTalk();
//    }
//}
void AwesomeDialogBox::onClick(){
    showNextTalk();
}
