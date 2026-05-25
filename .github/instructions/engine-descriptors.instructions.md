# Engine Descriptors: Adding and Testing Descriptors

**What this covers:** The remaining descriptor hierarchy used to query a
`PartGraph`, how to choose between node and chain descriptors, how to implement
and register them, and how to keep the human-facing workflow documentation in
sync.

---

## 1. The descriptor hierarchy

Descriptors are typed predicates (callable objects stored in `std::function`)
that answer questions about a `PartGraph`.

`PartGraph` is a type alias defined in `src/types/entity/MachinaFormScaffold.h`:

```cpp
using PartGraph = std::map<uint32_t, std::variant<JointInstance, FragmentInstance>>;
```

It is exposed as `MachinaFormScaffold::parts`. All descriptors receive
`const PartGraph&` directly. Call sites always pass `scaffold.parts`.

There are two descriptor levels:

| Type | Signature | Scope |
|---|---|---|
| `NodeDescriptor` | `NodeDescriptorResult(const PartGraph&, uint32_t)` | One node's own data |
| `ChainDescriptor` | `ChainDescriptorResult(const PartGraph&, uint32_t)` | Multi-hop walk from one anchor |

The descriptor headers live in `src/logic/descriptors/` in the
`steamrot::logic::descriptors` namespace:
- `NodeDescriptor` → `descriptors_node_descriptors.h`
- `ChainDescriptor`, `lift_to_chain` → `descriptors_chain_descriptors.h`
- `and_`, `or_`, `not_` → `descriptors_general.h`
- `ChainDescriptorBuilder` → `ChainDescriptorBuilder.h`

---

## 2. Decision flowchart — which level to use

```
Does your predicate only inspect one node's own data?
│
├── Yes ───────────────────────────────────────► NodeDescriptor
│                                                NodeDescriptorResult(const PartGraph&, uint32_t)
└── No
     │
     └── Does it need a multi-hop walk from one anchor node?
          └────────────────────────────────────► ChainDescriptor
                                                   ChainDescriptorResult(const PartGraph&, uint32_t)
                                                   Build with ChainDescriptorBuilder
```

Key rules:
- Prefer `NodeDescriptor` whenever the answer depends only on the anchor node.
- Use `ChainDescriptor` for traversal-based questions.
- Use `lift_to_chain()` to reuse node logic inside a chain descriptor.

---

## 3. Where new descriptors live

| Descriptor level | Header file | Source file |
|---|---|---|
| `NodeDescriptor` constants/factories | `src/logic/descriptors/descriptors_node_descriptors.h` | `descriptors_node_descriptors.cpp` |
| `ChainDescriptor` (via builder) | `src/logic/descriptors/descriptors_chain_descriptors.h` | `descriptors_chain_descriptors.cpp` |
| Combinators (`and_`, `or_`, `not_`) | `src/logic/descriptors/descriptors_general.h` | header-only |
| `ChainDescriptorBuilder` | `src/logic/descriptors/ChainDescriptorBuilder.h` | `ChainDescriptorBuilder.cpp` |

Follow the `extern const NodeDescriptor name;` pattern for named predicates and
the `Type factory_name(args)` pattern for parameterised factories.

---

## 4. Step-by-step: adding a NodeDescriptor

Predicates access the part variant via `parts.find(id)` and read
`connection_count` from `PartInstance` using `std::visit`.

### 4a. Declare in the header

In `src/logic/descriptors/descriptors_node_descriptors.h`, inside the
`steamrot::logic::descriptors` namespace, add:

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

In `src/logic/descriptors/descriptors_node_descriptors.cpp`:

```cpp
/////////////////////////////////////////////////
const NodeDescriptor my_new_descriptor{
    "my_new_descriptor",
    [](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
      const auto part_it = parts.find(id);
      if (part_it == parts.end())
        return NodeDescriptorResult{false, "incorrect key: part_id=" +
                                               std::to_string(id)};
      const size_t count = std::visit(
          [](const auto &inst) -> size_t { return inst.connection_count; },
          part_it->second);
      const bool ok = /* your condition */;
      return NodeDescriptorResult{ok};
    }};
```

---

## 5. Step-by-step: adding a ChainDescriptor

A chain descriptor answers a structural question about a walk from one node.

### 5a. Use ChainDescriptorBuilder

`ChainDescriptorBuilder` lives in `src/logic/descriptors/ChainDescriptorBuilder.h`
in the `steamrot::logic::descriptors` namespace.

```cpp
const ChainDescriptor linear_3_chain = ChainDescriptorBuilder{}
    .WhileIsTrue(is_serial)
    .Then(is_terminal)
    .Build("linear_3_chain")
    .value();
```

Declare it in `descriptors_chain_descriptors.h` as
`extern const ChainDescriptor linear_3_chain;`.

### 5b. Reuse node logic when helpful

