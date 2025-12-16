# Switching Strategies - Quick Reference Guide

## Overview

Quick decision guide for choosing the right strategy to switch on concrete data types.

**Full Analysis**: See [SWITCHING_STRATEGIES_ANALYSIS.md](SWITCHING_STRATEGIES_ANALYSIS.md)

---

## Decision Tree

```
Q: Is performance absolutely critical (inner loop, millions of calls)?
├─ YES → Is it FlatBuffers-generated code?
│   ├─ YES → Use SWITCH-CASE (#3)
│   └─ NO → Use STATIC CAST (#1)
└─ NO → Do you have virtual functions in base class?
    ├─ YES → Are all types known at compile time?
    │   ├─ YES → Can you refactor to value semantics?
    │   │   ├─ YES → Use STD::VARIANT (#5) ⭐ Recommended for new code
    │   │   └─ NO → How many types? (<10)
    │   │       ├─ YES → Use DYNAMIC CAST (#2)
    │   │       └─ NO → Use VISITOR (#4)
    │   └─ NO → Use DYNAMIC CAST (#2)
    └─ NO → Use SWITCH-CASE (#3)
```

---

## Strategy Cheat Sheet

### 1️⃣ Static Cast with Manual Type Checking

```cpp
// Cast and validate manually
FbsSceneData* fbs = static_cast<FbsSceneData*>(scene_data);
if (!fbs) return error;
```

✅ **Use When**: FlatBuffers, performance critical, single dominant type  
❌ **Avoid When**: Multiple types, runtime type discovery needed  
⚡ **Performance**: ★★★★★ (Excellent - Zero overhead)  
🛡️ **Safety**: ★☆☆☆☆ (Poor - Manual validation required)

---

### 2️⃣ Dynamic Cast Chain

```cpp
if (const auto* button = dynamic_cast<const ButtonElement*>(&element)) {
    DrawButton(*button);
} else if (const auto* panel = dynamic_cast<const PanelElement*>(&element)) {
    DrawPanel(*panel);
}
```

✅ **Use When**: True polymorphism, <10 types, readability matters  
❌ **Avoid When**: Performance critical, no virtual functions, many types  
⚡ **Performance**: ★★★☆☆ (Medium - RTTI overhead)  
🛡️ **Safety**: ★★★★☆ (Good - nullptr on failure)

---

### 3️⃣ Switch-Case with Type Tag Enum

```cpp
switch (data_type) {
case EventDataData::UserInputBitsetData: {
    auto* data = static_cast<const UserInputBitsetData*>(ptr);
    return CreateUserInputBitset(*data);
}
case EventDataData::SceneChangePacketData: {
    // ...
}
default:
    return error;
}
```

✅ **Use When**: FlatBuffers unions, performance critical, want compiler warnings  
❌ **Avoid When**: Open-ended type hierarchies, prefer OOP polymorphism  
⚡ **Performance**: ★★★★★ (Excellent - Jump table)  
🛡️ **Safety**: ★★★☆☆ (Medium - Enum must match type)

---

### 4️⃣ Visitor Pattern

```cpp
class SceneDataVisitor {
public:
    virtual void Visit(const FbsSceneData& data) = 0;
    virtual void Visit(const SaveSceneData& data) = 0;
};

class SceneData {
public:
    virtual void Accept(SceneDataVisitor& visitor) const = 0;
};

// Usage
scene_data.Accept(visitor);
```

✅ **Use When**: Multiple operations, operations change frequently, stable types  
❌ **Avoid When**: Types change frequently, simple operations, FlatBuffers  
⚡ **Performance**: ★★★☆☆ (Medium - Virtual function overhead)  
🛡️ **Safety**: ★★★★★ (Excellent - Compile-time checking)

---

### 5️⃣ std::variant with std::visit ⭐ Recommended

```cpp
using SceneDataVariant = std::variant<FbsSceneData, SaveSceneData, TestSceneData>;

auto result = std::visit(Overload{
    [&](const FbsSceneData& data) { return ConfigureFbs(scene, data); },
    [&](const SaveSceneData& data) { return ConfigureSave(scene, data); },
    [&](const TestSceneData& data) { return ConfigureTest(scene, data); }
}, scene_data);
```

