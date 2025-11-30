# EntityMemoryPool Copyability Proposal

## Executive Summary

This proposal outlines approaches to make `EntityMemoryPool` copyable, enabling critical testing infrastructure (tick-by-tick comparison, snapshot testing) and potential future features requiring state cloning.

**Status**: Proposal  
**Date**: 2025-11-30  
**Type**: Design Decision

## Background

### Current State

`EntityMemoryPool` is defined as a tuple of vectors of components:

```cpp
using EntityMemoryPool =
    ComponentContainer<ComponentRegister>::ComponentVectorTuple;
```

Which expands to:
```cpp
std::tuple<
    std::vector<CMeta>,
    std::vector<CUserInterface>,
    std::vector<CMachinaForm>,
    std::vector<CGrimoireMachina>,
    std::vector<CUIState>
>
```

### Problem Statement

`EntityMemoryPool` is **not copyable** because two components contain `std::unique_ptr` members:

| Component | Non-Copyable Member | Purpose |
|-----------|---------------------|---------|
| `CUserInterface` | `std::unique_ptr<UIElement> m_root_element` | Polymorphic UI tree root |
| `CGrimoireMachina` | `std::unique_ptr<CMachinaForm> m_holding_form` | Temporary machina being constructed |

Additionally, `UIElement` itself contains:
```cpp
std::vector<std::unique_ptr<UIElement>> child_elements;
```

This creates a recursive polymorphic tree structure that requires special handling for copying.

### Why Copyability Matters

1. **Tick-by-Tick Testing**: Snapshot entity states at specific ticks for comparison
2. **Undo/Redo Systems**: Clone states for rollback functionality
3. **State Serialization**: Deep copy for save/load features
4. **Test Isolation**: Create independent copies for parallel testing
5. **Debugging**: Capture state at specific points for analysis

## Proposed Solutions

### Option 1: Clone Method Pattern (Recommended)

Add a `Clone()` method to polymorphic types that performs deep copying.

#### Implementation

**1. Add Clone to UIElement and derived types:**

```cpp
// UIElement.h
struct UIElement {
    // ... existing members ...
    
    ////////////////////////////////////////////////////////////
    /// @brief Create a deep copy of this UI element and its children
    ///
    /// @return A new unique_ptr to a cloned UIElement
    ////////////////////////////////////////////////////////////
    virtual std::unique_ptr<UIElement> Clone() const = 0;
};
```

**2. Implement Clone in each derived UIElement type:**

```cpp
// ButtonElement.h
struct ButtonElement : public UIElement {
    std::unique_ptr<UIElement> Clone() const override {
        auto cloned = std::make_unique<ButtonElement>();
        CloneBaseUIElementData(*cloned);
        // Copy any ButtonElement-specific data here
        // (Currently ButtonElement has no additional fields beyond UIElement base)
        return cloned;
    }
};
```

**3. Add helper for base UIElement data:**

```cpp
// UIElement.cpp
void UIElement::CloneBaseUIElementData(UIElement& target) const {
    target.position = position;
    target.size = size;
    target.subscription = subscription;  // shared_ptr copies fine
    target.response_event = response_event;
    target.is_mouse_over = is_mouse_over;
    target.children_active = children_active;
    target.spacing_strategy = spacing_strategy;
    target.layout = layout;
    
    // Deep copy children
    for (const auto& child : child_elements) {
        if (child) {
            target.child_elements.push_back(child->Clone());
        }
    }
}
```

**4. Make CUserInterface copyable:**

```cpp
// CUserInterface.h
struct CUserInterface : public Component {
    // ... existing members ...
    
    // Copy constructor
    CUserInterface(const CUserInterface& other) 
        : Component(other),
          m_name(other.m_name),
          m_root_element(other.m_root_element ? other.m_root_element->Clone() : nullptr),
          m_visible(other.m_visible) {}
    
    // Copy assignment
    CUserInterface& operator=(const CUserInterface& other) {
        if (this != &other) {
            Component::operator=(other);
            m_name = other.m_name;
            m_root_element = other.m_root_element ? other.m_root_element->Clone() : nullptr;
            m_visible = other.m_visible;
        }
        return *this;
    }
};
```

**5. Make CGrimoireMachina copyable:**

```cpp
// CGrimoireMachina.h
struct CGrimoireMachina : public Component {
    // ... existing members ...
    
    // Copy constructor
    CGrimoireMachina(const CGrimoireMachina& other)
        : Component(other),
          m_all_fragments(other.m_all_fragments),
          m_all_joints(other.m_all_joints),
          m_machina_forms(other.m_machina_forms),
          m_holding_form(other.m_holding_form 
                         ? std::make_unique<CMachinaForm>(*other.m_holding_form) 
                         : nullptr) {}
    
    // Copy assignment
    CGrimoireMachina& operator=(const CGrimoireMachina& other) {
        if (this != &other) {
            Component::operator=(other);
            m_all_fragments = other.m_all_fragments;
            m_all_joints = other.m_all_joints;
            m_machina_forms = other.m_machina_forms;
            m_holding_form = other.m_holding_form 
                             ? std::make_unique<CMachinaForm>(*other.m_holding_form) 
                             : nullptr;
        }
        return *this;
    }
};
```

