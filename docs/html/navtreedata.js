/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "SteamRot", "index.html", [
    [ "SteamRot Wiki", "index.html", "index" ],
    [ "Coordinate Spaces: Screen Space vs World Space", "md_documentation_2concepts_2COORDINATE__SPACES.html", [
      [ "Overview", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md1", null ],
      [ "The Two Spaces", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md3", [
        [ "Screen Space", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md4", null ],
        [ "World Space", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md5", null ]
      ] ],
      [ "The Camera Transform: Connecting the Two Spaces", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md7", [
        [ "How SFML Views Work", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md8", null ],
        [ "What Zoom Does", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md9", null ],
        [ "The Conversion Formula", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md10", null ]
      ] ],
      [ "Key Conceptual Points", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md12", [
        [ "1. Types Are Your Best Ally", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md13", null ],
        [ "2. Convert Exactly Once, At the Boundary", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md14", null ],
        [ "3. Different Systems Belong to Different Spaces — Keep Them Separate", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md15", null ],
        [ "4. The View Must Match the Rendering Pass", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md16", null ],
        [ "5. Non-Mouse Screen→World Conversions Are Separate Concerns", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md17", null ]
      ] ],
      [ "Summary", "md_documentation_2concepts_2COORDINATE__SPACES.html#autotoc_md19", null ]
    ] ],
    [ "Coordinate Spaces in SteamRot: Implementation Guide", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html", [
      [ "The Coordinate Contract", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md22", null ],
      [ "Where the Conversion Happens", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md24", [
        [ "Execution Order", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md25", null ]
      ] ],
      [ "The Ownership Chain", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md27", null ],
      [ "How Each Consumer Uses the Fields", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md29", [
        [ "UI Collision (<span class=\"tt\">UICollisionLogic</span> / <span class=\"tt\">collision_mouse.cpp</span>)", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md30", null ],
        [ "Grimoire Socket Collision (<span class=\"tt\">GrimoireMachinaCollisionLogic</span>)", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md31", null ],
        [ "Ghost Positioning (<span class=\"tt\">GhostPositioningLogic</span>)", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md32", null ],
        [ "Piece Placement (<span class=\"tt\">GrimoireMachinaActionLogic</span> / <span class=\"tt\">action_grimoire_machina.cpp</span>)", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md33", null ]
      ] ],
      [ "Pitfalls to Avoid", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md35", [
        [ "❌ Casting <span class=\"tt\">mouse_position</span> to float and using it as world position", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md36", null ],
        [ "❌ Calling <span class=\"tt\">mapPixelToCoords</span> with the wrong view", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md37", null ],
        [ "❌ Repeating the conversion in multiple Logic classes", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md38", null ],
        [ "❌ Passing <span class=\"tt\">world_mouse_position</span> to UI collision", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md39", null ],
        [ "❌ Adding a new world-space Logic that reads <span class=\"tt\">world_mouse_position</span> before <span class=\"tt\">GhostPositioningLogic</span> has run", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md40", null ]
      ] ],
      [ "Rules for Adding New World-Space Logic", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md42", null ],
      [ "Quick Reference: Which Field to Use", "md_documentation_2concepts_2COORDINATE__SPACES__STEAMROT.html#autotoc_md44", null ]
    ] ],
    [ "GrimoireMachina Logic Responsibility Map", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html", [
      [ "Table of Contents", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md47", null ],
      [ "Overview", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md49", null ],
      [ "Logic Classes", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md51", [
        [ "Collision — GrimoireMachinaCollisionLogic", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md52", null ],
        [ "Action — GrimoireMachinaActionLogic", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md54", null ],
        [ "Positioning — GrimoireMachinaPositioningLogic", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md56", null ],
        [ "Render — GrimoireMachinaRenderLogic", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md58", null ]
      ] ],
      [ "Free-Function Modules", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md60", [
        [ "Collision — <span class=\"tt\">collision_mouse</span>", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md61", null ],
        [ "Action — <span class=\"tt\">action_grimoire_machina</span>", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md63", null ],
        [ "Positioning — <span class=\"tt\">positioning_grimoire_machina</span>", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md65", null ],
        [ "Render — <span class=\"tt\">render_grimoire_machina</span>", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md67", null ]
      ] ],
      [ "Data Types Referenced", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md69", null ],
      [ "Execution Order in the CRAFTING Scene", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md71", null ],
      [ "Gaps and TODOs", "md_documentation_2design_2GRIMOIRE__MACHINA__LOGIC__MAP.html#autotoc_md73", null ]
    ] ],
    [ "Logic Decomposition: Keeping <span class=\"tt\">ProcessLogic</span> Clean", "md_documentation_2design_2LOGIC__DECOMPOSITION.html", [
      [ "Overview", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md75", null ],
      [ "The Three Section Types", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md77", [
        [ "1. Event Sections", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md78", null ],
        [ "2. Archetype Sections", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md79", null ],
        [ "3. Asset Sections", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md80", null ]
      ] ],
      [ "Naming and Namespace Strategy", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md82", null ],
      [ "<span class=\"tt\">ProcessLogic</span> as an Orchestrator", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md84", null ],
      [ "Free Functions vs. Member Functions", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md86", [
        [ "Prefer free functions", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md87", null ],
        [ "When member functions are acceptable", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md88", null ]
      ] ],
      [ "Guide: Adding a New Wrapper Function", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md90", [
        [ "Step 1 – Identify the section type", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md91", null ],
        [ "Step 2 – Locate or create the free-function module", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md92", null ],
        [ "Step 3 – Determine the minimal parameter list", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md93", null ],
        [ "Step 4 – Declare in the header, implement in the <span class=\"tt\">.cpp</span>", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md94", null ],
        [ "Step 5 – Call from ProcessLogic", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md95", null ],
        [ "Step 6 – Write a unit test", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md96", null ]
      ] ],
      [ "Summary", "md_documentation_2design_2LOGIC__DECOMPOSITION.html#autotoc_md98", null ]
    ] ],
    [ "Adding Logic Classes", "md_documentation_2workflows_2ADDING__LOGIC.html", [
      [ "Table of Contents", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md108", null ],
      [ "Overview", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md110", null ],
      [ "When to Create a Logic Class", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md112", null ],
      [ "Logic System Architecture", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md114", [
        [ "LogicGrouping Categories", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md115", null ],
        [ "Execution Order", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md116", null ],
        [ "Key Components", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md117", null ]
      ] ],
      [ "Step-by-Step Workflow", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md119", [
        [ "Step 1: Create the Logic Class Files", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md120", [
          [ "1.1 Create Header File", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md121", null ],
          [ "1.2 Create Implementation File", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md122", null ],
          [ "1.3 Update CMakeLists.txt", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md123", null ]
        ] ],
        [ "Step 2: Register LogicType Enum", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md125", [
          [ "2.1 Add to LogicType Enum", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md126", null ],
          [ "2.2 Add to EnumNameLogicType Function", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md127", null ]
        ] ],
        [ "Step 3: Register with LogicFactory", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md129", [
          [ "3.1 Add Include", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md130", null ],
          [ "3.2 Add to CreateLogicObject Method", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md131", null ],
          [ "3.3 Add to Scene Configuration Method", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md132", null ]
        ] ],
        [ "Step 4: Write Unit Tests", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md134", [
          [ "4.1 Create Test File", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md135", null ],
          [ "4.2 Update Test CMakeLists.txt", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md136", null ]
        ] ],
        [ "Step 5: Update LogicFactory Tests", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md138", [
          [ "5.1 Add Include", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md139", null ],
          [ "5.2 Update Scene Test Case", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md140", null ]
        ] ],
        [ "Step 6: (Optional) Add FlatBuffers Configuration", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md142", [
          [ "6.1 Update FlatBuffers Schema", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md143", null ],
          [ "6.2 Update JSON Configuration File", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md144", null ],
          [ "6.3 Update FlatBuffers Provider", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md145", null ]
        ] ],
        [ "Step 7: Build and Test", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md147", [
          [ "7.1 Build the Project", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md148", null ],
          [ "7.2 Run Tests", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md149", null ],
          [ "7.3 Verify Integration", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md150", null ]
        ] ]
      ] ],
      [ "Complete Example", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md152", [
        [ "Header (<span class=\"tt\">src/logic/TimedColorLogic.h</span>)", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md153", null ],
        [ "Implementation (<span class=\"tt\">src/logic/TimedColorLogic.cpp</span>)", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md154", null ],
        [ "Test (<span class=\"tt\">tests/unit/logic/TimedColorLogic.test.cpp</span>)", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md155", null ]
      ] ],
      [ "Common Patterns", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md157", [
        [ "Archetype-Based Entity Processing", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md158", null ],
        [ "Rendering Logic", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md159", null ],
        [ "Event-Driven Logic", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md160", null ],
        [ "Asset Access", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md161", null ]
      ] ],
      [ "Best Practices", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md163", [
        [ "Design", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md164", null ],
        [ "Code Style", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md165", null ],
        [ "Performance", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md166", null ],
        [ "Testing", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md167", null ],
        [ "Error Handling", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md168", null ]
      ] ],
      [ "Troubleshooting", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md170", [
        [ "Logic Not Executing", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md171", null ],
        [ "Wrong Execution Order", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md172", null ],
        [ "Test Failures: \"dynamic_cast returned null\"", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md173", null ],
        [ "Build Errors: \"undefined reference\"", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md174", null ],
        [ "Components Not Found", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md175", null ],
        [ "FlatBuffers Configuration Not Loading", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md176", null ]
      ] ],
      [ "Summary Checklist", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md178", null ],
      [ "Additional Resources", "md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md180", null ]
    ] ],
    [ "SFML Event Conversion Workflow", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html", [
      [ "Table of Contents", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md183", null ],
      [ "Overview", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md185", null ],
      [ "Architecture", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md187", [
        [ "Class and File Map", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md188", null ],
        [ "Data Flow", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md189", null ],
        [ "Execution Order", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md190", null ]
      ] ],
      [ "Adding a New InputAction", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md192", [
        [ "Step 1: Add to the InputAction Enum", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md193", null ],
        [ "Step 2: Update the FlatBuffers Schema", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md194", null ],
        [ "Step 3: Update the Shared Configure Function", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md195", null ],
        [ "Step 5: Write Tests", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md196", null ]
      ] ],
      [ "Adding a New SFML Event Type", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md198", [
        [ "Step 1: Extend CollectInputEvents", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md199", null ],
        [ "Step 2: Extend UserInputBitset (if needed)", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md200", null ]
      ] ],
      [ "Adding a New EventPayload Type Conversion", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md202", [
        [ "Step 1: Add a Free Function in sfml_event_convert", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md203", null ],
        [ "Step 2: Call it from SFMLEventConverter", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md204", null ]
      ] ],
      [ "Startup Configuration", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md206", null ],
      [ "Best Practices", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md208", null ],
      [ "Troubleshooting", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md210", [
        [ "No InputPayload events appear on the bus", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md211", null ],
        [ "Registry is empty at runtime", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md212", null ],
        [ "New action enum value not recognised", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md213", null ],
        [ "Two patterns both match, wrong action fires", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md214", null ]
      ] ],
      [ "Summary Checklist", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md216", [
        [ "Adding a new InputAction", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md217", null ],
        [ "Adding a new SFML event type conversion", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md218", null ]
      ] ],
      [ "Additional Resources", "md_documentation_2workflows_2SFML__EVENT__CONVERSION.html#autotoc_md220", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", "namespacemembers_dup" ],
        [ "Functions", "namespacemembers_func.html", "namespacemembers_func" ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ],
        [ "Enumerator", "namespacemembers_eval.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", "functions_vars" ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"ArchetypeManager_8cpp.html",
"SceneData_8h.html",
"classsteamrot_1_1EntityManager.html#af295b934c6a00a5267eefddd163bf0b7",
"classsteamrot_1_1ISaveDataProvider.html",
"classsteamrot_1_1logic_1_1GhostRenderLogic.html#a2c989dada7051cae1e1e253caddfb434",
"functions_func.html",
"md_documentation_2workflows_2ADDING__LOGIC.html#autotoc_md175",
"namespacesteamrot.html#a5dd2b6c05f378b284019be357690f4f2a9ecaa5260a098712998749f2f49c572b",
"namespacesteamrot.html#ab11b104f764f50b458c94eddffed00cd",
"namespacesteamrot_1_1data_1_1configure.html#ad71b59d25aadad9b8a30fd2df6776c34",
"structDropDownListStyle.html#a91709d6015ae8332af7167e33f7d17cb",
"structsteamrot_1_1DisplayConfigFbsBuilder.html",
"structsteamrot_1_1EngineDataFbsBuilder.html#a34565124c9755dee61a436fe0f3a3845",
"structsteamrot_1_1EventPayloadFbsTraits_3_01steamrot_1_1SystemPayloadFbs_01_4.html#a8ae1de53d7e87d2d14a66d0ca0872089",
"structsteamrot_1_1FLATBUFFERS__FINAL__CLASS.html#a47e4ddac5430827c42a03fd18f4489e5",
"structsteamrot_1_1FLATBUFFERS__FINAL__CLASS.html#a7cbb8ac297024d9f6c2b601c0e9feb92",
"structsteamrot_1_1FLATBUFFERS__FINAL__CLASS.html#ad759b5aea639a310860fe02c2c4545b5",
"structsteamrot_1_1IndexOf_3_01T_00_01std_1_1tuple_3_01U_00_01Ts_8_8_8_01_4_01_4.html#a7a49cbdf7ce6bebecb220413d24b7e6e",
"structsteamrot_1_1MachinaForm.html#a740f11319f5eb5214ca8743a341d8613",
"structsteamrot_1_1SceneDataFbsBuilder.html#aeac60009056f5618d50a1bd742ea5fdb",
"structsteamrot_1_1SocketState.html#accd82e5957a5b9cea964343776d90b9f",
"structsteamrot_1_1UIElementDataFbsBuilder.html#a8f53c2b2198c577f4cef363d0561b14a",
"structsteamrot_1_1UserPreferencesConfigFbsBuilder.html#a512ab780800bea39799acec9c1688d89"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';