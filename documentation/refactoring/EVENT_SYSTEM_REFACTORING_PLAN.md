# Event System Refactoring Implementation Plan

**Document Version:** 1.0  
**Date:** 2026-02-15  
**Author:** GitHub Copilot Code Analysis  
**Status:** Draft for Review

---

## Executive Summary

This document provides a comprehensive implementation plan for refactoring the SteamRot event system to improve type safety, performance, and maintainability.

### Goals

1. **Restructure Event Data**: Hierarchical category-based structure
   - EventContext: Shared fields (UUIDs, entity IDs, names)
   - EventPayload: Category variants (Input, UI, Scene, Logic, System)

2. **Restructure Subscriber**: Separate configuration from state
   - EventFilter: What to match (configuration)
   - CapturedEvent: What was received (data)
   - Subscriber: Filter + is_active + captured

3. **Optimize EventHandler**: Hybrid multi-index approach
   - Fast path: O(1) lookups for common patterns
   - Slow path: Runtime filtering for complex cases

### Impact

- **Files to Modify:** 40-50 files
- **Timeline:** 3-4 weeks
- **Risk Level:** Medium-High (pervasive changes)
- **Benefits:** Type safety, performance, maintainability, extensibility

---

## 1. Current State Analysis

### 1.1 Discovered Components

**Core Types** (src/types/events/):
- `EventPacket`: Contains EventType, EventData variant, UUIDs, lifetime
- `EventType`: Enum with 7 values (USER_INPUT, TOGGLE_UI, CHANGE_SCENE, QUIT_GAME, TOGGLE_DROPDOWN, LOGIC_TOGGLE, TEST)
- `EventData`: Variant of std::monostate, UserInputBitset, SceneChangePacket, UserInterfaceName, ToggleName
- `Subscriber`: Has trigger type, trigger data, m_active flag, received data

**EventHandler** (src/events/):
- Single index: `unordered_map<EventType, vector<weak_ptr<Subscriber>>>`
- Registration-time filtering (by type only)
- Runtime data matching in UpdateSubscriber()
- Frame execution: Reset → Preload → Process → Update → Tick

### 1.2 Event Type Mapping

| EventType | Current Data | Use Case | Frequency |
|-----------|--------------|----------|-----------|
| USER_INPUT | UserInputBitset | Input handling | High |
| TOGGLE_UI | UserInterfaceName | UI show/hide | Medium |
| TOGGLE_DROPDOWN | UserInterfaceName | Dropdowns | Low |
| CHANGE_SCENE | SceneChangePacket | Scene transitions | Low |
| QUIT_GAME | std::monostate | Exit | Very Low |
| LOGIC_TOGGLE | ToggleName | Logic states | Medium |

### 1.3 Subscriber Patterns (Analyzed ~200 sites)

1. **Type-Only** (40%): Match EventType only
   - Example: `Subscriber(EventType::QUIT_GAME)`
   - Fast path candidate

2. **Type + Exact Data** (35%): Match type and specific value
   - Example: `Subscriber(EventType::TOGGLE_UI, UserInterfaceName("menu"))`
   - Fast path candidate if name extracted

3. **Type + Bitset Subset** (20%): Match type and input pattern
   - Example: Input combinations
   - Special matching: `(trigger & event) == trigger`

4. **Complex Multi-Filter** (5%): Multiple conditions
   - Currently in Logic classes
   - Slow path candidate

### 1.4 Key Usage Sites

**Event Creation**:
- EventHandler.cpp: SFML → USER_INPUT events
- action_ui.cpp: Button clicks → events
- Logic classes: State changes → events
- ~200 total event creation sites

**Subscriber Consumption**:
- Logic classes: Check m_active, process received data
- UIElement: Has subscription field
- GameEngine: Engine-level subscriptions (QUIT, scene changes)

---

## 2. Proposed Architecture

### 2.1 Event Data Hierarchy

#### EventContext (Shared Fields)
```cpp
// src/types/events/EventContext.h
struct EventContext {
    uuids::uuid event_id;                // Auto-generated
    uuids::uuid source_id;               // Source entity/system
    std::optional<size_t> entity_index;  // Optional entity reference
    std::optional<std::string> name;     // Optional name (UI, toggle)
    uint8_t event_lifetime{1};           // Tick-based TTL
};
```

