# UIElement Drawing Separation - Visual Diagrams

**Date**: December 11, 2025  
**Related**: [UIELEMENT_DRAWING_SEPARATION_ANALYSIS.md](./UIELEMENT_DRAWING_SEPARATION_ANALYSIS.md)

---

## Current Architecture (Problem)

### File Structure

```
┌─────────────────────────────────────────────────────────────────┐
│                      src/types/user_interface/                  │
│                         (Data Layer)                            │
│                                                                 │
│  ┌──────────────────┐  ┌──────────────────┐                   │
│  │ UIElement.h      │  │ UIStyle.h        │                   │
│  │ (abstract base)  │  │ (pure data) ✅   │                   │
│  │                  │  │                  │                   │
│  │ virtual Draw...  │  │ ButtonStyle      │                   │
│  └──────────────────┘  │ PanelStyle       │                   │
│           ▲            │ ...              │                   │
│           │            └──────────────────┘                   │
│           │                                                    │
│  ┌────────┴────────────────────────────────────────┐          │
│  │                                                  │          │
│  │  ┌─────────────────┐  ┌─────────────────┐     │          │
│  │  │ ButtonElement.h │  │ PanelElement.h  │     │          │
│  │  │                 │  │                 │     │          │
│  │  │ #include        │  │ #include        │ ❌  │          │
│  │  │ "logic_render.h"│  │ "logic_render.h"│     │          │
│  │  │                 │  │                 │     │          │
│  │  │ DrawUIElement() │  │ DrawUIElement() │     │          │
│  │  │ { logic::...  } │  │ { logic::...  } │     │          │
│  │  └─────────────────┘  └─────────────────┘     │          │
│  │                                                  │          │
│  │  (4 more element types, all same problem)       │          │
│  └──────────────────────────────────────────────────┘          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
                          ║
                          ║ WRONG DEPENDENCY DIRECTION
                          ║ (data types depend on logic)
                          ▼
┌─────────────────────────────────────────────────────────────────┐
│                        src/logic/                               │
│                       (Logic Layer)                             │
│                                                                 │
│  ┌──────────────────┐  ┌──────────────────┐                   │
│  │ logic_render.h   │  │ UIRenderLogic.h  │                   │
│  │                  │  │                  │                   │
│  │ DrawBorder...()  │  │ ProcessLogic()   │                   │
│  │ DrawText()       │  │ DrawUIElements() │                   │
│  │ DrawNested...()  │  │                  │                   │
│  └──────────────────┘  └──────────────────┘                   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Dependency Graph (Current)

```
┌──────────────┐
│    types     │ ─────┐
│ (UIElements) │      │
└──────────────┘      │
                      │
                      ▼ ❌ BAD: Upward dependency
┌──────────────┐
│    logic     │
│(logic_render)│
└──────────────┘
       │
       │
       ▼
┌──────────────┐
│   systems    │
│ (rendering)  │
└──────────────┘
```

---

## Recommended Solution: Option 6 (Renderer Class)

### File Structure

```
┌─────────────────────────────────────────────────────────────────┐
│                      src/types/user_interface/                  │
│                         (Data Layer)                            │
│                      ZERO DEPENDENCIES ✅                        │
│                                                                 │
│  ┌──────────────────┐  ┌──────────────────┐                   │
│  │ UIElement.h      │  │ UIStyle.h        │                   │
│  │ (pure data)      │  │ (pure data)      │                   │
│  │                  │  │                  │                   │
│  │ // NO virtual    │  │ ButtonStyle      │                   │
│  │ // DrawUIElement │  │ PanelStyle       │                   │
│  └──────────────────┘  │ ...              │                   │
│           ▲            └──────────────────┘                   │
│           │                                                    │
│           │                                                    │
│  ┌────────┴────────────────────────────────────────┐          │
│  │                                                  │          │
│  │  ┌─────────────────┐  ┌─────────────────┐     │          │
│  │  │ ButtonElement.h │  │ PanelElement.h  │     │          │
│  │  │                 │  │                 │     │          │
│  │  │ // Pure data    │  │ // Pure data    │ ✅  │          │
│  │  │ string label;   │  │ // No methods   │     │          │
│  │  │ // No methods   │  │                 │     │          │
│  │  │                 │  │                 │     │          │
│  │  └─────────────────┘  └─────────────────┘     │          │
│  │                                                  │          │
│  │  (4 more element types, all pure data)          │          │
│  └──────────────────────────────────────────────────┘          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
                          ▲
                          ║ CORRECT DEPENDENCY DIRECTION
                          ║ (logic depends on data types)
                          ║
