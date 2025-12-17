# SteamRot Documentation

Welcome to the SteamRot game engine documentation.

## 📁 Documentation Structure

### Analysis Documents

In-depth architectural analysis and decision-making documents:

#### Type Switching Strategies
- **[Switching Strategies Analysis](analysis/SWITCHING_STRATEGIES_ANALYSIS.md)** - Comprehensive analysis of strategies for switching on concrete data types (static_cast, dynamic_cast, switch-case, visitor pattern, std::variant)
- **[Switching Strategies Quick Reference](analysis/SWITCHING_STRATEGIES_QUICK_REFERENCE.md)** - Quick decision guide and cheat sheet for choosing the right switching strategy

#### Factory Selection Strategies
- **[Factory Selection Strategies Analysis](analysis/FACTORY_SELECTION_STRATEGIES_ANALYSIS.md)** - Comprehensive analysis of strategies for selecting concrete provider/configurator implementations (configuration file, environment variables, dependency injection, registry pattern, abstract factory)
- **[Factory Selection Quick Reference](analysis/FACTORY_SELECTION_QUICK_REFERENCE.md)** - Quick decision guide for choosing factory selection strategy

### Workflows (How-To Guides)

*Coming Soon* - Practical guides for common development tasks

### Architecture Documents

*Coming Soon* - System architecture and design patterns

### Configuration Guides

*Coming Soon* - Configuration and setup documentation

---

## 🎯 Quick Links by Role

### For Architects
- [Switching Strategies Analysis](analysis/SWITCHING_STRATEGIES_ANALYSIS.md) - Full analysis with trade-offs and recommendations

### For Developers
- [Switching Strategies Quick Reference](analysis/SWITCHING_STRATEGIES_QUICK_REFERENCE.md) - Fast decision tree and code patterns

---

## 📊 Analysis Documents

### Switching Strategies for Concrete Data Types

**Purpose**: Evaluate and recommend strategies for runtime type switching in polymorphic data structures **after data is loaded**

**Documents**:
1. **[SWITCHING_STRATEGIES_ANALYSIS.md](analysis/SWITCHING_STRATEGIES_ANALYSIS.md)** (15-20 min read)
   - Executive summary
   - Five strategies analyzed:
     1. Static Cast with Manual Type Checking
     2. Dynamic Cast Chain
     3. Switch-Case with Type Tag Enums
     4. Visitor Pattern
     5. std::variant with std::visit
   - Real-world examples from SteamRot codebase
   - Performance comparison
   - Recommendations for each use case
   - Migration guides

2. **[SWITCHING_STRATEGIES_QUICK_REFERENCE.md](analysis/SWITCHING_STRATEGIES_QUICK_REFERENCE.md)** (5 min read)
   - Decision tree
   - Strategy cheat sheet with pros/cons
   - Comparison matrix
   - Code patterns
   - Common pitfalls

**When to Read**:
- Implementing polymorphic data types (Scene data, Event data, Component variants)
- Deciding between static_cast, dynamic_cast, switch, visitor, or std::variant
- Optimizing performance-critical type dispatch
- Reviewing code that switches on concrete types

---

### Factory Selection Strategies for Providers and Configurators

**Purpose**: Evaluate and recommend strategies for selecting concrete provider/configurator implementations **at the factory level**

**Problem**: Current hard-coded approach in `provider_factory.cpp` and `SceneFactory.cpp` prevents supporting multiple data formats (FlatBuffers, JSON, XML, save files, test data).

**Documents**:
1. **[FACTORY_SELECTION_STRATEGIES_ANALYSIS.md](analysis/FACTORY_SELECTION_STRATEGIES_ANALYSIS.md)** (20-25 min read)
   - Executive summary
   - Current architecture issues
   - Six strategies analyzed:
     1. Configuration File
     2. Environment Variable
     3. Factory Method Parameter
     4. Registry Pattern
     5. Strategy Pattern with Dependency Injection
     6. Abstract Factory Pattern
   - Real-world examples from SteamRot provider_factory.cpp
   - Comparative analysis
   - Migration path (Phase 1: Environment Variables → Phase 2: Dependency Injection)
   - Complete implementation examples

2. **[FACTORY_SELECTION_QUICK_REFERENCE.md](analysis/FACTORY_SELECTION_QUICK_REFERENCE.md)** (5 min read)
   - Decision tree
   - Strategy cheat sheet with complexity ratings
   - Comparison matrix
   - Recommended solution for SteamRot (Environment Vars + Dependency Injection)
   - Migration checklist
   - Common patterns and pitfalls

**When to Read**:
- Need to support multiple data formats (production vs test vs save files)
- Fixing hard-coded provider factories
- Implementing dependency injection
- Adding plugin/modding support
- Making code more testable (injecting mock providers)

---

## 🔧 Contributing to Documentation

### Adding New Documents

1. **Analysis Documents**: Place in `documentation/analysis/`
   - Use format: `TOPIC_ANALYSIS.md` for full analysis
   - Use format: `TOPIC_QUICK_REFERENCE.md` for quick guides
   - Include executive summary, table of contents, examples

2. **Workflow Guides**: Place in `documentation/workflows/`
   - Use format: `DOING_TASK.md`
   - Include step-by-step instructions with code examples

3. **Architecture Docs**: Place in `documentation/architecture/`
   - Use format: `SYSTEM_NAME.md`
   - Include diagrams, design decisions, trade-offs

### Documentation Standards

- **Markdown Format**: All documentation in markdown
- **Code Examples**: Use syntax-highlighted code blocks
- **Real Examples**: Reference actual code from repository when possible
- **Executive Summary**: Start with summary for decision makers
- **Table of Contents**: Include for documents >5 sections
- **Cross-References**: Link to related documents
- **Version Info**: Include version and last updated date

---

## 📚 External Resources

- [Main README](../README.md) - Project overview and quick start
- [GitHub Repository](https://github.com/walliscode/SteamRot)

---

**Last Updated**: 2025-12-16