#### EventPayload (Category-Specific Data)
```cpp
// src/types/events/EventPayload.h

struct InputPayload {
    UserInputBitset input_bitset;
};

struct UIPayload {
    std::string ui_element_name;
    enum class UIAction { TOGGLE, CLICK, HOVER, DRAG } action;
    std::optional<sf::Vector2f> position;
};

struct ScenePayload {
    SceneType target_scene;
    std::optional<uuids::uuid> save_uuid;
    enum class TransitionType { IMMEDIATE, FADE, SLIDE } transition;
};

struct LogicPayload {
    std::string toggle_name;
    std::optional<std::variant<int, float, std::string>> value;
};

struct SystemPayload {
    enum class SystemCommand { QUIT, PAUSE, RESUME, SAVE, LOAD } command;
    std::optional<std::string> parameter;
};

using EventPayload = std::variant<
    std::monostate, InputPayload, UIPayload, 
    ScenePayload, LogicPayload, SystemPayload
>;
```

#### New EventPacket
```cpp
struct EventPacket {
    EventType event_type;     // Primary classifier
    EventContext context;     // Shared metadata
    EventPayload payload;     // Category-specific data
    
    // Static factories
    static EventPacket CreateInput(const InputPayload&, uuids::uuid source);
    static EventPacket CreateUI(const UIPayload&, uuids::uuid source);
    // ... etc for each category
};
```

### 2.2 Subscriber Restructuring

#### EventFilter (Configuration)
```cpp
// src/types/events/EventFilter.h
struct EventFilter {
    EventType event_type;                    // Required
    std::optional<uuids::uuid> source_filter;
    std::optional<size_t> entity_filter;
    std::optional<std::string> name_filter;
    std::optional<UserInputBitset> input_bitset_filter;
    
    using PredicateFn = std::function<bool(const EventPacket&)>;
    std::optional<PredicateFn> custom_predicate;
    
    FilterComplexity GetComplexity() const;
};

enum class FilterComplexity {
    TYPE_ONLY, TYPE_NAME, TYPE_ENTITY, TYPE_PAYLOAD, CUSTOM
};
```

#### CapturedEvent (Received Data)
```cpp
struct CapturedEvent {
    EventPacket event;
    uint64_t tick_captured;
    
    template<typename T>
    const T* TryGetPayload() const {
        return std::get_if<T>(&event.payload);
    }
};
```

#### New Subscriber
```cpp
struct Subscriber {
    EventFilter filter;                     // What to match (config)
    bool is_active{false};                  // Activated this frame? (state)
    std::optional<CapturedEvent> captured;  // Event received (data)
    
    explicit Subscriber(EventFilter filter);
    
    // Factory methods
    static Subscriber TypeOnly(EventType type);
    static Subscriber TypeAndName(EventType type, std::string name);
    static Subscriber InputPattern(UserInputBitset pattern);
    
    void Reset();
    void Activate(const EventPacket& event, uint64_t tick);
};
```

### 2.3 EventHandler Multi-Index

```cpp
class EventHandler {
private:
    EventBus m_global_event_bus;
    EventBus m_waiting_room_event_bus;
    
    // Multi-dimensional indices
    std::unordered_map<EventType, 
        std::vector<std::weak_ptr<Subscriber>>> m_type_index;
    
    std::unordered_map<TypeNameKey, 
        std::vector<std::weak_ptr<Subscriber>>> m_type_name_index;
    
    std::unordered_map<TypeEntityKey, 
        std::vector<std::weak_ptr<Subscriber>>> m_type_entity_index;
    
    std::unordered_map<EventType, 
        std::vector<std::weak_ptr<Subscriber>>> m_complex_index;
    
    void RouteSubscriber(std::weak_ptr<Subscriber> sub);
    FilterComplexity ClassifySubscriber(const Subscriber& sub) const;
    
public:
    void RegisterSubscriber(std::shared_ptr<Subscriber> subscriber);
    void UpdateSubscribersFromGlobalEventBus();
};
```

**Routing Logic**:
- TYPE_ONLY → m_type_index (40% of subscribers)
- TYPE_NAME → m_type_name_index (35% of subscribers)
- TYPE_ENTITY → m_type_entity_index (rare but useful)
- CUSTOM/TYPE_PAYLOAD → m_complex_index (5% of subscribers)

**Performance**:
- Fast paths: O(1) map lookup + O(n_matching) iteration
- Slow path: O(1) map lookup + O(n_complex) * O(filter_cost)
- Memory overhead: ~20% (subscribers indexed multiple times if complex)

---

## 3. Migration Strategy

### 3.1 Breaking Changes

**High Impact**:
- EventPacket structure (all ~200 event creation sites)
- EventData variant (all variant access patterns)
- Subscriber structure (all subscriber creation/consumption)