✅ **Use When**: All types known at compile-time, C++17+, value semantics  
❌ **Avoid When**: Open-ended hierarchies, pointer-based design, C++14  
⚡ **Performance**: ★★★★★ (Excellent - Zero overhead possible)  
🛡️ **Safety**: ★★★★★ (Excellent - Compiler enforced)

---

## Comparison Matrix

|  | Static Cast | Dynamic Cast | Switch-Case | Visitor | std::variant |
|---|:-----------:|:------------:|:-----------:|:-------:|:------------:|
| **Type Safety** | ★☆☆☆☆ | ★★★★☆ | ★★★☆☆ | ★★★★★ | ★★★★★ |
| **Performance** | ★★★★★ | ★★★☆☆ | ★★★★★ | ★★★☆☆ | ★★★★★ |
| **Maintainability** | ★★★☆☆ | ★★★★☆ | ★★★★☆ | ★★☆☆☆ | ★★★★★ |
| **Extensibility** | ★☆☆☆☆ | ★★★★☆ | ★★★☆☆ | ★☆☆☆☆ | ★☆☆☆☆ |
| **Compiler Help** | ☆☆☆☆☆ | ☆☆☆☆☆ | ★★★☆☆ | ★★★★☆ | ★★★★★ |
| **FlatBuffers** | ✅ | ❌ | ✅ | ❌ | ⚠️ |
| **C++ Version** | Any | Any | Any | Any | C++17+ |
| **Boilerplate** | ★★★★★ | ★★★★★ | ★★★☆☆ | ★☆☆☆☆ | ★★★★☆ |

---

## Common Use Cases

### Scene Configuration

**Current**: Static Cast  
**When adding SaveSceneData**: Migrate to Switch-Case

```cpp
// Before (single type)
FbsSceneData* fbs = static_cast<FbsSceneData*>(scene_data);

// After (multiple types)
switch (scene_data->type) {
    case SceneDataType::FlatBuffers: /* ... */ break;
    case SceneDataType::SaveFile: /* ... */ break;
}
```

---

### UI Element Rendering

**Current**: Dynamic Cast  
**Recommendation**: Keep (appropriate)

```cpp
if (const auto* button = dynamic_cast<const ButtonElement*>(&element)) {
    DrawButton(*button);
}
```

**Why**: True polymorphism, ~6 types, not performance-critical

---

### Event Processing

**Current**: Switch-Case  
**Recommendation**: Keep (ideal)

```cpp
switch (data_type) {
case EventDataData_UserInputBitsetData: /* ... */ break;
case EventDataData_SceneChangePacketData: /* ... */ break;
}
```

**Why**: FlatBuffers enum, performance-critical, compiler warnings

---

### New Subsystems

