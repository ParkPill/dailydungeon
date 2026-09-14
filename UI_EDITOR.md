# DailyDungeon UI workflow

DailyDungeon follows CartoonCraft's incremental UI migration:

- Existing screens keep using their checked-in Cocos Studio `.csb` files.
  Axmol's `sceneio` and `sceneext` extensions are enabled for this legacy
  content.
- New or redesigned screens must be exported by the UI editor as
  `CartoonCraftUI` version 1 (`*.ui.json`) and placed under `Content/res`.
- Load new layouts with `UiJsonLoader`, never `CSLoader`:

```cpp
#include "UiJsonLoader.h"

auto panel = UiJsonLoader::createNode("MyScreen.ui.json");
// Use this overload when the root should fill the current screen and honor
// the editor's Left/Right/Center/Stretch responsive rules.
auto fullScreenPanel = UiJsonLoader::createNode(
    "MyScreen.ui.json", ax::Director::getInstance()->getVisibleSize());
```

The loader supports `Panel`, `Layout`, `ImageView`, `Button`, `Text`,
`ScrollView`, `ListView`, and `PageView`; it also preserves names, z-order,
anchors, opacity, tint, Scale9 cap insets, and responsive layout metadata.

Do not replace a `.csb` call site until its matching `.ui.json` export has
been added and its child names have been checked.  Existing gameplay code
relies heavily on `getChildByName`, so node names are part of the runtime API.