**Medium Impact**:
- EventHandler internals (mostly internal)
- FlatBuffers schemas (data migration needed)

**Low Impact**:
- Factory functions (new additions)
- Test infrastructure

### 3.2 Compatibility Approach

**Recommendation**: No compatibility layer (atomic migration)

**Rationale**:
- Event system is internal (no public API)
- Compatibility layer adds complexity
- Atomic change easier to review/test
- Testing infrastructure exists

**Alternative**: Adapter pattern (more conservative but slower)

### 3.3 Migration Order

1. **Phase 1: Foundations** (Week 1) - Add new types, no breaking changes
2. **Phase 2: EventHandler** (Week 1-2) - Implement multi-index
3. **Phase 3: Event Creation** (Week 2-3) - Migrate creation sites
4. **Phase 4: Subscriber Creation** (Week 3) - Migrate subscriber sites
5. **Phase 5: Cleanup** (Week 4) - Remove old code, final testing

---

## 4. Implementation Plan

### Phase 1: Create New Data Structures (3-4 days)

**Goal**: Add new types without breaking existing code

**Tasks**:
1. Create EventContext.h + tests
2. Create EventPayload.h (all category structs) + tests
3. Create EventFilter.h + CapturedEvent.h + tests
4. Create event_factory_helpers.h/cpp (factory functions) + tests

**Files** (all new):
- `src/types/events/EventContext.h`
- `src/types/events/EventPayload.h`
- `src/types/events/EventFilter.h`
- `src/types/events/CapturedEvent.h`
- `src/events/event_factory_helpers.h/cpp`
- `tests/unit/events/EventContext.test.cpp`
- `tests/unit/events/EventPayload.test.cpp`
- `tests/unit/events/EventFilter.test.cpp`

**Success Criteria**:
- All new files compile
- All unit tests pass
- Zero impact on existing code

**Risks**:
- Incomplete category coverage → Start with 7 current types
- Factory API design → Review before implementing

---

### Phase 2: Implement EventHandler Multi-Index (4-5 days)

**Goal**: Add multi-index system while supporting old subscribers

**Tasks**:
1. Add index structures (TypeNameKey, TypeEntityKey, hashes)
2. Implement ClassifySubscriber() and RouteSubscriber()
3. Update RegisterSubscriber() with routing logic
4. Update UpdateSubscribersFromGlobalEventBus() with multi-index matching
5. Add backward compatibility bridge
6. Performance benchmarking

**Files** (modify):
- `src/events/EventHandler.h`
- `src/events/EventHandler.cpp`
- `tests/unit/events/EventHandler.test.cpp`
- `tests/perf/EventHandler.perf.test.cpp` (new)

**Success Criteria**:
- Subscribers route to correct index
- Multi-index matching works
- Performance ≥ baseline for simple cases
- Old and new subscribers coexist

**Risks**:
- Performance regression → Benchmark early
- Hash collisions → Use std::hash composition
- Complex filter bugs → Extensive testing

---

### Phase 3: Migrate Event Creation (5-6 days)

**Goal**: Update all event creation sites to use new EventPacket

**Tasks**:
1. Update EventPacket.h with new structure + static factories
2. Migrate HandleSFMLEvents() to use CreateUserInputEvent()
3. Migrate action_ui.cpp button events
4. Migrate all Logic class event creation (~15 files)
5. Migrate entity/scene event creation
6. Update event_factory.cpp for FlatBuffers conversion

**Files** (~25 files):
- `src/types/events/EventPacket.h` (modify)
- `src/events/EventHandler.cpp` (modify HandleSFMLEvents)
- `src/logic/action_ui.cpp` (modify)
- `src/logic/*ActionLogic.cpp` (~15 files, modify)
- `src/entity/FlatbuffersEntityConfigurator.cpp` (modify)
- `src/scenes/SceneManager.cpp` (modify)
- `src/events/event_factory.cpp` (modify)
- All event creation tests (modify)

**Success Criteria**:
- All events use new structure or factories
- Integration tests pass
- FlatBuffers conversion works

**Risks**:
- Missed creation sites → Grep for "EventPacket("
- Data mapping errors → Test each event type
- Integration failures → Test early

---

### Phase 4: Migrate Subscriber Creation (4-5 days)

**Goal**: Update all subscriber creation/consumption to new API

**Tasks**:
1. Update Subscriber.h with new structure + factories
2. Migrate Logic class subscribers (~15 files)
3. Migrate UIElement subscriptions
4. Migrate engine-level subscribers
5. Update subscriber_factory.cpp for FlatBuffers

