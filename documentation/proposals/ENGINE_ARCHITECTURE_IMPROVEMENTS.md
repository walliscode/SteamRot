# Engine Architecture Improvement Proposal

**Date**: December 4, 2025  
**Status**: Proposal  
**Type**: Architecture Refinement

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Current Architecture Analysis](#current-architecture-analysis)
3. [Identified Issues](#identified-issues)
4. [Proposed Improvements](#proposed-improvements)
5. [Implementation Strategy](#implementation-strategy)
6. [Benefits and Trade-offs](#benefits-and-trade-offs)
7. [Migration Path](#migration-path)

---

## Executive Summary

This document proposes improvements to the Engine/GameEngine/TestEngine class hierarchy to address current architectural concerns around:

- **Clarity of responsibility** - Some virtual methods have unclear purposes
- **Tick execution flexibility** - Limited control over tick granularity
- **Configuration injection** - Mixed patterns between file loading and injection
- **Testing ergonomics** - TestEngine setup could be more intuitive
- **Code duplication** - Similar patterns repeated across derived classes

The proposed improvements maintain backward compatibility while introducing clearer abstractions and more flexible execution control.

---

## Current Architecture Analysis

### Class Hierarchy

```
Engine (abstract)
  ├─ Protected members: m_game_core, m_game_context, m_scene_manager, m_user_preferences
  ├─ Virtual methods: ConfigureEngineStateFromData(), ExecuteSceneLevelLogic(), 
  │                   ExecuteDisplayManagerTick(), RunGameLoop(), ProcessSubscriptions()
  └─ Concrete methods: StartUp(), ExecuteSystemsTick(), ExecuteEngineLevelLogic()
      │
      ├─ GameEngine
      │    ├─ Additional: m_display_manager
      │    ├─ Overrides: StartUp(), ConfigureEngineStateFromData(), ExecuteSceneLevelLogic(),
      │    │             ExecuteDisplayManagerTick(), RunGameLoop(), ProcessSubscriptions()
      │    └─ Data source: Files (engine_data.json, default.preferences.bin)
      │
      └─ TestEngine
           ├─ Additional: m_test_config*, m_simulation_data, m_target_ticks, m_data_bank
           ├─ Overrides: ConfigureEngineStateFromData(), ExecuteSceneLevelLogic(),
           │             ExecuteDisplayManagerTick(), RunGameLoop(), ProcessSubscriptions()
           └─ Data source: Injected (TestDataConfig*) + Files (default.preferences.bin)
```

### Current Execution Flow

**GameEngine Tick:**
```
ExecuteSystemsTick()
  ├─ EventHandler::ExecuteEventHandlerLevelLogic()
  ├─ ExecuteEngineLevelLogic()
  │  └─ ProcessSubscriptions() → Check EVENT_QUIT_GAME
  ├─ SceneManager::ExecuteSceneManagerLevelLogic()
  ├─ ExecuteSceneLevelLogic() → SceneManager::UpdateScenes()
  │  └─ sAction() → sCollision() → sRender()
  └─ ExecuteDisplayManagerTick() → DisplayManager::CallRenderCycle()
```

**TestEngine Tick:**
```
ExecuteSystemsTick() [same as GameEngine]
  └─ ExecuteSceneLevelLogic() → simulation_runner::RunSimulationStep()
     └─ Configurable: FullEngine / SceneManager / SceneLogic / Custom
```

### Strengths

1. **Shared Core Management** - GameCore, GameContext, SceneManager properly shared
2. **Consistent Startup** - Both engines use same StartUp() flow for window/preferences
3. **Virtual Hook Pattern** - Allows specialization at key points
4. **TestEngine Flexibility** - TickLevel enum provides execution granularity

### Weaknesses

1. **Unclear Method Names**
   - `ExecuteSceneLevelLogic()` - Not immediately clear this means "run scene logic systems"
   - `ExecuteDisplayManagerTick()` - Specific to one implementation (GameEngine)
   - `ExecuteEngineLevelLogic()` - Only calls ProcessSubscriptions()

2. **Mixed Responsibilities**
   - Engine has both orchestration (ExecuteSystemsTick) and execution (ExecuteEngineLevelLogic)
   - StartUp() does configuration but also calls virtual methods
   - ProcessSubscriptions() mixes iteration with business logic

3. **Limited Extensibility**
   - Hard to add new execution phases without modifying base class
   - Virtual methods are too coarse-grained (all-or-nothing override)
   - No hooks for pre/post phases

4. **Configuration Pattern Inconsistency**
   - GameEngine: All file-based
   - TestEngine: Mixed (files + injection)
   - No clear interface for configuration sources

5. **Testing Complexity**
   - TestEngine requires understanding TickLevel, simulation_data, and TestDataConfig
   - Relationship between TickLevel and simulation_data is unclear
   - Data bank is implicit (happens automatically)

---

## Identified Issues

### Issue 1: Execution Phase Abstraction

**Problem**: The current virtual methods don't clearly represent the execution phases.

**Example**:
```cpp
virtual void ExecuteSceneLevelLogic() = 0;
virtual void ExecuteDisplayManagerTick() = 0;
```

These names don't convey:
- What "level" means
- Why DisplayManager gets its own tick method
- How these relate to the game loop structure

**Impact**: Developers need to read implementation to understand execution flow.

### Issue 2: Subscription Processing

**Problem**: ProcessSubscriptions() is abstract but each implementation duplicates similar patterns.

**Current**:
```cpp
// GameEngine
for (const auto &subscriber : m_subscriptions) {
  if (subscriber->m_active) {
    switch (subscriber->m_trigger_event_type) {
      case EventType::EventType_EVENT_QUIT_GAME: { /* ... */ break; }
      default: break;
    }
    subscriber->m_active = false;
  }
}

// TestEngine
for (const auto &subscriber : m_subscriptions) {
  if (subscriber->m_active) {
    switch (subscriber->m_trigger_event_type) {
      case EventType::EventType_TEST_SPECIFIC: { /* ... */ break; }
      default: break;
    }
    subscriber->m_active = false;
  }
}
```

**Impact**: Code duplication, hard to add engine-agnostic subscription behaviors.

### Issue 3: Configuration Strategy

**Problem**: No clear interface for configuration sources.

**Current**:
```cpp
virtual std::expected<std::monostate, FailInfo> ConfigureEngineStateFromData() = 0;
```

This requires:
- GameEngine to hardcode file paths
- TestEngine to store config pointer
- No ability to swap data sources

**Impact**: Can't easily test GameEngine with mock data, or run TestEngine with files.

### Issue 4: Tick Granularity

**Problem**: ExecuteSystemsTick() is monolithic - all or nothing.

**Current**: If you want to test just event handling, you must:
1. Create a TestEngine
2. Set TickLevel to Custom
3. Configure simulation_data to disable other systems
4. Hope the interaction ordering is correct

**Impact**: Hard to write focused integration tests.

### Issue 5: Testing Ergonomics

**Problem**: TestEngine setup requires understanding multiple interdependent concepts.

**Concepts needed**:
- TestDataConfig structure (starting_engine_state, simulation_data, num_ticks)
- TickLevel enum (FullEngine, SceneManager, SceneLogic, Custom)
- SimulationData structure (steps, logic_classes, free_functions)
- How these interact

**Impact**: High barrier to writing tests, confusion about which to use when.

---

## Proposed Improvements

### Improvement 1: Unified Tick Pipeline with Fine-Grained Control

**Proposal**: Replace ad-hoc virtual methods with a clear `Tick_()` nomenclature that provides both high-level pipeline structure and fine-grained component access.

**Design Philosophy**:
- Use consistent `Tick_()` naming for all execution phases
- Each tick method is a complete, composable unit
- Methods can be called individually for testing or composed for full execution
- Virtual methods allow derived classes to customize behavior
- Public tick methods enable external callers (tests) to execute specific phases

**New Structure**:
```cpp
class Engine {
protected:
  // Hook methods (virtual, optional to override)
  virtual void OnTickBegin() {}
  virtual void OnTickEnd() {}
  
  // Core tick methods (virtual for customization, public for direct access)
public:
  virtual void TickEvents();           // Process SFML events, update event bus
  virtual void TickEngineLogic();      // Process engine-level subscriptions
  virtual void TickSceneManager();     // Process scene manager subscriptions
  virtual void TickSceneLogic() = 0;   // Must implement: scene-specific logic
  virtual void TickRendering();        // Render to display (may be no-op)
  
  // Complete tick execution (calls all phases in order)
  void ExecuteTick() {
    OnTickBegin();
    TickEvents();
    TickEngineLogic();
    TickSceneManager();
    TickSceneLogic();
    TickRendering();
    OnTickEnd();
  }
};
```

**Key Advantages**:
1. **Consistent Naming** - All execution methods use `Tick_()` pattern
2. **Composable** - Each method is independently testable
3. **Clear Order** - Method names indicate execution sequence
4. **Flexible** - Can override entire phase or call base implementation
5. **Testable** - Public methods allow direct testing of individual phases

**GameEngine Implementation**:
```cpp
class GameEngine : public Engine {
public:
  void TickSceneLogic() override {
    // GameEngine uses real scene updates
    m_scene_manager.UpdateScenes();
  }

  void TickRendering() override {
    // GameEngine renders to display
    m_display_manager.CallRenderCycle();
  }
  
  void OnTickEnd() override {
    Engine::OnTickEnd();
    // GameEngine can add logging, profiling, etc.
  }
};
```

**TestEngine Implementation**:
```cpp
class TestEngine : public Engine {
public:
  void TickSceneLogic() override {
    // TestEngine uses simulation data to control execution
    if (m_simulation_data) {
      simulation_runner::RunSimulationStep(m_simulation_data, GetSceneContext());
    } else {
      // Fall back to normal scene updates for simple tests
      m_scene_manager.UpdateScenes();
    }
  }

  void TickRendering() override {
    // TestEngine doesn't render to display
    // (but scenes still render to their texture for validation)
  }

  void OnTickEnd() override {
    Engine::OnTickEnd();
    // Capture data bank snapshot after each tick
    if (m_capture_data_bank) {
      AddToDataBank(m_current_tick);
    }
  }
};
```

**Testing Individual Phases**:
```cpp
TEST_CASE("Event handler processes keyboard input", "[unit][Engine]") {
  TestEngine engine(simple_config);
  engine.StartUp();
  
  // Simulate keyboard press
  sf::Event event;
  event.type = sf::Event::KeyPressed;
  event.key.code = sf::Keyboard::A;
  // ... inject event ...
  
  // Test ONLY event handling phase
  engine.TickEvents();
  
  // Verify event bus state
  auto& event_bus = engine.GetGameContext().event_handler.GetGlobalEventBus();
  REQUIRE(event_bus.size() > 0);
  REQUIRE(event_bus[0].event_data.index() == /* KeyPressed index */);
}

TEST_CASE("Scene logic processes entities", "[unit][Engine]") {
  TestEngine engine(config_with_entities);
  engine.StartUp();
  
  // Test ONLY scene logic phase
  engine.TickSceneLogic();
  
  // Verify entity state changes
  auto final_state = engine.GetSceneSnapshot();
  REQUIRE(final_state.entity_count > 0);
}

TEST_CASE("Full tick integration", "[integration][Engine]") {
  TestEngine engine(config);
  engine.StartUp();
  
  // Test complete tick execution
  engine.ExecuteTick();
  
  // Verify all phases executed correctly
  REQUIRE(engine.GetTickNumber() == 1);
}
```

**Comparison with Old Approach**:

| Old Approach | New Approach | Benefit |
|--------------|--------------|---------|
| `ExecuteSceneLevelLogic()` | `TickSceneLogic()` | Clear naming |
| `ExecuteDisplayManagerTick()` | `TickRendering()` | Generic, not GameEngine-specific |
| `ExecuteEngineLevelLogic()` | `TickEngineLogic()` | Clearer scope |
| Monolithic `ExecuteSystemsTick()` | Composable `Tick_()` methods | Fine-grained testing |
| Private virtual methods | Public virtual methods | Direct test access |

### Improvement 2: Subscription Handler Strategy

**Proposal**: Extract subscription handling into a strategy pattern.

**New Interface**:
```cpp
class ISubscriptionHandler {
public:
  virtual ~ISubscriptionHandler() = default;
  virtual void ProcessSubscription(Subscriber& sub, GameCore& core) = 0;
};

class GameEngineSubscriptionHandler : public ISubscriptionHandler {
public:
  void ProcessSubscription(Subscriber& sub, GameCore& core) override {
    switch (sub.m_trigger_event_type) {
      case EventType::EventType_EVENT_QUIT_GAME:
        core.game_window.close();
        break;
      // ... other game-specific subscriptions
    }
  }
};

class Engine {
protected:
  std::unique_ptr<ISubscriptionHandler> m_subscription_handler;
  
  void TickEngineLogic() {  // No longer needs to be virtual
    for (auto& sub : m_subscriptions) {
      if (sub->m_active) {
        m_subscription_handler->ProcessSubscription(*sub, m_game_core);
        sub->m_active = false;
      }
    }
  }
};
```

**Benefits**:
- Eliminates code duplication between GameEngine and TestEngine
- Easy to add new subscription types without modifying Engine
- Can swap handlers for testing different subscription behaviors
- Separation of concerns - Engine orchestrates, handler decides

### Improvement 3: Configuration Source Abstraction

**Proposal**: Introduce configuration provider interface.

**New Interface**:
```cpp
class IEngineConfiguration {
public:
  virtual ~IEngineConfiguration() = default;
  virtual std::expected<const EngineData*, FailInfo> GetEngineData() = 0;
  virtual std::expected<UserPreferences, FailInfo> GetUserPreferences() = 0;
  virtual std::expected<const EngineCoreData*, FailInfo> GetEngineCoreData() = 0;
};

class FileEngineConfiguration : public IEngineConfiguration {
  // Uses FlatbuffersDataLoader
};

class InjectedEngineConfiguration : public IEngineConfiguration {
  // Uses provided TestDataConfig
};

class Engine {
protected:
  std::unique_ptr<IEngineConfiguration> m_config_source;
  
  std::expected<std::monostate, FailInfo> StartUp() {
    auto core_data = m_config_source->GetEngineCoreData();
    auto preferences = m_config_source->GetUserPreferences();
    auto engine_data = m_config_source->GetEngineData();
    // ... use data ...
  }
};
```

**Benefits**:
- GameEngine can be tested with mock data
- TestEngine can use files if desired
- Clear dependency injection point
- Easier to add new configuration sources (network, database, etc.)

### Improvement 4: Hybrid TestEngine Configuration

**Proposal**: Enhance TestEngine to work seamlessly with data-driven test harness while maintaining programmatic configuration for simple tests.

**Design Philosophy**:
The TestEngine should primarily be configured via TestDataConfig (data-driven), but provide convenience methods for simple, focused tests that don't require full JSON configuration files.

**Updated TestEngine Interface**:
```cpp
class TestEngine : public Engine {
private:
  const TestDataConfig* m_test_config{nullptr};
  const SimulationData* m_simulation_data{nullptr};
  size_t m_target_ticks{1};
  bool m_capture_data_bank{false};
  
public:
  // Primary constructor - Data-driven approach (existing)
  explicit TestEngine(const TestDataConfig* config);
  
  // Convenience constructors for focused tests (new)
  explicit TestEngine(SceneType scene_type, size_t num_ticks = 1);
  
  // Configuration methods for programmatic setup
  TestEngine& WithSimulation(const SimulationData* data) {
    m_simulation_data = data;
    return *this;
  }
  
  TestEngine& EnableDataBank() {
    m_capture_data_bank = true;
    return *this;
  }
  
  // Execution control (data-driven or manual)
  void RunGame() override;  // Uses m_target_ticks from config or constructor
  void RunSingleTick();     // Manual control for step-by-step testing
};
```

**Data-Driven Usage (Primary)**:
```cpp
TEST_CASE("Complex UI workflow", "[integration][data-driven]") {
  // Load comprehensive test configuration from JSON
  auto test_data = LoadTestData("ui_workflow_001.test_data.json");
  const TestDataConfig* config = test_data.get();
  
  // TestDataConfig contains:
  // - starting_engine_state (scenes, entities, components)
  // - simulation_data (which Logic classes to run, in what order)
  // - num_ticks (how many iterations)
  // - expected_entity_collection (validation data)
  
  TestEngine engine(config);
  engine.RunGame();  // Executes according to simulation_data
  
  // Validate using test harness
  auto result = CompareEngineState(engine, config);
  REQUIRE(result.has_value());
}
```

**Programmatic Usage (Convenience for Simple Tests)**:
```cpp
TEST_CASE("Event handler keyboard input", "[unit]") {
  // Simple focused test - no JSON needed
  TestEngine engine(SceneType::SceneType_TITLE, 1);
  engine.StartUp();
  
  // Directly test a specific tick phase
  engine.TickEvents();
  
  // Simple validation
  auto& event_bus = engine.GetGameContext().event_handler.GetGlobalEventBus();
  REQUIRE(event_bus.size() > 0);
}

TEST_CASE("Scene logic transformation", "[unit]") {
  // Slightly more complex - still no JSON
  TestEngine engine(SceneType::SceneType_CRAFTING, 5);
  engine.EnableDataBank();  // Opt-in to data capture
  engine.RunGame();
  
  // Access captured states
  auto initial_state = engine.GetDataBank().at(1);
  auto final_state = engine.GetDataBank().at(5);
  
  // Validate transformation
  REQUIRE(final_state.entity_count != initial_state.entity_count);
}
```

**Key Differences from Original Proposal**:

| Aspect | Original Proposal | Revised Hybrid Approach |
|--------|-------------------|-------------------------|
| Primary Use Case | Builder pattern for all tests | Data-driven with programmatic fallback |
| Configuration Source | Programmatic API calls | TestDataConfig JSON files |
| Logic Execution Control | Via builder methods | Via SimulationData in config |
| Simple Tests | Still requires builder | Lightweight convenience constructors |
| Complex Tests | Multiple builder calls | Single JSON file |
| Data Bank | Opt-in via builder | Opt-in via method or config |

**Rationale**:
- **TestDataConfig is the source of truth** - Complex tests with multiple ticks, specific Logic execution orders, and validation data should use JSON files
- **SimulationData defines execution** - What Logic classes run, in what order, with what parameters - all from data, not code
- **Convenience constructors for focused tests** - Unit tests that check a single Tick method don't need full JSON infrastructure
- **No duplication** - TestEngineBuilder would duplicate TestDataConfig structure; instead, enhance TestEngine directly
- **Backward compatible** - Existing tests using TestDataConfig continue to work

### Improvement 5: Observable Execution with Practical Use Cases

**Proposal**: Add observer pattern for monitoring engine execution without modifying the Engine class.

**Design Philosophy**:
- Non-invasive observation of engine execution
- Multiple observers can attach to single engine
- Observers can be added/removed at runtime
- Zero performance impact when no observers attached
- Enables logging, profiling, debugging, and test assertions

**Observer Interface**:
```cpp
class IEngineObserver {
public:
  virtual ~IEngineObserver() = default;
  
  // Tick lifecycle events
  virtual void OnTickBegin(size_t tick_number) {}
  virtual void OnTickEnd(size_t tick_number) {}
  
  // Phase completion events
  virtual void OnPhaseComplete(const std::string& phase_name, size_t tick_number) {}
  
  // Error and warning events
  virtual void OnError(const FailInfo& error, size_t tick_number) {}
  virtual void OnWarning(const std::string& warning, size_t tick_number) {}
  
  // State change events
  virtual void OnSceneChange(SceneType old_scene, SceneType new_scene) {}
  virtual void OnEntityCreated(size_t entity_id, SceneID scene_id) {}
  virtual void OnEntityDestroyed(size_t entity_id, SceneID scene_id) {}
};

class Engine {
protected:
  std::vector<std::weak_ptr<IEngineObserver>> m_observers;
  
  template<typename Notifier>
  void NotifyObservers(Notifier&& notifier) {
    // Clean up expired observers
    m_observers.erase(
      std::remove_if(m_observers.begin(), m_observers.end(),
        [](const auto& weak) { return weak.expired(); }),
      m_observers.end()
    );
    
    // Notify remaining observers
    for (auto& weak_obs : m_observers) {
      if (auto obs = weak_obs.lock()) {
        notifier(obs.get());
      }
    }
  }
  
  // Updated tick methods with notifications
  void ExecuteTick() {
    NotifyObservers([this](IEngineObserver* obs) {
      obs->OnTickBegin(m_game_core.loop_number);
    });
    
    OnTickBegin();
    TickEvents();
    NotifyObservers([this](IEngineObserver* obs) {
      obs->OnPhaseComplete("Events", m_game_core.loop_number);
    });
    
    TickEngineLogic();
    NotifyObservers([this](IEngineObserver* obs) {
      obs->OnPhaseComplete("EngineLogic", m_game_core.loop_number);
    });
    
    TickSceneManager();
    TickSceneLogic();
    TickRendering();
    OnTickEnd();
    
    NotifyObservers([this](IEngineObserver* obs) {
      obs->OnTickEnd(m_game_core.loop_number);
    });
  }
  
public:
  void AddObserver(std::shared_ptr<IEngineObserver> observer) {
    m_observers.push_back(observer);
  }
  
  void RemoveObserver(std::shared_ptr<IEngineObserver> observer) {
    m_observers.erase(
      std::remove_if(m_observers.begin(), m_observers.end(),
        [&observer](const auto& weak) {
          auto ptr = weak.lock();
          return !ptr || ptr == observer;
        }),
      m_observers.end()
    );
  }
};
```

**Practical Use Case 1: Performance Profiling**:
```cpp
class PerformanceProfiler : public IEngineObserver {
private:
  std::unordered_map<std::string, std::chrono::microseconds> m_phase_times;
  std::chrono::steady_clock::time_point m_phase_start;
  std::string m_current_phase;
  
public:
  void OnTickBegin(size_t tick_number) override {
    m_phase_start = std::chrono::steady_clock::now();
    m_current_phase = "FullTick";
  }
  
  void OnPhaseComplete(const std::string& phase_name, size_t tick_number) override {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      now - m_phase_start);
    
    m_phase_times[phase_name] += duration;
    m_phase_start = now;
  }
  
  void OnTickEnd(size_t tick_number) override {
    if (tick_number % 100 == 0) {
      // Log every 100 ticks
      spdlog::info("Performance Profile (last 100 ticks):");
      for (const auto& [phase, time] : m_phase_times) {
        spdlog::info("  {}: {} μs/tick", phase, time.count() / 100);
      }
      m_phase_times.clear();
    }
  }
};

// Usage
GameEngine engine;
auto profiler = std::make_shared<PerformanceProfiler>();
engine.AddObserver(profiler);
engine.RunGame();  // Automatic profiling throughout execution
```

**Practical Use Case 2: Test Assertions**:
```cpp
class StateValidationObserver : public IEngineObserver {
private:
  std::function<void(size_t)> m_validation_func;
  size_t m_target_tick;
  bool m_validation_passed{false};
  
public:
  StateValidationObserver(size_t target_tick, 
                         std::function<void(size_t)> validator)
    : m_target_tick(target_tick), m_validation_func(validator) {}
  
  void OnTickEnd(size_t tick_number) override {
    if (tick_number == m_target_tick) {
      try {
        m_validation_func(tick_number);
        m_validation_passed = true;
      } catch (const std::exception& e) {
        spdlog::error("Validation failed at tick {}: {}", tick_number, e.what());
        throw;
      }
    }
  }
  
  bool ValidationPassed() const { return m_validation_passed; }
};

// Usage in tests
TEST_CASE("Entity transformation after 5 ticks", "[integration]") {
  TestEngine engine(config);
  
  auto validator = std::make_shared<StateValidationObserver>(5, 
    [&engine](size_t tick) {
      auto scene = engine.GetScene(SceneID{0});
      auto entity_count = scene.GetEntityCount();
      REQUIRE(entity_count > 0);
      REQUIRE(entity_count < 100);  // Some entities destroyed
    });
  
  engine.AddObserver(validator);
  engine.RunGame();
  
  REQUIRE(validator->ValidationPassed());
}
```

**Practical Use Case 3: Debug Logging**:
```cpp
class DebugLogger : public IEngineObserver {
private:
  bool m_verbose;
  
public:
  explicit DebugLogger(bool verbose = false) : m_verbose(verbose) {}
  
  void OnTickBegin(size_t tick_number) override {
    spdlog::debug("=== Tick {} begin ===", tick_number);
  }
  
  void OnPhaseComplete(const std::string& phase_name, size_t tick_number) override {
    if (m_verbose) {
      spdlog::debug("  Tick {} - {} complete", tick_number, phase_name);
    }
  }
  
  void OnSceneChange(SceneType old_scene, SceneType new_scene) override {
    spdlog::info("Scene transition: {} -> {}", 
                 magic_enum::enum_name(old_scene),
                 magic_enum::enum_name(new_scene));
  }
  
  void OnError(const FailInfo& error, size_t tick_number) override {
    spdlog::error("Tick {} - Error: {}", tick_number, error.message);
  }
  
  void OnEntityCreated(size_t entity_id, SceneID scene_id) override {
    if (m_verbose) {
      spdlog::debug("  Entity {} created in scene {}", entity_id, scene_id.id);
    }
  }
};

// Usage - can be toggled via configuration
#ifdef STEAMROT_DEBUG_BUILD
  auto logger = std::make_shared<DebugLogger>(true /* verbose */);
  engine.AddObserver(logger);
#endif
```

**Practical Use Case 4: Data Collection for Analysis**:
```cpp
class DataCollector : public IEngineObserver {
private:
  struct TickData {
    size_t tick_number;
    size_t entity_count;
    size_t event_bus_size;
    std::chrono::microseconds duration;
  };
  
  std::vector<TickData> m_collected_data;
  std::chrono::steady_clock::time_point m_tick_start;
  
public:
  void OnTickBegin(size_t tick_number) override {
    m_tick_start = std::chrono::steady_clock::now();
  }
  
  void OnTickEnd(size_t tick_number) override {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::steady_clock::now() - m_tick_start);
    
    // Collect data (would need access to engine state)
    TickData data{tick_number, 0, 0, duration};
    m_collected_data.push_back(data);
  }
  
  void ExportToCSV(const std::string& filename) {
    std::ofstream file(filename);
    file << "Tick,EntityCount,EventBusSize,Duration_us\n";
    for (const auto& data : m_collected_data) {
      file << data.tick_number << ","
           << data.entity_count << ","
           << data.event_bus_size << ","
           << data.duration.count() << "\n";
    }
  }
};

// Usage for performance analysis
TEST_CASE("Performance under load", "[perf]") {
  auto collector = std::make_shared<DataCollector>();
  
  TestEngine engine(high_load_config);
  engine.AddObserver(collector);
  engine.RunGame();
  
  collector->ExportToCSV("performance_data.csv");
  // Analyze CSV with external tools (Excel, Python, etc.)
}
```

**Benefits**:
- **Non-invasive** - Engine doesn't need to know about observers
- **Flexible** - Can attach multiple observers for different purposes
- **Runtime control** - Add/remove observers during execution
- **Zero cost when unused** - No observers = no overhead
- **Testable** - Observers themselves can be unit tested
- **Composable** - Multiple observers work together
- **Production-ready** - Can be used in both GameEngine and TestEngine

**Implementation Notes**:
- Use `std::weak_ptr` to avoid circular dependencies
- Clean up expired observers periodically
- Notification should be fast (observers shouldn't block)
- Consider event queuing for async observers
- Provide observer base classes for common patterns
  
  // Only test event handling
  engine.TickEventHandler();
  
  // Verify event bus state
  REQUIRE(engine.GetGameCore().event_handler.GetGlobalEventBus().size() > 0);
}
```

---

## Implementation Strategy

### Phase 1: Unified Tick Pipeline (Week 1-2)

**Add new Tick_() methods alongside old ones:**
```cpp
class Engine {
protected:
  // Old methods (deprecated but working)
  virtual void ExecuteSceneLevelLogic() = 0;  // [[deprecated]]
  virtual void ExecuteDisplayManagerTick() = 0;  // [[deprecated]]
  
public:
  // New tick methods (public for testing)
  virtual void TickEvents() { /* default impl */ }
  virtual void TickEngineLogic() { /* default impl */ }
  virtual void TickSceneManager() { /* default impl */ }
  virtual void TickSceneLogic() { ExecuteSceneLevelLogic(); }  // Delegates to old
  virtual void TickRendering() { ExecuteDisplayManagerTick(); }  // Delegates to old
  
  void ExecuteTick() {
    TickEvents();
    TickEngineLogic();
    TickSceneManager();
    TickSceneLogic();
    TickRendering();
  }
};
```

**Deliverables**:
- New Tick_() methods in Engine
- ExecuteTick() using new methods
- All existing code continues to work
- Tests can now call individual Tick_() methods

**Timeline**: 2 weeks  
**Risk**: Low - No breaking changes

### Phase 2: Configuration Abstraction (Week 3-4)

**Introduce IEngineConfiguration:**
```cpp
class Engine {
protected:
  std::unique_ptr<IEngineConfiguration> m_config_source;
  
  std::expected<std::monostate, FailInfo> StartUp() {
    auto core_data = m_config_source->GetEngineCoreData();
    auto preferences = m_config_source->GetUserPreferences();
    // ... use data ...
  }
};
```

**Deliverables**:
- IEngineConfiguration interface
- FileEngineConfiguration implementation
- InjectedEngineConfiguration implementation
- Engine uses config source in StartUp()

**Timeline**: 2 weeks  
**Risk**: Medium - Changes StartUp() flow

### Phase 3: Subscription Strategy (Week 5)

**Extract subscription handling:**
- Create ISubscriptionHandler interface
- Implement GameEngineSubscriptionHandler
- Implement TestEngineSubscriptionHandler
- Update TickEngineLogic() to use handler

**Timeline**: 1 week  
**Risk**: Medium - Changes subscription flow

### Phase 4: TestEngine Hybrid Approach (Week 6)

**Enhance TestEngine with convenience constructors:**
```cpp
class TestEngine : public Engine {
public:
  // Primary constructor (existing)
  explicit TestEngine(const TestDataConfig* config);
  
  // Convenience constructors (new)
  explicit TestEngine(SceneType scene_type, size_t num_ticks = 1);
  
  // Fluent configuration (new)
  TestEngine& WithSimulation(const SimulationData* data);
  TestEngine& EnableDataBank();
};
```

**Timeline**: 1 week  
**Risk**: Low - Additive only

### Phase 5: Observer Pattern (Week 7)

**Add observer support:**
- Create IEngineObserver interface
- Add observer management to Engine
- Add notification points in ExecuteTick()
- Implement example observers (profiler, logger, validator)

**Timeline**: 1 week  
**Risk**: Low - Optional feature

### Phase 6: Migration and Documentation (Week 8)

**Complete migration:**
- Mark old methods as deprecated
- Migrate GameEngine to new Tick_() methods
- Migrate TestEngine to new Tick_() methods
- Update all tests
- Update documentation
- Create migration guide

**Timeline**: 1 week  
**Risk**: Medium - Requires coordinated changes

### Phase 7: Removal (Week 9, Optional)

**Remove deprecated methods:**
- Only after all code migrated
- Verify no external dependencies
- Final cleanup

**Timeline**: 1 week  
**Risk**: Low - Everything already migrated

**Total Timeline**: 8-9 weeks  
**Total Risk**: Low-Medium

---

## Benefits and Trade-offs

### Benefits

1. **Improved Clarity**
   - Method names clearly indicate purpose
   - Execution flow is self-documenting
   - Less cognitive load for new developers

2. **Enhanced Testability**
   - Fine-grained tick control
   - Easy to test individual phases
   - Mock configuration sources

3. **Better Extensibility**
   - Easy to add new execution phases
   - Observer pattern for monitoring
   - Strategy pattern for behaviors

4. **Reduced Duplication**
   - Subscription handling centralized
   - Configuration loading abstracted
   - Tick execution composable

5. **Backward Compatible**
   - Gradual migration path
   - Old code continues working
   - Low risk implementation

### Trade-offs

1. **More Classes**
   - Additional interfaces (IEngineConfiguration, ISubscriptionHandler, IEngineObserver)
   - More files to navigate
   - **Mitigation**: Good documentation, clear naming

2. **Indirection**
   - Strategy pattern adds indirection
   - May be harder to debug
   - **Mitigation**: Clear logging, good documentation

3. **Migration Effort**
   - Need to update existing code
   - Tests need updating
   - **Mitigation**: Gradual migration, wrapper pattern

4. **Learning Curve**
   - More patterns to understand
   - More flexible = more ways to do things wrong
   - **Mitigation**: Examples, documentation, builder pattern

---

## Migration Path

### Step 1: Document Current Architecture (Week 1)

- Update Engine class documentation
- Create architecture diagrams
- Document current execution flow
- **✅ Already complete** (this document)

### Step 2: Add New Interfaces (Week 2)

Create without breaking existing code:
- IEngineConfiguration interface
- ISubscriptionHandler interface  
- IEngineObserver interface

### Step 3: Add Pipeline Methods (Week 3)

Add alongside existing virtual methods:
- OnTickBegin(), ProcessEvents(), etc.
- Default implementations that delegate to old methods
- No changes to derived classes yet

### Step 4: Implement Configuration Sources (Week 4)

Create implementations:
- FileEngineConfiguration
- InjectedEngineConfiguration
- Update Engine to use configuration source

### Step 5: Implement Subscription Handlers (Week 5)

Create implementations:
- GameEngineSubscriptionHandler
- TestEngineSubscriptionHandler
- Update Engine::ProcessSubscriptions() to use handler

### Step 6: Create TestEngineBuilder (Week 6)

Implement fluent builder:
- TestEngineBuilder class
- Example tests using builder
- Documentation

### Step 7: Migrate Existing Code (Week 7)

Update implementations:
- Migrate GameEngine to new pipeline methods
- Migrate TestEngine to new pipeline methods
- Update tests
- Mark old methods as deprecated

### Step 8: Remove Deprecated Code (Week 8)

Final cleanup:
- Remove deprecated methods
- Update documentation
- Final testing

---

## Example: Before and After

### Before: Creating a Test

```cpp
TEST_CASE("Test UI action processing") {
  // Setup is verbose and unclear
  auto test_data = LoadTestData("ui_action_test.json");
  const TestDataConfig* config = test_data.get();
  
  // What does this even do? Need to read TestEngine source
  TestEngine engine(config);
  engine.RunGame();
  
  // What data is in the bank? When was it captured?
  auto& data_bank = engine.GetDataBank();
  auto final_state = data_bank.at(10);  // Magic number
  
  // Assertions...
}
```

### After: Creating a Test

```cpp
TEST_CASE("Test UI action processing") {
  // Clear, self-documenting setup
  auto engine = TestEngineBuilder()
    .WithScene(SceneType::SceneType_TITLE)
    .WithNumTicks(10)
    .EnableDataBank()
    .Build();
  
  // Obvious what this does
  engine->RunGame();
  
  // Clear API
  auto final_state = engine->GetFinalSceneSnapshot();
  
  // Assertions...
}
```

### Before: Testing Event Handling

```cpp
TEST_CASE("Test event handler") {
  // Must run full tick, can't isolate event handling
  auto config = CreateMinimalTestConfig();
  TestEngine engine(config);
  
  // This runs EVERYTHING - event handling, subscriptions, logic, rendering
  engine.ExecuteSystemsTick();
  
  // Hard to tell what caused what
}
```

### After: Testing Event Handling

```cpp
TEST_CASE("Test event handler") {
  auto engine = TestEngineBuilder()
    .WithScene(SceneType::SceneType_TITLE)
    .Build();
  
  // Only test event handling phase
  engine->TickEventHandler();
  
  // Clear what was tested
  REQUIRE(engine->GetEventBus().size() > 0);
}
```

---

## Recommendation

**Implement the proposed improvements in phases** as outlined in the implementation strategy.

**Priority Order**:
1. **High**: Unified Tick Pipeline (Improvement 1) - Biggest clarity gain + enables focused testing
2. **High**: Hybrid TestEngine (Improvement 4) - Best ergonomics improvement without duplication
3. **Medium**: Configuration abstraction (Improvement 3) - Enables testing GameEngine
4. **Medium**: Subscription strategy (Improvement 2) - Reduces duplication
5. **Low**: Observable execution (Improvement 5) - Future-proofing and debugging

**Start with**: Unified Tick Pipeline (Improvement 1)
- Highest impact on developer experience
- Enables all other improvements
- Can be implemented without breaking changes
- Makes testing individual phases possible

**Next**: Hybrid TestEngine + Configuration abstraction (Improvements 4 & 3)
- Enables better testing ergonomics
- TestEngine works with data-driven harness AND programmatic setup
- GameEngine can be tested with mock data
- Still maintains backward compatibility

**Finally**: Subscription strategy + Observable execution (Improvements 2 & 5)
- Polish and extensibility
- Can be done incrementally
- Completes the architecture

---

## Conclusion

The current Engine architecture is functional but has room for improvement in clarity, testability, and extensibility. The proposed improvements address these concerns while maintaining backward compatibility and providing a clear migration path.

Key improvements:
- **Unified Tick Pipeline** - Consistent Tick_() nomenclature for all execution phases
- **Hybrid TestEngine** - Data-driven primary approach with programmatic convenience methods
- **Strategy patterns** - Configuration sources and subscription handling
- **Observer pattern** - Comprehensive monitoring with practical use cases

These changes will make the codebase more maintainable, easier to test, and more extensible for future requirements.

**Responses to Feedback**:
1. ✅ **Merged Proposals 1 & 4** - Unified Tick_() nomenclature for pipeline and fine-grained control
2. ✅ **Expanded Observer section** - Added 4 practical use cases (profiling, test assertions, debugging, data collection)
3. ✅ **Revised TestEngine approach** - Hybrid design: data-driven primary, programmatic convenience, no duplication with test harness

**Next Steps**:
1. Review this updated proposal with the team
2. Confirm unified Tick_() approach addresses clunkiness concerns
3. Validate hybrid TestEngine approach aligns with data-driven philosophy
4. Begin implementation with Phase 1 (Unified Tick Pipeline)
5. Iterate based on feedback

---

**Last Updated**: December 4, 2025  
**Author**: Copilot Analysis  
**Version**: 2.0 (Revised based on feedback)
