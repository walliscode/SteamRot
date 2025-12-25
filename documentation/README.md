# SteamRot Documentation

This directory contains architectural analysis, design patterns, and workflow guides for the SteamRot game engine.

## Recent Additions

### 🆕 Nested Data Handling Pattern (2024-12-25)

Comprehensive documentation for handling nested data structures consistently across the codebase.

**Start Here**: [NESTED_DATA_INDEX.md](NESTED_DATA_INDEX.md)

**The Problem**: Accessing nested data (e.g., AssetConfig inside EngineData) requires multiple clunky method calls.

**The Solution**: Three simple rules:
1. Base structs contain nested structs
2. Providers create the full struct
3. Cache the main struct

**Documents**:
- 📄 [Quick Reference](NESTED_DATA_QUICK_REFERENCE.md) - 5 min overview
- 📊 [Visual Diagrams](NESTED_DATA_DIAGRAMS.md) - Architecture illustrations
- 📘 [Pattern Guide](workflows/NESTED_DATA_PATTERN_GUIDE.md) - Step-by-step implementation
- 📖 [Full Analysis](analysis/NESTED_DATA_HANDLING_PATTERNS.md) - Detailed exploration
- 📋 [Decision Record](analysis/ADR_NESTED_DATA_PATTERN.md) - Architecture decision

## Directory Structure

```
documentation/
├── README.md                           # This file - documentation index
├── NESTED_DATA_INDEX.md               # Entry point for nested data docs
├── NESTED_DATA_QUICK_REFERENCE.md     # Quick lookup guide
├── NESTED_DATA_DIAGRAMS.md            # Visual architecture diagrams
│
├── analysis/                          # Detailed analysis documents
│   ├── NESTED_DATA_HANDLING_PATTERNS.md
│   ├── ADR_NESTED_DATA_PATTERN.md
│   ├── USER_INTERFACE_DECOUPLING_ANALYSIS.md
│   ├── USER_INTERFACE_PHASES_1_2_IMPLEMENTATION.md
│   └── FONT_PROVIDER_DECOUPLING.md
│
└── workflows/                         # Step-by-step guides
    ├── NESTED_DATA_PATTERN_GUIDE.md
    └── UI_CONFIGURATION_WORKFLOW.md
```

## How to Use This Documentation

### For Quick Answers

Look in the root directory for quick reference guides:
- `NESTED_DATA_QUICK_REFERENCE.md` - Pattern cheat sheet
- `NESTED_DATA_DIAGRAMS.md` - Visual architecture

### For Implementation

Look in `workflows/` for step-by-step guides:
- `NESTED_DATA_PATTERN_GUIDE.md` - How to implement nested data
- `UI_CONFIGURATION_WORKFLOW.md` - UI setup procedures

### For Understanding

Look in `analysis/` for deep dives:
- `NESTED_DATA_HANDLING_PATTERNS.md` - Full pattern analysis
- `ADR_NESTED_DATA_PATTERN.md` - Decision rationale
- `USER_INTERFACE_*.md` - UI architecture documents

## Documentation Categories

### Architecture Decisions

Documents recording significant architectural choices:
- `analysis/ADR_NESTED_DATA_PATTERN.md` - Nested data handling pattern

### Design Patterns

Established patterns for common scenarios:
- **Nested Data Pattern** - How to handle nested data structures
- **Data Provider Pattern** - How to abstract data sources
- **UI Configuration** - How to configure user interface

### Workflow Guides

Step-by-step guides for specific tasks:
- `workflows/NESTED_DATA_PATTERN_GUIDE.md` - Implementing nested data
- `workflows/UI_CONFIGURATION_WORKFLOW.md` - Setting up UI

### Analysis Documents

Detailed analysis of architectural topics:
- `analysis/NESTED_DATA_HANDLING_PATTERNS.md` - Data handling analysis
- `analysis/USER_INTERFACE_DECOUPLING_ANALYSIS.md` - UI architecture
- `analysis/FONT_PROVIDER_DECOUPLING.md` - Font system design

## Contributing Documentation

When adding new documentation:

1. **Choose the right category**:
   - Quick reference → Root directory
   - Workflow guide → `workflows/`
   - Detailed analysis → `analysis/`

2. **Follow naming conventions**:
   - Use `UPPER_SNAKE_CASE` for important docs
   - Use descriptive names
   - Include topic area in name

3. **Link from index files**:
   - Update this README
   - Update relevant index files
   - Cross-link related docs

4. **Include metadata**:
   - Date created
   - Status (Proposed/Accepted/Deprecated)
   - Author
   - Related documents

## Document Status

- ✅ **Accepted** - Implemented and in use
- 📝 **Proposed** - Under review, not yet implemented
- 🚧 **In Progress** - Implementation underway
- ⚠️ **Deprecated** - No longer recommended

### Current Status

| Document | Status | Notes |
|----------|--------|-------|
| Nested Data Pattern | 📝 Proposed | Analysis complete, awaiting approval |
| UI Decoupling | ✅ Accepted | Implemented |
| Font Provider Decoupling | ✅ Accepted | Implemented |

## Getting Started with Documentation

### New to the Project?

Start with these documents in order:
1. Main README in repository root
2. Quick reference guides in this directory
3. Workflow guides as needed
4. Analysis documents for deep understanding

### Working on a Specific Feature?

1. Check `workflows/` for relevant guides
2. Check `analysis/` for design decisions
3. Check quick references for patterns to follow

### Making Architectural Decisions?

1. Review existing ADRs in `analysis/`
2. Review related analysis documents
3. Consider creating a new ADR for your decision

## Related Documentation

- **Main README**: `/README.md` - Project overview and getting started
- **Architecture Docs**: `/docs/` - Doxygen-generated architecture
- **Code Examples**: `/documentation/examples/` - Code samples

## Feedback and Questions

For questions about documentation:
- Check the relevant index file (e.g., `NESTED_DATA_INDEX.md`)
- Review FAQ sections in analysis documents
- Consult workflow guides for implementation questions

For suggesting improvements:
- Create an issue describing the documentation need
- Propose changes via pull request
- Discuss in code review comments

---

**Last Updated**: 2024-12-25  
**Maintainers**: SteamRot Development Team
