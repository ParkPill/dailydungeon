//
//  MyMessageBox.m
//  Doodle Girls
//
//  Created by SUNG PILL PACK on 12. 7. 25..
//  Copyright (c) 2012년 rightpill@me.com. All rights reserved.
//

#include "MyMessageBox.h"
#include "GameManager.h"
#include "LanguageManager.h"
using namespace cocos2d;
MyMessageBox* MyMessageBox::m_mySingleton = NULL;

MyMessageBox::MyMessageBox()
{

}
MyMessageBox* MyMessageBox::getInstance()
{
    if(NULL == m_mySingleton)
    {
        m_mySingleton = MyMessageBox::create();//new MyMessageBox();
        m_mySingleton->retain();
    }
    
    return m_mySingleton;
}


bool MyMessageBox::init(){
    bool bRet = false;
    do
    {
        CC_BREAK_IF(! Layer::init());
        fontName = "legendary.fnt";
        size = CCDirector::getInstance()->getWinSize();
        back = Sprite::create("inventoryInformationBackground.png");
        back->setPosition( Point(size.width/2, size.height/2));
        this->addChild(back);
        messageBox = NULL;
        this->setTouchEnabled(true);
        tar = NULL;
        closeCall = NULL;
        const char* fontName = GameManager::getInstance()->getFont(FONT_DEFAULT);
        CCLOG("fontName: %s", fontName);
        messageLabel = Label::createWithTTF("그러니까 지금 로그아웃 하시려는 겁니까?? 진정?!?!", fontName, 25);
        messageLabel->setPosition( Point(size.width/2, size.height/2 + 20));
        this->addChild(messageLabel);
//        messageLabel->getTexture()->setAliasTexParameters();
        
        
        MenuItemImage* item = MenuItemImage::create("backButtonBase.png", "backButtonBase.png", CC_CALLBACK_1(MyMessageBox::result, this));
        item->setTag( MESSAGE_BOX_RESULT_BUTTON_1);
        btnMenu1 = Menu::create(item, NULL);
        Point position = Point(size.width/2 + 130, size.height/2 - 135);;
        btnMenu1->setPosition( position);
        this->addChild(btnMenu1);
        
        
//        btnLabel1 = Label::createWithTTF("alskfjasldfjads;lkfj", 40, 4, Color3B::WHITE, Color3B(241, 74, 1), FONT_BIT_DUST);
        btnLabel1 = Label::createWithTTF("alskfjasldfjads;lkfj", GameManager::getInstance()->getFont(FONT_DEFAULT), 40);
        btnLabel1->setPosition(Point(position.x, position.y + 5));
        this->addChild(btnLabel1);
        
        item = MenuItemImage::create("closeButton.png", "closeButton.png", CC_CALLBACK_1(MyMessageBox::result, this));
        item->setTag(MESSAGE_BOX_RESULT_CANCEL);
        
        btnClose = Menu::create(item, NULL);
        position = Point(size.width/2 + back->getContentSize().width*back->getScale()/2 - 20, size.height/2 + back->getContentSize().height*back->getScale()/2 - 4);
        btnClose->setPosition( position);
        this->addChild(btnClose);
        
        item = MenuItemImage::create("equipButtonBase.png", "equipButtonBase.png", CC_CALLBACK_1(MyMessageBox::result, this));
        item->setTag( MESSAGE_BOX_RESULT_BUTTON_2);
        
        btnMenu2 = Menu::create(item, NULL);
        position = Point(size.width/2 - 140, size.height/2 - 135);
        btnMenu2->setPosition( position);
        this->addChild(btnMenu2);
        
        btnLabel2 = Label::createWithTTF("alskfjasldfjads;lkfj", GameManager::getInstance()->getFont(FONT_DEFAULT), 40);
        btnLabel2->setPosition( Point(position.x, position.y + 5));
        btnLabel2->setTextColor(Color4B(0,0,0,255));
        this->addChild(btnLabel2);
        
        item = MenuItemImage::create("backButtonBase.png", "backButtonBase.png", CC_CALLBACK_1(MyMessageBox::result, this));
        
        item->setTag( MESSAGE_BOX_RESULT_OK);
        btnMenu = Menu::create(item, NULL);
        position = Point(size.width/2, size.height/2  - 120);
        btnMenu->setPosition( position);
        this->addChild(btnMenu);
        
        btnLabel = Label::createWithTTF("alskfjasldfjads;lkfj", GameManager::getInstance()->getFont(FONT_DEFAULT), 40);
        btnLabel->setPosition( Point(position.x, position.y -6));
        btnLabel->setTextColor(Color4B(0,0,0,255));
        this->addChild(btnLabel);
        
        bRet = true;
    } while (0);
	
    return bRet;
}