**Files** (~20 files):
- `src/types/events/Subscriber.h` (modify)
- `src/logic/*.cpp` (~15 files, modify)
- `src/types/user_interface/UIElement.h` (modify)
- `src/engine/GameEngine.cpp` (modify)
- `src/scenes/SceneManager.cpp` (modify)
- `src/events/subscriber_factory.cpp` (modify)
- All subscriber tests (modify)

**Success Criteria**:
- All subscribers use new API
- All consumption uses is_active/captured
- FlatBuffers conversion works

**Risks**:
- Missed access sites → Grep for "m_active", "m_trigger_event_data"
- Filter semantic changes → Preserve exact matching
- UI bugs → Test all interactions

---

### Phase 5: Remove Old Structures and Cleanup (3-4 days)

**Goal**: Complete migration, remove deprecated code

**Tasks**:
1. Remove old EventData variant types
2. Remove backward compatibility code from EventHandler
3. Update FlatBuffers schemas + regenerate
4. Update documentation (README, workflow guides)
5. Final testing pass (full suite, performance, memory)

**Files** (~10 files):
- `src/types/events/EventPacket.h` (remove old EventData)
- `src/events/EventHandler.cpp` (remove compat code)
- `src/types/flatbuffers/events/*.fbs` (update schemas)
- `documentation/README.md` (update)
- `documentation/EVENT_SYSTEM.md` (create/update)

**Success Criteria**:
- All old code removed
- All tests pass
- Documentation updated
- Performance acceptable
- No memory leaks

**Risks**:
- Lingering references → Comprehensive search
- Schema migration issues → Version properly
- Documentation drift → Update inline with code

---

## 5. Risk Assessment

### Technical Risks

| Risk | Severity | Probability | Mitigation |
|------|----------|-------------|------------|
| Performance Regression | Medium | Low | Benchmark in Phase 2, optimize if needed |
| Memory Overhead | Low | Medium | Profile memory, limit indices if >20% increase |
| Integration Failures | High | Medium | Incremental testing, maintain compat in Phase 2 |
| FlatBuffers Migration | Medium | Low | Version schemas, test with existing data |

### Project Risks

| Risk | Severity | Probability | Mitigation |
|------|----------|-------------|------------|
| Scope Creep | Medium | Medium | Strict scope control, defer non-essentials |
| Estimation Errors | Low | High | 20% buffer per phase, track actual time |
| Test Coverage Gaps | High | Low | >80% coverage goal, write tests first |

### Contingency Plans

- **Performance regression >10%**: Revert to single-index optimization
- **Memory increase >20%**: Remove less-used indices
- **Critical integration failure**: Roll back to previous phase
- **Timeline slip >1 week**: Cut non-critical features (e.g., type+entity index)

---

## 6. Testing Strategy

### Unit Testing

**Coverage Goal**: >80% for new code

**Phase 1 Tests**:
- EventContext: Construction, UUID generation, optional fields
- EventPayload: All category types, variant access
- EventFilter: Complexity classification, matching logic

**Phase 2 Tests**:
- Subscriber routing to correct indices
- Multi-index event matching
- Complex filter execution

**Phase 3-5 Tests**:
- Factory functions produce correct events
- Subscriber activation semantics
- End-to-end event flow

### Integration Testing

**Scenarios**:
1. Input Flow: SFML → EventPacket → Subscriber → Logic
2. UI Flow: Button → Event → State change
3. Scene Flow: Event → Scene transition
4. Logic Flow: Toggle → Multiple subscribers

### Performance Testing

**Benchmarks**:
- Type-only matching: within 5% of baseline
- Type+Name matching: >50% faster than current filtered approach
- Memory: <20% increase

**Tools**: Google Benchmark, custom timing instrumentation

### System Testing

**Manual Checklist**:
- [ ] Game launches without errors
- [ ] All scenes load correctly
- [ ] Input handling works (keyboard, mouse)
- [ ] UI interactions work (buttons, dropdowns)
- [ ] Scene transitions work
- [ ] No memory leaks (5+ minute run)
- [ ] Performance acceptable (60 FPS)

---

## 7. Observability and Debugging

### Debugging Features

**EventInspector** (to be added):
```cpp
class EventInspector {
public:
    static void LogEventBus(const EventBus& bus);
    static void LogSubscriberRegister(const EventHandler& handler);
    static std::string ToString(const EventPacket& event);
    static std::string ToString(const Subscriber& sub);
    static std::map<FilterComplexity, size_t> 
        GetSubscriberDistribution(const EventHandler& handler);
};
```

