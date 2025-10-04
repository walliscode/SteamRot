# File Naming System Visual Guide

## File Type Decision Tree

```
┌─────────────────────────────────────────────────────────┐
│         What are you creating?                          │
└───────────────────┬─────────────────────────────────────┘
                    │
        ┌───────────┴───────────┐
        │                       │
    ┌───▼───┐              ┌────▼────┐
    │ Class │              │Functions│
    └───┬───┘              └────┬────┘
        │                       │
        │                       │
┌───────▼────────┐      ┌───────▼────────────┐
│  ClassName.h   │      │ What language      │
│  ClassName.cpp │      │ features?          │
│                │      └───────┬────────────┘
│ namespace:     │              │
│  steamrot      │      ┌───────┴────────┐
│                │      │                │
│ Examples:      │  ┌───▼────┐    ┌──────▼─────┐
│ • Logic.h      │  │Templates│    │Regular Fns │
│ • Factory.h    │  └───┬────┘    └──────┬─────┘
│ • Manager.h    │      │                │
└────────────────┘      │                │
                   ┌────▼──────────┐ ┌───▼────────────────┐
                   │SubsystemUtils.h│ │subsystem_category.h│
                   │                │ │subsystem_category. │
                   │namespace:      │ │cpp                 │
                   │ steamrot::     │ │                    │
                   │  subsystem     │ │namespace:          │
                   │                │ │ steamrot::         │
                   │Examples:       │ │  subsystem::       │
                   │• ArchetypeUtils│ │  category          │
                   │  .h            │ │                    │
                   │• LogicUtils.h  │ │Examples:           │
                   └────────────────┘ │• logic_collision.h │
                                      │• entity_memory.h   │
                                      │• event_conversion.h│
                                      └────────────────────┘
```

## File-to-Namespace Mapping

```
┌──────────────────────────────┬─────────────────────────────────────┐
│ File Pattern                 │ Namespace                           │
├──────────────────────────────┼─────────────────────────────────────┤
│ ClassName.h                  │ steamrot::ClassName                 │
│ PascalCase                   │                                     │
├──────────────────────────────┼─────────────────────────────────────┤
│ subsystem_category.h         │ steamrot::subsystem::category       │
│ snake_case                   │                                     │
├──────────────────────────────┼─────────────────────────────────────┤
│ SubsystemUtils.h             │ steamrot::subsystem                 │
│ PascalCase (header-only)     │                                     │
├──────────────────────────────┼─────────────────────────────────────┤
│ subsystem_test_helpers.h     │ steamrot::tests::subsystem          │
│ snake_case (test code)       │                                     │
└──────────────────────────────┴─────────────────────────────────────┘
```

## Subsystem Growth Pattern

### Example: Logic Subsystem Evolution

```
Stage 1: Initial Classes Only
├── src/logic/
│   ├── Logic.h/cpp                    (base class)
│   └── UIActionLogic.h/cpp            (derived class)

Stage 2: Adding Free Functions
├── src/logic/
│   ├── Logic.h/cpp
│   ├── UIActionLogic.h/cpp
│   └── logic_collision.h/cpp          (new: collision functions)

Stage 3: Growing the Function Library
├── src/logic/
│   ├── Logic.h/cpp
│   ├── UIActionLogic.h/cpp
│   ├── UIRenderLogic.h/cpp            (new: render class)
│   ├── logic_collision.h/cpp
│   ├── logic_ui.h/cpp                 (new: UI functions)
│   └── logic_render.h/cpp             (new: render functions)

Stage 4: Adding Template Utilities
├── src/logic/
│   ├── Logic.h/cpp
│   ├── UIActionLogic.h/cpp
│   ├── UIRenderLogic.h/cpp
│   ├── logic_collision.h/cpp
│   ├── logic_ui.h/cpp
│   ├── logic_render.h/cpp
│   └── LogicUtils.h                   (new: template utilities)
```

### Benefits Visualization

```
Benefits of Subsystem Prefix:

src/logic/
├── Logic.h                      ┐
├── UIActionLogic.h              │ Classes grouped
├── UIRenderLogic.h              │ (PascalCase)
├── LogicFactory.h               ┘
├── logic_collision.h            ┐
├── logic_ui.h                   │ Functions grouped
├── logic_render.h               │ (snake_case + prefix)
├── logic_validation.h           │ Easy to extend!
└── logic_debug.h                ┘

Visual scan immediately shows:
✓ Which files are classes vs functions
✓ All logic functions grouped together
✓ Easy to find where to add new functions
```

## Pattern Comparison: Before vs After

### Before: Inconsistent Naming

```
❌ Hard to distinguish types:
src/logic/
├── Logic.h              (class)
├── collision.h          (functions? class?)
├── ui_helpers.h         (functions)
├── UIActionLogic.h      (class)
└── UIRenderLogic.h      (class)

src/entity/
├── EntityManager.h      (class)
├── emp_helpers.h        (functions - unclear!)
└── ArchetypeHelpers.h   (templates? functions?)

tests/logic/
├── logic_helpers.h      (test utilities)
└── draw_ui_elements_helpers.h (test utilities)
```

### After: Cohesive System

```
✅ Clear visual distinction:
src/logic/
├── Logic.h                  (class)
├── LogicFactory.h           (class)
├── UIActionLogic.h          (class)
├── UIRenderLogic.h          (class)
├── logic_collision.h        (functions)
├── logic_ui.h               (functions)
└── LogicUtils.h             (templates)

src/entity/
├── EntityManager.h          (class)
├── ArchetypeManager.h       (class)
├── entity_memory.h          (functions)
└── ArchetypeUtils.h         (templates)

tests/logic/
├── logic_test_helpers.h     (test utilities)
└── ui_test_helpers.h        (test utilities)
```

