# Resource Management System - Quick Reference

**Status**: 📋 PLANNING PHASE  
**Full Plan**: See [RESOURCE_MANAGEMENT_PLAN.md](RESOURCE_MANAGEMENT_PLAN.md)

## Overview

The Resource Management System provides unified, data-driven resource initialization for production and test environments, eliminating duplication and enabling flexible resource configuration.

## Key Concepts

### ResourceHandle<T>
Type-safe wrapper around shared_ptr for resource access:
```cpp
ResourceHandle<EntityMemoryPool> pool;
if (pool.IsValid()) {
  auto& entity_pool = pool.Get();
}
```

### ResourceCollection
Container for all game/scene resources:
```cpp
ResourceCollection resources;
resources.render_window.Get();  // sf::RenderWindow&
resources.entity_pool.Get();    // EntityMemoryPool&
resources.asset_manager.Get();  // AssetManager&
```

### ResourcePreset
Named configuration combining multiple resources:
```json
{
  "name": "minimal_test",
  "resources": [
    {"resource_type": "EntityMemoryPool", "entity_pool_config": {"pool_size": 10}}
  ]
}
```

### ResourceManager
Static registry for resource lifecycle:
```cpp
ResourceManager::Initialize(manifest);
auto resources = ResourceManager::BuildResourcesFromPreset("ui_test");
```

## Common Presets

### Test Presets

**minimal_test** - Unit tests needing only entity storage:
- EntityMemoryPool (size: 10)

**ui_test** - UI component tests:
- EntityMemoryPool (size: 50)
- RenderTexture (800x600)
- AssetManager (defaults loaded)

**logic_test** - Logic class tests:
- EntityMemoryPool (size: 100)
- ArchetypeManager
- RenderTexture (800x600)
- EventHandler
- AssetManager (no defaults)

**full_scene_test** - Integration tests:
- RenderWindow (640x480)
- EventHandler
- AssetManager (with TEST scene assets)
- EntityMemoryPool (size: 100)
- ArchetypeManager
- RenderTexture (640x480)

### Production Presets

**full_game** - Complete game resources:
- RenderWindow (800x600)
- EventHandler
- AssetManager
- SceneManager
- DisplayManager

**title_scene** - Title screen:
- EntityMemoryPool (size: 50)
- RenderTexture (800x600)

**crafting_scene** - Crafting screen:
- EntityMemoryPool (size: 200)
- RenderTexture (800x600)

## Usage Patterns

### In Tests

**Simple preset usage**:
```cpp
TEST_CASE("My unit test", "[unit]") {
  steamrot::tests::TestContext context("minimal_test");
  
  auto& pool = context.GetResources().Get<EntityMemoryPool>();
  // Test using pool...
}
```

**Scene-specific test**:
```cpp
TEST_CASE("Title scene test", "[integration]") {
  auto context = steamrot::tests::TestContext::ForScene(SceneType::SceneType_TITLE);
  
  auto& logic_ctx = context.GetLogicContext();
  // Test title scene logic...
}
```

**Custom resources with builder**:
```cpp
TEST_CASE("Custom test", "[unit]") {
  auto context = steamrot::tests::TestContextBuilder::WithPreset("minimal_test")
    .WithRenderTexture(640, 480)
    .WithEventHandler()
    .Build()
    .value();
  
  // Test with custom configuration...
}
```

**Check resource availability**:
```cpp
TEST_CASE("Conditional test", "[unit]") {
  steamrot::tests::TestContext context("ui_test");
  
  REQUIRE(context.HasResource<EntityMemoryPool>());
  REQUIRE(context.HasResource<sf::RenderTexture>());
  
  if (context.HasResource<AssetManager>()) {
    auto& assets = context.GetResources().Get<AssetManager>();
    // Use assets...
  }
}
```

### In Production

**GameEngine with preset**:
```cpp
// Load manifest
FlatbuffersDataLoader loader;
auto manifest = loader.ProvideResourceManifest();

// Initialize resource manager
ResourceManager::Initialize(manifest);

// Create engine from preset
auto engine = GameEngine::CreateFromPreset("full_game");
```

