# Subscriber Processing Interface Analysis

**Date:** December 15, 2024  
**Status:** Analysis Complete  
**Decision:** See [Recommendation](#recommendation)

## Executive Summary

This document analyzes whether SteamRot should implement a unified subscriber processing interface to replace the current pattern of custom `ProcessSubscriptions()` methods at different architectural levels (Engine, SceneManager, etc.).

**Key Finding:** The current ad-hoc approach is **appropriate for this codebase** given the small number of processing locations (2), distinct responsibilities at each level, and minimal code duplication. Implementing an interface would introduce unnecessary abstraction without meaningful benefits.

**Recommendation:** **Do not implement** a subscriber processing interface at this time. The current pattern is sufficient and maintainable. Re-evaluate if subscriber processing locations grow beyond 4-5 distinct levels.

---

## Table of Contents

- [Current Architecture](#current-architecture)
- [Current Patterns](#current-patterns)
- [Code Duplication Analysis](#code-duplication-analysis)
- [Benefits of Interface Approach](#benefits-of-interface-approach)
- [Costs of Interface Approach](#costs-of-interface-approach)
- [Alternative Patterns](#alternative-patterns)
- [Recommendation](#recommendation)
- [Future Considerations](#future-considerations)

---

## Current Architecture

### Subscriber System Overview

The SteamRot event system uses a **Subscriber pattern** where:

1. **Subscriber** (`Subscriber.h`): Lightweight struct that stores:
   - `m_trigger_event_type`: EventType to listen for
   - `m_trigger_event_data`: Optional specific event data to match
   - `m_active`: Flag set by EventHandler when triggered
   - `m_received_event_data`: Actual event data received

2. **EventHandler**: Central event bus that:
   - Maintains `m_subscriber_register` (map of EventType → weak_ptr<Subscriber>)
   - Processes SFML events and converts to game events
   - Updates subscribers when matching events occur
   - Manages event lifetimes

3. **Processing Locations**: Various parts of the codebase check subscriber activation and take action

### Subscriber Processing Locations

Currently, subscribers are processed at **3 distinct architectural levels**:

#### 1. Engine Level (`GameEngine::ProcessSubscriptions()`)

**Location:** `src/engine/GameEngine.cpp:76-99`

**Responsibility:** Engine-wide actions (quit game, etc.)

**Subscribers:** Stored in `EngineState.subscriptions`

**Events Handled:**
- `EventType_EVENT_QUIT_GAME` → Closes window

**Pattern:**
```cpp
std::expected<std::monostate, FailInfo> GameEngine::ProcessSubscriptions() {
  for (const auto &subscriber : m_engine_state.subscriptions) {
    if (subscriber->m_active) {
      switch (subscriber->m_trigger_event_type) {
      case EventType::EventType_EVENT_QUIT_GAME:
        m_engine_resources.game_window.close();
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

#### 2. SceneManager Level (`SceneManager::ProcessSubscriptions()`)

**Location:** `src/scenes/SceneManager.cpp:161-219`

**Responsibility:** Scene lifecycle management (loading/unloading scenes)

**Subscribers:** Stored in `SceneManagerState.subscriptions`

**Events Handled:**
- `EventType_EVENT_CHANGE_SCENE` → Loads appropriate scene based on SceneChangePacket data
  - `SceneType_TITLE` → Calls `LoadTitleScene()`
  - `SceneType_CRAFTING` → Calls `LoadCraftingScene()`

**Pattern:**
```cpp
std::expected<std::monostate, FailInfo> SceneManager::ProcessSubscriptions() {
  for (auto &subscriber : m_scene_manager_state.subscriptions) {
    if (subscriber->m_active) {
      switch (subscriber->m_trigger_event_type) {
      case EventType::EventType_EVENT_CHANGE_SCENE: {
        // Validate event data
        if (!subscriber->m_received_event_data.has_value() ||
            !std::holds_alternative<SceneChangePacket>(...)) {
          subscriber->m_active = false;
          continue;
        }
        // Extract scene type and load appropriate scene
        const SceneChangePacket &scene_change_data = ...;
        switch (scene_change_data.second) {
        case SceneType_TITLE:
          auto result = LoadTitleScene();
          // error handling
          break;
        case SceneType_CRAFTING:
          auto result = LoadCraftingScene();
          // error handling
          break;
        }
        break;
      }
      default:
        break;
      }
    }
  }
  return std::monostate{};
}
```

#### 3. UI Element Level (Action Logic)

**Location:** `src/logic/logic_action.cpp:21-81`

**Responsibility:** UI interactions (button clicks, dropdown actions)

**Subscribers:** Stored in `UIElement.subscription` (individual elements)

**Events Handled:**
- Various UI interaction events trigger element-specific actions
- Buttons emit response events
- Dropdowns populate with data

**Pattern:**
```cpp
void ProcessUIActionsAndEvents(UIElement &ui_element, ...) {
  if (!ui_element.subscription || !ui_element.subscription->m_active) {
    return;
  }
  
  // Dispatch based on UIElement type (ButtonElement, DropDownListElement, etc.)
  if (ButtonElement *button = dynamic_cast<ButtonElement*>(&ui_element)) {
    ProcessButtonElementActions(*button, event_handler);
  } else if (DropDownListElement *dropdown = ...) {
    ProcessDropDownListElementActions(*dropdown, scene_context);
  }
  
  ui_element.subscription->m_active = false;
}
```

**Note:** This is called recursively via `ProcessNestedUIActionsAndEvents()` to handle child elements in a depth-first manner.

#### 4. Logic Classes (Currently Unused)

**Location:** `src/logic/Logic.h:41` and `Logic.cpp:21-28`

**Status:** Logic classes have `m_subscribers` vector and `AddSubscriber()` method, but:
- No active processing (line 81 in Logic.h: `// virtual void ProcessSubscribers() = 0;` is commented out)
- No concrete Logic subclass implements subscriber processing
- Subscribers are registered but never checked for activation

**Potential Use Case:** Scene-specific logic that doesn't fit at Engine or SceneManager level.

---

## Current Patterns

### Commonalities Across Processing Locations

All subscriber processing follows this **general pattern**:

1. **Iterate** over a collection of subscribers
2. **Check activation**: `if (subscriber->m_active)`
3. **Switch on event type**: `switch (subscriber->m_trigger_event_type)`
4. **Validate event data** (if needed)
5. **Execute location-specific action**
6. **Reset activation**: `subscriber->m_active = false`
7. **Return result**: `std::expected<std::monostate, FailInfo>`

### Key Differences

| Aspect | Engine | SceneManager | UI Elements |
|--------|--------|--------------|-------------|
| **Subscriber Storage** | `EngineState.subscriptions` | `SceneManagerState.subscriptions` | `UIElement.subscription` |
| **Collection Type** | `vector<shared_ptr<Subscriber>>` | `vector<shared_ptr<Subscriber>>` | Single `shared_ptr<Subscriber>` |
| **Action Scope** | Engine-wide (quit, pause) | Scene lifecycle (load/unload) | Element-specific (click, populate) |
| **Error Handling** | Returns `std::expected` | Returns `std::expected` | Void (no error propagation) |
| **Data Validation** | Minimal (simple events) | Complex (variant extraction) | Per-element type |
| **Action Complexity** | Single action per event | Multiple scene types per event | Dynamic dispatch by UIElement type |

---

## Code Duplication Analysis

### Quantitative Analysis

**Lines of Code:**
- `GameEngine::ProcessSubscriptions()`: **23 lines** (lines 76-99)
- `SceneManager::ProcessSubscriptions()`: **58 lines** (lines 161-219)
- UI action processing: **60+ lines** (distributed across multiple functions)

**Total:** ~141 lines across 3 locations

**Duplicated Boilerplate:**
- For-loop iteration: ~3 lines per location
- Active check: ~2 lines per location
- Switch statement: ~2 lines per location
- Deactivation: ~1 line per location

**Estimated Duplication:** ~8 lines × 2 locations = **~16 lines** (11% of total)

### Qualitative Analysis

**What's Actually Duplicated:**
```cpp
// This pattern appears twice (Engine and SceneManager)
for (const auto &subscriber : subscriptions) {
  if (subscriber->m_active) {
    switch (subscriber->m_trigger_event_type) {
      case EventType_X:
        // ... location-specific action ...
        break;
      default:
        break;
    }
    subscriber->m_active = false;  // Sometimes placed here
  }
}
```

**What's NOT Duplicated:**
- Event data validation logic (SceneManager-specific)
- Action execution (completely different at each level)
- Error handling patterns (varies by location)
- Return types and error propagation

**Key Insight:** The "duplication" is mostly **structural boilerplate** (loops, conditionals), not **business logic**. The actual work done in each `ProcessSubscriptions()` method is unique to that architectural level.

---

## Benefits of Interface Approach

### Potential Benefits

#### 1. **Reduced Boilerplate**

**Claim:** Abstract away the common iteration/activation checking pattern.

**Reality Check:**
- Boilerplate is only ~8 lines per location
- The pattern is simple and readable
- Abstraction overhead may exceed savings

#### 2. **Enforced Consistency**

**Claim:** Ensure all subscriber processing follows the same pattern.

**Reality Check:**
- Current pattern is already consistent
- Differences are intentional (based on level-specific needs)
- Interface might force artificial uniformity where variation is appropriate

#### 3. **Testability**

**Claim:** Easier to mock/test subscriber processing.

**Reality Check:**
- Current methods are already easily testable (inject subscribers, check results)
- Interface wouldn't significantly improve test isolation
- Could introduce additional test complexity (interface mocking)

#### 4. **Extensibility**

**Claim:** Easier to add new subscriber processing locations.

**Reality Check:**
- Only 2 active locations currently (Engine, SceneManager)
- UI processing is fundamentally different (element-based, not collection-based)
- Adding a 3rd location is trivial with current pattern

#### 5. **Code Reuse**

**Claim:** Share common logic across all processing locations.

**Reality Check:**
- Only ~11% of code is actually duplicated
- Shared logic would be trivial helper functions, not an interface
- Interface overhead may exceed reuse benefits

### Benefits Summary

**Real Benefits:** Minimal. The claimed benefits are mostly theoretical and don't apply strongly to this codebase given:
- Small number of processing locations (2)
- Low duplication percentage (11%)
- Intentionally different responsibilities at each level

---

## Costs of Interface Approach

### Implementation Costs

#### 1. **Interface Design Complexity**

**Challenge:** What would the interface look like?

**Option A: Generic Handler**
```cpp
class ISubscriberProcessor {
public:
  virtual ~ISubscriberProcessor() = default;
  
  virtual std::expected<std::monostate, FailInfo>
  ProcessSubscriptions(std::vector<std::shared_ptr<Subscriber>>& subscriptions) = 0;
};
```

**Problems:**
- Forces all locations to use `vector<shared_ptr<Subscriber>>`
- Doesn't fit UI elements (single subscription per element)
- Loses compile-time type safety
- Generic interface provides no real value

**Option B: Template Strategy**
```cpp
template<typename Context>
class SubscriberProcessor {
public:
  std::expected<std::monostate, FailInfo>
  ProcessSubscriptions(std::vector<std::shared_ptr<Subscriber>>& subs, Context& ctx) {
    for (auto& sub : subs) {
      if (sub->m_active) {
        auto result = ProcessSubscriber(sub, ctx);  // Virtual call
        if (!result) return result;
        sub->m_active = false;
      }
    }
    return std::monostate{};
  }
  
protected:
  virtual std::expected<std::monostate, FailInfo>
  ProcessSubscriber(std::shared_ptr<Subscriber>& sub, Context& ctx) = 0;
};
```

**Problems:**
- Complex template interface for minimal gain
- Forces single-subscriber processing (loses batch optimization opportunities)
- Derived classes still need full switch statements
- Doesn't reduce actual code significantly

#### 2. **Abstraction Overhead**

**Costs:**
- New interface file(s) to create and maintain
- Additional indirection (virtual function calls)
- More complex class hierarchy
- Harder to understand control flow (jumping through interface)

**Benefit Trade-off:** Is 16 lines of duplication worth 50+ lines of interface code?

#### 3. **Lost Flexibility**

**Current Flexibility:**
- Engine can check subscribers differently than SceneManager if needed
- Easy to add location-specific error handling
- Easy to add logging/metrics at specific points
- No forced structure

**With Interface:**
- Must conform to interface contract
- Harder to customize behavior at specific levels
- May need workarounds for special cases

#### 4. **Increased Complexity for Readers**

**Current Code:**
- Developer sees exactly what happens: loop → check → switch → action
- All logic in one method, easy to trace

**With Interface:**
- Developer must jump to interface definition
- Must understand polymorphism and virtual dispatch
- Logic split across multiple files/classes
- Harder to understand for newcomers

### Costs Summary

**Real Costs:**
- **50+ lines** of interface code to save **16 lines** of duplication
- Additional **cognitive load** for readers
- **Reduced flexibility** for level-specific customization
- No clear **runtime or maintenance benefits**

---

## Alternative Patterns

Rather than a full interface, consider these lightweight alternatives:

### Option 1: Helper Function (Recommended if any abstraction is needed)

```cpp
// subscriber_processing_helpers.h
namespace steamrot {
namespace subscriber {

template<typename ActionFunc>
std::expected<std::monostate, FailInfo>
ProcessSubscriberCollection(
    std::vector<std::shared_ptr<Subscriber>>& subscriptions,
    ActionFunc&& action_func) {
  
  for (auto& subscriber : subscriptions) {
    if (subscriber->m_active) {
      auto result = action_func(subscriber);
      if (!result.has_value()) {
        return std::unexpected(result.error());
      }
      subscriber->m_active = false;
    }
  }
  return std::monostate{};
}

} // namespace subscriber
} // namespace steamrot
```

**Usage:**
```cpp
// GameEngine
std::expected<std::monostate, FailInfo> GameEngine::ProcessSubscriptions() {
  return subscriber::ProcessSubscriberCollection(
    m_engine_state.subscriptions,
    [this](auto& sub) -> std::expected<std::monostate, FailInfo> {
      switch (sub->m_trigger_event_type) {
      case EventType::EventType_EVENT_QUIT_GAME:
        m_engine_resources.game_window.close();
        break;
      default:
        break;
      }
      return std::monostate{};
    });
}
```

**Pros:**
- Reduces boilerplate to ~8 lines at call site
- No interface hierarchy needed
- Preserves flexibility (lambda can do anything)
- Easy to understand

**Cons:**
- Still requires lambda boilerplate
- Not a huge improvement over current code

### Option 2: Keep Current Pattern (Recommended)

**Rationale:**
- Only 2 active processing locations
- Code is already clear and maintainable
- Duplication is minimal and intentional
- No abstraction = no abstraction cost

---

## Recommendation

### Decision: **Do Not Implement** Subscriber Processing Interface

**Reasoning:**

1. **Insufficient Scale:**
   - Only 2 active subscriber processing locations (Engine, SceneManager)
   - 16 lines of duplicated code is not a maintenance burden
   - UI element processing is fundamentally different and shouldn't be unified

2. **Poor Cost-Benefit Ratio:**
   - Interface would require 50+ lines of code
   - Saves only 16 lines of duplication
   - Increases cognitive complexity for readers
   - Reduces flexibility at each level

3. **Different Responsibilities:**
   - Engine: Application lifecycle (quit, pause)
   - SceneManager: Scene lifecycle (load, unload)
   - These are distinct concerns with intentionally different implementations
   - Unifying them provides no architectural value

4. **Current Pattern is Adequate:**
   - Simple, readable, and consistent
   - Easy to test and modify
   - No reported maintenance issues
   - Follows "You Aren't Gonna Need It" (YAGNI) principle

### When to Reconsider

Re-evaluate this decision if:

1. **Number of processing locations grows beyond 4-5**
   - At scale, interface might justify its cost
   
2. **Significant new duplication emerges**
   - If processing logic becomes more complex and duplicated
   
3. **Plugin/extension system is needed**
   - Interface would enable third-party subscriber processors
   
4. **Testing complexity increases**
   - If mocking becomes necessary for subscriber processing

### Alternative Recommendation

**If abstraction is still desired:**

Implement **Option 1: Helper Function** (`ProcessSubscriberCollection`) instead of a full interface:

**Why:**
- Reduces boilerplate without introducing inheritance
- Preserves flexibility via lambdas
- Easy to add/remove without breaking changes
- Much lower implementation cost (~20 lines)

**When:**
- Only if a 3rd subscriber processing location is added
- Only if team consensus agrees it improves readability

---

## Future Considerations

### Potential Evolution Paths

#### Path 1: Keep Current Pattern (Most Likely)

**Timeline:** Indefinite

**Scenario:** Number of subscriber processing locations stays at 2-3

**Action:** No changes needed

#### Path 2: Add Helper Function (If 3rd Location Added)

**Timeline:** When 3rd processing location is implemented

**Scenario:** DisplayManager or another component needs subscriber processing

**Action:**
1. Implement `ProcessSubscriberCollection` helper
2. Refactor existing locations to use helper
3. Document pattern in coding guidelines

#### Path 3: Implement Interface (Only if Scaling)

**Timeline:** If 5+ distinct processing locations exist

**Scenario:** Subscriber processing becomes a common pattern across many components

**Action:**
1. Design `ISubscriberProcessor` interface
2. Refactor all locations to implement interface
3. Add comprehensive tests for interface contract
4. Document interface usage in architecture docs

### Monitoring Metrics

Track these metrics to inform future decisions:

| Metric | Current | Threshold for Action |
|--------|---------|---------------------|
| Processing Locations | 2 active | 5+ active |
| Duplicated Lines | 16 lines | 50+ lines |
| Average Method Length | 23-58 lines | 100+ lines |
| Reported Maintenance Issues | 0 | 3+ issues |

---

## Appendix: Code Examples

### Current Implementation Examples

#### GameEngine::ProcessSubscriptions()

**File:** `src/engine/GameEngine.cpp:76-99`

```cpp
std::expected<std::monostate, FailInfo> GameEngine::ProcessSubscriptions() {
  // cycle through all subscribers and process active ones
  for (const auto &subscriber : m_engine_state.subscriptions) {

    // only process active subscribers
    if (subscriber->m_active) {

      // switch on the EventType
      switch (subscriber->m_trigger_event_type) {
      case EventType::EventType_EVENT_QUIT_GAME: {
        // close the window to quit the game
        m_engine_resources.game_window.close();
        break;
      }
      default:
        break;
      }

      // FINALLY set the subscriber to inactive
      subscriber->m_active = false;
    }
  }
  return std::monostate{};
}
```

#### SceneManager::ProcessSubscriptions()

**File:** `src/scenes/SceneManager.cpp:161-219`

```cpp
std::expected<std::monostate, FailInfo> SceneManager::ProcessSubscriptions() {

  for (auto &subscriber : m_scene_manager_state.subscriptions) {

    // only process active subscribers
    if (subscriber->m_active) {

      switch (subscriber->m_trigger_event_type) {
      case EventType::EventType_EVENT_CHANGE_SCENE: {

        // make sure the data type is correct - use received event data
        if (!subscriber->m_received_event_data.has_value() ||
            !std::holds_alternative<SceneChangePacket>(
                subscriber->m_received_event_data.value())) {

          // just continue if data is not valid, will need to log at some point
          //[TODO: logging here]

          // still need to set subscriber to inactive
          subscriber->m_active = false;
          continue;
        }
        // get SceneChangeData from received event data
        const SceneChangePacket &scene_change_data =
            std::get<SceneChangePacket>(
                subscriber->m_received_event_data.value());

        // check for scene type
        switch (scene_change_data.second) {

          // deal with Title Scene Loading
        case SceneType_TITLE: {
          auto load_scene_result = LoadTitleScene();
          if (!load_scene_result.has_value()) {
            return std::unexpected(load_scene_result.error());
          }
          break;
        }

        case SceneType_CRAFTING: {
          auto load_scene_result = LoadCraftingScene();
          if (!load_scene_result.has_value()) {
            return std::unexpected(load_scene_result.error());
          }
          break;
        }
        default:
          break;
        }

        break;
      }
      default:
        break;
      }
    }
  }
  return std::monostate{};
}
```

---

## References

### Related Files

- `src/types/events/Subscriber.h` - Subscriber struct definition
- `src/events/EventHandler.h` - Event bus and subscriber registration
- `src/engine/Engine.h` - Abstract Engine base class with ProcessSubscriptions() pure virtual
- `src/engine/GameEngine.cpp` - Concrete Engine implementation
- `src/scenes/SceneManager.cpp` - SceneManager subscriber processing
- `src/logic/Logic.h` - Logic base class (unused subscriber support)
- `src/logic/logic_action.cpp` - UI element subscriber processing
- `src/types/core/EngineState.h` - Engine subscription storage
- `src/types/core/SceneManagerState.h` - SceneManager subscription storage
- `src/types/user_interface/UIElement.h` - UI element subscription storage

### Architecture Documentation

- Repository README.md - Project overview and structure
- `.github/copilot-instructions.md` - Coding standards and patterns

---

## Document History

| Date | Author | Change |
|------|--------|--------|
| 2024-12-15 | GitHub Copilot Agent | Initial analysis |

