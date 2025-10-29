# Data-Driven Simulation System - Quick Reference

This is a quick reference guide for the comprehensive [Data-Driven Simulation Design document](DATA_DRIVEN_SIMULATION_DESIGN.md).

**PRIMARY FOCUS: Free functions for testing and incremental development!**

## Primary Approach: Free Function-Based System

**Best for:** Testing, prototyping, incremental development  
**Effort:** Minimal (just write functions!)  
**Flexibility:** High  
**Performance:** Excellent  

### Quick Start

Write simulation functions that take SceneContext:

```cpp
// File: src/logic/simulation_test.h
namespace steamrot::simulation::test {

void MyTestFunction(SceneContext &context, const Parameters &params) {
  // Access entities, components, archetypes directly
  ArchetypeID archetype = GenerateArchetypeIDfromTypes<CPosition>();
  
  const auto it = context.archetypes.find(archetype);
  if (it != context.archetypes.end()) {
    for (size_t entity_id : it->second) {
      // Test your logic here
    }
  }
}

} // namespace
```

### Test Directly

```cpp
// tests/logic/simulation_test.test.cpp
TEST_CASE("My test function works", "[unit][simulation]") {
  steamrot::tests::TestContext test_context;
  auto context = test_context.GetLogicContextForTestScene();
  
  steamrot::simulation::test::Parameters params;
  
  // Call directly - no Logic class needed!
  REQUIRE_NOTHROW(steamrot::simulation::test::MyTestFunction(context, params));
}
```

### Key Benefits for Testing

- ✅ **No Logic class required** - Write functions, test them
- ✅ **Direct testing** - Call functions in unit tests
- ✅ **Incremental** - Add one function at a time
- ✅ **Easy debugging** - Standard C++ function debugging
- ✅ **Reusable** - Functions can be called anywhere

## Alternative Approaches

### Logic Class-Based System
**Best for:** Organizing stable, tested functions  
**Effort:** Medium  
**Flexibility:** Medium  
**Performance:** Excellent  

Wrap tested functions in Logic classes when you have a stable set that belongs together.

```json
{
  "name": "CustomMovementLogic",
  "archetype_components": ["CPosition", "CVelocity"],
  "parameters": {"max_speed": 5.0}
}
```

### Lua Scripting System
**Best for:** Complex logic, designer content, modding  
**Effort:** High  
**Flexibility:** Excellent  
**Performance:** Good  

Full scripting when you need maximum flexibility:

```lua
function update_simulation(context)
  -- Full Lua scripting power
end
```

## Decision Guide

| Scenario | Use This |
|----------|----------|
| Testing new logic | **Free Functions** |
| Prototyping features | **Free Functions** |
| Building incrementally | **Free Functions** |
| Performance-critical | Free Functions or Logic Classes |
| Organizing tested code | Logic Classes |
| Complex state machines | Lua |
| Designer-driven content | Lua |
| Modding support | Lua |

## When to Use Logic Classes

Logic classes are **optional organizational wrappers**. Use them only when:
- ✅ You have a stable set of tested functions
- ✅ Functions always run together
- ✅ You need to manage shared state
- ✅ You want LogicFactory integration

**For testing and prototyping, stick with free functions!**

## Implementation Path

### Phase 1: Testing with Free Functions (Start Here!)
1. Create `src/logic/simulation_test.h/cpp`
2. Write free functions with SceneContext parameter
3. Write unit tests that call functions directly
4. Add more functions incrementally

**Time: Minutes to hours**

### Phase 2: Optional Data-Driven Execution
1. Create FunctionRegistry (only if you want data-driven pipelines)
2. Register functions for string-based lookup
3. Create JSON pipeline configurations
4. Implement SimulationPipeline class

**Time: 2-3 weeks (optional, not needed for testing!)**

### Phase 3: Optional Lua Integration
1. Add Sol2/Lua dependencies
2. Create C++ bindings
3. Write Lua scripts for complex logic

**Time: 4-6 weeks (optional, for advanced use cases)**

## Examples

### Example: Testing a Movement Function

```cpp
// src/logic/simulation_movement.h
namespace steamrot::simulation::movement {

void ApplyVelocity(SceneContext &context, const Parameters &params) {
  ArchetypeID archetype = GenerateArchetypeIDfromTypes<CPosition, CVelocity>();
  const auto it = context.archetypes.find(archetype);
  
  if (it != context.archetypes.end()) {
    for (size_t entity_id : it->second) {
      auto &pos = entity::memory::GetComponent<CPosition>(entity_id, context.scene_entities);
      auto &vel = entity::memory::GetComponent<CVelocity>(entity_id, context.scene_entities);
      
      pos.m_x += vel.m_x * 0.016f;
      pos.m_y += vel.m_y * 0.016f;
    }
  }
}

} // namespace
```

```cpp
// tests/logic/simulation_movement.test.cpp
TEST_CASE("ApplyVelocity moves entities", "[unit][movement]") {
  // Setup test context with entities
  steamrot::tests::TestContext test_context;
  auto context = test_context.GetLogicContextForTestScene();
  
  // Create test entities with position and velocity
  // ... test setup ...
  
  // Call function directly
  steamrot::simulation::movement::Parameters params;
  steamrot::simulation::movement::ApplyVelocity(context, params);
  
  // Assert positions changed
  // ... assertions ...
}
```

## Next Steps

1. **Read the full design document** for detailed explanations
2. **Start with free functions** for testing
3. **Build incrementally** - one function at a time
4. **Test each function** independently
5. **Optionally organize** into Logic classes or pipelines later

## See Also

- [DATA_DRIVEN_SIMULATION_DESIGN.md](DATA_DRIVEN_SIMULATION_DESIGN.md) - Complete design document
- [README.md](../README.md#adding-logic) - Current Logic class workflow
- [CONTEXT_CONFIGURATION.md](CONTEXT_CONFIGURATION.md) - Existing data-driven patterns
