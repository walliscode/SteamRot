# Data-Driven Simulation System Design Plan

This document outlines design options for creating a data-driven simulation system in SteamRot. **The primary focus is on the free function approach for testing and prototyping**, allowing you to build up individual simulation functions incrementally rather than requiring full Logic class implementations.

## Table of Contents

- [Overview](#overview)
- [Quick Start: Free Functions for Testing](#quick-start-free-functions-for-testing)
- [Current Architecture](#current-architecture)
- [Primary Approach: Free Function-Based System](#primary-approach-free-function-based-system)
- [Alternative Approach 1: Logic Class-Based System](#alternative-approach-1-logic-class-based-system)
- [Alternative Approach 2: Lua Scripting System](#alternative-approach-2-lua-scripting-system)
- [Comparison Matrix](#comparison-matrix)
- [Recommendations](#recommendations)
- [Implementation Roadmap](#implementation-roadmap)
- [Examples](#examples)

## Overview

A data-driven simulation system allows game logic to be defined in external configuration files, enabling:

- **Incremental Testing**: Build and test individual functions in isolation before integration
- **Runtime Reconfiguration**: Change simulation behavior without recompiling
- **Rapid Prototyping**: Test different simulation functions quickly
- **Function Composition**: Combine tested functions into larger systems
- **Reduced Code Complexity**: Separate logic definition from implementation

### Testing-First Philosophy

This document emphasizes a **testing-first approach** where you:
1. Write individual simulation functions
2. Test each function independently
3. Register functions for data-driven execution
4. Compose tested functions into pipelines (optional)
5. Optionally wrap in Logic classes for organization (not required)

### Key Requirements

1. **Access to SceneContext**: Simulation logic needs access to entities, components, archetypes, and game resources
2. **Execution Order Control**: Define which logic runs when and in what sequence
3. **Type Safety**: Maintain C++ type safety where possible
4. **Performance**: Minimize overhead for real-time game simulation
5. **Debuggability**: Easy to trace and debug simulation behavior

## Quick Start: Free Functions for Testing

**Start here if you want to test individual simulation functions without the overhead of Logic classes.**

### Step 1: Write a Simple Simulation Function

```cpp
// File: src/logic/simulation_test.h
#pragma once
#include "SceneContext.h"
#include <unordered_map>
#include <string>
#include <variant>

namespace steamrot::simulation::test {

// Simple parameter type for testing
using ParamValue = std::variant<int, float, std::string, bool>;
using Parameters = std::unordered_map<std::string, ParamValue>;

////////////////////////////////////////////////////////////
/// @brief Simple test function that logs entity count
///
/// @param context SceneContext with access to entities
/// @param params Parameters (unused in this example)
////////////////////////////////////////////////////////////
void LogEntityCount(SceneContext &context, const Parameters &params) {
  size_t total_entities = 0;
  
  // Count entities across all archetypes
  for (const auto &[archetype_id, entities] : context.archetypes) {
    total_entities += entities.size();
  }
  
  std::cout << "Total entities in scene: " << total_entities << std::endl;
}

} // namespace steamrot::simulation::test
```

### Step 2: Test Your Function Directly

```cpp
// File: tests/logic/simulation_test.test.cpp
#include "simulation_test.h"
#include "TestContext.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LogEntityCount function works", "[unit][simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  
  auto context = test_context.GetLogicContextForTestScene();
  steamrot::simulation::test::Parameters params;
  
  // Call function directly - no Logic class needed!
  REQUIRE_NOTHROW(steamrot::simulation::test::LogEntityCount(context, params));
}
```

### Step 3: Add More Functions Incrementally

```cpp
namespace steamrot::simulation::test {

////////////////////////////////////////////////////////////
/// @brief Test function that modifies component data
////////////////////////////////////////////////////////////
void IncrementAllPositions(SceneContext &context, const Parameters &params) {
  ArchetypeID archetype = GenerateArchetypeIDfromTypes<CPosition>();
  
  const auto it = context.archetypes.find(archetype);
  if (it != context.archetypes.end()) {
    for (size_t entity_id : it->second) {
      auto &pos = entity::memory::GetComponent<CPosition>(entity_id, context.scene_entities);
      pos.m_x += 1.0f;
      pos.m_y += 1.0f;
    }
  }
}

////////////////////////////////////////////////////////////
/// @brief Test function with parameters
////////////////////////////////////////////////////////////
void ScaleVelocity(SceneContext &context, const Parameters &params) {
  // Extract parameter with default
  float scale = 1.0f;
  if (params.contains("scale") && std::holds_alternative<float>(params.at("scale"))) {
    scale = std::get<float>(params.at("scale"));
  }
  
  ArchetypeID archetype = GenerateArchetypeIDfromTypes<CVelocity>();
  const auto it = context.archetypes.find(archetype);
  
  if (it != context.archetypes.end()) {
    for (size_t entity_id : it->second) {
      auto &vel = entity::memory::GetComponent<CVelocity>(entity_id, context.scene_entities);
      vel.m_x *= scale;
      vel.m_y *= scale;
    }
  }
}

} // namespace
```

### Step 4: (Optional) Register for Data-Driven Execution

Only when you're ready to use functions in a data-driven way:

```cpp
#include "FunctionRegistry.h"

// In your initialization code
namespace steamrot::simulation::test {
  void RegisterTestFunctions() {
    FunctionRegistry::Instance().RegisterFunction("test::log_entity_count", LogEntityCount);
    FunctionRegistry::Instance().RegisterFunction("test::increment_positions", IncrementAllPositions);
    FunctionRegistry::Instance().RegisterFunction("test::scale_velocity", ScaleVelocity);
  }
}
```

### Key Benefits for Testing

- ✅ **No Logic class required** - Functions are standalone and testable
- ✅ **Direct testing** - Call functions directly in unit tests
- ✅ **Incremental development** - Add one function at a time
- ✅ **Easy debugging** - Standard C++ function debugging
- ✅ **Reusable** - Functions can be called from anywhere
- ✅ **Optional organization** - Wrap in Logic classes later if desired

### When to Use Logic Classes

Logic classes are **optional organizational wrappers**. Use them when:
- You have a stable set of functions that always run together
- You need to manage shared state across multiple function calls
- You want to integrate with the existing LogicFactory system

**For testing and prototyping, stick with free functions!**

## Current Architecture

SteamRot uses an Entity-Component-System (ECS) architecture with Logic classes:

### Core Components

**EntityMemoryPool**: Contiguous memory storage for all components across all entities
```cpp
using EntityMemoryPool = std::tuple<
  std::vector<CMeta>,
  std::vector<CUserInterface>,
  std::vector<CMachinaForm>,
  std::vector<CGrimoireMachina>
>;
```

**SceneContext**: Lightweight reference container providing access to:
- `scene_entities` - EntityMemoryPool reference
- `archetypes` - Map of ArchetypeID to entity indices
- `scene_texture` - Render target
- `game_window` - Window reference
- `asset_manager` - Asset loading/management
- `event_handler` - Event bus
- `mouse_position` - Input state

**Logic Base Class**: Abstract class for game logic
```cpp
class Logic {
protected:
  virtual void ProcessLogic() = 0;
  SceneContext m_scene_context;
public:
  Logic(const SceneContext scene_context);
  void RunLogic();
};
```

**LogicFactory**: Creates Logic instances per scene type
```cpp
class LogicFactory {
  std::expected<LogicCollection, FailInfo> CreateLogicMap();
private:
  std::expected<LogicVector, FailInfo> CreateCollisionLogics();
  std::expected<LogicVector, FailInfo> CreateRenderLogics();
  std::expected<LogicVector, FailInfo> CreateActionLogics();
};
```

**LogicTypes**: Categories of logic execution
- `Collision` - Spatial interactions (UI collision, physics)
- `Render` - Drawing to render texture
- `Action` - Input processing and event triggering
- `Movement` - Position updates

### Current Execution Flow

1. Scene creates LogicFactory with SceneContext
2. LogicFactory creates LogicCollection (map of LogicType to LogicVector)
3. Scene stores and executes Logic instances by type in game loop
4. Each Logic class implements `ProcessLogic()` to modify game state

### Existing Free Function Pattern

SteamRot already uses free functions in logic namespaces:

**Example: `logic::collision` namespace** (`src/logic/logic_collision.h`)
```cpp
namespace steamrot::logic::collision {
  bool IsMouseOverBounds(const sf::Vector2i &mouse_position,
                         const sf::FloatRect &bounds);
  
  void CheckMouseOverUIElement(const sf::Vector2i &mouse_position,
                               UIElement &element);
  
  void CheckMouseOverNestedUIElement(const sf::Vector2i &mouse_position,
                                     UIElement &element);
}
```

**Example: `logic::ui` namespace** (`src/logic/logic_ui.h`)
```cpp
namespace steamrot::logic::ui {
  std::vector<std::string> GetAllFragmentNames(const CGrimoireMachina &grimoire_machina);
  
  std::vector<std::string> GetAllJointNames(const CGrimoireMachina &grimoire_machina);
}
```

These functions are called **by** Logic classes but form the foundation for a testing-friendly data-driven system.

## Primary Approach: Free Function-Based System

**Recommended for testing, prototyping, and incremental development.**

### Overview

Build simulation logic from composable free functions defined in namespaces. Functions can be tested independently, then optionally coordinated by data files or wrapped in Logic classes for organization.

### Architecture

**1. Function Registry System (Optional for Advanced Use)**

Create a registry mapping string names to function pointers:

```cpp
namespace steamrot::simulation {

// Function signature for simulation functions
using SimulationFunction = std::function<void(SceneContext&, const Parameters&)>;

// Global function registry
class FunctionRegistry {
private:
  std::unordered_map<std::string, SimulationFunction> m_functions;
  
public:
  static FunctionRegistry& Instance();
  
  void RegisterFunction(const std::string &name, SimulationFunction func);
  SimulationFunction GetFunction(const std::string &name) const;
};

// Helper macro for registration
#define REGISTER_SIMULATION_FUNCTION(name, func) \
  namespace { \
    struct Register_##name { \
      Register_##name() { \
        FunctionRegistry::Instance().RegisterFunction(#name, func); \
      } \
    }; \
    static Register_##name register_##name; \
  }
}
```

**2. Simulation Function Definitions**

Define simulation logic as free functions in namespaces:

```cpp
// File: src/logic/simulation_movement.h
namespace steamrot::simulation::movement {

void ApplyVelocity(SceneContext &context, const Parameters &params) {
  ArchetypeID archetype = GenerateArchetypeIDfromTypes<CPosition, CVelocity>();
  
  const auto it = context.archetypes.find(archetype);
  if (it != context.archetypes.end()) {
    for (size_t entity_id : it->second) {
      auto &pos = entity::memory::GetComponent<CPosition>(entity_id, context.scene_entities);
      auto &vel = entity::memory::GetComponent<CVelocity>(entity_id, context.scene_entities);
      
      float delta_time = params.GetFloat("delta_time", 1.0f);
      pos.m_x += vel.m_x * delta_time;
      pos.m_y += vel.m_y * delta_time;
    }
  }
}

void ApplyFriction(SceneContext &context, const Parameters &params) {
  ArchetypeID archetype = GenerateArchetypeIDfromTypes<CVelocity>();
  
  const auto it = context.archetypes.find(archetype);
  if (it != context.archetypes.end()) {
    float friction = params.GetFloat("friction", 0.9f);
    
    for (size_t entity_id : it->second) {
      auto &vel = entity::memory::GetComponent<CVelocity>(entity_id, context.scene_entities);
      vel.m_x *= friction;
      vel.m_y *= friction;
    }
  }
}

// Optional: Register functions for data-driven execution
REGISTER_SIMULATION_FUNCTION(movement::apply_velocity, ApplyVelocity);
REGISTER_SIMULATION_FUNCTION(movement::apply_friction, ApplyFriction);

} // namespace steamrot::simulation::movement
```

**3. Simulation Pipeline Configuration (Optional)**

Define execution pipeline in JSON:

```json
{
  "simulation_pipelines": [
    {
      "name": "physics_simulation",
      "type": "Movement",
      "scene_types": ["CRAFTING"],
      "execution_order": 10,
      "steps": [
        {
          "function": "movement::apply_velocity",
          "parameters": {
            "delta_time": 0.016
          }
        },
        {
          "function": "movement::apply_friction",
          "parameters": {
            "friction": 0.95
          }
        },
        {
          "function": "collision::detect_boundaries",
          "parameters": {
            "bounds_width": 800,
            "bounds_height": 600
          }
        }
      ]
    }
  ]
}
```

**4. Pipeline Executor (Optional - for organizing tested functions)**

```cpp
class SimulationPipeline : public Logic {
private:
  std::string m_name;
  std::vector<PipelineStep> m_steps;
  
  void ProcessLogic() override {
    for (const auto &step : m_steps) {
      auto func = FunctionRegistry::Instance().GetFunction(step.function_name);
      if (func) {
        func(m_scene_context, step.parameters);
      }
    }
  }

public:
  SimulationPipeline(const SceneContext scene_context,
                    const PipelineDefinition &definition);
};
```

### Implementation Steps

**For Testing (Minimal Setup):**

1. **Create simulation function namespace**
   - File: `src/logic/simulation_test.h/cpp`
   - Write free functions with SceneContext parameter
   - No registration needed initially

2. **Write unit tests**
   - File: `tests/logic/simulation_test.test.cpp`
   - Test functions directly by calling them
   - Use TestContext to provide SceneContext

**For Data-Driven Execution (Optional):**

3. **Create FunctionRegistry infrastructure**
   - File: `src/logic/FunctionRegistry.h/cpp`
   - Implement registration system and lookup

4. **Define simulation function namespaces**
   - Files: `src/logic/simulation_*.h/cpp`
   - Create functions for movement, collision, rendering, actions
   - Register each function with unique name

5. **Create Pipeline schema and loader**
   - File: `src/flatbuffers_headers/simulation_pipeline.fbs`
   - File: `src/data_handlers/SimulationPipelineLoader.h/cpp`

6. **Implement SimulationPipeline Logic class**
   - File: `src/logic/SimulationPipeline.h/cpp`
   - Execute functions in sequence based on configuration

7. **Update LogicFactory**
   - Load pipeline definitions
   - Create SimulationPipeline instances

8. **Add configuration files**
   - `data/simulation/movement_pipeline.json`
   - `data/simulation/combat_pipeline.json`

### Pros

- ✅ **Direct Testing**: Test functions in isolation without overhead
- ✅ **Composability**: Mix and match functions in different combinations
- ✅ **Reusability**: Functions can be used across multiple systems
- ✅ **Clear Boundaries**: Each function has well-defined responsibility
- ✅ **Incremental Development**: Add one function at a time
- ✅ **No Logic Class Required**: Functions work standalone
- ✅ **Runtime Configuration**: Change execution order without recompiling (when using pipelines)
- ✅ **Existing Pattern**: Builds on existing free function usage in codebase

### Cons

- ⚠️ **Manual Registration**: Functions must be explicitly registered (only for data-driven use)
- ⚠️ **String-Based Lookup**: Runtime overhead and potential for typos (only for data-driven use)
- ⚠️ **Limited Type Safety**: Parameters passed as generic map (only for data-driven use)
- ⚠️ **No Control Flow**: Can't express conditionals/loops in data (only for data-driven use)

**Note:** Most cons only apply when using the optional data-driven pipeline system. For testing, you just write and call functions directly!

### Best Use Cases

- ✅ Testing and prototyping simulation logic
- ✅ Sequential processing pipelines
- ✅ Combining existing logic functions in new ways
- ✅ Building up simulation incrementally
- ✅ Modular, composable game systems

## Alternative Approach 1: Logic Class-Based System

*Use when you have stable, well-tested functions that you want to organize into reusable Logic classes.*

### Overview

Extend the current Logic class pattern to be data-driven by defining Logic behavior in JSON/FlatBuffers configuration files. This approach wraps simulation logic in Logic classes that can be configured via data.

### Architecture

**1. Logic Configuration Schema**

Define simulation behavior in FlatBuffers/JSON:

```json
{
  "logic_definitions": [
    {
      "name": "CustomMovementLogic",
      "type": "Movement",
      "scene_types": ["CRAFTING"],
      "archetype_components": ["CPosition", "CVelocity"],
      "parameters": {
        "max_speed": 5.0,
        "acceleration": 0.5,
        "friction": 0.9
      },
      "execution_order": 10
    },
    {
      "name": "DamageOverTimeLogic",
      "type": "Action",
      "scene_types": ["CRAFTING", "TITLE"],
      "archetype_components": ["CHealth", "CStatusEffect"],
      "parameters": {
        "damage_per_tick": 1.0,
        "tick_rate": 60
      },
      "execution_order": 20
    }
  ]
}
```

**2. Generic ConfigurableLogic Class**

```cpp
class ConfigurableLogic : public Logic {
private:
  LogicDefinition m_definition;
  std::unordered_map<std::string, float> m_parameters;
  
  void ProcessLogic() override {
    // Use m_definition to determine which components to process
    ArchetypeID archetype_id = GenerateArchetypeFromDefinition(m_definition);
    
    const auto it = m_scene_context.archetypes.find(archetype_id);
    if (it != m_scene_context.archetypes.end()) {
      for (size_t entity_id : it->second) {
        // Apply logic based on m_definition and m_parameters
        ProcessEntity(entity_id);
      }
    }
  }
  
  void ProcessEntity(size_t entity_id);

public:
  ConfigurableLogic(const SceneContext scene_context, 
                   const LogicDefinition &definition);
};
```

**3. Enhanced LogicFactory**

```cpp
class LogicFactory {
private:
  std::vector<LogicDefinition> m_logic_definitions;
  
  std::expected<LogicVector, FailInfo> CreateLogicsFromDefinitions(
    LogicType type);

public:
  LogicFactory(const SceneType scene_type, 
               const SceneContext &scene_context,
               const std::vector<LogicDefinition> &definitions);
};
```

### Implementation Steps

1. **Create FlatBuffers schema for logic definitions**
   - File: `src/flatbuffers_headers/logic_definition.fbs`
   - Define tables for LogicDefinition, Parameters, ComponentList

2. **Implement LogicDefinitionLoader**
   - File: `src/data_handlers/LogicDefinitionLoader.h/cpp`
   - Load JSON files and convert to LogicDefinition objects

3. **Create ConfigurableLogic base class**
   - File: `src/logic/ConfigurableLogic.h/cpp`
   - Template or polymorphic approach for different logic types

4. **Update LogicFactory**
   - Load logic definitions from JSON
   - Create ConfigurableLogic instances based on definitions
   - Sort by execution_order

5. **Add configuration files**
   - `data/logic/title_logic.json`
   - `data/logic/crafting_logic.json`
   - `tests/data/logic/test_logic.json`

### Pros

- **Type Safety**: C++ types enforced at compile time
- **Performance**: No runtime interpretation overhead
- **Familiar Pattern**: Extends existing Logic class architecture
- **Debugging**: Standard C++ debugging tools work
- **Strong Integration**: Direct access to all C++ types and functions
- **Organization**: Groups related logic together

### Cons

- **Overhead**: Requires Logic class infrastructure
- **Limited Flexibility**: Only predefined logic patterns supported
- **Requires Recompilation**: Adding new logic types requires C++ changes
- **Complex Configuration**: JSON becomes verbose for complex logic
- **Not for Testing**: Too much overhead for simple function testing

### Best Use Cases

- Organizing stable, tested functions into reusable classes
- Parameterizing existing logic types
- Configuring component processing patterns
- Scene-specific logic variations
- Performance-critical simulation with stable patterns

## Alternative Approach 2: Lua Scripting System

### Overview

Embed Lua scripting engine to allow full simulation logic to be written in Lua scripts, with C++ providing the ECS infrastructure.

### Architecture

**1. Lua Integration Layer**

Use **Sol2** library (header-only, modern C++ Lua binding):

```cpp
// File: src/scripting/LuaEngine.h
#include <sol/sol.hpp>

namespace steamrot::scripting {

class LuaEngine {
private:
  sol::state m_lua;
  
  void BindSceneContext();
  void BindComponents();
  void BindUtilities();

public:
  LuaEngine();
  
  void LoadScript(const std::string &script_path);
  void ExecuteFunction(const std::string &function_name);
  
  sol::state& GetState() { return m_lua; }
};

} // namespace steamrot::scripting
```

**2. C++ to Lua Bindings**

Expose C++ types and functions to Lua:

```cpp
void LuaEngine::BindSceneContext() {
  // Bind SceneContext
  m_lua.new_usertype<SceneContext>("SceneContext",
    "entities", &SceneContext::scene_entities,
    "archetypes", &SceneContext::archetypes,
    "event_handler", &SceneContext::event_handler,
    "mouse_position", &SceneContext::mouse_position
  );
  
  // Bind component access helpers
  m_lua.set_function("get_component_position", 
    [](size_t entity_id, SceneContext& ctx) -> CPosition& {
      return entity::memory::GetComponent<CPosition>(entity_id, ctx.scene_entities);
    }
  );
  
  // Bind archetype utilities
  m_lua.set_function("generate_archetype_id",
    []() -> ArchetypeID {
      return GenerateArchetypeIDfromTypes<CPosition, CVelocity>();
    }
  );
}

void LuaEngine::BindComponents() {
  // Bind component types
  m_lua.new_usertype<CPosition>("CPosition",
    "x", &CPosition::m_x,
    "y", &CPosition::m_y,
    "active", &CPosition::m_active
  );
  
  m_lua.new_usertype<CVelocity>("CVelocity",
    "x", &CVelocity::m_x,
    "y", &CVelocity::m_y,
    "active", &CVelocity::m_active
  );
}
```

**3. Lua Simulation Scripts**

Write simulation logic directly in Lua:

```lua
-- File: data/scripts/movement_simulation.lua

-- Configuration
local config = {
  max_speed = 5.0,
  friction = 0.95,
  delta_time = 0.016
}

-- Apply velocity to position
function apply_velocity(context)
  local archetype_id = generate_archetype_id()  -- For CPosition, CVelocity
  
  local archetypes = context.archetypes
  if archetypes[archetype_id] then
    for _, entity_id in ipairs(archetypes[archetype_id]) do
      local pos = get_component_position(entity_id, context)
      local vel = get_component_velocity(entity_id, context)
      
      pos.x = pos.x + vel.x * config.delta_time
      pos.y = pos.y + vel.y * config.delta_time
    end
  end
end

-- Apply friction to velocity
function apply_friction(context)
  local archetype_id = generate_velocity_archetype_id()
  
  local archetypes = context.archetypes
  if archetypes[archetype_id] then
    for _, entity_id in ipairs(archetypes[archetype_id]) do
      local vel = get_component_velocity(entity_id, context)
      
      vel.x = vel.x * config.friction
      vel.y = vel.y * config.friction
    end
  end
end

-- Boundary collision
function check_boundaries(context)
  local archetype_id = generate_archetype_id()
  local bounds_x = 800
  local bounds_y = 600
  
  local archetypes = context.archetypes
  if archetypes[archetype_id] then
    for _, entity_id in ipairs(archetypes[archetype_id]) do
      local pos = get_component_position(entity_id, context)
      local vel = get_component_velocity(entity_id, context)
      
      if pos.x < 0 or pos.x > bounds_x then
        vel.x = -vel.x
        pos.x = math.max(0, math.min(pos.x, bounds_x))
      end
      
      if pos.y < 0 or pos.y > bounds_y then
        vel.y = -vel.y
        pos.y = math.max(0, math.min(pos.y, bounds_y))
      end
    end
  end
end

-- Main simulation update (called from C++)
function update_simulation(context)
  apply_velocity(context)
  apply_friction(context)
  check_boundaries(context)
end
```

**4. LuaLogic Class**

```cpp
// File: src/logic/LuaLogic.h
class LuaLogic : public Logic {
private:
  std::shared_ptr<scripting::LuaEngine> m_lua_engine;
  std::string m_script_path;
  std::string m_update_function;
  
  void ProcessLogic() override {
    // Pass SceneContext to Lua and call update function
    sol::function update = (*m_lua_engine->GetState())[m_update_function];
    if (update.valid()) {
      update(m_scene_context);
    }
  }

public:
  LuaLogic(const SceneContext scene_context,
           std::shared_ptr<scripting::LuaEngine> engine,
           const std::string &script_path,
           const std::string &update_function);
};
```

**5. Lua Configuration**

Define which scripts to load for each scene:

```json
{
  "lua_scripts": [
    {
      "name": "movement_simulation",
      "type": "Movement",
      "scene_types": ["CRAFTING"],
      "script_path": "data/scripts/movement_simulation.lua",
      "update_function": "update_simulation",
      "execution_order": 10,
      "hot_reload": true
    },
    {
      "name": "combat_simulation",
      "type": "Action",
      "scene_types": ["CRAFTING"],
      "script_path": "data/scripts/combat_simulation.lua",
      "update_function": "process_combat",
      "execution_order": 20,
      "hot_reload": true
    }
  ]
}
```

### Implementation Steps

1. **Add Sol2 dependency**
   - Update CMakeLists.txt with FetchContent for Sol2
   - Sol2 requires Lua, add Lua dependency as well

2. **Create LuaEngine infrastructure**
   - File: `src/scripting/LuaEngine.h/cpp`
   - Implement initialization, script loading, function execution

3. **Create binding layer**
   - File: `src/scripting/bindings/SceneContextBindings.h/cpp`
   - File: `src/scripting/bindings/ComponentBindings.h/cpp`
   - File: `src/scripting/bindings/UtilityBindings.h/cpp`
   - Expose SceneContext, components, helper functions

4. **Implement LuaLogic class**
   - File: `src/logic/LuaLogic.h/cpp`
   - Wrap Lua script execution in Logic interface

5. **Create script loader**
   - File: `src/data_handlers/LuaScriptLoader.h/cpp`
   - Load Lua configuration and instantiate LuaLogic

6. **Update LogicFactory**
   - Support creating LuaLogic instances
   - Integrate with existing Logic creation

7. **Add example scripts**
   - `data/scripts/movement_simulation.lua`
   - `data/scripts/ui_logic.lua`
   - `tests/data/scripts/test_simulation.lua`

8. **Implement hot-reloading (optional)**
   - Watch Lua files for changes
   - Reload scripts without restarting game

### Pros

- **Full Scripting Power**: Complete control flow, loops, conditionals in scripts
- **Rapid Iteration**: Modify scripts without recompiling C++
- **Familiar Language**: Lua is widely known in game development
- **Hot Reload**: Change behavior while game is running
- **Modding Support**: Easy for modders to create new content
- **Sandboxing**: Lua provides security boundaries
- **Rich Ecosystem**: Many Lua libraries available

### Cons

- **Performance Overhead**: Lua is slower than C++ (though JIT helps)
- **Additional Dependency**: Requires Lua and binding library (Sol2)
- **Type Safety Lost**: Lua is dynamically typed
- **Debugging Complexity**: Need Lua debugger, different tools
- **Learning Curve**: Team needs to know both C++ and Lua
- **Integration Complexity**: Binding layer requires maintenance

### Best Use Cases

- Highly dynamic gameplay logic
- Designer-driven behavior
- Modding and user-generated content
- Prototyping experimental features
- Complex conditional logic and state machines

## Comparison Matrix

| Feature | Logic Classes | Free Functions | Lua Scripting |
|---------|--------------|----------------|---------------|
| **Type Safety** | ✓✓✓ Strong | ✓✓ Moderate | ✗ Weak |
| **Performance** | ✓✓✓ Excellent | ✓✓✓ Excellent | ✓ Good (JIT) |
| **Runtime Reconfiguration** | ✓ Limited | ✓✓ Good | ✓✓✓ Excellent |
| **Ease of Use (Programmers)** | ✓✓✓ Familiar | ✓✓ Moderate | ✓ Learning Curve |
| **Ease of Use (Designers)** | ✗ Difficult | ✓ Moderate | ✓✓✓ Easy |
| **Debugging** | ✓✓✓ C++ Tools | ✓✓ C++ Tools | ✓ Lua Tools |
| **Code Reuse** | ✓ Inheritance | ✓✓✓ Composition | ✓✓ Modules |
| **Modding Support** | ✗ None | ✗ None | ✓✓✓ Excellent |
| **Hot Reload** | ✗ No | ✗ No | ✓✓✓ Yes |
| **Compile Time** | ✓ Requires Rebuild | ✓ Requires Rebuild | ✓✓✓ None |
| **Integration Effort** | ✓✓✓ Minimal | ✓✓ Moderate | ✓ Significant |
| **Control Flow Flexibility** | ✓✓ C++ Only | ✓✓ C++ Only | ✓✓✓ Full Lua |
| **Existing Codebase Fit** | ✓✓✓ Perfect | ✓✓ Good | ✓ New Pattern |

## Recommendations

### For SteamRot Specifically

Based on the current architecture and codebase analysis, here's a recommended **hybrid approach**:

#### Phase 1: Enhanced Free Function System (Short Term)

**Recommended because:**
- Builds on existing `logic::collision`, `logic::ui` namespaces
- Low integration effort
- Maintains C++ performance and type safety
- Enables runtime pipeline configuration
- Familiar to current development patterns

**Implementation:**
1. Create FunctionRegistry for existing free functions
2. Implement SimulationPipeline class
3. Add JSON configuration for pipelines
4. Register existing free functions from `logic::collision` and `logic::ui`
5. Create new simulation functions in organized namespaces

**Example migration:**
```cpp
// Current: Logic class calls free functions directly
void UICollisionLogic::ProcessLogic() {
  // ... archetype lookup ...
  for (size_t entity_id : archetype) {
    logic::collision::CheckMouseOverNestedUIElement(
      m_scene_context.mouse_position, *ui_component.m_root_element);
  }
}

// New: SimulationPipeline calls registered free functions
// data/simulation/ui_collision_pipeline.json
{
  "steps": [
    {
      "function": "collision::check_mouse_over_nested_ui",
      "parameters": { }
    }
  ]
}
```

#### Phase 2: Lua Scripting for High-Level Logic (Medium Term)

**Recommended for:**
- Complex game rules and state machines
- Designer-driven content (dialog systems, quests, etc.)
- Prototyping new mechanics
- Modding support

**Implementation:**
1. Add Sol2 and Lua dependencies
2. Create binding layer for SceneContext and core components
3. Implement LuaLogic class
4. Start with non-performance-critical systems (UI logic, game rules)
5. Gradually expand to more areas as comfort with Lua grows

**Hybrid approach:**
```cpp
// Performance-critical: Keep as C++ free functions
namespace steamrot::simulation::physics {
  void ApplyPhysics(SceneContext &context, const Parameters &params);
  REGISTER_SIMULATION_FUNCTION(physics::apply_physics, ApplyPhysics);
}

// Game logic: Use Lua scripts
-- data/scripts/quest_logic.lua
function update_quest_system(context)
  -- Complex quest state machine in Lua
end
```

#### Phase 3: Unified System (Long Term)

**Eventually:**
- C++ free functions for performance-critical paths
- Lua scripts for gameplay logic and content
- JSON/FlatBuffers for configuration and data
- Seamless interop between all three

### Decision Guide

**Use Logic Classes when:**
- Performance is critical (rendering, physics)
- Logic is stable and rarely changes
- Type safety is paramount
- Deep C++ integration needed

**Use Free Functions when:**
- Building composable systems
- Need runtime reconfiguration
- Creating reusable utilities
- Want clear, testable units

**Use Lua Scripts when:**
- Logic changes frequently
- Designers need control
- Supporting mods
- Complex state machines needed
- Prototyping new features

## Implementation Roadmap

### Milestone 1: Free Function System (2-3 weeks)

**Week 1:**
- [ ] Design and implement FunctionRegistry
- [ ] Create simulation function namespaces structure
- [ ] Register existing `logic::collision` and `logic::ui` functions
- [ ] Write tests for FunctionRegistry

**Week 2:**
- [ ] Design FlatBuffers schema for SimulationPipeline
- [ ] Implement SimulationPipelineLoader
- [ ] Create SimulationPipeline Logic class
- [ ] Update LogicFactory to support pipelines

**Week 3:**
- [ ] Create JSON configuration files for existing scenes
- [ ] Migrate one Logic class to pipeline (e.g., UICollisionLogic)
- [ ] Write tests for SimulationPipeline
- [ ] Documentation and examples

### Milestone 2: Lua Integration (4-6 weeks)

**Week 1-2:**
- [ ] Add Sol2 and Lua dependencies to CMake
- [ ] Design binding architecture
- [ ] Implement LuaEngine infrastructure
- [ ] Create basic SceneContext bindings

**Week 3-4:**
- [ ] Bind all component types
- [ ] Bind archetype utilities
- [ ] Bind event system
- [ ] Write comprehensive binding tests

**Week 5:**
- [ ] Implement LuaLogic class
- [ ] Create LuaScriptLoader
- [ ] Integrate with LogicFactory
- [ ] Hot-reload support

**Week 6:**
- [ ] Create example Lua scripts
- [ ] Migrate one simple Logic class to Lua
- [ ] Performance benchmarking
- [ ] Documentation and tutorials

### Milestone 3: Refinement and Expansion (Ongoing)

- [ ] Optimize hot-reload mechanism
- [ ] Expand Lua bindings based on needs
- [ ] Create library of reusable simulation functions
- [ ] Build visual pipeline editor (future)
- [ ] Profiling and optimization

## Examples

### Example 1: Movement System with Free Functions

**Current Logic Class:**
```cpp
// src/logic/MovementLogic.h
class MovementLogic : public Logic {
  void ProcessLogic() override;
};

// src/logic/MovementLogic.cpp
void MovementLogic::ProcessLogic() {
  ArchetypeID archetype = GenerateArchetypeIDfromTypes<CPosition, CVelocity>();
  
  const auto it = m_scene_context.archetypes.find(archetype);
  if (it != m_scene_context.archetypes.end()) {
    for (size_t entity_id : it->second) {
      auto &pos = entity::memory::GetComponent<CPosition>(entity_id, m_scene_context.scene_entities);
      auto &vel = entity::memory::GetComponent<CVelocity>(entity_id, m_scene_context.scene_entities);
      
      pos.m_x += vel.m_x;
      pos.m_y += vel.m_y;
      
      vel.m_x *= 0.95f; // friction
      vel.m_y *= 0.95f;
    }
  }
}
```

**Converted to Free Functions:**
```cpp
// src/logic/simulation_movement.h
namespace steamrot::simulation::movement {

void ApplyVelocityToPosition(SceneContext &context, const Parameters &params) {
  ArchetypeID archetype = GenerateArchetypeIDfromTypes<CPosition, CVelocity>();
  
  const auto it = context.archetypes.find(archetype);
  if (it != context.archetypes.end()) {
    float delta_time = params.GetFloat("delta_time", 1.0f);
    
    for (size_t entity_id : it->second) {
      auto &pos = entity::memory::GetComponent<CPosition>(entity_id, context.scene_entities);
      auto &vel = entity::memory::GetComponent<CVelocity>(entity_id, context.scene_entities);
      
      pos.m_x += vel.m_x * delta_time;
      pos.m_y += vel.m_y * delta_time;
    }
  }
}

void ApplyFriction(SceneContext &context, const Parameters &params) {
  ArchetypeID archetype = GenerateArchetypeIDfromTypes<CVelocity>();
  
  const auto it = context.archetypes.find(archetype);
  if (it != context.archetypes.end()) {
    float friction = params.GetFloat("friction", 0.95f);
    
    for (size_t entity_id : it->second) {
      auto &vel = entity::memory::GetComponent<CVelocity>(entity_id, context.scene_entities);
      
      vel.m_x *= friction;
      vel.m_y *= friction;
    }
  }
}

REGISTER_SIMULATION_FUNCTION(movement::apply_velocity, ApplyVelocityToPosition);
REGISTER_SIMULATION_FUNCTION(movement::apply_friction, ApplyFriction);

} // namespace
```

**Configuration:**
```json
{
  "name": "movement_pipeline",
  "type": "Movement",
  "scene_types": ["CRAFTING"],
  "execution_order": 10,
  "steps": [
    {
      "function": "movement::apply_velocity",
      "parameters": {
        "delta_time": 0.016
      }
    },
    {
      "function": "movement::apply_friction",
      "parameters": {
        "friction": 0.95
      }
    }
  ]
}
```

### Example 2: UI Action System with Lua

**Current Logic Class:**
```cpp
// Simplified from UIActionLogic
void UIActionLogic::ProcessLogic() {
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CUserInterface>();
  
  const auto it = m_scene_context.archetypes.find(archetype_id);
  if (it != m_scene_context.archetypes.end()) {
    for (size_t entity_id : it->second) {
      CUserInterface &ui = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);
      
      // Complex nested processing logic
      ProcessNestedUIActionsAndEvents(*ui.m_root_element,
                                     m_scene_context.event_handler,
                                     m_scene_context);
    }
  }
}
```

**Converted to Lua:**
```lua
-- data/scripts/ui_actions.lua

-- Process a single UI element
function process_ui_element(element, event_handler)
  if not element.subscription or not element.subscription:is_active() then
    return false
  end
  
  -- Check element type and process accordingly
  if element:is_button() then
    if element.is_mouse_over and element.response_event then
      event_handler:add_event(element.response_event)
      element.subscription:set_inactive()
      return true
    end
  elseif element:is_dropdown() then
    if element.data_populate_function ~= "None" then
      populate_dropdown_data(element)
      element.subscription:set_inactive()
      return true
    end
  end
  
  return false
end

-- Process element and children recursively
function process_nested_ui_elements(element, event_handler)
  -- Process children first (depth-first)
  for _, child in ipairs(element.child_elements) do
    if process_nested_ui_elements(child, event_handler) then
      return true  -- Child was processed, skip parent
    end
  end
  
  -- Process this element if no child was processed
  return process_ui_element(element, event_handler)
end

-- Main update function called from C++
function update_ui_actions(context)
  local archetype_id = generate_ui_archetype_id()
  local archetypes = context.archetypes
  
  if archetypes[archetype_id] then
    for _, entity_id in ipairs(archetypes[archetype_id]) do
      local ui = get_component_user_interface(entity_id, context)
      
      if ui.m_root_element then
        process_nested_ui_elements(ui.m_root_element, context.event_handler)
      end
    end
  end
end
```

**Configuration:**
```json
{
  "name": "ui_action_logic",
  "type": "Action",
  "scene_types": ["TITLE", "CRAFTING"],
  "script_path": "data/scripts/ui_actions.lua",
  "update_function": "update_ui_actions",
  "execution_order": 10,
  "hot_reload": true
}
```

### Example 3: Hybrid System

Combine C++ free functions for performance and Lua for flexibility:

**C++ Free Function (Performance-Critical):**
```cpp
// src/logic/simulation_physics.h
namespace steamrot::simulation::physics {

void DetectCollisions(SceneContext &context, const Parameters &params) {
  // Fast collision detection in C++
  ArchetypeID archetype = GenerateArchetypeIDfromTypes<CPosition, CCollider>();
  
  const auto it = context.archetypes.find(archetype);
  if (it == context.archetypes.end()) return;
  
  const auto& entities = it->second;
  
  // Broad phase: spatial hashing (fast)
  std::unordered_map<int, std::vector<size_t>> spatial_hash;
  for (size_t entity_id : entities) {
    auto &pos = entity::memory::GetComponent<CPosition>(entity_id, context.scene_entities);
    int hash = ComputeSpatialHash(pos.m_x, pos.m_y);
    spatial_hash[hash].push_back(entity_id);
  }
  
  // Narrow phase: detailed collision checks
  for (const auto &[hash, cell_entities] : spatial_hash) {
    for (size_t i = 0; i < cell_entities.size(); ++i) {
      for (size_t j = i + 1; j < cell_entities.size(); ++j) {
        CheckAndResolveCollision(cell_entities[i], cell_entities[j], context);
      }
    }
  }
}

REGISTER_SIMULATION_FUNCTION(physics::detect_collisions, DetectCollisions);

} // namespace
```

**Lua Script (Game Logic):**
```lua
-- data/scripts/combat_system.lua

-- Damage resolution (game rules, frequently changes)
function resolve_damage(attacker_id, target_id, context)
  local attacker_stats = get_component_stats(attacker_id, context)
  local target_stats = get_component_stats(target_id, context)
  local target_health = get_component_health(target_id, context)
  
  -- Complex damage calculation with modifiers
  local base_damage = attacker_stats.attack
  local defense_reduction = target_stats.defense * 0.5
  local critical_multiplier = 1.0
  
  -- Random critical hit
  if math.random() < attacker_stats.critical_chance then
    critical_multiplier = 2.0
    print("Critical hit!")
  end
  
  -- Apply damage
  local final_damage = math.max(1, (base_damage - defense_reduction) * critical_multiplier)
  target_health.current = target_health.current - final_damage
  
  -- Check for death
  if target_health.current <= 0 then
    handle_entity_death(target_id, context)
  end
end

-- Main combat update
function update_combat(context)
  -- Get entities in combat
  local combat_archetype = generate_combat_archetype_id()
  local archetypes = context.archetypes
  
  if archetypes[combat_archetype] then
    for _, entity_id in ipairs(archetypes[combat_archetype]) do
      local combat = get_component_combat(entity_id, context)
      
      if combat.has_target and combat.attack_ready then
        resolve_damage(entity_id, combat.target_id, context)
        combat.attack_ready = false
        combat.cooldown_timer = combat.attack_speed
      end
    end
  end
end
```

**Pipeline Configuration:**
```json
{
  "simulation_pipelines": [
    {
      "name": "game_simulation",
      "type": "Action",
      "scene_types": ["CRAFTING"],
      "execution_order": 15,
      "steps": [
        {
          "function": "physics::detect_collisions",
          "parameters": {
            "cell_size": 64
          }
        },
        {
          "type": "lua_script",
          "script": "data/scripts/combat_system.lua",
          "function": "update_combat",
          "parameters": {}
        }
      ]
    }
  ]
}
```

## Conclusion

SteamRot has a solid foundation for implementing any of these data-driven simulation approaches. The recommended **hybrid approach** leverages the strengths of each:

1. **Start with Free Functions** - Build on existing patterns, low risk, immediate benefits
2. **Add Lua Selectively** - For complex logic, designer content, and prototyping
3. **Keep Critical Paths in C++** - Maintain performance where it matters

This phased approach allows incremental adoption, minimizes risk, and provides maximum flexibility for future development.

The architecture is designed to be **extensible** - you can mix and match approaches based on specific needs:
- Performance-critical: C++ Logic classes or free functions
- Flexible content: Lua scripts
- Configuration: JSON/FlatBuffers data files

Each approach has its place in a modern game engine, and SteamRot's architecture supports all three seamlessly.
