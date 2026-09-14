#pragma once

// Transitional Cocos2d-x 3.x compatibility used only by Cartoon Craft.
// Keep this layer in the game project so the Axmol engine checkout remains
// pristine and legacy call sites can be migrated in small, reviewable groups.
// APIs already provided by Axmol keep their current names in game code:
//   Vec2::zero (not Vec2::ZERO), Widget::setAutoSize(bool)
//   (not ignoreContentAdaptWithSize(bool)). Do not #define ZERO: it would also
//   rewrite unrelated engine/SDK identifiers. tools/test_axmol_api_compatibility.py
//   checks game sources for these unsupported legacy spellings without a build.
#include "axmol/cocos2d.h"
#include "axmol/2d/FastTMXLayer.h"
#include "axmol/2d/FastTMXTiledMap.h"
#include "axmol/audio/AudioEngine.h"
#include "axmol/base/PointerEvent.h"
#include "axmol/base/PointerEventListener.h"
#include "axmol/base/text_utils.h"
#include "axmol/ui/Widget.h"

#include <cstdarg>
#include <cstdio>
#include <deque>
#include <map>
#include <string>
#include <vector>

namespace ax
{

class Touch
{
public:
    explicit Touch(PointerEvent* event) : _event(event) {}

    // Cocos2d-x Touch::getLocation() is a design-resolution canvas point
    // (bottom-left origin), not the camera-dependent 3D world hit point.
    // Fixed-priority listeners deliberately have no input camera in Axmol, so
    // getWorldPoint() becomes (0, 0) there and every editor click/drag jumps to
    // the map's lower-left corner.
    Vec2 getLocation() const { return Director::getInstance()->screenToCanvas(_event->getPoint()); }
    Vec2 getLocationInView() const { return _event->getPoint(); }
    intptr_t getID() const { return _event->getPointerId(); }
    float getCurrentForce() const { return _event->getPressure(); }

private:
    PointerEvent* _event;
};

// 데스크톱에서 레거시 터치 콜백이 한 번도 불리지 않던 이유가 여기 있었다.
//
// 이 프로젝트의 화면 조작(미니맵 클릭, 유닛 선택, 브러시 칠하기 등)은 전부
// cocos2d-x 시절의 onTouches* 안에 들어 있다. axmol 3.x 는 터치/마우스를
// PointerEvent 하나로 합쳤는데, 아래 셈이 PointerType::Touch 가 아닌 입력을
// 걸러내는 바람에 윈도우 마우스(PointerType::Mouse)는 전부 버려졌다.
// UI 가 멀쩡했던 것은 ui::Widget 이 axmol 의 PointerEventListener 를 직접
// 쓰기 때문이다.
//
// 그래서 마우스 왼쪽 버튼을 손가락 하나로 본다. 펜도 같이 받는다.
inline bool isTouchLikePointer(PointerEvent* event)
{
    switch (event->getPointerType())
    {
    case PointerType::Touch:
    case PointerType::Pen:
        return true;
    case PointerType::Mouse:
        // 오른쪽/가운데 버튼은 터치에 대응하는 것이 없다. 그쪽은
        // EventListenerMouse 를 쓰는 곳이 따로 처리한다.
        return event->getButton() == InputButton::Left;
    default:
        return false;
    }
}

// 누른 뒤의 이동/뗌은 axmol 이 "캡처된" 리스너에게만 보낸다. 캡처는
// onPointerDown 이 true 를 돌려줘야 걸린다(EventDispatcher::
// dispatchUncapturedPointerEvent). 그리고 끄는 중의 이벤트에는 버튼 정보가
// 없으므로(InputButton::None), 이동/뗌에서 다시 버튼을 보면 안 된다.
// 눌렸는지는 리스너가 스스로 기억한다.

class EventListenerTouchOneByOne : public PointerEventListener
{
public:
    std::function<bool(Touch*, Event*)> onTouchBegan;
    std::function<void(Touch*, Event*)> onTouchMoved;
    std::function<void(Touch*, Event*)> onTouchEnded;
    std::function<void(Touch*, Event*)> onTouchCancelled;

    static EventListenerTouchOneByOne* create()
    {
        auto* listener = new EventListenerTouchOneByOne();
        if (!listener->init())
        {
            delete listener;
            return nullptr;
        }
        listener->installLegacyCallbacks();
        listener->autorelease();
        return listener;
    }

