# Data-Driven Simulation System - Quick Reference

This is a quick reference guide for the comprehensive [Data-Driven Simulation Design document](DATA_DRIVEN_SIMULATION_DESIGN.md).

## Three Approaches Overview

### 1. Logic Class-Based System (Extend Current Pattern)
**Best for:** Parameterizing existing logic, scene-specific variations  
**Effort:** Low  
**Flexibility:** Medium  
**Performance:** Excellent  

Define Logic behavior in JSON/FlatBuffers configuration:
```json
{
  "name": "CustomMovementLogic",
  "type": "Movement",
  "archetype_components": ["CPosition", "CVelocity"],
  "parameters": {
    "max_speed": 5.0,
    "friction": 0.95
  }
}
```

### 2. Free Function-Based System
**Best for:** Composable pipelines, reusable utilities  
**Effort:** Medium  
**Flexibility:** High  
**Performance:** Excellent  

Compose simulation from registered free functions:
```json
{
  "steps": [
    { "function": "movement::apply_velocity", "parameters": {"delta_time": 0.016} },
    { "function": "movement::apply_friction", "parameters": {"friction": 0.95} }
  ]
}
```

### 3. Lua Scripting System
**Best for:** Complex logic, designer content, modding  
**Effort:** High  
**Flexibility:** Excellent  
**Performance:** Good  

Write simulation logic in Lua:
```lua
function update_simulation(context)
  apply_velocity(context)
  apply_friction(context)
  check_boundaries(context)
end
```

## Recommended Hybrid Approach

**Phase 1 (Short Term):** Free Function System
- Build on existing `logic::collision`, `logic::ui` namespaces
- Low integration effort, immediate benefits
- Runtime pipeline configuration

**Phase 2 (Medium Term):** Add Lua for High-Level Logic
- Complex game rules and state machines
- Designer-driven content
- Modding support

**Phase 3 (Long Term):** Unified System
- C++ for performance-critical paths
- Lua for gameplay logic
- Seamless interop

## Quick Decision Guide

| Scenario | Use This Approach |
|----------|-------------------|
| Parameterizing existing logic | Logic Classes |
| Performance-critical paths | Logic Classes or Free Functions |
| Composable, reusable systems | Free Functions |
| Runtime reconfiguration needed | Free Functions or Lua |
| Complex state machines | Lua |
| Designer-driven content | Lua |
| Modding support | Lua |
| Prototyping new features | Lua |

## Key Files to Create

### For Free Functions:
- `src/logic/FunctionRegistry.h/cpp` - Function registration system
- `src/logic/simulation_*.h/cpp` - Simulation function namespaces
- `src/logic/SimulationPipeline.h/cpp` - Pipeline executor Logic class
- `data/simulation/*.json` - Pipeline configuration files

### For Lua:
- `src/scripting/LuaEngine.h/cpp` - Lua engine wrapper
- `src/scripting/bindings/*.h/cpp` - C++ to Lua bindings
- `src/logic/LuaLogic.h/cpp` - Lua script executor Logic class
- `data/scripts/*.lua` - Lua simulation scripts

## Existing Patterns in SteamRot

SteamRot already uses free functions in logic namespaces:

**`logic::collision` namespace:**
- `IsMouseOverBounds()`
- `CheckMouseOverUIElement()`
- `CheckMouseOverNestedUIElement()`

**`logic::ui` namespace:**
- `GetAllFragmentNames()`
- `GetAllJointNames()`

These can be registered and used in pipelines!

## Example: Converting UICollisionLogic

**Current (Logic Class):**
```cpp
class UICollisionLogic : public Logic {
  void ProcessLogic() override {
    // Find UI entities...
    logic::collision::CheckMouseOverNestedUIElement(...);
  }
};
```

**Future (Free Function Pipeline):**
```json
{
  "name": "ui_collision_pipeline",
  "type": "Collision",
  "steps": [
    {
      "function": "collision::check_mouse_over_nested_ui",
      "parameters": {}
    }
  ]
}
```

## Implementation Timelines

- **Free Function System:** 2-3 weeks
- **Lua Integration:** 4-6 weeks
- **Full Hybrid System:** 3-4 months

## Next Steps

1. Read the full [Data-Driven Simulation Design](DATA_DRIVEN_SIMULATION_DESIGN.md) document
2. Choose which approach fits your immediate needs
3. Follow the implementation roadmap for your chosen approach
4. Start with one simple Logic class as a proof of concept

## See Also

- [DATA_DRIVEN_SIMULATION_DESIGN.md](DATA_DRIVEN_SIMULATION_DESIGN.md) - Complete design document
- [README.md](../README.md#adding-logic) - Current Logic class workflow
- [CONTEXT_CONFIGURATION.md](CONTEXT_CONFIGURATION.md) - Existing data-driven configuration patterns
