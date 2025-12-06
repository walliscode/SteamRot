# Refactoring Planning Documents

**Purpose**: Comprehensive planning for Scene/SceneManager data organization and flatbuffers restructuring  
**Date**: December 6, 2025  
**Status**: PLANNING PHASE - Awaiting review and approval  

---

## 📚 Document Suite Overview

This directory contains **five comprehensive planning documents** for the proposed refactoring. Each document serves a specific purpose:

### 🎯 Quick Start Guide

**New to this refactoring?** Start here:

1. **REFACTORING_QUICK_REF.md** (5 min read)
   - TL;DR summary of entire refactoring
   - Key decisions needed
   - Quick navigation guide

2. **PATTERN_COMPARISON.md** (10 min read)
   - Shows Engine ✅ vs Scene 🔄 vs SceneManager 🔄
   - Demonstrates pattern consistency
   - Real-world usage examples

3. **Provide feedback** on key decisions

---

## 📖 Complete Document List

### 1. REFACTORING_QUICK_REF.md ⚡
**Purpose**: Fast overview for busy reviewers  
**Length**: 2 pages  
**Best for**: Getting the TL;DR, understanding scope, making decisions  

**Contents**:
- Three-struct pattern overview
- What goes where (Resources/Config/State)
- Flatbuffers reorganization summary
- Data loading cascade
- Impact summary
- Key decisions needed
- Phase breakdown
- Quick examples

**When to read**: Always start here!

---

### 2. PATTERN_COMPARISON.md 🔄
**Purpose**: Show consistency across Engine/Scene/SceneManager  
**Length**: 10 pages  
**Best for**: Understanding why this pattern, seeing examples  

**Contents**:
- Side-by-side comparison of all three levels
- Pattern consistency matrix
- Access pattern examples
- Member naming conventions
- Benefits of consistency
- Real-world usage scenarios
- Migration path visualization
- Why this pattern works

**When to read**: After quick ref, before deep dive

---

### 3. REFACTORING_VISUALS.md 🎨
**Purpose**: Visual aids, diagrams, flowcharts  
**Length**: 15 pages  
**Best for**: Visual learners, understanding relationships  

**Contents**:
- Current vs proposed structure diagrams
- Consistent pattern visualization
- Flatbuffers directory organization
- Data loading cascade flowcharts
- Decision trees for data placement
- Migration strategy comparisons
- Timeline visualization
- Success metrics
- Risk mitigation diagrams

**When to read**: For visual understanding of concepts

---

### 4. REFACTORING_ANALYSIS.md 📊
**Purpose**: Comprehensive detailed analysis  
**Length**: 25+ pages  
**Best for**: Deep understanding, implementation planning  

**Contents**:
- Current state analysis
- Proposed state detailed design
- Scene organization details
- SceneManager organization details
- Flatbuffers reorganization plan
- Data loading hierarchy design
- Impact analysis (~94 files)
- Migration strategy comparison
- Timeline estimates (8-12 days)
- Open questions
- Success criteria
- Risk mitigation

**When to read**: For complete understanding and planning

---

### 5. REFACTORING_FILE_LIST.md 📝
**Purpose**: File-by-file change tracking  
**Length**: 20+ pages  
**Best for**: Implementation tracking, effort estimation  

**Contents**:
- Phase 1: Scene organization (files to create/modify)
- Phase 2: SceneManager organization (files to create/modify)
- Phase 3: Flatbuffers reorganization (files to move)
- Phase 4: Data loading hierarchy (files to update)
- CMake changes
- Include path updates (~94 files if move headers)
- Test file updates
- Documentation updates
- Verification checklists
- Rollback strategies

**When to read**: During implementation for tracking progress

---

## 🗺️ Reading Paths

Choose your reading path based on your goal:

### Path A: Quick Decision Making (15 minutes)
```
1. REFACTORING_QUICK_REF.md
   └─> Get overview and key decisions
   
2. PATTERN_COMPARISON.md
   └─> See pattern consistency
   
3. Review key decisions section
   └─> Make choices

4. Provide feedback
```

### Path B: Full Understanding (1 hour)
```
1. REFACTORING_QUICK_REF.md (5 min)
   └─> Overview
   
2. PATTERN_COMPARISON.md (10 min)
   └─> Pattern understanding
   
3. REFACTORING_VISUALS.md (15 min)
   └─> Visual aids
   
4. REFACTORING_ANALYSIS.md (30 min)
   └─> Deep dive
   
5. REFACTORING_FILE_LIST.md (reference)
   └─> Check specific details
```

### Path C: Implementation Reference (ongoing)
```
During implementation, reference:

1. REFACTORING_FILE_LIST.md
   └─> Track which files to modify
   
2. REFACTORING_ANALYSIS.md
   └─> Reference design decisions
   
3. PATTERN_COMPARISON.md
   └─> Verify pattern consistency
   
4. REFACTORING_VISUALS.md
   └─> Check structure diagrams
```