┌─────────────────────────────────────────────────────────────────┐
│                        src/logic/                               │
│                       (Logic Layer)                             │
│                                                                 │
│  ┌────────────────────────────────────────────┐                │
│  │        UIElementRenderer.h/cpp (NEW)       │                │
│  │                                            │                │
│  │  class UIElementRenderer {                 │                │
│  │    sf::RenderTexture& m_texture;          │                │
│  │    const UIStyle& m_style;                │                │
│  │                                            │                │
│  │    void DrawButton(const ButtonElement&); │                │
│  │    void DrawPanel(const PanelElement&);   │                │
│  │    // ... all element types                │                │
│  │                                            │                │
│  │  public:                                   │                │
│  │    void Draw(const UIElement& element) {   │                │
│  │      if (auto* btn = dynamic_cast<...>())  │                │
│  │        DrawButton(*btn);                   │                │
│  │      // ... dispatch to appropriate method │                │
│  │    }                                       │                │
│  │                                            │                │
│  │    void DrawNested(const UIElement&);      │                │
│  │  };                                        │                │
│  └────────────────────────────────────────────┘                │
│                                                                 │
│  ┌──────────────────┐  ┌──────────────────┐                   │
│  │ logic_render.h   │  │ UIRenderLogic.h  │                   │
│  │                  │  │                  │                   │
│  │ DrawBorder...()  │  │ ProcessLogic()   │                   │
│  │ DrawText()       │  │ DrawUIElements() │                   │
│  │ (helpers)        │  │  - uses renderer │                   │
│  └──────────────────┘  └──────────────────┘                   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Dependency Graph (Recommended)

```
┌──────────────┐
│    types     │ ◄─────┐
│ (UIElements) │       │
└──────────────┘       │
       ▲               │
       │               │ ✅ GOOD: Downward dependency
       │               │
┌──────────────┐       │
│    logic     │───────┘
│(UI Renderer) │
└──────────────┘
       │
       │
       ▼
┌──────────────┐
│   systems    │
│ (rendering)  │
└──────────────┘
```

---

## Call Flow Comparison

### Current (Virtual Method Dispatch)

```
┌─────────────────────┐
│  UIRenderLogic      │
│  ::DrawUIElements() │
└──────────┬──────────┘
           │
           │ Calls for each UI component
           ▼
┌──────────────────────────────────┐
│  logic::render::                 │
│  DrawNestedUIElements(element)   │
└──────────┬───────────────────────┘
           │
           │ Virtual call
           ▼
┌──────────────────────────────────┐
│  element.DrawUIElement()         │
│  (virtual dispatch to subclass)  │
└──────────┬───────────────────────┘
           │
           │ Which calls:
           ▼
┌──────────────────────────────────┐
│  logic::render::DrawBorder...()  │
│  logic::render::DrawText()       │
│  (actual drawing code)           │
└──────────────────────────────────┘
```

### Recommended (Renderer Class)

```
┌─────────────────────┐
│  UIRenderLogic      │
│  ::DrawUIElements() │
└──────────┬──────────┘
           │
           │ Create renderer
           ▼
┌──────────────────────────────────┐
│  UIElementRenderer renderer(...) │
└──────────┬───────────────────────┘
           │
           │ For each UI component
           ▼
┌──────────────────────────────────┐
│  renderer.DrawNested(element)    │
└──────────┬───────────────────────┘
           │
           │ Type dispatch (dynamic_cast chain)
           ▼
┌──────────────────────────────────┐
│  renderer.DrawButton(button)     │
│  (or DrawPanel, DrawDropDown...) │
└──────────┬───────────────────────┘
           │
           │ Which calls:
           ▼
┌──────────────────────────────────┐
│  logic::render::DrawBorder...()  │
│  logic::render::DrawText()       │
│  (actual drawing code)           │
└──────────────────────────────────┘
```

**Key Difference**: Type dispatch moves from virtual method (in data type) to dynamic_cast (in logic layer)

---

## Option 1: Visitor Pattern (Alternative)

### Structure

