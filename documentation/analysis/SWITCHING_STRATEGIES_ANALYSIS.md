# Switching Strategies for Concrete Data Types - Analysis

## Executive Summary

This document analyzes multiple strategies for switching on concrete data types in the SteamRot engine. When working with polymorphic data structures (base classes/interfaces with multiple derived implementations), code must determine the actual concrete type at runtime and execute type-specific logic. This analysis examines five distinct strategies currently used or available in SteamRot, with real-world examples from the codebase.

**Target Audience**: Architects, senior developers, and anyone implementing polymorphic data handling patterns.

**Reading Time**: 15-20 minutes

**Key Findings**:
- **Five viable strategies** identified: Static Cast, Dynamic Cast, Switch-Case with Type Tags, Visitor Pattern, and std::variant
- **No single best approach** - each strategy has optimal use cases
- **Existing usage**: SteamRot currently uses Static Cast (FlatBuffers), Dynamic Cast (UI elements), and Switch-Case (Events)
- **Recommendation**: Continue with current patterns; use Visitor for future complex polymorphic hierarchies

---

## Table of Contents

1. [Problem Statement](#problem-statement)
2. [Strategy 1: Static Cast with Manual Type Checking](#strategy-1-static-cast-with-manual-type-checking)
3. [Strategy 2: Dynamic Cast Chain](#strategy-2-dynamic-cast-chain)
4. [Strategy 3: Switch-Case with Type Tag Enums](#strategy-3-switch-case-with-type-tag-enums)
5. [Strategy 4: Visitor Pattern](#strategy-4-visitor-pattern)
6. [Strategy 5: std::variant with std::visit](#strategy-5-stdvariant-with-stdvisit)
7. [Comparative Analysis](#comparative-analysis)
8. [Recommendations](#recommendations)
9. [Migration Considerations](#migration-considerations)
10. [Future Considerations](#future-considerations)

---

## Problem Statement

When working with polymorphic data types, we frequently encounter the need to:

1. **Determine the concrete type** of an object at runtime
2. **Execute type-specific logic** based on that concrete type
3. **Maintain type safety** and avoid runtime errors
4. **Handle new types** gracefully as the codebase evolves

### Common Scenarios in SteamRot

#### Scenario A: Scene Data Configuration
```cpp
// Base class: SceneData
// Derived classes: FbsSceneData, SaveSceneData (future), TestSceneData (future)
void ConfigureScene(const SceneData* scene_data) {
    // How do we determine if it's FbsSceneData or SaveSceneData?
    // How do we access derived class members?
}
```

#### Scenario B: UI Element Rendering
```cpp
// Base class: UIElement
// Derived classes: ButtonElement, PanelElement, DropDownListElement, etc.
void DrawUIElement(const UIElement& element) {
    // How do we dispatch to the correct drawing function?
    // Each element type has unique rendering requirements
}
```

#### Scenario C: Event Data Processing
```cpp
// Union-like structure: EventData can be UserInputBitset, SceneChangePacket, etc.
void ProcessEvent(EventDataData data_type, const void* data) {
    // How do we safely cast void* to the correct type?
    // How do we handle new event data types?
}
```

### Requirements

A good switching strategy should:

1. ✅ **Type Safety**: Prevent invalid casts and undefined behavior
2. ✅ **Maintainability**: Easy to add new types without breaking existing code
3. ✅ **Performance**: Minimal runtime overhead
4. ✅ **Readability**: Clear intent and easy to understand
5. ✅ **Error Handling**: Graceful handling of unexpected types
6. ✅ **Compile-Time Checks**: Catch errors early when possible

---

## Strategy 1: Static Cast with Manual Type Checking

### Description

Use `static_cast` to convert base pointer/reference to derived type, with manual validation before casting. This approach assumes the caller has verified the type through external mechanisms (e.g., type tag, separate metadata).

### Implementation Pattern

```cpp
// Base and derived types
struct SceneData {
    SceneInfo scene_info;
};

struct FbsSceneData : public SceneData {
    const SceneDataFbs* scene_data_fbs;
};

// Static cast with validation
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(Scene& scene, const SceneData* scene_data) {
    
    // Cast to derived type
    FbsSceneData* fbs_scene_data = 
        const_cast<FbsSceneData*>(static_cast<const FbsSceneData*>(scene_data));
    
    // Validate the cast (manual null check)
    if (!fbs_scene_data)
        return std::unexpected(
            FailInfo(FailMode::InvalidCast, "SceneData is not FbsSceneData"));
    
    // Use derived type members
    scene.GetSceneInfo().type = fbs_scene_data->scene_info.type;
    return std::monostate{};
}
```

### Real-World Example from SteamRot

**File**: `src/scenes/FlatbuffersSceneConfigurator.cpp`

```cpp
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneInfo(Scene &scene,
                                                 const SceneData *scene_data) {
    // cast to derived SceneData type
    FbsSceneData *fbs_scene_data =
        const_cast<FbsSceneData *>(static_cast<const FbsSceneData *>(scene_data));

    // check its valid
    if (!fbs_scene_data)
        return std::unexpected(
            FailInfo(FailMode::InvalidCast, "SceneData is not FbsSceneData"));

    scene.GetSceneInfo().type = fbs_scene_data->scene_info.type;
    return std::monostate{};
}
```

### Pros

✅ **Zero Runtime Overhead**: No vtable lookup, no RTTI cost  
✅ **Simple and Direct**: Straightforward code flow  
✅ **Works with FlatBuffers**: Compatible with non-polymorphic generated code  
✅ **Explicit Control**: Developer controls exactly when and how casts happen  

### Cons

❌ **No Type Safety**: Incorrect casts cause undefined behavior  
❌ **Manual Validation Required**: Easy to forget null checks  
❌ **Assumes Prior Knowledge**: Caller must know the concrete type beforehand  
❌ **Silent Failures**: NULL check might not catch all invalid casts  
❌ **Brittle**: Adding new derived types requires updating all cast sites  

### When to Use

- ✅ Working with **FlatBuffers** or other code generation tools that don't support virtual functions
- ✅ **Performance-critical** paths where RTTI overhead is unacceptable
- ✅ **Type is known at compile time** via external context (e.g., factory pattern)
- ✅ **Single derived type** dominates usage (e.g., 95% of cases are FbsSceneData)

### When NOT to Use

- ❌ Multiple derived types with equal probability of use
- ❌ Runtime type identification is required without external hints
- ❌ Code must be extensible to new types without modification

---

## Strategy 2: Dynamic Cast Chain

### Description

Use `dynamic_cast` to safely check and convert types at runtime. The cast returns `nullptr` if the type doesn't match, allowing safe type checking through a chain of if-else statements.

### Implementation Pattern

```cpp
// Base class with virtual destructor (required for dynamic_cast)
class UIElement {
public:
    virtual ~UIElement() = default;
    sf::Vector2f position;
    sf::Vector2f size;
    // ... common members
};

// Derived classes
class ButtonElement : public UIElement {
public:
    std::string label;
    // ... button-specific members
};

class PanelElement : public UIElement {
public:
    sf::Color background_color;
    // ... panel-specific members
};

// Dynamic cast chain for type dispatch
void DrawUIElementDispatch(sf::RenderTexture& texture, const UIElement& element,
                          const UIStyle& style) {
    if (const auto* button = dynamic_cast<const ButtonElement*>(&element)) {
        DrawButtonElement(texture, *button, style);
    } else if (const auto* panel = dynamic_cast<const PanelElement*>(&element)) {
        DrawPanelElement(texture, *panel, style);
    } else if (const auto* list = dynamic_cast<const DropDownListElement*>(&element)) {
        DrawDropDownListElement(texture, *list, style);
    }
    // ... more types
}
```

### Real-World Example from SteamRot

**File**: `src/logic/logic_render.cpp`

```cpp
void DrawUIElementDispatch(sf::RenderTexture &texture, const UIElement &element,
                           const UIStyle &style) {
    // Type dispatch using dynamic_cast
    if (const auto *button = dynamic_cast<const ButtonElement *>(&element)) {
        DrawButtonElement(texture, *button, style);
    } else if (const auto *panel = dynamic_cast<const PanelElement *>(&element)) {
        DrawPanelElement(texture, *panel, style);
    } else if (const auto *list =
                   dynamic_cast<const DropDownListElement *>(&element)) {
        DrawDropDownListElement(texture, *list, style);
    } else if (const auto *item =
                   dynamic_cast<const DropDownItemElement *>(&element)) {
        DrawDropDownItemElement(texture, *item, style);
    } else if (const auto *dd_button =
                   dynamic_cast<const DropDownButtonElement *>(&element)) {
        DrawDropDownButtonElement(texture, *dd_button, style);
    } else if (const auto *container =
                   dynamic_cast<const DropDownContainerElement *>(&element)) {
        DrawDropDownContainerElement(texture, *container, style);
    }
}
```

**File**: `src/logic/logic_action.cpp`

```cpp
// Check for ButtonElement
if (ButtonElement *button_element =
        dynamic_cast<ButtonElement *>(&ui_element)) {
    // Process button logic
}
// Check for DropDownListElement
else if (DropDownListElement *drop_down_list_element =
             dynamic_cast<DropDownListElement *>(&ui_element)) {
    // Process dropdown logic
}
```

### Pros

✅ **Type Safety**: Returns nullptr for invalid casts, no undefined behavior  
✅ **Runtime Type Identification**: Can determine type without external information  
✅ **Built-in Language Feature**: No additional infrastructure needed  
✅ **Readable**: Clear intent of checking and casting  
✅ **Supports Complex Hierarchies**: Works with multiple inheritance  

### Cons

❌ **Runtime Cost**: RTTI lookup has performance overhead  
❌ **Requires Virtual Functions**: Base class must have virtual destructor  
❌ **Linear Search**: Each cast is attempted sequentially (O(n) for n types)  
❌ **Incompatible with FlatBuffers**: Generated FlatBuffers code lacks virtual functions  
❌ **No Exhaustiveness Checking**: Compiler won't warn if you forget a type  
❌ **Order Matters**: More common types should be checked first for performance  

### Performance Considerations

**Typical dynamic_cast overhead**: 50-200 CPU cycles per cast attempt
**Linear chain cost**: N attempts for N types in worst case

For 6 UI element types (as in SteamRot), worst case is 6 dynamic_cast attempts per element per frame.

**Mitigation**: Order checks by frequency (most common types first)

### When to Use

- ✅ **True polymorphism** with proper virtual functions
- ✅ **UI element rendering** where performance is adequate (60 FPS easily achievable)
- ✅ **Small to medium number of types** (< 10 types)
- ✅ **Non-critical performance paths** (not inner loops)
- ✅ **Type frequency varies widely** (can optimize order)

### When NOT to Use

- ❌ High-performance inner loops (called millions of times per frame)
- ❌ Large number of derived types (>15-20)
- ❌ FlatBuffers or POD structs without virtual functions
- ❌ Embedded systems or performance-constrained environments

---

## Strategy 3: Switch-Case with Type Tag Enums

### Description

Store a type identifier (enum) alongside the data pointer, then use a switch statement to dispatch to type-specific logic. This approach is essentially a manual vtable.

### Implementation Pattern

```cpp
// Type tag enum (often generated by FlatBuffers)
enum class EventDataData : uint8_t {
    EventDataData_NONE = 0,
    EventDataData_UserInputBitsetData = 1,
    EventDataData_SceneChangePacketData = 2,
    EventDataData_UserInterfaceNameData = 3
};

// Switching function
std::expected<EventData, FailInfo>
CreateEventData(const EventDataData data_type, const void* data) {
    
    switch (data_type) {
    case EventDataData::EventDataData_UserInputBitsetData: {
        auto user_input_bitset_data = 
            static_cast<const UserInputBitsetData*>(data);
        auto result = CreateUserInputBitset(*user_input_bitset_data);
        if (!result.has_value())
            return std::unexpected(result.error());
        return result.value();
    }
    
    case EventDataData::EventDataData_SceneChangePacketData: {
        auto scene_change_packet_data = 
            static_cast<const SceneChangePacketData*>(data);
        auto result = CreateSceneChangePacket(*scene_change_packet_data);
        if (!result.has_value())
            return std::unexpected(result.error());
        return result.value();
    }
    
    case EventDataData::EventDataData_UserInterfaceNameData: {
        auto ui_name_data = static_cast<const UserInterfaceNameData*>(data);
        auto result = CreateUserInterfaceName(*ui_name_data);
        if (!result.has_value())
            return std::unexpected(result.error());
        return result.value();
    }
    
    case EventDataData::EventDataData_NONE: {
        return std::monostate();
    }
    
    default:
        return std::unexpected(
            FailInfo{FailMode::EnumValueNotHandled,
                     "CreateEventData: EventDataData type not handled."});
    }
}
```

### Real-World Example from SteamRot

**File**: `src/events/event_factory.cpp`

```cpp
std::expected<EventData, FailInfo>
CreateEventData(const EventDataData data_type, const void *data) {

    switch (data_type) {
    case EventDataData::EventDataData_UserInputBitsetData: {
        // cast data to UserInputBitsetData
        auto user_input_bitset_data =
            static_cast<const UserInputBitsetData *>(data);

        // convert to UserInputBitset
        auto user_input_bitset_result =
            CreateUserInputBitset(*user_input_bitset_data);
        if (!user_input_bitset_result.has_value())
            return std::unexpected(user_input_bitset_result.error());

        return user_input_bitset_result.value();
    }
    case EventDataData::EventDataData_SceneChangePacketData: {
        // cast data to SceneChangePacketData
        auto scene_change_packet_data =
            static_cast<const SceneChangePacketData *>(data);

        // convert to SceneChangePacket
        auto scene_change_packet_result =
            CreateSceneChangePacket(*scene_change_packet_data);
        if (!scene_change_packet_result.has_value())
            return std::unexpected(scene_change_packet_result.error());

        return scene_change_packet_result.value();
    }

    case EventDataData::EventDataData_UserInterfaceNameData: {
        // cast data to UserInterfaceNameData
        auto ui_name_data = static_cast<const UserInterfaceNameData *>(data);

        // convert to UserInterfaceName
        auto ui_name_result = CreateUserInterfaceName(*ui_name_data);
        if (!ui_name_result.has_value())
            return std::unexpected(ui_name_result.error());

        return ui_name_result.value();
    }

    case EventDataData::EventDataData_NONE: {
        return std::monostate();
    }

    default:
        return std::unexpected(
            FailInfo{FailMode::EnumValueNotHandled,
                     "CreateEventData: EventDataData type not handled."});
    }
}
```

### Pros

✅ **Excellent Performance**: O(1) lookup via jump table  
✅ **Compiler Warnings**: Can warn about unhandled enum values (with `-Wswitch`)  
✅ **Exhaustiveness Checking**: Compiler can verify all cases are handled  
✅ **Clear Structure**: All cases in one place  
✅ **Works with POD Types**: No virtual functions required  
✅ **Compatible with FlatBuffers**: FlatBuffers generates type enums automatically  
✅ **Explicit Default Handling**: Can catch unexpected enum values  

### Cons

❌ **Requires Type Tag Storage**: Must maintain enum alongside pointer  
❌ **Manual Synchronization**: Enum and actual type must stay in sync  
❌ **Centralized Dispatch**: All type-specific logic must go through switch  
❌ **Code Duplication**: Similar patterns repeated in each case  
❌ **Void Pointer**: Often paired with `void*`, losing some type safety  

### Performance Analysis

**Jump table cost**: ~5-10 CPU cycles (constant time)  
**Comparison**: 5-20x faster than dynamic_cast chain for 5+ types

### Compiler Optimization: Jump Tables

Modern compilers optimize switch statements into jump tables when possible:

```cpp
// Switch statement
switch (enum_value) {
    case Type1: handle_type1(); break;
    case Type2: handle_type2(); break;
    case Type3: handle_type3(); break;
}

// Compiler generates jump table (pseudo-assembly)
jump_table[enum_value]();  // O(1) indirect jump
```

**Requirements for jump table optimization**:
- Enum values are dense (0, 1, 2, 3... or close)
- Sufficient number of cases (typically 4+)
- Default case doesn't prevent optimization

### When to Use

- ✅ **FlatBuffers unions** (type enum + void pointer pattern)
- ✅ **Performance-critical dispatch** (event processing, serialization)
- ✅ **Enum-driven design** where types are known at compile time
- ✅ **Large number of types** (>10) where dynamic_cast would be too slow
- ✅ **Want compiler exhaustiveness checking**

### When NOT to Use

- ❌ Open-ended type hierarchies (plugins, user-defined types)
- ❌ Types not known at compile time
- ❌ Strong preference for OOP polymorphism

### Best Practices

1. **Enable compiler warnings**: Use `-Wswitch` to catch missing cases
2. **Always include default**: Handle unexpected enum values gracefully
3. **Use strong typing**: Avoid raw integers, prefer `enum class`
4. **Keep cases simple**: Delegate to functions rather than inline logic
5. **Document enum values**: Comment each enum variant's purpose

```cpp
// Good: Enable warnings
#pragma GCC diagnostic error "-Wswitch"

// Good: Delegate to functions
switch (type) {
case Type::Foo: return HandleFoo(data);
case Type::Bar: return HandleBar(data);
}

// Bad: Complex logic inline
switch (type) {
case Type::Foo: {
    // 50 lines of logic
    // Hard to read and maintain
}
}
```

---

## Strategy 4: Visitor Pattern

### Description

The Visitor Pattern uses double dispatch to achieve type-specific behavior without explicit type checking. The base class defines an `Accept` method that calls back to a visitor interface with the concrete type.

### Implementation Pattern

```cpp
// Forward declaration of visitor
class SceneDataVisitor;

// Base class with Accept method
class SceneData {
public:
    virtual ~SceneData() = default;
    virtual void Accept(SceneDataVisitor& visitor) const = 0;
    SceneInfo scene_info;
};

// Visitor interface
class SceneDataVisitor {
public:
    virtual ~SceneDataVisitor() = default;
    virtual void Visit(const FbsSceneData& data) = 0;
    virtual void Visit(const SaveSceneData& data) = 0;
    virtual void Visit(const TestSceneData& data) = 0;
};

// Derived classes implement Accept
class FbsSceneData : public SceneData {
public:
    void Accept(SceneDataVisitor& visitor) const override {
        visitor.Visit(*this);  // Calls visitor with concrete type
    }
    const SceneDataFbs* scene_data_fbs;
};

class SaveSceneData : public SceneData {
public:
    void Accept(SceneDataVisitor& visitor) const override {
        visitor.Visit(*this);
    }
    // ... save-specific members
};

// Concrete visitor implementation
class SceneConfigurationVisitor : public SceneDataVisitor {
private:
    Scene& m_scene;
    
public:
    SceneConfigurationVisitor(Scene& scene) : m_scene(scene) {}
    
    void Visit(const FbsSceneData& data) override {
        // FlatBuffers-specific configuration
        m_scene.GetSceneInfo().type = data.scene_info.type;
        // ... more configuration
    }
    
    void Visit(const SaveSceneData& data) override {
        // Save file-specific configuration
        m_scene.GetSceneInfo().type = data.scene_info.type;
        // Restore saved state
        // ...
    }
    
    void Visit(const TestSceneData& data) override {
        // Test-specific configuration
        m_scene.GetSceneInfo().type = data.scene_info.type;
        // ...
    }
};

// Usage
void ConfigureScene(Scene& scene, const SceneData& scene_data) {
    SceneConfigurationVisitor visitor(scene);
    scene_data.Accept(visitor);  // Double dispatch to correct Visit method
}
```

### Advanced Visitor Example: Multiple Operations

```cpp
// Different visitors for different operations
class SceneDataDebugPrinter : public SceneDataVisitor {
public:
    void Visit(const FbsSceneData& data) override {
        std::cout << "FlatBuffers Scene Data\n";
        std::cout << "  Type: " << data.scene_info.type << "\n";
        // ... print FlatBuffers-specific info
    }
    
    void Visit(const SaveSceneData& data) override {
        std::cout << "Save File Scene Data\n";
        std::cout << "  Type: " << data.scene_info.type << "\n";
        // ... print save-specific info
    }
    
    void Visit(const TestSceneData& data) override {
        std::cout << "Test Scene Data\n";
        // ... print test-specific info
    }
};

class SceneDataValidator : public SceneDataVisitor {
private:
    std::vector<std::string> m_errors;
    
public:
    void Visit(const FbsSceneData& data) override {
        if (!data.scene_data_fbs)
            m_errors.push_back("FlatBuffers data is null");
        // ... validate FlatBuffers data
    }
    
    void Visit(const SaveSceneData& data) override {
        // ... validate save file data
    }
    
    void Visit(const TestSceneData& data) override {
        // ... validate test data
    }
    
    const std::vector<std::string>& GetErrors() const { return m_errors; }
};

// Usage: Multiple operations on same data
const SceneData& data = /* ... */;

SceneConfigurationVisitor config_visitor(scene);
data.Accept(config_visitor);

SceneDataDebugPrinter debug_visitor;
data.Accept(debug_visitor);

SceneDataValidator validation_visitor;
data.Accept(validation_visitor);
if (!validation_visitor.GetErrors().empty()) {
    // Handle validation errors
}
```

### Pros

✅ **Type Safety**: No manual casting required  
✅ **Open/Closed Principle**: Add new operations without modifying base classes  
✅ **Centralized Operations**: All type-specific logic for one operation in one place  
✅ **Compile-Time Checking**: Compiler ensures all types have Visit methods  
✅ **Double Dispatch**: Resolves both object type and visitor type at runtime  
✅ **Multiple Operations**: Easy to add new visitors for new operations  

### Cons

❌ **Complexity**: Requires significant boilerplate (Accept methods, visitor interface)  
❌ **Not Extensible to New Types**: Adding a new derived class requires updating visitor interface  
❌ **Tight Coupling**: Base class knows about visitor, visitor knows about all derived classes  
❌ **Incompatible with FlatBuffers**: Generated code can't implement Accept methods  
❌ **Verbose**: More code than other strategies  
❌ **Circular Dependencies**: Can create header dependency cycles  

### When to Use

- ✅ **Multiple operations** on the same type hierarchy (rendering, serialization, validation, etc.)
- ✅ **Operations change frequently** but types are stable
- ✅ **Complex type-specific logic** that benefits from being grouped by operation
- ✅ **Academic/clean code** projects where pattern purity matters
- ✅ **True polymorphic hierarchies** with virtual functions

### When NOT to Use

- ❌ Type hierarchy changes frequently (every new type breaks visitor interface)
- ❌ FlatBuffers or other code generation (can't modify generated code)
- ❌ Simple operations that don't warrant the complexity
- ❌ Performance-critical code (virtual function overhead + indirection)

### Visitor Pattern: Compile-Time Variant (Modern C++)

Modern C++ can implement a compile-time visitor using templates:

```cpp
// Type-based visitor using overloading
struct SceneDataVisitor {
    Scene& scene;
    
    void operator()(const FbsSceneData& data) {
        scene.GetSceneInfo().type = data.scene_info.type;
    }
    
    void operator()(const SaveSceneData& data) {
        scene.GetSceneInfo().type = data.scene_info.type;
        // ... load saved state
    }
    
    void operator()(const TestSceneData& data) {
        scene.GetSceneInfo().type = data.scene_info.type;
    }
};

// Usage with std::variant (see Strategy 5)
std::variant<FbsSceneData, SaveSceneData, TestSceneData> scene_data_variant;
std::visit(SceneDataVisitor{scene}, scene_data_variant);
```

---

## Strategy 5: std::variant with std::visit

### Description

Use `std::variant` (C++17) to create a type-safe union that can hold one of several alternative types. Use `std::visit` to apply operations based on the active type. This is the modern C++ approach that combines the benefits of type safety, performance, and compile-time checking.

### Implementation Pattern

```cpp
// Define variant with all possible types
using SceneDataVariant = std::variant<
    FbsSceneData,
    SaveSceneData,
    TestSceneData
>;

// Visitor using overload pattern
template<class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;  // C++17 deduction guide

// Configure scene using std::visit
std::expected<std::monostate, FailInfo>
ConfigureScene(Scene& scene, const SceneDataVariant& scene_data) {
    
    return std::visit(Overload{
        [&](const FbsSceneData& data) -> std::expected<std::monostate, FailInfo> {
            scene.GetSceneInfo().type = data.scene_info.type;
            // ... FlatBuffers-specific configuration
            return std::monostate{};
        },
        [&](const SaveSceneData& data) -> std::expected<std::monostate, FailInfo> {
            scene.GetSceneInfo().type = data.scene_info.type;
            // ... Save file-specific configuration
            return std::monostate{};
        },
        [&](const TestSceneData& data) -> std::expected<std::monostate, FailInfo> {
            scene.GetSceneInfo().type = data.scene_info.type;
            // ... Test-specific configuration
            return std::monostate{};
        }
    }, scene_data);
}

// Alternative: Separate visitor struct
struct SceneConfigurationVisitor {
    Scene& scene;
    
    std::expected<std::monostate, FailInfo> operator()(const FbsSceneData& data) {
        scene.GetSceneInfo().type = data.scene_info.type;
        return std::monostate{};
    }
    
    std::expected<std::monostate, FailInfo> operator()(const SaveSceneData& data) {
        scene.GetSceneInfo().type = data.scene_info.type;
        return std::monostate{};
    }
    
    std::expected<std::monostate, FailInfo> operator()(const TestSceneData& data) {
        scene.GetSceneInfo().type = data.scene_info.type;
        return std::monostate{};
    }
};

// Usage
std::expected<std::monostate, FailInfo>
ConfigureScene(Scene& scene, const SceneDataVariant& scene_data) {
    return std::visit(SceneConfigurationVisitor{scene}, scene_data);
}
```

### Advanced std::variant Techniques

#### 1. Type Queries

```cpp
// Check active type
if (std::holds_alternative<FbsSceneData>(scene_data)) {
    // scene_data currently holds FbsSceneData
}

// Get active type index
size_t index = scene_data.index();

// Safe access with std::get_if
if (auto* fbs = std::get_if<FbsSceneData>(&scene_data)) {
    // Use fbs pointer
}

// Unsafe access with std::get (throws if wrong type)
try {
    FbsSceneData& fbs = std::get<FbsSceneData>(scene_data);
} catch (const std::bad_variant_access& e) {
    // Handle error
}
```

#### 2. Generic Visitors

```cpp
// Visit with generic lambda
auto result = std::visit([](const auto& data) {
    // Code that works for any type
    return data.scene_info.type;
}, scene_data);

// Visit with SFINAE constraints
std::visit([](const auto& data) {
    if constexpr (std::is_same_v<std::decay_t<decltype(data)>, FbsSceneData>) {
        // FlatBuffers-specific code
    } else if constexpr (std::is_same_v<std::decay_t<decltype(data)>, SaveSceneData>) {
        // Save-specific code
    }
}, scene_data);
```

#### 3. Return Value Handling

```cpp
// Return different types based on input
auto result = std::visit([](const auto& data) -> std::variant<int, std::string> {
    if constexpr (std::is_same_v<std::decay_t<decltype(data)>, FbsSceneData>) {
        return 42;  // Return int for FlatBuffers
    } else {
        return "saved";  // Return string for other types
    }
}, scene_data);
```

### Real-World Application in SteamRot

**How it could be used for Scene Data:**

```cpp
// Define variant type
using SceneDataVariant = std::variant<
    FbsSceneData,
    SaveSceneData,
    TestSceneData
>;

// Provider returns variant instead of pointer
class ISceneDataProvider {
public:
    virtual std::expected<SceneDataVariant, FailInfo> 
    LoadSceneData(SceneType type) = 0;
};

// Configurator accepts variant
class SceneConfigurator {
public:
    std::expected<std::monostate, FailInfo>
    ConfigureScene(Scene& scene, const SceneDataVariant& scene_data) {
        return std::visit(Overload{
            [&](const FbsSceneData& data) { 
                return ConfigureFbsScene(scene, data); 
            },
            [&](const SaveSceneData& data) { 
                return ConfigureSaveScene(scene, data); 
            },
            [&](const TestSceneData& data) { 
                return ConfigureTestScene(scene, data); 
            }
        }, scene_data);
    }
    
private:
    std::expected<std::monostate, FailInfo> 
    ConfigureFbsScene(Scene& scene, const FbsSceneData& data) { /* ... */ }
    
    std::expected<std::monostate, FailInfo>
    ConfigureSaveScene(Scene& scene, const SaveSceneData& data) { /* ... */ }
    
    std::expected<std::monostate, FailInfo>
    ConfigureTestScene(Scene& scene, const TestSceneData& data) { /* ... */ }
};
```

### Pros

✅ **Type Safety**: Cannot access wrong type, compiler enforced  
✅ **No Heap Allocation**: Stack-based, no pointers needed  
✅ **Exhaustiveness Checking**: Compiler ensures all types handled  
✅ **Performance**: Excellent (minimal overhead)  
✅ **Modern C++**: Idiomatic C++17/20 approach  
✅ **Value Semantics**: No pointer management, RAII-friendly  
✅ **Compile-Time Dispatch**: Zero runtime overhead when possible  
✅ **Exception Safety**: Strong exception guarantees  

### Cons

❌ **Requires Refactoring**: Changes from pointer-based to value-based design  
❌ **Size Overhead**: Variant size = max(all types) + discriminator  
❌ **Not Polymorphic**: Cannot use base class pointers  
❌ **All Types Known**: Cannot add types at runtime or via plugins  
❌ **Copy Semantics**: Copying variant copies the active value  
❌ **C++17 Required**: Not available in C++14 and earlier  

### Performance Characteristics

**Memory Layout**:
```
sizeof(std::variant<A, B, C>) = max(sizeof(A), sizeof(B), sizeof(C)) 
                                 + sizeof(discriminator)
                                 + padding
```

**Example**:
```cpp
struct FbsSceneData { char data[100]; };     // 100 bytes
struct SaveSceneData { char data[200]; };    // 200 bytes
struct TestSceneData { char data[50]; };     // 50 bytes

using Variant = std::variant<FbsSceneData, SaveSceneData, TestSceneData>;
// sizeof(Variant) ≈ 200 + 8 = 208 bytes (on most platforms)
```

**std::visit Performance**:
- Optimized to jump table or inline code
- Similar performance to switch-case
- No virtual function overhead
- No heap allocation overhead

### When to Use

- ✅ **All types known at compile time** and are fixed
- ✅ **Value semantics preferred** over pointer semantics
- ✅ **Performance critical** code paths
- ✅ **Modern codebase** using C++17 or later
- ✅ **Type safety is paramount** (safety-critical systems)
- ✅ **Small to medium number of types** (variant size is manageable)

### When NOT to Use

- ❌ Open-ended type hierarchies (plugins, user extensions)
- ❌ Types not known at compile time
- ❌ Large number of very large types (memory overhead)
- ❌ Existing pointer-based architecture (major refactoring required)
- ❌ Need to store types polymorphically in containers
- ❌ C++14 or earlier codebase

### Migration Path to std::variant

**Current Architecture** (Pointer-based):
```cpp
class SceneData { /* base */ };
class FbsSceneData : public SceneData { /* derived */ };

std::unique_ptr<SceneData> LoadScene() {
    return std::make_unique<FbsSceneData>();
}

void Configure(const SceneData* data) {
    auto* fbs = static_cast<const FbsSceneData*>(data);
}
```

**Step 1**: Wrap in variant (hybrid approach)
```cpp
using SceneDataPtr = std::unique_ptr<SceneData>;
using SceneDataVariant = std::variant<SceneDataPtr, /* future types */>;

SceneDataVariant LoadScene() {
    return std::make_unique<FbsSceneData>();
}
```

**Step 2**: Full conversion to value-based
```cpp
using SceneDataVariant = std::variant<FbsSceneData, SaveSceneData>;

SceneDataVariant LoadScene() {
    return FbsSceneData{/* ... */};
}

void Configure(const SceneDataVariant& data) {
    std::visit([](const auto& d) { /* configure */ }, data);
}
```

---

## Comparative Analysis

### Summary Table

| Strategy | Type Safety | Performance | Maintainability | Extensibility | Compiler Help | FlatBuffers Compatible |
|----------|-------------|-------------|-----------------|---------------|---------------|------------------------|
| **Static Cast** | ❌ Low | ✅✅ Excellent | ⚠️ Medium | ❌ Poor | ❌ None | ✅ Yes |
| **Dynamic Cast** | ✅ High | ⚠️ Medium | ✅ Good | ✅ Good | ❌ None | ❌ No |
| **Switch-Case** | ⚠️ Medium | ✅✅ Excellent | ✅ Good | ⚠️ Medium | ✅ Warnings | ✅ Yes |
| **Visitor** | ✅✅ Excellent | ⚠️ Medium | ⚠️ Complex | ❌ Poor | ✅ Compile Errors | ❌ No |
| **std::variant** | ✅✅ Excellent | ✅✅ Excellent | ✅✅ Excellent | ❌ Poor | ✅✅ Full | ⚠️ Requires Refactoring |

### Performance Comparison

**Relative Performance** (normalized to static_cast = 1.0):

| Strategy | CPU Cycles (avg) | Relative Cost | Scalability |
|----------|------------------|---------------|-------------|
| Static Cast | 5-10 | 1.0x | O(1) |
| Switch-Case | 10-20 | 1.5x | O(1) |
| std::variant | 10-25 | 1.5-2x | O(1) |
| Dynamic Cast (first match) | 50-100 | 5-10x | O(1) |
| Dynamic Cast (nth match) | 50n-100n | 5n-10n x | O(n) |
| Visitor Pattern | 100-200 | 10-20x | O(1) |

**Note**: Actual performance depends on hardware, compiler, and optimization level.

### Use Case Decision Tree

```
Are all types known at compile time?
├─ YES → Is performance critical (inner loop)?
│   ├─ YES → Is FlatBuffers involved?
│   │   ├─ YES → Use Switch-Case (Strategy 3)
│   │   └─ NO → Can refactor to value semantics?
│   │       ├─ YES → Use std::variant (Strategy 5)
│   │       └─ NO → Use Switch-Case (Strategy 3)
│   └─ NO → Do you have proper virtual functions?
│       ├─ YES → How many types (<10)?
│       │   ├─ YES → Use Dynamic Cast (Strategy 2)
│       │   └─ NO → Use Visitor (Strategy 4) or std::variant (Strategy 5)
│       └─ NO → Use Switch-Case (Strategy 3) or Static Cast (Strategy 1)
└─ NO (runtime extensibility needed)
    → Use Dynamic Cast (Strategy 2) or plugin architecture
```

### Current Usage in SteamRot

| Component | Strategy Used | Justification |
|-----------|---------------|---------------|
| **Scene Configuration** | Static Cast (Strategy 1) | FlatBuffers compatibility, single dominant type (FbsSceneData) |
| **UI Element Rendering** | Dynamic Cast (Strategy 2) | True polymorphism, ~6 types, not performance-critical |
| **Event Data Processing** | Switch-Case (Strategy 3) | FlatBuffers enum + void pointer, performance-critical |

### Trade-off Matrix

|  | Static Cast | Dynamic Cast | Switch-Case | Visitor | std::variant |
|---|-------------|--------------|-------------|---------|--------------|
| **Adding New Type** | Manual search & update all sites | Add type + update chains | Update switch cases | Update visitor interface | Update variant typedef + visitors |
| **Code Locality** | Scattered across callers | Scattered across callers | Centralized in switch | Centralized in visitor | Centralized in visit |
| **Error Detection Time** | Runtime (undefined behavior) | Runtime (nullptr) | Compile-time (with warnings) | Compile-time (pure virtual) | Compile-time (type error) |
| **Boilerplate** | Minimal | Minimal | Medium | High | Low-Medium |
| **Learning Curve** | Low | Low | Low | High | Medium |

---

## Recommendations

### For SteamRot Codebase

Based on the analysis and current usage:

#### ✅ KEEP Current Approaches

1. **Scene Configuration**: Continue using Static Cast (Strategy 1)
   - FlatBuffers dominates (FbsSceneData)
   - SaveSceneData and TestSceneData are future additions
   - When implemented, consider migrating to Switch-Case (Strategy 3) with a SceneDataType enum

2. **UI Element Rendering**: Continue using Dynamic Cast (Strategy 2)
   - True polymorphic hierarchy
   - ~6 element types (manageable)
   - Not in inner loop (renders once per frame)
   - Readability > performance for UI rendering

3. **Event Processing**: Continue using Switch-Case (Strategy 3)
   - FlatBuffers generates enum automatically
   - Performance-critical path
   - Clear exhaustiveness checking
   - Excellent compiler support

#### 🎯 ADOPT for New Code

4. **New Polymorphic Hierarchies**: Consider std::variant (Strategy 5)
   - For new subsystems being designed from scratch
   - When all types are known at compile time
   - Especially for value-based types (components, packets)
   - Example: New component type variants, resource variants

5. **Complex Multi-Operation Scenarios**: Consider Visitor (Strategy 4)
   - If you need 3+ distinct operations on the same hierarchy
   - Example: Serialize, Deserialize, Validate, Debug print
   - Only if types are stable (don't add new types frequently)

#### ❌ AVOID

6. **Don't Mix Strategies**: Be consistent within a subsystem
   - Don't use dynamic_cast in event system (use existing switch-case)
   - Don't use static_cast in UI rendering (use existing dynamic_cast)

### General Guidelines

#### Choose Static Cast (Strategy 1) when:
- Working with FlatBuffers generated code
- Performance is absolutely critical
- Single dominant type (90%+ of cases)
- External context provides type information

#### Choose Dynamic Cast (Strategy 2) when:
- True polymorphic hierarchy with virtual functions
- Small to medium number of types (<10)
- Readability matters more than micro-optimization
- Types are added/removed infrequently

#### Choose Switch-Case (Strategy 3) when:
- FlatBuffers union with type enum
- Performance matters
- Want compiler warnings for unhandled cases
- Types are enumerable and known at compile time

#### Choose Visitor (Strategy 4) when:
- Multiple distinct operations on same hierarchy
- Operations change more than types
- Academic/architectural purity is important
- Type hierarchy is stable

#### Choose std::variant (Strategy 5) when:
- Starting fresh codebase or subsystem
- All types known at compile time
- C++17 or later
- Value semantics preferred
- Maximum type safety desired

---

## Migration Considerations

### Migrating from Static Cast to Switch-Case

**Scenario**: Adding SaveSceneData and TestSceneData to Scene configuration

**Current Code**:
```cpp
// Static cast assumes FbsSceneData
FbsSceneData* fbs = static_cast<FbsSceneData*>(scene_data);
```

**Step 1**: Add SceneDataType enum
```cpp
enum class SceneDataType {
    FlatBuffers,
    SaveFile,
    Test
};

struct SceneData {
    SceneDataType type;  // Add type tag
    SceneInfo scene_info;
};
```

**Step 2**: Update all derived classes
```cpp
struct FbsSceneData : public SceneData {
    FbsSceneData() { type = SceneDataType::FlatBuffers; }
    const SceneDataFbs* scene_data_fbs;
};

struct SaveSceneData : public SceneData {
    SaveSceneData() { type = SceneDataType::SaveFile; }
    // ... save-specific members
};
```

**Step 3**: Replace static_cast with switch
```cpp
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(Scene& scene, const SceneData* scene_data) {
    
    switch (scene_data->type) {
    case SceneDataType::FlatBuffers: {
        auto* fbs = static_cast<const FbsSceneData*>(scene_data);
        scene.GetSceneInfo().type = fbs->scene_info.type;
        break;
    }
    case SceneDataType::SaveFile: {
        auto* save = static_cast<const SaveSceneData*>(scene_data);
        // ... configure from save file
        break;
    }
    case SceneDataType::Test: {
        auto* test = static_cast<const TestSceneData*>(scene_data);
        // ... configure test data
        break;
    }
    default:
        return std::unexpected(
            FailInfo(FailMode::EnumValueNotHandled, "Unknown SceneDataType"));
    }
    
    return std::monostate{};
}
```

**Step 4**: Enable compiler warnings
```cmake
# CMakeLists.txt
target_compile_options(steamrot PRIVATE -Wswitch)
```

### Migrating from Dynamic Cast to std::variant

**Scenario**: Refactoring UI elements to value semantics

**Current Code**:
```cpp
std::unique_ptr<UIElement> element = CreateElement();

if (auto* button = dynamic_cast<ButtonElement*>(element.get())) {
    DrawButton(*button);
} else if (auto* panel = dynamic_cast<PanelElement*>(element.get())) {
    DrawPanel(*panel);
}
```

**Step 1**: Define variant
```cpp
using UIElementVariant = std::variant<
    ButtonElement,
    PanelElement,
    DropDownListElement,
    DropDownItemElement,
    DropDownButtonElement,
    DropDownContainerElement
>;
```

**Step 2**: Create factory returning variant
```cpp
UIElementVariant CreateElement(UIElementType type) {
    switch (type) {
    case UIElementType::Button:
        return ButtonElement{/* ... */};
    case UIElementType::Panel:
        return PanelElement{/* ... */};
    // ...
    }
}
```

**Step 3**: Replace dynamic_cast with std::visit
```cpp
UIElementVariant element = CreateElement(type);

std::visit(Overload{
    [&](const ButtonElement& btn) { DrawButton(btn); },
    [&](const PanelElement& pnl) { DrawPanel(pnl); },
    [&](const DropDownListElement& ddl) { DrawDropDownList(ddl); },
    // ...
}, element);
```

**Step 4**: Handle nested elements
```cpp
struct UIElement {
    sf::Vector2f position;
    sf::Vector2f size;
    std::vector<UIElementVariant> child_elements;  // Recursive variant
};

void DrawNestedElements(const UIElementVariant& element) {
    std::visit([](const auto& e) {
        DrawElement(e);
        for (const auto& child : e.child_elements) {
            DrawNestedElements(child);  // Recursive
        }
    }, element);
}
```

### Risks and Mitigation

#### Risk 1: Breaking Existing Code
**Mitigation**: 
- Migrate incrementally (one subsystem at a time)
- Use feature flags to enable new code path
- Run comprehensive tests after each change
- Keep both implementations temporarily during transition

#### Risk 2: Performance Regression
**Mitigation**:
- Benchmark before and after migration
- Use profiler to identify hotspots
- Compare assembly output (godbolt.org)
- Optimize based on measurements, not assumptions

#### Risk 3: Increased Binary Size
**Mitigation**:
- Variant adds size overhead (max type size + discriminator)
- Profile memory usage in resource-constrained scenarios
- Consider std::any for rare cases where size matters more than type safety

---

## Future Considerations

### C++20 and Beyond

#### 1. Concepts for Type Constraints

```cpp
// Define concept for SceneData types
template<typename T>
concept SceneDataType = std::is_base_of_v<SceneData, T> && requires(T t) {
    { t.scene_info } -> std::convertible_to<SceneInfo>;
};

// Use concept in generic functions
template<SceneDataType T>
void ConfigureScene(Scene& scene, const T& scene_data) {
    // Guaranteed to have scene_info member
}
```

#### 2. Pattern Matching (C++23+)

```cpp
// Future C++ pattern matching syntax (proposal)
inspect (scene_data) {
    <FbsSceneData> fbs => ConfigureFbsScene(scene, fbs),
    <SaveSceneData> save => ConfigureSaveScene(scene, save),
    <TestSceneData> test => ConfigureTestScene(scene, test)
};
```

#### 3. Expanded std::variant Support

- **std::variant2** (proposed): Better support for recursive variants
- **std::expected<std::variant<...>>**: Cleaner error handling patterns
- **Improved visit syntax**: Less boilerplate for common cases

### Integration with Other Systems

#### 1. Reflection (C++26 potential)

```cpp
// Hypothetical reflection-based dispatch
template<typename Base>
void Dispatch(const Base& obj, auto visitor) {
    constexpr auto derived_types = get_derived_types<Base>();
    
    for_each_type(derived_types, [&](auto type_tag) {
        using DerivedT = typename decltype(type_tag)::type;
        if (typeid(obj) == typeid(DerivedT)) {
            visitor(static_cast<const DerivedT&>(obj));
        }
    });
}
```

#### 2. Serialization Integration

```cpp
// Generic serialization with variant
template<typename... Ts>
void Serialize(const std::variant<Ts...>& data, OutputStream& out) {
    out << data.index();  // Write discriminator
    std::visit([&](const auto& value) {
        SerializeValue(value, out);  // Dispatch to correct serializer
    }, data);
}
```

### Performance Optimization Techniques

#### 1. Likely/Unlikely Annotations

```cpp
// Hint to compiler about common types
if (auto* fbs = dynamic_cast<FbsSceneData*>(data)) [[likely]] {
    // Most common case
} else if (auto* save = dynamic_cast<SaveSceneData*>(data)) [[unlikely]] {
    // Rare case
}
```

#### 2. Profile-Guided Optimization (PGO)

- Collect runtime type distribution data
- Reorder switch cases based on frequency
- Compiler uses profile data to optimize branch prediction

```cpp
// Reorder based on profiling
switch (type) {
case MostCommonType:  // 80% of cases
    // ...
case SecondMostCommon:  // 15% of cases
    // ...
case RareType:  // 5% of cases
    // ...
}
```

#### 3. Type-Erased Dispatch Table

```cpp
// For very large number of types, use function pointer table
using DispatchFunc = void(*)(const void*, Scene&);

const std::unordered_map<std::type_index, DispatchFunc> dispatch_table = {
    {typeid(FbsSceneData), [](const void* p, Scene& s) {
        ConfigureFbsScene(s, *static_cast<const FbsSceneData*>(p));
    }},
    // ... more entries
};

void Configure(const SceneData& data, Scene& scene) {
    auto it = dispatch_table.find(typeid(data));
    if (it != dispatch_table.end()) {
        it->second(&data, scene);
    }
}
```

### Research Directions

#### 1. Compile-Time Polymorphism

- Explore using CRTP (Curiously Recurring Template Pattern) instead of runtime polymorphism
- Trade-off: Binary size vs. runtime performance

#### 2. Hot-Path Specialization

- Identify most common type in profiler
- Generate specialized fast-path for that type
- Fall back to generic dispatch for other types

```cpp
// Fast path for common case
if (std::holds_alternative<FbsSceneData>(data)) {
    return FastPathConfigureFbs(scene, std::get<FbsSceneData>(data));
}
// Slow path for everything else
return GenericConfigure(scene, data);
```

#### 3. Adaptive Dispatch

- Runtime system learns type distribution
- Dynamically reorders checks or builds optimized dispatch table
- Useful for long-running servers where patterns change over time

---

## Conclusion

This analysis has examined five distinct strategies for switching on concrete data types in SteamRot:

1. **Static Cast** - Minimal overhead, works with FlatBuffers, requires careful manual validation
2. **Dynamic Cast** - Type-safe, readable, small runtime cost, requires virtual functions
3. **Switch-Case** - Excellent performance, compiler checking, requires type enum
4. **Visitor Pattern** - Clean architecture, supports multiple operations, significant boilerplate
5. **std::variant** - Modern C++, type-safe, performant, requires refactoring to value semantics

**Key Takeaways**:

- ✅ **No single best solution** - each strategy has optimal use cases
- ✅ **Current SteamRot usage is appropriate** - strategies match their use cases
- ✅ **Consider std::variant for new code** - best modern C++ approach when applicable
- ✅ **Consistency matters** - use same strategy within a subsystem
- ✅ **Measure before optimizing** - profile actual performance, don't assume

**Recommendations Summary**:

| Subsystem | Current Strategy | Recommendation |
|-----------|------------------|----------------|
| Scene Configuration | Static Cast | Keep for now; migrate to Switch-Case when adding SaveSceneData |
| UI Element Rendering | Dynamic Cast | Keep (appropriate for the use case) |
| Event Processing | Switch-Case | Keep (ideal for FlatBuffers + performance) |
| Future Components | N/A | Use std::variant for new subsystems |

This document should serve as a reference when implementing new polymorphic data types or refactoring existing ones. The choice of strategy should be driven by the specific requirements of each use case, balanced against the trade-offs documented here.

---

## Appendix: Code Examples Repository

For complete, compilable examples of each strategy, see:
- `tests/unit/analysis/switching_strategies/` (future implementation)

---

## References

### C++ Standards
- **C++17**: std::variant, std::visit
- **C++20**: Concepts, designated initializers
- **C++23**: Pattern matching (proposed), std::expected

### SteamRot Source Files
- `src/scenes/FlatbuffersSceneConfigurator.cpp` - Static Cast example
- `src/logic/logic_render.cpp` - Dynamic Cast chain example
- `src/events/event_factory.cpp` - Switch-Case example
- `src/types/core/SceneData.h` - Polymorphic data base class
- `src/types/core/FbsSceneData.h` - Derived data type

### External Resources
- [CppReference: std::variant](https://en.cppreference.com/w/cpp/utility/variant)
- [CppReference: std::visit](https://en.cppreference.com/w/cpp/utility/variant/visit)
- [CppReference: dynamic_cast](https://en.cppreference.com/w/cpp/language/dynamic_cast)
- [Wikipedia: Visitor Pattern](https://en.wikipedia.org/wiki/Visitor_pattern)
- [Compiler Explorer (Godbolt)](https://godbolt.org/) - Compare assembly output

---

**Document Version**: 1.0  
**Last Updated**: 2025-12-16  
**Author**: SteamRot Architecture Team  
**Status**: Analysis Complete - Implementation Pending
