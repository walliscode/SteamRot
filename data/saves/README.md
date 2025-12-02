# Save Files Directory

This directory stores game save files.

## Structure

Save files are stored as FlatBuffers binary files:
- `slot_0.save.bin` - Save slot 1
- `slot_1.save.bin` - Save slot 2
- ... up to `slot_9.save.bin`

## Format

Each save file contains:
- **metadata**: Save name, timestamps, play time
- **current_scene_type**: The scene the player was in when saving
- **scene_states**: Entity states for each visited scene
- **global_event_bus**: Persistent event data

## Notes

- Empty save slots don't have files
- Save files are binary and not human-readable
- User should not manually edit save files