    void setSwallowTouches(bool swallow) noexcept { _swallowTouches = swallow; }
    bool isSwallowTouches() const noexcept { return _swallowTouches; }

private:
    void installLegacyCallbacks()
    {
        onPointerDown = [this](PointerEvent* event) {
            if (!isTouchLikePointer(event))
                return false;
            Touch touch(event);
            // onTouchBegan 이 false 면 그 뒤 이동/뗌을 받지 않는다는 뜻이다.
            // 캡처도 걸지 않아야 다음 리스너에게 넘어간다.
            const bool began = onTouchBegan ? onTouchBegan(&touch, event) : true;
            _pressed = began;
            return began;
        };
        onPointerMove = [this](PointerEvent* event) {
            if (!_pressed)
                return;
            Touch touch(event);
            if (onTouchMoved)
                onTouchMoved(&touch, event);
        };
        onPointerUp = [this](PointerEvent* event) {
            if (!_pressed)
                return;
            _pressed = false;
            Touch touch(event);
            if (onTouchEnded)
                onTouchEnded(&touch, event);
        };
        onPointerCancel = [this](PointerEvent* event) {
            if (!_pressed)
                return;
            _pressed = false;
            Touch touch(event);
            if (onTouchCancelled)
                onTouchCancelled(&touch, event);
        };
    }

    bool _swallowTouches{false};
    bool _pressed{false};
};

class EventListenerTouchAllAtOnce : public PointerEventListener
{
public:
    using Touches = std::vector<Touch*>;
    std::function<void(const Touches&, Event*)> onTouchesBegan;
    std::function<void(const Touches&, Event*)> onTouchesMoved;
    std::function<void(const Touches&, Event*)> onTouchesEnded;
    std::function<void(const Touches&, Event*)> onTouchesCancelled;

    static EventListenerTouchAllAtOnce* create()
    {
        auto* listener = new EventListenerTouchAllAtOnce();
        if (!listener->init())
        {
            delete listener;
            return nullptr;
        }
        listener->installLegacyCallbacks();
        listener->autorelease();
        return listener;
    }

private:
    void installLegacyCallbacks()
    {
        onPointerDown = [this](PointerEvent* event) {
            if (!isTouchLikePointer(event))
                return false;
            _pressed = true;
            Touch touch(event);
            if (onTouchesBegan)
                onTouchesBegan(Touches{&touch}, event);
            // true 를 돌려줘야 이 포인터가 캡처되어 이동/뗌이 이어서 온다.
            return true;
        };
        onPointerMove = [this](PointerEvent* event) {
            if (!_pressed)
                return;
            Touch touch(event);
            if (onTouchesMoved)
                onTouchesMoved(Touches{&touch}, event);
        };
        onPointerUp = [this](PointerEvent* event) {
            if (!_pressed)
                return;
            _pressed = false;
            Touch touch(event);
            if (onTouchesEnded)
                onTouchesEnded(Touches{&touch}, event);
        };
        onPointerCancel = [this](PointerEvent* event) {
            if (!_pressed)
                return;
            _pressed = false;
            Touch touch(event);
            if (onTouchesCancelled)
                onTouchesCancelled(Touches{&touch}, event);
        };
    }

    bool _pressed{false};
};

class EventMouse
{
public:
    explicit EventMouse(PointerEvent* event) : _event(event) {}

    int getMouseButton() const { return _event->getButton(); }
    // Axmol exposes PointerEvent::getPoint() in native screen coordinates
    // (top-left origin, before design-resolution scaling). Cocos2d-x's
    // EventMouse::getLocationInView() call sites in this project expect canvas
    // coordinates (bottom-left origin, after viewport/scaling conversion).
    Vec2 getLocationInView() const
    {
        return Director::getInstance()->screenToCanvas(_event->getPoint());
    }
    float getScrollX() const { return _event->getScrollX(); }
    float getScrollY() const { return _event->getScrollY(); }
    void stopPropagation() { _event->stopPropagation(); }
    // 데스크톱에서는 EventListenerTouchAllAtOnce 가 한 번도 불리지 않는다
    // (윈도우 마우스는 PointerType::Mouse 라 위의 터치 셈이 걸러낸다).
    // 마우스 콜백에서 같은 PointerEvent 로 Touch 를 만들어 터치 쪽 처리를
    // 그대로 태우려면 원본 이벤트가 필요하다.
    // 끄는 중의 이벤트에는 getMouseButton() 이 InputButton::None 이다.
    // 무엇이 눌려 있는지는 이 비트로 물어야 한다.
    uint32_t getPressedButtons() const { return _event->getPressedButtons(); }
    PointerEvent* getPointerEvent() const { return _event; }
    bool isButtonPressed(int button) const
    {
        return (_event->getPressedButtons() & (1u << button)) != 0;
    }

private:
    PointerEvent* _event;
};

class EventListenerMouse : public PointerEventListener
{
public:
    std::function<void(EventMouse*)> onMouseDown;
    std::function<void(EventMouse*)> onMouseMove;
    std::function<void(EventMouse*)> onMouseUp;
    std::function<void(EventMouse*)> onMouseScroll;

