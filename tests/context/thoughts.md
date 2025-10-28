# Thoughts on Testing Strategy

## Resources

- resource set up for the game and scene should be mirrored in the test setup,
  so we can reuse set up code and just pass in data to customize for each test.
- consider using fixtures to manage common setup and teardown tasks, though
  catch 2 does not have built-in fixture support, we can create helper functions
  to achieve similar results.

## Context

- we do not need a specific test context, but maybe some kind of test
  environment object with different levels of initiation.
- The test environment object should be able to produce any of these contexts.
