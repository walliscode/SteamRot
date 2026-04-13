# SteamRot Wiki

Welcome to the SteamRot game engine wiki. Use the navigation on the left to browse
conceptual documentation, workflow guides, and design notes.

## What is SteamRot?

SteamRot is a data-driven C++ game engine built with:

- **SFML** — graphics, windowing, and input
- **FlatBuffers** — data serialization and runtime configuration
- **Catch2** — unit and integration testing
- **spdlog** — structured logging
- **C++23** — modern language features throughout

## Quick Links

| Section | Description |
|---------|-------------|
| [Workflows](workflows/ADDING_LOGIC.md) | Step-by-step guides for common development tasks |
| [Design](design/LOGIC_DECOMPOSITION.md) | Architectural decisions and design notes |
| [Coordinate Spaces](design/COORDINATE_SPACES.md) | Screen space vs world space — concepts and mechanics |
| [Coordinate Spaces in SteamRot](design/COORDINATE_SPACES_STEAMROT.md) | How the codebase handles coordinate conversion and pitfalls |

## Getting Started

### Build

```bash
cmake --preset Debug
cmake --build --preset Debug
```

### Run tests

```bash
ctest --preset Debug
```

### Generate test dashboard

```bash
# Activate the virtual environment first (see "Serve this wiki locally" below)
python -m tools.test_dashboard --tests-dir tests/
```

### Serve this wiki locally

```bash
# Activate the virtual environment (once per shell session)
source .venv/bin/activate   # macOS/Linux
# .venv\Scripts\activate    # Windows

# Install dependencies (once)
pip install -r tools/wiki/requirements.txt

# Start the local wiki server
mkdocs serve
```
