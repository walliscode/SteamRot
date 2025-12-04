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

### Improvement 1: Execution Pipeline Pattern

**Proposal**: Replace ad-hoc virtual methods with a clear pipeline pattern.

**New Structure**:
```cpp
class Engine {
protected:
  // Pipeline stages (virtual for customization)
  virtual void OnTickBegin() {}
  virtual void ProcessEvents() { /* default implementation */ }
  virtual void ProcessEngineLogic() { /* default implementation */ }
  virtual void ProcessSceneManagerLogic() { /* default implementation */ }
  virtual void ProcessSceneLogic() = 0;  // Must implement
  virtual void ProcessRendering() {}
  virtual void OnTickEnd() {}
  
  // Final non-virtual pipeline executor
  void ExecuteTick() {
    OnTickBegin();
    ProcessEvents();
    ProcessEngineLogic();
    ProcessSceneManagerLogic();
    ProcessSceneLogic();
    ProcessRendering();
    OnTickEnd();
  }
};
```

**Benefits**:
- Clear phase names
- Easy to override specific phases
- Can add pre/post hooks
- Self-documenting execution order

**GameEngine**:
```cpp
void ProcessSceneLogic() override {
  m_scene_manager.UpdateScenes();
}

void ProcessRendering() override {
  m_display_manager.CallRenderCycle();
}
```

