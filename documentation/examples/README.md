# Logic Class Examples

This directory contains complete example files demonstrating best practices for creating Logic classes in the SteamRot game engine.

## Files

### ExampleLogic.h
Header file showing recommended structure for Logic class headers:
- Proper inheritance from `Logic` base class
- Private `ProcessLogic()` method override
- Public constructor with `LogicContext` parameter
- Comprehensive Doxygen documentation
- Visual dividers between sections

### ExampleLogic.cpp
Implementation file demonstrating common patterns:
1. **Single Component Archetype** - Processing entities with one component type
2. **Multiple Component Archetype** - Processing entities with multiple components
3. **Rendering Logic** - Drawing to the scene render texture
4. **Collision/Mouse Interaction** - Using mouse position for UI interactions
5. **Event-Based Logic** - Triggering game events based on state
6. **Asset Manager Access** - Loading and using game assets
7. **Conditional Processing** - Processing entities based on criteria

### ExampleLogic.test.cpp
Comprehensive test file showing testing best practices:
1. **Constructor Test** - Basic instantiation verification
2. **Logic Execution Test** - Verifying state changes
3. **Edge Case Test** - Handling empty archetypes
4. **Integration Test** - Testing with event system
5. **Multiple Entities Test** - Processing multiple entities
6. **Conditional Logic Test** - Verifying conditional execution
7. **Input Interaction Test** - Testing mouse position handling
8. **Render Test** - Testing rendering operations

## Usage

These files are **examples only** and are not compiled as part of the project. They demonstrate:

- Code structure and organization
- Documentation standards (Doxygen comments)
- Common implementation patterns
- Comprehensive testing strategies
- Best practices for Logic class development

## How to Use These Examples

1. **Read First**: Study the examples to understand the patterns and structure
2. **Copy and Modify**: Use these as templates for your own Logic classes
3. **Follow TDD**: Write tests (based on the test example) before implementing
4. **Adapt Patterns**: Select and adapt the patterns that fit your use case
5. **Maintain Style**: Keep the visual dividers, documentation, and naming conventions

## Key Takeaways

### Logic Class Structure
- Inherit from `Logic`
- Override `ProcessLogic()` (private method)
- Constructor takes `const LogicContext`
- Call base constructor: `Logic(logic_context)`

### Implementation Patterns
- Always check if archetype exists before iterating
- Use `GenerateArchetypeIDfromTypes<Components...>()` for archetype lookup
- Use `emp_helpers::GetComponent<T>()` for component access
- Access game systems via `m_logic_context` members

### Testing Patterns
- Use `TestContext` for mock dependencies
- Test construction, execution, edge cases, and integration
- Use `CAPTURE()` for debugging
- Verify state changes with `REQUIRE()`
- Test with different scene types

### Documentation Standards
- Use Doxygen-style comments (`///`)
- Use `@brief`, `@param`, `@return` tags
- Add visual dividers (`/////////////////////////////////////////////////`)
- Explain purpose and patterns in file-level comments

## Related Documentation

- **Main README**: `/README.md` - Full workflow for adding Logic classes
- **Copilot Instructions**: `/.github/copilot-instructions.md` - Development guidelines
- **Logic Base Class**: `/src/logic/Logic.h` - Abstract base class
- **LogicFactory**: `/src/logic/LogicFactory.cpp` - Logic instantiation
- **Test Helpers**: `/tests/logic/logic_helpers.cpp` - Test validation functions

## Real Examples in the Codebase

For production examples, see:
- `src/logic/UIActionLogic.{h,cpp}` - Action processing logic
- `src/logic/UICollisionLogic.{h,cpp}` - Collision detection logic
- `src/logic/UIRenderLogic.{h,cpp}` - UI rendering logic
- `src/logic/CraftingRenderLogic.{h,cpp}` - Scene-specific rendering

And their corresponding test files in `tests/logic/`.
