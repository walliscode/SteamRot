# Provider Interface Standardization - Summary for Review

## What Was Delivered

This PR provides **comprehensive analysis and documentation** for standardizing Provider and Configurator interfaces across the SteamRot codebase. **No code changes** are included - this is pure analysis and planning work.

## The Problem

You identified that the current Provider and Configurator interfaces lack consistency:

- Naming varies (`Load*`, `Provide*`, `Get*`)
- Some classes are Providers, some are Configurators, some do both
- Configuration logic is buried in private methods
- No enforcement of patterns (you asked about templating)
- Hard to test and reuse configuration logic

## The Solution We Propose

### 1. **Standardized Interface Pattern**

```cpp
class IMyDataProvider {
public:
  virtual ~IMyDataProvider() = default;
  virtual std::expected<MyData, FailInfo> Provide() const = 0;
};
```

- All use `Provide()` method name
- Optionally add `Configure()` for configuring existing objects
- Simple, no templates (see below)

### 2. **Free Functions for Configuration**

Extract reusable configuration logic:

```cpp
// In my_data_config.h/cpp
namespace steamrot::config {
  std::expected<std::monostate, FailInfo>
  configure_my_data(MyData& data, const MyDataFbs* fb_data);
}
```

Benefits:
- Testable in isolation
- Reusable across implementations
- Not coupled to specific provider class

### 3. **Merge Configurators into Providers**

Instead of separate `IEntityConfigurator`, `ISceneConfigurator`, etc., absorb their functionality into corresponding Providers with `Configure()` methods.

### 4. **No Templates**

After analyzing three templating approaches (none, simple helpers, CRTP), we recommend **no templates** because:
- Pattern is simple enough without them
- Each provider has unique needs
- Easier to understand and maintain
- Rely on documentation and code review

## Documents Created

All in `documentation/` directory:

1. **PROVIDER_INTERFACE_STANDARDIZATION.md** (18KB)
   - Complete technical analysis
   - Current state inventory
   - Templating decision analysis
   - Proposed solution with examples

2. **PROVIDER_IMPLEMENTATION_GUIDE.md** (18KB)
   - Practical how-to guide
   - Step-by-step instructions
   - Code templates ready to copy
   - Testing patterns
   - Best practices

3. **PROVIDER_MIGRATION_PLAN.md** (21KB)
   - Detailed task breakdown
   - 9 specific migration tasks
   - Time estimates: 78-96 hours total
   - Step-by-step for each interface
   - Success criteria

4. **PROVIDER_QUICK_REFERENCE.md** (7KB)
   - TL;DR for developers
   - Quick lookup tables
   - Checklists
   - Anti-patterns to avoid

5. **README.md** (11KB)
   - Documentation index
   - Key decisions summary
   - FAQs
   - Implementation status tracker

## What You Asked For

### ✅ "Does this seem sensible?"

**Yes**, the approach is sensible. Our analysis confirms:
- Virtual `Configure()` and `Provide()` functions work well
- Free functions provide testability and reusability
- Merging Configurators into Providers simplifies architecture
- Discipline > Templates for this use case

### ✅ "Should we go with templating?"

**No**, we recommend **no templates**. Here's why:

**Templating Options Analyzed:**
1. **No templates** (Recommended) ⭐
   - Pros: Simple, flexible, easy to understand
   - Cons: No compile-time enforcement
   - Decision: Discipline and code review are sufficient

2. **Template helpers** (Not recommended)
   - Pros: Some type checking
   - Cons: Limited benefit, restricts flexibility
   - Decision: Adds complexity without sufficient value

3. **CRTP** (Not recommended)
   - Pros: Compile-time polymorphism
   - Cons: Very complex, loses runtime polymorphism
   - Decision: Overkill for this use case

**Rationale**: The pattern is simple (one or two virtual methods returning `std::expected`). Templates would add complexity that code review and documentation can handle better.

### ✅ "Is it worth it?"

**Yes**, worth it for:
- Testability: Free functions tested independently
- Reusability: Configuration logic shared
- Consistency: Clear pattern across codebase
- Maintainability: Changes easier to make

**Not worth it for**:
- Template enforcement - use discipline instead

### ✅ "Plan for bringing classes into line"

