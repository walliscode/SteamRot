# Meta-Workflows: How to Maintain Agent Instructions

**What this covers:** The rules and process for creating, updating, and
organising instruction files in `.github/instructions/`. Read this file whenever
you are asked to add a workflow, improve existing guidance, or assess whether an
instruction gap exists.

---

## 1. When you have a matching workflow

1. Load the relevant `instructions/<file>.instructions.md` before writing any
   code.
2. Follow the numbered steps precisely.
3. If you encounter a step that is wrong, incomplete, or no longer accurate, do
   **not** silently skip it — complete the task using good judgment, then report
   the discrepancy (see section 3).

---

## 2. When no matching workflow exists

1. Complete the task using good judgment and the foundations in
   [`project-overview.instructions.md`](project-overview.instructions.md).
2. **After completing the task**, propose a new instruction file by including
   the following in your response:

```
### Proposed new instruction file
- **Filename**: `.github/instructions/<category>-<topic>.instructions.md`
- **Covers**:
  - <bullet 1>
  - <bullet 2>
  - …
- **Suggested sections**: What this covers / Step-by-step / Key rules / Gotchas /
  Related files
```

   Do not create the file unless the user explicitly asks you to. Leave it as a
   proposal so the user can review it first.

---

## 3. When you improve on an existing workflow

If following an instruction file reveals a gap, error, or missing step:

1. Note the issue at the end of your response in a clearly labelled block:

```
### Suggested update to `instructions/<filename>.instructions.md`
- **Section**: <section heading>
- **Issue**: <what is wrong or missing>
- **Suggested change**: <what the step/paragraph should say>
```

2. Do not edit the instruction file yourself unless the user explicitly asks you
   to update it.

---

## 4. Instruction file format

Every instruction file in `.github/instructions/` must follow this template:

```markdown
# <Title>: <Topic>

**What this covers:** One or two sentences describing the scope and when to use
this file.

---

## 1. <First major step or section>
…

## 2. <Second major step or section>
…

## Key rules

- <Rule 1>
- <Rule 2>

## Gotchas

- <Gotcha 1>

## Related files

- `<path/to/relevant/file>` — why it matters
```

**Required sections** (in order):

| Section | Purpose |
|---|---|
| Title + "What this covers" | Scope declaration so an agent can decide in one read whether this file is relevant |
| Numbered steps | Actionable, ordered instructions with code examples where needed |
| Key rules | Absolute constraints (things that must or must never be done) |
| Gotchas | Common mistakes and how to avoid them |
| Related files | Links to source files, headers, or other instruction files the agent will need |
| How to improve this workflow | *(see section 5 below)* |

---

## 5. "How to improve this workflow" footer

Every instruction file ends with this footer (fill in the actual file name):

```markdown
---

## How to improve this workflow

If you followed this file and found a step that was wrong, missing, or
out-of-date, please report it using the process in
[`meta-workflows.instructions.md`](meta-workflows.instructions.md) (section 3).
```

---

## 6. Naming convention

Files are named `<category>-<topic>.instructions.md` using lowercase
kebab-case.

| Category prefix | Covers |
|---|---|
| `engine-` | Core engine subsystems (ECS, Logic, Scenes, Events, UI) |
| `data-` | Data layer (FlatBuffers schemas, context config, assets) |
| `testing-` | Test infrastructure, patterns, data-driven tests |
| `assets-` | `PathProvider`, `data/` layout, adding images/JSON/schemas |
| `meta-` | The instruction system itself |
| `project-` | Foundations: tech stack, style, naming, error handling |

Examples:
- `engine-components.instructions.md`
- `engine-logic.instructions.md`
- `data-flatbuffers.instructions.md`
- `testing-overview.instructions.md`

---

## Key rules

- Instruction files are narrow and scoped. One category, no cross-cutting
  content.
- Never include build or test commands that agents should execute — those
  sections are reference-only and must say so explicitly.
- Steps must be actionable and ordered. Prose descriptions of *what* something
  is belong in `project-overview.instructions.md`, not in workflow files.
- An agent should be able to follow a workflow file cold, with no prior context
  beyond the project overview.

## Gotchas

- Do not create a new instruction file speculatively. Propose it first; the user
  decides whether to commit it.
- Do not duplicate content that already exists in another instruction file.
  Cross-reference instead.
- Instruction files are not README files. Keep them terse and agent-oriented.

## Related files

- `.github/copilot-instructions.md` — root index; routes agents to this and
  other instruction files
- `.github/instructions/project-overview.instructions.md` — foundational
  project knowledge every agent needs

---

## How to improve this workflow

If you followed this file and found a step that was wrong, missing, or
out-of-date, please report it using the process described in section 3 of this
file.