bool MyMessageBox::onTouchBegan(Touch *touch, Event *unused_event){
    CCLOG("MessageBox touch began.");
    return true;
}
void MyMessageBox::onTouchMoved(Touch *touch, Event *unused_event){
    
}
void MyMessageBox::onTouchEnded(Touch *touch, Event *unused_event){
    
}

void MyMessageBox::showDialog(Node* target, CallFuncN* call, const char* msg, const char* btn1, const char* btn2){
    removeUsedAsset();
//    GameManager::getInstance()->pushLayer(target, this);
    addItToParent(target);
    tar = target;
	closeCall = call;
    closeCall->retain();
    
    messageLabel->setString(msg);
    messageLabel->setVisible(true);
    
    btnLabel1->setString(btn1);
    btnLabel1->setVisible(true);
    btnMenu1->setVisible(true);
    
    btnLabel2->setString(btn2);
    btnLabel2->setVisible(true);
    btnMenu2->setVisible(true);
    
    btnClose->setVisible(false);
}

void MyMessageBox::showDialogCry(Node* target, CallFuncN* call, const char* msg, const char* btn1, const char* btn2){
    back->setTexture(Sprite::create("dialogBoxBackgroundReally.png")->getTexture());
    showDialog(target, call, msg, btn1, btn2);
}

void MyMessageBox::showDialogWithCloseBox(Node* target, CallFuncN* call, const char* msg, const char* btn1){
    removeUsedAsset();
    tar = target;
    callback = call;
    callback->autorelease();
    callback->retain();
    callback->retain();
    messageBox = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("LegendDary_UI_MessageBox.json"));
    messageBox->setPosition(Point(size.width/2 - messageBox->getContentSize().width/2, 0));
    target->addChild(messageBox, 1000);
    
    Sprite* black = Sprite::create("blackSquare.png");
    black->setScale(size.width, size.height);
    black->setPosition(Point(messageBox->getContentSize().width/2, size.height/2));
    messageBox->addChild(black, -1);
    
    Text* lbl = dynamic_cast<Text*>(Helper::seekWidgetByName(messageBox, "lblContent"));
    lbl->setFontName(GameManager::getInstance()->getFont(FONT_DEFAULT));
    lbl->setString(msg);
    lbl->setPosition(lbl->getPosition() + Point(0, -30));
    
    ImageView* spt = dynamic_cast<ImageView*>(Helper::seekWidgetByName(messageBox, "sptGem"));
    spt->setVisible(false);
    
    lbl = dynamic_cast<Text*>(Helper::seekWidgetByName(messageBox, "lblPrice"));
    lbl->setVisible(false);
    
    Button* btnYes = dynamic_cast<Button*>(Helper::seekWidgetByName(messageBox, "btnYes"));
    btnYes->addTouchEventListener(CC_CALLBACK_2(MyMessageBox::messageBoxWithCloseClosed, this));
    GameManager::getInstance()->setFontSize(btnYes->getTitleRenderer(), 30);
    btnYes->getTitleRenderer()->setString(LanguageManager::getInstance()->getText("ok"));
    btnYes->setPosition(btnYes->getPosition() + Point(100, 0));
    btnYes->setTitleFontName(GameManager::getInstance()->getFont(FONT_DEFAULT));
