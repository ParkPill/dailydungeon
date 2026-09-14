//
//  UiJsonLoader.cpp
//  CartoonCraftNV
//

#include "UiJsonLoader.h"
#include "json/document.h"
#include "ui/UIButton.h"
#include "ui/UIImageView.h"
#include "ui/UILayout.h"
#include "ui/UIListView.h"
#include "ui/UIPageView.h"
#include "ui/UIScrollView.h"
#include "ui/UIScale9Sprite.h"
#include "ui/UIText.h"

#include <algorithm>

using namespace cocos2d;
using namespace cocos2d::ui;

namespace {

// Unity의 Pixels Per Unit Multiplier와 같은 역할을 한다. 렌더러의 논리 크기를
// 반대로 키우고 렌더러 자체를 sliceScale만큼 줄이면 위젯의 바깥 크기는 그대로
// 유지되면서 9-slice의 모서리와 테두리만 해당 비율로 작아진다.
static void applySliceScaleToRenderer(Scale9Sprite* renderer,
                                      const Size& widgetSize,
                                      float sliceScale) {
    if (renderer == nullptr || sliceScale <= 0.0f) {
        return;
    }
    renderer->setContentSize(
        Size(widgetSize.width / sliceScale, widgetSize.height / sliceScale));
    renderer->setScale(sliceScale);
    renderer->setPosition(widgetSize.width * 0.5f, widgetSize.height * 0.5f);
}

class JsonImageView : public ImageView {
public:
    static JsonImageView* create() {
        JsonImageView* image = new JsonImageView();
        if (image != nullptr && image->init()) {
            image->autorelease();
            return image;
        }
        AX_SAFE_DELETE(image);
        return nullptr;
    }

    void setSliceScale(float value) { _sliceScale = value > 0.0f ? value : 1.0f; }

protected:
    void updateLayout() override {
        ImageView::updateLayout();
        if (isScale9Enabled() && _sliceScale != 1.0f) {
            applySliceScaleToRenderer(
                dynamic_cast<Scale9Sprite*>(getRenderNode()), getContentSize(),
                _sliceScale);
        }
    }

private:
    float _sliceScale = 1.0f;
};

class JsonButton : public Button {
public:
    static JsonButton* create() {
        JsonButton* button = new JsonButton();
        if (button != nullptr && button->init()) {
            button->autorelease();
            return button;
        }
        AX_SAFE_DELETE(button);
        return nullptr;
    }

    void setSliceScale(float value) { _sliceScale = value > 0.0f ? value : 1.0f; }

protected:
    Widget* createCloneInstance() override { return JsonButton::create(); }

    void copySpecialProperties(Widget* widget) override {
        Button::copySpecialProperties(widget);
        if (JsonButton* button = dynamic_cast<JsonButton*>(widget)) {
            _sliceScale = button->_sliceScale;
        }
    }

    void updateLayout() override {
        Button::updateLayout();
        if (!isScale9Enabled() || _sliceScale == 1.0f) {
            return;
        }
        const Size widgetSize = getContentSize();
        applySliceScaleToRenderer(getRendererNormal(), widgetSize, _sliceScale);
        applySliceScaleToRenderer(getRendererClicked(), widgetSize, _sliceScale);
        applySliceScaleToRenderer(getRendererDisabled(), widgetSize, _sliceScale);
    }

private:
    float _sliceScale = 1.0f;
};

} // namespace

// json 의 수를 꺼낸다. 없으면 기본값. 정수/실수가 섞여 있어서 둘 다 받는다.
static float jsonFloat(const rapidjson::Value& v, const char* key, float def) {
    if (!v.HasMember(key) || !v[key].IsNumber()) {
        return def;
    }
    return (float)v[key].GetDouble();
}
static int jsonInt(const rapidjson::Value& v, const char* key, int def) {
    if (!v.HasMember(key) || !v[key].IsNumber()) {
        return def;
    }
    return v[key].GetInt();
}
static bool jsonBool(const rapidjson::Value& v, const char* key, bool def) {
    if (!v.HasMember(key) || !v[key].IsBool()) {
        return def;
    }
    return v[key].GetBool();
}
static std::string jsonStr(const rapidjson::Value& v, const char* key) {
    if (!v.HasMember(key) || !v[key].IsString()) {
        return "";
    }
    return v[key].GetString();
}

