# Console Output Color Reference

This document describes the ANSI colors used in the test harness console output.

## Color Scheme

### Success Messages (✓)
- **Color**: Bold Green (`\033[1;32m`)
- **Usage**: Successful operations, completed ticks, passing comparisons
- **Example**: `✓ [Tick 1] Tick completed`

### Error Messages (✗)
- **Color**: Bold Red (`\033[1;31m`)
- **Usage**: Failed operations, errors, failing comparisons
- **Example**: `✗ [Tick 2] Simulation step failed`

### Info Messages (•)
- **Color**: Blue (`\033[34m`)
- **Usage**: General information, status updates
- **Example**: `• Total ticks to execute: 5`

### Tick Numbers ([Tick N])
- **Color**: Cyan (`\033[36m`)
- **Usage**: Tick context annotations
- **Example**: `[Tick 3] Executing tick`

### Progress Indicators (➤)
- **Color**: Magenta (`\033[35m`)
- **Usage**: Current execution progress
- **Example**: `➤ Executing Tick 3 of 10`

### Tick Number Values
- **Color**: Bold Blue (`\033[1;34m`)
- **Usage**: Numeric tick values in progress
- **Example**: `Executing Tick 3 of 10` (3 and 10 are bold blue)

### Section Headers (━━━━)
- **Color**: Bold Yellow (`\033[1;33m`)
- **Usage**: Section dividers, organizational headers
- **Example**: `━━━━ Tick-Based Execution ━━━━`

### Test Banners (┌─┐)
- **Color**: Bold Cyan (`\033[1;36m`)
- **Usage**: Test start banners, box drawing
- **Example**: 
```
┌─────────────────────────────────────
│ Running Test: sample_test
└─────────────────────────────────────
```

### Test Names in Banners
- **Color**: Bold (`\033[1m`)
- **Usage**: Test name emphasis
- **Example**: Inside banner, the test name is bolded

## Visual Hierarchy

1. **Most Critical**: Bold Red (errors) and Bold Green (success)
2. **Structural**: Bold Yellow (sections) and Bold Cyan (banners)
3. **Contextual**: Cyan (tick numbers) and Bold Blue (tick values)
4. **Informational**: Blue (info) and Magenta (progress)

## Accessibility

Colors can be disabled by setting environment variables:
- `NO_COLOR=1` (standard convention)
- `STEAMROT_NO_COLOR=1` (SteamRot-specific)

When colors are disabled, all formatting and symbols remain, only colors are removed.

## Example Output Pattern

```
[Bold Cyan Banner]
┌─────────────────────────────────────
│ Running Test: [Bold]test_name[Reset]
└─────────────────────────────────────
[Reset]

[Bold Yellow]━━━━ Section Name ━━━━[Reset]

[Blue]•[Reset] Info message here

[Magenta]➤[Reset] Executing Tick [Bold Blue]1[Reset] of [Bold Blue]5[Reset]

[Blue]•[Reset] [Cyan][Tick 1][Reset] Action description

[Bold Green]✓[Reset] [Cyan][Tick 1][Reset] Success message

[Bold Red]✗[Reset] [Cyan][Tick 2][Reset] Error message
```

## Implementation Notes

- Colors are defined as `constexpr const char*` in the `Color` namespace
- `IsColorEnabled()` checks environment variables once and caches the result
- All print functions check color status before applying codes
- Reset code (`\033[0m`) always applied after colored text
- Functions work identically with or without colors (behavior unchanged)
