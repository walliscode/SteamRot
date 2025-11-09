# Logic System Architecture

[← Back to Documentation](../README.md) | [Architecture Overview](../README.md#architecture)

This document describes the architecture of the Logic system, including Logic classes and LogicFactory.

**Related Documentation:**
- [Adding Logic](../workflows/ADDING_LOGIC.md) - Implementation workflow
- [Game Loop](GAME_LOOP.md) - Where logic fits in the game loop

---

## Classes

### Logic Class

The Logic class will be an abstract class. The derived classes will be
responsible for changing the game state, such as movement logic, physics logic,
e.t.c.

An inherited member function (PerformLogic()) will take in a unique pointer to
the EntityMemoryPool as well as a container of indices to the entities that need
to be updated. The logic class will then be responsible for updating the game
state of the entities in the container.

It will not be responsible for determining which archetype to use.

Logic objects will provided by the [LogicFactory](#logicfactory) abstract class.

### LogicFactory

The LogicFactory abstract class will be responsible for creating Logic objects.
It will have a pure virtual function (CreateLogic()) that will return a unique
pointer to a Logic object. So a MovementLogicFactory will create a MovementLogic
object depending on the parameters passed to it.
