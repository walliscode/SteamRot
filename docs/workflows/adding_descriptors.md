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
that answer structural questions about a `PartGraph`. A `PartGraph` is a
lightweight, non-owning snapshot of a `MachinaFormScaffold` as an adjacency
structure, with one `PartNode` per part and one `PartEdge` per connection.

There are four descriptor levels, ordered from narrowest to broadest scope:

| Level | Type alias | Signature |
| --- | --- | --- |
| Node | `NodeDescriptor` | `bool(const PartNode&)` |
| Contextual node | `ContextualNodeDescriptor` | `bool(const PartGraph&, const PartNode&)` |
| Chain | `ChainDescriptor` | `bool(const PartGraph&, const PartNode&)` |
| Graph | `GraphDescriptor` | `bool(const PartGraph&)` |

All four aliases live in `src/types/entity/PartGraph.h` in the `steamrot` namespace.
Generic lifting utilities, graph queries, and combinators live in
`src/types/entity/descriptor_ops.h` under the `steamrot::descriptors` namespace.
The `ChainDescriptorBuilder` class lives in `src/logic/ChainDescriptorBuilder.h`
under the `steamrot::logic` namespace.

---

## The Descriptor Hierarchy

```
NodeDescriptor           bool(const PartNode&)
       │  examined by
       │    lift()
       ▼
ContextualNodeDescriptor bool(const PartGraph&, const PartNode&)
       │  same signature as
       │
ChainDescriptor          bool(const PartGraph&, const PartNode&)
       │  consumed by
       │    any_node_satisfies()
       │    all_nodes_satisfy()
       ▼
GraphDescriptor          bool(const PartGraph&)
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
- Combinators (`and_`, `or_`, `not_`) are generic templates. Both arguments
  must be the **same descriptor level** — mixing levels is a compile error.

---

## When to Add a Descriptor — Decision Flowchart

```
Start: I need to query something about the assembled machine.
│
├─ Does my question need MORE than one node?
│   │
│   ├── No ──────────────────────────────────────► NodeDescriptor
│   │                                               bool(const PartNode&)
│   │                                               Declare in analysis_grimoire_machina.h
│   │                                               Define in analysis_grimoire_machina.cpp
│   │
│   └── Yes
│        │
│        ├── Only direct neighbours (depth 1)?
│        │    └────────────────────────────────► ContextualNodeDescriptor
│        │                                        bool(const PartGraph&, const PartNode&)
│        │                                        Use is_connected_to() pattern (section 5b)
│        │
│        ├── Multi-hop walk / path matching?
│        │    └────────────────────────────────► ChainDescriptor
│        │                                        bool(const PartGraph&, const PartNode&)
│        │                                        Use ChainDescriptorBuilder (section 6a)
│        │
│        └── No anchor — whole machine question?
│             └────────────────────────────────► GraphDescriptor
│                                                 bool(const PartGraph&)
│                                                 Derive via any_node_satisfies() or
│                                                 all_nodes_satisfy()
│
├─ Can I express it as a composition of existing descriptors?
│   └── Yes → use and_() / or_() / not_() / lift()
│              No new declaration needed.
│
└─ Does the question belong in a Logic/Action class (gates a gameplay decision)?
    └── Yes → GraphDescriptor consumed directly in that class.
              Define there, not in analysis_grimoire_machina.
