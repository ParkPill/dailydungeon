//
//  ServerManager.cpp
//  LegendDaryKakao
//
//  Created by SUNG PILL PACK on 14. 2. 14..
//
//

#include "ServerManager.h"
#include "GameManager.h"
#include "GameSharing.h"
//#include "HUD.h"
//#include "HttpsManager.h"
#include "json/document.h"
#include <ctime>
#include <iomanip>
#include <sstream>

//#include "Kakao/Common/GameUserInfo.h"
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//#include "UrlJni.h"
//#endif

using namespace cocos2d;
using namespace cocos2d::network;
ServerManager* ServerManager::m_mySingleton = NULL;

ServerManager::ServerManager()
{
    timeEstablished = false;
    isTrying = false;
}

ServerManager* ServerManager::getInstance()
{
    if(NULL == m_mySingleton)
    {
        m_mySingleton = ServerManager::create();
        m_mySingleton->webTime = -1;
        m_mySingleton->retain();
        //        m_mySingleton->HUDLayer = (HUDScene*)GameManager::sharedGameManager()->getHUDLayer();
    }
    
    return m_mySingleton;
}
bool ServerManager::init()
{
    Layer::init();
//    serverUrl = "http://118.32.41.70:8091";
//    serverUrl = "http://192.168.123.165:8091";
//    serverUrl = "http://222.120.115.95:8101"; // cartoon craft server
//    serverUrl = "http://222.120.115.95:8102"; // buggyland server
    
//    serverUrl = "http://localhost:8102"; // buggyland server test
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_TARGET_ANDROID
//    serverUrl = "http://192.168.0.02:8102"; // buggyland server
#endif
    
//    serverUrl = "http://222.120.115.95:8102"; // yangpyung server
    serverUrl = "http://54.180.72.202:8102"; // aws test 
    
    gameName = "dailydungeon";
    apiName = "ldd";
    return true;
}
void ServerManager::checkServer(){
    HttpRequest* request = new HttpRequest();
    request->setUrl(strmake("%s/checkVersion", serverUrl.c_str()));
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(this, httpresponse_selector(ServerManager::onCheckServerCompleted));
    HttpClient::getInstance()->send(request);
    request->release();
}
void ServerManager::onCheckServerCompleted(Node *sender, void *data)
{
    log("time complete request http");
    HttpResponse *response = (HttpResponse*)data;
    std::string str = std::string(response->getResponseHeader()->begin(), response->getResponseHeader()->end());
    int size = (int)response->getResponseHeader()->size();
    std::string responseData = std::string(response->getResponseData()->begin(), response->getResponseData()->end());
    rapidjson::Document document = getDocument(sender, data);
    if (!response->isSucceed())
    {
        log("!isSucceed %s", response->getErrorBuffer());
        return;
    }
    if(document.IsNull()){
        this->isServerFailed = true;
        
        return;
    }
    if (document.HasMember("versionCode")) {
        int versionCode = Value(document["versionCode"].GetString()).asInt();
        log("version code: %d", versionCode);
        GM->receivedVersionCode = versionCode;
    }
    if (document.HasMember("serverMaintenance")) {
        int maintenance = Value(document["serverMaintenance"].GetString()).asInt();
        log("serverMaintenance: %d", maintenance);
        GM->serverMaintenance = maintenance;
    }
    if (document.HasMember("time")) {
        setTime(document["time"].GetString());
    }
    
    log("resposeData: %s", responseData.c_str());
//    setTime(responseData);
}
void ServerManager::getHttpTime()
{
    if (getLocalTime) {
        time_t currentLocalTime;
        struct tm * timeinfo;
        time (&currentLocalTime);
        timeinfo = localtime (&currentLocalTime);
        
        double theTime = currentLocalTime;
        webTime = currentLocalTime;
        timeEstablished = true;
        
        uint64_t yesterdayTimeInInt = theTime - 60*60*24;
        time_t yesterdayNow;
        yesterdayNow = yesterdayTimeInInt;
        receivedYesterdayTime = *localtime(&yesterdayNow);
        
        return;
    }
    isTrying = true;
    timeEstablished = false;
    HttpRequest* request = new HttpRequest();
    //    request->setUrl("http://www.timeapi.org/utc/now.json");
    //    request->setUrl("https://fstimeserver.appspot.com/time");
//    request->setUrl("http://1-dot-fsjavatime.appspot.com/fsfreetime");
    request->setUrl(strmake("%s/time", serverUrl.c_str()));
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(this, httpresponse_selector(ServerManager::onHttpRequestCompleted));
    request->setTag("Http time from fsfreetime");
    HttpClient::getInstance()->send(request);
    request->release();
}
int ServerManager::getMonthIndex( std::string name )
{
    std::map<std::string, int> months
    {
        { "Jan", 1 },
        { "Feb", 2 },
        { "Mar", 3 },
        { "Apr", 4 },
        { "May", 5 },
        { "Jun", 6 },
        { "Jul", 7 },
        { "Aug", 8 },
        { "Sep", 9 },
        { "Oct", 10 },
        { "Nov", 11 },
        { "Dec", 12 },
        { "jan", 1 },
        { "feb", 2 },
        { "mar", 3 },
        { "apr", 4 },
        { "may", 5 },
        { "jun", 6 },
        { "jul", 7 },
        { "aug", 8 },
        { "sep", 9 },
        { "oct", 10 },
        { "nov", 11 },
        { "dec", 12 }
    };
    
    const auto iter = months.find( name );
    
    if( iter != months.cend() )
        return iter->second;
    return -1;
}
static constexpr time_t const NULL_TIME = -1;
long tz_offset(time_t when = NULL_TIME)
{
    if (when == NULL_TIME)
        when = std::time(nullptr);
    auto const tm = *std::localtime(&when);
    std::ostringstream os;
    os << std::put_time(&tm, "%z");
    std::string s = os.str();
    // s is in ISO 8601 format: "±HHMM"
    int h = std::stoi(s.substr(0,3), nullptr, 10);
    int m = std::stoi(s[0]+s.substr(3), nullptr, 10);
    
    return h * 3600 + m * 60;
}
void ServerManager::onHttpRequestCompleted(Node *sender, void *data)
{
    log("time complete request http");
    HttpResponse *response = (HttpResponse*)data;
    std::string str = std::string(response->getResponseHeader()->begin(), response->getResponseHeader()->end());
    int size = (int)response->getResponseHeader()->size();
    std::string responseData = std::string(response->getResponseData()->begin(), response->getResponseData()->end());
    
    if (!response->isSucceed())
    {
        log("!isSucceed %s", response->getErrorBuffer());
        if(HUD != nullptr){
//            HUD->isTimeGetFailed = true;
        }
        return;
    }
    //    response->getResponseHeader()
    
    //    int countryIndex = (int)resposeData.find("google.co.");
    //    std::string countryStr = resposeData.substr(countryIndex + 10, 2);
    //    countryCode = countryStr;
    //    if (countryIndex < 0) {
    //        countryCode = "kr";
    //    }
    log("resposeData: %s", responseData.c_str());
    setTime(responseData);
    /*
     //    printf("Http Test, dump data: ");
     //char numberInChar[12];
     int startIndex = -1;
     int endIndex = -1;
     for (unsigned int i = 0; i < buffer->size(); i++)
     {
     printf("%c", (*buffer)[i]);
     if ((*buffer)[i] == ':') {
     startIndex = i+1;
     }else if((*buffer)[i] == '}'){
     endIndex = i;
     }
     }
     long tick = 0;
     int num = 0;
     long p = 0;
     if (endIndex - startIndex > 10) {
     startIndex = endIndex - 10;
     }
     for (int i = startIndex; i < endIndex; i++) {
     num = ((int)((*buffer)[i]) - 48);
     //log("num: %d", num);
     p = pow(10, endIndex - i -1);
     //log("p: %d", p);
     tick += num*(p);
     }
     timeEstablished = true;
     webTime = tick;
     log("web time: %ld", webTime);
     //    time_t startTime;
     struct tm * timeinfo;
     time (&startLocalTime);
     timeinfo = localtime (&startLocalTime);*/
}
double ServerManager::getTimeFromStr(std::string strTime){
    std::string year = Value(strTime.substr(0, 4)).asString();
    std::string month = Value(strTime.substr(5, 2)).asString();
    std::string date = Value(strTime.substr(8, 2)).asString();
    //    std::string timeStr = Value(strTime.substr(0, 4)).asString();
    
    //    ValueVector timeVector = GameManager::getInstance()->split(timeStr, ":");
    //    log("timeVector size %d, %s, %s", (int)timeVector.size(), timeVector.at(0).asString().c_str(), timeStr.c_str());
    std::string hour = Value(strTime.substr(11, 2)).asString();
    std::string min = Value(strTime.substr(14, 2)).asString();
    std::string sec = Value(strTime.substr(17, 2)).asString();
    
    struct tm theTime;
    theTime.tm_year = Value(year).asInt()-1900;
    theTime.tm_mon = Value(getMonthIndex(month)).asInt();
    theTime.tm_mday = Value(date).asInt();
    theTime.tm_hour = Value(hour).asInt();
    theTime.tm_min = Value(min).asInt();
    theTime.tm_sec = Value(sec).asInt();
    time_t what = mktime(&theTime); // UTC time - 1 hour can diff for summertime
    double tempWebTime = what;
//    tempWebTime += tz_offset(); // + local timezone diff
    return tempWebTime;
}
void ServerManager::setTime(std::string strTime){
    double tempWebTime = getTimeFromStr(strTime);
    double tick = tempWebTime;
    
    webTime = tempWebTime;
    log("** web time str: %s", Value(tempWebTime).asString().c_str());
    time_t now;
    now = webTime;
    //    startLocalTime = webTime;
    receivedTime = *localtime(&now); // 지역표준시로 변환한다 (대한민국은 KST)
    //ts = gmtime(&now);  // 국제표준시 GMT로 변환한다
    //    struct tm etet = *localtime(&
    uint64_t yesterdayTimeInInt = tick - 60*60*24;
    time_t yesterdayNow;
    yesterdayNow = yesterdayTimeInInt;
    receivedYesterdayTime = *localtime(&yesterdayNow);
    
    struct tm * timeinfo;
    time (&startLocalTime);
    timeinfo = localtime (&startLocalTime);
    
    //    log("http time-> %d/%d/%d %d:%d:%d", receivedTime->tm_year,receivedTime->tm_mon, receivedTime->tm_mday, receivedTime->tm_hour, receivedTime->tm_min, receivedTime->tm_sec);
    timeEstablished = true;
}
double ServerManager::getCurrentTime(){
    if(!timeEstablished){
        log("internet not connected");
    }
    time_t currentLocalTime = time(0);
    double timeDiff = difftime(currentLocalTime, startLocalTime);
    
    return webTime + timeDiff;
}

