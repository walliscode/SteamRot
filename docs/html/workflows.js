var workflows =
[
    [ "Adding Logic Classes", "adding_logic_classes.html", [
      [ "Common Workflows", "workflows.html#autotoc_md238", null ],
      [ "Table of Contents", "adding_logic_classes.html#autotoc_md125", null ],
      [ "Overview", "adding_logic_classes.html#autotoc_md127", null ],
      [ "When to Create a Logic Class", "adding_logic_classes.html#autotoc_md129", null ],
      [ "Logic System Architecture", "adding_logic_classes.html#autotoc_md131", [
        [ "LogicGrouping Categories", "adding_logic_classes.html#autotoc_md132", null ],
        [ "Execution Order", "adding_logic_classes.html#autotoc_md133", null ],
        [ "Key Components", "adding_logic_classes.html#autotoc_md134", null ]
      ] ],
      [ "Step-by-Step Workflow", "adding_logic_classes.html#autotoc_md136", [
        [ "Step 1: Create the Logic Class Files", "adding_logic_classes.html#autotoc_md137", [
          [ "1.1 Create Header File", "adding_logic_classes.html#autotoc_md138", null ],
          [ "1.2 Create Implementation File", "adding_logic_classes.html#autotoc_md139", null ],
          [ "1.3 Update CMakeLists.txt", "adding_logic_classes.html#autotoc_md140", null ]
        ] ],
        [ "Step 2: Register LogicType Enum", "adding_logic_classes.html#autotoc_md142", [
          [ "2.1 Add to LogicType Enum", "adding_logic_classes.html#autotoc_md143", null ],
          [ "2.2 Add to EnumNameLogicType Function", "adding_logic_classes.html#autotoc_md144", null ]
        ] ],
        [ "Step 3: Register with LogicFactory", "adding_logic_classes.html#autotoc_md146", [
          [ "3.1 Add Include", "adding_logic_classes.html#autotoc_md147", null ],
          [ "3.2 Add to CreateLogicObject Method", "adding_logic_classes.html#autotoc_md148", null ],
          [ "3.3 Add to Scene Configuration Method", "adding_logic_classes.html#autotoc_md149", null ]
        ] ],
        [ "Step 4: Write Unit Tests", "adding_logic_classes.html#autotoc_md151", [
          [ "4.1 Create Test File", "adding_logic_classes.html#autotoc_md152", null ],
          [ "4.2 Update Test CMakeLists.txt", "adding_logic_classes.html#autotoc_md153", null ]
        ] ],
        [ "Step 5: Update LogicFactory Tests", "adding_logic_classes.html#autotoc_md155", [
          [ "5.1 Add Include", "adding_logic_classes.html#autotoc_md156", null ],
          [ "5.2 Update Scene Test Case", "adding_logic_classes.html#autotoc_md157", null ]
        ] ],
        [ "Step 6: (Optional) Add FlatBuffers Configuration", "adding_logic_classes.html#autotoc_md159", [
          [ "6.1 Update FlatBuffers Schema", "adding_logic_classes.html#autotoc_md160", null ],
          [ "6.2 Update JSON Configuration File", "adding_logic_classes.html#autotoc_md161", null ],
          [ "6.3 Update FlatBuffers Provider", "adding_logic_classes.html#autotoc_md162", null ]
        ] ],
        [ "Step 7: Build and Test", "adding_logic_classes.html#autotoc_md164", [
          [ "7.1 Build the Project", "adding_logic_classes.html#autotoc_md165", null ],
          [ "7.2 Run Tests", "adding_logic_classes.html#autotoc_md166", null ],
          [ "7.3 Verify Integration", "adding_logic_classes.html#autotoc_md167", null ]
        ] ]
      ] ],
      [ "Complete Example", "adding_logic_classes.html#autotoc_md169", [
        [ "Header (<span class=\"tt\">src/logic/TimedColorLogic.h</span>)", "adding_logic_classes.html#autotoc_md170", null ],
        [ "Implementation (<span class=\"tt\">src/logic/TimedColorLogic.cpp</span>)", "adding_logic_classes.html#autotoc_md171", null ],
        [ "Test (<span class=\"tt\">tests/unit/logic/TimedColorLogic.test.cpp</span>)", "adding_logic_classes.html#autotoc_md172", null ]
      ] ],
      [ "Common Patterns", "adding_logic_classes.html#autotoc_md174", [
        [ "Archetype-Based Entity Processing", "adding_logic_classes.html#autotoc_md175", null ],
        [ "Rendering Logic", "adding_logic_classes.html#autotoc_md176", null ],
        [ "Event-Driven Logic", "adding_logic_classes.html#autotoc_md177", null ],
        [ "Asset Access", "adding_logic_classes.html#autotoc_md178", null ]
      ] ],
      [ "Best Practices", "adding_logic_classes.html#autotoc_md180", [
        [ "Design", "adding_logic_classes.html#autotoc_md181", null ],
        [ "Code Style", "adding_logic_classes.html#autotoc_md182", null ],
        [ "Performance", "adding_logic_classes.html#autotoc_md183", null ],
        [ "Testing", "adding_logic_classes.html#autotoc_md184", null ],
        [ "Error Handling", "adding_logic_classes.html#autotoc_md185", null ]
      ] ],
      [ "Troubleshooting", "adding_logic_classes.html#autotoc_md187", [
        [ "Logic Not Executing", "adding_logic_classes.html#autotoc_md188", null ],
        [ "Wrong Execution Order", "adding_logic_classes.html#autotoc_md189", null ],
        [ "Test Failures: \"dynamic_cast returned null\"", "adding_logic_classes.html#autotoc_md190", null ],
        [ "Build Errors: \"undefined reference\"", "adding_logic_classes.html#autotoc_md191", null ],
        [ "Components Not Found", "adding_logic_classes.html#autotoc_md192", null ],
        [ "FlatBuffers Configuration Not Loading", "adding_logic_classes.html#autotoc_md193", null ]
      ] ],
      [ "Summary Checklist", "adding_logic_classes.html#autotoc_md195", null ],
      [ "Additional Resources", "adding_logic_classes.html#autotoc_md197", null ]
    ] ],
    [ "Adding SFML Event Conversions", "adding_sfml_event_conversions.html", [
      [ "Table of Contents", "adding_sfml_event_conversions.html#autotoc_md200", null ],
      [ "Overview", "adding_sfml_event_conversions.html#autotoc_md202", null ],
      [ "Architecture", "adding_sfml_event_conversions.html#autotoc_md204", [
        [ "Class and File Map", "adding_sfml_event_conversions.html#autotoc_md205", null ],
        [ "Data Flow", "adding_sfml_event_conversions.html#autotoc_md206", null ],
        [ "Execution Order", "adding_sfml_event_conversions.html#autotoc_md207", null ]
      ] ],
      [ "Adding a New InputAction", "adding_sfml_event_conversions.html#autotoc_md209", [
        [ "Step 1: Add to the InputAction Enum", "adding_sfml_event_conversions.html#autotoc_md210", null ],
        [ "Step 2: Update the FlatBuffers Schema", "adding_sfml_event_conversions.html#autotoc_md211", null ],
        [ "Step 3: Update the Shared Configure Function", "adding_sfml_event_conversions.html#autotoc_md212", null ],
        [ "Step 5: Write Tests", "adding_sfml_event_conversions.html#autotoc_md213", null ]
      ] ],
      [ "Adding a New SFML Event Type", "adding_sfml_event_conversions.html#autotoc_md215", [
        [ "Step 1: Extend CollectInputEvents", "adding_sfml_event_conversions.html#autotoc_md216", null ],
        [ "Step 2: Extend UserInputBitset (if needed)", "adding_sfml_event_conversions.html#autotoc_md217", null ]
      ] ],
      [ "Adding a New EventPayload Type Conversion", "adding_sfml_event_conversions.html#autotoc_md219", [
        [ "Step 1: Add a Free Function in sfml_event_convert", "adding_sfml_event_conversions.html#autotoc_md220", null ],
        [ "Step 2: Call it from SFMLEventConverter", "adding_sfml_event_conversions.html#autotoc_md221", null ]
      ] ],
      [ "Startup Configuration", "adding_sfml_event_conversions.html#autotoc_md223", null ],
      [ "Best Practices", "adding_sfml_event_conversions.html#autotoc_md225", null ],
      [ "Troubleshooting", "adding_sfml_event_conversions.html#autotoc_md227", [
        [ "No InputPayload events appear on the bus", "adding_sfml_event_conversions.html#autotoc_md228", null ],
        [ "Registry is empty at runtime", "adding_sfml_event_conversions.html#autotoc_md229", null ],
        [ "New action enum value not recognised", "adding_sfml_event_conversions.html#autotoc_md230", null ],
        [ "Two patterns both match, wrong action fires", "adding_sfml_event_conversions.html#autotoc_md231", null ]
      ] ],
      [ "Summary Checklist", "adding_sfml_event_conversions.html#autotoc_md233", [
        [ "Adding a new InputAction", "adding_sfml_event_conversions.html#autotoc_md234", null ],
        [ "Adding a new SFML event type conversion", "adding_sfml_event_conversions.html#autotoc_md235", null ]
      ] ],
      [ "Additional Resources", "adding_sfml_event_conversions.html#autotoc_md237", null ]
    ] ],
    [ "Adding Descriptors", "adding_descriptors.html", [
      [ "Table of Contents", "adding_descriptors.html#autotoc_md102", null ],
      [ "Overview", "adding_descriptors.html#autotoc_md104", null ],
      [ "Descriptor Levels", "adding_descriptors.html#autotoc_md105", null ],
      [ "Decision Flowchart", "adding_descriptors.html#autotoc_md106", null ],
      [ "Key Files", "adding_descriptors.html#autotoc_md107", null ],
      [ "Adding a NodeDescriptor", "adding_descriptors.html#autotoc_md109", [
        [ "Declaration", "adding_descriptors.html#autotoc_md110", null ],
        [ "Definition", "adding_descriptors.html#autotoc_md111", null ]
      ] ],
      [ "Adding a ChainDescriptor", "adding_descriptors.html#autotoc_md112", [
        [ "Declaration", "adding_descriptors.html#autotoc_md113", null ],
        [ "Definition", "adding_descriptors.html#autotoc_md114", null ]
      ] ],
      [ "Composing Descriptors", "adding_descriptors.html#autotoc_md115", null ],
      [ "Testing Descriptors", "adding_descriptors.html#autotoc_md116", [
        [ "Node descriptors", "adding_descriptors.html#autotoc_md117", null ],
        [ "Chain descriptors", "adding_descriptors.html#autotoc_md118", null ]
      ] ],
      [ "Analysis Trace System", "adding_descriptors.html#autotoc_md119", null ],
      [ "Future Implementation Notes", "adding_descriptors.html#autotoc_md120", null ],
      [ "Best Practices", "adding_descriptors.html#autotoc_md121", null ],
      [ "Troubleshooting", "adding_descriptors.html#autotoc_md122", null ],
      [ "Summary Checklist", "adding_descriptors.html#autotoc_md123", null ]
    ] ]
];