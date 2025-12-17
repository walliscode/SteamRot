# FlatBuffers Layer Migration - Quick TODO List

**Date:** 2025-12-17  
**Reference:** See `FLATBUFFERS_LAYER_ANALYSIS.md` for detailed analysis

---

## Priority Order

1. **SceneType** - CRITICAL (most widespread)
2. **EventType** - HIGH (core event system)
3. **DataPopulateFunction** - MEDIUM (UI-specific)
4. **ViewDirection** - LOW (limited scope)

---

## Phase 1: SceneType Migration (CRITICAL)

### Files to Create
- [ ] `src/types/core/SceneType.h` - Native enum

### Files to Modify
- [ ] `src/types/flatbuffers/core/scene_types.fbs` - Rename to `SceneTypeFbs`
- [ ] `src/types/events/EventPacket.h` - Remove FlatBuffers include, use native enum
- [ ] `src/types/core/SceneInfo.h` - Remove FlatBuffers include, use native enum
- [ ] Create conversion function in Layer 2 (data provider or configurator)

### Conversion Function Location
- Likely: `src/data_providers/` or `src/configuration/`
- Pattern: `static SceneType ConvertSceneType(int8_t fbs_scene_type)`

---

## Phase 2: EventType Migration (HIGH)

### Files to Create
- [ ] `src/types/events/EventType.h` - Native enum (uint64_t for bit_flags)

### Files to Modify
- [ ] `src/types/flatbuffers/events/events.fbs` - Rename to `EventTypeFbs`
- [ ] `src/types/events/EventPacket.h` - Remove FlatBuffers include, use native enum
- [ ] `src/types/events/Subscriber.h` - Remove FlatBuffers include, use native enum
- [ ] Create conversion function in Layer 2 (event factory or configurator)

### Conversion Function Location
- Likely: `src/events/` or `src/configuration/`
- Pattern: `static EventType ConvertEventType(uint64_t fbs_event_type)`

### Special Notes
- **Important:** EventType is bit_flags, use uint64_t
- Need to preserve bitwise operations capability

---

## Phase 3: DataPopulateFunction Migration (MEDIUM)

### Files to Create
- [ ] `src/types/user_interface/DataPopulateFunction.h` - Native enum

### Files to Modify
- [ ] `src/types/flatbuffers/entities/user_interface.fbs` - Rename to `DataPopulateFunctionFbs`
- [ ] `src/types/user_interface/DropDownListElement.h` - Remove FlatBuffers include, use native enum
- [ ] `src/user_interface/FlatbuffersUIElementConfigurator.cpp` - Add conversion function
- [ ] `src/logic/logic_action.cpp` - Update switch statement to use native enum

### Conversion Function Location
- **Definite:** `src/user_interface/FlatbuffersUIElementConfigurator`
- Pattern: `static DataPopulateFunction ConvertDataPopulateFunction(int8_t fbs_func)`

### Special Notes
- Follows exact pattern as Layout and SpacingAndSizing (already done)
- Look at those for reference

---

## Phase 4: ViewDirection Migration (LOW)

### Files to Create
- [ ] `src/types/core/ViewDirection.h` - Native enum

### Files to Modify
- [ ] `src/types/flatbuffers/entities/fragments.fbs` - Rename to `ViewDirectionFbs`
- [ ] `src/types/core/Fragment.h` - Remove FlatBuffers include, use native enum
- [ ] `src/logic/CraftingRenderLogic.cpp` - Update map access
- [ ] Create conversion function in Layer 2 (entity configurator)

### Conversion Function Location
- Likely: `src/entity/FlatbuffersEntityConfigurator` or similar
- Pattern: `static ViewDirection ConvertViewDirection(uint8_t fbs_direction)`

---

## Testing Checklist (Per Phase)

After each enum migration:

### Compile & Link
- [ ] Code compiles without errors
- [ ] No missing include warnings
- [ ] Linker completes successfully

### Unit Tests
- [ ] Conversion function tested with all values
- [ ] Default case tested
- [ ] Edge cases covered

### Integration Tests
- [ ] Data loading from FlatBuffers works
- [ ] Enum conversion correct in practice
- [ ] No runtime errors

### Regression Tests
- [ ] Full test suite passes
- [ ] No new failures
- [ ] No performance regressions

---

## Final Validation

After all phases complete:

### Architecture Compliance
- [ ] Run: `grep -r "_generated\.h" src/types --include="*.h" | grep -v flatbuffers | grep -v FbsSceneData`
- [ ] Expected result: Empty (no Layer 1 violations except FbsSceneData wrapper)

### Documentation
- [ ] Update `UI_CONFIGURATION_WORKFLOW.md` with new examples if needed
- [ ] Mark this TODO as complete
- [ ] Archive analysis document

### Code Review
- [ ] All conversion functions follow established pattern
- [ ] All native enums have matching values to FlatBuffers enums
- [ ] All FlatBuffers enums have `Fbs` suffix

---

## Reference Pattern (from Layout enum)

### Native Enum Template
```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of [EnumName] enum
////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief [Brief description]
////////////////////////////////////////////////////////////
enum class [EnumName] : int8_t {
  Value1 = 0,
  Value2 = 1,
  Value3 = 2,
};

} // namespace steamrot
```

### FlatBuffers Schema Template
```fbs
enum [EnumName]Fbs: byte {
  Value1 = 0,
  Value2 = 1,
  Value3 = 2,
}
```

### Conversion Function Template
```cpp
////////////////////////////////////////////////////////////
[EnumName] Configurator::Convert[EnumName](int8_t fbs_value) {
  switch (fbs_value) {
  case [EnumName]Fbs_Value1:
    return [EnumName]::Value1;
  case [EnumName]Fbs_Value2:
    return [EnumName]::Value2;
  case [EnumName]Fbs_Value3:
    return [EnumName]::Value3;
  default:
    return [EnumName]::Value1;  // Or appropriate default
  }
}
```

---

## Quick Command References

### Find Layer 1 FlatBuffers violations
```bash
find src/types -name "*.h" -type f ! -path "*/flatbuffers/*" -exec grep -l "_generated\.h" {} \;
```

### Check for specific enum usage
```bash
grep -r "EnumName" src/types --include="*.h" --include="*.cpp"
```

### Verify conversion function exists
```bash
grep -r "Convert.*EnumName" src --include="*.cpp"
```

---

## Notes

- **Do migrations in order** - SceneType first (most critical)
- **Test after each phase** - Don't accumulate untested changes
- **Follow the pattern** - Layout and SpacingAndSizing are the reference
- **Document as you go** - Update this TODO with actual file locations
- **Keep FbsSceneData** - It's a wrapper, not a violation

---

## Estimated Time

- **Phase 1 (SceneType):** 4-6 hours
- **Phase 2 (EventType):** 4-6 hours
- **Phase 3 (DataPopulateFunction):** 2-3 hours
- **Phase 4 (ViewDirection):** 2-3 hours

**Total: 12-18 hours (1.5-2 days)**
