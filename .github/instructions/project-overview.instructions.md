# Project Overview: Foundations, Style, and Structure

**What this covers:** The tech stack, build system, code style, naming
conventions, directory layout, key class patterns, and error-handling rules for
the SteamRot C++ game engine. Load this file for any task — it is the baseline
every agent needs before consulting a more specific instruction file.

---

## 1. Technology stack

| Concern | Technology |
|---|---|
| Language | C++23 (minimum required) |
| Build system | CMake ≥ 3.31, using CMake Presets (`Debug` / `Release`) |
| Graphics | SFML (graphics, window, system modules) |
| Testing | Catch2 |
| Serialization | FlatBuffers |
| Logging | spdlog |

The build system uses CMake Presets. All building and testing is done **locally
by the user**. Agents must never run `cmake`, build commands, `ctest`, or
linters.

---

## 2. Directory layout

The project follows the
[Pitchfork](https://github.com/vector-of-bool/pitchfork) layout:

```
src/            Source code
  assets/
  context/
  data_providers/
  display/
  engine/
  entity/         Entity management and configurators
  error_loop/
  events/         Event handling system
  logger/         Logging utilities
  logic/          Game logic classes
  resources/
  scenes/         Scene management
  types/

data/           Non-code assets (images, JSON, FlatBuffers schemas)
                Organised by purpose, not file type (e.g. scene/ not json/)

tests/          Test files (mirrors src/ structure)
  unit/           Unit tests, organised by subsystem
  integration/    Integration tests (2+ components)
  system/         End-to-end tests (future)
  perf/           Performance benchmarks (future)
  data/           Test data files
  context/        Test utilities

docs/           Doxygen output
cmake/          CMake modules and scripts
.github/
  instructions/   Focused agent instruction files (this directory)
```

Key CMake variables:
- `data_dir` → `${CMAKE_SOURCE_DIR}/data`
- `test_data_dir` → `${CMAKE_SOURCE_DIR}/tests/data`

---

## 3. Code style

### General

Follow the
[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
with SFML-inspired formatting conventions.

### Indentation

- **2 spaces** — no tabs. Enforced by `.editorconfig`.

### File structure

1. `#pragma once` header guard (headers only)
2. Visual divider
3. `// Headers` section with includes
4. Visual divider
5. `namespace steamrot { … }`

### Visual dividers

Use exactly 60 slashes between sections and between function definitions:

```cpp
////////////////////////////////////////////////////////////
```

### Function documentation

```cpp
////////////////////////////////////////////////////////////
/// @brief Brief description.
///
/// @param param_name Description of parameter.
/// @return Description of return value.
////////////////////////////////////////////////////////////
```

- Use `///` Doxygen-style comments.
- Use `@brief`, `@param`, `@return` tags.
- Add comments for complex logic; omit them for obvious code.

---

## 4. Naming conventions

### Code elements

| Element | Convention | Example |
|---|---|---|
| Member variables | `m_` prefix + `snake_case` | `m_entity_id` |
| Classes | `PascalCase` | `FlatbuffersConfigurator` |
| Public methods / free functions | `PascalCase` | `GetComponent`, `ProcessLogic` |
| Namespaces | `lowercase` | `steamrot`, `steamrot::tests` |
| Component classes | `C` prefix + `PascalCase` | `CUserInterface`, `CGrimoireMachina` |

### File naming

| File type | Pattern | Namespace |
|---|---|---|
| Class header/source | `ClassName.h` / `ClassName.cpp` | `steamrot::ClassName` |
| Free-function files | `subsystem_category.h/cpp` (snake_case) | `steamrot::subsystem::category` |
| Template utilities | `SubsystemUtils.h` (header-only, PascalCase) | `steamrot::subsystem` |
| Test helpers | `subsystem_test_helpers.h/cpp` (snake_case) | `steamrot::tests::subsystem` |
| Test files | `ModuleName.test.cpp` | — |

### Member variable initialisation

Always use brace-initialisation:

```cpp
int m_count{0};
std::string m_name{};
bool m_active{false};
```

---

## 5. Key classes and patterns

### PathProvider

- Provides absolute paths to data files.
- Configured via CMake with the source directory.
- `EnvironmentType` (`Test` or `Production`) is set **once** per runtime.

### Components

- Pure data containers — no logic, only data members and
  `GetComponentRegisterIndex()`.
- Inherit from the `Component` struct.
- Must be default-constructible (initialise all members).
- Use the `C` prefix (`CUserInterface`, `CGrimoireMachina`, …).
- Registered in the `ComponentRegister` tuple in `src/entity/containers.h`.

### FlatBuffers data

- Each component has a corresponding FlatBuffers schema (`.fbs` file).
- Schema table names use the `Data` suffix (e.g., `NewComponentData`).
- Schemas live in `src/flatbuffers_headers/`.
- Generated headers are created automatically during the local build.

### Logic classes

- Subclass the abstract `Logic` class.
- Override the private `ProcessLogic()` method.
- Constructor signature: `MyLogic(const LogicContext logic_context)`.
- `ProcessLogic()` must be a **clean list of named free-function calls** — no
  inline loops or business logic directly inside the method body.
- Subscribers are registered via `data/defaults/logic_config/logic_config.json`,
  never manually in a constructor.

### std::expected error handling

```cpp
// Failable operations return std::expected
std::expected<std::monostate, FailInfo>
DoThing(const Data* data) {
  if (!data)
    return std::unexpected(FailInfo{"data is null"});

  // … logic …

  return std::monostate{};
}
```

- Use `std::expected<T, FailInfo>` for runtime failures.
- Return `std::expected<std::monostate, FailInfo>` when there is no meaningful
  return value on success.
- Use `ErrorHandler` namespace to process errors by severity.
- Critical errors throw exceptions; everything else propagates via
  `std::expected`.
- **Do not** use `try/catch` except at the top-level game loop.

### FlatBuffers null safety

Every FlatBuffers field access must be guarded:

```cpp
// Primitive types (int, bool, float) — no guard needed
component.m_count = data->count();

// Complex types (string, vector, nested table) — always guard
if (data->name())
  component.m_name = data->name()->str();

if (data->nested_table())
  Configure(data->nested_table(), component);
```

Unguarded access to a missing FlatBuffers field causes a segfault.

---

## 6. Common patterns quick-reference

### Visual divider template

```cpp
////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "MyHeader.h"

////////////////////////////////////////////////////////////
/// @brief Does the thing.
////////////////////////////////////////////////////////////
void DoThing() {
  // …
}

////////////////////////////////////////////////////////////
```

### FlatBuffers null check

```cpp
if (flatbuffer_data) {
  if (flatbuffer_data->field())
    use(flatbuffer_data->field());
}
```

### std::expected propagation

```cpp
auto result = DoThing(data);
if (!result.has_value())
  return std::unexpected(result.error());
```

---

## 7. Build system reference (agents do NOT run these)

```bash
# Configure
cmake --preset Debug

# Build
cmake --build --preset Debug

# Test (all; visual tests excluded by default via [.visual] hidden tag)
ctest --preset Debug

# Run only unit tests
ctest --preset Debug -L unit

# Run specific subsystem
ctest --preset Debug -R logic
```

These commands are **documentation only**. Agents must never execute them.

---

## Key rules

- Never build, test, or lint — all done locally by the user.
- Keep `ProcessLogic()` as a clean list of named free-function calls.
- Never register subscribers in a Logic constructor — use `logic_config.json`.
- Always guard FlatBuffers complex-type field accesses.
- Always initialise all member variables with brace-initialisation.
- Use `C` prefix for component classes; `m_` prefix for member variables.
- 2-space indentation — no tabs.

## Gotchas

1. **FlatBuffers segfaults** — always null-check strings, vectors, and nested
   tables before access.
2. **Component registration** — new components must be added to the
   `ComponentRegister` tuple or they will not be visible to the ECS.
3. **PathProvider** — `EnvironmentType` must be set exactly once per runtime;
   setting it twice is an error.
4. **Logic execution order** — the order items are pushed into a `LogicVector`
   is the execution order; tests validate this order with `dynamic_cast`.
5. **Subscriber member variables** — never add `std::shared_ptr<Subscriber>`
   members to a Logic class; all subscribers live in the base-class
   `m_subscribers` vector.

## Related files

- `src/entity/containers.h` — `ComponentRegister` tuple
- `src/logic/LogicFactory.cpp` — maps scene types to Logic instances
- `data/defaults/logic_config/logic_config.json` — subscriber declarations
- `CMakePresets.json` — build presets
- `.editorconfig` — enforces 2-space indentation
- `README.md` — project-level documentation and workflows

---

## How to improve this workflow

If you followed this file and found a step that was wrong, missing, or
out-of-date, please report it using the process in
[`meta-workflows.instructions.md`](meta-workflows.instructions.md) (section 3).
