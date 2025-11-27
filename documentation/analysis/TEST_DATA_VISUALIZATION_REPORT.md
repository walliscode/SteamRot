# Test Data Visualization Report

[← Back to Documentation](../README.md) | [Test Data Configuration](../testing/TEST_DATA_CONFIGURATION.md)

## Executive Summary

This report analyzes **8 different approaches** for creating a visual UI to search, browse, and visualize the test_data.json files in the SteamRot project. As the test suite grows (currently 25+ test data files), manually searching through files to understand test coverage becomes increasingly difficult. A visualization tool would allow users to:

1. **Search** - Find tests by function, logic class, tags, or any metadata field
2. **Browse** - Navigate test data in a structured, hierarchical manner
3. **Visualize** - See test coverage as graphs or matrices showing function/logic class permutations
4. **Discover** - Identify gaps in testing and find related test cases

**Key Finding**: The recommended approach is a **hybrid Python+HTML solution** that combines:
- **Python** (like Option 4) for file scanning and data aggregation
- **Browser-based JavaScript** (like Option 2) for rich interactive visualization

This hybrid provides dropdown filters, clickable coverage matrices, and drill-down views without requiring a server. Tkinter (Option 8) is a viable pure-Python alternative if web technologies are undesired, though with less visual polish.

---

## Table of Contents

