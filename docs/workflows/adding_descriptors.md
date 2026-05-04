# Adding Descriptors

\page adding_descriptors Adding Descriptors

This guide provides a comprehensive workflow for adding new graph descriptors to
the SteamRot engine's `PartGraph` analysis system.

## Table of Contents

- [Overview](#overview)
- [The Descriptor Hierarchy](#the-descriptor-hierarchy)
- [When to Add a Descriptor — Decision Flowchart](#when-to-add-a-descriptor--decision-flowchart)
- [Key Files](#key-files)
- [Step-by-Step Workflow](#step-by-step-workflow)
  - [Step 1: Choose the Correct Descriptor Level](#step-1-choose-the-correct-descriptor-level)
  - [Step 2: Declare the Descriptor](#step-2-declare-the-descriptor)
  - [Step 3: Define the Descriptor](#step-3-define-the-descriptor)
  - [Step 4: Write Unit Tests](#step-4-write-unit-tests)
  - [Step 5: Build and Test](#step-5-build-and-test)
- [Adding Each Descriptor Level](#adding-each-descriptor-level)
  - [NodeDescriptor](#nodedescriptor)
  - [ContextualNodeDescriptor](#contextualnodedescriptor)
  - [ChainDescriptor](#chaindescriptor)
  - [GraphDescriptor](#graphdescriptor)
- [Lifting and Composing Descriptors](#lifting-and-composing-descriptors)
- [Testing Descriptors](#testing-descriptors)
- [Common Patterns](#common-patterns)
- [Future Implementation Notes](#future-implementation-notes)
- [Best Practices](#best-practices)
- [Troubleshooting](#troubleshooting)
- [Summary Checklist](#summary-checklist)

---

## Overview

Descriptors are typed predicates — callable objects stored in `std::function` —
that answer structural questions about a `PartGraph`.

`PartGraph` is a type alias defined in `src/types/entity/MachinaFormScaffold.h`:

```cpp
using PartGraph = std::map<uint32_t, std::variant<JointInstance, FragmentInstance>>;
```

It is exposed as `MachinaFormScaffold::parts`. Descriptors receive the map
directly (as `const PartGraph&`) rather than the whole scaffold. Call sites
always pass `scaffold.parts`.

There are four descriptor levels, ordered from narrowest to broadest scope:

| Level           | Type alias                 | Signature                                    |
| --------------- | -------------------------- | -------------------------------------------- |
| Node            | `NodeDescriptor`           | `NodeDescriptorResult(const PartGraph&, uint32_t)` |
| Contextual node | `ContextualNodeDescriptor` | `NodeDescriptorResult(const PartGraph&, uint32_t)` |
| Chain           | `ChainDescriptor`          | `ChainDescriptorResult(const PartGraph&, uint32_t)` |
| Graph           | `GraphDescriptor`          | `GraphDescriptorResult(const PartGraph&)`    |

The four aliases live in their respective files in `src/logic/descriptors/` in
the `steamrot::logic::descriptors` namespace. Generic combinators live in
`src/logic/descriptors/descriptors_general.h`. The `ChainDescriptorBuilder`
class lives in `src/logic/descriptors/ChainDescriptorBuilder.h` under the
same namespace.

---

## The Descriptor Hierarchy

```
NodeDescriptor           NodeDescriptorResult(const PartGraph&, uint32_t)
       │  promoted by
       │    lift()
       ▼
ContextualNodeDescriptor NodeDescriptorResult(const PartGraph&, uint32_t)
       │  same underlying type as
       │
ChainDescriptor          ChainDescriptorResult(const PartGraph&, uint32_t)
       │  consumed by
       │    any_node_satisfies()
       │    all_nodes_satisfy()
       ▼
GraphDescriptor          GraphDescriptorResult(const PartGraph&)
       │
       │  (terminal — never passes back into a modifier)
       ▼
   Logic / Action layer
```

**Key properties:**

- `ContextualNodeDescriptor` and `ChainDescriptor` share the same underlying
  `std::function` type. The distinction is semantic: contextual descriptors
  examine the anchor node and its direct neighbours; chain descriptors walk
  multiple hops from the anchor.
- `GraphDescriptor` is **terminal**. It is consumed in Logic/Action classes to
  gate gameplay decisions. Never pass a `GraphDescriptor` into a combinator or
  modifier.
- Combinators (`and_`, `or_`, `not_`) are generic templates. Both arguments must
  be the **same descriptor level** — mixing levels is a compile error.
- Predicates access the part variant via `parts.at(id)` and read
  `connection_count` from the base `PartInstance` via `std::visit`.

---

## When to Add a Descriptor — Decision Flowchart

```
Start: I need to query something about the assembled machine.
│
├─ Does my question need MORE than one node?
│   │
│   ├── No ──────────────────────────────────────► NodeDescriptor
│   │                                               NodeDescriptorResult(const PartGraph&, uint32_t)
│   │                                               Declare in descriptors_node_descriptors.h
│   │                                               Define in descriptors_node_descriptors.cpp
│   │
│   └── Yes
│        │
│        ├── Only direct neighbours (depth 1)?
│        │    └────────────────────────────────► ContextualNodeDescriptor
│        │                                        NodeDescriptorResult(const PartGraph&, uint32_t)
│        │                                        Walk neighbours via socket.connected_to
│        │
│        ├── Multi-hop walk / path matching?
│        │    └────────────────────────────────► ChainDescriptor
│        │                                        ChainDescriptorResult(const PartGraph&, uint32_t)
│        │                                        Use ChainDescriptorBuilder (see section below)
│        │
│        └── No anchor — whole machine question?
│             └────────────────────────────────► GraphDescriptor
│                                                 GraphDescriptorResult(const PartGraph&)
│                                                 Derive via any_node_satisfies() or
│                                                 all_nodes_satisfy()
│
├─ Can I express it as a composition of existing descriptors?
│   └── Yes → use and_() / or_() / not_() / lift()
│              No new declaration needed.
│
└─ Does the question belong in a Logic/Action class (gates a gameplay decision)?
    └── Yes → GraphDescriptor consumed directly in that class.
              Define there, not in descriptors_graph_descriptors.
```

---

## Key Files

| File | Purpose |
|------|---------|
| `src/types/entity/MachinaFormScaffold.h` | `PartGraph` type alias; `JointInstance`, `FragmentInstance`, `PartInstance`, `SocketMap`, `SocketData`, `SocketConnection` |
| `src/types/logic/DescriptorResult.h` | `NodeDescriptorResult`, `ChainDescriptorResult`, `GraphDescriptorResult` |
| `src/logic/descriptors/descriptors_node_descriptors.h/.cpp` | `NodeDescriptor`, `ContextualNodeDescriptor`, `lift`, concrete predicates |
| `src/logic/descriptors/descriptors_chain_descriptors.h/.cpp` | `ChainDescriptor`, `lift_to_chain`, concrete chain predicates |
| `src/logic/descriptors/descriptors_graph_descriptors.h` | `GraphDescriptor`, `any_node_satisfies`, `all_nodes_satisfy` |
| `src/logic/descriptors/descriptors_general.h` | `and_`, `or_`, `not_` combinators |
| `src/logic/descriptors/ChainDescriptorBuilder.h/.cpp` | `ChainDescriptorBuilder` class |
| `tests/unit/logic/part_library.h/.cpp` | `TestPartLibrary`, `PartLibraryBuilder`, `CheckNodeDescriptorForAllScenarios` |
| `tests/unit/logic/descriptors/descriptors_node_descriptors.test.cpp` | Descriptor unit tests |

---

## Step-by-Step Workflow

### Step 1: Choose the Correct Descriptor Level

Use the [Decision Flowchart](#when-to-add-a-descriptor--decision-flowchart)
above. If in doubt, start with the narrowest level that answers your question.

### Step 2: Declare the Descriptor

Open `src/logic/descriptors/descriptors_node_descriptors.h`.

Add your declaration inside the `steamrot::logic::descriptors` namespace,
grouped with descriptors of the same level:

```cpp
/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when <your condition here>.
/////////////////////////////////////////////////
extern const NodeDescriptor my_descriptor;
```

For a parameterised factory:

```cpp
/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when <condition>.
///
/// @param n <parameter description>.
/// @return NodeDescriptor returning true when <condition>.
/////////////////////////////////////////////////
NodeDescriptor my_factory(size_t n);
```

### Step 3: Define the Descriptor

Open `src/logic/descriptors/descriptors_node_descriptors.cpp`.

Add the definition inside the same namespace. All predicates access the part
variant via `parts.at(id)` and read `connection_count` from `PartInstance`:

```cpp
/////////////////////////////////////////////////
const NodeDescriptor my_descriptor =
    [](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
  const size_t count = std::visit(
      [](const auto &inst) -> size_t { return inst.connection_count; },
      parts.at(id));
  return NodeDescriptorResult{/* your condition using count or variant type */};
};
```

For a factory:

```cpp
/////////////////////////////////////////////////
NodeDescriptor my_factory(size_t n) {
  return [n](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
    const size_t count = std::visit(
        [](const auto &inst) -> size_t { return inst.connection_count; },
        parts.at(id));
    return NodeDescriptorResult{count == n};
  };
}
```

### Step 4: Write Unit Tests

Open `tests/unit/logic/descriptors/descriptors_node_descriptors.test.cpp`.

Add a `TEST_CASE` using `CheckNodeDescriptorForAllScenarios` (see
[Testing Descriptors](#testing-descriptors)).

### Step 5: Build and Test

```bash
cmake --build --preset Debug
ctest --preset Debug -R descriptors_node_descriptors
```

---

## Adding Each Descriptor Level

### NodeDescriptor

A `NodeDescriptor` answers a question about **one node's own data** — its type
(fragment vs joint) or its `connection_count`.

#### Declaration (`descriptors_node_descriptors.h`)

```cpp
/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when the node has exactly
/// @p n connections.
///
/// @param n Number of connections.
/// @return NodeDescriptor returning true when connection_count == n.
/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n);

/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when the node is a terminal
/// (at most 1 connected socket).
/////////////////////////////////////////////////
extern const NodeDescriptor is_terminal;
```

#### Definition (`descriptors_node_descriptors.cpp`)

```cpp
/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n) {
  return [n](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
    const size_t count = std::visit(
        [](const auto &inst) -> size_t { return inst.connection_count; },
        parts.at(id));
    return NodeDescriptorResult{count == n};
  };
}

/////////////////////////////////////////////////
const NodeDescriptor is_terminal = has_maximum_n_edges(1);
```

---

### ContextualNodeDescriptor

A `ContextualNodeDescriptor` answers a question about **a node and its direct
neighbours**. Neighbours are reached by iterating the part's `SocketMap` and
following `SocketData::connected_to`.

#### Declaration (`descriptors_node_descriptors.h`)

```cpp
/////////////////////////////////////////////////
/// @brief Returns a ContextualNodeDescriptor that returns true when the
///        node has at least one neighbour satisfying @p nd.
///
/// @param nd NodeDescriptor to test against each neighbour.
/// @return ContextualNodeDescriptor.
/////////////////////////////////////////////////
ContextualNodeDescriptor is_connected_to(NodeDescriptor nd);
```

#### Definition (`descriptors_node_descriptors.cpp`)

Walk the part's sockets; for each connected socket follow `connected_to.peer_part_id`:

```cpp
/////////////////////////////////////////////////
ContextualNodeDescriptor is_connected_to(NodeDescriptor nd) {
  return [nd = std::move(nd)](const PartGraph &parts,
                               uint32_t id) -> NodeDescriptorResult {
    const auto &sockets = std::visit(
        [](const auto &inst) -> const SocketMap & { return inst.sockets; },
        parts.at(id));
    for (const auto &[socket_id, socket_data] : sockets) {
      if (!socket_data.connected_to.has_value())
        continue;
      const uint32_t peer_id = socket_data.connected_to->peer_part_id;
      if (parts.count(peer_id) && nd(parts, peer_id))
        return NodeDescriptorResult{true};
    }
    return NodeDescriptorResult{false};
  };
}
```

---

### ChainDescriptor

A `ChainDescriptor` answers a structural question about a **multi-hop walk**
from an anchor node. Use `ChainDescriptorBuilder` from
`src/logic/descriptors/ChainDescriptorBuilder.h`.

#### Declaration (`descriptors_chain_descriptors.h`)

```cpp
/////////////////////////////////////////////////
/// @brief ChainDescriptor that returns true when the anchor starts a
///        linear 3-node chain: terminal → serial → terminal.
/////////////////////////////////////////////////
extern const ChainDescriptor linear_3_chain;
```

#### Definition (`descriptors_chain_descriptors.cpp`)

```cpp
/////////////////////////////////////////////////
// NOTE: ChainDescriptorBuilder::End() always returns false until the
// DFS traversal is fully implemented in ChainDescriptorBuilder.cpp.
const ChainDescriptor linear_3_chain =
    steamrot::logic::descriptors::ChainDescriptorBuilder{}
        .StartWith(is_terminal)
        .Then(is_serial)
        .End(is_terminal);
```

> ⚠️ **DFS traversal is not yet fully implemented.** `ChainDescriptorBuilder::End()`
> currently returns a descriptor that always returns `false`. Do not write tests
> that expect `true` results until the TODO in `ChainDescriptorBuilder.h` is
> resolved.

---

### GraphDescriptor

A `GraphDescriptor` answers a question about the **whole graph** — no anchor
node. Always derive it from `any_node_satisfies` or `all_nodes_satisfy`; never
write a `GraphDescriptor` lambda from scratch.

#### Where it lives

`GraphDescriptor`s are declared in `descriptors_graph_descriptors.h` and defined
in `descriptors_graph_descriptors.cpp`. Both iterate `const PartGraph&` directly.

#### Example

```cpp
namespace descriptors = steamrot::logic::descriptors;

// "Does the graph contain at least one terminal fragment?"
descriptors::GraphDescriptor has_terminal_fragment =
    descriptors::any_node_satisfies(
        descriptors::lift(descriptors::and_(
            descriptors::is_terminal,
            descriptors::is_fragment)));

// "Are all nodes either a fragment or a joint?"
descriptors::GraphDescriptor all_known_types =
    descriptors::all_nodes_satisfy(
        descriptors::lift(descriptors::or_(
            descriptors::is_fragment,
            descriptors::is_joint)));

// Evaluate against scaffold.parts
if (has_terminal_fragment(scaffold.parts)) {
  // gate some gameplay action
}
```

---

## Lifting and Composing Descriptors

| Operation                                              | Input → Output                                | Purpose                                               |
| ------------------------------------------------------ | --------------------------------------------- | ----------------------------------------------------- |
| `lift(nd)`                                             | `NodeDescriptor` → `ContextualNodeDescriptor` | Promote narrower descriptor without duplicating logic |
| `lift_to_chain(nd)`                                    | `NodeDescriptor` → `ChainDescriptor`          | Same, typed as `ChainDescriptor`                      |
| `any_node_satisfies(cd)`                               | `ChainDescriptor` → `GraphDescriptor`         | True if any part in the `PartGraph` satisfies `cd`    |
| `all_nodes_satisfy(cd)`                                | `ChainDescriptor` → `GraphDescriptor`         | True if every part in the `PartGraph` satisfies `cd`  |
| `and_(a, b)`                                           | `Desc × Desc` → `Desc`                        | Both must be true; same level required                |
| `or_(a, b)`                                            | `Desc × Desc` → `Desc`                        | Either must be true; same level required              |
| `not_(a)`                                              | `Desc` → `Desc`                               | Negate; same level required                           |

All utilities live in `steamrot::logic::descriptors`. Use
`namespace descriptors = steamrot::logic::descriptors;` for brevity.

### Example: composing without declaring a new descriptor

```cpp
namespace descriptors = steamrot::logic::descriptors;

// "Is this node a fragment with exactly 1 connection?"
descriptors::NodeDescriptor is_terminal_fragment =
    descriptors::and_(descriptors::is_fragment, descriptors::is_terminal);

// Lift to work in a ContextualNodeDescriptor context
descriptors::ContextualNodeDescriptor cnd =
    descriptors::lift(is_terminal_fragment);
```

---

## Testing Descriptors

All descriptor tests live in
`tests/unit/logic/descriptors/descriptors_node_descriptors.test.cpp`.

### Using CheckNodeDescriptorForAllScenarios

This helper runs a `NodeDescriptor` against every pre-built topology stored in
`TestPartLibrary` and `CHECK`s per-node results. Pass `scaffold.parts` (the
`PartGraph`) to each descriptor call.

**Node order within each array is fragments first, then joints**, matching
insertion order. Mismatching the order silently flips expected values.

```cpp
namespace descriptors = steamrot::logic::descriptors;

TEST_CASE("my_descriptor tests", "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();

  SECTION("Analyses all ScaffoldScenarios correctly") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        descriptors::my_descriptor,
        {// fragments first, then joints, per scenario
         .linear_chain   = {/* 2 fragments */ true, false,
                            /* 1 joint    */ false},
         .ring           = {/* 0 fragments */
                            /* 3 joints   */ false, false, false},
         .isolated_pair  = {/* 2 fragments */ true, true},
         .simple_branch  = {/* 3 fragments */ true, true, true,
                            /* 1 joint    */ false}},
        lib);
  }
}
```

### Pre-built scaffold scenarios

| Scenario key   | Topology                                             | Node count |
| -------------- | ---------------------------------------------------- | ---------- |
| `LinearChain`  | fragment – joint – fragment (both sockets connected) | 3          |
| `Ring`         | joint – joint – joint (ring of joints)               | 3          |
| `IsolatedPair` | fragment – fragment (no connections)                 | 2          |
| `SimpleBranch` | three fragments all connected to one joint           | 4          |

### Building a custom scaffold

Use `PartLibraryBuilder` for topologies not covered by the standard scenarios.
The `ConnectionSpec` fields are `{part_index_a, socket_id_a, part_index_b, socket_id_b}`
(fragments are inserted before joints in the resulting `part_ids` vector):

```cpp
namespace descriptors = steamrot::logic::descriptors;

steamrot::tests::PartLibraryBuilder builder{lib};
// fragment[0].socket[0] → joint[0].socket[0]
steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
    {"fragment_two_sockets"}, {"joint_two_sockets"}, {{0, 0, 1, 0}});

// Pass scaffold.parts (the PartGraph) to descriptors:
REQUIRE(descriptors::my_descriptor(result.scaffold.parts, result.part_ids[0]));
```

### Testing ContextualNodeDescriptors and ChainDescriptors

Pass `parts` and the part ID:

```cpp
namespace descriptors = steamrot::logic::descriptors;

descriptors::ContextualNodeDescriptor cnd =
    descriptors::is_connected_to(descriptors::is_fragment);
REQUIRE(cnd(result.scaffold.parts, result.part_ids[1]));
```

### Testing GraphDescriptors

Pass the whole `PartGraph`:

```cpp
namespace descriptors = steamrot::logic::descriptors;

descriptors::GraphDescriptor gd =
    descriptors::any_node_satisfies(
        descriptors::lift(descriptors::is_terminal));
REQUIRE(gd(result.scaffold.parts));
```

---

## Common Patterns

### Parameterised descriptors (factories)

When the descriptor logic depends on a runtime value (count, threshold), write a
factory function that returns a descriptor by value:

```cpp
// Declaration
NodeDescriptor has_exactly_n_edges(size_t n);

// Use
namespace descriptors = steamrot::logic::descriptors;
descriptors::NodeDescriptor has_two = descriptors::has_exactly_n_edges(2);
```

### Named constant descriptors

When the descriptor has a fixed, well-understood meaning, declare it as an
`extern const` and give it a descriptive verb-phrase name:

```cpp
extern const NodeDescriptor is_terminal;   // at most 1 connection
extern const NodeDescriptor is_serial;     // exactly 2 connections
extern const NodeDescriptor is_branched;   // at least 3 connections
```

### Building on existing descriptors

Prefer composing existing descriptors with `and_`/`or_`/`not_` before writing a
new lambda from scratch. This keeps the predicate vocabulary small and each
piece independently testable.

---

## Future Implementation Notes

The following are planned but not yet implemented:

1. **ChainDescriptorBuilder DFS traversal** (`ChainDescriptorBuilder.h` TODO)
   The builder captures the step list but `End()` always returns `false`. A
   depth-first walk from the anchor node (via `SocketData::connected_to`),
   matching each step predicate in order, needs to be written. Cycle detection
   (via `std::unordered_set<uint32_t>` visited set) and branching (match any
   valid path) should be considered.

2. **ContextualNodeDescriptor modifier library**
   Common factories (`is_connected_to`, `exactly_n_of`, `at_least_n_of`) are not
   yet declared in `descriptors_node_descriptors.h`. Add them as needed following
   the `ContextualNodeDescriptor` pattern above.

3. **GraphDescriptor consumption in Logic**
   `GrimoireMachinaActionLogic` (or a dedicated analysis step) should evaluate
   `GraphDescriptor` instances to gate gameplay actions (e.g. "is the assembled
   machine structurally valid?").

4. **New scaffold scenarios**
   Add topologies to `TestPartLibrary` (e.g. `StarTopology`, `LongLinearChain`)
   as chain and graph descriptors that need richer structures are added.

5. **EdgeDescriptor integration**
   A future `EdgeDescriptor` type alias could be added to express
   connection-type constraints (e.g. "connected via a fragment-to-joint edge
   only"). It would operate on `SocketData` or `SocketConnection` directly.

---

## Best Practices

- **Choose the narrowest level that answers the question.** A `NodeDescriptor`
  is faster and easier to test than a `GraphDescriptor`; only widen scope when
  necessary.
- **Name descriptors as verb phrases** that read as questions: `is_terminal`,
  `is_connected_to`, `has_exactly_n_edges`.
- **Test named constants with `CheckNodeDescriptorForAllScenarios`** to catch
  edge cases across all standard topologies.
- **Test factory functions** with at least boundary values (0, 1, n) plus a
  scenario that covers the factory's expected `true` case.
- **Do not add descriptor member variables to Logic classes.** Descriptors are
  free-function values. Store them as local variables or static constants inside
  the free functions that use them.
- **Never pass a `GraphDescriptor` into a combinator or modifier.** It is the
  terminal type and has no meaningful further composition.
- **Always pass `scaffold.parts`** (the `PartGraph`) to descriptor call sites,
  never the whole `MachinaFormScaffold`.

---

## Troubleshooting

| Symptom                                                         | Likely Cause                                     | Fix                                                                                                      |
| --------------------------------------------------------------- | ------------------------------------------------ | -------------------------------------------------------------------------------------------------------- |
| `and_()` / `or_()` compile error about deduced types            | Both arguments must be the same descriptor level | Use `lift()` to promote the narrower argument                                                            |
| `ChainDescriptorBuilder` descriptor always returns `false`      | DFS traversal not yet implemented                | Do not rely on `ChainDescriptorBuilder` results until the TODO in `ChainDescriptorBuilder.h` is resolved |
| `CheckNodeDescriptorForAllScenarios` test fails on one scenario | Node order mismatch                              | Arrays are fragments-first then joints; verify insertion order in `TestPartLibrary`                      |
| Segfault in descriptor accessing FlatBuffers field              | Unguarded access to a null optional field        | Guard all string, vector, and nested-table accesses: `if (data->field()) ...`                            |
| Custom scaffold node index unexpected                           | `PartLibraryBuilder` insertion order             | Fragments are inserted first (`part_ids[0..F-1]`), joints follow (`part_ids[F..F+J-1]`)                 |
| Linker error: undefined reference to descriptor                 | `extern const` declared but not defined          | Add the definition in the corresponding `.cpp` file                                                      |

---

## Summary Checklist

- [ ] Chose the correct descriptor level using the decision flowchart
- [ ] Checked whether existing descriptors can be composed instead
- [ ] Declared the descriptor in the appropriate level header (`descriptors_node_descriptors.h`, etc.)
- [ ] Defined the descriptor in the appropriate level source (lambda takes `(const PartGraph&, uint32_t)`)
- [ ] Added a `TEST_CASE` in `tests/unit/logic/descriptors/descriptors_node_descriptors.test.cpp`
- [ ] Used `CheckNodeDescriptorForAllScenarios` for named constants
- [ ] Verified arrays are fragments-first, then joints
- [ ] All descriptor call sites pass `scaffold.parts` (not `scaffold`)
- [ ] Built and all tests pass
- [ ] Updated `docs/workflows/adding_descriptors.md` if the descriptor API changed
