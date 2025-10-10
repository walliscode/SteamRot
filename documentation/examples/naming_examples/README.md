# File Naming Examples

This directory contains concrete examples demonstrating the file naming conventions in practice.

## Example Scenarios

### Scenario 1: Adding Validation Functions to Logic Subsystem

**Need**: Functions to validate logic state and configuration

**Solution**: Create `logic_validation.h/cpp`

**Files**:
```
src/logic/logic_validation.h
src/logic/logic_validation.cpp
tests/logic/logic_validation.test.cpp
```

**Namespace**: `steamrot::logic::validation`

**Example content**:
```cpp
// src/logic/logic_validation.h
namespace steamrot::logic::validation {
  bool IsValidLogicContext(const LogicContext& ctx);
  bool HasRequiredComponents(const ArchetypeID& id);
}
```

### Scenario 2: Adding Memory Pool Utilities to Entity Subsystem

**Need**: Template functions for working with EntityMemoryPool

**Solution**: Already exists as `emp_helpers.h`, but following new convention would be `EntityMemoryUtils.h`

**Recommended new name**: `src/entity/EntityMemoryUtils.h`

**Namespace**: `steamrot::entity`

**Example content**:
```cpp
// src/entity/EntityMemoryUtils.h (header-only templates)
namespace steamrot::entity {
  template <typename T>
  std::vector<T>& GetComponentVector(EntityMemoryPool& pool);
  
  template <typename T>
  T& GetComponent(size_t id, EntityMemoryPool& pool);
}
```

### Scenario 3: Adding Test Helpers for Scene Testing

**Need**: Helper functions for setting up scene tests

**Solution**: Create or add to `scene_test_helpers.h/cpp`

**Files**:
```
tests/scenes/scene_test_helpers.h
tests/scenes/scene_test_helpers.cpp
```

**Namespace**: `steamrot::tests::scene`

**Example content**:
```cpp
// tests/scenes/scene_test_helpers.h
namespace steamrot::tests::scene {
  void SetupMockScene(SceneType type);
  void VerifySceneInitialization(const Scene& scene);
  void CleanupSceneResources(Scene& scene);
}
```

### Scenario 4: Growing the Logic Subsystem

**Need**: Add multiple new function categories as logic subsystem grows

**Solution**: Create multiple focused files

**Files**:
```
src/logic/logic_collision.h/cpp     - Collision detection functions
src/logic/logic_ui.h/cpp            - UI-specific logic functions
src/logic/logic_validation.h/cpp   - Validation functions
src/logic/logic_debug.h/cpp         - Debug utilities
src/logic/LogicUtils.h              - Template utilities
```

**Namespaces**:
```cpp
steamrot::logic::collision
steamrot::logic::ui
steamrot::logic::validation
steamrot::logic::debug
steamrot::logic (for templates)
```

**Benefits**:
- Clear organization by purpose
- Easy to find where to add new functions
- Subsystem prefix (`logic_`) groups related files
- Can grow without file name conflicts

## Directory Structure Examples

### Well-Organized Entity Subsystem
```
src/entity/
├── EntityManager.h/cpp              # Class: manages entities
├── ArchetypeManager.h/cpp           # Class: manages archetypes
├── EntityConfigurator.h/cpp         # Class: configures entities
├── FlatbuffersConfigurator.h/cpp   # Class: flatbuffers config
├── entity_memory.h/cpp              # Functions: memory operations
├── EntityMemoryUtils.h              # Templates: memory utilities
└── ArchetypeUtils.h                 # Templates: archetype utilities

tests/entity/
├── EntityManager.test.cpp           # Tests EntityManager
├── ArchetypeManager.test.cpp        # Tests ArchetypeManager
├── entity_memory.test.cpp           # Tests entity_memory functions
└── entity_test_helpers.h/cpp        # Test utilities
```