```

---

## Key Files

| File | Purpose |
| --- | --- |
| `src/types/entity/PartGraph.h` | Type aliases for all four descriptor levels (`steamrot` namespace) |
| `src/types/entity/descriptor_ops.h` | `lift`, `lift_to_chain`, combinators, graph queries (`steamrot::descriptors` namespace) |
| `src/logic/ChainDescriptorBuilder.h` | `ChainDescriptorBuilder` class (`steamrot::logic` namespace) |
| `src/logic/analysis_grimoire_machina.h` | Declarations of concrete descriptors and `build_part_graph` |
| `src/logic/analysis_grimoire_machina.cpp` | Definitions of concrete descriptors and `build_part_graph` |
| `tests/unit/logic/analysis_grimoire_machina.test.cpp` | Descriptor unit tests |
| `tests/unit/logic/part_library.h` / `.cpp` | `TestPartLibrary`, `CheckNodeDescriptorForAllScenarios` |

> **No CMake changes are needed.** All source files are already compiled into
> the `logic` target; the test file is already listed in
> `tests/unit/logic/CMakeLists.txt`.

---

## Step-by-Step Workflow

### Step 1: Choose the Correct Descriptor Level

Use the [Decision Flowchart](#when-to-add-a-descriptor--decision-flowchart)
above. If in doubt, start with the narrowest level that answers your question.

### Step 2: Declare the Descriptor

Open `src/logic/analysis_grimoire_machina.h`.

Add your declaration inside the
`steamrot::logic::analysis::grimoire_machina` namespace, grouped with
descriptors of the same level:

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

Open `src/logic/analysis_grimoire_machina.cpp`.

Add the definition inside the same namespace:

```cpp
/////////////////////////////////////////////////
const NodeDescriptor my_descriptor = [](const PartNode &node) -> bool {
  return /* your condition */;
};
```

For a factory:

```cpp
/////////////////////////////////////////////////
NodeDescriptor my_factory(size_t n) {
  return [n](const PartNode &node) -> bool {
    return /* your condition involving n */;
  };
}
```

### Step 4: Write Unit Tests

Open `tests/unit/logic/analysis_grimoire_machina.test.cpp`.

Add a `TEST_CASE` using `CheckNodeDescriptorForAllScenarios` (see
[Testing Descriptors](#testing-descriptors)).

### Step 5: Build and Test

```bash
cmake --build --preset Debug
ctest --preset Debug -R analysis_grimoire_machina
```

---

## Adding Each Descriptor Level

### NodeDescriptor

A `NodeDescriptor` answers a question about **one node's own data** — its type
(fragment vs joint) or its edge count.

#### Declaration (`analysis_grimoire_machina.h`)

```cpp
/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when the node has exactly
/// @p n PartEdges.
///
/// @param n Number of edges.
/// @return NodeDescriptor returning true when edges count == n.
/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n);

/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when the node is a terminal
/// (at most 1 connected socket).
/////////////////////////////////////////////////
extern const NodeDescriptor is_terminal;
```

#### Definition (`analysis_grimoire_machina.cpp`)

```cpp
/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n) {
  return [n](const PartNode &node) -> bool {
    return node.edge_indices.size() == n;
  };
}

/////////////////////////////////////////////////
const NodeDescriptor is_terminal = has_maximum_n_edges(1);
```

---

### ContextualNodeDescriptor

A `ContextualNodeDescriptor` answers a question about **a node and its direct
neighbours** (depth-1 walk via `graph.edges`).

#### Declaration (`analysis_grimoire_machina.h`)

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

#### Definition (`analysis_grimoire_machina.cpp`)

```cpp
/////////////////////////////////////////////////
ContextualNodeDescriptor is_connected_to(NodeDescriptor nd) {
  return [nd = std::move(nd)](const PartGraph &graph,
                               const PartNode &node) -> bool {
    for (size_t edge_idx : node.edge_indices) {
      const PartEdge &edge = graph.edges[edge_idx];
      const uint32_t neighbour_id =
          (edge.part_id_a == node.id) ? edge.part_id_b : edge.part_id_a;
      auto it = graph.node_index_by_id.find(neighbour_id);
      if (it != graph.node_index_by_id.end() &&
          nd(graph.nodes[it->second]))
        return true;
    }
    return false;
  };
}
```

---

### ChainDescriptor

A `ChainDescriptor` answers a structural question about a **multi-hop walk**
from an anchor node. Use `ChainDescriptorBuilder` from
`src/logic/ChainDescriptorBuilder.h` (`steamrot::logic` namespace).

#### Declaration (`analysis_grimoire_machina.h`)

```cpp
/////////////////////////////////////////////////
/// @brief ChainDescriptor that returns true when the anchor starts a
///        linear 3-node chain: terminal → serial → terminal.
/////////////////////////////////////////////////
extern const ChainDescriptor linear_3_chain;
```

#### Definition (`analysis_grimoire_machina.cpp`)

```cpp
/////////////////////////////////////////////////
// NOTE: ChainDescriptorBuilder::End() always returns false until the
// DFS traversal is implemented in ChainDescriptorBuilder.h.
const ChainDescriptor linear_3_chain =
    steamrot::logic::ChainDescriptorBuilder{}
        .StartWith(is_terminal)
        .Then(is_serial)
        .End(is_terminal);