// "#rrggbb" 를 색으로. 형식이 아니면 흰색으로 둔다(색을 못 읽었다고 화면을
// 통째로 버릴 이유는 없다).
static Color3B parseColor(const std::string& s) {
    if (s.length() != 7 || s[0] != '#') {
        return Color3B::WHITE;
    }
    long v = strtol(s.c_str() + 1, nullptr, 16);
    return Color3B((v >> 16) & 0xff, (v >> 8) & 0xff, v & 0xff);
}

static cocos2d::Rect parseCapInsets(const rapidjson::Value& v) {
    if (!v.HasMember("capInsets") || !v["capInsets"].IsObject()) {
        return cocos2d::Rect::zero;
    }
    const rapidjson::Value& c = v["capInsets"];
    return cocos2d::Rect(jsonFloat(c, "x", 0), jsonFloat(c, "y", 0),
                         jsonFloat(c, "width", 0), jsonFloat(c, "height", 0));
}

// Layout 계열 위젯의 배경과 클리핑은 Widget::setColor로는 표현되지 않는다.
// 에디터 포맷의 colorType 0/1/2는 Layout의 NONE/SOLID/GRADIENT와 같다.
static void applyLayout(Layout* layout, const rapidjson::Value& v) {
    if (layout == nullptr) {
        return;
    }
    int colorType = jsonInt(v, "colorType", 0);
    if (colorType < 0 || colorType > 2) {
        colorType = 0;
    }
    layout->setBackGroundColorType((Layout::BackGroundColorType)colorType);
    const Color3B color = parseColor(jsonStr(v, "color"));
    if (colorType == 2) {
        // 현재 포맷에는 gradient의 두 번째 색 필드가 없으므로 같은 색으로
        // 초기화한다. 적어도 에디터에서 지정한 색과 투명도는 보존된다.
        layout->setBackGroundColor(color, color);
    } else {
        layout->setBackGroundColor(color);
    }
    layout->setBackGroundColorOpacity(
        (GLubyte)jsonInt(v, "bgColorOpacity", 255));
    layout->setClippingEnabled(
        jsonBool(v, "clipEnabled", layout->isClippingEnabled()));
}

// scale9 는 크기를 정하기 전에 켜야 한다. 켜는 순간 콘텐츠 크기가 원본 그림
// 크기로 되돌아가기 때문이다.
static void applyScale9(Widget* w, const rapidjson::Value& v) {
    if (!jsonBool(v, "scale9Enabled", false)) {
        return;
    }
    cocos2d::Rect caps = parseCapInsets(v);
    if (ImageView* img = dynamic_cast<ImageView*>(w)) {
        img->setScale9Enabled(true);
        if (!caps.equals(cocos2d::Rect::zero)) {
            img->setCapInsets(caps);
        }
    } else if (Button* btn = dynamic_cast<Button*>(w)) {
        btn->setScale9Enabled(true);
        if (!caps.equals(cocos2d::Rect::zero)) {
            btn->setCapInsets(caps);
        }
    }
}