int ServerManager::getDay(){
    return receivedTime.tm_mday;
}
int ServerManager::getMonth(){
    return receivedTime.tm_mon;
}
int ServerManager::getYear(){
    return receivedTime.tm_year;
}

int ServerManager::getYesterdayDay(){
    return receivedYesterdayTime.tm_mday;
}
int ServerManager::getYesterdayMonth(){
    return receivedYesterdayTime.tm_mon;
}
int ServerManager::getYesterdayYear(){
    return receivedYesterdayTime.tm_year;
}

rapidjson::Document ServerManager::getDocument(cocos2d::Node *sender, void *data){
    log("sm request complete");
    HttpResponse *response = (HttpResponse*)data;
    std::string str = std::string(response->getResponseHeader()->begin(), response->getResponseHeader()->end());
//    int size = (int)response->getResponseHeader()->size();
    std::string resposeData = std::string(response->getResponseData()->begin(), response->getResponseData()->end());
    if (!response->isSucceed())
    {
        log("sm %s", response->getErrorBuffer());
        
        return nullptr;
    }
    
    log("sm resposeData: %s", resposeData.c_str());
    
    std::vector<char> *buffer = response->getResponseData();
    if (buffer->size() > 0) {
        std::string res;
        res.insert(res.begin(), buffer->begin(), buffer->end());
        log("sm ** res %s", res.c_str());
        //cocos2d::MessageBox(__String::createWithFormat("** res %s", res.c_str())->getCString(), "result");
        
        rapidjson::Document document;
        if(document.Parse(res.c_str()).HasParseError()){
            log("sm ** https json parsing error");
            //            isNetworkFail = true;
            return nullptr;
        }
//        log("league is array: %d", document.IsArray());
//        log("league kill: %d", document[0].GetObject()["kill"].GetInt());
        return document;
    }
    return nullptr;
}