```

> ⚠️ **DFS traversal is not yet implemented.** `ChainDescriptorBuilder::End()`
> currently returns a descriptor that always returns `false`. Do not write tests
> that expect `true` results until the TODO in `ChainDescriptorBuilder.h` is resolved.

---

### GraphDescriptor

A `GraphDescriptor` answers a question about the **whole graph** — no anchor
node. Always derive it from `any_node_satisfies` or `all_nodes_satisfy`;
never write a `GraphDescriptor` lambda from scratch.

#### Where it lives

`GraphDescriptor` instances are typically defined directly in the Logic or
Action class that consumes them (e.g. `GrimoireMachinaActionLogic`), not in
`analysis_grimoire_machina`.

#### Example

```cpp
// "Does the graph contain at least one terminal fragment?"
steamrot::GraphDescriptor has_terminal_fragment =
    steamrot::descriptors::any_node_satisfies(
        steamrot::descriptors::lift(agm::and_(agm::is_terminal, agm::is_fragment)));

// "Are all nodes either a fragment or a joint?"
steamrot::GraphDescriptor all_known_types =
    steamrot::descriptors::all_nodes_satisfy(
        steamrot::descriptors::lift(agm::or_(agm::is_fragment, agm::is_joint)));

// Evaluate
if (has_terminal_fragment(graph)) {
  // gate some gameplay action
}
```

---

## Lifting and Composing Descriptors

| Operation | Input → Output | Purpose |
| --- | --- | --- |
| `steamrot::descriptors::lift(nd)` | `NodeDescriptor` → `ContextualNodeDescriptor` | Promote narrower descriptor without duplicating logic |
| `steamrot::descriptors::lift_to_chain(nd)` | `NodeDescriptor` → `ChainDescriptor` | Same, typed as `ChainDescriptor` |
| `steamrot::descriptors::any_node_satisfies(cd)` | `ChainDescriptor` → `GraphDescriptor` | True if any node satisfies `cd` |
| `steamrot::descriptors::all_nodes_satisfy(cd)` | `ChainDescriptor` → `GraphDescriptor` | True if every node satisfies `cd` |
| `steamrot::descriptors::and_(a, b)` | `Desc × Desc` → `Desc` | Both must be true; same level required |
| `steamrot::descriptors::or_(a, b)` | `Desc × Desc` → `Desc` | Either must be true; same level required |
| `steamrot::descriptors::not_(a)` | `Desc` → `Desc` | Negate; same level required |

> **Note:** When working inside `analysis_grimoire_machina.h/cpp`, the `and_`,
> `or_`, and `not_` combinators are available as `agm::and_`, `agm::or_`,
> `agm::not_` via the `using steamrot::descriptors::and_` declarations in
> `analysis_grimoire_machina.h`. The full `steamrot::descriptors::` prefix is
> always valid from any context.

### Example: composing without declaring a new descriptor

```cpp
// "Is this node a fragment with exactly 1 edge?"
steamrot::NodeDescriptor is_terminal_fragment =
    agm::and_(agm::is_fragment, agm::is_terminal);

// Lift to work in a ContextualNodeDescriptor context
steamrot::ContextualNodeDescriptor cnd =
    steamrot::descriptors::lift(is_terminal_fragment);