**Recommendation**: std::variant (Strategy #5)

```cpp
using ComponentDataVariant = std::variant<
    ComponentData1,
    ComponentData2,
    ComponentData3
>;

void Process(const ComponentDataVariant& data) {
    std::visit([](const auto& d) {
        // Type-safe dispatch
    }, data);
}
```

**Why**: Modern C++, type safety, performance, maintainability

---

## Performance Quick Facts

| Strategy | Typical Overhead | Scalability |
|----------|-----------------|-------------|
| Static Cast | 5-10 cycles | O(1) |
| Switch-Case | 10-20 cycles | O(1) |
| std::variant | 10-25 cycles | O(1) |
| Dynamic Cast (1st) | 50-100 cycles | O(1) |
| Dynamic Cast (nth) | 50n-100n cycles | O(n) |
| Visitor | 100-200 cycles | O(1) |

**Note**: For 5+ types, Switch-Case and std::variant are 5-20x faster than dynamic_cast chain

---

## Code Patterns

### Overload Pattern for std::visit

```cpp
template<class... Ts> 
struct Overload : Ts... { using Ts::operator()...; };

template<class... Ts> 
Overload(Ts...) -> Overload<Ts...>;

// Usage
std::visit(Overload{
    [](const Type1& t) { /* ... */ },
    [](const Type2& t) { /* ... */ },
}, variant);
```

---

### Enable Switch Warnings

```cpp
// In CMakeLists.txt
target_compile_options(steamrot PRIVATE -Wswitch)

// In switch statement
switch (type) {
case Type::A: /* ... */ break;
case Type::B: /* ... */ break;
// Compiler warns if you forget Type::C
}
```

---

### Dynamic Cast Optimization

```cpp
// Order by frequency (most common first)
if (auto* common = dynamic_cast<MostCommonType*>(ptr)) {
    // 80% of cases handled first
} else if (auto* rare = dynamic_cast<RareType*>(ptr)) {
    // Only checked if first fails
}
```

---

## Common Pitfalls

### ❌ DON'T: Mix strategies in same subsystem

```cpp
// Bad: Inconsistent
void Configure1(SceneData* data) {
    auto* fbs = static_cast<FbsSceneData*>(data);  // Static cast
}

void Configure2(SceneData* data) {
    if (auto* fbs = dynamic_cast<FbsSceneData*>(data)) {  // Dynamic cast
    }
}
```

### ✅ DO: Be consistent

```cpp
// Good: All functions use same strategy
void Configure1(SceneData* data) {
    auto* fbs = static_cast<FbsSceneData*>(data);
}

void Configure2(SceneData* data) {
    auto* fbs = static_cast<FbsSceneData*>(data);
}
```

---

### ❌ DON'T: Forget default case

```cpp
// Bad: No default
switch (type) {
case Type::A: /* ... */ break;
case Type::B: /* ... */ break;
// What if Type::C is added?
}
```

### ✅ DO: Always handle unexpected values

```cpp
// Good: Explicit default
switch (type) {
case Type::A: /* ... */ break;
case Type::B: /* ... */ break;
default:
    return std::unexpected(FailInfo{"Unhandled type"});
}
```

---

### ❌ DON'T: Use dynamic_cast with non-virtual base

```cpp
// Bad: Won't compile
struct Base { };  // No virtual functions
struct Derived : Base { };

Base* b = new Derived;
auto* d = dynamic_cast<Derived*>(b);  // ERROR
```

### ✅ DO: Ensure virtual destructor

```cpp
// Good: Virtual destructor enables dynamic_cast
struct Base {
    virtual ~Base() = default;  // Virtual!
};

struct Derived : Base { };

Base* b = new Derived;
auto* d = dynamic_cast<Derived*>(b);  // OK
```

---

## Migration Checklist

### Static Cast → Switch-Case

- [ ] Add type enum to base class
- [ ] Initialize enum in all derived constructors
- [ ] Replace static_cast sites with switch statements
- [ ] Add default case with error handling
- [ ] Enable `-Wswitch` compiler warning
- [ ] Test all code paths

### Dynamic Cast → std::variant

- [ ] Define variant typedef with all types
- [ ] Update factories to return variant instead of pointer
- [ ] Replace dynamic_cast chains with std::visit
- [ ] Update function signatures to accept variant
- [ ] Update tests
- [ ] Benchmark performance (should improve)

---

## When in Doubt

1. **Start with Switch-Case (#3)** for most new code
   - Good balance of safety, performance, maintainability
   - Works with FlatBuffers
   - Compiler helps catch errors

2. **Use std::variant (#5)** for new subsystems if:
   - Using C++17 or later ✓
   - All types known at compile time ✓
   - Can design with value semantics ✓

3. **Keep existing patterns** unless:
   - Clear performance problem (profile first)
   - Major maintainability issue (evidence-based)
   - Adding new types (evaluate strategy fit)

---

## Getting Help

- **Full Analysis**: [SWITCHING_STRATEGIES_ANALYSIS.md](SWITCHING_STRATEGIES_ANALYSIS.md)
- **Code Examples**: See `src/events/event_factory.cpp` (switch), `src/logic/logic_render.cpp` (dynamic_cast)
- **Questions**: Tag @architecture-team in PR comments

---

**Quick Reference Version**: 1.0  
**Last Updated**: 2025-12-16
