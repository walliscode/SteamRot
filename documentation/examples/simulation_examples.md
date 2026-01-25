# Simulation Runner JSON Configuration Examples

This document provides comprehensive examples of JSON configurations for the Simulation Runner.

See SIMULATION_RUNNER_DESIGN.md for complete design details.

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

### Single Function Test
```json
{
  "simulation_data": {
    "steps": [{"function_type": "ProcessUIActionsAndEvents"}]
  },
  "num_ticks": 1
}
```

### Multi-Step Sequence
```json
{
  "simulation_data": {
    "steps": [
      {"logic_class_type": "UICollisionLogic"},
      {"function_type": "ProcessUIActionsAndEvents"},
      {"logic_class_type": "UIActionLogic"}
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