**PROVIDER_MIGRATION_PLAN.md** has complete plan:

**Phase 1: Foundation** ✅ COMPLETE (This PR)

**Phase 2: Data Providers** (30-36 hours)
1. IEngineDataProvider (4-6h) - Simplest, start here
2. ISaveDataProvider (3-4h)
3. ISceneManagerDataProvider (4-5h)
4. IUIStyleDataProvider (5-6h)
5. ISceneDataProvider (8-10h) - Most complex
6. IFontProvider (2-3h)

**Phase 3: Configurators** (32-40 hours)
7. ISceneConfigurator (6-8h)
8. IUIElementConfigurator (10-12h)
9. IEntityConfigurator (16-20h) - Very complex

**Phase 4: Cleanup** (8-12 hours)

**Total**: ~78-96 hours (2-2.5 weeks)

Each task has:
- Step-by-step instructions
- Files to modify
- Success criteria
- Risk assessment

## Key Decisions Made

| Decision | Choice | Rationale |
|----------|--------|-----------|
| **Templating** | None | Simplicity > enforcement |
| **Method naming** | `Provide()` | Consistency |
| **Configuration** | Free functions | Testability |
| **Configurators** | Merge into Providers | Simplification |
| **Namespace** | `steamrot::config` | Consistency |

## Example: Before and After

### Before (Current State)
```cpp
// Separate interfaces
class IEngineDataProvider {
  virtual std::expected<EngineData, FailInfo> LoadEngineData() const = 0;
};

// Implementation has private methods
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
private:
  std::expected<std::monostate, FailInfo>
  PopulateEngineConfig(EngineConfig& config, const EngineConfigFbs* data) const {
    // 50 lines of configuration logic
  }
};
```

### After (Proposed)
```cpp
// Standardized interface
class IEngineDataProvider {
  virtual std::expected<EngineData, FailInfo> Provide() const = 0;
};

// Free function (testable!)
namespace steamrot::config {
  std::expected<std::monostate, FailInfo>
  configure_engine_config(EngineConfig& config, const EngineConfigFbs* data) {
    // Configuration logic
  }
}

// Implementation uses free functions
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
  std::expected<EngineData, FailInfo> Provide() const override {
    auto fb_data = m_loader.LoadEngineDataFbs();
    EngineData data;
    return config::configure_engine_config(data.config, fb_data);
  }
};
```

## Next Steps

1. **Review Documentation** - Read at least the standardization doc and migration plan
2. **Approve Approach** - Confirm you agree with the direction
3. **Discuss Open Questions**:
   - Timeline acceptable?
   - Start with IEngineDataProvider?
   - Any concerns about breaking changes?
4. **Begin Implementation** - If approved, start Phase 2, Task 2.1

## Open Questions for You

1. **Timeline**: Is 2-2.5 weeks acceptable for full migration?
2. **Breaking Changes**: Some call sites will need updates. Acceptable?
3. **Priority**: Agree with starting at IEngineDataProvider?
4. **Incremental**: Should each Provider be a separate PR?
5. **Templating**: Comfortable with no-template approach?

## What's NOT in This PR

- ❌ No code changes
- ❌ No Provider implementations modified
- ❌ No test changes
- ❌ Only documentation

## Files Added

```
documentation/
├── README.md                                    (Index)
├── PROVIDER_INTERFACE_STANDARDIZATION.md       (Analysis)
├── PROVIDER_IMPLEMENTATION_GUIDE.md            (How-to)
├── PROVIDER_MIGRATION_PLAN.md                  (Task list)
└── PROVIDER_QUICK_REFERENCE.md                 (Cheat sheet)
```

## Recommendation

✅ **Approve this analysis**  
✅ **Start with IEngineDataProvider** (Priority 1, simplest)  
✅ **Use the step-by-step guide** in PROVIDER_MIGRATION_PLAN.md  
✅ **Do each provider as a separate PR** for easier review  

## Questions?

Refer to:
- FAQ in documentation/README.md
- Detailed rationale in PROVIDER_INTERFACE_STANDARDIZATION.md
- Implementation examples in PROVIDER_IMPLEMENTATION_GUIDE.md

---

**This is analysis only - ready for your review and approval before implementation begins.**
