#ifndef  _MYMESSAGEBOX_LAYER_H_
#define  _MYMESSAGEBOX_LAYER_H_

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "ui/UIScrollView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"


#define MESSAGE_BOX_RESULT_OK 0
#define MESSAGE_BOX_RESULT_CANCEL 1
#define MESSAGE_BOX_RESULT_BUTTON_1 0
#define MESSAGE_BOX_RESULT_BUTTON_2 1

using namespace cocos2d;
using namespace cocos2d::ui;
class MyMessageBox : public Layer
{
public:
    //Instance of the singleton
    static MyMessageBox* m_mySingleton;
    
    Size size;
    const char* fontName;
//    Vector<Droppable*>menuArray;
//    Vector<Droppable*>labelArray;
    Sprite* back;
    Sprite* winBack;
    Node* tar;
    //SEL sel;
    CallFuncN* closeCall;
    Label* messageLabel;
    Label* btnLabel1;
    Label* btnLabel2;
    Menu* btnMenu1;
    Menu* btnMenu2;
    Label* btnLabel;
    Menu* btnMenu;
    Menu* btnClose;
    
    void addItToParent(Node* layer);
    Layout* messageBox;
	MyMessageBox();
	virtual bool init();
    CREATE_FUNC(MyMessageBox);
    Button* btnYes;
    Button* btnNo;
    //Get instance of singleton
    static MyMessageBox* getInstance();
    
	void showDialog(Node* target, CallFuncN* call, const char* msg, const char* btn1, const char* btn2);
    void showDialogCry(Node* target, CallFuncN* call, const char* msg, const char* btn1, const char* btn2);
	void showDialog(Node* target, CallFuncN* call, const char* msg, const char* btn1);
    void showDialogWithCloseBox(Node* target, CallFuncN* call, const char* msg, const char* btn1);
    void showDialog(Node* target, const char* msg);
    void showDialogWithoutButton(Node* target, const char* msg);
    void showDialog(Node* target, const char* msg, CallFuncN* call);
	void result(Ref* sender);
    void resultFromSel(Ref* sender, ui::Widget::TouchEventType eEventType);
    CallFuncN* callback;
	void close();
    void removeUsedAsset();
    
    void messageBoxClosed(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void messageBoxWithCloseClosed(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
};
#endif