```
┌─────────────────────────────────────────────────────────────────┐
│                      src/types/user_interface/                  │
│                                                                 │
│  ┌──────────────────┐                                          │
│  │ UIElement.h      │                                          │
│  │                  │                                          │
│  │ virtual Accept(  │  ◄──── One virtual method (Accept)      │
│  │   UIElementVis.) │                                          │
│  └──────────────────┘                                          │
│           ▲                                                    │
│           │                                                    │
│  ┌────────┴────────────────────────────┐                      │
│  │                                      │                      │
│  │  ButtonElement   PanelElement       │                      │
│  │  void Accept(v) { v.Visit(*this); } │                      │
│  └──────────────────────────────────────┘                      │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
                          ▲
                          │ uses
                          │
┌─────────────────────────────────────────────────────────────────┐
│                        src/logic/                               │
│                                                                 │
│  ┌────────────────────────────────────────┐                    │
│  │   UIElementVisitor (interface)         │                    │
│  │                                        │                    │
│  │   virtual Visit(ButtonElement&) = 0;   │                    │
│  │   virtual Visit(PanelElement&) = 0;    │                    │
│  │   // ... all element types             │                    │
│  └────────────────────────────────────────┘                    │
│                          ▲                                      │
│                          │ implements                           │
│  ┌────────────────────────────────────────┐                    │
│  │   UIRenderVisitor : UIElementVisitor   │                    │
│  │                                        │                    │
│  │   Visit(ButtonElement& btn) override { │                    │
│  │     // Draw button logic here          │                    │
│  │   }                                    │                    │
│  │                                        │                    │
│  │   Visit(PanelElement& panel) override {│                    │
│  │     // Draw panel logic here           │                    │
│  │   }                                    │                    │
│  └────────────────────────────────────────┘                    │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Call Flow (Visitor)

```
┌─────────────────────┐
│  UIRenderLogic      │
└──────────┬──────────┘
           │ Create visitor
           ▼
┌──────────────────────────────────┐
│  UIRenderVisitor visitor(...)    │
└──────────┬───────────────────────┘
           │
           │ For each element
           ▼
┌──────────────────────────────────┐
│  element.Accept(visitor)         │  ◄── Virtual call #1
└──────────┬───────────────────────┘
           │
           │ Dispatches to:
           ▼
┌──────────────────────────────────┐
│  ButtonElement::Accept(visitor)  │
│  { visitor.Visit(*this); }       │
└──────────┬───────────────────────┘
           │
           │ Overload resolution (compile-time)
           ▼
┌──────────────────────────────────┐
│  visitor.Visit(ButtonElement&)   │  ◄── Virtual call #2
└──────────┬───────────────────────┘
           │
           ▼
┌──────────────────────────────────┐
│  Drawing logic for ButtonElement │
└──────────────────────────────────┘
```

**Double Dispatch**: Two virtual calls (Accept + Visit) for type-safe dispatch

---

## Performance Comparison

### Virtual Method (Current)

```
CPU Cycles per Draw Call:
┌─────────────────────┐
│ Virtual dispatch: 1 │ ◄─── vtable lookup
├─────────────────────┤
│ Drawing ops: 1000+  │ ◄─── SFML rendering (dominant cost)
└─────────────────────┘

Total: ~1001 cycles
Overhead: ~0.1%
```

### Renderer with dynamic_cast (Recommended)

```
CPU Cycles per Draw Call:
┌─────────────────────┐
│ dynamic_cast: 5-30  │ ◄─── RTTI check chain (varies by position)
├─────────────────────┤
│ Drawing ops: 1000+  │ ◄─── SFML rendering (dominant cost)
└─────────────────────┘

Total: ~1005-1030 cycles
Overhead: ~0.5-3%
```

### Visitor Pattern (Alternative)

```
CPU Cycles per Draw Call:
┌─────────────────────┐
│ Accept virtual: 1   │ ◄─── First virtual call
│ Visit virtual: 1    │ ◄─── Second virtual call
├─────────────────────┤
│ Drawing ops: 1000+  │ ◄─── SFML rendering (dominant cost)
└─────────────────────┘

Total: ~1002 cycles
Overhead: ~0.2%
```

**Conclusion**: All options have negligible overhead for UI rendering

---

## Element Type Hierarchy (Before & After)

### Before (Current)

```
                    UIElement (abstract)
                    ├─ position, size (data)
                    ├─ children (data)
                    ├─ DrawUIElement() = 0  ◄── PROBLEM: Behavior in data type
                    └─ Clone() = 0
                         │
        ┌────────────────┼────────────────┐
        │                │                │
   ButtonElement    PanelElement   DropDownListElement
   ├─ label         ├─ (no extra)  ├─ labels
   └─ DrawUIElement └─ DrawUIElement└─ DrawUIElement
      (inline impl)    (inline impl)   (inline impl)
```

### After (Recommended)

```
                    UIElement (abstract)
                    ├─ position, size (data)
                    ├─ children (data)
                    └─ Clone() = 0  ◄── Only lifecycle methods
                         │
        ┌────────────────┼────────────────┐
        │                │                │
   ButtonElement    PanelElement   DropDownListElement
   ├─ label         ├─ (no extra)  ├─ labels
   └─ (pure data)   └─ (pure data) └─ (pure data)

