# Adding Actions

[← Back to Documentation](../README.md) | [Workflows Overview](../README.md#workflows)

This guide describes how to add user input actions to SteamRot. Actions capture user input (keyboard, mouse) and make them available to game systems.

**Related Documentation:**
- [Architecture](../architecture/) - System design
- [Examples](../examples/) - Code examples

---

### Actions

This will attempt to describe the intent behind how actions are set up and how
an operator can add actions to the game.

#### Action Generation

Currently, actions are designed around capturing user input. Though I imagine
this will be expanded to act on global events e.t.c. The events are handled at
the beginning of the game loop (keyboard input and mouse input), this generates
a bit flag that. The bit flag will be available globally or passed to any Action
Managers that need it.

ActionManager instances will be placed in key locations such as Scene instance
and the DisplayManager. They will compare this global bit flag against their own
internal map of actions and generate another bit flag, this time of actions
(that have been defined in an enum). This will be made available to the Scene
Logic to allow for flow control.

These bit flags will be reset each tick of the game loop, so that they are only
valid for the current tick.

#### Action Registration

There is an enum called Actions which the ActionManager has access to. All
actions will be registered in this enum, the predicition being that their will
not be massives on actions that a user can take.

An operator then creates a json which maps keys to actions. The string
representation of the keys and mouse are checked by internal static maps for
correctness. The internal logic of the ActionManager creates bitset id for each
action that maps one or more key/mouse bindings to the action (as a bitflag).

So the steps an operator would take to add an action are:

- check the Action enum to see if the action exists, if not add it to the enum
  and recompile the code.
- create a json segment of the scene JSON that maps the key/mouse to the action.
  The json file should be in the format of:

```json
{
  "actions": [
    {
      "name": "action_name",
      "inputs": [{ "type": "keyboard/mouse", "value": "key_name/mouse_name" }]
    }
  ]
}
```
