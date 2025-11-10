# Resource and Context Separation Architecture

## Before Separation

```
┌─────────────────────────────────────────────────────────┐
│                     ContextData                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │         GameContextConfig                        │  │
│  │  - window_width                                  │  │
│  │  - window_height                                 │  │
│  │  - window_title                                  │  │
│  │  - framerate_limit                               │  │
│  │  - environment_type                              │  │
│  └──────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────┐  │
│  │       SceneContextConfig[]                       │  │
│  │  - scene_type                                    │  │
│  │  - entity_pool_size                              │  │
│  │  - render_texture_width                          │  │
│  │  - render_texture_height                         │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                          │
                          │ Mixed Resource and Context Data
                          ▼
          ┌───────────────────────────────┐
          │   ResourceConfigurator        │
          │  (Used ContextData)           │
          └───────────────────────────────┘
                          │
          ┌───────────────┴────────────────┐
          ▼                                ▼
  ┌──────────────┐              ┌──────────────────┐
  │GameResources │              │ SceneResources   │
  └──────────────┘              └──────────────────┘
```

## After Separation

```
┌─────────────────────────────────────────────────────────┐
│              RESOURCE LAYER (Independent)               │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌─────────────────────────┐  ┌─────────────────────┐  │
│  │  GameResourcesData      │  │SceneResourcesData[] │  │
│  │  ─────────────────      │  │  ──────────────     │  │
│  │  - window_width         │  │  - scene_type       │  │
│  │  - window_height        │  │  - render_width     │  │
│  │  - window_title         │  │  - render_height    │  │
│  │  - framerate_limit      │  │                     │  │
│  │  - environment_type     │  │                     │  │
│  └─────────────────────────┘  └─────────────────────┘  │
│              │                            │             │
│              └────────────┬───────────────┘             │
│                           ▼                             │
│              ┌─────────────────────────┐                │
│              │ ResourceConfigurator    │                │
│              │ (Resource-specific)     │                │
│              └─────────────────────────┘                │
│                           │                             │
│              ┌────────────┴────────────┐                │
│              ▼                         ▼                │
│      ┌──────────────┐         ┌──────────────────┐     │
│      │GameResources │         │ SceneResources   │     │
│      │(Owns objects)│         │  (Owns objects)  │     │
│      └──────────────┘         └──────────────────┘     │
└─────────────────────────────────────────────────────────┘
                          │
                          │ Resources owned independently
                          ▼
┌─────────────────────────────────────────────────────────┐
│              CONTEXT LAYER (References)                 │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌─────────────────────────────────────────────┐       │
│  │           GameContext                       │       │
│  │  ┌───────────────────────────────────────┐  │       │
│  │  │  Constructor:                         │  │       │
│  │  │  GameContext(GameResources &resources)│  │       │
│  │  └───────────────────────────────────────┘  │       │
│  │                                             │       │
│  │  References:                                │       │
│  │  - game_window (ref)                        │       │
│  │  - event_handler (ref)                      │       │
│  │  - asset_manager (ref)                      │       │
│  │  - env_type (ref)                           │       │
│  └─────────────────────────────────────────────┘       │
│                                                         │
│  ┌─────────────────────────────────────────────┐       │
│  │           SceneContext                      │       │
│  │  ┌───────────────────────────────────────┐  │       │
│  │  │  Constructor:                         │  │       │
│  │  │  SceneContext(SceneResources &,       │  │       │
│  │  │               GameResources &,        │  │       │
│  │  │               EntityManager &)        │  │       │
│  │  └───────────────────────────────────────┘  │       │
│  │                                             │       │
│  │  References:                                │       │
│  │  - scene_texture (ref)                      │       │
│  │  - game_window (ref)                        │       │
│  │  - asset_manager (ref)                      │       │
│  │  - scene_entities (ref)                     │       │
│  │  - archetypes (ref)                         │       │
│  └─────────────────────────────────────────────┘       │
│                                                         │
│  ┌─────────────────────────────────────────────┐       │
│  │           ContextData (Future)              │       │
│  │  Reserved for context-specific config:     │       │
│  │  - System dependencies                      │       │
│  │  - Runtime configurations                   │       │
│  │  - Component relationships                  │       │
│  └─────────────────────────────────────────────┘       │
└─────────────────────────────────────────────────────────┘
```

## Key Differences

### Resource Layer (Bottom-Up Ownership)
1. **GameResourcesData** and **SceneResourcesData** are independent schemas
2. **ResourceConfigurator** creates actual resource objects
3. **GameResources** and **SceneResources** own the objects
4. **No cross-dependencies** between resource data structures

### Context Layer (Top-Down References)
1. **GameContext** and **SceneContext** reference owned resources
2. Contexts are lightweight (just references)
3. Contexts can be created/destroyed frequently
4. Resources outlive contexts

## Usage Patterns

### Pattern 1: Direct Resource Configuration
```cpp
// Load resource data
auto game_data = loader.ProvideGameResourcesData().value();
auto scene_data = loader.ProvideSceneResourcesData().value();

// Configure resources independently
ResourceConfigurator configurator(game_data, scene_data);
configurator.ConfigureGameResources(game_resources);
configurator.ConfigureSceneResources(scene_resources, scene_type);

// Create contexts from resources
GameContext game_ctx(game_resources);
SceneContext scene_ctx(scene_resources, game_resources, entity_manager);
```

### Pattern 2: Via ContextConfigurator (Backward Compatible)
```cpp
// Load context data (may be used for future context config)
auto context_data = loader.ProvideContextData().value();

// Configure resources via convenience wrapper
ContextConfigurator configurator(context_data);
configurator.ConfigureGameResources(game_resources);  // Loads resource data internally
configurator.ConfigureSceneResources(scene_resources, scene_type);

// Create contexts from resources
GameContext game_ctx(game_resources);
SceneContext scene_ctx(scene_resources, game_resources, entity_manager);
```

## Benefits

### 1. Clear Ownership
- Resources **own** objects (windows, textures)
- Contexts **reference** objects (lightweight)

### 2. Independent Configuration
- Resources configured from resource schemas
- Contexts configured from resource objects
- No circular dependencies

### 3. Testability
- Test resources in isolation
- Mock resources easily
- Test contexts with test resources

### 4. Flexibility
- Resources loaded from different sources
- Multiple contexts can reference same resources
- Easy to add new resource types

## File Organization

```
src/
├── resources/
│   ├── GameResources.h           # Owns game resources
│   ├── SceneResources.h          # Owns scene resources
│   └── ResourceConfigurator.h/cpp # Configures from data
│
├── context/
│   ├── GameContext.h/cpp          # References game resources
│   └── ContextConfigurator.h/cpp  # Convenience wrapper
│
├── scenes/
│   └── SceneContext.h/cpp         # References scene+game resources
│
└── flatbuffers_headers/
    ├── game_resources.fbs         # Resource schema
    ├── scene_resources.fbs        # Resource schema
    └── context_data.fbs           # Context schema (future)

data/
└── resources/
    ├── game_resources.game_resources.json  # Resource config
    └── scene_resources.scene_resources.json # Resource config
```
