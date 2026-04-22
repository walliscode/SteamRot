# GitHub Copilot Instructions for SteamRot

This is the **root index** for all Copilot agent instructions in this
repository. It states universal rules that apply to every task, then routes you
to the focused instruction file that matches your current task.

---

## ⚠️ Universal Agent Rules (always apply)

1. **NEVER build, test, or lint.** All building, testing, and linting is done
   locally by the user.
   - Do NOT run `cmake`, build commands, `ctest`, or linters.
   - Write test files but do not execute them.

2. **Make minimal, targeted changes.** Only modify what is needed to accomplish
   the task. Do not refactor unrelated code.

3. **Follow the code style** defined in
   [`instructions/project-overview.instructions.md`](.github/instructions/project-overview.instructions.md):
   - C++23, 2-space indentation, `m_` member prefix, `PascalCase` methods,
     Doxygen `///` comments, visual dividers (`////////////////////////////////////////////////////////////`).

4. **Check for a matching workflow before writing code.** See
   [Using the Instruction System](#using-the-instruction-system) below.

5. **Read `README.md`** at the repo root to understand the current project
   state before making changes.

---

## Using the Instruction System

### Step 1 — Find the right instruction file

| I am working on… | Load this file |
|---|---|
| Project foundations, style, naming, error handling | [`instructions/project-overview.instructions.md`](.github/instructions/project-overview.instructions.md) |
| ECS components (adding/modifying `C*` structs, FlatBuffers schemas) | `instructions/engine-components.instructions.md` *(coming soon)* |
| Logic classes (`Logic` subclasses, `LogicFactory`, `ProcessLogic`) | `instructions/engine-logic.instructions.md` *(coming soon)* |
| Scene management | `instructions/engine-scenes.instructions.md` *(coming soon)* |
| Event / subscriber system, `logic_config.json` | `instructions/engine-events.instructions.md` *(coming soon)* |
| UI elements (`UIElement` subclasses, styles, drawing) | `instructions/engine-ui.instructions.md` *(coming soon)* |
| FlatBuffers schemas (authoring `.fbs` files, null-safety) | `instructions/data-flatbuffers.instructions.md` *(coming soon)* |
| Context configuration (`context_data.json`, `ContextConfigurator`) | `instructions/data-context-config.instructions.md` *(coming soon)* |
| Writing or organising tests | `instructions/testing-overview.instructions.md` *(coming soon)* |
| Data-driven tests (`test_data.fbs`, JSON fixture files) | `instructions/testing-data-driven.instructions.md` *(coming soon)* |
| Assets, `PathProvider`, `data/` layout | `instructions/assets-data-files.instructions.md` *(coming soon)* |
| Adding or updating instruction files themselves | [`instructions/meta-workflows.instructions.md`](.github/instructions/meta-workflows.instructions.md) |

### Step 2 — No matching workflow?

If no instruction file covers your task:

1. Complete the task using good judgment and the project overview as a
   foundation.
2. **After completing the task**, propose a new instruction file by stating:
   - Suggested filename (e.g., `engine-scenes.instructions.md`)
   - What it should cover (3–5 bullet points)
   
   Leave this as a comment in your final response so the user can decide whether
   to commit it.

### Step 3 — Workflow is incomplete or wrong?

If you follow an instruction file but discover the steps are missing, outdated,
or incorrect, **note the discrepancy** at the end of your response and suggest
the specific change to make to that file.

---

## Instruction File Catalogue

All instruction files live in `.github/instructions/`. See
[`instructions/meta-workflows.instructions.md`](.github/instructions/meta-workflows.instructions.md)
for the rules governing how these files are structured and maintained.

