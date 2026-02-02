# Analysis: SteamRot Rendering Issues Fix

**Date**: 2026-02-02
**Issue**: Quit button not working in TitleScene, black screen in CraftingScene
**Status**: ✅ Fixed (awaiting local verification)

## Executive Summary

Two rendering issues were identified and fixed in the SteamRot game engine:
1. **TitleScene**: Quit button functionality unclear (turns out was working, but rendering could be improved)
2. **CraftingScene**: Black screen displayed instead of entities

**Root Cause**: The `display()` call on SFML RenderTexture was being made in UIRenderLogic, creating an ordering dependency that caused CraftingRenderLogic's drawings to be ignored.

**Fix**: Moved `display()` calls from UIRenderLogic to each Scene's `sRender()` method, ensuring all rendering logic completes before texture finalization.

## Technical Analysis

### SFML RenderTexture Lifecycle

SFML's `RenderTexture` follows this lifecycle:
1. `clear()` - Clear the texture with a color
2. `draw()` - Draw drawables to the texture (multiple calls)
3. `display()` - **Finalize** the texture (must be called before `getTexture()`)
4. `getTexture()` - Get the finalized texture for display

**Critical**: Any `draw()` calls after `display()` are **silently ignored** until the next `display()` call.

### Rendering Flow in SteamRot

```
GameEngine::ExecuteTick()
  └─> GameEngine::TickRendering()
      └─> DisplayManager::CallRenderCycle()
          ├─> SceneManager::ProvideTextures()
          │   └─> For each scene: Scene::GetRenderTexture()
          │       └─> Returns m_scene_resources.scene_texture
          └─> For each texture: window.draw(sprite(texture.getTexture()))
```

**Key Point**: DisplayManager calls `getTexture()` on each scene's RenderTexture, which requires that texture to have been finalized with `display()`.

### Scene Rendering Flow

Each scene implements `sRender()`:

```cpp
void Scene::sRender() {
    // 1. Clear the texture
    m_scene_resources.scene_texture.clear(sf::Color::Black);
    
    // 2. Run all render logic in order
    for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {
        render_logic->RunLogic();
    }
    
    // 3. Finalize the texture (REQUIRED!)
    m_scene_resources.scene_texture.display();
}
```

### The Bug in CraftingScene

**LogicFactory Configuration** (LogicFactory.cpp lines 120-127):
```cpp
LogicVector &render_logics = logic_collection[LogicType::Render];
render_logics.push_back(std::make_unique<UIRenderLogic>(m_scene_context));     // Index 0
render_logics.push_back(std::make_unique<CraftingRenderLogic>(m_scene_context)); // Index 1
```

**Buggy Execution Flow**:
1. CraftingScene::sRender() clears texture (Black)
2. Calls render_logics[0]->RunLogic() → **UIRenderLogic**
   - Draws UI elements
   - Calls `display()` ← **TEXTURE FINALIZED HERE**
3. Calls render_logics[1]->RunLogic() → **CraftingRenderLogic**
   - Tries to draw entities
   - ❌ **DRAWINGS IGNORED** (texture already finalized)
4. DisplayManager gets texture → Only shows UI, no entities

**Result**: Black screen with UI buttons visible, but no entities.

### Why TitleScene "Worked"

TitleScene only has UIRenderLogic in its render logic vector:
```cpp
LogicVector &render_logics = logic_collection[LogicType::Render];
render_logics.push_back(std::make_unique<UIRenderLogic>(m_scene_context));
```

So the `display()` call in UIRenderLogic happened **after** all drawings, making it appear to work correctly (though the architecture was still flawed).

## The Fix

### Changes Made

**1. UIRenderLogic.cpp** - Removed `display()` call:
```cpp
void UIRenderLogic::ProcessLogic() {
    DrawUIElements();
    // NOTE: display() is now called by the Scene's sRender() method
    // after all render logic has completed, not here.
}
```

