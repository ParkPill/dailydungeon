#pragma once

#include "cocos2d.h"

#include <string>
#include <string_view>

// The original Cocos fork replaced every TMX <image source> through a custom
// Director::tilesetName field. Keep that behavior in the game, not Axmol.
inline ax::TMXTiledMap* createLegacyTiledMap(std::string_view file, std::string_view tilesetName)
{
    if (tilesetName.empty())
        return ax::TMXTiledMap::create(file);

    auto* files = ax::FileUtils::getInstance();
    const std::string fullPath = files->fullPathForFilename(file);
    std::string xml = files->getStringFromFile(fullPath);
    if (xml.empty())
        return nullptr;

    constexpr std::string_view imageSource = "<image source=\"";
    std::size_t cursor = 0;
    while ((cursor = xml.find(imageSource, cursor)) != std::string::npos)
    {
        const std::size_t start = cursor + imageSource.size();
        const std::size_t end = xml.find('"', start);
        if (end == std::string::npos)
            break;
        xml.replace(start, end - start, tilesetName);
        cursor = start + tilesetName.size();
    }

    const std::size_t slash = fullPath.find_last_of("/\\");
    const std::string resourcePath = slash == std::string::npos ? std::string() : fullPath.substr(0, slash);
    return ax::TMXTiledMap::createWithXML(xml, resourcePath);
}