    static EventListenerMouse* create()
    {
        auto* listener = new EventListenerMouse();
        if (!listener->init())
        {
            delete listener;
            return nullptr;
        }
        listener->installLegacyCallbacks();
        listener->autorelease();
        return listener;
    }

private:
    void installLegacyCallbacks()
    {
        onPointerDown = [this](PointerEvent* event) {
            if (event->getPointerType() != PointerType::Mouse)
                return false;
            // A listener used only for wheel scrolling must not capture every
            // mouse press. Axmol stops dispatching a captured pointer before
            // scene-graph UI widgets (such as title-screen Buttons) see it.
            if (!onMouseDown)
                return false;
            EventMouse mouse(event);
            onMouseDown(&mouse);
            return true;
        };
        onPointerMove = [this](PointerEvent* event) {
            if (event->getPointerType() != PointerType::Mouse)
                return;
            EventMouse mouse(event);
            if (onMouseMove)
                onMouseMove(&mouse);
        };
        onPointerUp = [this](PointerEvent* event) {
            if (event->getPointerType() != PointerType::Mouse)
                return;
            EventMouse mouse(event);
            if (onMouseUp)
                onMouseUp(&mouse);
        };
        onPointerScroll = [this](PointerEvent* event) {
            if (event->getPointerType() != PointerType::Mouse)
                return false;
            if (!onMouseScroll)
                return false;
            EventMouse mouse(event);
            onMouseScroll(&mouse);
            // 콜백이 stopPropagation() 을 부른 것만 "내가 먹었다" 로 친다.
            // 여기서 무조건 true 를 돌려주면 EventDispatcher 가 그 자리에서
            // 배달을 멈추기 때문에(dispatchPointerEvent 의 captured), 맨 앞에
            // 달린 리스너 하나가 휠을 통째로 삼킨다. 실제로 GameManager 의
            // 목록 스크롤 리스너(우선순위 -1)가 커서 밑에 ScrollView 가
            // 없을 때도 휠을 먹어서, 맵 확대(EditorWorld/HelloWorld 의
            // onMouseScroll, 우선순위 1)가 한 번도 불리지 않았다.
            return event->isStopped();
        };
    }
};

namespace experimental
{
using AudioEngine = ::ax::AudioEngine;
using AudioProfile = ::ax::AudioProfile;
using TMXLayer = ::ax::TMXLayer;
using TMXTiledMap = ::ax::TMXTiledMap;
}  // namespace experimental

namespace ui
{
using TouchEventType = Widget::TouchEventType;
}

struct Color3B : public Color32
{
    constexpr Color3B() noexcept = default;
    constexpr Color3B(uint8_t r, uint8_t g, uint8_t b) noexcept : Color32(r, g, b) {}
    constexpr Color3B(const Color32& color) noexcept : Color32(color) {}

    static const Color3B WHITE;
    static const Color3B YELLOW;
    static const Color3B BLUE;
    static const Color3B GREEN;
    static const Color3B RED;
    static const Color3B BLACK;
    static const Color3B ORANGE;
    static const Color3B GRAY;
};

inline constexpr Color3B Color3B::WHITE{255, 255, 255};
inline constexpr Color3B Color3B::YELLOW{255, 255, 0};
inline constexpr Color3B Color3B::BLUE{0, 0, 255};
inline constexpr Color3B Color3B::GREEN{0, 255, 0};
inline constexpr Color3B Color3B::RED{255, 0, 0};
inline constexpr Color3B Color3B::BLACK{0, 0, 0};
inline constexpr Color3B Color3B::ORANGE{255, 127, 0};
inline constexpr Color3B Color3B::GRAY{166, 166, 166};

struct Color4B : public Color32
{
    constexpr Color4B() noexcept = default;
    constexpr Color4B(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept : Color32(r, g, b, a) {}
    constexpr Color4B(const Color3B& color, uint8_t alpha) noexcept : Color32(color.r, color.g, color.b, alpha) {}
    constexpr Color4B(const Color32& color, uint8_t alpha) noexcept : Color32(color.r, color.g, color.b, alpha) {}
    constexpr Color4B(const Color32& color) noexcept : Color32(color) {}

    static const Color4B WHITE;
    static const Color4B YELLOW;
    static const Color4B BLUE;
    static const Color4B GREEN;
    static const Color4B RED;
    static const Color4B BLACK;
    static const Color4B ORANGE;
    static const Color4B GRAY;
};

inline constexpr Color4B Color4B::WHITE{255, 255, 255, 255};
inline constexpr Color4B Color4B::YELLOW{255, 255, 0, 255};
inline constexpr Color4B Color4B::BLUE{0, 0, 255, 255};
inline constexpr Color4B Color4B::GREEN{0, 255, 0, 255};
inline constexpr Color4B Color4B::RED{255, 0, 0, 255};
inline constexpr Color4B Color4B::BLACK{0, 0, 0, 255};
inline constexpr Color4B Color4B::ORANGE{255, 127, 0, 255};
inline constexpr Color4B Color4B::GRAY{166, 166, 166, 255};

struct Color4F : public Color
{
    constexpr Color4F() noexcept = default;
    constexpr Color4F(float r, float g, float b, float a) noexcept : Color(r, g, b, a) {}
    constexpr Color4F(const Color32& color) noexcept : Color(color) {}
    constexpr Color4F(const Color& color) noexcept : Color(color) {}