---

## 🎯 What This Refactoring Does

### 1. Organize Scene Data (Like Engine)
Apply Engine's proven Resources/Config/State pattern:
- **SceneResources**: EntityManager, ActionManager, LogicMap, SceneCore
- **SceneConfig**: Event types, scene settings
- **SceneState**: Active flag, runtime state

### 2. Organize SceneManager Data (Like Engine)
Same pattern for SceneManager:
- **SceneManagerResources**: (Future resources)
- **SceneManagerConfig**: Scene manager settings
- **SceneManagerState**: Subscriptions, runtime state

### 3. Organize Flatbuffers Schemas
Move 28 schemas from flat structure to 8 logical directories:
- core/ (types, scene_types)
- engine/ (already organized!)
- scenes/ (scene_data, scene_manager_data, fragments)
- entities/ (entities, UI, components)
- events/ (events, subscribers, user_input)
- logic/ (logic_data)
- assets/ (assets)
- configuration/ (context, preferences, save_data)
- testing/ (test_data, simulation)

### 4. Establish Data Loading Hierarchy
Clear cascade system:
1. Shipped defaults (always)
2. User preferences (if applicable)
3. Save data (if applicable)

---

## 📊 Scope At-A-Glance

| Metric | Value |
|--------|-------|
| New struct headers | 6-8 |
| Modified files | 30-40 |
| Include updates | 0 (recommended) or 94 |
| CMake changes | 2-3 |
| Moved .fbs files | 25 |
| New directories | 8 |
| Timeline (incremental) | 8-12 days |
| Timeline (big bang) | 6-8 days (risky) |

---

## 🔑 Key Decisions Needed

| Decision | Recommendation |
|----------|----------------|
| Migration Strategy | ✅ Incremental (one phase at a time) |
| Generated Headers | ✅ Keep in root (no include changes) |
| SceneManagerResources | ✅ Create now (for consistency) |
| TitleScene Data | ✅ Defaults only (consistent experience) |
| Flatbuffers Structure | ✅ 8 subdirectories (as proposed) |

All recommendations are marked with ✅ but are open for discussion!

---

## 📈 Phases (If Incremental - Recommended)

### Phase 1: Scene Organization (2-3 days)
Create SceneResources/Config/State, update Scene class

### Phase 2: SceneManager Organization (1-2 days)
Create SceneManager structs, update SceneManager class

### Phase 3: Flatbuffers Reorganization (3-4 days)
Move files, update CMake, test generation

### Phase 4: Data Loading Hierarchy (2-3 days)
Implement cascade, update providers

---

## 💡 Why This Matters

### Before (Current State)
```
Scene class:
├─ EntityManager         ← What category is this?
├─ ActionManager         ← Where does new data go?
├─ LogicMap              ← Is this a resource?
├─ SceneCore             ← Or configuration?
├─ bool m_active         ← Or runtime state?
└─ event_types           ← Hard to know!
```

### After (Proposed State)
```
Scene class:
├─ SceneResources        ← Managers and systems
│  ├─ EntityManager
│  ├─ ActionManager
│  ├─ LogicMap
│  └─ SceneCore
├─ SceneConfig           ← Loaded from files
│  └─ event_types
└─ SceneState            ← Runtime flags
   └─ active
```

**Clear categorization = Easy to navigate and extend!**

---

## 🎬 Next Steps

### 1. Read the Documents
- Start with REFACTORING_QUICK_REF.md (5 minutes)
- Then PATTERN_COMPARISON.md (10 minutes)
- Dive deeper as needed

### 2. Review Key Decisions
- Migration strategy (incremental recommended)
- Generated header location (keep in root recommended)
- SceneManagerResources (create now recommended)
- TitleScene data (defaults only recommended)
- Flatbuffers structure (8 subdirs recommended)

### 3. Provide Feedback
- Overall approach
- Timeline and effort
- Concerns or suggestions
- Alternative approaches
- Priority

### 4. If Approved
- Begin Phase 1 implementation
- Test after each phase
- Document learnings
- Iterate

---

## 🤝 Questions?

**Quick questions**: See REFACTORING_QUICK_REF.md  
**Pattern questions**: See PATTERN_COMPARISON.md  
**Detailed questions**: See REFACTORING_ANALYSIS.md  
**Visual aids**: See REFACTORING_VISUALS.md  
**Implementation details**: See REFACTORING_FILE_LIST.md  

---

## 📍 Status

**Current**: PLANNING PHASE  
**Next**: Awaiting review and decisions  
**Then**: Begin Phase 1 (if approved)  

---

## 🏆 Success Criteria

### Must Have
- ✅ All tests pass
- ✅ Project builds
- ✅ No regression
- ✅ Clear organization
- ✅ Complete documentation

### Should Have
- ✅ Improved clarity
- ✅ Easier to extend
- ✅ Better testability

---

**Ready to dive in?** Start with REFACTORING_QUICK_REF.md! 🚀
