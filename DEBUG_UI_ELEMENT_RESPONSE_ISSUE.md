# Debug Guide: UIElement Response Event Issue

## Problem Summary

UIElements in the title scene appear to be configured with the correct response data, but when clicking on them, they all trigger the same response event (either CHANGE_SCENE or QUIT_GAME depending on which buttons are present in the JSON).

## Hypothesized Root Causes

### 1. **Shared Pointer/Reference Issue** (Most Likely)
**Hypothesis**: Multiple button elements are sharing the same response_event object through pointer aliasing or improper copying.

**What to look for in logs**:
- Check if different button elements show the same address
- Compare element addresses during configuration vs. during action processing
- Verify each button gets a unique `response_event` during configuration

**Expected pattern if this is the issue**:
```
[ConfigureButtonElement] Button at 0x7fff1234 configured with label: 'Start Game'
[ConfigureBaseUIElement] Set response_event on element 0x7fff1234 with event_type: 1
[ConfigureButtonElement] Button at 0x7fff5678 configured with label: 'Crafting Table'
[ConfigureBaseUIElement] Set response_event on element 0x7fff5678 with event_type: 2
[ProcessButtonElementActions] Button 'Start Game' at 0x7fff1234 triggering response_event with type: 2  <-- WRONG! Should be type 1
```

**Key files to investigate**:
- `src/types/user_interface/UIElement.h` - CloneBaseUIElementData function (line 92: shared_ptr copy)
- `src/data_providers/configure/configure_ui_elements.cpp` - Response event assignment

### 2. **Child Processing Overwrites Parent Data**
**Hypothesis**: During recursive configuration, child elements' data overwrites parent or sibling data due to incorrect element references.

**What to look for in logs**:
- Configuration order: parent configured, then children
- Check if the same element address appears multiple times during configuration
- Verify children have different addresses than parent

**Expected pattern if this is the issue**:
```
[ConfigureBaseUIElement] Configuring element at address: 0x7fff1234  <-- Parent
[CreateUIElement] Creating ButtonElement at 0x7fff5678 with label: 'Start Game'  <-- Child 1
[ConfigureBaseUIElement] Configuring element at address: 0x7fff5678
[CreateUIElement] Creating ButtonElement at 0x7fff9abc with label: 'Crafting Table'  <-- Child 2
[ConfigureBaseUIElement] Configuring element at address: 0x7fff1234  <-- SAME AS PARENT! Bug!
```

### 3. **EventHandler Registration Side Effects**
**Hypothesis**: Registering subscribers with the EventHandler causes unintended modifications to element state.

**What to look for in logs**:
- Response events being set correctly during configuration
- Response events changing after EventHandler registration
- Subscription addresses being reused across elements

**Expected pattern if this is the issue**:
```
[ConfigureBaseUIElement] Element 0x7fff1234 has response_event_data
[ConfigureBaseUIElement] Set response_event on element 0x7fff1234 with event_type: 1
[ConfigureBaseUIElement] Created subscription at 0x7fff2222 for element 0x7fff1234
[ProcessButtonElementActions] Button 'Start Game' at 0x7fff1234 triggering response_event with type: 2  <-- Changed!
```

### 4. **Clone Operation Sharing State**
**Hypothesis**: If UIElements are cloned (via CUserInterface copy constructor), the Clone() operation copies shared_ptr subscriptions, causing elements to share the same Subscriber object.

**What to look for in logs**:
- Same subscription addresses across multiple elements
- Elements that should be independent showing identical subscription pointers

**Expected pattern if this is the issue**:
```
[ConfigureBaseUIElement] Created subscription at 0x7fff2222 for element 0x7fff1234
[ConfigureBaseUIElement] Created subscription at 0x7fff2222 for element 0x7fff5678  <-- SAME ADDRESS!
```

### 5. **FlatBuffers Data Pointer Reuse**
**Hypothesis**: FlatBuffers data pointers are being reused incorrectly, causing the same data to be read for multiple elements.

**What to look for in logs**:
- All buttons showing the same event_type during initial configuration
- No variation in event types even when JSON has different values

**Expected pattern if this is the issue**:
```
[ConfigureBaseUIElement] Element 0x7fff1234 event_type: 2  <-- Should be different
[ConfigureBaseUIElement] Element 0x7fff5678 event_type: 2  <-- All the same!
[ConfigureBaseUIElement] Element 0x7fff9abc event_type: 2  <-- All the same!
```

## How to Diagnose

### Step 1: Build and Run with Debug Logging
```bash
# Build the project (done locally)
cmake --preset Debug
cmake --build --preset Debug

# Run the application
./build/Debug/steamrot
```

### Step 2: Navigate to Title Scene
- Start the application
- Navigate to the title scene
- **DO NOT CLICK ANY BUTTONS YET**
- Check configuration logs first

### Step 3: Analyze Configuration Logs
Look for patterns in the configuration phase:

1. **Count button creations**: Should see 3 buttons created with labels "Start Game", "Crafting Table", "Quit Game"
2. **Verify unique addresses**: Each button should have a different memory address
3. **Check event types**: 
   - "Start Game" should have NO response_event_data
   - "Crafting Table" should have event_type: CHANGE_SCENE (type value varies)
   - "Quit Game" should have event_type: QUIT_GAME

### Step 4: Click Buttons and Compare
- Click each button one at a time
- Compare the event_type logged during configuration vs. during action processing
- Note any discrepancies

### Step 5: Test with Modified JSON
To isolate the issue, try these JSON modifications:

**Test A: Remove "Crafting Table" button**
- Remove the second button from JSON
- Rebuild/rerun
- Check if "Start Game" and "Quit Game" now both trigger QUIT_GAME

**Test B: Swap button order**
- Move "Quit Game" button to be first in the children array
- Check if all buttons now trigger QUIT_GAME instead of CHANGE_SCENE

**Test C: Add response_event to "Start Game"**
- Add a response_event_data to the first button
- Check if that event is the one that all buttons trigger

## Expected Normal Behavior

### Configuration Logs Should Show:
```
[CreateUIElement] Creating ButtonElement at 0x7fff1234 with label: 'Start Game'
[ConfigureBaseUIElement] Configuring element at address: 0x7fff1234
[ConfigureBaseUIElement] Element 0x7fff1234 has NO response_event_data
[ConfigureBaseUIElement] Element 0x7fff1234 has NO subscriber_data

[CreateUIElement] Creating ButtonElement at 0x7fff5678 with label: 'Crafting Table'
[ConfigureBaseUIElement] Configuring element at address: 0x7fff5678
[ConfigureBaseUIElement] Element 0x7fff5678 has response_event_data
[ConfigureBaseUIElement] Element 0x7fff5678 event_type: 5  (or whatever CHANGE_SCENE is)
[ConfigureBaseUIElement] Set response_event on element 0x7fff5678 with event_type: 5
[ConfigureBaseUIElement] Element 0x7fff5678 has subscriber_data
[ConfigureBaseUIElement] Created subscription at 0x7fff2222 for element 0x7fff5678

[CreateUIElement] Creating ButtonElement at 0x7fff9abc with label: 'Quit Game'
[ConfigureBaseUIElement] Configuring element at address: 0x7fff9abc
[ConfigureBaseUIElement] Element 0x7fff9abc has response_event_data
[ConfigureBaseUIElement] Element 0x7fff9abc event_type: 3  (or whatever QUIT_GAME is)
[ConfigureBaseUIElement] Set response_event on element 0x7fff9abc with event_type: 3
[ConfigureBaseUIElement] Element 0x7fff9abc has subscriber_data
[ConfigureBaseUIElement] Created subscription at 0x7fff3333 for element 0x7fff9abc
```

### Action Logs Should Show:
When clicking "Start Game":
```
[ProcessButtonElementActions] Button 'Start Game' at 0x7fff1234 has response_event: false
(no event triggered)
```

When clicking "Crafting Table":
```
[ProcessButtonElementActions] Button 'Crafting Table' at 0x7fff5678 triggering response_event with type: 5
```

When clicking "Quit Game":
```
[ProcessButtonElementActions] Button 'Quit Game' at 0x7fff9abc triggering response_event with type: 3
```

## Next Steps Based on Findings

### If Hypothesis 1 is confirmed (Shared Pointer/Reference):
- Investigate UIElement::CloneBaseUIElementData
- Check if elements are being improperly aliased
- Look for unintended shared_ptr copying

### If Hypothesis 2 is confirmed (Child Overwrites Parent):
- Investigate the recursive callback in ConfigureBaseUIElement
- Check element references during child processing
- Verify std::move semantics in CreateUIElement

### If Hypothesis 3 is confirmed (EventHandler Side Effects):
- Investigate EventHandler::RegisterSubscriber implementation
- Check if registration modifies element state
- Look for global state that affects element configuration

### If Hypothesis 4 is confirmed (Clone Sharing State):
- Fix UIElement::CloneBaseUIElementData to create new Subscriber instances
- Ensure deep copy of response_event
- Update CUserInterface copy constructor if needed

### If Hypothesis 5 is confirmed (FlatBuffers Data Reuse):
- Check FlatbuffersUIElementProvider::CreateUIElement pointer handling
- Verify each child gets correct data pointer
- Look for iterator/pointer issues in child processing loop

## Additional Debug Flags

To get even more detailed logging, you can temporarily add:
- `spdlog::set_level(spdlog::level::debug);` at the start of main()
- Add logging to UIElement::CloneBaseUIElementData if cloning is suspected
- Add logging to EventHandler::RegisterSubscriber to track registration

## Contact

If you identify the root cause through these logs, please document your findings and the fix applied.