//    ((Label*)btnYes->getVirtualRenderer())->enableOutline(Color4B::WHITE, 3);
    
    Button* btnNo = dynamic_cast<Button*>(Helper::seekWidgetByName(messageBox, "btnNo"));
    btnNo->loadTextureNormal("closeButton.png");
    btnNo->setPosition(Point(790, 490));
    btnNo->setTitleText("");
    btnNo->addTouchEventListener(CC_CALLBACK_2(MyMessageBox::messageBoxWithCloseClosed, this));
}

void MyMessageBox::showDialog(Node* target, CallFuncN* call, const char* msg, const char* btn1){
    removeUsedAsset();
    
    addItToParent(target);
//    GameManager::getInstance()->pushLayer(target, this);
    
    tar = target;
    //tar->retain();
    //sel = selector;
 
    messageLabel->setString(msg);
//    messageLabel->setPosition( Point(size.width/2, size.height/2 + 21));
    messageLabel->setVisible(true);
    
    btnLabel->setString(btn1);
    btnLabel->setVisible(true);
    btnMenu->setVisible(true);
    
    btnClose->setVisible(false);
    
    closeCall = call;
    closeCall->retain();
}


void MyMessageBox::showDialog(Node* target, const char* msg, CallFuncN* call){
    messageBox = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("LegendDary_UI_MessageBox.json"));
    messageBox->setPosition(Point(size.width/2 - messageBox->getContentSize().width/2, 0));
    target->addChild(messageBox, 1000);
    
    Sprite* black = Sprite::create("blackSquare.png");
    black->setScale(size.width, size.height);
    black->setPosition(Point(messageBox->getContentSize().width/2, size.height/2));
    messageBox->addChild(black, -1);
    
    Text* lbl = dynamic_cast<Text*>(Helper::seekWidgetByName(messageBox, "lblContent"));
    lbl->setFontName(GameManager::getInstance()->getFont(FONT_DEFAULT));
    lbl->setString(msg);
    
    ImageView* spt = dynamic_cast<ImageView*>(Helper::seekWidgetByName(messageBox, "sptGem"));
    spt->setVisible(false);
    
    lbl = dynamic_cast<Text*>(Helper::seekWidgetByName(messageBox, "lblPrice"));
    lbl->setVisible(false);
    
    btnYes = dynamic_cast<Button*>(Helper::seekWidgetByName(messageBox, "btnYes"));
//    btnYes->addTouchEventListener(CC_CALLBACK_2(MyMessageBox::resultFromSel, this));
    btnYes->setTag(0);
    GameManager::getInstance()->setFontSize(btnYes->getTitleRenderer(), 30);
//    ((Label*)btnYes->getVirtualRenderer())->enableOutline(Color4B::WHITE, 3);
    btnYes->setTitleText(LanguageManager::getInstance()->getText("yes"));
    btnYes->setTitleFontName(GameManager::getInstance()->getFont(FONT_DEFAULT));
    
    btnNo = dynamic_cast<Button*>(Helper::seekWidgetByName(messageBox, "btnNo"));
//    btnNo->addTouchEventListener(CC_CALLBACK_2(MyMessageBox::resultFromSel, this));
    btnNo->setTag(1);
    GameManager::getInstance()->setFontSize(btnNo->getTitleRenderer(), 30);
//    ((Label*)btnNo->getVirtualRenderer())->enableOutline(Color4B::WHITE, 3);
    btnNo->setTitleText(LanguageManager::getInstance()->getText("no"));
    btnNo->setTitleFontName(GameManager::getInstance()->getFont(FONT_DEFAULT));
    
    btnNo->setUserData(call);
    
