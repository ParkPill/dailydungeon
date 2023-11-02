//
//  ServerManager.h
//  LegendDaryKakao
//
//  Created by SUNG PILL PACK on 14. 2. 14..
//
//

#ifndef __LegendDaryKakao__ServerManager__
#define __LegendDaryKakao__ServerManager__

#include <iostream>
#include <time.h>
#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "json/document.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "network/HttpClient.h"

//#define DATA_KEY_LEVEL 

#define SET_DOCUMENT_AND_CHECK_ERROR rapidjson::Document document = getDocument(sender, data); if(document.IsNull()){ this->isServerFailed = true; return; } if (document.HasMember("error")) { log("server data error: %s", document["error"].GetString()); isFailedToGetNetworkData = true;}

//#include "Title.h"

class ServerManager: public cocos2d::Layer
{
private:
    //Constructor
    ServerManager();
    
    //Instance of the singleton
    static ServerManager* m_mySingleton;
   
    struct tm receivedTime;
    struct tm receivedYesterdayTime;
public:
    std::string apiName;
    bool getLocalTime = false;
    time_t startLocalTime;
    double webTime;
    bool timeEstablished;
    bool isTrying;
    int retryCount=0;
    double getCurrentTime();
    const char* getRemainTime(double targetTime);
    static ServerManager* getInstance();
    
    virtual bool init();
    CREATE_FUNC(ServerManager);
    void getHttpTime();
    
    void onHttpRequestCompleted(cocos2d::Node *sender, void *data);
    void updateTick(float dt);
    
    int getDay();
    int getMonth();
    int getYear();
    
    int getYesterdayDay();
    int getYesterdayMonth();
    int getYesterdayYear();
    int getMonthIndex( std::string name );
    
    
    rapidjson::Document getDocument(cocos2d::Node *sender, void *data);
    rapidjson::Document lastDocument;
//    bool isServerConnected = false;
    bool isServerFailed = false;
    std::string serverUrl;
    std::string gameName;
    

    std::string requestedID = "";
    std::string syncRequestKey;
    
    bool shouldRequestTime;
    float timeRetryTimer = 0;
        
    
    void setTime(std::string strTime);
    

    void getPostBoxItem();
    void onGetPostBoxItem(cocos2d::Node *sender, void *data);
    void setPostBoxItem(std::string items);
    void onSetPostBoxItem(cocos2d::Node *sender, void *data);
    void registerName(std::string text);
    void onRegisterNameComplete(cocos2d::Node *sender, void *data);
    void changeAccount(std::string name, std::string password);
    void onChangeAccountComplete(cocos2d::Node *sender, void *data);
    void changeName(std::string name);
    void onChangeNameComplete(cocos2d::Node *sender, void *data);
    void getAllUserData();
    void saveUserData(std::string data, bool getUserDataAfterSaveSuccess = true);
    void onSaveUserData(cocos2d::Node *sender, void *data);
    void getUserData(std::string data);
    void onGetUserDataComplete(cocos2d::Node *sender, void *data);
    void getOtherUserData(std::string data);
    void onGetOtherUserDataComplete(cocos2d::Node *sender, void *data);
    void getTopPlayers(int count, std::string field);
    void onGetTopPlayersComplete(cocos2d::Node *sender, void *data);
    void checkServer();
    void onCheckServerCompleted(cocos2d::Node *sender, void *data);
    int topPlayerCount;
    bool isUserDataRequiredAfterSaveSuccess = false;
    std::string strDataForGetUserDataAfterSaveSuccess;
    
    std::string strOtherUserEquip="";
    std::string strOtherUserRuneEquip="";
    std::string strOtherUserResearch="";
    std::string strOtherUserArmor="";
    int otherUserAvatar = 0;
    int otherUserTrophy = 0;
    bool isFailedToGetNetworkData = false;
    std::string strOtherUserName = "";
    
    void sendPost(std::string method, std::string requestData, cocos2d::network::SEL_HttpResponse pSelector);
    
    double getTimeFromStr(std::string strTime);
    int getIntIfUndefineZero(std::string str);
    
    std::string resultMessage;
    void findMatches(int trophy);
    void onFindMatches(cocos2d::Node *sender, void *data);
    
    void getMyRank(int trophy);
    void onGetMyRankComplete(cocos2d::Node *sender, void *data);
    
    void verifyReceipt(std::string strSignedData, std::string strSignature);
    void onVerifyReceiptComplete(cocos2d::Node *sender, void *data);
};

#endif /* defined(__LegendDaryKakao__ServerManager__) */