#### Advantages

- ✅ Standard C++ pattern for polymorphic copying
- ✅ Makes components fully copyable (STL-compatible)
- ✅ EntityMemoryPool becomes automatically copyable
- ✅ Can simply use `auto pool_copy = original_pool;`
- ✅ Each type controls its own cloning logic
- ✅ Extensible as new UIElement types are added

#### Disadvantages

- ⚠️ Requires implementing Clone() in every UIElement subclass
- ⚠️ Risk of incomplete cloning if new fields are added without updating Clone()
- ⚠️ Some performance overhead for deep copying

---

### Option 2: Free Function Copy Utility

Create a dedicated copy function without modifying component interfaces.

#### Implementation

```cpp
// entity_memory_copy.h
namespace steamrot::entity::memory {

////////////////////////////////////////////////////////////
/// @brief Deep copy an EntityMemoryPool
///
/// @param source Source pool to copy
/// @return New EntityMemoryPool with all data deep copied
////////////////////////////////////////////////////////////
EntityMemoryPool DeepCopyPool(const EntityMemoryPool& source);

////////////////////////////////////////////////////////////
/// @brief Deep copy a UIElement tree
///
/// @param source Source UIElement to copy
/// @return New unique_ptr to copied UIElement hierarchy
////////////////////////////////////////////////////////////
std::unique_ptr<UIElement> DeepCopyUIElement(const UIElement* source);

} // namespace steamrot::entity::memory
```

```cpp
// entity_memory_copy.cpp
EntityMemoryPool DeepCopyPool(const EntityMemoryPool& source) {
    EntityMemoryPool copy;
    
    // Copy simple components directly
    std::get<std::vector<CMeta>>(copy) = std::get<std::vector<CMeta>>(source);
    std::get<std::vector<CMachinaForm>>(copy) = std::get<std::vector<CMachinaForm>>(source);
    std::get<std::vector<CUIState>>(copy) = std::get<std::vector<CUIState>>(source);
    
    // Deep copy CUserInterface
    // WARNING: This approach is brittle - if new fields are added to 
    // CUserInterface, they must be manually added here. This is a key
    // reason why the Clone method pattern (Option 1) is preferred.
    auto& src_ui = std::get<std::vector<CUserInterface>>(source);
    auto& dst_ui = std::get<std::vector<CUserInterface>>(copy);
    dst_ui.reserve(src_ui.size());
    for (const auto& ui : src_ui) {
        CUserInterface ui_copy;
        ui_copy.m_name = ui.m_name;
        ui_copy.m_visible = ui.m_visible;
        ui_copy.m_active = ui.m_active;
        ui_copy.m_root_element = DeepCopyUIElement(ui.m_root_element.get());
        dst_ui.push_back(std::move(ui_copy));
    }
    
    // Deep copy CGrimoireMachina
    auto& src_gm = std::get<std::vector<CGrimoireMachina>>(source);
    auto& dst_gm = std::get<std::vector<CGrimoireMachina>>(copy);
    dst_gm.reserve(src_gm.size());
    for (const auto& gm : src_gm) {
        CGrimoireMachina gm_copy;
        gm_copy.m_all_fragments = gm.m_all_fragments;
        gm_copy.m_all_joints = gm.m_all_joints;
        gm_copy.m_machina_forms = gm.m_machina_forms;
        gm_copy.m_active = gm.m_active;
        if (gm.m_holding_form) {
            gm_copy.m_holding_form = std::make_unique<CMachinaForm>(*gm.m_holding_form);
        }
        dst_gm.push_back(std::move(gm_copy));
    }
    
    return copy;
}

std::unique_ptr<UIElement> DeepCopyUIElement(const UIElement* source) {
    if (!source) return nullptr;
    
    // Use type dispatch to create correct derived type
    // This requires runtime type identification or visitor pattern
    // ... implementation depends on UIElement hierarchy
}
```

#### Advantages

- ✅ No modifications to existing interfaces
- ✅ Centralized copy logic
- ✅ Easy to add logging/debugging

#### Disadvantages

- ⚠️ Requires type dispatch for polymorphic UIElement (messy)
- ⚠️ Needs updating whenever ComponentRegister changes
- ⚠️ Components still not copyable individually
- ⚠️ Less intuitive API (`DeepCopyPool(pool)` vs `pool`)

---

### Option 3: Shared Pointer for Polymorphic Members

Replace `unique_ptr` with `shared_ptr` for polymorphic members.

#### Implementation

```cpp
// CUserInterface.h
struct CUserInterface : public Component {
    std::shared_ptr<UIElement> m_root_element;  // Was unique_ptr
    // ...
};

// CGrimoireMachina.h
struct CGrimoireMachina : public Component {
    std::shared_ptr<CMachinaForm> m_holding_form;  // Was unique_ptr
    // ...
};
```

