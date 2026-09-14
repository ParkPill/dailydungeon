//
//  Ios_NativeInterface.m
//  AsteroidInvaders2
//
//  Created by EAA van Poeijer on 17-12-12.
//
//

// Copyright (c) 2012 SeventyFour
#import "NativeInterface.h"
#import "GameManager.h"


namespace NativeInterface
{
    bool NativeInterface::init(actionFinished callback)
    {
//        [Ios_BillingManager sharedManager].billingAvailableCallback = callback;
//        [[Ios_BillingManager sharedManager] initialize];
        return true;
    }
    void NativeInterface::loadSKUItems(actionFinished callback)
    {
//        CCLOG("iOS load SKU Items");
//        [Ios_BillingManager sharedManager].loadSKUItemsCallback = callback;
//        [[Ios_BillingManager sharedManager] requestProductData];
    }
    const char* NativeInterface::getSkuDescription(const char* item)
    {
//        return [[Ios_BillingManager sharedManager]getSkuDescription:item];
    }
    const char* NativeInterface::getSkuPrice(const char* item)
    {
//        return [[Ios_BillingManager sharedManager]getSkuPrice:item];
    }
    const char* NativeInterface::getSkuDetails(const char* item)
    {
//        return [[Ios_BillingManager sharedManager]getSkuDetails:item];
    }
    const char* NativeInterface::getSkuTitle(const char* item)
    {
//        return [[Ios_BillingManager sharedManager]getSkuTitle:item];
    }
    const char* NativeInterface::getSkuType(const char* item)
    {
//        return [[Ios_BillingManager sharedManager]getSkuType:item];
    }
    
    void NativeInterface::registerLocalNotification(int sec, const char* msg, const char* title){
//        [[Ios_BillingManager sharedManager] registerLocalNotification:sec msg:msg title:title];
    }
    
    void NativeInterface::cancelAllLocalNotification(){
//        [[Ios_BillingManager sharedManager] cancelAllLocalNotification];
    }
    
    bool NativeInterface::isPurchased(const char *item)
    {
        // NOT IMPLEMENTED IS HANDLED DIFFERENT IN IOS
        
        return false;
    }
    void NativeInterface::purchase(const char* item, int code, actionFinished callback)
    {
        CCLOG("purchasing: %s, code: %d", item, code);
        // int code is used for restoring 1 = restore
//        [Ios_BillingManager sharedManager].purchaseCallback = callback;
//        [[Ios_BillingManager sharedManager] purchase:item code:code];
    }
    /*
    void NativeInterface::naverPurchase(const char* item, int code, actionFinished callback)
    {
        // int code is used for restoring 1 = restore
        //[Ios_BillingManager sharedManager].purchaseCallback = callback;
        //[[Ios_BillingManager sharedManager] purchase:item code:code];
    }
    void NativeInterface::ktPurchase(const char* item, actionFinished callback){}
    void NativeInterface::samsungPurchase(const char* item, actionFinished callback){}
    void NativeInterface::requestPaymentTStore(const char* item, actionFinished callback)
    {
        // int code is used for restoring 1 = restore
        //       //[Ios_BillingManager sharedManager].purchaseCallback = callback;
        //        //[[Ios_BillingManager sharedManager] purchase:item];
    }*/
    
    void NativeInterface::restorePurchases(const char* item, actionFinished callback)
    {
//        [Ios_BillingManager sharedManager].checkPurchasesCallback = callback;
//        [[Ios_BillingManager sharedManager] restorePurchases];
        
    }
    void NativeInterface::showAds()
    {
//        [[Ios_BillingManager sharedManager] showAds];
    }
    void NativeInterface::showTopAds()
    {
        //        //[[Ios_BillingManager sharedManager] showTopAds];
    }
    void NativeInterface::destroyAds()
    {
//        [[Ios_BillingManager sharedManager] hideAds];
    }
    void NativeInterface::showRectAds()
    {
        //        //[[Ios_BillingManager sharedManager] showRectAds];
    }
    void NativeInterface::destroyRectAds()
    {
        //        //[[Ios_BillingManager sharedManager] hideRectAds];
    }
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    /*void NativeInterface::ShowGameCenterAchievement(){
//        [GameCenter showAchievements];
    }
    void NativeInterface::StartGameCenter(){
        [GameCenter startGameCenter];
    }
    void NativeInterface::SendAchievement(const char* text, float percent){
//        [GameCenter sendAchievementComplete:text percentage:percent];
    }
    
    void NativeInterface::showLeaderboard(const char* leaderboardID)
    {
        [GameCenter showLeaderboard:leaderboardID];
    }
    
    void NativeInterface::postScore(int score, const char* leaderboardID)
    {
        [GameCenter reportScore:score scoreType:leaderboardID];
    }*/
#endif
    void NativeInterface::showUnityAdsVideo(){
        
    }
    void NativeInterface::showAdColonyVideo(){
        /*[AdColony playVideoAdForZone:@"vz2d6e46b8ab3f4888ab"
         withDelegate:[Ios_BillingManager sharedManager]
         withV4VCPrePopup:YES
         andV4VCPostPopup:YES];*/
    }
    void NativeInterface::addAdColonyRewards(){
        //        GameManager::getInstance()->showAdColonyVideoDone();
    }
    