    static const Color4F WHITE;
    static const Color4F YELLOW;
    static const Color4F BLUE;
    static const Color4F GREEN;
    static const Color4F RED;
    static const Color4F BLACK;
    static const Color4F ORANGE;
    static const Color4F GRAY;
};

inline constexpr Color4F Color4F::WHITE{1, 1, 1, 1};
inline constexpr Color4F Color4F::YELLOW{1, 1, 0, 1};
inline constexpr Color4F Color4F::BLUE{0, 0, 1, 1};
inline constexpr Color4F Color4F::GREEN{0, 1, 0, 1};
inline constexpr Color4F Color4F::RED{1, 0, 0, 1};
inline constexpr Color4F Color4F::BLACK{0, 0, 0, 1};
inline constexpr Color4F Color4F::ORANGE{1, 0.5F, 0, 1};
inline constexpr Color4F Color4F::GRAY{0.65F, 0.65F, 0.65F, 1};

class __String : public Object
{
public:
    static __String* create(const std::string& value)
    {
        auto* result = new __String(value);
        result->autorelease();
        return result;
    }

    static __String* createWithFormat(const char* format, ...)
    {
        if (format == nullptr)
            return create("");

        va_list args;
        va_start(args, format);
        va_list countArgs;
        va_copy(countArgs, args);
        const int required = std::vsnprintf(nullptr, 0, format, countArgs);
        va_end(countArgs);

        std::string value;
        if (required > 0)
        {
            std::vector<char> buffer(static_cast<size_t>(required) + 1);
            std::vsnprintf(buffer.data(), buffer.size(), format, args);
            value.assign(buffer.data(), static_cast<size_t>(required));
        }
        va_end(args);
        return create(value);
    }

    const char* getCString() const noexcept { return _value.c_str(); }

private:
    explicit __String(std::string value) : _value(std::move(value)) {}
    std::string _value;
};

using String = __String;

// Cocos2d-x 3.x exposed printf-style helpers through StringUtils. Axmol's
// text utilities intentionally use type-safe formatting, so keep the legacy
// behavior at the project boundary while old call sites are being migrated.
namespace StringUtils
{
inline std::string format(const char* format, ...)
{
    if (format == nullptr)
        return {};

    va_list args;
    va_start(args, format);
    va_list countArgs;
    va_copy(countArgs, args);
    const int required = std::vsnprintf(nullptr, 0, format, countArgs);
    va_end(countArgs);

    std::string result;
    if (required > 0)
    {
        std::vector<char> buffer(static_cast<size_t>(required) + 1);
        std::vsnprintf(buffer.data(), buffer.size(), format, args);
        result.assign(buffer.data(), static_cast<size_t>(required));
    }
    va_end(args);
    return result;
}

inline bool UTF8ToUTF16(std::string_view input, std::u16string& output)
{
    return text_utils::UTF8ToUTF16(input, output);
}

inline bool UTF16ToUTF8(std::u16string_view input, std::string& output)
{
    return text_utils::UTF16ToUTF8(input, output);
}
}  // namespace StringUtils

inline void log(const char* format, ...)
{
    if (format == nullptr)
        return;

    va_list args;
    va_start(args, format);
    va_list countArgs;
    va_copy(countArgs, args);
    const int required = std::vsnprintf(nullptr, 0, format, countArgs);
    va_end(countArgs);

    if (required > 0)
    {
        std::vector<char> buffer(static_cast<size_t>(required) + 1);
        std::vsnprintf(buffer.data(), buffer.size(), format, args);
        AXLOGD("{}", buffer.data());
    }
    va_end(args);
}

}  // namespace ax

// A few source files relied on Cocos2d-x headers importing these STL names.
// Keep narrow declarations (never `using namespace std`) to avoid colliding
// with the Windows SDK's `byte` typedef.
using std::deque;
using std::map;
using std::string;
using std::vector;

#ifndef TOUCH_EVENT_ENDED
#    define TOUCH_EVENT_ENDED ENDED
#endif

#ifndef USING_NS_CC
#    define USING_NS_CC using namespace cocos2d
#endif

#ifndef CCLOG
#    define CCLOG cocos2d::log
#endif
#ifndef CCLOGERROR
#    define CCLOGERROR cocos2d::log
#endif
#ifndef CCLOGWARN
#    define CCLOGWARN cocos2d::log
#endif
