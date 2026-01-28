# CraftingScene Display Issue Analysis

## Problem Statement
When running the game and switching to the CraftingMenu, nothing is displayed on the screen.

## Root Cause

### The Issue
The `scene_texture.display()` call was embedded inside `UIRenderLogic::ProcessLogic()`, which meant it was being called in the middle of the render cycle when multiple render logic classes exist. This caused content drawn by subsequent render logic classes to not be properly displayed.

### Architectural Problem

**Original (broken) flow:**
1. `CraftingScene::sRender()` calls `clear()`
2. `UIRenderLogic` draws UI and calls `display()` - texture is finalized
3. `CraftingRenderLogic` draws machina forms - goes to new buffer that hasn't been displayed
4. Frame ends, DisplayManager shows only what was finalized (just UI, but even that may not appear correctly)
5. Next frame `clear()` wipes out the machina forms that were never displayed

**Key insight:** SFML's `RenderTexture::display()` updates the texture from the draw buffer. When called in the middle of rendering, subsequent draws go to a new buffer that won't be displayed until the next `display()` call. Since the next operation is `clear()` (start of next frame), content drawn after `display()` is lost.

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

## Recommended Solution (Implemented)

### Move display() Call to Scene Level

**The proper architectural fix:**

Remove `display()` from individual render logic classes and add it to each Scene's `sRender()` method after all render logic has been processed.

**Benefits:**
- Single clear at start, single display at end of render cycle
- Order of render logic classes doesn't affect display (only layering)
- More predictable and maintainable
- Follows principle: Scene controls the render cycle, Logic classes just draw

**Implementation:**

```cpp
// UIRenderLogic.cpp - Remove display() call
void UIRenderLogic::ProcessLogic() {
  DrawUIElements();
  // REMOVED: m_scene_context.scene_texture.display();
}

// CraftingScene.cpp - Add display() call at end
void CraftingScene::sRender() {
  m_scene_resources.scene_texture.clear(sf::Color::Black);
  
  for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {
    render_logic->RunLogic();
  }
  
  m_scene_resources.scene_texture.display();  // Add this
}

// TitleScene.cpp - Add display() call at end
void TitleScene::sRender() {
  m_scene_resources.scene_texture.clear(sf::Color::Black);
  
  for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {
    render_logic->RunLogic();
  }
  
  m_scene_resources.scene_texture.display();  // Add this
}
```

This ensures:
1. All drawing happens between `clear()` and `display()`
2. Order of render logics only affects layering (draw order)
3. Each Scene controls its own render cycle timing

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
For any scene with render Logic classes:
1. Call `clear()` at the start of `sRender()`
2. Run all render Logic classes (they only draw, don't display)
3. Call `display()` at the end of `sRender()`
4. Order of render Logic classes only affects layering (what's drawn on top)

**Pattern to follow:**
```cpp
void MyScene::sRender() {
  m_scene_resources.scene_texture.clear(sf::Color::Black);
  
  for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {
    render_logic->RunLogic();
  }
  
  m_scene_resources.scene_texture.display();
}
```