//    callback = call;
//    callback->retain();
}
void MyMessageBox::resultFromSel(Ref* sender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    messageBox->removeFromParentAndCleanup(true);
    messageBox = NULL;
    Button* btn = (Button*)sender;
//    btn->runAction(callback);
//    CallFuncN* call = (void)btn->getUserData();
//    btn->setUserData(NULL);
//    btn->runAction(call);
}
void MyMessageBox::showDialog(Node* target, const char* msg){
    if (messageBox) {
//        messageBox->removeFromParent();
        messageBox = NULL;
    }
    messageBox = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("LegendDary_UI_MessageBox.json"));
    messageBox->setPosition(Point(size.width/2 - messageBox->getContentSize().width/2, 0));
    target->addChild(messageBox, 1000);
    
    Sprite* black = Sprite::create("blackSquare.png");
    black->setScale(size.width, size.height);
    black->setPosition(Point(messageBox->getContentSize().width/2, size.height/2));
    messageBox->addChild(black, -1);
    
    Text* lbl = dynamic_cast<Text*>(Helper::seekWidgetByName(messageBox, "lblContent"));
    lbl->setFontName(GameManager::getInstance()->getFont(FONT_DEFAULT));
    lbl->setString(msg);
    lbl->setContentSize(Size(600,500));
    
    ImageView* spt = dynamic_cast<ImageView*>(Helper::seekWidgetByName(messageBox, "sptGem"));
    spt->setVisible(false);
    
    lbl = dynamic_cast<Text*>(Helper::seekWidgetByName(messageBox, "lblPrice"));
    lbl->setVisible(false);
    
    Button* btnYes = dynamic_cast<Button*>(Helper::seekWidgetByName(messageBox, "btnYes"));
    btnYes->addTouchEventListener(CC_CALLBACK_2(MyMessageBox::messageBoxClosed, this));
    GameManager::getInstance()->setFontSize(btnYes->getTitleRenderer(), 30);
    btnYes->getTitleRenderer()->setString(LanguageManager::getInstance()->getText("ok"));
//    ((Label*)btnYes->getVirtualRenderer())->enableOutline(Color4B::WHITE, 3);
    btnYes->setTitleText(LanguageManager::getInstance()->getText("yes"));
    btnYes->setTitleFontName(GameManager::getInstance()->getFont(FONT_DEFAULT));
    
    Button* btnNo = dynamic_cast<Button*>(Helper::seekWidgetByName(messageBox, "btnNo"));
    btnNo->setVisible(false);
//    ((Label*)btnNo->getVirtualRenderer())->enableOutline(Color4B::WHITE, 3);
    btnNo->setTitleText(LanguageManager::getInstance()->getText("no"));
    btnNo->setTitleFontName(GameManager::getInstance()->getFont(FONT_DEFAULT));
    
    return;
    /*removeUsedAsset();
    
    addItToParent(target);
    
    tar = target;
    closeCall = NULL;
    
    btnClose->setVisible(false);
    
    btnLabel->setString("OK");
    btnLabel->setVisible(true);
    btnMenu->setVisible(true);
    
    messageLabel->setString(msg);
    messageLabel->setVisible(true);*/
}