**Custom resource setup**:
```cpp
auto manifest = loader.ProvideResourceManifest();
ResourceBuilder builder(manifest);

auto resources = builder
  .WithRenderWindow()
  .WithEventHandler()
  .WithAssetManager()
  .Build()
  .value();

GameEngine engine(resources);
```

## Configuration Files

### Production Manifest
**Location**: `data/resources/resource_manifest.json`

```json
{
  "presets": [
    {
      "name": "full_game",
      "description": "Complete game resources",
      "resources": [
        {
          "resource_type": "RenderWindow",
          "render_window_config": {
            "width": 800,
            "height": 600,
            "title": "SteamRot",
            "framerate_limit": 60
          }
        }
      ]
    }
  ]
}
```

### Test Manifest
**Location**: `tests/data/resources/test_resource_presets.json`

```json
{
  "presets": [
    {
      "name": "minimal_test",
      "description": "Minimal resources for unit tests",
      "resources": [
        {
          "resource_type": "EntityMemoryPool",
          "entity_pool_config": {"pool_size": 10}
        }
      ]
    }
  ]
}
```

## Resource Types

| Resource Type | Production | Tests | Config Options |
|--------------|------------|-------|----------------|
| RenderWindow | Always | Rarely | width, height, title, framerate |
| EventHandler | Always | Often | None |
| AssetManager | Always | Sometimes | load_defaults, scene_assets |
| EntityMemoryPool | Scene | Always | pool_size |
| ArchetypeManager | Scene | Often | None |
| RenderTexture | Scene | Often | width, height |
| SceneManager | Always | Never | None |
| DisplayManager | Always | Never | None |

## When to Use Which Preset

### Use "minimal_test" when:
- Testing entity/component manipulation
- Testing archetype generation
- Unit testing pure logic functions
- **Don't need**: Graphics, events, assets

### Use "ui_test" when:
- Testing UI element rendering
- Testing UI layout logic
- Testing UI collision
- **Need**: RenderTexture, assets, entity pool

### Use "logic_test" when:
- Testing Logic classes (Render, Collision, Action, Movement)
- Testing archetype-based systems
- Testing event handling
- **Need**: Full Logic infrastructure

### Use "full_scene_test" when:
- Integration testing scenes
- Testing scene transitions
- End-to-end workflow tests
- **Need**: Everything except real display

### Use custom builder when:
- Testing specific resource combinations
- Benchmarking with different configurations
- Debugging resource issues
- **Need**: Fine-grained control

## Performance Guidelines

### Initialization Costs (Approximate)

- **EntityMemoryPool (100 entities)**: ~0.5ms
- **RenderTexture (800x600)**: ~2ms
- **AssetManager (defaults)**: ~50ms
- **RenderWindow**: ~100ms
- **Full GameEngine**: ~200ms

### Optimization Tips

1. **Use smallest preset possible**: Start with "minimal_test", add only what's needed
2. **Avoid RenderWindow in tests**: Unless testing actual display
3. **Share resources**: Use ResourceManager caching for test suites
4. **Lazy asset loading**: Set `load_default_assets: false` if not needed
5. **Small entity pools**: Use 10-50 for unit tests, 100+ for integration

## Migration Guide

### Old TestContext
```cpp
// Old way - always creates everything
TEST_CASE("Old test", "[unit]") {
  steamrot::tests::TestContext ctx;
  auto& pool = ctx.scene_entities;
  // ...
}
```

### New TestContext
```cpp
// New way - only creates what's needed
TEST_CASE("New test", "[unit]") {
  steamrot::tests::TestContext ctx("minimal_test");
  auto& pool = ctx.GetResources().Get<EntityMemoryPool>();
  // ...
}
```

### Benefits of Migration

1. **~10x faster** for unit tests (10ms → 1ms setup)
2. **Clearer intent**: Preset name documents requirements
3. **More flexible**: Easy to add/remove resources
4. **Less memory**: Don't create unused resources
5. **Maintainable**: Shared configuration across tests

## Common Patterns

### Testing Component Behavior
```cpp
TEST_CASE("Component test", "[unit][component]") {
  TestContext ctx("minimal_test");  // Just entity pool
  auto& pool = ctx.GetResources().Get<EntityMemoryPool>();
  
  // Test component in isolation
}
```

