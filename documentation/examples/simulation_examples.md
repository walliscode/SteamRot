# Simulation Runner JSON Configuration Examples

This document provides comprehensive examples of JSON configurations for the Simulation Runner.

See SIMULATION_RUNNER_DESIGN.md for complete design details.

**Note**: The design uses only Logic classes. Free functions are not supported as they operate on individual UIElements and would require manual entity iteration, duplicating what Logic classes already do.

## Quick Examples

### Single Logic Class Test
```json
{
  "simulation_data": {
    "steps": [{"logic_class_type": "UICollisionLogic"}]
  },
  "num_ticks": 1
}
```

### Multi-Step Logic Sequence
```json
{
  "simulation_data": {
    "steps": [
      {"logic_class_type": "UICollisionLogic"},
      {"logic_class_type": "UIActionLogic"},
      {"logic_class_type": "UIStateLogic"}
    ]
  },
  "num_ticks": 1
}
```

### Complete UI Frame
```json
{
  "simulation_data": {
    "steps": [
      {"logic_class_type": "UICollisionLogic"},
      {"logic_class_type": "UIActionLogic"},
      {"logic_class_type": "UIStateLogic"},
      {"logic_class_type": "UIRenderLogic"}
    ]
  },
  "num_ticks": 1
}
```

### Multi-Tick Test
```json
{
  "simulation_data": {
    "steps": [{"logic_class_type": "UIStateLogic"}]
  },
  "num_ticks": 5
}
```

Refer to the main design document for detailed explanations and additional examples.
