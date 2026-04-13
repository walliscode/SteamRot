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
python -m tools.test_dashboard --tests-dir tests/
```

### Serve this wiki locally

```bash
# Install dependencies (once)
pip install -r tools/wiki/requirements.txt

# Start the local wiki server
mkdocs serve
```