- [Executive Summary](#executive-summary)
- [Current State Analysis](#current-state-analysis)
- [Requirements](#requirements)
- [Technology Options Analysis](#technology-options-analysis)
- [Recommended Approach](#recommended-approach)
- [Alternative Approaches](#alternative-approaches)
- [Implementation Phases](#implementation-phases)
- [Decision Matrix](#decision-matrix)
- [Conclusion](#conclusion)

---

## Current State Analysis

### Test Data Structure

Each test_data.json file contains:

```json
{
  "metadata": {
    "test_name": "string (required)",
    "description": "string",
    "tags": ["array", "of", "strings"],
    "expected_to_pass": true/false,
    "author": "string",
    "version": 1
  },
  "simulation_data": {
    "description": "string",
    "steps": [
      {
        "simulation_type": "Action|Movement|Render|Collision",
        "execution_mode": "Function|LogicClass",
        "function_type": "FunctionName (if Function)",
        "logic_class_type": "ClassName (if LogicClass)",
        "description": "string"
      }
    ]
  },
  "input_sequence": { /* tick-by-tick input injection */ },
  "event_sequence": { /* tick-by-tick event injection */ },
  "start_data_collection": { /* initial entity/event state */ },
  "expected_data_collection": { /* expected final state */ },
  "tick_snapshots": [ /* intermediate checkpoints */ ],
  "num_ticks": 1
}
```

### Key Data Points for Visualization

| Data Point | Description | Visualization Use |
|------------|-------------|-------------------|
| `metadata.test_name` | Test identifier | Primary search/filter |
| `metadata.tags` | Categorization | Faceted filtering |
| `metadata.description` | What test validates | Full-text search |
| `simulation_data.steps[].function_type` | Functions tested | Coverage matrix |
| `simulation_data.steps[].logic_class_type` | Logic classes tested | Coverage matrix |
| `simulation_data.steps[].simulation_type` | System category | Grouping/hierarchy |
| `input_sequence.inputs[]` | User inputs simulated | Input coverage |
| `event_sequence.events[]` | Events injected | Event coverage |
| `num_ticks` | Test complexity | Complexity metrics |

### Current File Organization

```
tests/
├── harness/data/           # 16+ files (infrastructure tests)
│   ├── harness_basic_001.test_data.json
│   ├── harness_simulation_001.test_data.json
│   └── ...
├── unit/logic/data/        # 9+ files (logic unit tests)
│   ├── ui_collision_logic_basic.test_data.json
│   ├── ui_visibility_from_state.test_data.json
│   └── ...
├── unit/entity/data/       # 4+ files (entity tests)
│   ├── pool_comparison_equal.test_data.json
│   └── ...
└── integration/            # Future growth area
```

### Available Functions (from simulation.fbs)

**FunctionType enum values:**
- `ProcessUIActionsAndEvents`
- `ProcessNestedUIActionsAndEvents`
- `ProcessButtonElementActions`
- `ProcessDropDownListElementActions`
- `CheckMouseOverNestedUIElement`
- `UpdateCUserInterfaceVisibilityFromCUIState`

**LogicClassType enum values:**
- `UIActionLogic`
- `UICollisionLogic`
- `UIRenderLogic`
- `UIStateLogic`
- `CraftingRenderLogic`

---

## Requirements

### Functional Requirements

1. **Search & Filter**
   - Search by test name, description, tags
   - Filter by function type, logic class type
   - Filter by simulation type (Action, Movement, Render, Collision)
   - Filter by expected_to_pass value

2. **Browse**
   - Tree view by directory structure
   - Grouped view by tags, functions, or logic classes
   - Sortable list view

3. **Visualize**
   - Coverage matrix: functions vs. logic classes
   - Graph view: connections between tests and components
   - Timeline/tick visualization for multi-tick tests

4. **Drill-down**
   - Click to view full test details
   - See simulation steps in order
   - View input/event sequences

### Non-Functional Requirements

- **Lightweight**: No heavy infrastructure requirements
- **Fast startup**: Should work instantly, not require server spin-up time
- **Offline-capable**: Works without internet connection
- **Low maintenance**: Minimal updates needed as test data evolves
- **Cross-platform**: Works on Windows, macOS, Linux
- **Easy to extend**: Simple to add new visualization features

---

## Technology Options Analysis

### Option 1: Python + Flask Web Application

**Overview**: Python backend with Flask web server, serving HTML/CSS/JS frontend.

**Pros:**
- ✅ Familiar Python ecosystem
- ✅ Rich JSON parsing libraries
- ✅ Can serve static HTML with dynamic data
- ✅ Good charting libraries (Plotly, Chart.js integration)
- ✅ Easy to add API endpoints for filtering
- ✅ Can be containerized with Docker

**Cons:**
- ❌ Requires server process running
- ❌ Extra dependency (Python environment)
- ❌ More complex deployment for team members
- ❌ Overkill for static data browsing

**Best for**: Teams already using Python, needing complex server-side processing.

**Complexity**: Medium

---

### Option 2: Static HTML + JavaScript

**Overview**: Single HTML file with embedded JavaScript that loads and parses JSON files.

**Pros:**
- ✅ **Zero dependencies** - works in any browser
- ✅ **Instant startup** - just open the file
- ✅ **Offline-capable** - no server needed
- ✅ **Single file distribution** - easy to share
- ✅ **Low maintenance** - no backend to maintain
- ✅ **Cross-platform** - works everywhere
- ✅ Rich visualization with D3.js, Chart.js (loaded via CDN or embedded)

**Cons:**
- ❌ Limited to client-side file reading (requires file input or hardcoded paths)
- ❌ Cannot automatically scan directories (needs file list)
- ❌ Large embedded libraries increase file size

**Best for**: Simplest possible solution, maximum portability.

**Complexity**: Low

#### User Navigation Capabilities

A static HTML + JavaScript solution **fully supports interactive navigation**:

1. **Navigation Features Available:**
   - **Dropdown menus**: Select starting function/logic class to filter tests
   - **Clickable coverage matrix**: Click any cell to see tests covering that combination
   - **Searchable table**: Real-time filtering as user types
   - **Tree navigation**: Expandable/collapsible test categories
   - **Drill-down views**: Click any test to see full details
   - **Back/forward browser navigation**: History API integration
   - **Bookmarkable URLs**: Hash-based routing for sharing specific views

2. **Visualization Libraries:**
   - **D3.js**: Force-directed graphs showing function/logic relationships
   - **Chart.js**: Bar charts, pie charts for coverage statistics
   - **Vis.js**: Network graphs, timelines for simulation steps
   - **DataTables**: Rich sortable, filterable tables

3. **Key Limitation - File Loading:**
   The main challenge is loading the JSON test data files. Options:
   
   - **Pre-embedded data**: Python script embeds all JSON as JavaScript objects (recommended)
   - **File input dialog**: User selects files via `<input type="file">`
   - **Fetch API**: Requires local server or CORS-enabled hosting
   
   The **hybrid approach** (Python generates HTML with embedded data) solves this limitation while keeping full interactivity.

#### Sample Architecture for Interactive Navigation

```html
<!-- Navigation structure -->
<nav id="main-nav">
  <select id="function-filter">
    <option value="">All Functions</option>
    <option value="ProcessUIActionsAndEvents">ProcessUIActionsAndEvents</option>
    <!-- ... -->
  </select>
  
  <select id="logic-class-filter">
    <option value="">All Logic Classes</option>
    <option value="UICollisionLogic">UICollisionLogic</option>
    <!-- ... -->
  </select>
</nav>

<!-- Interactive coverage matrix -->
<table id="coverage-matrix">
  <tr onclick="showTestsForCell(this)">...</tr>
</table>

<!-- Drill-down panel -->
<div id="test-details" class="panel">
  <!-- Shows when user clicks a test -->
</div>

<script>
// All test data embedded by Python generator
const TEST_DATA = [/* generated */];

// Interactive filtering
function filterByFunction(funcName) {
  return TEST_DATA.filter(t => t.functions.includes(funcName));
}

// Graph visualization
function renderCoverageGraph() {
  // D3.js force-directed graph
}
</script>
```

---

### Option 3: Node.js + Express Application

**Overview**: JavaScript backend with Express server, React/Vue frontend.

**Pros:**
- ✅ Same language for backend and frontend
- ✅ NPM ecosystem for visualization libraries
- ✅ Real-time updates with WebSockets
- ✅ Modern development experience

**Cons:**
- ❌ Requires Node.js installed
- ❌ More complex build pipeline
- ❌ Server process required
- ❌ Higher learning curve for non-JS developers

**Best for**: Teams comfortable with JavaScript, wanting modern React/Vue UI.

**Complexity**: Medium-High

---

### Option 4: Python Script Generating Static HTML

**Overview**: Python script that scans test_data.json files and generates a static HTML report.

**Pros:**
- ✅ **One-time generation** - no server needed after generation
- ✅ Can scan directories automatically
- ✅ Integrates with build system (CMake can run it)
- ✅ Output is portable static HTML
- ✅ Python easily parses JSON
- ✅ Can be run as part of CI/CD

**Cons:**
- ❌ Requires regeneration when tests change
- ❌ Requires Python to generate (but not to view)
- ❌ Static output - no dynamic filtering without JavaScript

**Best for**: Integration with build system, automatic catalog generation.

**Complexity**: Low-Medium

---

### Option 5: Electron Desktop Application

**Overview**: Cross-platform desktop app using web technologies.

**Pros:**
- ✅ Full filesystem access (can scan directories)
- ✅ Rich UI capabilities
- ✅ Offline by default
- ✅ Single distributable package

**Cons:**
- ❌ Large application size (~100MB)
- ❌ Complex build and distribution
- ❌ Overkill for data browsing
- ❌ Updates require redistribution

**Best for**: Full-featured desktop application needs.

**Complexity**: High

---

### Option 6: Python + Streamlit

**Overview**: Python framework for creating data apps quickly.

**Pros:**
- ✅ Extremely fast development
- ✅ Built-in data visualization
- ✅ Interactive widgets out of the box
- ✅ Python ecosystem integration
- ✅ Hot reloading during development

**Cons:**
- ❌ Requires server process
- ❌ Streamlit dependency
- ❌ Less customizable UI
- ❌ May feel limited for complex interactions

**Best for**: Quick prototyping, data exploration focus.

**Complexity**: Low

---

### Option 7: VS Code Extension

**Overview**: Extension for Visual Studio Code providing integrated visualization.

**Pros:**
- ✅ Integrated into development workflow
- ✅ Access to workspace files
- ✅ Rich extension API
- ✅ Already installed by many developers

**Cons:**
- ❌ Tied to VS Code users
- ❌ Extension development has learning curve
- ❌ Requires publishing/maintenance
- ❌ TypeScript/JavaScript required

**Best for**: Teams heavily using VS Code.

**Complexity**: Medium

---

### Option 8: Python + Tkinter Desktop Application

**Overview**: Native desktop GUI application using Python's built-in Tkinter library.

**Pros:**
- ✅ **No external dependencies** - Tkinter is included with Python
- ✅ **Full filesystem access** - can automatically scan directories
- ✅ **Native desktop experience** - feels like a real application
- ✅ **Already using Python** - if Python generator is needed anyway, why not use it for UI too
- ✅ **Live updates** - can watch for file changes and auto-refresh
- ✅ **Single script distribution** - just share the .py file
- ✅ **Works offline** - no browser or server needed

**Cons:**
- ❌ **Dated visual appearance** - Tkinter looks less modern than web UIs
- ❌ **Limited visualization libraries** - no D3.js, Chart.js equivalents built-in
- ❌ **Requires Python runtime** - users must have Python installed
- ❌ **Platform inconsistencies** - may look different on Windows/Mac/Linux
- ❌ **Harder to create rich visualizations** - graphs and charts require matplotlib integration
- ❌ **No easy sharing** - can't bookmark/share specific views via URL

**Best for**: Simple tree/list navigation, when Python is already a requirement.

**Complexity**: Low-Medium

#### Tkinter vs Hybrid Python+HTML Comparison

Since Python is already being used, here's a direct comparison:

| Feature | Tkinter | Python + HTML (Recommended) |
|---------|---------|---------------------------|
| Dependency | Python only | Python + Browser |
| File scanning | ✅ Native | ✅ Native |
| Rich visualizations | ❌ Limited (matplotlib) | ✅ D3.js, Chart.js |
| Modern look | ❌ Dated | ✅ Modern CSS |
| Shareability | ❌ Script only | ✅ HTML file + URLs |
| Development speed | Medium | Medium |
| Coverage matrix | Basic table | Interactive clickable grid |
| Graph views | Requires canvas work | D3.js force graphs |
| Search/filter | TreeView + Entry | DataTables built-in |

**Recommendation**: The hybrid approach (Python generates HTML) gives the best of both worlds:
- Python handles file scanning and data aggregation
- HTML/JavaScript provides rich, modern visualization
- Output is a single portable HTML file

#### Sample Tkinter Implementation

```python
#!/usr/bin/env python3
"""
test_explorer_tkinter.py - Tkinter-based test data explorer
"""
import tkinter as tk
from tkinter import ttk
import json
from pathlib import Path

class TestExplorer:
    def __init__(self, root):
        self.root = root
        self.root.title("SteamRot Test Data Explorer")
        self.root.geometry("1200x800")
        
        # Left panel - Tree navigation
        self.tree_frame = ttk.Frame(root)
        self.tree_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=False)
        
        self.tree = ttk.Treeview(self.tree_frame, columns=("tags", "ticks"))
        self.tree.heading("#0", text="Test Name")
        self.tree.heading("tags", text="Tags")
        self.tree.heading("ticks", text="Ticks")
        self.tree.pack(fill=tk.BOTH, expand=True)
        self.tree.bind("<<TreeviewSelect>>", self.on_select)
        
        # Right panel - Details
        self.details_frame = ttk.Frame(root)
        self.details_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)
        
        self.details_text = tk.Text(self.details_frame, wrap=tk.WORD)
        self.details_text.pack(fill=tk.BOTH, expand=True)
        
        # Filter controls
        self.filter_frame = ttk.Frame(root)
        self.filter_frame.pack(side=tk.TOP, fill=tk.X)
        
        ttk.Label(self.filter_frame, text="Function:").pack(side=tk.LEFT)
        self.function_combo = ttk.Combobox(self.filter_frame, values=["All", "ProcessUIActionsAndEvents", "..."])
        self.function_combo.pack(side=tk.LEFT)
        self.function_combo.bind("<<ComboboxSelected>>", self.on_filter)
        
        # Load tests
        self.load_tests()
    
    def load_tests(self):
        """Scan and load all test_data.json files"""
        tests_dir = Path("tests")
        for file in tests_dir.rglob("*.test_data.json"):
            with open(file) as f:
                data = json.load(f)
            name = data["metadata"]["test_name"]
            tags = ", ".join(data["metadata"].get("tags", []))
            ticks = data.get("num_ticks", 1)
            self.tree.insert("", tk.END, text=name, values=(tags, ticks), tags=(str(file),))
    
    def on_select(self, event):
        """Show details when test is selected"""
        item = self.tree.selection()[0]
        file_path = self.tree.item(item, "tags")[0]
        with open(file_path) as f:
            data = json.load(f)
        self.details_text.delete(1.0, tk.END)
        self.details_text.insert(tk.END, json.dumps(data, indent=2))
    
    def on_filter(self, event):
        """Filter tree based on selected function"""
        # Implement filtering logic
        pass

if __name__ == "__main__":
    root = tk.Tk()
    app = TestExplorer(root)
    root.mainloop()
```

**Verdict**: Tkinter is a viable option if you prefer a pure-Python solution and don't need rich visualizations. However, the hybrid Python+HTML approach provides better visualizations and shareability with similar development effort.

---

## Recommended Approach

### Primary Recommendation: Python Script + Static HTML (Option 4)

**Why this approach?**

1. **Aligns with existing infrastructure**
   - Documentation mentions future tooling: `./tools/generate_test_catalog.sh`
   - Already have CMake integration for FlatBuffers compilation
   - Python is commonly available on development machines

2. **Minimal maintenance burden**
   - Generated HTML is static - no server to maintain
   - Script can be run manually or integrated with build
   - No external dependencies for viewing output

3. **Progressive enhancement**
   - Start with simple catalog/table
   - Add filtering with JavaScript
   - Add visualizations incrementally

4. **Cross-platform**
   - Python script runs anywhere
   - HTML output works in any browser

### Implementation Architecture

```
tools/
└── generate_test_dashboard.py    # Main generator script

documentation/
└── generated/
    └── test_dashboard.html       # Generated output (gitignored or committed)

tests/
├── harness/data/*.test_data.json
├── unit/logic/data/*.test_data.json
└── ...
```

### Generated HTML Structure

```html
<!DOCTYPE html>
<html>
<head>
  <title>SteamRot Test Data Dashboard</title>
  <style>/* Embedded CSS */</style>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
  <header>
    <h1>Test Data Dashboard</h1>
    <input type="text" id="search" placeholder="Search tests...">
  </header>
  
  <aside id="filters">
    <!-- Filter checkboxes for tags, functions, logic classes -->
  </aside>
  
  <main>
    <section id="coverage-matrix">
      <!-- Function vs Logic Class coverage grid -->
    </section>
    
    <section id="test-list">
      <!-- Filterable, sortable test table -->
    </section>
    
    <section id="test-details" hidden>
      <!-- Drill-down view for selected test -->
    </section>
  </main>
  
  <script>
    // Embedded test data (generated)
    const TEST_DATA = [
      { /* parsed test_data.json contents */ }
    ];
    
    // Filtering and visualization logic
  </script>
</body>
</html>
```

### Key Features

1. **Coverage Matrix**
   ```
                  UIActionLogic  UICollisionLogic  UIRenderLogic  UIStateLogic
   ProcessUI...       ✓✓✓            ✓                              ✓
   ProcessNested...   ✓              ✓✓                             
   ProcessButton...   ✓✓✓✓           ✓✓              ✓              ✓✓
   ```
   - Click cell to see tests that cover that combination

2. **Searchable Test List**
   - Full-text search across name, description, tags
   - Sort by name, tags, num_ticks, date modified

3. **Filter Panel**
   - Checkboxes for each tag
   - Checkboxes for each function type
   - Checkboxes for each logic class type
   - Range slider for num_ticks

4. **Test Details View**
   - Full metadata display
   - Simulation steps timeline
   - Input/event sequence visualization
   - Entity state diff (start vs expected)

---

## Alternative Approaches

### Alternative 1: Streamlit (Quick Prototype)

If you want the fastest path to a working prototype, Streamlit is excellent:

```python
# tools/test_dashboard.py
import streamlit as st
import json
from pathlib import Path

st.title("SteamRot Test Data Dashboard")

# Sidebar filters
tags = st.sidebar.multiselect("Tags", ["unit", "integration", "simulation", "ui"])
logic_classes = st.sidebar.multiselect("Logic Classes", ["UIActionLogic", "UICollisionLogic"])

# Load and display data
test_files = list(Path("tests").rglob("*.test_data.json"))
for file in test_files:
    data = json.load(open(file))
    if matches_filters(data, tags, logic_classes):
        st.expander(data["metadata"]["test_name"]).json(data)
```

Run with: `streamlit run tools/test_dashboard.py`

**Pros**: 10-minute prototype, built-in charts, interactive
**Cons**: Requires `streamlit` installed, server process

### Alternative 2: Flask with Modern Frontend

For a more polished web application:

```python
# tools/server.py
from flask import Flask, jsonify, render_template
from pathlib import Path
import json

app = Flask(__name__)

@app.route("/")
def dashboard():
    return render_template("dashboard.html")

@app.route("/api/tests")
def get_tests():
    tests = []
    for file in Path("tests").rglob("*.test_data.json"):
        tests.append(json.load(open(file)))
    return jsonify(tests)

@app.route("/api/tests/<path:name>")
def get_test(name):
    # Return specific test
    pass
```

**Pros**: Clean API, separation of concerns, scalable
**Cons**: More setup, requires Flask running

### Alternative 3: Interactive Jupyter Notebook

For data exploration without building a full UI:

```python
# exploration.ipynb
import pandas as pd
import plotly.express as px
from pathlib import Path
import json

# Load all test data
tests = []
for file in Path("tests").rglob("*.test_data.json"):
    data = json.load(open(file))
    data["file"] = str(file)
    tests.append(data)

# Convert to DataFrame for analysis
df = pd.json_normalize(tests)

# Visualize coverage
fig = px.scatter(df, x="simulation_type", y="logic_class_type", 
                 color="metadata.tags", size="num_ticks")
fig.show()
```

**Pros**: Interactive exploration, easy to modify, good for analysis
**Cons**: Requires Jupyter, not a standalone tool

---

## Implementation Phases

### Phase 1: Basic Catalog (1-2 days)

**Goal**: Generate a searchable HTML table of all tests

**Deliverables**:
- Python script that scans test_data.json files
- HTML output with sortable table
- Basic search functionality

**Features**:
- Test name, description, tags columns
- Click row to expand details
- Text search across all fields

### Phase 2: Filtering & Coverage (2-3 days)

**Goal**: Add filtering and coverage visualization

**Deliverables**:
- Filter panel for tags, functions, logic classes
- Coverage matrix showing function/logic combinations
- Interactive filtering updates matrix and table

**Features**:
- Multi-select filters
- Coverage heatmap
- Click cell to filter to matching tests

### Phase 3: Detailed Views (2-3 days)

**Goal**: Rich visualization of individual tests

**Deliverables**:
- Simulation step timeline
- Input/event sequence visualization
- Entity state comparison viewer

**Features**:
- Step-by-step simulation walkthrough
- Tick-by-tick input/event display
- Diff view for entity changes

### Phase 4: Integration (1 day)

**Goal**: Integrate with build system

**Deliverables**:
- CMake target to generate dashboard
- README updates
- CI integration (optional)

**Features**:
- `cmake --build --preset Debug --target generate_test_dashboard`
- Auto-regeneration when test files change (optional)

---

## Decision Matrix

| Criteria | Flask | Static HTML | Streamlit | Electron | VS Code Ext | Tkinter |
|----------|-------|-------------|-----------|----------|-------------|---------|
| Setup Complexity | Medium | **Low** | Low | High | Medium | **Low** |
| Dependencies | Python, Flask | **None** | Python, Streamlit | Node, Electron | VS Code, Node | Python |
| Maintenance | Medium | **Low** | Low | High | Medium | **Low** |
| Offline Support | Partial | **Full** | Partial | Full | Full | **Full** |
| Development Speed | Fast | Medium | **Fast** | Slow | Medium | Medium |
| Customization | High | **High** | Limited | High | Medium | Medium |
| Distribution | Server | **File share** | Server | Installer | Marketplace | Script |
| Visualization | Good | **Excellent** | Excellent | Good | Limited | Limited |
| File Scanning | Good | None | Good | **Full** | Full | **Full** |
| Integration | Good | **Good** | Limited | Limited | Excellent | Good |

**Legend**: **Bold** = best in category

### Recommendation Summary

| Priority | Option | Why |
|----------|--------|-----|
| **1st** | Hybrid: Python Generator + Static HTML | Python scans files, generates interactive HTML with full navigation |
| **2nd** | Tkinter (Option 8) | Pure Python if browser-based UI is undesired |
| **3rd** | Streamlit (Option 6) | Quick prototype for immediate exploration |

---

## Conclusion

### Primary Recommendation

**Python Script Generating Static HTML** provides the best balance of:
- Low complexity and maintenance
- No runtime dependencies for viewers
- Good visualization capabilities
- Integration with existing build system
- Progressive enhancement path

### Quick Start Steps

1. Create `tools/generate_test_dashboard.py`
2. Scan all `*.test_data.json` files recursively
3. Parse and normalize data
4. Generate `documentation/generated/test_dashboard.html`
5. Add CMake target for regeneration
6. Add to `.gitignore` or commit generated file

### Alternative Quick Prototype

For immediate exploration before building the full solution:
1. Install Streamlit: `pip install streamlit`
2. Create `tools/test_explorer.py` (20 lines of code)
3. Run: `streamlit run tools/test_explorer.py`
4. Iterate on requirements before building static generator

### Long-term Considerations

As the test suite grows beyond 100+ files, consider:
- Database backend (SQLite) for faster querying
- Full-text search indexing
- CI-generated dashboard published to GitHub Pages
- Integration with test execution for pass/fail status

---

## Appendix A: Sample Generator Script Outline

> **Note**: This is a conceptual outline to illustrate the approach. Production implementation should include:
> - Command-line argument parsing for input/output paths
> - Proper error handling with descriptive messages
> - Logging for debugging
> - Configuration file support

```python
#!/usr/bin/env python3
"""
generate_test_dashboard.py - Generate static HTML dashboard for test_data.json files

Usage:
    python tools/generate_test_dashboard.py
    
Output:
    documentation/generated/test_dashboard.html
"""

import json
from pathlib import Path
from collections import defaultdict
from typing import Dict, List, Any

def find_test_files(tests_dir: Path) -> List[Path]:
    """Recursively find all .test_data.json files."""
    return list(tests_dir.rglob("*.test_data.json"))

def parse_test_file(file_path: Path) -> Dict[str, Any]:
    """Parse a test data JSON file and extract key fields."""
    with open(file_path) as f:
        data = json.load(f)
    
    return {
        "file": str(file_path),
        "name": data["metadata"]["test_name"],
        "description": data["metadata"].get("description", ""),
        "tags": data["metadata"].get("tags", []),
        "expected_to_pass": data["metadata"].get("expected_to_pass", True),
        "functions": extract_functions(data),
        "logic_classes": extract_logic_classes(data),
        "num_ticks": data.get("num_ticks", 1),
        "has_inputs": "input_sequence" in data,
        "has_events": "event_sequence" in data,
        "has_simulation": "simulation_data" in data,
        "raw": data
    }

def extract_functions(data: Dict) -> List[str]:
    """Extract function types from simulation steps."""
    functions = []
    if "simulation_data" in data and "steps" in data["simulation_data"]:
        for step in data["simulation_data"]["steps"]:
            if step.get("execution_mode") == "Function":
                func = step.get("function_type")
                if func and func != "None":
                    functions.append(func)
    return list(set(functions))

def extract_logic_classes(data: Dict) -> List[str]:
    """Extract logic class types from simulation steps."""
    classes = []
    if "simulation_data" in data and "steps" in data["simulation_data"]:
        for step in data["simulation_data"]["steps"]:
            if step.get("execution_mode") == "LogicClass":
                cls = step.get("logic_class_type")
                if cls and cls != "None":
                    classes.append(cls)
    return list(set(classes))

def build_coverage_matrix(tests: List[Dict]) -> Dict[str, Dict[str, List[str]]]:
    """Build function vs logic class coverage matrix."""
    matrix = defaultdict(lambda: defaultdict(list))
    for test in tests:
        for func in test["functions"] or ["(none)"]:
            for cls in test["logic_classes"] or ["(none)"]:
                matrix[func][cls].append(test["name"])
    return matrix

def generate_html(tests: List[Dict], matrix: Dict) -> str:
    """Generate the HTML dashboard."""
    # Template with embedded CSS and JS
    return f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>SteamRot Test Data Dashboard</title>
    <!-- CSS styles here -->
</head>
<body>
    <h1>Test Data Dashboard</h1>
    <p>Generated from {len(tests)} test files</p>
    
    <!-- Search and filters -->
    <input type="text" id="search" placeholder="Search...">
    
    <!-- Coverage matrix -->
    <h2>Coverage Matrix</h2>
    <!-- Generate matrix table -->
    
    <!-- Test list -->
    <h2>Tests</h2>
    <table id="test-table">
        <thead>
            <tr>
                <th>Name</th>
                <th>Tags</th>
                <th>Functions</th>
                <th>Logic Classes</th>
                <th>Ticks</th>
            </tr>
        </thead>
        <tbody>
            <!-- Generate rows from tests -->
        </tbody>
    </table>
    
    <script>
        const TEST_DATA = {json.dumps(tests, indent=2)};
        // Add filtering, searching, drill-down logic
    </script>
</body>
</html>"""

def main():
    tests_dir = Path("tests")
    output_file = Path("documentation/generated/test_dashboard.html")
    
    # Find and parse all test files
    test_files = find_test_files(tests_dir)
    tests = [parse_test_file(f) for f in test_files]
    
    # Build coverage matrix
    matrix = build_coverage_matrix(tests)
    
    # Generate HTML
    html = generate_html(tests, matrix)
    
    # Write output
    output_file.parent.mkdir(parents=True, exist_ok=True)
    output_file.write_text(html)
    
    print(f"Generated dashboard with {len(tests)} tests: {output_file}")

if __name__ == "__main__":
    main()
```

---

## Appendix B: Sample Streamlit Explorer

```python
#!/usr/bin/env python3
"""
test_explorer.py - Quick Streamlit-based test data explorer

Usage:
    pip install streamlit
    streamlit run tools/test_explorer.py
"""

import streamlit as st
import json
from pathlib import Path
from collections import Counter

st.set_page_config(page_title="Test Data Explorer", layout="wide")
st.title("🧪 SteamRot Test Data Explorer")

# Load all test files
@st.cache_data
def load_tests():
    tests = []
    for file in Path("tests").rglob("*.test_data.json"):
        try:
            data = json.load(open(file))
            data["_file"] = str(file)
            tests.append(data)
        except Exception as e:
            st.error(f"Error loading {file}: {e}")
    return tests

tests = load_tests()
st.sidebar.info(f"Loaded {len(tests)} test files")

# Sidebar filters
st.sidebar.header("Filters")

# Collect all tags
all_tags = set()
for t in tests:
    all_tags.update(t.get("metadata", {}).get("tags", []))
selected_tags = st.sidebar.multiselect("Tags", sorted(all_tags))

# Filter tests
filtered = tests
if selected_tags:
    filtered = [t for t in filtered 
                if any(tag in t.get("metadata", {}).get("tags", []) 
                       for tag in selected_tags)]

st.header(f"Tests ({len(filtered)} matching)")

# Display tests
for test in filtered:
    meta = test.get("metadata", {})
    with st.expander(f"📋 {meta.get('test_name', 'Unknown')}"):
        st.write(f"**Description:** {meta.get('description', 'No description')}")
        st.write(f"**Tags:** {', '.join(meta.get('tags', []))}")
        st.write(f"**Expected to pass:** {meta.get('expected_to_pass', True)}")
        st.write(f"**File:** `{test.get('_file', 'Unknown')}`")
        
        if "simulation_data" in test:
            st.subheader("Simulation Steps")
            for i, step in enumerate(test["simulation_data"].get("steps", [])):
                st.write(f"{i+1}. [{step.get('simulation_type')}] "
                        f"{step.get('execution_mode')}: "
                        f"{step.get('function_type') or step.get('logic_class_type')}")
        
        st.subheader("Raw JSON")
        st.json(test)
```

---

## Appendix C: Future Enhancements

### 1. GitHub Pages Integration
- Automatically generate and publish dashboard to GitHub Pages
- CI workflow regenerates on test file changes
- Link from README

### 2. Test Execution Integration
- Color-code tests by last run status (pass/fail)
- Show execution time metrics
- Link to CI logs

### 3. Coverage Gaps Analysis
- Highlight function/logic combinations with no tests
- Suggest missing test scenarios
- Track coverage over time

### 4. Graph Visualization
- D3.js force-directed graph
- Nodes = functions/logic classes
- Edges = tests covering both
- Cluster by simulation type

### 5. Test Template Generator
- Select function + logic class combination
- Generate skeleton test_data.json
- Pre-fill metadata based on selection

---

**Last Updated**: 2025-11-26  
**Version**: 1.0
