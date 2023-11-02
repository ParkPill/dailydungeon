// Copyright (c) 2012 ursinepaw.com
#include "../NativeInterface.h"
#include "../GameManager.h"
#include "../SettingLayer.h"

#include "platform/android/jni/JniHelper.h"
#include "cocos2d.h"
using namespace cocos2d;

namespace NativeInterface
{
    void NativeInterface::postScore(int score, const char* leaderboardID){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "updateTopScoreLeaderboard",
                                           "(ILjava/lang/String;)V"))
        {
            jstring arg = t.env->NewStringUTF(leaderboardID);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, (jint) score, arg);
            
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::updateAchievement(const char* idStr, int percentage) {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "updateAchievement",
                                           "(Ljava/lang/String;I)V"))
        {
            jstring arg = t.env->NewStringUTF(idStr);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg,(jint) percentage);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::showAchievements(){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showAchievements",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    bool NativeInterface::init(actionFinished callback)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "init",
                                           "(J)Z"))
        {
            jlong arg = (long)(void*)callback;
            jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(t.classID);
            return ret == JNI_TRUE;
        }
        return false;
    }
    
    void NativeInterface::loadSKUItems(actionFinished callback)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "loadSKUItems",
                                           "(J)V"))
        {
            jlong arg = (long)(void*)callback;
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    
    const char* NativeInterface::getSkuDescription(const char* key)
    {
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getSkuDescription",
                                           "(Ljava/lang/String;)Ljava/lang/String;"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
            
        }
        return result.c_str();
    }
    
    const char* NativeInterface::getSkuPrice(const char* key)
    {
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getSkuPrice",
                                           "(Ljava/lang/String;)Ljava/lang/String;"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
        }
        return result.c_str();
    }
    
    const char* NativeInterface::getSkuDetails(const char* key)
    {
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getSkuDetails",
                                           "(Ljava/lang/String;)Ljava/lang/String;"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
        }
        return result.c_str();
    }
    
    const char* NativeInterface::getSkuTitle(const char* key)
    {
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getSkuTitle",
                                           "(Ljava/lang/String;)Ljava/lang/String;"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
        }
        return result.c_str();
    }
    
    const char* NativeInterface::getSkuType(const char* key)
    {
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getSkuType",
                                           "(Ljava/lang/String;)Ljava/lang/String;"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
        }
        return result.c_str();
    }
    void NativeInterface::registerLocalNotification(int sec, const char* msg, const char* title){
        
    }
    void NativeInterface::cancelAllLocalNotification(){
        
    }
    
    bool NativeInterface::isPurchased(const char* item)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "isPurchased",
                                           "(Ljava/lang/String;)Z"))
        {
            jstring arg = t.env->NewStringUTF(item);
            jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(t.classID);
            return ret == JNI_TRUE;
        }
        return false;
    }
    
    void NativeInterface::sharePhoto(const char* fileName, const char* msg)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "sharePhoto",
                                           "(Ljava/lang/String;Ljava/lang/String;)V"))
        {
            jstring arg0 = t.env->NewStringUTF(fileName);
            jstring arg1 = t.env->NewStringUTF(msg);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg0, arg1);
