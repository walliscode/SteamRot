#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

namespace TestUtility {

using json = nlohmann::json;

inline void check_structure(json defined_structure, json test_structure) {
  for (auto &[key, value] : defined_structure.items()) {
    // first check that the key exists in the test structure
    INFO("checking key: " << key << " exists in test structure");
    REQUIRE(test_structure.contains(key));

    // second check that the test structure is not longer than the defined
    // structure
    INFO("checking that the test structure is not longer than the defined "
         "structure");

    // rather than using == we want the test structure to be able to be shorter.
    // This allows for TDD we add the structure we want first (the defined
    // strucutre) and test for that. then we add the actual structure
    REQUIRE(test_structure.size() <= defined_structure.size());

    // third check that the key is a dictionary, if yes then recursively check
    if (value.is_object()) {

      check_structure(value, test_structure[key]);
    }
  }
}

} // namespace TestUtility