#### Advantages

- ✅ Simple change
- ✅ Components become copyable immediately
- ✅ No Clone() implementation needed

#### Disadvantages

- ❌ **Creates shallow copies** - original and copy share the same UIElement tree
- ❌ Modifications to one pool affect the other
- ❌ Not suitable for snapshot testing where independent states are needed
- ❌ Changes ownership semantics

**Verdict**: Not recommended for this use case.

---

### Option 4: Copy-on-Write (COW) Pattern

Use shared_ptr but clone on modification.

#### Implementation

```cpp
template<typename T>
class CopyOnWrite {
    std::shared_ptr<T> m_data;
    
public:
    const T& read() const { return *m_data; }
    
    T& write() {
        if (m_data.use_count() > 1) {
            m_data = std::make_shared<T>(*m_data);
        }
        return *m_data;
    }
};
```

#### Advantages

- ✅ Efficient for read-heavy workloads
- ✅ Automatic deep copy when needed
- ✅ Reduced memory for unused copies

#### Disadvantages

- ⚠️ Adds complexity to access patterns (read vs write)
- ⚠️ Still requires Clone() for polymorphic types
- ⚠️ May have unexpected performance characteristics
- ⚠️ Harder to reason about ownership

---

## Recommendation

**Option 1: Clone Method Pattern** is the recommended approach because:

1. **Standard Pattern**: Widely recognized C++ idiom for polymorphic copying
2. **Type Safety**: Compile-time errors if Clone() not implemented
3. **Encapsulation**: Each type controls its own copying logic
4. **Future-Proof**: Easy to extend for new component types
5. **Full Copyability**: Components become proper value types

### Implementation Priority

1. **Phase 1**: Add Clone() to UIElement base class and all derived types
2. **Phase 2**: Add copy constructor/assignment to CUserInterface
3. **Phase 3**: Add copy constructor/assignment to CGrimoireMachina  
4. **Phase 4**: Write tests to verify EntityMemoryPool copyability

### Affected Files

| File | Change Type |
|------|------------|
| `src/user_interface/UIElement.h` | Add Clone() pure virtual |
| `src/user_interface/ButtonElement.h` | Implement Clone() |
| `src/user_interface/PanelElement.h` | Implement Clone() |
| `src/user_interface/DropDownButtonElement.h` | Implement Clone() |
| `src/user_interface/DropDownContainerElement.h` | Implement Clone() |
| `src/user_interface/DropDownItemElement.h` | Implement Clone() |
| `src/user_interface/DropDownListElement.h` | Implement Clone() |
| `src/components/CUserInterface.h` | Add copy constructor/assignment |
| `src/components/CGrimoireMachina.h` | Add copy constructor/assignment |

### Estimated Effort

- **Clone() implementations**: 2-3 hours
- **Copy constructors**: 1 hour
- **Tests**: 2 hours
- **Total**: ~6 hours

## Testing Strategy

### Unit Tests

```cpp
TEST_CASE("EntityMemoryPool is copyable", "[entity_memory]") {
    // Create pool with data
    EntityMemoryPool original;
    entity::memory::ResizeEntityMemoryPool(original, 5);
    
    // Set up test data in components
    auto& ui = entity::memory::GetComponent<CUserInterface>(0, original);
    ui.m_name = "TestUI";
    ui.m_visible = true;
    ui.m_root_element = std::make_unique<ButtonElement>();
    
    // Copy the pool
    EntityMemoryPool copy = original;  // Should work!
    
    // Verify independent copies
    auto& ui_copy = entity::memory::GetComponent<CUserInterface>(0, copy);
    REQUIRE(ui_copy.m_name == "TestUI");
    REQUIRE(ui_copy.m_visible == true);
    REQUIRE(ui_copy.m_root_element != nullptr);
    
    // Verify they are independent
    REQUIRE(ui_copy.m_root_element.get() != ui.m_root_element.get());
}
```

### Integration Tests

Verify tick-by-tick snapshot testing works with copied pools.

## Risks and Mitigation

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Incomplete Clone() | Medium | High | Add tests for each UIElement type |
| New fields not cloned | Medium | Medium | Code review, tests |
| Performance overhead | Low | Low | Avoid unnecessary copies |
| Breaking changes | Low | Medium | Ensure existing tests pass |

## Alternatives Considered

1. **Leave as non-copyable**: Limits testing and future features
2. **Serialize/deserialize**: Too slow, overkill for this use case
3. **Prototype pattern registry**: Over-engineering

## Future Work

- Consider `std::pmr` (polymorphic memory resource) for arena-based allocation
- Investigate copy elision optimization opportunities
- Add move-only semantic support where appropriate

## Conclusion

Making EntityMemoryPool copyable is achievable through the Clone method pattern with moderate implementation effort. This enables important testing infrastructure and prepares the codebase for future features requiring state cloning.

**Recommendation**: Proceed with Option 1 (Clone Method Pattern) implementation.

---

**Proposal Version**: 1.0