// 스크롤 관련 값들을 넣는다. 반드시 setContentSize 뒤에 불러야 한다 —
// setInnerContainerSize 는 준 값을 스크롤뷰의 콘텐츠 크기와 견줘서 더 작으면
// 무시하기 때문에(UIScrollView.cpp), 크기를 정하기 전에 부르면 안쪽 크기가
// 통째로 버려진다.
static void applyScrollView(ScrollView* sv, const rapidjson::Value& v) {
    // 없는 값은 지금 값을 그대로 둔다. 아래 Panel-"scrollView" 갈래가 미리
    // 정해 둔 것(세로/바운스)을 json 에 그 항목이 없다고 덮어쓰면 안 된다.
    //
    // json 의 direction 은 csb 값을 그대로 옮긴 것이고 ScrollView::Direction 과
    // 순서가 같다. 0=NONE 1=VERTICAL 2=HORIZONTAL 3=BOTH.
    int dir = jsonInt(v, "direction", (int)sv->getDirection());
    if (dir < 0 || dir > (int)ScrollView::Direction::BOTH) {
        dir = (int)sv->getDirection();
    }
    sv->setDirection((ScrollView::Direction)dir);
    const cocos2d::Size inner = sv->getInnerContainerSize();
    sv->setInnerContainerSize(
        cocos2d::Size(jsonFloat(v, "innerWidth", inner.width),
                      jsonFloat(v, "innerHeight", inner.height)));
    sv->setClippingEnabled(jsonBool(v, "clipEnabled", sv->isClippingEnabled()));
    sv->setBounceEnabled(jsonBool(v, "bounceEnabled", sv->isBounceEnabled()));
    sv->setScrollBarEnabled(
        jsonBool(v, "scrollBarEnabled", sv->isScrollBarEnabled()));
    if (sv->isScrollBarEnabled()) {
        sv->setScrollBarAutoHideEnabled(jsonBool(v, "scrollBarAutoHide", true));
        sv->setScrollBarAutoHideTime(jsonFloat(v, "scrollBarAutoHideTime", 0.2f));
    }
    // 포맷에 touchEnabled 가 없다. csb 리더는 그 값을 읽어서 넣어 주지만
    // 여기서는 켤 수밖에 없다 - 끌 수 없는 스크롤뷰는 쓸모가 없다.
    sv->setTouchEnabled(true);
}

struct ResponsiveGeometry {
    Size size;
    Vec2 position;
};

// layout 수치는 원래 부모 크기를 기준으로 저장된 가장자리 여백이다.
// AnchorPoint는 노드 안에서 피벗이 어디인지이고, layout의 Left/Right 등은
// 부모 크기가 달라졌을 때 어느 가장자리를 붙잡을지이므로 서로 별개다.
static ResponsiveGeometry resolveResponsiveGeometry(
    const rapidjson::Value& v, const Size& parentDesign,
    const Size& parentActual, bool applyResponsive) {
    const float width = jsonFloat(v, "width", 0.0f);
    const float height = jsonFloat(v, "height", 0.0f);
    const float x = jsonFloat(v, "x", 0.0f);
    const float y = jsonFloat(v, "y", 0.0f);
    const float anchorX = jsonFloat(v, "anchorX", 0.5f);
    const float anchorY = jsonFloat(v, "anchorY", 0.5f);

    ResponsiveGeometry result{Size(width, height), Vec2(x, y)};
    if (!applyResponsive || !v.HasMember("layout") ||
        !v["layout"].IsObject()) {
        return result;
    }

    const rapidjson::Value& layout = v["layout"];
    const float defaultLeft = x - anchorX * width;
    const float defaultRight =
        parentDesign.width - (x + (1.0f - anchorX) * width);
    const float defaultBottom = y - anchorY * height;
    const float defaultTop =
        parentDesign.height - (y + (1.0f - anchorY) * height);
    // centerX/centerY 는 "부모 한가운데에서 이 노드의 한가운데까지의 거리" 다.
    // 노드의 한가운데는 위치가 아니라 위치 + (0.5 - 앵커)*크기 다. 다른 규칙
    // (Left/Right/Top/Bottom/Stretch)은 전부 앵커를 셈에 넣는데 Center 만
    // 빠져 있었다.
    const float defaultCenterX =
        (x + (0.5f - anchorX) * width) - parentDesign.width * 0.5f;
    const float defaultCenterY =
        (y + (0.5f - anchorY) * height) - parentDesign.height * 0.5f;

    const float left = jsonFloat(layout, "left", defaultLeft);
    const float right = jsonFloat(layout, "right", defaultRight);
    const float bottom = jsonFloat(layout, "bottom", defaultBottom);
    const float top = jsonFloat(layout, "top", defaultTop);
    const float centerX = jsonFloat(layout, "centerX", defaultCenterX);
    const float centerY = jsonFloat(layout, "centerY", defaultCenterY);
    const std::string horizontal = jsonStr(layout, "horizontal");
    const std::string vertical = jsonStr(layout, "vertical");

    if (horizontal == "Right") {
        result.position.x =
            parentActual.width - right - (1.0f - anchorX) * result.size.width;
    } else if (horizontal == "Center") {
        result.position.x = parentActual.width * 0.5f + centerX -
                            (0.5f - anchorX) * result.size.width;
    } else if (horizontal == "Stretch") {
        result.size.width = std::max(0.0f, parentActual.width - left - right);
        result.position.x = left + anchorX * result.size.width;
    } else if (horizontal == "Left") {
        result.position.x = left + anchorX * result.size.width;
    }

    if (vertical == "Top") {
        result.position.y =
            parentActual.height - top - (1.0f - anchorY) * result.size.height;
    } else if (vertical == "Center") {
        result.position.y = parentActual.height * 0.5f + centerY -
                            (0.5f - anchorY) * result.size.height;
    } else if (vertical == "Stretch") {
        result.size.height = std::max(0.0f, parentActual.height - top - bottom);
        result.position.y = bottom + anchorY * result.size.height;
    } else if (vertical == "Bottom") {
        result.position.y = bottom + anchorY * result.size.height;
    }
    return result;
}

