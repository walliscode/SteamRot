# Unified Test Dashboard - Analysis & Implementation Plan

**Status**: Implementation Approved  
**Date**: 2026-02-02  
**Version**: 2.0  
**Approach**: Option 1 - Web-Based Application (Unified Dashboard)

## Executive Summary

This document outlines the implementation plan for a **unified test dashboard** that combines test data visualization with test data creation capabilities. The dashboard will use a single web-based interface with two modes:

1. **"Analyze Tests"** - Visualize existing test coverage, explore simulation paths, filter tests
2. **"Create New Test"** - Step-by-step guided workflow for creating test_data.json files

### Key Decisions

- ✅ **Web-based** using Python Flask + HTML/CSS/JavaScript
- ✅ **Unified dashboard** - single application with mode switching
- ✅ **Simplified simulation** - Logic classes only (removed fine function control)
- ✅ **Automatic component discovery** - schema introspection from FlatBuffers
- ✅ **UUID-based filenames** - eliminates naming conflicts

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Unified Dashboard Design](#unified-dashboard-design)
3. [Component Registration System](#component-registration-system)
4. [Simplified Simulation Builder](#simplified-simulation-builder)
5. [Implementation Plan](#implementation-plan)
6. [File Organization](#file-organization)
7. [API Specification](#api-specification)
8. [Development Phases](#development-phases)

---

## Architecture Overview

### Unified Dashboard Structure

The dashboard combines two modes in a single Flask application:

```
┌─────────────────────────────────────────────────────────┐
│              SteamRot Test Dashboard                    │
│         [Analyze Tests]  [Create New Test]              │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  Mode: Analyze Tests                                    │
│  ┌────────────────────────────────────────────────┐    │
│  │  - Coverage Matrix                              │    │
│  │  - Simulation Path Explorer                     │    │
│  │  - Test Filtering & Search                      │    │
│  │  - Test Details View                            │    │
│  └────────────────────────────────────────────────┘    │
│                                                          │
│  Mode: Create New Test                                  │
│  ┌────────────────────────────────────────────────┐    │
│  │  Step 1: Metadata Editor                        │    │
│  │  Step 2: Entity & Component Configuration       │    │
│  │  Step 3: Simulation Builder (Logic Classes)     │    │
│  │  Step 4: Expected Snapshots                     │    │
│  │  Step 5: Preview & Save (UUID filename)         │    │
│  └────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
```

### Technology Stack

- **Backend**: Python Flask (lightweight web framework)
- **Frontend**: HTML + Vanilla JavaScript + CSS
- **Data**: FlatBuffers for schema validation
- **Storage**: JSON files with UUID naming

### Shared Components

Both modes share:
- Flask backend server
- CSS styling and UI components
- FlatBuffers schema parser
- File scanner and loader
- Validation logic

---

## Unified Dashboard Design

### Navigation

```html
<nav class="mode-switcher">
  <button class="mode-btn active" data-mode="analyze">
    📊 Analyze Tests
  </button>
  <button class="mode-btn" data-mode="create">
    ✏️ Create New Test
  </button>
</nav>
```

### Mode Switching

JavaScript toggles between modes:
```javascript
document.querySelectorAll('.mode-btn').forEach(btn => {
  btn.addEventListener('click', () => {
    const mode = btn.dataset.mode;
    document.getElementById('analyze-view').classList.toggle('hidden', mode !== 'analyze');
    document.getElementById('create-view').classList.toggle('hidden', mode !== 'create');
  });
});
```

### Analyze Mode (Existing Dashboard)

Keeps all current functionality:
- Coverage matrix (function × logic class)
- Simulation path explorer
- Search and filtering
- Test details panel

### Create Mode (New Functionality)

Wizard-style interface with steps:

1. **Metadata Step**
   - Test name, description, tags
   - Expected to pass checkbox
   - Version selector

2. **Entities Step**
   - Entity memory pool size
   - Add/remove entities
   - Select components per entity
   - Configure component values

3. **Simulation Step**
   - Add simulation steps
   - Select Logic class for each step
   - Reorder steps with drag-and-drop

4. **Snapshots Step**
   - Define expected states at specific ticks
   - Copy from starting state option

5. **Preview & Save**
   - JSON preview with syntax highlighting
   - Validation results
   - Save with auto-generated UUID filename

---

## Component Registration System

### Automatic Discovery

The system **automatically discovers** components by parsing FlatBuffers schemas:

```python
# schema_loader.py
class SchemaLoader:
    def load_entity_schema(self):
        """Parse entities.fbs to discover all components."""
        schema_path = "src/types/flatbuffers/entities/entities.fbs"
        
        # Parse FlatBuffers schema
        entity_table = self.parse_fbs_file(schema_path)
        
        # Extract component fields
        components = []
        for field in entity_table.fields:
            if field.name.startswith('c_'):
                components.append({
                    'name': field.name,
                    'type': field.type,
                    'schema': self.load_component_schema(field.type)
                })
        
        return components
```

### Adding New Components

To register a new component, developers only need to update:

#### 1. C++ Component Registration

**File**: `src/types/components/containers.h`

```cpp
// Add to ComponentRegister tuple
typedef std::tuple<CMeta, CUserInterface, CMachinaForm, 
                   CGrimoireMachina, CUIState, 
                   CNewComponent>  // Add here
    ComponentRegister;
```

#### 2. FlatBuffers Schema

**File**: `src/types/flatbuffers/entities/entities.fbs`

```fbs
// Include the new component schema
include "new_component.fbs";

table EntityDataFbs {
  index: uint32;
  c_user_interface: UserInterfaceFbs;
  c_grimoire_machina: GrimoireMachinaData;
  c_ui_state: UIStateCollectionData;
  c_new_component: NewComponentData;  // Add here
}
```

**File**: `src/types/flatbuffers/entities/new_component.fbs`

```fbs
namespace steamrot;

table NewComponentData {
  field1: string;
  field2: int;
  field3: bool;
}
```

#### 3. Automatic Detection (No Action Required)

The Flask backend automatically:
1. Scans `entities.fbs` on startup
2. Detects the new `c_new_component` field
3. Loads `new_component.fbs` schema
4. Generates UI form fields dynamically
5. Provides validation for component data

### Schema Introspection API

The backend exposes component schemas via REST API:

```python
# GET /api/schema/components
{
  "components": [
    {
      "name": "c_user_interface",
      "type": "UserInterfaceFbs",
      "fields": [...]
    },
    {
      "name": "c_new_component",
      "type": "NewComponentData",
      "fields": [
        {"name": "field1", "type": "string", "required": false},
        {"name": "field2", "type": "int", "required": false},
        {"name": "field3", "type": "bool", "required": false}
      ]
    }
  ]
}
```

Frontend uses this to build component editor forms dynamically.

---

## Simplified Simulation Builder

### Removed: Fine Function Control

**Previous approach** (being removed):
```json
{
  "simulation_type": "Action",
  "execution_mode": "Function",  // ❌ Removed
  "function_type": "some_function"  // ❌ Removed
}
```

**New simplified approach**:
```json
{
  "logic_class_type": "UIActionLogic"  // ✅ Only this
}
```

### Updated FlatBuffers Schema

**File**: `src/types/flatbuffers/testing/simulation_data.fbs`

```fbs
enum LogicClassEnumFbs : byte {
  None = 0,
  UIActionLogic = 1,
  UICollisionLogic = 2,
  UIRenderLogic = 3,
  UIStateLogic = 4,
  CraftingRenderLogic = 5,
}

table SimulationStepFbs {
  /// @brief Logic class to execute
  logic_class_type: LogicClassEnumFbs = None;
}

table SimulationDataFbs {
  steps: [SimulationStepFbs];
  description: string;
}
```

### Simulation Builder UI

Simple dropdown selection:

```html
<div class="simulation-builder">
  <h3>Simulation Steps</h3>
  <div id="step-list">
    <!-- Draggable step items -->
    <div class="step-item" draggable="true">
      <span class="step-number">1</span>
      <select class="logic-class-selector">
        <option value="None">-- Select Logic Class --</option>
        <option value="UIActionLogic">UIActionLogic</option>
        <option value="UICollisionLogic">UICollisionLogic</option>
        <option value="UIRenderLogic">UIRenderLogic</option>
        <option value="UIStateLogic">UIStateLogic</option>
        <option value="CraftingRenderLogic">CraftingRenderLogic</option>
      </select>
      <button class="delete-step">❌</button>
    </div>
  </div>
  <button id="add-step">+ Add Step</button>
</div>
```

### Benefits of Simplification

1. ✅ **Simpler UI** - just select Logic class from dropdown
2. ✅ **Less code** - removed function type parsing and validation
3. ✅ **Clearer semantics** - one concept instead of two
4. ✅ **Easier maintenance** - fewer moving parts

---

## Implementation Plan


### Phase 1: Core Unified Dashboard (3-4 weeks)

**Goal**: Single dashboard with mode switching and basic test creation

**Features:**
- Unified Flask backend serving both modes
- Mode switcher UI (Analyze / Create)
- Existing analyze mode (current dashboard functionality)
- Basic create mode:
  - Metadata editor
  - Simple entity editor
  - Logic class simulation builder (simplified, no functions)
  - JSON preview
  - UUID file saving

**Deliverables:**
- Single Flask application
- Shared CSS/JS between modes
- Schema loader with automatic component discovery
- Basic validation
- File generation working

**Timeline**: 3-4 weeks

### Phase 2: Advanced Creation Features (2-3 weeks)

**Goal**: Complete test data creation capabilities

**Features:**
- Multiple entity support
- Complex component editors (nested structures)
- Dynamic form generation from FlatBuffers schemas
- Expected snapshot editor
- Enhanced validation with detailed errors
- Load and modify existing files
- Copy/paste entities between tests

**Deliverables:**
- Full component editors for all existing components
- Comprehensive validation
- Enhanced UI/UX with drag-and-drop
- Documentation

**Timeline**: 2-3 weeks

### Phase 3: Polish & Coverage Integration (1-2 weeks)

**Goal**: Seamless workflow and excellent UX

**Features:**
- Coverage gap highlighting in create mode
- Suggest untested combinations
- Keyboard shortcuts
- Auto-save drafts
- Export/import templates
- Tutorial tooltips

**Deliverables:**
- Coverage integration
- Polish and refinements
- User documentation
- Tutorial guide

**Timeline**: 1-2 weeks

**Total Timeline**: 6-9 weeks

---

## File Organization

### Unified Dashboard Structure

```
tools/test_dashboard/          # Unified dashboard (renamed from separate tools)
├── __init__.py
├── __main__.py                # Entry point
├── server.py                  # Flask server (NEW)
├── parser.py                  # Existing test data parser
├── scanner.py                 # Existing file scanner  
├── generate.py                # Dashboard HTML generator (existing)
├── html_generator.py          # HTML template engine (existing)
│
├── api/                       # NEW: Backend API
│   ├── __init__.py
│   ├── routes.py              # API route definitions
│   ├── schema_loader.py       # FlatBuffers schema introspection
│   ├── validator.py           # Test data validation
│   └── file_manager.py        # File I/O and UUID generation
│
├── static/                    # Frontend assets
│   ├── css/
│   │   ├── dashboard.css      # Existing analyze mode styles
│   │   ├── creator.css        # NEW: Create mode styles
│   │   └── shared.css         # NEW: Shared styles
│   ├── js/
│   │   ├── dashboard.js       # Existing analyze mode script
│   │   ├── mode-switcher.js   # NEW: Mode switching logic
│   │   ├── creator/           # NEW: Creator mode scripts
│   │   │   ├── metadata-editor.js
│   │   │   ├── entity-editor.js
│   │   │   ├── component-editor.js
│   │   │   ├── simulation-builder.js
│   │   │   └── preview.js
│   │   └── shared/            # NEW: Shared utilities
│   │       ├── api-client.js
│   │       └── validation.js
│
├── templates/                 # Flask templates
│   ├── base.html              # Existing dashboard template (updated)
│   ├── analyze.html           # NEW: Analyze mode content
│   └── create.html            # NEW: Create mode content
│
└── requirements.txt           # Dependencies
```

### CMake Integration

Update `cmake/GenerateTestDashboard.cmake`:

```cmake
# Launch unified test dashboard (analyze + create modes)
add_custom_target(test_dashboard
    COMMAND ${Python3_EXECUTABLE} -m tools.test_dashboard
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Launching Unified Test Dashboard..."
)
```

Usage:
```bash
cmake --build --preset Debug --target test_dashboard
# Opens browser at http://localhost:5000
# Default mode: Analyze Tests
# Switch to Create mode via UI button
```

---

## API Specification

### Schema Introspection

```python
# GET /api/schema/components
# Returns all available components with their schemas
{
  "components": [
    {
      "name": "c_user_interface",
      "display_name": "User Interface",
      "type": "UserInterfaceFbs",
      "required": false,
      "fields": [...]
    },
    {
      "name": "c_grimoire_machina",
      "display_name": "Grimoire Machina",
      "type": "GrimoireMachinaData",
      "required": false,
      "fields": [...]
    }
  ]
}

# GET /api/schema/logic-classes
# Returns all available logic classes
{
  "logic_classes": [
    {"value": "UIActionLogic", "label": "UI Action Logic"},
    {"value": "UICollisionLogic", "label": "UI Collision Logic"},
    {"value": "UIRenderLogic", "label": "UI Render Logic"},
    {"value": "UIStateLogic", "label": "UI State Logic"},
    {"value": "CraftingRenderLogic", "label": "Crafting Render Logic"}
  ]
}
```

### File Operations

```python
# POST /api/files/create
# Body: { test_data: {...} }
# Response: { uuid: "3f2504e0-...", path: "tests/unit/ui/3f2504e0-....json" }

# GET /api/files/list
# Response: { files: [ { uuid: "...", name: "...", path: "..." }, ... ] }

# GET /api/files/:uuid
# Response: { test_data: {...} }

# PUT /api/files/:uuid
# Body: { test_data: {...} }
# Response: { success: true }
```

### Validation

```python
# POST /api/validate/full
# Body: { test_data: {...} }
# Response: 
{
  "valid": true,
  "errors": [],
  "warnings": []
}

# Or on error:
{
  "valid": false,
  "errors": [
    {
      "field": "meta_data.test_name",
      "message": "Test name is required",
      "severity": "error"
    }
  ],
  "warnings": []
}
```

### Coverage Integration

```python
# GET /api/coverage/gaps
# Response:
{
  "gaps": [
    {
      "logic_class": "UIActionLogic",
      "suggested_components": ["c_user_interface"],
      "reason": "No tests cover UIActionLogic with c_user_interface"
    }
  ]
}
```

---

## Development Phases

### Phase 1 Tasks

**Week 1-2: Backend Foundation**
- [ ] Set up Flask server structure
- [ ] Implement schema loader (parse .fbs files)
- [ ] Create API routes for schema introspection
- [ ] Add file manager with UUID generation
- [ ] Basic validation endpoint

**Week 2-3: Frontend Foundation**
- [ ] Add mode switcher to existing dashboard
- [ ] Create metadata editor UI
- [ ] Create entity editor UI (basic)
- [ ] Implement simulation builder (Logic classes only)
- [ ] Add JSON preview panel

**Week 3-4: Integration & Testing**
- [ ] Connect frontend to API
- [ ] Test component discovery
- [ ] Verify file saving with UUIDs
- [ ] End-to-end test creation workflow
- [ ] Bug fixes and polish

### Phase 2 Tasks

**Week 4-5: Advanced Component Editors**
- [ ] Dynamic form generation from schemas
- [ ] Nested object editors
- [ ] UI element tree editor
- [ ] Component field validation
- [ ] Copy/paste functionality

**Week 5-6: Snapshot Editor & File Management**
- [ ] Expected snapshot editor
- [ ] Load existing test files
- [ ] Edit and save as new
- [ ] Batch operations
- [ ] Advanced validation

**Week 6-7: Polish & UX**
- [ ] Drag-and-drop for simulation steps
- [ ] Keyboard shortcuts
- [ ] Error handling improvements
- [ ] Loading states
- [ ] Success/failure feedback

### Phase 3 Tasks

**Week 7-8: Coverage Integration**
- [ ] Query analyze mode coverage data
- [ ] Highlight gaps in create mode
- [ ] Suggest next tests
- [ ] Show related tests
- [ ] Coverage stats in create mode

**Week 8-9: Documentation & Launch**
- [ ] User guide
- [ ] Tutorial tooltips
- [ ] Video walkthrough
- [ ] API documentation
- [ ] Final testing and release

---

## Risk Assessment
    ├── models.py         # Common data models
    └── utils.py          # Utility functions
```


### Technical Risks

| Risk | Impact | Likelihood | Mitigation |
|------|--------|------------|------------|
| FlatBuffers schema parsing complexity | High | Medium | Use Python FlatBuffers library, start with simple schemas, test thoroughly |
| Component editor complexity for nested UI elements | High | High | Build incrementally, start with flat fields, add nesting gradually |
| Mode switching UX confusion | Medium | Low | Clear visual indicators, persistent mode state, user testing |
| Validation performance with large snapshots | Medium | Medium | Async validation, progress indicators, optimize schema parsing |
| Browser compatibility | Low | Low | Use standard HTML5/CSS3, test on major browsers |

### Process Risks

| Risk | Impact | Likelihood | Mitigation |
|------|--------|------------|------------|
| Scope creep | High | High | Strict adherence to 3-phase plan, feature freeze after each phase |
| Schema evolution breaking tool | High | Medium | Version detection, schema migration support, fallback to manual edit |
| User adoption resistance | Medium | Low | Involve users early, gather feedback, maintain manual JSON option |
| Integration conflicts | Medium | Low | Unified codebase from start, continuous integration testing |

### Maintenance Considerations

**Long-term:**
- **Schema Evolution**: Automatic component discovery means tool adapts as schemas change
- **Documentation**: Keep user guide in sync with features
- **Testing**: Unit tests for API, integration tests for workflows
- **Monitoring**: Log usage patterns, track errors

**Sustainability:**
- Standard web technologies (Python, HTML/CSS/JS)
- Minimal dependencies (Flask, FlatBuffers)
- Clear code structure
- Contributors familiar with web development

---

## Quick Start Guide

### For Users

```bash
# Start the unified dashboard
cmake --build --preset Debug --target test_dashboard

# Opens browser at http://localhost:5000
# Default view: Analyze Tests mode
```

**To create a new test:**
1. Click "Create New Test" in navigation
2. Fill metadata (name, description, tags)
3. Add entities and configure components
4. Add simulation steps (select Logic classes)
5. Define expected snapshots (optional)
6. Preview JSON
7. Click "Save" - generates UUID filename automatically

**To analyze existing tests:**
1. Click "Analyze Tests" in navigation
2. View coverage matrix
3. Explore simulation paths
4. Search and filter tests
5. Click any test to view details

### For Developers

**Adding a new component:**

1. Create C++ component class (`src/types/components/CNewComponent.h`)
2. Add to `ComponentRegister` tuple in `containers.h`
3. Create FlatBuffers schema (`src/types/flatbuffers/entities/new_component.fbs`)
4. Add field to `EntityDataFbs` in `entities.fbs`
5. Rebuild project
6. **Dashboard automatically detects the new component** ✅

**Adding a new Logic class:**

1. Create Logic class (`src/logic/NewLogic.cpp/h`)
2. Add to `LogicClassEnumFbs` in `simulation_data.fbs`
3. Rebuild project
4. **Dashboard automatically shows it in simulation builder** ✅

---

## Conclusion

The unified test dashboard approach provides the best balance of:

- **Single Tool**: One interface for creating and analyzing tests
- **Automatic Discovery**: Components and Logic classes detected from schemas
- **Simplified Workflow**: Removed function control, focus on Logic classes
- **Consistent UX**: Shared styles and patterns between modes
- **Easy Maintenance**: Fewer moving parts than separate tools

**Key Benefits:**
- 🚀 **Faster test creation** - guided workflow vs manual JSON
- 🎯 **Fewer errors** - real-time validation
- 📊 **Better coverage** - visualize gaps, suggest tests
- 🔄 **Single codebase** - easier to maintain
- 🌐 **Cross-platform** - runs anywhere with Python + browser
- 🔧 **Auto-discovery** - no manual component registration

**Timeline**: 6-9 weeks across 3 phases

**Recommended Next Step**: Begin Phase 1 implementation - Core Unified Dashboard

---

## Appendix: Updated Examples

### Example 1: Create Simple Test (Unified Dashboard)

**In Create Mode:**

Step 1: Metadata
```
Test name: "UI Button Visibility Test"
Description: "Verify button visibility toggle"
Tags: ["unit", "ui"]
Expected to pass: ✓
```

Step 2: Entities (automatically shows all available components)
```
Pool size: 5
Entity 0:
  [x] c_user_interface
      ui_name: "main_menu"
      is_visible: true
      root_ui_element: [configure...]
  [ ] c_grimoire_machina
  [ ] c_ui_state
```

Step 3: Simulation (simplified - Logic classes only)
```
Steps:
  1. UIRenderLogic
  2. UIStateLogic
```

Step 4: Expected Snapshots (optional)
```
Tick 1: [Copy from start] → Modify visibility
```

Step 5: Save
```
✓ Saved to: tests/unit/ui/a3b7c4d5-1234-5678-90ab-cdef12345678.test_data.json
```

### Example 2: Analyze Coverage Then Create Test

**In Analyze Mode:**
- View coverage matrix
- Notice gap: "UICollisionLogic + c_user_interface" not tested
- Click cell showing "0 tests"
- See suggestion: "Create test for this combination"

**Switch to Create Mode:**
- Click "Create Test for This Gap"
- Pre-filled metadata based on gap
- Pre-selected component: c_user_interface
- Pre-filled simulation: UICollisionLogic
- User completes remaining fields
- Save new test
- Switch back to Analyze mode
- Coverage matrix now shows "1 test" ✓

---

**End of Updated Proposal**


### Immediate Actions

1. **Get User Feedback** on this proposal
   - Validate requirements with actual users
   - Prioritize features based on user needs
   - Identify any missing workflows

2. **Proof of Concept** (1 week)
   - Build minimal Flask backend
   - Create basic metadata editor frontend
   - Test FlatBuffers schema parsing
   - Validate JSON generation

3. **Design Review** (before Phase 1)
   - UI/UX mockups for all editors
   - API contract finalization
   - Schema introspection strategy
   - Integration points with dashboard

### Implementation Priority

**Must Have (Phase 1):**
- ✅ Metadata editor
- ✅ Basic entity editor with simple components
- ✅ JSON generation and validation
- ✅ UUID-based file saving

**Should Have (Phase 2):**
- ✅ Complex component editors (nested structures)
- ✅ Simulation builder
- ✅ Load and modify existing files
- ✅ Enhanced validation feedback

**Nice to Have (Phase 3+):**
- ⭐ Dashboard integration
- ⭐ Coverage visualization
- ⭐ Visual entity layout editor
- ⭐ Test execution from creator

### Technology Stack Decision

**Recommended**: Web-based application (Python Flask + HTML/JS)

**Dependencies:**
```python
# requirements.txt for test_creator
flask>=3.0.0         # Web framework
flatbuffers>=23.0.0  # Schema validation
click>=8.1.0         # CLI interface
```

**Rationale:**
- Lightweight dependencies
- Consistent with existing Python tooling
- Easy to contribute to
- Cross-platform with no installation
- Can reuse test dashboard patterns

### Next Steps

1. **Approval**: Get stakeholder approval on approach
2. **PoC**: Build proof of concept (1 week)
3. **Phase 1**: Implement MVP (2-3 weeks)
4. **User Testing**: Get feedback from actual users
5. **Phase 2**: Advanced features (2-3 weeks)
6. **Phase 3**: Integration and polish (1-2 weeks)

### Alternative: Incremental Approach

If full web application is too ambitious initially:

**Start with**: Enhanced test dashboard
- Add "Create New Test" button to existing dashboard
- Simple form for basic test creation
- Generates template JSON file
- User completes in text editor

**Benefits:**
- Faster to implement (1 week)
- Leverages existing tool
- Validates user interest
- Can evolve into full creator later

---

## Conclusion

A web-based GUI tool for creating test_data.json files would significantly improve developer productivity and reduce errors in test data creation. The recommended approach uses Python Flask with HTML/JavaScript to build a user-friendly interface that integrates seamlessly with the existing test dashboard.

**Key Benefits:**
- 🚀 **Faster test creation** - step-by-step workflow
- 🎯 **Fewer errors** - real-time validation
- 📊 **Better coverage** - visualize gaps and suggest tests
- 🔄 **Seamless integration** - works with existing tooling
- 🌐 **Cross-platform** - runs anywhere with Python and a browser

**Recommended Next Step**: Build a proof of concept to validate the approach and gather early user feedback before committing to full implementation.

---

