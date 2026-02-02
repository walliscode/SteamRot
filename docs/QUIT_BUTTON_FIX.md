# Fix: TitleScene Quit Button Not Working

## Problem Summary

The quit button in the TitleScene was not triggering the QUIT_GAME event, causing the game window to remain open when the button was clicked.

## Root Cause

The quit button's `response_event_data` configuration in `data/defaults/scenes/title.scene_data.json` was missing the required `event_data_data_type` field.

### FlatBuffers Union Type Requirements

FlatBuffers unions require TWO fields in JSON:
1. `<field_name>_type`: Specifies which variant of the union is being used
2. `<field_name>`: Contains the actual data for that variant

For the `EventPacketData` table with its `event_data_data` union field:
- `event_data_data_type`: Must be one of: "NONE", "UserInputBitsetData", "SceneChangePacketData", "UserInterfaceNameData"
- `event_data_data`: The actual data (can be omitted when type is "NONE")

### What Was Wrong

**Before (INCORRECT):**
```json
"response_event_data": {
  "event_lifetime": 1,
  "event_type": "EVENT_QUIT_GAME"
}
```

**After (CORRECT):**
```json
"response_event_data": {
  "event_lifetime": 1,
  "event_type": "EVENT_QUIT_GAME",
  "event_data_data_type": "NONE"
}
```

## Why This Matters

Even though QUIT_GAME events don't need any associated data (they're "data-less" events), the FlatBuffers JSON parser still requires the union type field to be explicitly specified. Without it, the event may not be properly serialized/deserialized from the JSON to the binary format.

## Event Flow (For Reference)

When the quit button is clicked, here's what happens:

1. **Input Detection**: Mouse click generates a `USER_INPUT` event
2. **Subscription Activation**: Button's subscription (listening for `USER_INPUT` with `LEFT_CLICK`) becomes active
3. **Collision Detection**: `CheckMouseOverNestedUIElement` sets `button.is_mouse_over = true`
4. **Action Processing**: `ProcessButtonElementActions` checks:
   - Is subscription active? ✓
   - Is mouse over button? ✓
   - Does button have response_event? ✓
5. **Event Emission**: `event_handler.AddEvent(button.response_event.value())` adds QUIT_GAME event to waiting room
6. **Event Bus Processing**: `ProcessWaitingRoomEventBus()` moves QUIT_GAME to global bus
7. **Subscriber Activation**: Engine's QUIT_GAME subscriber is activated
8. **Game Exit**: `GameEngine::ProcessSubscriptions()` closes the window

## Best Practices

### When Creating Response Events in JSON

**For events WITH data:**
```json
"response_event_data": {
  "event_lifetime": 1,
  "event_type": "EVENT_CHANGE_SCENE",
  "event_data_data_type": "SceneChangePacketData",
  "event_data_data": { "scene_type": "TITLE" }
}
```

**For events WITHOUT data (data-less):**
```json
"response_event_data": {
  "event_lifetime": 1,
  "event_type": "EVENT_QUIT_GAME",
  "event_data_data_type": "NONE"
}
```

### Always Specify the Union Type

Even when the variant is "NONE" (no data), you MUST specify:
- `"event_data_data_type": "NONE"`

This ensures FlatBuffers properly serializes the event.

## Testing

A new test was added to verify the fix:
- **File**: `tests/unit/data_providers/configure_response_event.test.cpp`
- **Purpose**: Tests that `ConfigureBaseUIElement` properly handles data-less events
- **Run**: `ctest --preset Debug -R configure_response_event`

## Related Files

- `data/defaults/scenes/title.scene_data.json` - Quit button configuration (fixed)
- `src/data_providers/configure/configure_ui_elements.cpp` - Configuration logic
- `src/events/event_factory.cpp` - Event creation (handles NONE type correctly)
- `src/engine/GameEngine.cpp` - QUIT_GAME event handler
- `data/defaults/engine/default.engine_data.json` - QUIT_GAME subscriber registration

## Lessons Learned

1. **FlatBuffers unions are strict**: Always specify the `_type` field in JSON
2. **Check similar patterns**: Other buttons in the codebase already had the correct pattern
3. **Data-less events still need type specification**: Even with no data payload, specify "NONE"
4. **Consistency matters**: Following existing patterns helps avoid bugs