static Node* createOne(const rapidjson::Value& v, bool applyResponsive,
                       const Size& parentDesign, const Size& parentActual,
                       const Size* forcedSize = nullptr);

// 자식을 순서대로 붙인다. ScrollView 는 addChild 가 안쪽 컨테이너로 넘어가므로
// 여기서 따로 할 일이 없다.
static void addChildren(Node* parent, const rapidjson::Value& v,
                        bool applyResponsive, const Size& parentDesign,
                        const Size& parentActual) {
    if (!v.HasMember("children") || !v["children"].IsArray()) {
        return;
    }
    const rapidjson::Value& kids = v["children"];
    for (rapidjson::SizeType i = 0; i < kids.Size(); i++) {
        Node* child = createOne(kids[i], applyResponsive, parentDesign,
                                parentActual);
        if (child != nullptr) {
            parent->addChild(child);
        }
    }
}

static Node* createOne(const rapidjson::Value& v, bool applyResponsive,
                       const Size& parentDesign, const Size& parentActual,
                       const Size* forcedSize) {
    std::string type = jsonStr(v, "type");
    std::string name = jsonStr(v, "name");
    const Size designSize(jsonFloat(v, "width", 0),
                          jsonFloat(v, "height", 0));
    ResponsiveGeometry geometry = resolveResponsiveGeometry(
        v, parentDesign, parentActual, applyResponsive);
    if (forcedSize != nullptr) {
        geometry.size = *forcedSize;
    }

    Node* node = nullptr;
    Widget* widget = nullptr;

    if (type == "ImageView") {
        JsonImageView* img = JsonImageView::create();
        img->setSliceScale(jsonFloat(v, "sliceScale", 1.0f));
        std::string tex = jsonStr(v, "texture");
        if (!tex.empty()) {
            img->loadTexture(tex);
        }
        applyScale9(img, v);
        node = widget = img;
    } else if (type == "Button") {
        JsonButton* btn = JsonButton::create();
        btn->setSliceScale(jsonFloat(v, "sliceScale", 1.0f));
        std::string tex = jsonStr(v, "texture");
        if (!tex.empty()) {
            // 눌렸을 때 보여줄 그림은 json 의 pressedTexture 다. 그 항목이 없는
            // 버튼은 평소 그림을 눌린 상태에도 그대로 쓴다. 빈 문자열을 넣으면
            // Button 이 그림 대신 확대 연출로 빠진다(Button::onPressStateChangedToPressed).
            std::string pressed = jsonStr(v, "pressedTexture");
            std::string disabled = jsonStr(v, "disabledTexture");
            btn->loadTextures(tex, pressed.empty() ? tex : pressed, disabled);
        }
        applyScale9(btn, v);
        btn->setTitleText(jsonStr(v, "text"));
        btn->setTitleFontSize(jsonFloat(v, "fontSize", 20.0f));
        std::string buttonFont = jsonStr(v, "fontPath");
        if (buttonFont.empty()) {
            buttonFont = jsonStr(v, "fontName");
        }
        if (!buttonFont.empty()) {
            btn->setTitleFontName(buttonFont);
        }
        std::string titleColor = jsonStr(v, "textColor");
        if (titleColor.empty()) {
            titleColor = jsonStr(v, "color");
        }
        btn->setTitleColor(parseColor(titleColor));
        node = widget = btn;
    } else if (type == "Text") {
        std::string fontPath = jsonStr(v, "fontPath");
        if (fontPath.empty()) {
            fontPath = jsonStr(v, "fontName");
        }
        float fontSize = jsonFloat(v, "fontSize", 20.0f);
        Text* txt = Text::create(jsonStr(v, "text"), fontPath, fontSize);
        // Text 는 색이 setColor 가 아니라 setTextColor 로 들어가야 한다.
        std::string textColor = jsonStr(v, "textColor");
        if (textColor.empty()) {
            textColor = jsonStr(v, "color");
        }
        txt->setTextColor(Color4B(parseColor(textColor), 255));
        if (v.HasMember("hAlignment")) {
            txt->setTextHorizontalAlignment((TextHAlignment)jsonInt(v, "hAlignment", 0));
        }
        if (v.HasMember("vAlignment")) {
            txt->setTextVerticalAlignment((TextVAlignment)jsonInt(v, "vAlignment", 0));
        }
        node = widget = txt;
    } else if (type == "ScrollView") {
        // 스크롤 값들은 콘텐츠 크기가 정해진 뒤에 넣는다(아래 applyScrollView).
        node = widget = ScrollView::create();
    } else if (type == "ListView") {
        node = widget = ListView::create();
    } else if (type == "PageView") {
        node = widget = PageView::create();
    } else if (type == "Panel" || type == "Layout") {
        // 옛 export 는 csb 의 ScrollView 를 Panel 로 떨궜다. 그 파일들
        // (MapCreator.ui.json)을 위해 이름으로 가려내는 길을 남겨 둔다.
        if (name == "scrollView") {
            ScrollView* sv = ScrollView::create();
            sv->setDirection(ScrollView::Direction::VERTICAL);
            sv->setBounceEnabled(true);
            node = widget = sv;
        } else {
            Layout* layout = Layout::create();
            node = widget = layout;
        }
    } else {
        // 모르는 타입은 빈 노드로 두고 자식은 살린다. 화면이 통째로 사라지는 것보다
        // 낫고, 로그를 보면 무엇을 더 만들어야 하는지 알 수 있다.
        log("[UiJsonLoader] 모르는 타입 '%s' (name=%s)", type.c_str(), name.c_str());
        node = Node::create();
    }

    node->setName(name);

    if (widget != nullptr) {
        // JSON 의 width/height 는 모든 Widget 의 실제 정렬 영역이다. Widget 은
        // 기본 autoSize 상태에서 setContentSize() 값을 버리고 가상 렌더러 크기를
        // 쓰므로 먼저 꺼야 한다. Text 도 예외가 아니다. Text 만 autoSize 로 두면
        // hAlignment/vAlignment 값은 읽혀도 영역이 글자 크기로 줄어 정렬 효과가
        // 사라진다.
        widget->setAutoSize(false);
        widget->setContentSize(geometry.size);
        // ImageView/Button 의 색은 틴트다. Text 는 위에서 이미 넣었다.
        if (dynamic_cast<Text*>(widget) == nullptr) {
            widget->setColor(parseColor(jsonStr(v, "color")));
        }
        if (ScrollView* sv = dynamic_cast<ScrollView*>(widget)) {
            applyScrollView(sv, v);
        }
        if (Layout* layout = dynamic_cast<Layout*>(widget)) {
            applyLayout(layout, v);
        }
        if (ListView* list = dynamic_cast<ListView*>(widget)) {
            list->setItemsMargin(jsonFloat(v, "itemMargin", 0.0f));
            const std::string gravity = jsonStr(v, "gravity");
            if (gravity == "RIGHT") {
                list->setGravity(ListView::Gravity::RIGHT);
            } else if (gravity == "CENTER_HORIZONTAL") {
                list->setGravity(ListView::Gravity::CENTER_HORIZONTAL);
            } else if (gravity == "TOP") {
                list->setGravity(ListView::Gravity::TOP);
            } else if (gravity == "BOTTOM") {
                list->setGravity(ListView::Gravity::BOTTOM);
            } else if (gravity == "CENTER_VERTICAL") {
                list->setGravity(ListView::Gravity::CENTER_VERTICAL);
            } else {
                list->setGravity(ListView::Gravity::LEFT);
            }
        }
    } else {
        node->setContentSize(geometry.size);
    }

    node->setAnchorPoint(Vec2(jsonFloat(v, "anchorX", 0.5f), jsonFloat(v, "anchorY", 0.5f)));
    node->setPosition(geometry.position);
    node->setScaleX(jsonFloat(v, "scaleX", 1));
    node->setScaleY(jsonFloat(v, "scaleY", 1));
    node->setRotation(jsonFloat(v, "rotation", 0));
    node->setLocalZOrder(jsonInt(v, "zOrder", 0));
    node->setTag(jsonInt(v, "tag", 0));
    node->setVisible(jsonBool(v, "visible", true));
    node->setCascadeOpacityEnabled(false);
    node->setOpacity((GLubyte)jsonInt(v, "opacity", 255));

    addChildren(node, v, applyResponsive, designSize, geometry.size);
    return node;
}

