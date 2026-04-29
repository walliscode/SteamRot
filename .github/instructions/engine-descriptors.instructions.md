# Engine Descriptors: Adding and Testing Graph Descriptors

**What this covers:** The four-level descriptor hierarchy used to query a
`PartGraph`, the rules for choosing the right level, how to implement and
register new descriptors, how to write tests against `TestPartLibrary`
scenarios, and notes on planned future work.

---

## 1. The descriptor hierarchy

Descriptors are typed predicates (callable objects stored in `std::function`)
that answer questions about a `PartGraph`. There are four levels, ordered from
narrowest to broadest scope:

| Type | Signature | Scope |
|---|---|---|
| `NodeDescriptor` | `bool(const PartNode&)` | One node's own data |
| `ContextualNodeDescriptor` | `bool(const PartGraph&, const PartNode&)` | One node + its direct neighbours |
| `ChainDescriptor` | `bool(const PartGraph&, const PartNode&)` | Multi-hop walk from one anchor |
| `GraphDescriptor` | `bool(const PartGraph&)` | Whole graph, no anchor |

All four type aliases live in `src/types/entity/PartGraph.h`.
The generic operations (`lift`, `and_`, `or_`, `not_`, `any_node_satisfies`,
`all_nodes_satisfy`, `ChainDescriptorBuilder`) live in
`src/types/entity/descriptor_ops.h`.

---

## 2. Decision flowchart — which level to use

```
Does your predicate need MORE than one node?
│
├── No  ──────────────────────────────────►  NodeDescriptor
│                                            bool(const PartNode&)
└── Yes
     │
     ├── Only the immediate neighbours
     │   (depth 1, read via graph.edges)?
     │    └──────────────────────────────►  ContextualNodeDescriptor
     │                                      bool(const PartGraph&, const PartNode&)
     │
     ├── A walk or path through multiple
     │   hops (DFS/BFS traversal needed)?
     │    └──────────────────────────────►  ChainDescriptor
     │                                      (build with ChainDescriptorBuilder)
     │
     └── No anchor at all — a question
         about the whole assembled machine?
          └─────────────────────────────►  GraphDescriptor
                                           (derive via any_node_satisfies /
                                            all_nodes_satisfy)
```

Key rules:
- `GraphDescriptor` is **terminal** — never feed it back into a modifier.
- Never flow information **downward**: `ChainDescriptor` must not consume a
  `GraphDescriptor`.
- Use `lift()` to promote a narrower descriptor to a wider one without
  duplicating logic.

---

## 3. Where new descriptors live

| Descriptor level | Implementation file |
|---|---|
| `NodeDescriptor` constants/factories | `src/logic/analysis_grimoire_machina.cpp` |
| `ContextualNodeDescriptor` modifiers | `src/logic/analysis_grimoire_machina.cpp` |
| `ChainDescriptor` (via builder) | `src/logic/analysis_grimoire_machina.cpp` |
| `GraphDescriptor` | Derived in the Logic/Action layer via `any_node_satisfies` |
| Generic infrastructure | `src/types/entity/descriptor_ops.h` (header-only) |

Declare new descriptors in the matching `.h` file and define them in the `.cpp`.
Follow the `extern const NodeDescriptor name;` pattern for named predicates,
and the `Type factory_name(args)` pattern for parameterised factories.

---

## 4. Step-by-step: adding a NodeDescriptor

### 4a. Declare in the header

In `src/logic/analysis_grimoire_machina.h`, inside the
`steamrot::logic::analysis::grimoire_machina` namespace, add:

```cpp
/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when <description>.
/////////////////////////////////////////////////
extern const NodeDescriptor my_new_descriptor;
```

For a factory (parameterised descriptor):

```cpp
/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when <description>.
///
/// @param n <parameter description>.
/// @return NodeDescriptor returning true when <condition>.
/////////////////////////////////////////////////
NodeDescriptor my_factory(size_t n);
```

### 4b. Define in the source

In `src/logic/analysis_grimoire_machina.cpp`:

```cpp
/////////////////////////////////////////////////
const NodeDescriptor my_new_descriptor = [](const PartNode &node) -> bool {
  return /* your condition */;
};

/////////////////////////////////////////////////
NodeDescriptor my_factory(size_t n) {
  return [n](const PartNode &node) -> bool {
    return /* your condition involving n */;
  };
}
```

### 4c. No CMake changes needed

Both files are already compiled into the `logic` target. No edits to
`CMakeLists.txt` are required.

---

## 5. Step-by-step: adding a ContextualNodeDescriptor