//            jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, arg0, arg1);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::shareLink(const char* title, const char* link)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "shareLink",
                                           "(Ljava/lang/String;Ljava/lang/String;)V"))
        {
            jstring arg0 = t.env->NewStringUTF(title);
            jstring arg1 = t.env->NewStringUTF(link);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg0, arg1);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::openUrl(const char* str)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org/cocos2dx/lib/Cocos2dxActivity",
                                           "openUrl",
                                           "(Ljava/lang/String;)V"))
        {
            jstring arg = t.env->NewStringUTF(str);
            jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    
    void NativeInterface::purchase(const char* key, int amount, actionFinished callback)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "purchase",
                                           "(Ljava/lang/String;IJ)V"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jlong arg2 = (long)(void*)callback;
            
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg,(jint) amount, arg2);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::loginPlayService()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "loginPlayService",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::logoutPlayService()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "logoutPlayService",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::showLeaderboard(const char* leaderboardID){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showLeaderboards",
                                           "(Ljava/lang/String;)V"))
        {
            jstring arg = t.env->NewStringUTF(leaderboardID);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showUnityAdsVideo()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showUnityAdsVideo",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::showAds()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showAds",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showTopAds()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showTopAds",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showChartboost()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showChartboost",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showChartboostMoreApps()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showChartboostMoreApps",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showAdColonyVideo()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showAdColonyVideo",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::addAdColonyRewards()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "addAdColonyRewards",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::destroyAds()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "destroyAds",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::showRectAds()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showRectAds",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::destroyRectAds()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "destroyRectAds",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    /*
     void NativeInterface::Push_Notification(const char* msg, int seconds)
     {
     
     }
     
     void NativeInterface::FB_CreateNewSession()
     {
     
     }
     void NativeInterface::FB_Login(actionFinished callBack)
     {
     
     }
     void NativeInterface::FB_Customize(actionFinishedLong idCallBack, actionFinishedUTF8 nameCallBack)
     {
     
     }
     void NativeInterface::FB_RequestFriendsInfo(actionFinishedCharArray arrayCallBack, actionFinishedLongArray longArrayCallBack ,actionFinished countCallBack, actionFinishedCharArray userNamesCallBack)
     {
     
     }
     void NativeInterface::FB_Invite(int score, long ids[], int count)
     {
     
     }
     void NativeInterface::FB_PostScore(int score, long fbid)
     {
     
     }
     void NativeInterface::FB_RequestPostScore()
     {
     
     }
     void NativeInterface::FB_RequestHelp()
     {
     
     }
     void NativeInterface::FB_SendLife()
     {
     
     }
     void NativeInterface::FB_RequestScores(actionFinishedCharArray namesCallBack, actionFinishedCharArray scoresCallBack)
     {
     
     }
     
     void NativeInterface::FB_ProcessIncomingURL()
     {
     
     }
     void NativeInterface::FB_ProcessIncomingRequest(const char* urlStringChar)
     {
     
     }
     void NativeInterface::FB_ProcessIncomingFeed(const char* urlStringChar)
     {
     
     }
     
     */
    void NativeInterface::restorePurchases(const char* item, actionFinished callback)
    {
        // Do nothing, not implemented for android
    }
    
    //void NativeInterface::consumePurchase(const char* item, actionFinished callback)
    //{
    //    JniMethodInfo t;
    //    if (JniHelper::getStaticMethodInfo(t,
    //                                       "org.cocos2dx.cpp/AppActivity",
    //                                       "consumePurchase",
    //                                       "(Ljava/lang/String;J)V"))
    //    {
    //        //jlong arg = (long)(void*)callback;
    //        jstring arg = t.env->NewStringUTF(item);
    //        jlong arg1 = (long)(void*)callback;
    //        t.env->CallStaticVoidMethod(t.classID, t.methodID, arg, arg1);
    //        t.env->DeleteLocalRef(t.classID);
    //     }
    //}
}
extern "C"
{
    void Java_org_cocos2dx_cpp_NativeInterface_nativeactionFinished(JNIEnv* env, jobject thiz, jlong callback_address,jint error_code)
    {
//        if (callback_address)
//            (*reinterpret_cast<NativeInterface::actionFinished>(callback_address))(error_code);
    }
    void Java_org_cocos2dx_cpp_AppActivity_unityAdsCallback(JNIEnv* env, jobject thiz)
    {
//        GameManager::getInstance()->getHudLayer()->OnVideoEnded();
    }
    void Java_org_cocos2dx_cpp_AppActivity_playServiceConnected(JNIEnv* env, jobject thiz, jboolean login)
    {
//        GameManager::getInstance()->isPlayServiceLogIn = (bool)login;
//        if (GameManager::getInstance()->settingLayer != NULL ) {
//            SettingLayer* setting = (SettingLayer*)GameManager::getInstance()->settingLayer;
//            setting->onPlayServiceLogin((bool) login);
//        }
    }
    
    //	void Java_com_seventyfour_NativeInterface_nativeactionFinished(JNIEnv* env, jobject thiz, jlong callback_address)
    //	{
    //		if (callback_address)
    //			(*reinterpret_cast<NativeInterface::actionFinished>(callback_address))();
    //	}
}
