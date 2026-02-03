# SteamRot Documentation

Welcome to the SteamRot documentation! This directory contains comprehensive guides, architectural documentation, and references for the SteamRot game engine.

---

## 📋 Table of Contents

- [Architecture Documentation](#architecture-documentation)
- [Future Documentation Areas](#future-documentation-areas)
- [Contributing to Documentation](#contributing-to-documentation)

---

## Architecture Documentation

Documentation on system design, architectural decisions, and design proposals.

### Current Documents

#### CGrimoireMachina Refactoring Analysis

Analysis and design documents for refactoring CGrimoireMachina from a Component to a standalone class managed by AssetManager.

- **[Executive Summary](architecture/CGRIMOIREMACHINA_EXECUTIVE_SUMMARY.md)** - For decision makers
  - 📄 4 pages, recommendation and decision points
  - Best for: Stakeholders needing to approve/reject proposal

- **[Quick Reference](architecture/CGRIMOIREMACHINA_QUICK_REFERENCE.md)** - TL;DR summary of the proposal
  - 📄 2 pages, quick overview
  - Best for: Getting the gist quickly

- **[Full Analysis](architecture/CGRIMOIREMACHINA_REFACTORING_ANALYSIS.md)** - Comprehensive analysis
  - 📄 16KB, detailed analysis with pros/cons
  - Best for: Understanding the reasoning and trade-offs

- **[Design Proposal](architecture/CGRIMOIREMACHINA_DESIGN_PROPOSAL.md)** - Concrete implementation design
  - 📄 21KB, detailed implementation plan
  - Best for: Understanding how to implement the change

**Status**: Analysis & Design Phase - Awaiting stakeholder review

**Summary**: Proposal to move CGrimoireMachina from EntityMemoryPool to AssetManager, transforming it from an entity-bound component to a global catalog/database. Includes hybrid migration strategy with minimal breaking changes.

---

## Future Documentation Areas

The following documentation sections will be added as the project grows:

### Workflows (How-To Guides)
*Planned*: Step-by-step guides for common development tasks
- Adding Components
- Adding Logic Classes
- Adding UI Elements
- Adding Actions
- Creating Test Data

### Configuration
*Planned*: System configuration documentation
- Context Configuration
- Resource Configuration
- Asset Configuration

### Testing
*Planned*: Testing infrastructure and patterns
- Testing Overview
- Test Data Configuration
- Writing Unit Tests
- Writing Integration Tests

### Style & Conventions
*Planned*: Code style and naming conventions
- Code Style Guide
- File Naming Conventions
- Naming Quick Reference

### Examples
*Planned*: Code examples and patterns
- Logic Class Examples
- Component Examples
- UI Element Examples

### Reference
*Planned*: API references and technical details
- Component Reference
- Logic System Reference
- Event System Reference

---

## Contributing to Documentation

When adding new documentation:

1. **Choose the Right Category**: Place documents in the appropriate subdirectory
   - `architecture/` - System design, architectural decisions
   - `workflows/` - How-to guides
   - `configuration/` - Configuration documentation
   - `testing/` - Testing guides
   - `style/` - Code style and conventions
   - `examples/` - Code examples
   - `reference/` - API references

2. **Use Clear Titles**: Make document titles descriptive and searchable

3. **Add to This Index**: Update this README.md with a link and description

4. **Include Metadata**: Add date, status, and version info to documents

5. **Use Markdown**: Standard markdown formatting for consistency

6. **Link Related Docs**: Cross-reference related documentation

7. **Keep Updated**: Update documentation when code changes

---

## Documentation Standards

### Document Structure

All major documentation should include:

```markdown
# Title

**Date**: YYYY-MM-DD
**Status**: Draft | Review | Approved | Deprecated
**Version**: X.Y

---

## Overview
Brief summary of the document purpose

## Content Sections
...

---

**Last Updated**: YYYY-MM-DD
**Author**: Name or Team
```

### File Naming

- Use `SCREAMING_SNAKE_CASE` for documentation files
- Be descriptive: `ADDING_COMPONENTS.md` not `COMPONENTS.md`
- Use prefixes for series: `CGRIMOIREMACHINA_ANALYSIS.md`

### Cross-References

Link to related documentation:
- Use relative paths: `[Link](./other_doc.md)`
- Link to specific sections: `[Link](./doc.md#section)`
- Maintain link integrity when moving files

---

## Quick Links

- **[Main README](../README.md)** - Project overview
- **[Architecture](./architecture/)** - System design documentation

---

**Last Updated**: 2026-02-03
