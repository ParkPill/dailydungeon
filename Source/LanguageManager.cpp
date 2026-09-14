//
//  LanguageManager.cpp
//  LegendDaryKakao
//
//  Created by SUNG PILL PACK on 14. 8. 13..
//
//

#include "LanguageManager.h"
#include "GameManager.h"

LanguageManager::LanguageManager(){
    
}
LanguageManager* LanguageManager::m_mySingleton = NULL;
LanguageManager* LanguageManager::getInstance()
{
    if(NULL == m_mySingleton)
    {
        m_mySingleton = new LanguageManager();
        m_mySingleton->retain();
    }
    return m_mySingleton;
}

std::string LanguageManager::getLocalizedFont(){
    return GM->getFont(FONT_DEFAULT);
}

cocos2d::Label* LanguageManager::getLocalizedLabel(const char* text, Color4B color, int fontSize, float scale){
    std::string font = GM->getFont(FONT_DEFAULT);
    Label* lbl;
    if(GM->endsWith(font, ".ttf")){
        lbl = Label::createWithTTF(text, font, fontSize);
    }else{
        lbl = Label::createWithSystemFont(text, font, fontSize);
    }
    
    lbl->setScale(scale);
    return lbl;
    if (LanguageManager::getInstance()->getLanguageType() == LanguageType::KOREAN) {
        Label* lbl = Label::createWithTTF(text, getLocalizedFont(), 80);
        lbl->setTextColor(color);
        lbl->setScale(0.08);
        return lbl;
    }else{
        
//                Label* lbl = Label::createWithTTF("label", "bitdust1.ttf", 80);
        Label* lbl = Label::createWithTTF(text, getLocalizedFont(), 80);
        lbl->setTextColor(color);
        lbl->setScale(0.08);
        return lbl;
    }
}
cocos2d::Label* LanguageManager::getLocalizedLabel(){
    return getLocalizedLabel("label", Color4B::WHITE);
}
cocos2d::LanguageType LanguageManager::getLanguageType(){
    int languageType = UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_LANGUAGE, -1);
    cocos2d::LanguageType type = cocos2d::LanguageType::ENGLISH;
    if (languageType > -1) {
        type = (LanguageType)languageType;
    }else{
        type = Application::getInstance()->getCurrentLanguage();
    }
    return type;
}
std::string LanguageManager::getText(std::string textId){
    if(languageTable.find(textId) == languageTable.end()){
        return textId;
    }
    ValueMap row = languageTable[textId].asValueMap();
    cocos2d::LanguageType type = getLanguageType();
    std::string str;
    if (type == LanguageType::KOREAN) {
        str = row.at("korean").asString();
    }else{
        str = row.at("english").asString();
    }
//    str = row.at("english").asString();
    int index = (int)str.find("\\n");
    if (index >= 0) {
        str.replace(index, 2, "\n");
    }
    while((index = (int)str.find("`")) >= 0){
        if (index >= 0) {
            str.replace(index, 1, ",");
        }
    }
    return str;
}
void LanguageManager::loadLanguageSheet(){
    std::string fileName = "lddlanguageSheet.csv";
    std::string csvStr = FileUtils::getInstance()->getStringFromFile(fileName);
    ValueVector rows = GameManager::getInstance()->split(csvStr, "\r\n");
    ValueVector keys = GameManager::getInstance()->split(rows.at(0).asString(), ",");
    for (int i = 1; i < (int)rows.size(); i ++) {
        ValueMap userParams;
        std::string strRow = rows.at(i).asString();
                log("%s", strRow.c_str());
        ValueVector params = GameManager::getInstance()->split(rows.at(i).asString(), ",");
        for (int j = 1; j < (int)params.size(); j ++) {
            std::string value =  params.at(j).asString();
            std::string theKey = keys.at(j).asString();
            userParams[theKey] = value;
            log("%s:%s", theKey.c_str(), value.c_str());
        }
        std::string rowKey = params.at(0).asString();
        languageTable[rowKey] = userParams;
    }
    log("end language");
}
void LanguageManager::setLocalizedString(Label* lbl, std::string str){
    std::string text = getText(str.c_str());
    if (text.compare(lbl->getString()) != 0) {
        lbl->setString(text);
    }
}
void LanguageManager::setLocalizedString(Text* lbl, std::string str){
    setLocalizedString(str, lbl);
}

void LanguageManager::setLocalizedStringNotKey(Text* lbl, std::string str){
    if (str.compare(lbl->getString()) != 0) {
        lbl->setString(str);
        setLabelLocalizedEffect(lbl);
    }
}
void LanguageManager::setLocalizedString(std::string str, Text* lbl){
    std::string text = getText(str.c_str());
    if (text.compare(lbl->getString()) != 0) {
        lbl->setString(text);
        if(text.length() == 0){
            lbl->setString(str);
        }
        setLabelLocalizedEffect(lbl);
    }
}
void LanguageManager::setLabelLocalizedEffect(Text* lbl){
    std::string strFont = getLocalizedFont();
    
    if(getLanguageType() == LanguageType::KOREAN && lbl->getFontName().compare(strFont) != 0){
        lbl->setFontSize(lbl->getFontSize()*0.7f);
    }
    lbl->setFontName(strFont);
    Label* lblRenderer = (Label*)lbl->getVirtualRenderer();
    if (strFont.at(strFont.length() - 4) != '.' && lblRenderer->getOutlineSize() > 0) {
        lbl->enableShadow(Color4B(lblRenderer->getEffectColor()), Size(2, -2), 10);
    }
}
