# Error and Exception Handling

[← Back to Documentation](../README.md) | [Architecture Overview](../README.md#architecture)

This document describes SteamRot's error and exception handling philosophy and patterns.

**Related Documentation:**
- [Testing Overview](../testing/TESTING_OVERVIEW.md) - Error testing patterns
- [Style Guide](../style/STYLE_GUIDE.md) - Code conventions

---

## Error/Exception handling

In general, we want the program to fail if it encounters an exception. We do not
want to propagate failures further into the code (failure to load assets
e.t.c.). However, we want to fail gracefully and give the user a good experience
upon failure, not just a program crashing.

### Exceptions handling

We will let exceptions propagate up through the stack and _not_ use try/catch
statements. Except for the one try/catch statement.

This try/catch statement will be at the top game loop level and move us out of
the general game loop and into some kind of Error screen, where we will display
the error information, actionable info and the option to quit/reload. The goal
will to keep the dependencies small so that further errors can't be generated in
the Error scene (maybe self contained assets)

### Error handling

Where we expect a certain outcome from code but a runtime dependency could cause
a bug (such as a required file being missing) we will use std::expected to
indicate intent and then provide a mechanism for handling the error.

The current method will be to have an ErrorHandler namespace, this takes in an
Error object and processes it depending on severity e.g. critical errors will
throw an exception which then gets call backed up the stack and handled like
other exceptions.