## Namespace Organization

```
steamrot::
│
├── Classes (no nested namespace)
│   ├── EntityManager
│   ├── LogicFactory
│   └── EventHandler
│
├── logic::
│   ├── collision::              (from logic_collision.h)
│   │   ├── IsMouseOverBounds()
│   │   └── CheckCollision()
│   │
│   ├── ui::                     (from logic_ui.h)
│   │   ├── GetAllFragmentNames()
│   │   └── GetAllJointNames()
│   │
│   └── (templates)              (from LogicUtils.h)
│       └── ValidateContext()
│
├── entity::
│   ├── memory::                 (from entity_memory.h)
│   │   ├── GetComponent()
│   │   └── GetComponentVector()
│   │
│   └── (templates)              (from ArchetypeUtils.h)
│       └── GenerateArchetypeID()
│
└── tests::
    ├── logic::                  (from logic_test_helpers.h)
    │   └── CheckStaticLogicCollections()
    │
    └── entity::                 (from entity_test_helpers.h)
        └── CompareToDefault()
```

## Extensibility Pattern

### Adding New Functions: Step-by-Step

```
Question: Where do I add collision validation functions?

Step 1: Identify subsystem
        → logic subsystem

Step 2: Identify category
        → collision category (validation is part of collision)

Step 3: Find or create file
        → src/logic/logic_collision.h already exists!

Step 4: Add to existing namespace
        → namespace steamrot::logic::collision { ... }

Step 5: Add your function
        ✅ Done! No new files needed.

┌─────────────────────────────────────────┐
│ src/logic/logic_collision.h             │
├─────────────────────────────────────────┤
│ namespace steamrot::logic::collision {  │
│                                         │
│   // Existing functions                │
│   bool IsMouseOverBounds(...);         │
│   void CheckCollision(...);            │
│                                         │
│   // Your new function - just add here!│
│   bool ValidateCollisionState(...);    │
│                                         │
│ }                                       │
└─────────────────────────────────────────┘
```

### Adding New Category: Step-by-Step

```
Question: Where do I add rendering utility functions?

Step 1: Identify subsystem
        → logic subsystem

Step 2: Identify category
        → NEW category: rendering utilities

Step 3: Create new file
        → src/logic/logic_render.h
        → src/logic/logic_render.cpp

Step 4: Use appropriate namespace
        → namespace steamrot::logic::render { ... }

Step 5: Add your functions
        ✅ New category created!

┌─────────────────────────────────────────┐
│ src/logic/logic_render.h (NEW)         │
├─────────────────────────────────────────┤
│ namespace steamrot::logic::render {    │
│                                         │
│   void DrawDebugBounds(...);           │
│   void RenderGrid(...);                │
│   void HighlightElement(...);          │
│                                         │
│ }                                       │
└─────────────────────────────────────────┘

Result in directory:
src/logic/
├── logic_collision.h    (existing category)
├── logic_ui.h          (existing category)
└── logic_render.h      (NEW category)
                        ↑
                All follow same pattern!
```

## Color-Coded Reference (Text Version)

```
[CLASS]     = PascalCase, steamrot namespace
[FUNCTIONS] = snake_case with subsystem prefix, nested namespace
[TEMPLATES] = PascalCase with "Utils" suffix, subsystem namespace
[TESTS]     = snake_case with "_test_helpers" suffix, tests namespace

Examples:

[CLASS]     EntityManager.h      → steamrot::EntityManager
[FUNCTIONS] logic_collision.h    → steamrot::logic::collision
[TEMPLATES] ArchetypeUtils.h     → steamrot::entity
[TESTS]     logic_test_helpers.h → steamrot::tests::logic
```

## Quick Decision Guide

```
┌────────────────────────────────────────────────────────────┐
│ I need to create...                    │ Use this pattern  │
├────────────────────────────────────────┼───────────────────┤
│ A class with member variables          │ ClassName.h/cpp   │
│ A factory for creating objects         │ FactoryName.h/cpp │
│ A manager for a subsystem              │ ManagerName.h/cpp │
├────────────────────────────────────────┼───────────────────┤
│ Free functions for a subsystem         │ subsystem_        │
│                                        │ category.h/cpp    │
├────────────────────────────────────────┼───────────────────┤
│ Template utilities                     │ SubsystemUtils.h  │
│                                        │ (header only)     │
├────────────────────────────────────────┼───────────────────┤
│ Test helper functions                  │ subsystem_test_   │
│                                        │ helpers.h/cpp     │
├────────────────────────────────────────┼───────────────────┤
│ Unit tests                             │ ModuleName.test.  │
│                                        │ cpp               │
└────────────────────────────────────────┴───────────────────┘
```

## Summary: The Core Pattern

```
 ┌──────────────────────────────────────────────────────────┐
 │                 SteamRot Naming System                   │
 ├──────────────────────────────────────────────────────────┤
 │                                                          │
 │  Classes:     PascalCase              (EntityManager)   │
 │               ↓                                          │
 │               namespace steamrot                         │
 │                                                          │
 │  Functions:   subsystem_category      (logic_collision) │
 │               ↓                                          │
 │               namespace steamrot::subsystem::category    │
 │                                                          │
 │  Templates:   SubsystemUtils          (ArchetypeUtils)  │
 │               ↓                                          │
 │               namespace steamrot::subsystem              │
 │                                                          │
 │  Test Helpers: subsystem_test_helpers (logic_test_...)  │
 │               ↓                                          │
 │               namespace steamrot::tests::subsystem       │
 │                                                          │
 └──────────────────────────────────────────────────────────┘
```
