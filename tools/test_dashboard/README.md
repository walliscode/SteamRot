# Test Data Dashboard Generator

A Python tool that generates an interactive HTML dashboard for browsing and visualizing SteamRot test data files.

## Status

- ✅ **Phase 1 Complete**: Scanner and Parser modules
- ⬜ Phase 2: HTML template and CSS
- ⬜ Phase 3: JavaScript interactions
- ⬜ Phase 4: CMake integration

## Quick Start

```bash
# Run the scanner/parser (Phase 1)
python -m tools.test_dashboard --tests-dir tests/

# Export parsed data as JSON
python -m tools.test_dashboard --tests-dir tests/ --json output.json

# Verbose mode
python -m tools.test_dashboard --tests-dir tests/ -v
```

## Current Output (Phase 1)

The tool currently scans all `*.test_data.json` files and displays:
- Total test count
- Unique tags, functions, and logic classes
- Tests organized by category
- Coverage statistics

## Directory Structure

```
tools/test_dashboard/
├── __init__.py      # Package initialization
├── __main__.py      # Module entry point
├── scanner.py       # File discovery
├── parser.py        # JSON parsing and data models
├── generate.py      # Main generator (Phase 2+)
├── requirements.txt # Dependencies
└── README.md        # This file
```

## Features

### Implemented (Phase 1)

- **TestFileScanner**: Recursively finds `*.test_data.json` files
- **TestDataParser**: Parses JSON and extracts:
  - Metadata (name, description, tags, author, version)
  - Simulation steps (function types, logic classes)
  - Data presence flags (input sequences, events, tick snapshots)
- **JSON Export**: Export all parsed data for external tools
- **Category Detection**: Organizes tests by directory structure

### Planned (Phase 2+)

- Interactive HTML dashboard
- Coverage matrix (function × logic class)
- Search and filter functionality
- Test detail drill-down views
- CMake integration

## Options

| Option | Default | Description |
|--------|---------|-------------|
| `--tests-dir` | `tests/` | Root directory for test data files |
| `--output` | `documentation/generated/test_dashboard.html` | Output HTML file (Phase 2) |
| `--json` | None | Export parsed data as JSON |
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