void ServerManager::sendPost(std::string method, std::string requestData, SEL_HttpResponse pSelector){
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(strmake("%s/%s/%s/", serverUrl.c_str(),apiName.c_str(), method.c_str()));
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    request->setResponseCallback(this, pSelector);
    
    // write the post data
    request->setRequestData(requestData.c_str(), strlen(requestData.c_str()));
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void ServerManager::getPostBoxItem(){
    std::string savedRID = UDGetStr(KEY_RID, "");
    if(savedRID.length() > 0){
        sendPost("getuserdata", "_id=" + savedRID + "&" + "post=1", httpresponse_selector(ServerManager::onGetPostBoxItem));
    }else{
        sendPost("getuserdata", "id=" + requestedID + "&" + "post=1", httpresponse_selector(ServerManager::onGetPostBoxItem));
    }
}
void ServerManager::onGetPostBoxItem(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("post")) {
        HUD->postBoxTextFromServer = document["post"].GetString();
        UDSetStr(KEY_POST_MESSAGES, document["post"].GetString());
        HUD->networkStateSendData = NETWORK_HANDLE_STATE_ARRIVED;
        HUD->updatePostBox();
    }else{
        this->isServerFailed = true;
    }
}


void ServerManager::saveUserData(std::string data, bool getUserDataAfterSaveSuccess){
    isUserDataRequiredAfterSaveSuccess = getUserDataAfterSaveSuccess;
    strDataForGetUserDataAfterSaveSuccess = data;
    
    std::string savedRID = UDGetStr(KEY_RID, "");
    if(savedRID.length() > 0){
        sendPost("saveuser", "_id=" + savedRID + "&" + data, httpresponse_selector(ServerManager::onSaveUserData));
    }else{
        sendPost("saveuser", "id=" + requestedID + "&" + data, httpresponse_selector(ServerManager::onSaveUserData));
    }
}
void ServerManager::onSaveUserData(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("error")) {
        this->isServerFailed = true;
    }else{
        if (MOBILE_TITLE) {
            MOBILE_TITLE->nameHandleState = NETWORK_HANDLE_STATE_ARRIVED;
        }else{
            if(HUD){
                HUD->networkStateSendData = NETWORK_HANDLE_STATE_ARRIVED;
            }
        }
        if (isUserDataRequiredAfterSaveSuccess) {
            isUserDataRequiredAfterSaveSuccess = false;
            getUserData(strDataForGetUserDataAfterSaveSuccess);
        }
    }
}
void ServerManager::findMatches(int trophy){
    sendPost("findmatch", strmake("network_matches=%d", trophy), httpresponse_selector(ServerManager::onFindMatches));
}