**Metrics Collection**:
- Events processed per frame
- Subscribers activated per event type
- Index usage statistics (fast path vs slow path hits)
- Performance timing per update cycle

**Logging Integration**:
- Use spdlog for debug/trace output
- Configurable log levels
- Event/subscriber pretty-printing

---

## 8. Future Extensibility

### Adding New Event Categories

**Process**:
1. Define payload struct in EventPayload.h
2. Add to EventPayload variant
3. Create factory function
4. Update FlatBuffers schema (optional)
5. Add tests

**Example**: Animation events
```cpp
struct AnimationPayload {
    std::string animation_name;
    size_t entity_index;
    enum class AnimAction { START, STOP, PAUSE } action;
};
```

### Adding New Filter Types

**Process**:
1. Add filter field to EventFilter
2. Update GetComplexity() if creating new fast path
3. Update MatchesFilter() for matching logic
4. Add new index if common pattern
5. Add tests

### Custom Predicate Examples

**Complex collision**:
```cpp
auto filter = EventFilter{
    .event_type = EventType::UI_COLLISION,
    .custom_predicate = [](const EventPacket& e) {
        auto* ui = std::get_if<UIPayload>(&e.payload);
        return ui && ui->position->x > 100 && ui->position->y < 500;
    }
};
```

---

## 9. Recommendations

### Pre-Migration

- [ ] Review and approve plan
- [ ] Ensure >70% test coverage on current system
- [ ] Create feature branch
- [ ] Set up CI/CD
- [ ] Back up data files

### During Migration

- [ ] Commit after each phase
- [ ] Run tests after each commit
- [ ] Track time vs estimates
- [ ] Weekly progress reviews

### Post-Migration

- [ ] Performance benchmarking report
- [ ] Updated architecture documentation
- [ ] Developer guide with examples
- [ ] Retrospective meeting

### Success Metrics

**Technical**:
- All tests pass (100%)
- Coverage >80%
- Performance <5% regression
- No memory leaks
- Zero critical bugs in first week

**Project**:
- Complete within 4 weeks
- <10% scope creep
- Documentation updated
- Stakeholder approval

---

## 10. Appendices

### Appendix A: EventType to Payload Mapping

| EventType | Payload Category | Migration Notes |
|-----------|-----------------|-----------------|
| USER_INPUT | InputPayload | UserInputBitset → input_bitset |
| TOGGLE_UI | UIPayload | UserInterfaceName → ui_element_name, action=TOGGLE |
| TOGGLE_DROPDOWN | UIPayload | UserInterfaceName → ui_element_name, action=TOGGLE |
| CHANGE_SCENE | ScenePayload | SceneChangePacket → target_scene + save_uuid |
| QUIT_GAME | SystemPayload | std::monostate → command=QUIT |
| LOGIC_TOGGLE | LogicPayload | ToggleName → toggle_name |

### Appendix B: Filter Complexity Reference

| Complexity | Index Used | Performance |
|------------|------------|-------------|
| TYPE_ONLY | m_type_index | O(1) + O(n_subs) |
| TYPE_NAME | m_type_name_index | O(1) + O(n_matching) |
| TYPE_ENTITY | m_type_entity_index | O(1) + O(n_matching) |
| TYPE_PAYLOAD | m_complex_index | O(1) + O(n) * O(filter) |
| CUSTOM | m_complex_index | O(1) + O(n) * O(predicate) |

### Appendix C: Key Files Reference

**Core System**:
- `src/types/events/EventPacket.h`
- `src/types/events/EventType.h`
- `src/types/events/Subscriber.h`
- `src/events/EventHandler.h/cpp`

**Factories**:
- `src/events/event_factory.h/cpp`
- `src/events/subscriber_factory.h/cpp`

**Usage Sites**:
- `src/logic/*.cpp` (15 files)
- `src/events/EventHandler.cpp` (SFML)
- `src/entity/FlatbuffersEntityConfigurator.cpp`
- `src/scenes/SceneManager.cpp`
- `src/engine/GameEngine.cpp`

**Tests**:
- `tests/unit/events/` (unit tests)
- `tests/integration/` (integration tests)
- `tests/perf/` (benchmarks)

---

## Document Change Log

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-02-15 | Initial comprehensive plan created |

---

**End of Plan Document**

*This is a PLAN document only. No code changes have been made. Implementation to be performed based on this plan.*
