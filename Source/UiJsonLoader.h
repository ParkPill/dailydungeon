//
//  UiJsonLoader.hpp
//  CartoonCraftNV
//
//  Cocos Studio 레이아웃을 .ui.json 에서 읽어 화면을 만든다.
//
//  왜 csb 가 아니라 json 인가:
//  csb 는 바이너리라 무엇이 들어 있는지 눈으로 볼 수 없고, Cocos Studio 에서
//  export 해서 복사해 넣는 것을 잊으면 화면이 조용히 빈 채로 뜬다(실제로 그랬다).
//  json 은 열어보면 그대로 읽히고, 좌표를 손으로 고칠 수도 있다.
//
//  포맷은 tools 로 csb 에서 뽑아낸 "CartoonCraftUI" v1 이다. 필드는
//  Resources/res/MapCreator.ui.json 참고.
//
//  주의: 게임의 다른 화면들은 여전히 CSLoader + csb 를 쓴다. 이 로더는 그것을
//  대신하려는 것이 아니라, json 을 유지하는 화면에서만 쓴다.
//

#ifndef UiJsonLoader_hpp
#define UiJsonLoader_hpp

#include <stdio.h>
#include "cocos2d.h"

class UiJsonLoader
{
public:
    // 파일을 읽어 노드 트리를 만든다. 실패하면 nullptr 을 주고 이유를 로그에 남긴다.
    // 돌려주는 노드의 콘텐츠 크기는 json root 의 width/height 다.
    // designSize는 에디터 메타데이터이므로 런타임 크기로 사용하지 않는다.
    static cocos2d::Node* createNode(const std::string& fileName);

    // 루트를 targetSize로 만들고 각 노드의 layout 규칙을 부모 크기 변화에
    // 맞춰 적용한다. Left/Right/Center/Stretch와
    // Bottom/Top/Center/Stretch를 지원한다.
    static cocos2d::Node* createNode(const std::string& fileName,
                                     const cocos2d::Size& targetSize);
};

#endif /* UiJsonLoader_hpp */
