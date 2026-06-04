# Adding Descriptors

\page adding_descriptors Adding Descriptors

This guide covers the descriptor APIs that remain in the SteamRot `PartGraph`
analysis system: `NodeDescriptor` and `ChainDescriptor`.

## Table of Contents

- [Overview](#overview)
- [Descriptor Levels](#descriptor-levels)
- [Decision Flowchart](#decision-flowchart)
- [Key Files](#key-files)
- [Adding a NodeDescriptor](#adding-a-nodedescriptor)
- [Adding a ChainDescriptor](#adding-a-chaindescriptor)
- [Composing Descriptors](#composing-descriptors)
- [Testing Descriptors](#testing-descriptors)
- [Analysis Trace System](#analysis-trace-system)
- [Future Implementation Notes](#future-implementation-notes)
- [Best Practices](#best-practices)
- [Troubleshooting](#troubleshooting)
- [Summary Checklist](#summary-checklist)

---

## Overview

Descriptors are named, callable predicates that answer structural questions
about a `PartGraph`.

`PartGraph` is defined in `src/types/entity/MachinaFormScaffold.h`:

```cpp
using PartGraph = std::map<uint32_t, std::variant<JointInstance, FragmentInstance>>;
```

Call sites always pass `scaffold.parts`.

## Descriptor Levels

There are two descriptor levels:

| Level | Class             | Signature                                           |
| ----- | ----------------- | --------------------------------------------------- |
| Node  | `NodeDescriptor`  | `NodeDescriptorResult(const PartGraph&, uint32_t)`  |
| Chain | `ChainDescriptor` | `ChainDescriptorResult(const PartGraph&, uint32_t)` |

`NodeDescriptor` and `ChainDescriptor` are wrapper classes with stable names,
call operators, and trace-aware result types.

## Decision Flowchart

```
Start: I need to query something about the assembled machine.
│
├─ Does the answer depend only on one node's own data?
│  └──────────────────────────────────────────────► NodeDescriptor
│                                                   Declare in descriptors_node_descriptors.h
│                                                   Define in descriptors_node_descriptors.cpp
│
└─ Does the answer require a traversal from one anchor node?
   └──────────────────────────────────────────────► ChainDescriptor
                                                    Declare in descriptors_chain_descriptors.h
                                                    Define in descriptors_chain_descriptors.cpp
                                                    Build with ChainDescriptorBuilder
```

Prefer the narrowest descriptor level that answers the question.

## Key Files

| File                                                                 | Purpose                                                                                                         |
| -------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------- |
| `src/types/entity/MachinaFormScaffold.h`                             | `PartGraph`, `JointInstance`, `FragmentInstance`, `PartInstance`, `SocketMap`, `SocketData`, `SocketConnection` |
| `src/types/logic/AnalysisEvent.h`                                    | `TraceEventKind`, `ScopeKind`, `AnalysisEvent`, `AnalysisTrace`, `Merge()`                                      |
| `src/types/logic/DescriptorResult.h`                                 | `DescriptorResult`, `NodeDescriptorResult`, `ChainDescriptorResult`                                             |
| `src/logic/descriptors/descriptors_node_descriptors.h/.cpp`          | `NodeDescriptor` and concrete node predicates                                                                   |
| `src/logic/descriptors/descriptors_chain_descriptors.h/.cpp`         | `ChainDescriptor`, `lift_to_chain`, concrete chain predicates                                                   |
| `src/logic/descriptors/descriptors_general.h`                        | `and_`, `or_`, `not_` combinators                                                                               |
| `src/logic/descriptors/ChainDescriptorBuilder.h/.cpp`                | `ChainDescriptorBuilder`                                                                                        |
| `src/logic/descriptors/DescriptorFormatter.h`                        | Base trace formatter                                                                                            |
| `src/logic/descriptors/TerminalDescriptorFormatter.h/.cpp`           | Plain-text trace rendering                                                                                      |
| `tests/unit/logic/descriptors/descriptors_node_descriptors.test.cpp` | Descriptor unit tests                                                                                           |
| `tests/unit/logic/part_library.h/.cpp`                               | `TestPartLibrary`, `PartLibraryBuilder`, `CheckNodeDescriptorForAllScenarios`                                   |

---

## Adding a NodeDescriptor

A `NodeDescriptor` answers a question about one node's own data, such as its
variant type or `connection_count`.

### Declaration

Add the declaration inside
`src/logic/descriptors/descriptors_node_descriptors.h`:

```cpp
/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when <condition>.
/////////////////////////////////////////////////
extern const NodeDescriptor my_descriptor;
```

For a parameterised descriptor:

```cpp
NodeDescriptor my_factory(size_t n);
```

### Definition

Define the descriptor in `descriptors_node_descriptors.cpp` using the named
constructor form so the trace contains a stable predicate name:

```cpp
const NodeDescriptor my_descriptor{
    "my_descriptor",
    [](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
      const auto part_it = parts.find(id);
      if (part_it == parts.end())
        return NodeDescriptorResult{false, "incorrect key: part_id=" +
                                               std::to_string(id)};
      const bool ok = /* your condition */;
      return NodeDescriptorResult{ok};
    }};
```

## Adding a ChainDescriptor

A `ChainDescriptor` answers a traversal-based question from one anchor node. Use
`ChainDescriptorBuilder`.

Chain evaluation is first-match only: traversal stops as soon as the first valid
chain is found (based on DFS neighbour iteration order).

### Declaration

Add the declaration in `src/logic/descriptors/descriptors_chain_descriptors.h`:

```cpp
extern const ChainDescriptor my_chain;
```

### Definition

Define it in `src/logic/descriptors/descriptors_chain_descriptors.cpp`:

```cpp
const ChainDescriptor my_chain = ChainDescriptorBuilder{}
    .WhileIsTrue(is_serial)
    .Then(is_terminal)
    .Build("my_chain")
    .value();
```

If you only need to reuse node logic at the anchor node, convert it with
`lift_to_chain()`:

```cpp
ChainDescriptor terminal_anchor = lift_to_chain(is_terminal);
```

## Composing Descriptors

| Operation           | Input → Output                     | Purpose                                    |
| ------------------- | ---------------------------------- | ------------------------------------------ |
| `lift_to_chain(nd)` | `NodeDescriptor → ChainDescriptor` | Reuse node logic inside a chain descriptor |
| `and_(a, b)`        | `Desc × Desc → Desc`               | Both must be true                          |
| `or_(a, b)`         | `Desc × Desc → Desc`               | Either may be true                         |
| `not_(a)`           | `Desc → Desc`                      | Negate a descriptor                        |

`and_`, `or_`, and `not_` only combine descriptors of the same level.

## Testing Descriptors

All descriptor tests live in
`tests/unit/logic/descriptors/descriptors_node_descriptors.test.cpp`.

### Node descriptors

Use `CheckNodeDescriptorForAllScenarios` for named node predicates:

```cpp
steamrot::tests::CheckNodeDescriptorForAllScenarios(
    descriptors::my_descriptor,
    {.linear_chain = {/* expected per node */},
     .ring = {/* expected per node */},
     .isolated_pair = {/* expected per node */},
     .simple_branch = {/* expected per node */}},
    lib);
```

Node order is always fragments first, then joints.

### Chain descriptors

Pass `scaffold.parts` and the anchor part ID:

```cpp
auto result = descriptors::is_serial_chain(scaffold.parts, anchor_id);
REQUIRE(result.m_result == true);
```

## Analysis Trace System

Every descriptor evaluation populates an `AnalysisTrace` in the result's
`m_trace` field.

`ChainDescriptorResult` also records path outputs:

- `valid_subgraph` stores the first valid path, if one is found.
- `invalid_subgraphs` stores rejected paths encountered during traversal.

| `TraceEventKind`    | When emitted                 | Key fields                                               |
| ------------------- | ---------------------------- | -------------------------------------------------------- |
| `NodeEval`          | Before testing a predicate   | `depth`, `part_id`, `predicate_name`                     |
| `NodeResult`        | After the predicate test     | `depth`, `part_id`, `predicate_name`, `result`, `reason` |
| `MovingToNeighbour` | DFS traverses an edge        | `depth`, `from_id`, `to_id`, `socket_id`                 |
| `Backtracking`      | DFS returns from a neighbour | `depth`, `from_id`                                       |
| `ScopeBegin`        | Chain evaluation starts      | `depth=0`, `scope_name`, `scope_kind`, `anchor_id`       |
| `ScopeEnd`          | Chain evaluation ends        | `depth=0`, `scope_name`, `scope_kind`, `result`          |

Use `TerminalDescriptorFormatter` to render a readable trace string.

## Future Implementation Notes

1. **ChainDescriptorBuilder traversal polish** — continue improving cycle
   handling, branching diagnostics, and trace detail as the chain vocabulary
   grows.
2. **New scaffold scenarios** — add richer topologies to `TestPartLibrary` as
   chain descriptors need them.
3. **EdgeDescriptor integration** — a future `EdgeDescriptor` type alias could
   express connection-type constraints directly on connections.
4. **Additional trace formatter targets** — JSON, HTML, or in-engine debug
   overlays can be added without changing descriptor code.

## Best Practices

- Choose the narrowest descriptor level that answers the question.
- Prefer composing existing descriptors before writing a new lambda.
- Name descriptors as verb phrases such as `is_terminal` or `is_serial_chain`.
- Always pass `scaffold.parts`, never the whole scaffold.
- Update this guide whenever the descriptor API changes.

## Troubleshooting

| Symptom                                                    | Likely Cause                                     | Fix                                                 |
| ---------------------------------------------------------- | ------------------------------------------------ | --------------------------------------------------- |
| `and_()` / `or_()` compile error about deduced types       | Both arguments must be the same descriptor level | Combine like-for-like descriptors only              |
| `CheckNodeDescriptorForAllScenarios` fails on one scenario | Node order mismatch                              | Arrays are fragments-first then joints              |
| Segfault in descriptor accessing FlatBuffers field         | Unguarded optional access                        | Guard all string, vector, and nested-table accesses |
| Linker error: undefined reference to descriptor            | `extern const` declared but not defined          | Add the definition in the matching `.cpp` file      |
| Trace is empty after evaluation                            | Descriptor constructed without a name            | Pass the name as the first constructor argument     |

## Summary Checklist

- [ ] Chose node vs chain descriptor based on the required scope
- [ ] Checked whether an existing descriptor can be composed instead
- [ ] Declared the descriptor in the correct header
- [ ] Defined the descriptor in the matching source file
- [ ] Added or updated descriptor tests
- [ ] Verified all call sites pass `scaffold.parts`
- [ ] Updated this document if the descriptor API changed