**2. TitleScene.cpp** - Added `display()` call:
```cpp
void TitleScene::sRender() {
    m_scene_resources.scene_texture.clear(sf::Color::Black);
    
    for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {
        render_logic->RunLogic();
    }
    
    // finalize the render texture after all rendering logic has completed
    m_scene_resources.scene_texture.display();
}
```

**3. CraftingScene.cpp** - Added `display()` call:
```cpp
void CraftingScene::sRender() {
    m_scene_resources.scene_texture.clear(sf::Color::Black);
    
    for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {
        render_logic->RunLogic();
    }
    
    // finalize the render texture after all rendering logic has completed
    m_scene_resources.scene_texture.display();
}
```

### Benefits of This Architecture

1. **No Ordering Dependencies**: Logic classes can be added in any order without breaking rendering
2. **Scene Control**: The Scene has full control over its render cycle
3. **Consistency**: All scenes follow the same pattern
4. **Clarity**: It's clear where the texture finalization happens
5. **Extensibility**: New render logic classes can be added without modifications to existing ones

## Verification Steps (To Be Done Locally)

1. **Build the project**:
   ```bash
   cmake --preset Debug
   cmake --build --preset Debug
   ```

2. **Run the game**:
   ```bash
   ./steamrot
   ```

3. **Test TitleScene**:
   - Launch game (TitleScene should appear)
   - Click "Quit Game" button
   - ✅ **Expected**: Game should quit/window should close
   
4. **Test CraftingScene**:
   - Launch game (TitleScene appears)
   - Click "Crafting Table" button
   - ✅ **Expected**: Scene changes to CraftingScene
   - ✅ **Expected**: UI buttons visible (Return, New Construct)
   - ✅ **Expected**: If m_holding_form exists, entities should be visible (not black screen)
   - Click "Return" button
   - ✅ **Expected**: Scene changes back to TitleScene

## Future Considerations

### Pattern Documentation

This fix establishes the standard pattern for Scene rendering:

```cpp
void CustomScene::sRender() {
    // 1. Clear the texture
    m_scene_resources.scene_texture.clear(ClearColor);
    
    // 2. Run all render logic (order matters for layering!)
    for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {
        render_logic->RunLogic();
    }
    
    // 3. ALWAYS finalize the texture
    m_scene_resources.scene_texture.display();
}
```

**Key Rules**:
- Always `clear()` first
- Always `display()` last
- Render logic order determines drawing order (first drawn = bottom layer)
- Never call `display()` inside a Logic class

### Testing Render Logic

When adding new render logic:
1. Add it to LogicFactory in the desired order
2. Implement ProcessLogic() with draw calls
3. Do NOT call display() in the logic class
4. The Scene will handle finalization

### Potential Optimization

Consider adding a check to ensure `display()` is only called if rendering occurred:

```cpp
void Scene::sRender() {
    m_scene_resources.scene_texture.clear(sf::Color::Black);
    
    if (m_scene_resources.logic_map.contains(LogicType::Render)) {
        for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {
            render_logic->RunLogic();
        }
        m_scene_resources.scene_texture.display();
    }
}
```

## Related Files

- `src/logic/UIRenderLogic.cpp` - UI rendering logic
- `src/logic/CraftingRenderLogic.cpp` - Entity rendering logic for crafting
- `src/scenes/TitleScene.cpp` - Title screen scene
- `src/scenes/CraftingScene.cpp` - Crafting screen scene
- `src/scenes/Scene.h` - Base Scene class
- `src/logic/LogicFactory.cpp` - Logic configuration for scenes
- `src/display/DisplayManager.cpp` - Window rendering management

## Commit

**Commit**: Fix rendering issues by moving display() calls to Scene::sRender()

**Files Changed**:
- src/logic/UIRenderLogic.cpp
- src/scenes/TitleScene.cpp
- src/scenes/CraftingScene.cpp

**Minimal Changes**: Only 3 files modified, 8 lines added, 2 lines removed.
