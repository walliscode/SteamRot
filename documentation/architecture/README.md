# SteamRot Architecture Documentation

This directory contains architectural documentation for the SteamRot game engine.

## Quick Links

### Interface Library Analysis (NEW - December 2025)
- **[Quick Reference](INTERFACE_LIBRARY_QUICK_REF.md)** - Start here! TL;DR and decision tree
- **[Full Analysis](INTERFACE_LIBRARY_ANALYSIS.md)** - Comprehensive analysis of 52 structures
- **[Dependency Diagrams](INTERFACE_LIBRARY_DEPENDENCY_DIAGRAMS.md)** - Visual dependency graphs

**Purpose:** Identifies which data structures can be moved to interface libraries to prevent circular CMake dependencies.

**Key Results:**
- ✅ 13 structures can be moved (8 config/state + 5 interfaces)
- ❌ 39 structures cannot (SFML, FlatBuffers, UUID, or complex implementations)
- Provides phase-by-phase implementation guide

---

### Core Architecture Documents
- **[Game Loop](GAME_LOOP.md)** - Main game loop architecture
- **[Data Loading Hierarchy](DATA_LOADING_HIERARCHY.md)** - Data loading system design
- **[Error Handling](ERROR_HANDLING.md)** - Error handling patterns
- **[Event Namespaces](EVENT_NAMESPACES.md)** - Event system organization
- **[Logic System](LOGIC_SYSTEM.md)** - Logic processing architecture

---

## Document Index by Topic

### Dependency Management
- [Interface Library Quick Reference](INTERFACE_LIBRARY_QUICK_REF.md) - Quick guide
- [Interface Library Analysis](INTERFACE_LIBRARY_ANALYSIS.md) - Full analysis
- [Interface Library Dependency Diagrams](INTERFACE_LIBRARY_DEPENDENCY_DIAGRAMS.md) - Visual guides

### Data Management
- [Data Loading Hierarchy](DATA_LOADING_HIERARCHY.md) - How data flows through the system

### Game Systems
- [Game Loop](GAME_LOOP.md) - Main execution flow
- [Logic System](LOGIC_SYSTEM.md) - Logic processing
- [Event Namespaces](EVENT_NAMESPACES.md) - Event organization

### Error Handling
- [Error Handling](ERROR_HANDLING.md) - Error patterns and best practices

---

## Related Documentation

### Proposals
See [../proposals/](../proposals/) for architectural proposals and design documents:
- Engine Architecture Improvements
- Engine Data Organization
- Entity View Pattern
- Data Loading Interface System
- And more...

### Workflows
See [../workflows/](../workflows/) for step-by-step guides:
- Adding Logic Classes
- Adding Components
- Data Configuration
- And more...

---

## Recent Updates

**December 7, 2025:**
- Added Interface Library Analysis documentation
  - Analyzed 52 data structures across the codebase
  - Identified 13 candidates for interface library extraction
  - Documented 39 blocked structures with dependency analysis
  - Created quick reference and visual dependency diagrams
  - Provided implementation roadmap

---

## Contributing to Architecture Documentation

When adding new architecture documentation:

1. **Place it here** if it describes how systems work together
2. **Use proposals/** if you're proposing a change
3. **Use workflows/** if you're documenting a process

### Documentation Standards
- Use clear section headers
- Include diagrams where helpful
- Provide examples
- Link to related documents
- Keep technical but readable

---

## Questions?

If you're looking for:
- **How to implement a feature**: Check [../workflows/](../workflows/)
- **Why a design was chosen**: Check [../proposals/](../proposals/)
- **How a system works**: You're in the right place!
- **Testing patterns**: Check [../testing/](../testing/)