### Well-Organized Logic Subsystem
```
src/logic/
├── Logic.h/cpp                      # Base class
├── LogicContext.h                   # Context struct
├── LogicFactory.h/cpp               # Factory class
├── UIActionLogic.h/cpp             # Derived class
├── UIRenderLogic.h/cpp             # Derived class
├── UICollisionLogic.h/cpp          # Derived class
├── CraftingRenderLogic.h/cpp       # Derived class
├── logic_collision.h/cpp            # Functions: collision utils
├── logic_ui.h/cpp                   # Functions: UI utils
└── LogicUtils.h                     # Templates: logic utilities

tests/logic/
├── UIActionLogic.test.cpp           # Tests UIActionLogic class
├── UIRenderLogic.test.cpp           # Tests UIRenderLogic class
├── logic_collision.test.cpp         # Tests collision functions
├── logic_ui.test.cpp                # Tests UI functions
└── logic_test_helpers.h/cpp         # Test utilities
```

### Well-Organized Events Subsystem
```
src/events/
├── EventHandler.h/cpp               # Class: handles events
├── EventPacket.h                    # Data structure
├── Subscriber.h/cpp                 # Class: event subscriber
├── SubscriberFactory.h/cpp          # Class: creates subscribers
├── event_conversion.h/cpp           # Functions: data conversion
└── EventUtils.h                     # Templates: event utilities

tests/events/
├── EventHandler.test.cpp            # Tests EventHandler
├── Subscriber.test.cpp              # Tests Subscriber
├── event_conversion.test.cpp        # Tests conversion functions
└── event_test_helpers.h/cpp         # Test utilities
```

## Anti-Pattern Examples

### ❌ Don't: Generic Helper Files
```
src/utils/helpers.h                  # Too generic!
src/common/utils.h                   # What kind of utils?
src/logic/helpers.h                  # Helpers for what?
```

### ✅ Do: Specific Purpose Files
```
src/logic/logic_collision.h          # Clear: logic subsystem, collision category
src/entity/entity_memory.h           # Clear: entity subsystem, memory operations
src/events/event_conversion.h        # Clear: event subsystem, conversion functions
```

### ❌ Don't: Unclear Abbreviations
```
src/entity/emp_helpers.h             # What's "emp"?
src/logic/ui_h.h                     # Confusing abbreviation
```

### ✅ Do: Clear Full Names
```
src/entity/entity_memory.h           # Clear: entity memory operations
src/logic/logic_ui.h                 # Clear: logic UI utilities
```

### ❌ Don't: Class-Like Names for Function Files
```
src/logic/Collision.h                # Looks like a class!
src/entity/Memory.h                  # Confusing - class or functions?
```

### ✅ Do: Clear Function File Names
```
src/logic/logic_collision.h          # Obviously functions
src/entity/entity_memory.h           # Obviously functions
```

## Naming Convention Examples

### Example 1: Logic Collision Functions

**File structure**:
```
src/logic/logic_collision.h/cpp
namespace steamrot::logic::collision
```

**Implementation**:
```cpp
// src/logic/logic_collision.h
namespace steamrot::logic::collision {
  bool IsMouseOverBounds(const sf::FloatRect& bounds, const sf::Vector2f& mousePos);
  void CheckCollision(Entity& entity1, Entity& entity2);
}
```

### Example 2: Logic UI Functions

**File structure**:
```
src/logic/logic_ui.h/cpp
namespace steamrot::logic::ui
```

**Implementation**:
```cpp
// src/logic/logic_ui.h
namespace steamrot::logic::ui {
  std::vector<std::string> GetAllFragmentNames();
  std::vector<std::string> GetAllJointNames();
}
```

### Example 3: Test Helpers

**File structure**:
```
tests/logic/logic_test_helpers.h/cpp
namespace steamrot::tests::logic
```

**Implementation**:
```cpp
// tests/logic/logic_test_helpers.h
namespace steamrot::tests::logic {
  void CheckStaticLogicCollections(const LogicCollection& collection, SceneType type);
  LogicContext CreateMockLogicContext();
}
```

## Summary

The key principles demonstrated in these examples:

1. **Visual Distinction**: File name immediately tells you if it's a class or functions
2. **Subsystem Prefix**: Free function files prefixed with subsystem name
3. **Namespace Alignment**: File name matches namespace structure
4. **Extensibility**: Easy to add new categories to a subsystem
5. **Clarity**: No ambiguity about file contents

When creating new files, follow these patterns. Existing files can be migrated opportunistically.