A contextual descriptor answers "is this node X, given its neighbours?"

### 5a. Declare a modifier function in the header

```cpp
/////////////////////////////////////////////////
/// @brief Returns a ContextualNodeDescriptor that returns true when the
///        node is connected to at least one node satisfying @p nd.
///
/// @param nd NodeDescriptor to test against each neighbour.
/// @return ContextualNodeDescriptor.
/////////////////////////////////////////////////
ContextualNodeDescriptor is_connected_to(NodeDescriptor nd);
```

### 5b. Implement in the source

```cpp
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

## 6. Step-by-step: adding a ChainDescriptor

A chain descriptor answers a structural question about a walk from one node.

### 6a. Use ChainDescriptorBuilder

`ChainDescriptorBuilder` lives in `descriptor_ops.h` and is available via
the `steamrot` namespace.

```cpp
// In analysis_grimoire_machina.cpp
const ChainDescriptor linear_3_chain =
    steamrot::ChainDescriptorBuilder{}
        .StartWith(is_terminal)
        .Then(is_serial)
        .End(is_terminal);
```

Declare it in the header as `extern const ChainDescriptor linear_3_chain;`.

### 6b. Wait for DFS implementation

`ChainDescriptorBuilder::End()` currently returns a descriptor that always
returns `false`. The TODO comment in `descriptor_ops.h` marks the DFS
traversal work needed. Do not write tests that expect `true` results from
`ChainDescriptorBuilder` until the traversal is implemented.

---

## 7. Step-by-step: adding a GraphDescriptor

`GraphDescriptor` is always derived, never written from scratch. Use
`any_node_satisfies` or `all_nodes_satisfy` from `descriptor_ops.h`:

```cpp
// "Does the graph contain at least one terminal fragment?"
GraphDescriptor has_terminal_fragment =
    steamrot::any_node_satisfies(
        steamrot::lift(agm::and_(agm::is_terminal, agm::is_fragment)));

// "Are all nodes either fragments or joints?" (always true by construction)
GraphDescriptor all_known_types =
    steamrot::all_nodes_satisfy(
        steamrot::lift(agm::or_(agm::is_fragment, agm::is_joint)));
```

`GraphDescriptor` instances are consumed in Logic/Action classes (e.g.
`GrimoireMachinaActionLogic`) as final gate checks. They are never passed back
into `is_connected_to`, `any_node_satisfies`, or combinators.

---

## 8. Writing tests

All descriptor tests live in
`tests/unit/logic/analysis_grimoire_machina.test.cpp` and use
`TestPartLibrary` and `CheckNodeDescriptorForAllScenarios`.

### 8a. Use CheckNodeDescriptorForAllScenarios for named predicates

This helper runs your descriptor against every pre-built scaffold scenario and
CHECKs per-node results against a `ScaffoldScenarioExpectations` struct.
Node order within each array is **fragments first, then joints**, matching the
insertion order documented in `part_library.h`.

```cpp
TEST_CASE("my_new_descriptor tests", "[unit][analysis][grimoire_machina]") {
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();

  SECTION("Analyses all ScaffoldScenarios correctly for my_new_descriptor") {
    steamrot::tests::CheckNodeDescriptorForAllScenarios(
        agm::my_new_descriptor,
        {.linear_chain   = {/* expected per node */},
         .ring            = {/* expected per node */},
         .isolated_pair   = {/* expected per node */},
         .simple_branch   = {/* expected per node */}},
        lib);
  }
}
```

### 8b. Build custom scaffolds for edge cases

Use `PartLibraryBuilder` to construct scaffolds that the standard scenarios do
not cover:

```cpp
steamrot::tests::PartLibraryBuilder builder{lib};
steamrot::tests::ScaffoldResult result = builder.MakeConnectedScaffold(
    {"fragment_two_sockets"}, {"joint_two_sockets"}, {{0, 0, 1, 0}});
steamrot::PartGraph graph = agm::build_part_graph(result.scaffold);
REQUIRE(agm::my_new_descriptor(graph.nodes[0]));
```

### 8c. Test ContextualNodeDescriptors and ChainDescriptors with graph + node

Pass both arguments:

```cpp
steamrot::ContextualNodeDescriptor cnd = agm::is_connected_to(agm::is_fragment);
REQUIRE(cnd(graph, graph.nodes[0]));
```

### 8d. Test GraphDescriptors against a whole graph

```cpp
steamrot::GraphDescriptor gd =
    steamrot::any_node_satisfies(steamrot::lift(agm::is_terminal));
