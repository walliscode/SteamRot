# Provider Pattern Quick Reference

## TL;DR

**Provider Pattern**: Interfaces for loading and configuring data with reusable free functions.

## Key Principles

1. ✅ Use `Provide()` method name (not `Load()` or `Get()`)
2. ✅ Extract configuration logic to free functions
3. ✅ Place free functions in `<domain>_config.h/cpp`
4. ✅ Test free functions independently
5. ✅ No templates - keep it simple

## Interface Template

```cpp
class IMyDataProvider {
public:
  virtual ~IMyDataProvider() = default;
  virtual std::expected<MyData, FailInfo> Provide() const = 0;
};
```

## Free Function Template

```cpp
// In my_domain_config.h
namespace steamrot::config {

std::expected<std::monostate, FailInfo>
configure_my_data(MyData& data, const MyDataFbs* fb_data);

} // namespace steamrot::config

// In my_domain_config.cpp
namespace steamrot::config {

std::expected<std::monostate, FailInfo>
configure_my_data(MyData& data, const MyDataFbs* fb_data) {
  if (!fb_data) {
    return std::unexpected(
      FailInfo{FailMode::FlatbuffersDataNotFound, "Data is null"});
  }
  
  // Configure fields...
  
  return std::monostate{};
}

} // namespace steamrot::config
```

## Provider Implementation Template

```cpp
class FlatbuffersMyDataProvider : public IMyDataProvider {
private:
  FlatbuffersDataLoader m_loader;

public:
  std::expected<MyData, FailInfo> Provide() const override {
    auto fb_result = m_loader.LoadMyDataFbs();
    if (!fb_result.has_value()) 
      return std::unexpected(fb_result.error());
    
    MyData data;
    auto config_result = config::configure_my_data(data, fb_result.value());
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    
    return data;
  }
};
```

## Naming Conventions

| Element | Convention | Example |
|---------|-----------|---------|
| Interface | `I<Domain>Provider` | `IEngineDataProvider` |
| Concrete class | `Flatbuffers<Domain>Provider` | `FlatbuffersEngineDataProvider` |
| Interface method | `Provide()` | `Provide()`, `ProvideDefault()` |
| Free function | `configure_<domain>_<aspect>()` | `configure_engine_config()` |
| Free function file | `<domain>_config.h/cpp` | `engine_data_config.h` |
| Free function namespace | `steamrot::config` | `steamrot::config` |

## FlatBuffers Checklist

When working with FlatBuffers data:

- [ ] ✅ Check if pointer is null before using
- [ ] ✅ Check if strings/vectors/tables exist before accessing
- [ ] ✅ Primitive types (int, bool, float) don't need null checks
- [ ] ✅ Return clear error messages

```cpp
// ❌ BAD
data.name = fb_data->name()->str(); // Can segfault!

// ✅ GOOD
if (fb_data && fb_data->name()) {
  data.name = fb_data->name()->str();
}
```

## Testing Checklist

For each free function:

- [ ] Test with valid data
- [ ] Test with null data
- [ ] Test with missing required fields
- [ ] Test with missing optional fields
- [ ] Test error messages are clear

```cpp
TEST_CASE("configure_my_data with valid data", "[unit][config]") {
  // Create test data
  // Call free function
  // Verify output
}

TEST_CASE("configure_my_data with null data", "[unit][config]") {
  MyData data;
  auto result = config::configure_my_data(data, nullptr);
  REQUIRE(!result.has_value());
}
```

## Common Patterns

### Required Field
```cpp
if (!fb_data->required_field()) {
  return std::unexpected(
    FailInfo{FailMode::FlatbuffersDataNotFound, 
             "Missing required 'required_field'"});
}
data.required_field = fb_data->required_field()->str();
```

### Optional Field
```cpp
if (fb_data->optional_field()) {
  data.optional_field = fb_data->optional_field()->str();
}
```

### Nested Configuration
```cpp
if (fb_data->child()) {
  auto result = configure_child(data.child, fb_data->child());
  if (!result.has_value()) return std::unexpected(result.error());
}
```

### Collection
```cpp
if (fb_data->items()) {
  for (const auto* fb_item : *fb_data->items()) {
    Item item;
    auto result = configure_item(item, fb_item);
    if (!result.has_value()) return std::unexpected(result.error());
    data.items.push_back(std::move(item));
  }
}
```

## File Organization

```
src/
├── types/interfaces/
│   └── IMyDataProvider.h          # Interface
├── data_providers/
│   ├── FlatbuffersMyDataProvider.h  # Concrete provider
│   ├── FlatbuffersMyDataProvider.cpp
│   ├── my_data_config.h            # Free functions
│   └── my_data_config.cpp

tests/unit/data_providers/
├── FlatbuffersMyDataProvider.test.cpp  # Provider tests
└── my_data_config.test.cpp            # Free function tests
```

## When to Use Provider vs Configurator

| Use Case | Pattern |
|----------|---------|
| Load data from file | Provider with `Provide()` |
| Configure existing object | Provider with `Configure()` method |
| Create and configure object | Provider with `Provide()` |
| Both load and configure | Provider with both methods |

**Rule of Thumb**: Use Provider for everything. Configurators are legacy.

## Migration Quick Steps

1. Create `<domain>_config.h/cpp`
2. Extract private methods to free functions
3. Add tests for free functions
4. Update provider to use free functions
5. Rename method to `Provide()`
6. Update call sites
7. Run all tests

## Anti-Patterns

### ❌ Don't: Hidden dependencies in free functions
```cpp
std::expected<std::monostate, FailInfo>
configure_data(MyData& data, const MyDataFbs* fb_data) {
  data.id = GlobalIDGenerator::GetNext(); // Hidden dependency!
}
```

### ✅ Do: Explicit dependencies
```cpp
std::expected<std::monostate, FailInfo>
configure_data(MyData& data, const MyDataFbs* fb_data, 
               IDGenerator& id_gen) {
  data.id = id_gen.GetNext();
}
```

### ❌ Don't: Configuration logic in provider
```cpp
std::expected<MyData, FailInfo> 
FlatbuffersMyDataProvider::Provide() const {
  MyData data;
  // 50 lines of configuration logic here...
  return data;
}
```

### ✅ Do: Use free functions
```cpp
std::expected<MyData, FailInfo> 
FlatbuffersMyDataProvider::Provide() const {
  MyData data;
  auto result = config::configure_my_data(data, fb_data);
  if (!result.has_value()) return std::unexpected(result.error());
  return data;
}
```

## Questions?

- 📖 Full details: `PROVIDER_INTERFACE_STANDARDIZATION.md`
- 🛠️ Implementation guide: `PROVIDER_IMPLEMENTATION_GUIDE.md`
- 📋 Migration plan: `PROVIDER_MIGRATION_PLAN.md`

## Decision: No Templates

We decided **NOT** to use templates because:
- Pattern is simple enough without them
- Each provider has unique needs
- Easier to understand and maintain
- Rely on discipline and code review instead

## Current Status

Phase 1: ✅ Documentation complete  
Phase 2: ⏳ Provider migration (not started)  
Phase 3: ⏳ Configurator migration (not started)  
Phase 4: ⏳ Cleanup (not started)

---

**Remember**: Keep it simple. Free functions + clear interfaces = maintainable code.