```

---

## Testing Descriptors

All descriptor tests live in
`tests/unit/logic/analysis_grimoire_machina.test.cpp`.

### Using CheckNodeDescriptorForAllScenarios

This helper runs a `NodeDescriptor` against every pre-built topology stored in
`TestPartLibrary` and `CHECK`s per-node results.

**Node order within each array is fragments first, then joints**, matching
insertion order. Mismatching the order silently flips expected values.

```cpp
TEST_CASE("my_descriptor tests", "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();

  SECTION("Analyses all ScaffoldScenarios correctly") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        agm::my_descriptor,
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

| Scenario key | Topology | Node count |
| --- | --- | --- |
| `LinearChain` | fragment – joint – fragment (both sockets connected) | 3 |
| `Ring` | joint – joint – joint (ring of joints) | 3 |
| `IsolatedPair` | fragment – fragment (no connections) | 2 |
| `SimpleBranch` | three fragments all connected to one joint | 4 |

### Building a custom scaffold

Use `PartLibraryBuilder` for topologies not covered by the standard scenarios:

```cpp
steamrot::tests::PartLibraryBuilder builder{lib};
// fragment[0].socket[0] → joint[0].socket[0]
steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
    {"fragment_two_sockets"}, {"joint_two_sockets"}, {{0, 0, 1, 0}});
steamrot::PartGraph graph = agm::build_part_graph(result.scaffold);

REQUIRE(agm::my_descriptor(graph.nodes[0]));
```

### Testing ContextualNodeDescriptors and ChainDescriptors

Pass graph + node:

```cpp
steamrot::ContextualNodeDescriptor cnd = agm::is_connected_to(agm::is_fragment);
REQUIRE(cnd(graph, graph.nodes[1]));
```

### Testing GraphDescriptors

Pass the whole graph:

```cpp
steamrot::GraphDescriptor gd =
    steamrot::descriptors::any_node_satisfies(
        steamrot::descriptors::lift(agm::is_terminal));
REQUIRE(gd(graph));
```

---

## Common Patterns

### Parameterised descriptors (factories)

When the descriptor logic depends on a runtime value (count, threshold, name),
write a factory function that returns a descriptor by value:

```cpp
// Declaration
NodeDescriptor has_exactly_n_edges(size_t n);

// Use
steamrot::NodeDescriptor has_two = agm::has_exactly_n_edges(2);
```

### Named constant descriptors

When the descriptor has a fixed, well-understood meaning, declare it as an
`extern const` and give it a descriptive verb-phrase name:

```cpp
extern const NodeDescriptor is_terminal;   // at most 1 edge
extern const NodeDescriptor is_serial;     // exactly 2 edges
extern const NodeDescriptor is_branched;   // at least 3 edges
```

### Building on existing descriptors

Prefer composing existing descriptors with `and_`/`or_`/`not_` before writing
a new lambda from scratch. This keeps the predicate vocabulary small and each
piece independently testable.

---

## Future Implementation Notes

The following are planned but not yet implemented:

1. **ChainDescriptorBuilder DFS traversal** (`descriptor_ops.h` TODO)  
   The builder captures the step list but `End()` always returns `false`. A
   depth-first walk from the anchor node, matching each step predicate in
   order, needs to be written. Cycle detection (mark visited nodes) and
   branching (match any valid path) should be considered.

2. **ContextualNodeDescriptor modifier library**  
   Common factories (`is_connected_to`, `exactly_n_of`, `at_least_n_of`) are
   not yet declared in `analysis_grimoire_machina.h`. Add them as needed
   following the `ContextualNodeDescriptor` pattern above.

3. **GraphDescriptor consumption in Logic**  
   `GrimoireMachinaActionLogic` (or a dedicated analysis step) should evaluate
   `GraphDescriptor` instances to gate gameplay actions (e.g. "is the assembled
   machine structurally valid?").

4. **New scaffold scenarios**  
   Add topologies to `TestPartLibrary` (e.g. `StarTopology`,
   `LongLinearChain`) as chain and graph descriptors that need richer
   structures are added.

5. **EdgeDescriptor integration**  
   `EdgeDescriptor` is declared in `PartGraph.h` but has no concrete instances
   or tests. Future modifiers could accept edge predicates to express
   connection-type constraints (e.g. "connected via a fragment-to-joint edge
   only").

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

---

## Troubleshooting

| Symptom | Likely Cause | Fix |
| --- | --- | --- |
| `and_()` / `or_()` compile error about deduced types | Both arguments must be the same descriptor level | Use `lift()` to promote the narrower argument |
| `ChainDescriptorBuilder` descriptor always returns `false` | DFS traversal not yet implemented | Do not rely on `ChainDescriptorBuilder` results until the TODO in `ChainDescriptorBuilder.h` is resolved |
| `CheckNodeDescriptorForAllScenarios` test fails on one scenario | Node order mismatch | Arrays are fragments-first then joints; verify insertion order in `TestPartLibrary` |
| Segfault in descriptor accessing FlatBuffers field | Unguarded access to a null optional field | Guard all string, vector, and nested-table accesses: `if (data->field()) ...` |
| Custom scaffold node index unexpected | `PartLibraryBuilder` insertion order | Fragments are inserted first (indices 0…F-1), joints follow (indices F…F+J-1) |

---

## Summary Checklist

- [ ] Chose the correct descriptor level using the decision flowchart
- [ ] Checked whether existing descriptors can be composed instead
- [ ] Declared the descriptor in `analysis_grimoire_machina.h`
- [ ] Defined the descriptor in `analysis_grimoire_machina.cpp`
- [ ] Added a `TEST_CASE` in `analysis_grimoire_machina.test.cpp`
- [ ] Used `CheckNodeDescriptorForAllScenarios` for named constants
- [ ] Verified arrays are fragments-first, then joints
- [ ] Built and all tests pass
