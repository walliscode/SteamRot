# Subscriber Processing Quick Reference

**For full analysis, see:** [SUBSCRIBER_PROCESSING_INTERFACE_ANALYSIS.md](SUBSCRIBER_PROCESSING_INTERFACE_ANALYSIS.md)

---

## TL;DR

**Question:** Should we implement a subscriber processing interface?

**Answer:** **NO** - Current pattern is appropriate for 2 processing locations.

**Re-evaluate when:** 5+ distinct processing locations exist.

---

## Current Subscriber Processing Locations

### 1. Engine Level
- **File:** `src/engine/GameEngine.cpp:76-99`
- **Storage:** `EngineState.subscriptions`
- **Handles:** Quit game, pause, engine-wide actions
- **Size:** 23 lines

### 2. SceneManager Level
- **File:** `src/scenes/SceneManager.cpp:161-219`
- **Storage:** `SceneManagerState.subscriptions`
- **Handles:** Scene loading/unloading (Title, Crafting scenes)
- **Size:** 58 lines

### 3. UI Element Level
- **File:** `src/logic/logic_action.cpp:21-81`
- **Storage:** `UIElement.subscription` (individual)
- **Handles:** Button clicks, dropdown actions, element-specific logic
- **Size:** 60+ lines

### 4. Logic Classes (UNUSED)
- **File:** `src/logic/Logic.h:41`
- **Storage:** `Logic.m_subscribers`
- **Status:** Vector exists but processing is commented out
- **Note:** Available for future scene-specific subscription needs

---

## Standard Processing Pattern

```cpp
std::expected<std::monostate, FailInfo> ProcessSubscriptions() {
  for (auto& subscriber : subscriptions) {
    if (subscriber->m_active) {
      switch (subscriber->m_trigger_event_type) {
      case EventType_X:
        // Execute location-specific action
        break;
      default:
        break;
      }
      subscriber->m_active = false;
    }
  }
  return std::monostate{};
}
```

---

## Key Metrics

| Metric | Value |
|--------|-------|
| **Active Processing Locations** | 2 (Engine, SceneManager) |
| **Total Processing Code** | ~141 lines |
| **Duplicated Boilerplate** | ~16 lines (11%) |
| **Interface Would Add** | 50+ lines |
| **Cost-Benefit Ratio** | 3:1 increase |

---

## Decision Criteria

### When to Keep Current Pattern (NOW)
- ✅ 2-4 processing locations
- ✅ < 20% code duplication
- ✅ Distinct responsibilities at each level
- ✅ Simple, readable implementations

### When to Reconsider Interface
- ❌ 5+ processing locations
- ❌ > 20% code duplication
- ❌ Complex shared logic emerges
- ❌ Plugin/extension system needed

---

## Adding New Subscriber Processing

**If you need to add subscriber processing in a new location:**

1. **First, determine the right level:**
   - Engine-wide actions? → Add to `GameEngine::ProcessSubscriptions()`
   - Scene lifecycle? → Add to `SceneManager::ProcessSubscriptions()`
   - Scene-specific logic? → Consider using/activating `Logic.m_subscribers`
   - UI interactions? → Use `logic_action.cpp` pattern

2. **Follow the standard pattern:**
   - Iterate over subscriber collection
   - Check `m_active` flag
   - Switch on `m_trigger_event_type`
   - Execute location-specific action
   - Set `m_active = false`
   - Return `std::expected<std::monostate, FailInfo>`

3. **Consider re-evaluation:**
   - If this creates a 5th distinct processing location
   - Review [SUBSCRIBER_PROCESSING_INTERFACE_ANALYSIS.md](SUBSCRIBER_PROCESSING_INTERFACE_ANALYSIS.md)
   - Discuss with team whether interface is now justified

---

## Related Files

- **Analysis:** `documentation/analysis/SUBSCRIBER_PROCESSING_INTERFACE_ANALYSIS.md`
- **Subscriber:** `src/types/events/Subscriber.h`
- **EventHandler:** `src/events/EventHandler.h`
- **Engine:** `src/engine/GameEngine.cpp:76-99`
- **SceneManager:** `src/scenes/SceneManager.cpp:161-219`
- **UI Actions:** `src/logic/logic_action.cpp:21-81`
- **State Storage:** `src/types/core/EngineState.h`, `src/types/core/SceneManagerState.h`

---

## Quick Decision Tree

```
Need to process subscribers?
│
├─ At Engine level?
│  └─ Add case to GameEngine::ProcessSubscriptions()
│
├─ At SceneManager level?
│  └─ Add case to SceneManager::ProcessSubscriptions()
│
├─ At Scene level?
│  └─ Activate Logic.m_subscribers pattern
│
├─ At UI element level?
│  └─ Use logic_action.cpp pattern
│
└─ New architectural level?
   ├─ Follow standard pattern
   └─ Count total locations:
      ├─ < 5? Continue with current pattern
      └─ ≥ 5? Review analysis doc, consider interface
```

---

**Last Updated:** December 15, 2024
