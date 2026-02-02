# GUI Tool for Creating test_data.json Files - Analysis & Proposal

**Status**: Discussion / Planning Phase  
**Date**: 2026-02-02  
**Version**: 1.0

## Executive Summary

This document provides analysis and recommendations for creating a GUI tool to generate `test_data.json` files for the SteamRot game engine. The tool would streamline the test data creation process by providing:

1. **Step-by-step workflow** for building test configurations
2. **Component and logic class selection** with visual interface
3. **Auto-generation of files** with UUID naming
4. **Integration with existing test dashboard** for coverage visualization

---

## Table of Contents

1. [Current State Analysis](#current-state-analysis)
2. [Requirements Analysis](#requirements-analysis)
3. [Technology Stack Recommendations](#technology-stack-recommendations)
4. [Architecture Proposal](#architecture-proposal)
5. [Implementation Approach](#implementation-approach)
6. [Integration Strategy](#integration-strategy)
7. [Risk Assessment](#risk-assessment)
8. [Recommendations](#recommendations)

---

## Current State Analysis

### Existing Test Data Infrastructure

#### 1. FlatBuffers Schema (`test_data.fbs`)

The current test data structure includes:

```fbs
table TestDataFbs {
  meta_data: TestMetadataFbs (required);
  simulation_data: SimulationDataFbs;
  num_ticks: uint32 = 1;
  starting_engine_snapshot: EngineSnapshotFbs;
  expected_engine_snapshots: [TickSnapshotPairFbs];
}
```

**Key Components:**
- **Metadata**: Test name, description, tags, expected outcome
- **Simulation Data**: Sequence of logic class executions
- **Engine Snapshots**: Starting state and expected states at specific ticks
- **Entity Collections**: Entity memory pools with component data

#### 2. Component Types

Current components (from `entities.fbs`):
- `c_user_interface` - UI elements with hierarchical structure
- `c_grimoire_machina` - Game-specific data (fragments, joints)
- `c_ui_state` - UI state management

Each component has complex nested structures (e.g., UI elements with position, size, children, layout, spacing strategies).

#### 3. Logic Classes

Available logic classes (from `simulation_data.fbs`):
- `UIActionLogic`
- `UICollisionLogic`
- `UIRenderLogic`
- `UIStateLogic`
- `CraftingRenderLogic`

#### 4. Existing Tooling

**Test Dashboard** (`tools/test_dashboard/`):
- Python-based tool that **reads** test data files
- Generates interactive HTML dashboard for visualization
- Shows coverage matrix, simulation paths, test filtering
- Uses Python standard library only (no external dependencies)
- Outputs static HTML with embedded CSS/JavaScript

### Current Workflow Challenges

1. **Manual JSON Creation**: Users must hand-write complex nested JSON structures
2. **Schema Complexity**: FlatBuffers schemas require understanding of all field types
3. **Error-Prone**: Easy to make syntax errors or miss required fields
4. **No Validation**: No real-time feedback on valid configurations
5. **Difficult Discoverability**: Hard to know what components/logic classes are available

---

## Requirements Analysis

### Core Requirements

Based on the problem statement:

#### 1. Step-by-Step Creation
- **Guided workflow** through test data configuration
- **Progressive disclosure** - show only relevant options at each step
- **Validation at each step** to prevent errors

#### 2. Logic Class Selection
- **Visual picker** for logic classes
- **Show available logic types** (Action, Render, Collision, Movement)
- **Order simulation steps** with drag-and-drop or reordering controls

#### 3. Component Configuration
- **Select components** to include in entities
- **Configure component values** with appropriate UI controls:
  - Text inputs for strings
  - Number inputs for integers/floats
  - Checkboxes for booleans
  - Nested editors for complex objects (Vector2f, UI elements)
- **Multiple entity support** - create multiple entities with different configurations

#### 4. File Management
- **UUID-based filenames** to avoid naming conflicts
- **Specify output directory** or use default location
- **Auto-generate valid JSON** that matches FlatBuffers schema

#### 5. Test Coverage Visualization (Optional)
- **Integration with test dashboard** to show what's tested
- **Coverage gaps** - identify untested combinations
- **Test execution tracking** - which tests are passing/failing

### User Workflows

#### Workflow 1: Simple Entity Test
1. Start with metadata (name, description, tags)
2. Define starting entity collection (pool size, entities)
3. Configure components for each entity
4. Set expected entity collection (if different from start)
5. Generate and save file

#### Workflow 2: Simulation Test
1. Start with metadata
2. Define starting engine snapshot with entities
3. Add simulation steps (select logic classes)
4. Define expected snapshots at specific ticks
5. Generate and save file

#### Workflow 3: Browse and Modify
1. Load existing test data file
2. Modify any section
3. Save as new file (with new UUID)

---

## Technology Stack Recommendations

### Option 1: Web-Based Application (Recommended)

**Tech Stack:**
- **Frontend**: HTML + Vanilla JavaScript + CSS
- **Backend**: Python (Flask or FastAPI)
- **Data Validation**: Python FlatBuffers library
- **File I/O**: Python standard library

**Pros:**
- ✅ **Cross-platform** - works on any OS with a browser
- ✅ **Consistent with existing dashboard** - reuse CSS/JS patterns
- ✅ **No installation** - runs locally with Python
- ✅ **Leverages existing Python ecosystem** in the project
- ✅ **Easy to integrate** with existing test dashboard
- ✅ **Rich UI capabilities** with modern HTML5/CSS3
- ✅ **Can reuse FlatBuffers validation** from Python

**Cons:**
- ⚠️ Requires running a local web server
- ⚠️ Additional dependencies (Flask/FastAPI)

**Implementation Estimate:** 
- Phase 1 (Basic Creator): 2-3 weeks
- Phase 2 (Advanced Features): 2-3 weeks
- Phase 3 (Integration & Polish): 1-2 weeks

### Option 2: Desktop Application (Qt/Python)

**Tech Stack:**
- **Framework**: PyQt6 or PySide6
- **Language**: Python
- **Data Validation**: Python FlatBuffers library

**Pros:**
- ✅ Native application feel
- ✅ Rich UI components built-in
- ✅ No web server required
- ✅ Python ecosystem integration

**Cons:**
- ⚠️ **Heavy dependencies** (Qt framework)
- ⚠️ Platform-specific packaging required
- ⚠️ Steeper learning curve for contributors
- ⚠️ More complex distribution and installation

**Implementation Estimate:**
- Phase 1: 3-4 weeks
- Phase 2: 3-4 weeks
- Phase 3: 2-3 weeks

### Option 3: Electron Desktop App

**Tech Stack:**
- **Framework**: Electron
- **Frontend**: React or Vue.js
- **Data Validation**: Node.js with FlatBuffers

**Pros:**
- ✅ Rich web technologies
- ✅ Cross-platform native app
- ✅ Modern UI framework ecosystem

**Cons:**
- ⚠️ **Large bundle size** (Electron overhead)
- ⚠️ **Introduces Node.js** to project (currently Python-based tooling)
- ⚠️ Separate package management (npm vs pip)
- ⚠️ Learning curve for team

**Implementation Estimate:**
- Phase 1: 3-4 weeks
- Phase 2: 3-4 weeks  
- Phase 3: 2-3 weeks

### Option 4: Command-Line Interactive Tool

**Tech Stack:**
- **Framework**: Python Click + Rich (for TUI)
- **Language**: Python

**Pros:**
- ✅ Minimal dependencies
- ✅ Consistent with existing tooling
- ✅ Fast to implement
- ✅ CI/CD friendly

**Cons:**
- ⚠️ **Limited UI capabilities** - no visual component editors
- ⚠️ Poor for complex nested structures (UI elements)
- ⚠️ Less user-friendly for visual tasks
- ⚠️ Difficult to visualize entity layouts

**Implementation Estimate:**
- Phase 1: 1-2 weeks
- Phase 2: 2-3 weeks

### Recommendation: **Option 1 - Web-Based Application**

**Rationale:**
1. **Best balance** of UI richness and implementation complexity
2. **Aligns with existing tooling** (test dashboard already uses HTML/JS)
3. **Cross-platform** with no installation hassles
4. **Python backend** leverages existing infrastructure
5. **Can share code** with test dashboard (CSS, validation logic)
6. **Easier to contribute to** - more developers know HTML/CSS/JS than Qt
7. **Future extensibility** - easy to add features like drag-and-drop, preview windows

---

## Architecture Proposal

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Web Browser                          │
│                                                          │
│  ┌────────────────────────────────────────────────┐   │
│  │          Test Data Creator UI                   │   │
│  │  (HTML + CSS + JavaScript)                      │   │
│  │                                                  │   │
│  │  ┌──────────────┐  ┌──────────────┐           │   │
│  │  │   Metadata   │  │   Entity     │           │   │
│  │  │   Editor     │  │   Editor     │           │   │
│  │  └──────────────┘  └──────────────┘           │   │
│  │                                                  │   │
│  │  ┌──────────────┐  ┌──────────────┐           │   │
│  │  │  Simulation  │  │   Preview    │           │   │
│  │  │   Builder    │  │   Panel      │           │   │
│  │  └──────────────┘  └──────────────┘           │   │
│  └────────────────────────────────────────────────┘   │
│                       │ REST API                       │
└───────────────────────┼────────────────────────────────┘
                        │
┌───────────────────────┼────────────────────────────────┐
│                  Python Backend                        │
│                 (Flask/FastAPI)                        │
│                                                          │
│  ┌─────────────────┐  ┌──────────────────────────┐   │
│  │   API Routes    │  │   Schema Validator        │   │
│  │                 │  │   (FlatBuffers)           │   │
│  └─────────────────┘  └──────────────────────────┘   │
│                                                          │
│  ┌─────────────────┐  ┌──────────────────────────┐   │
│  │  File Manager   │  │   Schema Loader           │   │
│  │  (UUID gen)     │  │   (reads .fbs files)      │   │
│  └─────────────────┘  └──────────────────────────┘   │
│                                                          │
│  ┌──────────────────────────────────────────────┐     │
│  │        Integration with Test Dashboard        │     │
│  │        (shared utilities)                      │     │
│  └──────────────────────────────────────────────┘     │
└──────────────────────────────────────────────────────┘
                        │
                        ▼
            ┌───────────────────────┐
            │   File System         │
            │                       │
            │  test_data.json files │
            │  (with UUID names)    │
            └───────────────────────┘
```

### Component Breakdown

#### Frontend Components

1. **MetadataEditor**
   - Form inputs for test name, description, tags
   - Expected outcome toggle
   - Version selector

2. **EntityEditor**
   - Entity memory pool size input
   - Entity list with add/remove buttons
   - Per-entity component selector
   - Component value editors (dynamically generated based on schema)

3. **ComponentEditor** (Dynamic)
   - Generates form fields based on FlatBuffers schema
   - Nested object editors for complex types
   - Vector editors (position, size)
   - UI element tree editor with drag-and-drop

4. **SimulationBuilder**
   - Logic class selector
   - Step list with reordering
   - Add/remove simulation steps
   - Preview of execution order

5. **SnapshotEditor**
   - Tick number input
   - Expected engine state configuration
   - Copy from starting state feature

6. **PreviewPanel**
   - JSON preview (formatted)
   - Validation status display
   - Error messages with line numbers

7. **FileManager**
   - Save button (generates UUID filename)
   - Directory selector
   - Load existing file
   - Export options

#### Backend API Endpoints

```python
# Schema introspection
GET  /api/schema/components          # List available components
GET  /api/schema/components/:name    # Get component schema details
GET  /api/schema/logic-classes       # List available logic classes
GET  /api/schema/enums               # Get all enum definitions

# File operations
POST /api/files/create               # Generate UUID, create file
GET  /api/files/:uuid                # Load existing file
PUT  /api/files/:uuid                # Update existing file
GET  /api/files/list                 # List all test data files

# Validation
POST /api/validate/metadata          # Validate metadata section
POST /api/validate/entity-collection # Validate entity collection
POST /api/validate/simulation        # Validate simulation data
POST /api/validate/full              # Validate complete test data

# Integration with dashboard
GET  /api/coverage/matrix            # Get current test coverage
GET  /api/coverage/gaps              # Find untested combinations
```

#### Backend Modules

1. **`schema_loader.py`**
   - Parse `.fbs` files at startup
   - Build schema registry
   - Provide schema introspection API

2. **`validator.py`**
   - Validate JSON against FlatBuffers schema
   - Return detailed error messages
   - Check required fields, type correctness

3. **`file_manager.py`**
   - Generate UUIDs for new files
   - Save/load JSON files
   - Handle directory structure
   - File locking for concurrent access

4. **`api_routes.py`**
   - Flask/FastAPI route definitions
   - Request/response handling
   - Error handling

5. **`dashboard_integration.py`**
   - Interface with existing test dashboard
   - Coverage analysis
   - Test result tracking

### Data Flow

#### Creating a New Test

```
User Interface → Metadata Form → JSON Builder
                    ↓
                Entity Editor → JSON Builder
                    ↓
              Simulation Builder → JSON Builder
                    ↓
              Snapshot Editor → JSON Builder
                    ↓
              JSON Builder → Preview Panel
                    ↓
          Validate Button → Backend API
                    ↓
          Backend Validator → FlatBuffers Validation
                    ↓
          Success/Errors → Frontend
                    ↓
          Save Button → Backend File Manager
                    ↓
          UUID Generation → Write JSON File
                    ↓
          Success Response → Frontend
```

---

## Implementation Approach

### Phase 1: MVP (Minimum Viable Product)

**Goal**: Basic test data creation with metadata and simple entities

**Features:**
- Metadata editor (name, description, tags)
- Simple entity editor (pool size, single entity)
- Component selector (UI, Grimoire Machina)
- Basic component value editors (text inputs)
- JSON preview
- Save with UUID filename
- Basic validation

**Deliverables:**
- Python backend with Flask
- HTML/CSS/JS frontend
- Basic API endpoints
- File generation working

**Timeline**: 2-3 weeks

### Phase 2: Advanced Features

**Goal**: Complete test data creation capabilities

**Features:**
- Multiple entity support
- Complex component editors (nested UI elements)
- Simulation builder with logic class selection
- Expected snapshot editor
- Enhanced validation with detailed errors
- Load and modify existing files
- Copy/paste entities

**Deliverables:**
- Full component editors
- Simulation workflow
- Enhanced UI/UX
- Comprehensive validation

**Timeline**: 2-3 weeks

### Phase 3: Integration & Polish

**Goal**: Seamless integration with existing tooling and excellent UX

**Features:**
- Integration with test dashboard
- Coverage visualization in creator
- Suggest next tests to write based on gaps
- Keyboard shortcuts
- Undo/redo support
- Auto-save drafts
- Export/import templates
- Documentation and tutorials

**Deliverables:**
- Dashboard integration
- Polish and refinements
- User documentation
- Tutorial videos

**Timeline**: 1-2 weeks

### Phase 4: Optional Enhancements

**Future Considerations:**
- Visual entity layout editor (for UI elements)
- Test execution from creator (run and see results)
- Diff viewer for expected vs actual snapshots
- Test generation from code (analyze Logic classes)
- Template library (common test patterns)
- Collaborative features (share test configurations)

---

## Integration Strategy

### Integration with Existing Test Dashboard

**Shared Components:**
1. **CSS Styles**: Reuse dashboard styles for consistency
2. **Data Models**: Share Python data classes between tools
3. **Schema Parsing**: Use same FlatBuffers parsing logic
4. **File Discovery**: Use same scanner for finding test files

**Navigation Flow:**
```
Test Dashboard → "Create New Test" button → Test Creator
Test Creator → "View in Dashboard" button → Test Dashboard
```

**Coverage Integration:**
- Test creator queries dashboard for current coverage
- Highlights gaps in coverage matrix
- Suggests new test combinations to create
- Shows test execution results inline

### CMake Integration

Add to `cmake/GenerateTestDashboard.cmake` (or new module):

```cmake
# Custom target to launch test creator
add_custom_target(test_creator
    COMMAND ${Python3_EXECUTABLE} -m tools.test_creator
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Launching Test Data Creator..."
)
```

Usage:
```bash
cmake --build --preset Debug --target test_creator
```

### File Organization

```
tools/
├── test_dashboard/       # Existing visualization tool
│   ├── __init__.py
│   ├── parser.py         # Shared with creator
│   ├── scanner.py        # Shared with creator
│   └── ...
│
├── test_creator/         # New creator tool
│   ├── __init__.py
│   ├── __main__.py       # Entry point
│   ├── server.py         # Flask/FastAPI server
│   ├── api/
│   │   ├── routes.py
│   │   ├── validator.py
│   │   └── file_manager.py
│   ├── schema/
│   │   └── loader.py     # FlatBuffers schema parser
│   ├── static/
│   │   ├── css/
│   │   │   └── creator.css
│   │   ├── js/
│   │   │   ├── app.js
│   │   │   ├── metadata-editor.js
│   │   │   ├── entity-editor.js
│   │   │   ├── component-editor.js
│   │   │   └── simulation-builder.js
│   │   └── index.html
│   ├── templates/
│   │   └── index.html    # Main application template
│   └── requirements.txt  # Dependencies
│
└── shared/               # Shared utilities
    ├── __init__.py
    ├── models.py         # Common data models
    └── utils.py          # Utility functions
```

---

## Risk Assessment

### Technical Risks

| Risk | Impact | Likelihood | Mitigation |
|------|--------|------------|------------|
| FlatBuffers schema parsing complexity | High | Medium | Use existing FlatBuffers Python library, start with simple schemas |
| Complex nested UI element editor | High | High | Build incrementally, start with flat structures |
| Browser compatibility issues | Medium | Low | Use standard HTML5/CSS3, test on major browsers |
| Performance with large entity collections | Medium | Medium | Implement virtualization for large lists |
| Validation logic complexity | High | Medium | Reuse existing validation, thorough testing |

### Process Risks

| Risk | Impact | Likelihood | Mitigation |
|------|--------|------------|------------|
| Scope creep | High | High | Strictly follow phased approach, MVP first |
| User adoption | Medium | Medium | Involve users early, gather feedback |
| Maintenance burden | Medium | Medium | Good documentation, clean architecture |
| Integration issues with existing tools | Medium | Low | Early integration testing, shared code |

### Maintenance Considerations

**Long-term maintenance:**
- **Schema Evolution**: Tool must adapt as FlatBuffers schemas change
- **Documentation**: Keep user guide updated
- **Testing**: Unit tests for backend, integration tests for API
- **Monitoring**: Log usage, track errors

**Sustainability:**
- Use standard technologies (Python, HTML/CSS/JS)
- Minimize external dependencies
- Follow project's existing patterns
- Clear code structure for contributors

---

## Recommendations

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

## Appendix

### A. Example Workflows

#### Example 1: Create Simple UI Test

```
Step 1: Metadata
├─ Test name: "UI Button Click Test"
├─ Description: "Verify button responds to click"
├─ Tags: ["unit", "ui", "button"]
└─ Expected to pass: ✓

Step 2: Starting Entities
├─ Pool size: 5
└─ Entity 0:
    └─ CUserInterface:
        ├─ ui_name: "main_menu"
        ├─ is_visible: true
        └─ root_ui_element: PanelData
            ├─ position: {x: 0, y: 0}
            ├─ size: {x: 800, y: 600}
            └─ children: [Button]
                ├─ position: {x: 100, y: 100}
                ├─ size: {x: 200, y: 50}
                └─ label: "Click Me"

Step 3: Simulation
└─ No simulation steps (static test)

Step 4: Expected State
└─ Same as starting state (or define changes)

Step 5: Save
└─ Generated: tests/unit/ui/3f2504e0-4f89-41d3-9a0c-0305e82c3301.test_data.json
```

#### Example 2: Create Simulation Test

```
Step 1: Metadata
├─ Test name: "UI Action Logic Test"
├─ Description: "Test that UIActionLogic processes clicks"
├─ Tags: ["integration", "logic", "ui"]
└─ Expected to pass: ✓

Step 2: Starting Entities
└─ [Same as Example 1]

Step 3: Simulation
├─ Step 1: UICollisionLogic (check mouse position)
├─ Step 2: UIActionLogic (process click event)
└─ Step 3: UIStateLogic (update button state)

Step 4: Expected Snapshots
├─ Tick 0: [Starting state]
├─ Tick 1: [After collision check]
└─ Tick 3: [After action processed]

Step 5: Save
└─ Generated: tests/integration/ui/workflow_001.test_data.json
```

### B. UI Mockup Structure

```
┌─────────────────────────────────────────────────────────┐
│  🧪 Test Data Creator                    [?] [Save] [⚙]│
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌─────────────────┐  ┌──────────────────────────────┐ │
│  │   Navigation    │  │     Content Area             │ │
│  │                 │  │                               │ │
│  │ ► Metadata      │  │  ┏━━━━━━━━━━━━━━━━━━━━━━━┓  │ │
│  │   ✓ Complete    │  │  ┃  Test Name:           ┃  │ │
│  │                 │  │  ┃  [________________]   ┃  │ │
│  │ ► Entities      │  │  ┃                        ┃  │ │
│  │   ⚠ In Progress │  │  ┃  Description:         ┃  │ │
│  │                 │  │  ┃  [________________]   ┃  │ │
│  │ ○ Simulation    │  │  ┃  [________________]   ┃  │ │
│  │   Not Started   │  │  ┃                        ┃  │ │
│  │                 │  │  ┃  Tags: [unit] [x]     ┃  │ │
│  │ ○ Snapshots     │  │  ┃        [___] [Add]    ┃  │ │
│  │   Not Started   │  │  ┗━━━━━━━━━━━━━━━━━━━━━━━┛  │ │
│  │                 │  │                               │ │
│  │ ▼ Preview       │  │  [Continue to Entities →]    │ │
│  │   JSON          │  │                               │ │
│  └─────────────────┘  └──────────────────────────────┘ │
│                                                          │
│  ┌──────────────────────────────────────────────────┐  │
│  │  Preview Panel (Collapsible)                     │  │
│  │  ```json                                          │  │
│  │  {                                                │  │
│  │    "meta_data": {                                │  │
│  │      "test_name": "My Test",                    │  │
│  │      ...                                          │  │
│  │  }                                                │  │
│  │  ```                                              │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

### C. File Naming Convention

**Suggested Naming Pattern:**
```
{category}/{uuid}.test_data.json

Examples:
tests/unit/ui/3f2504e0-4f89-41d3-9a0c-0305e82c3301.test_data.json
tests/integration/logic/7c9e6679-7425-40de-944b-e07fc1f90ae7.test_data.json
```

**Metadata stores human-readable name:**
```json
{
  "meta_data": {
    "test_name": "UI Button Click Test",
    "description": "Verify button responds to click",
    ...
  }
}
```

**Benefits:**
- No naming conflicts
- Easy to generate
- Test dashboard displays human-readable names
- Can move/copy files without breaking references

### D. Schema Introspection Example

How the backend would expose component schemas to frontend:

```python
# GET /api/schema/components/user_interface

{
  "name": "UserInterfaceFbs",
  "fields": [
    {
      "name": "root_ui_element",
      "type": "PanelData",
      "required": true,
      "description": "Root UI element (must be Panel)"
    },
    {
      "name": "ui_name",
      "type": "string",
      "required": false,
      "description": "Name identifier for this UI"
    },
    {
      "name": "is_visible",
      "type": "bool",
      "required": false,
      "default": false,
      "description": "Whether UI is visible on start"
    }
  ]
}
```

Frontend uses this to dynamically generate form fields.

---

**End of Proposal**