Drawing logic moved to:
┌──────────────────────────────────┐
│     UIElementRenderer            │
│                                  │
│  DrawButton(ButtonElement&)      │
│  DrawPanel(PanelElement&)        │
│  DrawDropDownList(DropDown...)   │
│  ...                             │
└──────────────────────────────────┘
```

---

## Migration Path Visualization

```
┌────────────────────────────────────────────────────────────────────┐
│                         MIGRATION PHASES                           │
└────────────────────────────────────────────────────────────────────┘

Phase 1: CREATE RENDERER (No Breaking Changes)
┌────────────────────────────────────────────────────────────────────┐
│ ✅ Create UIElementRenderer.h/cpp                                  │
│ ✅ Implement all Draw*() methods (copy from existing)              │
│ ✅ Add tests for renderer                                          │
│                                                                    │
│ Result: Two ways to draw UIElements (old + new)                   │
└────────────────────────────────────────────────────────────────────┘

Phase 2: SWITCH LOGIC LAYER (Minimal Breaking)
┌────────────────────────────────────────────────────────────────────┐
│ ✅ Update UIRenderLogic to use UIElementRenderer                   │
│ ✅ Deprecate logic::render::DrawNestedUIElements()                 │
│ ✅ Test that rendering still works                                 │
│                                                                    │
│ Result: Production code uses renderer, types still have methods   │
└────────────────────────────────────────────────────────────────────┘

Phase 3: CLEAN UP TYPES (Breaking for Tests)
┌────────────────────────────────────────────────────────────────────┐
│ ✅ Remove DrawUIElement from all 6 UIElement types                 │
│ ✅ Remove #include "logic_render.h" from type headers              │
│ ✅ Remove pure virtual from UIElement base                         │
│ ✅ Update any tests that called DrawUIElement directly             │
│                                                                    │
│ Result: Pure data types, zero dependencies ✅                      │
└────────────────────────────────────────────────────────────────────┘

Total Estimated Time: 4-6 hours
```

---

## Comparison to Similar Patterns in Codebase

### Pattern: Components + Logic Classes

```
CURRENT (Good Example):
┌──────────────────┐
│  CUserInterface  │  ◄── Pure data (position, visibility)
│  (Component)     │      NO behavioral methods
└──────────────────┘
         ▲
         │ operated on by
         │
┌──────────────────┐
│ UIRenderLogic    │  ◄── Operations on components
│ UICollisionLogic │      ProcessLogic() methods
│ UIActionLogic    │
└──────────────────┘

TARGET (What We Want):
┌──────────────────┐
│  UIElement types │  ◄── Pure data (position, size, label)
│  (Data)          │      NO DrawUIElement method
└──────────────────┘
         ▲
         │ operated on by
         │
┌──────────────────┐
│UIElementRenderer │  ◄── Operations on UIElements
│ (Logic)          │      Draw*() methods
└──────────────────┘
```

---

## Summary Diagram: The Big Picture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        GOAL: CLEAN LAYERS                           │
└─────────────────────────────────────────────────────────────────────┘

BEFORE (Current):                    AFTER (Recommended):
┌─────────────┐                     ┌─────────────┐
│   Systems   │                     │   Systems   │
└──────┬──────┘                     └──────┬──────┘
       │                                   │
       ▼                                   ▼
┌─────────────┐                     ┌─────────────┐
│    Logic    │                     │    Logic    │
│             │                     │ ┌─────────┐ │
│ UIRenderLg. │◄──┐                 │ │Renderer │ │
└─────────────┘   │                 │ └────┬────┘ │
                  │                 └──────┼──────┘
                  │ BAD                    │ GOOD
                  │ dependency             │ dependency
                  │                        ▼
┌─────────────┐   │                 ┌─────────────┐
│    Types    │───┘                 │    Types    │
│             │                     │             │
│ UIElements  │                     │ UIElements  │
│ (has Draw   │                     │ (pure data) │
│  methods)   │                     │             │
└─────────────┘                     └─────────────┘

❌ Circular dependency               ✅ Clean one-way dependency
❌ Data knows about logic            ✅ Logic operates on data
❌ Hard to test                      ✅ Easy to test
```

---

**Document Status**: Visual supplement to full analysis  
**See Also**: [UIELEMENT_DRAWING_SEPARATION_ANALYSIS.md](./UIELEMENT_DRAWING_SEPARATION_ANALYSIS.md)  
**Quick Ref**: [UIELEMENT_DRAWING_SEPARATION_QUICK_REF.md](./UIELEMENT_DRAWING_SEPARATION_QUICK_REF.md)