### Testing Logic Classes
```cpp
TEST_CASE("Logic test", "[unit][logic]") {
  TestContext ctx("logic_test");  // Full logic infrastructure
  auto& logic_ctx = ctx.GetLogicContext();
  
  MyLogic logic(logic_ctx);
  logic.RunLogic();
  // Verify behavior
}
```

### Testing UI Rendering
```cpp
TEST_CASE("UI test", "[unit][ui]") {
  TestContext ctx("ui_test");  // UI resources
  auto& texture = ctx.GetResources().Get<sf::RenderTexture>();
  auto& assets = ctx.GetResources().Get<AssetManager>();
  
  // Render and verify
}
```

### Testing Scene Integration
```cpp
TEST_CASE("Scene test", "[integration][scene]") {
  auto ctx = TestContext::ForScene(SceneType::SceneType_TITLE);
  auto& logic_ctx = ctx.GetLogicContext();
  
  // Full scene workflow test
}
```

## Troubleshooting

### Resource Not Found

**Error**: `Resource not initialized`

**Cause**: Trying to access resource not in preset

**Solution**: Either add resource to preset or use builder:
```cpp
// Add to preset
auto ctx = TestContextBuilder::WithPreset("minimal_test")
  .WithRenderTexture()
  .Build();

// Or use preset that has it
TestContext ctx("ui_test");  // Has RenderTexture
```

### Configuration File Not Found

**Error**: `Failed to load resource manifest`

**Cause**: Binary file not generated or wrong path

**Solution**: Rebuild project to generate .bin files:
```bash
cmake --build --preset Debug
```

### Preset Not Found

**Error**: `Preset 'my_preset' not found in manifest`

**Cause**: Typo in preset name or not defined in JSON

**Solution**: Check preset name spelling or add to JSON:
```json
{
  "presets": [
    {
      "name": "my_preset",
      "resources": [...]
    }
  ]
}
```

## Implementation Status

### Completed (Stage 0)
- ✅ Design plan created
- ✅ Documentation written
- ✅ Configuration examples defined

### Planned

**Stage 1** (Week 1):
- [ ] FlatBuffers schema
- [ ] ResourceHandle template
- [ ] ResourceCollection struct

**Stage 2** (Week 2):
- [ ] ResourceBuilder class
- [ ] Preset-based building

**Stage 3** (Week 3):
- [ ] ResourceManager static registry
- [ ] FlatbuffersDataLoader integration

**Stage 4** (Week 4):
- [ ] GameEngine integration
- [ ] Factory methods

**Stage 5** (Week 5):
- [ ] TestContext modernization
- [ ] TestContextBuilder

**Stage 6-7** (Week 6-7):
- [ ] Full migration
- [ ] Cleanup and optimization

**Stage 8** (Week 8):
- [ ] Documentation polish
- [ ] Migration guides

## Related Documentation

- **Full Design**: [RESOURCE_MANAGEMENT_PLAN.md](RESOURCE_MANAGEMENT_PLAN.md)
- **Context System**: [CONTEXT_HANDLING_IMPROVEMENT_PLAN.md](CONTEXT_HANDLING_IMPROVEMENT_PLAN.md)
- **Testing**: [TESTING_IMPROVEMENT_PLAN.md](TESTING_IMPROVEMENT_PLAN.md)
- **Context Config**: [CONTEXT_CONFIGURATION.md](CONTEXT_CONFIGURATION.md)

## Quick Decision Tree

```
Do I need to test...
├─ Just entity/component logic?
│  └─ Use "minimal_test" ✓
├─ UI rendering?
│  └─ Use "ui_test" ✓
├─ Logic class (Render/Collision/Action/Movement)?
│  └─ Use "logic_test" ✓
├─ Full scene workflow?
│  └─ Use "full_scene_test" ✓
├─ Something custom?
│  └─ Use TestContextBuilder ✓
└─ Production code?
   └─ Use ResourceManager::CreateFromPreset() ✓
```

## Key Takeaways

1. **Use presets for 90% of cases** - They cover common scenarios
2. **Start minimal, add as needed** - Don't over-initialize
3. **Test performance improves** - Only create what you use
4. **Configuration is data-driven** - Change JSON, not C++
5. **Production and tests share config** - Single source of truth

---

**Next Steps**: Review plan → Approval → Begin Stage 1 implementation