REQUIRE(gd(graph));
```

### 8e. No new CMake edits needed for tests

`analysis_grimoire_machina.test.cpp` is already listed in
`tests/unit/logic/CMakeLists.txt`.

---

## 9. Lifting and composing descriptors

| Operation | Signature | Purpose |
|---|---|---|
| `lift(nd)` | `NodeDescriptor → ContextualNodeDescriptor` | Ignore the graph, apply nd to the node |
| `lift_to_chain(nd)` | `NodeDescriptor → ChainDescriptor` | Same as lift but typed as ChainDescriptor |
| `any_node_satisfies(cd)` | `ChainDescriptor → GraphDescriptor` | True if any node satisfies cd |
| `all_nodes_satisfy(cd)` | `ChainDescriptor → GraphDescriptor` | True if every node satisfies cd |
| `and_(a, b)` | `Desc × Desc → Desc` | Both must be true; works at any level |
| `or_(a, b)` | `Desc × Desc → Desc` | Either must be true; works at any level |
| `not_(a)` | `Desc → Desc` | Negate; works at any level |

`and_`, `or_`, `not_` are templated — both arguments must be the **same**
descriptor level. Mixing levels is a compile error.

---

## 10. Future implementation notes

The following are not yet implemented and represent planned work:

1. **ChainDescriptorBuilder DFS traversal** (`descriptor_ops.h` TODO).
   The builder captures the step list but the `End()` method always returns
   `false`. A depth-first walk from the anchor node, matching each step
   predicate in order, needs to be implemented. Consider handling cycles
   (mark visited nodes) and branching (match any valid path).

2. **ContextualNodeDescriptor modifiers** — common modifier factories
   (`is_connected_to`, `exactly_n_of`, `at_least_n_of`) are not yet declared
   in `analysis_grimoire_machina.h`. Add them as needed following section 5.

3. **GraphDescriptor consumption in Logic** — `GrimoireMachinaActionLogic`
   (or a dedicated analysis step) should hold and evaluate `GraphDescriptor`
   instances to gate gameplay actions (e.g. "is the assembled machine valid?").

4. **ScaffoldScenario expansion** — add new scenarios to `TestPartLibrary`
   (e.g. `StarTopology`, `LongLinearChain`) as chain/graph descriptors that
   need richer topologies are added.

5. **EdgeDescriptor integration** — `EdgeDescriptor` is declared in
   `PartGraph.h` but has no concrete instances or tests. Future modifiers
   could accept edge predicates to express connection-type constraints
   (e.g. "connected via a fragment-joint edge only").

---

## Key rules

- Never add `std::shared_ptr<Subscriber>` members to a Logic class to store
  a descriptor. Descriptors are free-function values, not objects with
  lifetime management.
- Never pass a `GraphDescriptor` into `any_node_satisfies`, `all_nodes_satisfy`,
  or any combinator — it is a terminal type.
- Always guard FlatBuffers field accesses in any descriptor that reads
  component data serialised via FlatBuffers.
- Keep `ProcessLogic()` as a clean list of named free-function calls; evaluate
  descriptors inside the free functions, not directly in `ProcessLogic`.

## Gotchas

- `ContextualNodeDescriptor` and `ChainDescriptor` are the **same underlying
  `std::function` type**. The type system cannot distinguish them; the
  distinction is purely semantic and enforced by naming convention.
- `and_/or_/not_` require both arguments to be the same type. Passing a
  `NodeDescriptor` and a `ContextualNodeDescriptor` to `and_()` will not
  compile. Use `lift()` first if needed.
- Node order in `ScaffoldScenarioExpectations` arrays is **fragments first,
  then joints** — mismatching the order silently flips expected values.
- `ChainDescriptorBuilder::End()` always returns `false` until the DFS
  traversal is implemented (see section 10).

## Related files

- `src/types/entity/PartGraph.h` — type aliases for all four descriptor levels
- `src/types/entity/descriptor_ops.h` — lift, combinators, graph queries,
  ChainDescriptorBuilder
- `src/logic/analysis_grimoire_machina.h` / `.cpp` — concrete descriptors and
  `build_part_graph`
- `tests/unit/logic/analysis_grimoire_machina.test.cpp` — descriptor tests
- `tests/unit/logic/part_library.h` / `.cpp` — TestPartLibrary,
  CheckNodeDescriptorForAllScenarios, ScaffoldScenario

---

## How to improve this workflow

If you followed this file and found a step that was wrong, missing, or
out-of-date, please report it using the process in
[`meta-workflows.instructions.md`](meta-workflows.instructions.md) (section 3).
