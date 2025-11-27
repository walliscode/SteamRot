# Test Data Dashboard Generator

A Python tool that generates an interactive HTML dashboard for browsing and visualizing SteamRot test data files.

## Status

- ✅ **Phase 1 Complete**: Scanner and Parser modules
- ✅ **Phase 2 Complete**: HTML template, CSS styles, JavaScript interactions
- ⬜ Phase 3: Advanced features (CMake integration)
- ⬜ Phase 4: Polish and documentation

## Quick Start

```bash
# Generate the HTML dashboard
python -m tools.test_dashboard --tests-dir tests/

# Open the generated dashboard in your browser
# File: documentation/generated/test_dashboard.html
```

## Features

### Interactive Dashboard
- **Search**: Full-text search across test names, descriptions, and tags
- **Filters**: Filter by tags, functions, logic classes, and categories
- **Coverage Matrix**: Visual grid showing function × logic class combinations
- **Test Details**: Click any test to view full details including simulation steps
- **Sorting**: Click column headers to sort the test list

### Coverage Matrix
Click any cell in the coverage matrix to filter tests that use that specific function/logic class combination.

## Usage

```bash
# Basic usage - generate HTML dashboard
python -m tools.test_dashboard --tests-dir tests/

# Export parsed data as JSON
python -m tools.test_dashboard --tests-dir tests/ --json output.json

# Custom output location
python -m tools.test_dashboard --tests-dir tests/ --output my_dashboard.html

# Skip HTML generation, only show summary
python -m tools.test_dashboard --tests-dir tests/ --no-html

# Verbose mode
python -m tools.test_dashboard -v
```

## Directory Structure

```
tools/test_dashboard/
├── __init__.py          # Package initialization
├── __main__.py          # Module entry point
├── scanner.py           # File discovery
├── parser.py            # JSON parsing and data models
├── html_generator.py    # HTML generation from templates
├── generate.py          # Main generator
├── templates/
│   └── base.html        # Dashboard HTML template
├── static/
│   ├── styles.css       # Dashboard styles
│   └── dashboard.js     # Interactive JavaScript
├── requirements.txt     # Dependencies
└── README.md            # This file
```

## Output

The tool generates a single self-contained HTML file with embedded CSS and JavaScript.

**Default output**: `documentation/generated/test_dashboard.html`

## Options

| Option | Default | Description |
|--------|---------|-------------|
| `--tests-dir` | `tests/` | Root directory for test data files |
| `--output` | `documentation/generated/test_dashboard.html` | Output HTML file |
| `--json` | None | Export parsed data as JSON |
| `--no-html` | false | Skip HTML generation |
| `--verbose` | false | Enable detailed logging |

## Data Model

### ParsedTest

Each test file is parsed into a `ParsedTest` object:

```python
ParsedTest(
    file_path: str,           # Absolute path
    relative_path: str,       # Path relative to tests/
    category: str,            # e.g., 'harness', 'unit/logic'
    metadata: TestMetadata,   # Name, description, tags, etc.
    simulation_steps: [...],  # List of SimulationStep
    functions_used: [...],    # Unique function types
    logic_classes_used: [...],# Unique logic class types
    num_ticks: int,
    has_input_sequence: bool,
    has_event_sequence: bool,
    has_tick_snapshots: bool,
    has_start_data: bool,
    has_expected_data: bool,
    raw_data: dict            # Original JSON
)
```

## Contributing

See the [Test Data Visualization Report](../../documentation/analysis/TEST_DATA_VISUALIZATION_REPORT.md) for the full implementation plan and design decisions.