    void NativeInterface::showChartboost(){
        /*Chartboost *cb = [Chartboost sharedChartboost];
         
         cb.appId = @"53b7a7c089b0bb7eabde7325";
         cb.appSignature = @"99b4dfb4a386236f2b20530b4822df5e86fffab2";
         
         [cb startSession];
         [cb showInterstitial];*/
    }
    void NativeInterface::shareLink(const char* title, const char* link){
        
    }
    void NativeInterface::sharePhoto(const char* fileName, const char* msg){
        
    }
    void NativeInterface::showChartboostMoreApps(){
        /*NSLog(@"show chartboost more apps");
         Chartboost *cb = [Chartboost sharedChartboost];
         
         cb.appId = @"53b7a7c089b0bb7eabde7325";
         cb.appSignature = @"99b4dfb4a386236f2b20530b4822df5e86fffab2";
         
         [cb startSession];
         [cb showMoreApps];*/
    }
    
    //    void NativeInterface::consumePurchase(const char* item,actionFinished callback )
    //    {
    //        // Implementation is different
    //        // On ios this is done on server side;
    //        if (callback)
    //            (*callback)(NativeInterface::CONSUMEPURCHASE_YES);
    //
    //    }
    
    void NativeInterface::openUrl(const char* str)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:str]];
        [[UIApplication sharedApplication] openURL:url];
#endif
        //[[NSUserDefaults standardUserDefaults] setObject:@"YES" forKey:@"GotFreeCoin"];
    }
    int NativeInterface::getIntegerForKey(const char* key)
    {
        NSNumber* num = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithUTF8String:key]];
        if (num) {
            return [num intValue];
        }
        return -1;
    }
    
    void NativeInterface::Push_Notification(const char* msg, int seconds)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
        
        [[UIApplication sharedApplication] cancelAllLocalNotifications];
        
        // UILocalNotification 객체 생성
        UILocalNotification *noti = [[UILocalNotification alloc]init];
        
        // 알람 발생 시각 설정. 5초후로 설정. NSDate 타입.
        noti.fireDate = [NSDate dateWithTimeIntervalSinceNow:seconds];
        
        // timeZone 설정.
        noti.timeZone = [NSTimeZone systemTimeZone];
        
        // 알림 메시지 설정
        noti.alertBody = [NSString stringWithUTF8String: msg];
        
        // 알림 액션 설정
        noti.alertAction = @"Play!";
        
        // 아이콘 뱃지 넘버 설정. 임의로 1 입력
        noti.applicationIconBadgeNumber = 1;
        
        // 알림 사운드 설정. 자체 제작 사운드도 가능. (if nil = no sound)
        noti.soundName = UILocalNotificationDefaultSoundName;
        
        // 임의의 사용자 정보 설정. 알림 화면엔 나타나지 않음
        noti.userInfo = [NSDictionary dictionaryWithObject:@"FromNotification" forKey:@"How"];
        
        // UIApplication을 이용하여 알림을 등록.
        [[UIApplication sharedApplication] scheduleLocalNotification:noti];
        
        
        for (int i = 1; i < 10; i++) {
            noti = [[UILocalNotification alloc]init];
            noti.fireDate = [NSDate dateWithTimeIntervalSinceNow:60*60*24*i];
            noti.timeZone = [NSTimeZone systemTimeZone];
            noti.alertBody = @"Your energy is full! Time to kill the umpaka, finally!";
            noti.alertAction = @"Play!";
            noti.applicationIconBadgeNumber = 1;
            noti.soundName = UILocalNotificationDefaultSoundName;
            noti.userInfo = [NSDictionary dictionaryWithObject:@"FromNotification" forKey:@"How"];
            [[UIApplication sharedApplication] scheduleLocalNotification:noti];
        }
#endif
    }
    
}