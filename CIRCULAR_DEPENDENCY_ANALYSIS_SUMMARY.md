# Circular Dependency Analysis Summary

## Quick Answer

**Question:** Why does building `components` also build `matchers`?

**Answer:** A circular dependency exists where production code incorrectly depends on test code.

## The Cycle

```
components → user_interface → events → matchers → components (CYCLE!)
```

## The Problem

**File:** `src/events/CMakeLists.txt` (line 22)

The `events` library (production code) incorrectly links to `matchers` (test library):

```cmake
target_link_libraries(events PUBLIC
    ...
    matchers    # ← THIS IS THE PROBLEM
)
```

This creates:
1. **Circular dependency:** Building any target in the cycle requires building all targets
2. **Architectural violation:** Production code should never depend on test code

## Verification

✓ Checked all `src/events/` source files  
✓ **NO files use anything from matchers**  
✓ The dependency is unnecessary

## The Fix

Remove line 22 from `src/events/CMakeLists.txt`:

```cmake
target_link_libraries(events PUBLIC
    SFML::Graphics
    flatbuffers
    flatbuffers_headers
    stduuid
    magic_enum::magic_enum
    logger
    # matchers removed - was creating circular dependency
)
```

## Impact of Fix

- ✓ Breaks circular dependency
- ✓ Fixes architectural violation  
- ✓ Building `components` won't build `matchers`
- ✓ Tests still work (they link matchers directly)
- ✓ Reduces build time for production targets

## Detailed Analysis

See: `documentation/analysis/BUILD_DEPENDENCY_CIRCULAR_ISSUE.md`

---

**Note:** This is an **ANALYSIS ONLY** document. The issue describes a request for analysis, not a request to fix the problem. The fix should be implemented separately if desired.