void MyMessageBox::showDialogWithoutButton(Node* target, const char* msg){
    messageBox = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("LegendDary_UI_MessageBox.json"));
    messageBox->setPosition(Point(size.width/2 - messageBox->getContentSize().width/2, 0));
    target->addChild(messageBox, 1000);
    
    Sprite* black = Sprite::create("blackSquare.png");
    black->setScale(size.width, size.height);
    black->setPosition(Point(messageBox->getContentSize().width/2, size.height/2));
    messageBox->addChild(black, -1);
    
    Text* lbl = dynamic_cast<Text*>(Helper::seekWidgetByName(messageBox, "lblContent"));
    lbl->setFontName(GameManager::getInstance()->getFont(FONT_DEFAULT));
    lbl->setString(msg);
    lbl->setContentSize(Size(600,500));
    
    ImageView* spt = dynamic_cast<ImageView*>(Helper::seekWidgetByName(messageBox, "sptGem"));
    spt->setVisible(false);
    
    lbl = dynamic_cast<Text*>(Helper::seekWidgetByName(messageBox, "lblPrice"));
    lbl->setVisible(false);
    
    Button* btnYes = dynamic_cast<Button*>(Helper::seekWidgetByName(messageBox, "btnYes"));
    btnYes->addTouchEventListener(CC_CALLBACK_2(MyMessageBox::messageBoxClosed, this));
    GameManager::getInstance()->setFontSize(btnYes->getTitleRenderer(), 30);
    btnYes->getTitleRenderer()->setString(LanguageManager::getInstance()->getText("ok"));
//    ((Label*)btnYes->getVirtualRenderer())->enableOutline(Color4B::WHITE, 3);
    btnYes->setVisible(false);
    btnYes->setTitleText(LanguageManager::getInstance()->getText("yes"));
    btnYes->setTitleFontName(GameManager::getInstance()->getFont(FONT_DEFAULT));
    
    Button* btnNo = dynamic_cast<Button*>(Helper::seekWidgetByName(messageBox, "btnNo"));
    btnNo->setVisible(false);
//    ((Label*)btnNo->getVirtualRenderer())->enableOutline(Color4B::WHITE, 3);
    btnNo->setTitleText(LanguageManager::getInstance()->getText("no"));
    btnNo->setTitleFontName(GameManager::getInstance()->getFont(FONT_DEFAULT));
}
void MyMessageBox::messageBoxClosed(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    messageBox->removeFromParentAndCleanup(true);
    messageBox = NULL;
}
void MyMessageBox::messageBoxWithCloseClosed(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    Button* btn = (Button*)pSender;
    tar->setTag(btn->getTag());
    messageBox->removeFromParentAndCleanup(true);
    messageBox = NULL;
    tar->runAction(callback);
}
void MyMessageBox::addItToParent(Node* layer){
    
//    layer->addChild(this, 1000);
    Scene* scene = Scene::create();
    scene->addChild(this);
    Director::getInstance()->pushScene(scene);
    
    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(MyMessageBox::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MyMessageBox::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MyMessageBox::onTouchEnded, this);
    
    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
    
}
void MyMessageBox::result(Ref* sender){
//    CSingleton<GameManager>::getInstancePtr()->playSound(Sounds::Pop);
    MenuItem* item = (MenuItem*)sender;
    //CCLOG("Item tag: %d", item->getTag());

	if (tar && closeCall){//tar) {
        tar->setTag(item->getTag());
        //Layer* layer = (Layer*)tar;
        //layer->setTag( menu->getTag());
        //CallFuncND* call = CallFuncND actionWithTarget:tar selector:sel data:NSNumber numberWithInt:menu->getTag()));
        tar->runAction(closeCall);
    }
    Director::getInstance()->popScene();
//    tar->removeChild(this);
}
void MyMessageBox::removeUsedAsset()
{
//    this->removeFromParentAndCleanup(false);
    
    btnLabel->setVisible(false);
    btnLabel1->setVisible(false);
    btnLabel2->setVisible(false);
    
    btnMenu->setVisible(false);
    btnMenu1->setVisible(false);
    btnMenu2->setVisible(false);
    
    messageLabel->setVisible(false);
}
void MyMessageBox::close(){
    removeUsedAsset();
    return;
    //tar removeChild:this cleanup:false);
    winBack->setVisible(false);

    /*if (tar && sel) {
        if (tar) {
            Layer* layer = (Layer*)tar;
            CallFuncN* call = CallFuncN::create(tar, sel);
            tar runAction:call);
        }
    }*/
	tar->runAction(closeCall);
    tar->retain();
}