**TestEngine**:
```cpp
void ProcessSceneLogic() override {
  simulation_runner::RunSimulationStep(m_simulation_data, GetSceneContext());
}

void ProcessRendering() override {
  // No rendering in tests
}

void OnTickEnd() override {
  Engine::OnTickEnd();
  AddToDataBank(m_current_tick);
}
```

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
  
  void ProcessSubscriptions() {  // No longer virtual
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
- Eliminates code duplication
- Easy to add new subscription types
- Can swap handlers for testing
- Separation of concerns

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

### Improvement 4: Fine-Grained Tick Control

**Proposal**: Break ExecuteTick() into smaller, composable units.

**New Structure**:
```cpp
class Engine {
protected:
  // Smallest units of execution
  void TickEventHandler();
  void TickEngineSubscriptions();
  void TickSceneManagerSubscriptions();
  void TickSceneLogic();
  void TickRendering();
  
  // Composed execution (can be overridden)
  virtual void ExecuteTick() {
    TickEventHandler();
    TickEngineSubscriptions();
    TickSceneManagerSubscriptions();
    TickSceneLogic();
    TickRendering();
  }
};
```

**Benefits**:
- Test individual components in isolation
- Easy to compose custom tick sequences
- Clear dependencies between phases
- No need for TickLevel enum

**Test Usage**:
```cpp
TEST_CASE("Event handler processes SFML events") {
  TestEngine engine(config);
  engine.StartUp();
  
  // Only test event handling
  engine.TickEventHandler();
  
  // Verify event bus state
  REQUIRE(engine.GetGameCore().event_handler.GetGlobalEventBus().size() > 0);
}
```

### Improvement 5: Builder Pattern for TestEngine

**Proposal**: Provide a fluent builder API for TestEngine configuration.

**New API**:
```cpp
class TestEngineBuilder {
public:
  TestEngineBuilder& WithConfig(const TestDataConfig* config) { /* ... */ return *this; }
  TestEngineBuilder& WithScene(SceneType type) { /* ... */ return *this; }
  TestEngineBuilder& WithNumTicks(size_t ticks) { /* ... */ return *this; }
  TestEngineBuilder& WithSimulation(const SimulationData* data) { /* ... */ return *this; }
  TestEngineBuilder& EnableDataBank() { /* ... */ return *this; }
  TestEngineBuilder& SetTickLevel(TickLevel level) { /* ... */ return *this; }
  
  std::expected<TestEngine, FailInfo> Build();
};

// Usage
TEST_CASE("Test scene logic") {
  auto engine = TestEngineBuilder()
    .WithScene(SceneType::SceneType_TITLE)
    .WithNumTicks(10)
    .EnableDataBank()
    .Build();
    
  engine->RunGame();
  
  // Assertions...
}
```

**Benefits**:
- Progressive disclosure (only specify what you need)
- Clear API for common test scenarios
- Validation at build time
- Self-documenting test setup

### Improvement 6: Observable Execution

**Proposal**: Add hooks for observing execution without modifying base class.

**New Interface**:
```cpp
class IEngineObserver {
public:
  virtual void OnTickBegin(size_t tick_number) {}
  virtual void OnPhaseComplete(const std::string& phase_name) {}
  virtual void OnTickEnd(size_t tick_number) {}
  virtual void OnError(const FailInfo& error) {}
};

class Engine {
protected:
  std::vector<std::weak_ptr<IEngineObserver>> m_observers;
  
  void NotifyObservers(auto&& notifier) {
    for (auto& weak_obs : m_observers) {
      if (auto obs = weak_obs.lock()) {
        notifier(obs);
      }
    }
  }
  
public:
  void AddObserver(std::shared_ptr<IEngineObserver> observer) {
    m_observers.push_back(observer);
  }
};
```

**Benefits**:
- TestEngine can observe without storing data
- Can add logging, profiling, debugging observers
- No performance impact if no observers
- Open/Closed Principle

---

## Implementation Strategy

### Phase 1: Non-Breaking Additions

**Add new methods alongside old ones:**
```cpp
class Engine {
protected:
  // Old methods (deprecated but working)
  virtual void ExecuteSceneLevelLogic() = 0;  // [[deprecated]]
  virtual void ExecuteDisplayManagerTick() = 0;  // [[deprecated]]
  
  // New pipeline methods
  virtual void OnTickBegin() {}
  virtual void ProcessEvents() { /* ... */ }
  virtual void ProcessSceneLogic() { ExecuteSceneLevelLogic(); }  // Delegates to old
  virtual void ProcessRendering() { ExecuteDisplayManagerTick(); }  // Delegates to old
};
```

**Timeline**: 1 week
**Risk**: Low - No breaking changes

### Phase 2: Configuration Abstraction

**Introduce IEngineConfiguration:**
```cpp
class Engine {
protected:
  std::unique_ptr<IEngineConfiguration> m_config_source;
  
  // Old method (deprecated but calls new method)
  std::expected<std::monostate, FailInfo> ConfigureEngineStateFromData() {
    return m_config_source->GetEngineData().and_then([this](auto data) {
      // Use data...
    });
  }
};
```

**Timeline**: 2 weeks
**Risk**: Low - Wrapper pattern maintains compatibility

### Phase 3: Subscription Strategy

**Extract subscription handling:**
- Create ISubscriptionHandler interface
- Implement GameEngineSubscriptionHandler
- Implement TestEngineSubscriptionHandler
- Update ProcessSubscriptions() to use handler

**Timeline**: 1 week
**Risk**: Medium - Changes subscription flow

### Phase 4: TestEngine Builder

**Add builder without modifying TestEngine:**
```cpp
class TestEngineBuilder {
  // Builds TestDataConfig internally
  // Calls TestEngine(config) constructor
};
```

**Timeline**: 1 week
**Risk**: Low - Additive only

### Phase 5: Deprecation and Migration

**Mark old methods as deprecated:**
- Add deprecation warnings
- Update documentation
- Migrate GameEngine to new methods
- Migrate TestEngine to new methods
- Update all tests

**Timeline**: 2 weeks
**Risk**: Medium - Requires coordinated changes

### Phase 6: Removal

**Remove deprecated methods:**
- Only after all code migrated
- Verify no external dependencies

**Timeline**: 1 week
**Risk**: Low - Everything already migrated

**Total Timeline**: 8 weeks
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
1. **High**: Pipeline pattern (Improvement 1) - Biggest clarity gain
2. **High**: TestEngine builder (Improvement 5) - Best ergonomics improvement
3. **Medium**: Configuration abstraction (Improvement 3) - Enables testing
4. **Medium**: Fine-grained tick control (Improvement 4) - Enables focused tests
5. **Low**: Subscription strategy (Improvement 2) - Nice to have
6. **Low**: Observable execution (Improvement 6) - Future-proofing

**Start with**: Pipeline pattern + TestEngine builder (Improvements 1 & 5)
- Highest impact on developer experience
- Can be implemented without breaking changes
- Provides foundation for other improvements

**Next**: Configuration abstraction + Fine-grained tick control (Improvements 3 & 4)
- Enables better testing
- Builds on pipeline pattern
- Still maintains backward compatibility

**Finally**: Subscription strategy + Observable execution (Improvements 2 & 6)
- Polish and extensibility
- Can be done incrementally
- Completes the architecture

---

## Conclusion

The current Engine architecture is functional but has room for improvement in clarity, testability, and extensibility. The proposed improvements address these concerns while maintaining backward compatibility and providing a clear migration path.

Key improvements:
- **Pipeline pattern** for clear execution phases
- **Builder pattern** for easier test setup
- **Strategy pattern** for configuration and subscriptions
- **Observer pattern** for monitoring execution

These changes will make the codebase more maintainable, easier to test, and more extensible for future requirements.

**Next Steps**:
1. Review this proposal with the team
2. Prioritize improvements based on current needs
3. Begin implementation with Phase 1 (non-breaking additions)
4. Iterate based on feedback

---

**Last Updated**: December 4, 2025  
**Author**: Copilot Analysis  
**Version**: 1.0