void ServerManager::onFindMatches(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("ids")) {
        GM->strNetworkMatchIDs = document["ids"].GetString();
        GM->strNetworkMatchTrophys = document["trophys"].GetString();
        GM->strNetworkMatchAvatars = document["avatars"].GetString();
        GM->strNetworkMatchNames = document["names"].GetString();
        GM->strNetworkMatchEquips = document["equips"].GetString();
        GM->strNetworkMatchRunes = document["runes"].GetString();
        GM->strNetworkMatchArmors = document["armors"].GetString();
        GM->strNetworkMatchResearchs = document["researchs"].GetString();
        if(HUD){
            HUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
        }
        
        GM->strNetworkMatchDatas = "" + GM->strNetworkMatchIDs + "|" +
                                    GM->strNetworkMatchTrophys + "|" +
                                    GM->strNetworkMatchAvatars + "|" +
                                    GM->strNetworkMatchNames + "|" +
                                    GM->strNetworkMatchEquips + "|" +
                                    GM->strNetworkMatchRunes + "|" +
                                    GM->strNetworkMatchArmors + "|" +
                                    GM->strNetworkMatchResearchs;
        
        std::string strData = "network_matches_defeat=";
        for (int i = 0; i < 10; i++) {
            strData = strmake("%s%d", strData.c_str(), 0);
            UDSetInt(strmake(KEY_NETWORK_BATTLE_RESULT_FORMAT, i).c_str(), 0);
        }
        strData += strData + "&network_matches=" + GM->strNetworkMatchDatas;
        SM->saveUserData(strData);
    }else{
        HUD->networkStateGetData = NETWORK_HANDLE_STATE_ERROR;
    }
}
void ServerManager::setPostBoxItem(std::string items){
    sendPost("saveuser", "id=" + requestedID + "&" + "post=" + items, httpresponse_selector(ServerManager::onGetPostBoxItem));
}
void ServerManager::onSetPostBoxItem(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("post")) {
        
    }else{
        
    }
}
void ServerManager::registerName(std::string name){
    sendPost("saveuser", "id=" + requestedID + "&" + "name=" + name, httpresponse_selector(ServerManager::onRegisterNameComplete));
}
void ServerManager::onRegisterNameComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("result")) {
        log("saved id: %s", document["result"].GetString());
        requestedID = document["result"].GetString();
        UDSetStr(KEY_SAVED_ID, requestedID);
        resultMessage = requestedID;
        GameSharing::firebaseLog("create_account", "create_account", "create_account");
        if (document.HasMember("_id")) {
            std::string strRID = "";
            strRID += document["_id"].GetString();
            UDSetStr(KEY_RID, strRID);
        }
    }else{
        resultMessage = document["error"].GetString();
    }
    MOBILE_TITLE->nameHandleState = NETWORK_HANDLE_STATE_ARRIVED;
}
void ServerManager::changeAccount(std::string name, std::string password){
    sendPost("userlogin", strmake("name=%s&password=%s", name.c_str(), password.c_str()), httpresponse_selector(ServerManager::onChangeAccountComplete));
}
void ServerManager::onChangeAccountComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        return;
    }
    bool success = false;
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
        log("login failed");
        //        return;
    }else if(document.HasMember("result")) {
        std::string result = document["result"].GetString();
        requestedID = document["result"].GetString();
        UDSetStr(KEY_SAVED_ID, requestedID);
        UDSetStr(KEY_RID, "");
        
        for (int i = 0; i < 4; i++) {
            UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, i).c_str(), -1);
            UDSetInt(strmake(KEY_SHIELD_SLOT_FORMAT, i).c_str(), -1);
        }
        if(HUD){
            HUD->isWeaponEquipChanged = true;
        }
        
        success = true;
    }
    
    if(HUD){
        HUD->isChangingAccountSuccess = success;
        HUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
    }else if(MOBILE_TITLE){
        MOBILE_TITLE->changeAccountResult = success?1:0;
    }
}
void ServerManager::changeName(std::string name){
    sendPost("changename", strmake("id=%s&name=%s", requestedID.c_str(), name.c_str()), httpresponse_selector(ServerManager::onChangeNameComplete));
}
void ServerManager::onChangeNameComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        return;
    }
    bool success = false;
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
        log("login failed");
        //        return;
    }else if(document.HasMember("result")) {
        std::string result = document["result"].GetString();
        UDSetStr(KEY_NAME, document["name"].GetString());
        if(HUD){
            HUD->playerName = document["name"].GetString();
        }
        success = true;
    }
    if(HUD){
        HUD->isChangingNameSuccess = success;
        HUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void ServerManager::getAllUserData(){
    std::string savedRID = UDGetStr(KEY_RID, "");
    std::string strIDPart;
    if(savedRID.length() > 0){
        strIDPart = "_id=" + savedRID;
    }else{
        strIDPart = "id=" + requestedID;
    }
    sendPost("getuserdata", strIDPart + "&level=1&exp=1&name=1&trophy=1&avatar=1&avatar_unlock=1&inventory=1&dailymission=1&achieve=1&armor=1&research=1&weapon_collect=1&shield_collect=1&rune_collect=1&gem=1&achieve0=1&achieve1=1&achieve2=1&achieve3=1&crystal_red=1&crystal_orange=1&crystal_gray=1&crystal_green=1&crystal_purple=1&attend=1&gift_day=1&search_ticket_count=1&search_state=1&search_items=1&search_refill_start_time=1&local_ticket_count=1&local_ticket_refill_start_time=1&network_ticket_count=1&network_ticket_refill_start_time=1&network_matches=1&network_matches_defeat=1&dungeon_ticket_count=1&dungeon_ticket_refill_start_time=1&boss_ticket_count=1&boss_ticket_refill_start_time=1&equip=1&rune_equip=1&iap_list=1&iap_total=1&stage=1&mission=1&post=1&tutorial=1&last_launch_date=1&monthly_gem_end_date=1&monthly_ticket_end_date=1&monthly_ticket_last_receive_date=1&monthly_gem_last_receive_date=1&monthly_ticket_instant_receive=1&monthly_gem_instant_receive=1&access_reward_receive=1&first_launch_date=1&video_shop_index=1&video_shop_watched_time=1&video_store_0_index=1&video_store_0_watched_time=1&video_store_1_index=1&video_store_1_watched_time=1&video_store_2_index=1&video_store_2_watched_time=1", httpresponse_selector(ServerManager::onGetUserDataComplete));
}
void ServerManager::getOtherUserData(std::string data){
//    sendPost("getuserdata", data, httpresponse_selector(ServerManager::onGetOtherUserDataComplete));
}
void ServerManager::onGetOtherUserDataComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        return;
    }
    isFailedToGetNetworkData = false;
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
        return;
    }
    if(document.HasMember("time")) setTime(document["time"].GetString());
//    SET_DOCUMENT_AND_CHECK_ERROR
//    if(document.HasMember("level")) UDSetInt(KEY_LEVEL, document["level"].GetInt());
//    if(document.HasMember("exp")) UDSetInt(KEY_EXP, document["exp"].GetInt());
    if(document.HasMember("name")) strOtherUserName = document["name"].GetString();
    if(document.HasMember("trophy")) otherUserTrophy = document["trophy"].GetInt();
    if(document.HasMember("avatar")) otherUserAvatar = document["avatar"].GetInt();
