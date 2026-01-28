# CraftingScene Display Issue Analysis

## Problem Statement
When running the game and switching to the CraftingMenu, nothing is displayed on the screen.

## Root Cause

### The Issue
The `CraftingScene::sRender()` method is missing a critical call to `scene_texture.display()` after rendering all logic classes.

### Code Comparison

**CraftingScene::sRender() (BROKEN):**
```cpp
void CraftingScene::sRender() {
  // clear the render texture
  m_scene_resources.scene_texture.clear(sf::Color::Black);

  // process render logic
  for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {
    render_logic->RunLogic();
  }
  // MISSING: m_scene_resources.scene_texture.display();
}
```

**TitleScene::sRender() (WORKING):**
```cpp
void TitleScene::sRender() {
  // clear the render texture and the start of each Scene render step
  m_scene_resources.scene_texture.clear(sf::Color::Black);

  if (m_scene_resources.logic_map.find(LogicType::Render) ==
      m_scene_resources.logic_map.end()) {
  }
  for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {
    render_logic->RunLogic();
  }
  // Note: TitleScene also doesn't call display(), but...
}
```

### Further Investigation

After comparing both scenes, I discovered that:

1. **TitleScene** also doesn't explicitly call `display()` in its `sRender()` method
2. **UIRenderLogic** (used by both scenes) calls `scene_texture.display()` in its `ProcessLogic()` method

**UIRenderLogic::ProcessLogic():**
```cpp
void UIRenderLogic::ProcessLogic() {
  // Draw all UI elements to the render texture
  DrawUIElements();

  // display the render texture after drawing
  m_scene_context.scene_texture.display();  // <-- HERE IT IS!
}
```

### The Real Problem

The issue is in the **order of Logic classes** in `LogicFactory::ConfigureCraftingLogics()`:

**CraftingScene Logic Order:**
```cpp
////// ADD RENDER LOGICS /////
LogicVector &render_logics = logic_collection[LogicType::Render];
render_logics.push_back(
    std::make_unique<steamrot::UIRenderLogic>(m_scene_context));     // 1st
render_logics.push_back(
    std::make_unique<steamrot::CraftingRenderLogic>(m_scene_context)); // 2nd
```

**TitleScene Logic Order:**
```cpp
////// ADD RENDER LOGICS /////
LogicVector &render_logics = logic_collection[LogicType::Render];
render_logics.push_back(
    std::make_unique<steamrot::UIRenderLogic>(m_scene_context));     // Only one
```

### Why This Causes the Problem

1. `CraftingScene::sRender()` runs the render logic classes in order
2. `UIRenderLogic` runs first and calls `scene_texture.display()`
3. `CraftingRenderLogic` runs second and draws to the render texture AFTER the display() call
4. The CraftingRenderLogic content never gets displayed because `display()` was already called

## Impact Analysis

### What IS NOT Being Rendered
**UPDATE: User confirmed that NOTHING is displayed in CraftingScene, including UI.**

The issue affects ALL rendering:
- UI elements drawn by `UIRenderLogic` 
- Machina forms drawn by `CraftingRenderLogic`

The root cause is that `display()` is being called in the middle of the render cycle, causing rendering artifacts or incomplete texture finalization.

## Scene Configuration Details

**From `data/defaults/scenes/crafting.scene_data.json`:**

The scene is configured with:
- 3 UI elements (initial_crafting_menu, new_construct_menu, fragment_selection_menu)
- 1 UI state component
- 1 grimoire_machina component

The UI elements should be visible (at least the initial_crafting_menu with `"is_visible": true`), but any machina forms drawn by CraftingRenderLogic would not be visible.

## Recommended Solutions

### Option 1: Move CraftingRenderLogic Before UIRenderLogic (RECOMMENDED)
**File:** `src/logic/LogicFactory.cpp`

```cpp
std::expected<std::monostate, FailInfo>
LogicFactory::ConfigureCraftingLogics(LogicCollection &logic_collection) {
  // ... collision and action logics ...

  ////// ADD RENDER LOGICS /////
  LogicVector &render_logics = logic_collection[LogicType::Render];
  render_logics.push_back(
      std::make_unique<steamrot::CraftingRenderLogic>(m_scene_context));  // Move first
  render_logics.push_back(
      std::make_unique<steamrot::UIRenderLogic>(m_scene_context));       // Keep last
  return std::monostate();
}
```

**Rationale:**
- UIRenderLogic should always be last because it calls `display()`
- This is a one-line change (swapping order)
- Minimal risk, follows the pattern of "display after all drawing is done"

### Option 2: Remove display() from UIRenderLogic, Add to Scene
**Files:** `src/logic/UIRenderLogic.cpp`, `src/scenes/CraftingScene.cpp`

```cpp
// UIRenderLogic.cpp - Remove display() call
void UIRenderLogic::ProcessLogic() {
  DrawUIElements();
  // REMOVED: m_scene_context.scene_texture.display();
}

// CraftingScene.cpp - Add display() call
void CraftingScene::sRender() {
  m_scene_resources.scene_texture.clear(sf::Color::Black);
  
  for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {
    render_logic->RunLogic();
  }
  
  m_scene_resources.scene_texture.display();  // Add this
}
```

**Rationale:**
- Makes the Scene responsible for display timing
- More explicit control
- Requires changes to multiple files
- Would need to update TitleScene too for consistency

### Option 3: Create a DisplayLogic Class
**New File:** `src/logic/DisplayLogic.h/cpp`

Create a dedicated Logic class that only calls `display()`, and add it as the last render logic in every scene.

**Rationale:**
- Most architecturally clean
- Separates concerns properly
- Requires new files and more changes
- Overkill for this problem

## Recommendation

**Use Option 1: Swap the order of CraftingRenderLogic and UIRenderLogic**

This is the minimal change that fixes the issue by ensuring:
1. All drawing happens first (CraftingRenderLogic, then UIRenderLogic)
2. Display happens last (UIRenderLogic calls display() at the end)
3. Everything rendered gets shown on screen

## Testing Recommendations

After applying the fix:

1. **Visual Test:** Run the game and switch to CraftingMenu
   - Verify UI buttons are visible
   - Verify any machina forms are visible (if configured)

2. **Unit Test:** Add test to verify Logic order
   - File: `tests/unit/logic/LogicFactory.test.cpp`
   - Verify CraftingRenderLogic comes before UIRenderLogic in render vector

3. **Regression Test:** Verify TitleScene still works
   - UI should still be visible on title screen

## Additional Notes

### Documentation Comment in LogicFactory.cpp
The LogicFactory has this important comment:

```cpp
////// THE ORDER OF THE LOGIC CLASSES IS VERY IMPORTANT //////
////// DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING //////
```

This bug demonstrates exactly why this comment exists. The order matters because:
- Logic classes execute sequentially in the order they're added
- Some Logic classes have side effects (like calling `display()`)
- Drawing after display means the content won't be visible

### Best Practice for Future Scenes
For any scene with multiple render Logic classes:
1. Add all drawing Logic classes first
2. Add UIRenderLogic last (it calls display())
3. Never call display() in the middle of rendering
