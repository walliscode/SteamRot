# Build Dependency Visualization

## Current State: WITH Circular Dependencies

### Primary Cycle (Production ↔ Test)

```
┌─────────────────────────────────────────────────────────────┐
│                    CIRCULAR DEPENDENCY                       │
│                                                              │
│    ┌──────────────┐                                         │
│    │  components  │◄────────────────────────────┐           │
│    │ (src/comp)   │                             │           │
│    └──────┬───────┘                             │           │
│           │                                     │           │
│           │ PUBLIC                              │           │
│           ▼                                     │           │
│    ┌──────────────────┐                        │           │
│    │ user_interface   │                        │           │
│    │ (src/user_int)   │                        │           │
│    └──────┬───────────┘                        │           │
│           │                                     │           │
│           │ PUBLIC                              │           │
│           ▼                                     │           │
│    ┌──────────────┐                            │           │
│    │   events     │                            │           │
│    │ (src/events) │                            │           │
│    └──────┬───────┘                            │           │
│           │                                     │           │
│           │ PUBLIC ❌                           │           │
│           ▼        PROBLEM!                    │           │
│    ┌──────────────┐                            │           │
│    │   matchers   │  ← TEST LIBRARY            │           │
│    │(tests/match) │                            │           │
│    └──────┬───────┘                            │           │
│           │                                     │           │
│           │ PUBLIC                              │           │
│           └─────────────────────────────────────┘           │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

**Impact:** Building any target in this cycle requires building ALL targets.

### Secondary Cycle (Test ↔ Test)

```
┌─────────────────────────────────────────┐
│        TEST LIBRARY CYCLE               │
│                                         │
│    ┌──────────────┐                    │
│    │ test_harness │◄──────────┐        │
│    └──────┬───────┘           │        │
│           │                   │        │
│           │ PUBLIC            │        │
│           ▼                   │        │
│    ┌──────────────┐           │        │
│    │   matchers   │           │        │
│    └──────┬───────┘           │        │
│           │                   │        │
│           │ PUBLIC ⚠️         │        │
│           └───────────────────┘        │
│                                         │
└─────────────────────────────────────────┘
```

**Impact:** Tight coupling between test utilities, harder to maintain.

---

## After Fix: WITHOUT Primary Circular Dependency

### Corrected Structure (Production → Test: One Direction Only)

```
Production Code (src/)
═════════════════════════

    ┌──────────────┐
    │  components  │
    │              │
    └──────┬───────┘
           │
           │ PUBLIC
           ▼
    ┌──────────────────┐
    │ user_interface   │
    │                  │
    └──────┬───────────┘
           │
           │ PUBLIC
           ▼
    ┌──────────────┐
    │   events     │  ✅ No longer links to matchers
    │              │
    └──────────────┘


Test Code (tests/)
═══════════════════

    ┌──────────────┐
    │   matchers   │  ← TEST LIBRARY
    │              │     Can depend on src (correct direction!)
    └──────┬───────┘
           │
           │ PUBLIC
           ▼
    Multiple src libs
    ├─ components
    ├─ entity
    ├─ user_interface
    └─ events


    ┌──────────────┐
    │ test_harness │  ← TEST LIBRARY
    │              │
    └──────┬───────┘
           │
           │ PUBLIC
           ▼
    ├─ matchers
    └─ (other libs)
```

**Impact:** 
- ✅ Production libraries can be built independently
- ✅ Test libraries depend on production (correct direction)
- ✅ Building `components` does NOT build `matchers`
- ✅ Faster incremental builds
- ✅ Clear architectural boundaries

---

## Dependency Flow Rules

```
✓ CORRECT Flow:
═══════════════
    tests ────→ src
    
    (Tests depend on production code)


✗ INCORRECT Flow:
═════════════════
    src ────→ tests
    
    (Production depends on test code)
    ❌ THIS IS WHAT WE HAVE NOW
    ✅ THE FIX REMOVES THIS
```

---

## Build Behavior Comparison

### Before Fix

```bash
$ cmake --build --preset Debug --target components
```

**Builds:**
- components ✓
- user_interface ✓
- events ✓
- matchers ❌ (unnecessary)
- test_harness ❌ (unnecessary)
- All their transitive dependencies

**Time:** Slow (includes test compilation)

### After Fix

```bash
$ cmake --build --preset Debug --target components
```

**Builds:**
- components ✓
- user_interface ✓
- events ✓
- (only necessary production dependencies)

**Time:** Fast (production code only)

---

## Color Coding Guide

- **Green (✓, ✅):** Correct behavior, working as intended
- **Red (✗, ❌):** Critical issue, architectural violation
- **Yellow (⚠️):** Warning, should be addressed
- **Blue (→):** Dependency direction
- **Purple (◄─):** Part of a circular dependency

---

## References

- **Executive Summary:** `documentation/analysis/CIRCULAR_DEPENDENCY_EXECUTIVE_SUMMARY.md`
- **Detailed Analysis:** `documentation/analysis/BUILD_DEPENDENCY_CIRCULAR_ISSUE.md`
- **Quick Summary:** `CIRCULAR_DEPENDENCY_ANALYSIS_SUMMARY.md`
