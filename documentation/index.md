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
| [Coordinate Spaces](concepts/COORDINATE_SPACES.md) | Screen space vs world space — concepts and mechanics |
| [Coordinate Spaces in SteamRot](concepts/COORDINATE_SPACES_STEAMROT.md) | How the codebase handles coordinate conversion and pitfalls |
| [GrimoireMachina Logic Map](design/GRIMOIRE_MACHINA_LOGIC_MAP.md) | Audit and responsibility map for all GrimoireMachina logic (Collision / Action / Positioning / Render) |

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
# Activate a Python virtual environment first
python -m tools.test_dashboard --tests-dir tests/
```

### Generate and view docs locally

```bash
# Install Doxygen (once)
sudo apt-get install doxygen   # Debian / Ubuntu
# brew install doxygen         # macOS

# Generate docs (from repository root)
doxygen Doxyfile

# Open in browser
xdg-open docs/html/index.html  # Linux
# open docs/html/index.html    # macOS
```