Use `lift_to_chain()` when you need a chain descriptor that simply evaluates the
anchor node with existing node-level logic:

```cpp
ChainDescriptor terminal_node_only = lift_to_chain(is_terminal);
```

---

## 6. Writing tests

All descriptor tests live in
`tests/unit/logic/descriptors/descriptors_node_descriptors.test.cpp` and use
`TestPartLibrary` plus `CheckNodeDescriptorForAllScenarios` for node-level
predicates.

### 6a. Use CheckNodeDescriptorForAllScenarios for named node predicates

This helper runs your descriptor against every pre-built scaffold scenario and
CHECKs per-node results against a `ScaffoldScenarioExpectations` struct.

### 6b. Test chain descriptors with parts + anchor id

Pass `result.scaffold.parts` and the chosen part ID:

```cpp
namespace descriptors = steamrot::logic::descriptors;

auto result = descriptors::is_serial_chain(scaffold.parts, anchor_id);
REQUIRE(result.m_result == true);
```

---

## 7. Lifting and composing descriptors

| Operation | Signature | Purpose |
|---|---|---|
| `steamrot::logic::descriptors::lift_to_chain(nd)` | `NodeDescriptor → ChainDescriptor` | Reuse node logic in a chain context |
| `steamrot::logic::descriptors::and_(a, b)` | `Desc × Desc → Desc` | Both must be true; works within one descriptor level |
| `steamrot::logic::descriptors::or_(a, b)` | `Desc × Desc → Desc` | Either must be true; works within one descriptor level |
| `steamrot::logic::descriptors::not_(a)` | `Desc → Desc` | Negate; works within one descriptor level |

`and_`, `or_`, `not_` are templated — both arguments must be the same
descriptor level. Mixing levels is a compile error.

---

## 8. Future implementation notes

The following are not yet implemented and represent planned work:

1. **ChainDescriptorBuilder DFS traversal polish** — keep improving traversal
   behaviour, cycle handling, and branching diagnostics as the chain vocabulary
   grows.
2. **ScaffoldScenario expansion** — add new scenarios to `TestPartLibrary`
   (e.g. `StarTopology`, `LongLinearChain`) as chain descriptors need richer
   topologies.
3. **EdgeDescriptor integration** — a future `EdgeDescriptor` type alias could
   express connection-type constraints directly on `SocketData` or
   `SocketConnection`.

## Key rules

- Never add `std::shared_ptr<Subscriber>` members to a Logic class to store a
  descriptor. Descriptors are free-function values, not objects with lifetime
  management.
- Always guard FlatBuffers field accesses in any descriptor that reads component
  data serialised via FlatBuffers.
- Keep `ProcessLogic()` as a clean list of named free-function calls; evaluate
  descriptors inside the free functions, not directly in `ProcessLogic`.
- **Always pass `scaffold.parts`** (the `PartGraph`) to descriptor call sites,
  never the whole `MachinaFormScaffold`.
- **Always update `docs/workflows/adding_descriptors.md`** when making any
  change to descriptor type aliases, signatures, or patterns.

## Gotchas

- `and_/or_/not_` require both arguments to be the same type.
- Node order in `ScaffoldScenarioExpectations` arrays is **fragments first,
  then joints** — mismatching the order silently flips expected values.
- There is no `build_part_graph` function. The `PartGraph` is `scaffold.parts`
  directly — no build step required.

## Related files

- `src/types/entity/MachinaFormScaffold.h` — `PartGraph` type alias; `PartInstance`, `JointInstance`, `FragmentInstance`, `SocketMap`, `SocketData`, `SocketConnection`
- `src/types/logic/DescriptorResult.h` — `NodeDescriptorResult`, `ChainDescriptorResult`, `DescriptorResult`
- `src/logic/descriptors/descriptors_node_descriptors.h` / `.cpp` — `NodeDescriptor` and concrete node-level predicates
- `src/logic/descriptors/descriptors_chain_descriptors.h` / `.cpp` — `ChainDescriptor`, `lift_to_chain`, and concrete chain predicates
- `src/logic/descriptors/descriptors_general.h` — combinators (`and_`, `or_`, `not_`)
- `src/logic/descriptors/ChainDescriptorBuilder.h` / `.cpp` — `ChainDescriptorBuilder` class
- `tests/unit/logic/descriptors/descriptors_node_descriptors.test.cpp` — descriptor unit tests
- `tests/unit/logic/part_library.h` / `.cpp` — `TestPartLibrary`, `CheckNodeDescriptorForAllScenarios`, `ScaffoldScenario`
- `docs/workflows/adding_descriptors.md` — primary human-facing descriptor workflow

---

## How to improve this workflow

If you followed this file and found a step that was wrong, missing, or
out-of-date, please report it using the process in
[`meta-workflows.instructions.md`](meta-workflows.instructions.md) (section 3).