static Node* createNodeFromFile(const std::string& fileName,
                                const Size* targetSize) {
    std::string text = FileUtils::getInstance()->getStringFromFile(fileName);
    if (text.empty()) {
        log("[UiJsonLoader] %s 를 읽지 못했다", fileName.c_str());
        return nullptr;
    }
    rapidjson::Document doc;
    doc.Parse<0>(text.c_str());
    if (doc.HasParseError() || !doc.IsObject()) {
        log("[UiJsonLoader] %s 파싱 실패", fileName.c_str());
        return nullptr;
    }
    if (jsonStr(doc, "format") != "CartoonCraftUI") {
        log("[UiJsonLoader] %s 의 format 이 CartoonCraftUI가 아니다",
            fileName.c_str());
        return nullptr;
    }
    if (doc.HasMember("decoderVersion") &&
        jsonInt(doc, "decoderVersion", 0) != 7) {
        log("[UiJsonLoader] %s 의 decoderVersion을 지원하지 않는다",
            fileName.c_str());
        return nullptr;
    }
    if (!doc.HasMember("root") || !doc["root"].IsObject()) {
        log("[UiJsonLoader] %s 에 root 가 없다", fileName.c_str());
        return nullptr;
    }
    Size designSize(jsonFloat(doc["root"], "width", 0.0f),
                    jsonFloat(doc["root"], "height", 0.0f));
    Size rootSize = targetSize != nullptr ? *targetSize : designSize;
    if (rootSize.width <= 0.0f || rootSize.height <= 0.0f) {
        rootSize = designSize;
    }

    Node* root = createOne(doc["root"], targetSize != nullptr, Size::zero,
                           Size::zero, &rootSize);
    if (root == nullptr) {
        return nullptr;
    }
    return root;
}

Node* UiJsonLoader::createNode(const std::string& fileName) {
    return createNodeFromFile(fileName, nullptr);
}

Node* UiJsonLoader::createNode(const std::string& fileName,
                               const Size& targetSize) {
    return createNodeFromFile(fileName, &targetSize);
}