//    if(document.HasMember("inventory")) {
//        UDSetStr(KEY_BAG_CONTENT, document["inventory"].GetString());
//        GM->loadBagContent();
//    }
//    if(document.HasMember("achieve")) UDSetStr(KEY_ACHIEVEMENT, document["achieve"].GetString());
    if(document.HasMember("armor")){
        strOtherUserArmor=document["armor"].GetString();
    }
    if(document.HasMember("research")){
        strOtherUserResearch=document["research"].GetString();
    }
    
    if(document.HasMember("equip")) {
        strOtherUserEquip = document["equip"].GetString();
    }
    if(document.HasMember("rune_equip")) {
        strOtherUserRuneEquip = document["rune_equip"].GetString();
    }
    
    lastDocument = getDocument(sender, data);
    if(HUD != nullptr){
        HUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
    }
}

void ServerManager::getTopPlayers(int count, std::string field){
    topPlayerCount = count;
    sendPost("findtopplayers", strmake("count=%d&field=%s", count, field.c_str()), httpresponse_selector(ServerManager::onGetTopPlayersComplete));
}
void ServerManager::onGetTopPlayersComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        return;
    }
    isFailedToGetNetworkData = false;
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
        this->isServerFailed = true;
        return;
    }
//    if(document.HasMember("time")) setTime(document["time"].GetString());
    HUD->topNameArray.clear();
    HUD->topAvatarArray.clear();
    HUD->topEquipArray.clear();
    for (int i = 0; i < document["result"].GetArray().Size(); i++) {
        std::string name = "Unknown";
        if (document["result"].GetArray()[i].HasMember("name")) {
            name = document["result"].GetArray()[i]["name"].GetString();
        }
        HUD->topNameArray.push_back(name);
        HUD->topAvatarArray.push_back(document["result"].GetArray()[i]["avatar"].GetInt());
        HUD->topEquipArray.push_back(document["result"].GetArray()[i]["equip"].GetString());
    }
    
    lastDocument = getDocument(sender, data);
    if(HUD != nullptr){
        HUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void ServerManager::getUserData(std::string data){
    std::string savedRID = UDGetStr(KEY_RID, "");
    std::string strIDPart;
    if(savedRID.length() > 0){
        strIDPart = "_id=" + savedRID;
    }else{
        strIDPart = "id=" + requestedID;
    }
    sendPost("getuserdata", strIDPart + "&" + data.c_str(), httpresponse_selector(ServerManager::onGetUserDataComplete));
}
void ServerManager::onGetUserDataComplete(cocos2d::Node *sender, void *data){
    log("*sm user data complete 0");
    rapidjson::Document document = getDocument(sender, data);
    log("*sm user data complete 0.5");
    if(document.IsNull()){
        this->isServerFailed = true;
        return;
    }
    log("*sm user data complete 0.7");
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        return;
    }
    double now = getCurrentTime();
    log("*sm user data complete 1");
    setTime(document["time"].GetString());
    //    SET_DOCUMENT_AND_CHECK_ERROR
    if(document.HasMember("level")) UDSetInt(KEY_LEVEL, document["level"].GetInt());
    if(document.HasMember("_id")) {
        UDSetStr(KEY_RID, document["_id"].GetString());
    }
    if(document.HasMember("password")) UDSetStr(KEY_PASSWORD, document["password"].GetString());
    if(document.HasMember("exp")) UDSetInt(KEY_EXP, document["exp"].GetInt());
    if(document.HasMember("name")) {
        UDSetStr(KEY_NAME, document["name"].GetString());
        log("name: %s", UDGetStr(KEY_NAME).c_str());
        if(HUD){
            HUD->playerName = document["name"].GetString();
        }
    }
    log("*sm user data complete 2");
    if(document.HasMember("trophy")) UDSetInt(KEY_TROPHY, document["trophy"].GetInt());
    if(document.HasMember("avatar")) UDSetInt(KEY_AVATAR, document["avatar"].GetInt());
    if(document.HasMember("avatar_unlock")) UDSetInt(KEY_UNLOCKED_LOCAL_ENEMY, document["avatar_unlock"].GetInt());
    if(document.HasMember("inventory")) {
        std::string strIvt = document["inventory"].GetString();
        UDSetStr(KEY_BAG_CONTENT, strIvt);
        GM->loadBagContent();
    }
    if(document.HasMember("dailymission")) {
        std::string str = document["dailymission"].GetString();
        for (int i = 0; i < 12; i++) {
            int num = Value(strmake("%c", str.at(i))).asInt();
            log("num: %d", num);
            GM->dailyMissionComplete[i] = num;
        }
    }
    if(document.HasMember("achieve")) UDSetStr(KEY_ACHIEVEMENT, document["achieve"].GetString());
    if(document.HasMember("armor")){
        ValueVector values = GM->split(document["armor"].GetString(), ",");
        for (int i = 0; i < 4; i++) {
            UDSetInt(strmake(KEY_ARMOR_ABILITY_LEVEL_FORMAT, i).c_str(), values.at(i).asInt());
        }
    }
    log("*sm user data complete 3");
    if(document.HasMember("research")){
        ValueVector values = GM->split(document["research"].GetString(), ",");
        for (int i = 0; i < 4; i++) {
            UDSetInt(strmake(KEY_RESEARCH_ABILITY_LEVEL_FORMAT, i).c_str(), values.at(i).asInt());
        }
    }
    std::string str;
    if(document.HasMember("weapon_collect")) UDSetStr(KEY_COLLECTION_WEAPON_FOUND, document["weapon_collect"].GetString());
    if(document.HasMember("shield_collect")) UDSetStr(KEY_COLLECTION_SHIELD_FOUND, document["shield_collect"].GetString());
    if(document.HasMember("rune_collect")) UDSetStr(KEY_COLLECTION_RUNE_FOUND, document["rune_collect"].GetString());
    if(document.HasMember("gem")) GM->setGem(document["gem"].GetInt());//UDSetInt(KEY_GEM, document["gem"].GetInt());
    if(document.HasMember("achieve0")) {
        int value = document["achieve0"].GetInt();
        UDSetInt(strmake(KEY_ACHIEVE_LEVEL_FORMAT, 0).c_str(), value/1000);
        UDSetInt(strmake(KEY_ACHIEVE_FORMAT, 0).c_str(), value%1000);
    }
    if(document.HasMember("achieve1")) {
        int value = document["achieve1"].GetInt();
        UDSetInt(strmake(KEY_ACHIEVE_LEVEL_FORMAT, 1).c_str(), value/1000);
        UDSetInt(strmake(KEY_ACHIEVE_FORMAT, 1).c_str(), value%1000);
    }
    if(document.HasMember("achieve2")) {
        int value = document["achieve2"].GetInt();
        UDSetInt(strmake(KEY_ACHIEVE_LEVEL_FORMAT, 2).c_str(), value/1000);
        UDSetInt(strmake(KEY_ACHIEVE_FORMAT, 2).c_str(), value%1000);
    }
    if(document.HasMember("achieve3")) {
        int value = document["achieve3"].GetInt();
        UDSetInt(strmake(KEY_ACHIEVE_LEVEL_FORMAT, 3).c_str(), value/1000);
        UDSetInt(strmake(KEY_ACHIEVE_FORMAT, 3).c_str(), value%1000);
    }
    if(document.HasMember("crystal_red")) UDSetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, CURRENCY_TYPE_GEM_RED).c_str(), document["crystal_red"].GetInt());
    if(document.HasMember("crystal_orange")) UDSetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, CURRENCY_TYPE_GEM_ORANGE).c_str(), document["crystal_orange"].GetInt());
    if(document.HasMember("crystal_gray")) UDSetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, CURRENCY_TYPE_GEM_GRAY).c_str(), document["crystal_gray"].GetInt());
    if(document.HasMember("crystal_green")) UDSetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, CURRENCY_TYPE_GEM_GREEN).c_str(), document["crystal_green"].GetInt());
    if(document.HasMember("crystal_purple")) UDSetInt(strmake(KEY_CURRENCY_COUNT_FORMAT, CURRENCY_TYPE_GEM_PURPLE).c_str(), document["crystal_purple"].GetInt());
    if(document.HasMember("attend")) {
        str = document["attend"].GetString();
        if(str.length() > 0){
            ValueVector values = GM->split(str, ",");
            UDSetInt(KEY_DAY_COUNTER, values.at(0).asInt());
            UDSetInt(KEY_DAILY_REWARD_COUNTER, values.at(1).asInt());
        }
    }
    log("*sm user data complete 4");
    if(document.HasMember("access_reward_receive")) {
        str = document["access_reward_receive"].GetString();
        for(int i = 0; i < str.length(); i++){
            if(i == 0){
                UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_AM79, Value(str.at(i)).asInt() == 1);
            }else if(i == 1){
                UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1214, Value(str.at(i)).asInt() == 1);
            }else if(i == 2){
                UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1720, Value(str.at(i)).asInt() == 1);
            }else if(i == 3){
                UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2022, Value(str.at(i)).asInt() == 1);
            }else if(i == 4){
                UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2224, Value(str.at(i)).asInt() == 1);
            }
        }
    }
    if(document.HasMember("gift_day")) UDSetInt(key_GIFT_TAKE_DAY, document["gift_day"].GetInt());
    if(document.HasMember("search_state")){
        std::string str = document["search_state"].GetString();
        for(int i = 0; i < 3;i++){
            if(str.length() > i){
                UDSetInt(strmake(KEY_SEARCH_STATE_FORMAT, i).c_str(), Value(str.substr(i, 1)).asInt());
            }else{
                UDSetInt(strmake(KEY_SEARCH_STATE_FORMAT, i).c_str(), SEARCH_STATE_NOT_TRY);
            }
        }
    }
    if(document.HasMember("search_items")){
        std::string str = document["search_items"].GetString();
        ValueVector values = GM->split(str, ",");
        for(int i = 0; i < 3;i++){
            if(values.at(i).asString().length() > 0){
                UDSetInt(strmake(KEY_SEARCH_ITEM_INDICES_FORMAT, i).c_str(), values.at(i).asInt());
            }
        }
    }
    if(document.HasMember("search_ticket_count")) {
        GM->searchTicketCount = document["search_ticket_count"].GetInt();
    }
    log("*sm user data complete 5");
    if(document.HasMember("search_refill_start_time")) {
        //        UDSetInt(KEY_SEARCH_TICKET_REFILL_STARTED_TIME, getIntIfUndefineZero(document["search_refill_start_time"].GetString()));
        int refillStartTime = getIntIfUndefineZero(document["search_refill_start_time"].GetString());
        if(refillStartTime > now){
            refillStartTime = 0;
        }
        GM->searchTicketRefillTimeLeft = refillStartTime + SEARCH_TICKET_REFILL_TIME - now;
    }
    if(document.HasMember("local_ticket_count")) {
        int value = document["local_ticket_count"].GetInt();
        GM->localTicketCount = value;
    }
    if(document.HasMember("local_ticket_refill_start_time")) {
        int refillStartTime = getIntIfUndefineZero(document["local_ticket_refill_start_time"].GetString());
        if(refillStartTime > now){
            refillStartTime = 0;
        }
        GM->localTicketRefillTimeLeft = refillStartTime + LOCAL_TICKET_REFILL_TIME - now;
    }
    if(document.HasMember("network_ticket_count")) {
        GM->networkTicketCount = document["network_ticket_count"].GetInt();
    }
    if(document.HasMember("network_ticket_refill_start_time")) {
        int refillStartTime = getIntIfUndefineZero(document["network_ticket_refill_start_time"].GetString());
        if(refillStartTime > now){
            refillStartTime = 0;
        }
        GM->networkTicketRefillTimeLeft = refillStartTime + NETWORK_TICKET_REFILL_TIME - now;
    }
    if(document.HasMember("network_matches")) {
        std::string str = document["network_matches"].GetString();
        if(str.length() > 30){
            GM->strNetworkMatchDatas = str;
            ValueVector list = GM->split(GM->strNetworkMatchDatas, "|");
            GM->strNetworkMatchIDs = "" + list.at(0).asString();
            GM->strNetworkMatchTrophys = "" + list.at(1).asString();
            GM->strNetworkMatchAvatars = "" + list.at(2).asString();
            GM->strNetworkMatchNames = "" + list.at(3).asString();
            GM->strNetworkMatchEquips = "" + list.at(4).asString();
            GM->strNetworkMatchRunes = "" + list.at(5).asString();
            GM->strNetworkMatchArmors = "" + list.at(6).asString();
            GM->strNetworkMatchResearchs = "" + list.at(7).asString();
        }
//        log("netowrk matches: %s", HUD->strNetworkMatches.c_str());
    }
    if(document.HasMember("network_matches_defeat")) {
        std::string strDefeat = document["network_matches_defeat"].GetString();
        for (int i = 0; i < strDefeat.length(); i++) {
            int value = Value(strDefeat.substr(i, 1)).asInt();
            UDSetInt(strmake(KEY_NETWORK_BATTLE_RESULT_FORMAT, i).c_str(), value);
        }
    }
    
    if(document.HasMember("dungeon_ticket_count")) {
        GM->dungeonTicketCount = document["dungeon_ticket_count"].GetInt();
    }
    if(document.HasMember("dungeon_ticket_refill_start_time")) {
        int refillStartTime = getIntIfUndefineZero(document["dungeon_ticket_refill_start_time"].GetString());
        if(refillStartTime > now){
            log("refillStartTime > now dungeon");
            refillStartTime = 0;
        }
        GM->dungeonTicketRefillTimeLeft = refillStartTime + DUNGEON_TICKET_REFILL_TIME - now;
    }
    if(document.HasMember("boss_ticket_count")) {
        GM->bossTicketCount = document["boss_ticket_count"].GetInt();
    }
    if(document.HasMember("boss_ticket_refill_start_time")) {
        int refillStartTime = getIntIfUndefineZero(document["boss_ticket_refill_start_time"].GetString());
        if(refillStartTime > now){
            refillStartTime = 0;
        }
        GM->bossTicketRefillTimeLeft = refillStartTime + BOSS_TICKET_REFILL_TIME - now;
    }
    if(document.HasMember("equip")) {
        str = document["equip"].GetString();
        ValueVector list = GM->split(str, ",");
        for (int i = 0; i < 8; i++) {
            std::string keyFormat = KEY_WEAPON_SLOT_FORMAT;
            
            if (i >= list.size() || list.at(i).asString().length() <= 0) {
                if(i < 4) {
                    UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, i).c_str(), -1);
                }else{
                    UDSetInt(strmake(KEY_SHIELD_SLOT_FORMAT, i-4).c_str(), -1);
                }
            }else{
                if(i < 4) {
                    UDSetInt(strmake(KEY_WEAPON_SLOT_FORMAT, i).c_str(), list.at(i).asInt());
                }else{
                    UDSetInt(strmake(KEY_SHIELD_SLOT_FORMAT, i-4).c_str(), list.at(i).asInt());
                }
            }
        }
        if(HUD){
            HUD->isWeaponEquipChanged = true;
        }
    }
    if(document.HasMember("rune_equip")) {
        str = document["rune_equip"].GetString();
        ValueVector list = GM->split(str, ",");
        for (int i = 0; i < 4; i++) {
            if (i >= list.size() || list.at(i).asString().length() <= 0) {
                UDSetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, i).c_str(), -1);
            }else{
                UDSetInt(strmake(KEY_RUNE_EQUIPPED_FORMAT, i).c_str(), list.at(i).asInt());
            }
        }
    }
    if(document.HasMember("iap_list")) {
        std::string strIAPList = document["iap_list"].GetString();
        if(HUD && strIAPList.find("starter0") != std::string::npos){
            HUD->isNoAdsPurchased = true;
        }
        GM->iapList = strIAPList;
    }
    if(document.HasMember("iap_total")) UDSetInt(KEY_IAP_TOTAL, document["iap_total"].GetInt());
    if(document.HasMember("stage")) UDSetInt(KEY_STAGE, document["stage"].GetInt());
    if(document.HasMember("mission")){
        std::string str = document["mission"].GetString();
        if(str.length() > 0){
            for (int i = 0; i < 4; i++) {
                int num = Value(strmake("%c", str.at(i))).asInt();
                UDSetInt(strmake(KEY_COLLECT_MISSION_COLLECTED_COUNT_FORMAT, i).c_str(), num);
            }
        }else{
            for (int i = 0; i < 4; i++) {
                UDSetInt(strmake(KEY_COLLECT_MISSION_COLLECTED_COUNT_FORMAT, i).c_str(), 0);
            }
        }
    }
    if(document.HasMember("post")) UDSetStr(KEY_POST_MESSAGES, document["post"].GetString());
    if(document.HasMember("tutorial")) {
        GM->isTutorialOn = document["tutorial"].GetInt();
        UDSetBool(KEY_IS_TUTORIAL_CLEAR, GM->isTutorialOn==0?true:false);
    }
    if(document.HasMember("last_launch_date")) UDSetStr(KEY_LAST_LAUNCH_DATE, document["last_launch_date"].GetString());
    if(document.HasMember("monthly_gem_end_date")) {
        int day = document["monthly_gem_end_date"].GetInt();
        UDSetInt(KEY_MONTHLY_GEM_END_DATE, day);
    }
    if(document.HasMember("monthly_ticket_end_date")) {
        int day = document["monthly_ticket_end_date"].GetInt();
        UDSetInt(KEY_MONTHLY_TICKET_END_DATE, day);
    }
    if(document.HasMember("monthly_gem_last_receive_date")) UDSetInt(KEY_MONTHLY_GEM_RECEIVE_DATE, document["monthly_gem_last_receive_date"].GetInt());
    if(document.HasMember("monthly_ticket_last_receive_date")) UDSetInt(KEY_MONTHLY_TICKET_RECEIVE_DATE, document["monthly_ticket_last_receive_date"].GetInt());
    if(document.HasMember("monthly_gem_instant_receive")) UDSetBool(KEY_MONTHLY_GEM_IS_INSTANT_RECEIVED, document["monthly_gem_instant_receive"].GetInt());
    if(document.HasMember("monthly_ticket_instant_receive")) UDSetBool(KEY_MONTHLY_TICKET_IS_INSTANT_RECEIVED, document["monthly_ticket_instant_receive"].GetInt());
    if(document.HasMember("first_launch_date")) {
        std::string strFirstDay = document["first_launch_date"].GetString();
//        UDSetStr(KEY_FIRST_LAUNCH_DATE, strFirstDay);
        std::string strNow = document["time"].GetString();
        int dayCount = GM->getDayDiff(Value(strFirstDay.substr(0, 4)).asInt(), Value(strFirstDay.substr(5, 2)).asInt(), Value(strFirstDay.substr(8, 2)).asInt(), Value(strNow.substr(0, 4)).asInt(), Value(strNow.substr(5, 2)).asInt(), Value(strNow.substr(8, 2)).asInt());
        UDSetInt(KEY_DAY_COUNT, dayCount);
    }
    if(document.HasMember("video_shop_index")) {
        GM->videoShopIndex = document["video_shop_index"].GetInt();
    }
    if(document.HasMember("video_shop_watched_time")) {
        GM->videoShopWatchedTime = Value(document["video_shop_watched_time"].GetString()).asDouble();
    }
    if(document.HasMember("video_store_0_index")) {
        GM->videoStore0Index = document["video_store_0_index"].GetInt();
    }
    if(document.HasMember("video_store_0_watched_time")) {
        GM->videoStore0WatchedTime = Value(document["video_store_0_watched_time"].GetString()).asDouble();
    }
    if(document.HasMember("video_store_1_index")) {
        GM->videoStore1Index = document["video_store_1_index"].GetInt();
    }
    if(document.HasMember("video_store_1_watched_time")) {
        GM->videoStore1WatchedTime = Value(document["video_store_1_watched_time"].GetString()).asDouble();
    }
    if(document.HasMember("video_store_2_index")) {
        GM->videoStore2Index = document["video_store_2_index"].GetInt();
    }
    if(document.HasMember("video_store_2_watched_time")) {
        GM->videoStore2WatchedTime = Value(document["video_store_2_watched_time"].GetString()).asDouble();
    }
    log("*sm user data complete 7");
    lastDocument = getDocument(sender, data);
    if(HUD != nullptr){
        HUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
    }
    log("*sm user data complete 7");
}
int ServerManager::getIntIfUndefineZero(std::string str){
    if(str.compare("undefined") == 0){
        return 0;
    }
    return Value(str).asInt();
}
void ServerManager::getMyRank(int trophy){
    sendPost("getrank", strmake("id=%s&trophy=%d", requestedID.c_str(), trophy), httpresponse_selector(ServerManager::onGetMyRankComplete));
}
void ServerManager::onGetMyRankComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if(HUD != nullptr){
        HUD->isLoadingRankSuccess = false;
    }
    if(document.HasMember("result")) {
        if(HUD != nullptr){
            HUD->myRank = document["result"].GetInt() + 1;
            HUD->isLoadingRankSuccess = true;
            HUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
        }
    }
}
void ServerManager::verifyReceipt(std::string strSignedData, std::string strSignature){
    std::string strPlatform = "google";
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    strPlatform = "apple";
#endif
    
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
//    request->setUrl(strmake("%s/verifyReceipt", serverUrl.c_str()));
    request->setUrl(strmake("%s/verifyReceipt", serverUrl.c_str()));
    
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    request->setResponseCallback(this, httpresponse_selector(ServerManager::onVerifyReceiptComplete));
    
    // write the post data
    std::string requestData = strmake("id=%s&signedData=%s&signature=%s&platform=%s", requestedID.c_str(), strSignedData.c_str(), strSignature.c_str(), strPlatform.c_str());
    log("verifyReceipt %s", requestData.c_str());
    request->setRequestData(requestData.c_str(), strlen(requestData.c_str()));
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void ServerManager::onVerifyReceiptComplete(cocos2d::Node *sender, void *data){
    //    return; // test for verifying test
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
//        errorCode = 9;
        return;
    }
    isFailedToGetNetworkData = false;
    log("onVerifyReceiptComplete done");
    if (document.HasMember("error")) {
        log("onVerifyReceiptComplete done1");
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
    }
    
    log("onVerifyReceiptComplete done2");
    if (document.HasMember("result")) {
        log("onVerifyReceiptComplete done4");
        if(document["result"].GetBool()){
            GM->iapFlag = IAP_FLAG_SUCCESS;
            if(!UDGetBool(KEY_IAP_USER, false)){
                UDSetBool(KEY_IAP_USER, true);
            }
        }else{
            GM->iapFlag = IAP_FLAG_FAILED;
        }
    }else{
        GM->iapFlag = IAP_FLAG_FAILED;
        log("onVerifyReceiptComplete done3");
    }
}
