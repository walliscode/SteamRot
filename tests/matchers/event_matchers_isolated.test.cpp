/////////////////////////////////////////////////
/// @file
/// @brief Isolated unit tests for Event matcher string output
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_matchers.h"
#include "matcher_test_helpers.h"
#include "EventPacket.h"
#include "EventHandler.h"
#include "UserInputBitset.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EventPacket matcher describe() output for matching events",
          "[unit][matchers][Event][isolated]") {
  
  steamrot::EventPacket expected;
  expected.m_event_type = steamrot::EventType::EventType_UserInput;
  expected.m_event_data = steamrot::UserInputBitset{};
  
  steamrot::EventPacket actual;
  actual.m_event_type = steamrot::EventType::EventType_UserInput;
  actual.m_event_data = steamrot::UserInputBitset{};
  
  steamrot::tests::EventPacketEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match succeeds when event packets are equal") {
    REQUIRE(result.matched);
  }
  
  SECTION("Description contains PASSED indicator") {
    REQUIRE(result.description.find("PASSED") != std::string::npos);
  }
  
  SECTION("Description contains EventPacket identifier") {
    REQUIRE(result.description.find("EventPacket") != std::string::npos);
  }
  
  SECTION("Description has proper structure") {
    REQUIRE(steamrot::tests::ValidateOutputStructure(
        result.description, true, true, false));
  }
}

TEST_CASE("EventPacket matcher describe() output for event type mismatch",
          "[unit][matchers][Event][isolated]") {
  
  steamrot::EventPacket expected;
  expected.m_event_type = steamrot::EventType::EventType_UserInput;
  expected.m_event_data = std::monostate{};
  
  steamrot::EventPacket actual;
  actual.m_event_type = steamrot::EventType::EventType_SceneChange;  // Mismatch
  actual.m_event_data = std::monostate{};
  
  steamrot::tests::EventPacketEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when event type differs") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description contains FAILED indicator") {
    REQUIRE(result.description.find("FAILED") != std::string::npos);
  }
  
  SECTION("Description mentions event type") {
    bool has_type_info = result.description.find("event_type") != std::string::npos ||
                         result.description.find("EventType") != std::string::npos;
    REQUIRE(has_type_info);
  }
  
  SECTION("Description contains actual and expected labels") {
    REQUIRE(result.description.find("actual") != std::string::npos);
    REQUIRE(result.description.find("expected") != std::string::npos);
  }
}

TEST_CASE("EventPacket matcher describe() output for event data variant type mismatch",
          "[unit][matchers][Event][isolated]") {
  
  steamrot::EventPacket expected;
  expected.m_event_type = steamrot::EventType::EventType_UserInput;
  expected.m_event_data = steamrot::UserInputBitset{};
  
  steamrot::EventPacket actual;
  actual.m_event_type = steamrot::EventType::EventType_UserInput;
  actual.m_event_data = std::monostate{};  // Different variant type
  
  steamrot::tests::EventPacketEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when event data variant types differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description mentions variant type") {
    bool has_variant_info = result.description.find("variant") != std::string::npos ||
                            result.description.find("type") != std::string::npos ||
                            result.description.find("monostate") != std::string::npos;
    REQUIRE(has_variant_info);
  }
}

TEST_CASE("EventPacket matcher describe() output for UserInputBitset mismatch",
          "[unit][matchers][Event][isolated]") {
  
  steamrot::UserInputBitset expected_bitset;
  expected_bitset.set(0);  // Set first bit
  
  steamrot::UserInputBitset actual_bitset;
  actual_bitset.set(1);  // Set different bit
  
  steamrot::EventPacket expected;
  expected.m_event_type = steamrot::EventType::EventType_UserInput;
  expected.m_event_data = expected_bitset;
  
  steamrot::EventPacket actual;
  actual.m_event_type = steamrot::EventType::EventType_UserInput;
  actual.m_event_data = actual_bitset;
  
  steamrot::tests::EventPacketEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when UserInputBitset values differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description mentions bitset or UserInput") {
    bool has_bitset_info = result.description.find("bitset") != std::string::npos ||
                           result.description.find("UserInput") != std::string::npos;
    REQUIRE(has_bitset_info);
  }
}

TEST_CASE("EventPacket matcher describe() output for multiple field mismatches",
          "[unit][matchers][Event][isolated]") {
  
  steamrot::EventPacket expected;
  expected.m_event_type = steamrot::EventType::EventType_UserInput;
  expected.m_event_data = steamrot::UserInputBitset{};
  expected.m_target_entity_id = 1;
  
  steamrot::EventPacket actual;
  actual.m_event_type = steamrot::EventType::EventType_SceneChange;  // Mismatch
  actual.m_event_data = std::monostate{};  // Mismatch
  actual.m_target_entity_id = 2;  // Mismatch
  
  steamrot::tests::EventPacketEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when multiple fields differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description is not empty") {
    REQUIRE_FALSE(result.description.empty());
  }
  
  SECTION("Description contains failure indicator") {
    REQUIRE(result.description.find("FAILED") != std::string::npos);
  }
}

TEST_CASE("EventBus matcher describe() output for matching buses",
          "[unit][matchers][EventBus][isolated]") {
  
  steamrot::EventHandler expected_bus;
  steamrot::EventHandler actual_bus;
  
  steamrot::tests::EventBusEqualsMatcher matcher(expected_bus);
  auto result = steamrot::tests::TestMatcherOutput(actual_bus, matcher);
  
  SECTION("Match succeeds when buses are empty and equal") {
    REQUIRE(result.matched);
  }
  
  SECTION("Description contains PASSED indicator") {
    REQUIRE(result.description.find("PASSED") != std::string::npos);
  }
  
  SECTION("Description contains EventBus identifier") {
    REQUIRE(result.description.find("EventBus") != std::string::npos);
  }
}

TEST_CASE("EventBus matcher describe() output for size mismatch",
          "[unit][matchers][EventBus][isolated]") {
  
  steamrot::EventHandler expected_bus;
  steamrot::EventHandler actual_bus;
  
  // Add an event to actual bus
  steamrot::EventPacket event;
  event.m_event_type = steamrot::EventType::EventType_UserInput;
  actual_bus.AddEvent(event);
  
  steamrot::tests::EventBusEqualsMatcher matcher(expected_bus);
  auto result = steamrot::tests::TestMatcherOutput(actual_bus, matcher);
  
  SECTION("Match fails when bus sizes differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description mentions size or count") {
    bool has_size_info = result.description.find("size") != std::string::npos ||
                         result.description.find("count") != std::string::npos;
    REQUIRE(has_size_info);
  }
}

TEST_CASE("EventBus matcher describe() output for event differences",
          "[unit][matchers][EventBus][isolated]") {
  
  steamrot::EventHandler expected_bus;
  steamrot::EventHandler actual_bus;
  
  steamrot::EventPacket expected_event;
  expected_event.m_event_type = steamrot::EventType::EventType_UserInput;
  expected_bus.AddEvent(expected_event);
  
  steamrot::EventPacket actual_event;
  actual_event.m_event_type = steamrot::EventType::EventType_SceneChange;
  actual_bus.AddEvent(actual_event);
  
  steamrot::tests::EventBusEqualsMatcher matcher(expected_bus);
  auto result = steamrot::tests::TestMatcherOutput(actual_bus, matcher);
  
  SECTION("Match fails when events differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description is hierarchical with event details") {
    // Should contain both EventBus and EventPacket level information
    bool has_bus_info = result.description.find("EventBus") != std::string::npos;
    REQUIRE(has_bus_info);
  }
}

TEST_CASE("Event matcher helper functions",
          "[unit][matchers][Event][isolated]") {
  
  SECTION("EqualsEventPacket helper creates valid matcher") {
    steamrot::EventPacket expected;
    expected.m_event_type = steamrot::EventType::EventType_UserInput;
    
    steamrot::EventPacket actual;
    actual.m_event_type = steamrot::EventType::EventType_UserInput;
    
    auto matcher = steamrot::tests::EqualsEventPacket(expected);
    auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
    
    REQUIRE(result.matched);
    REQUIRE_FALSE(result.description.empty());
  }
  
  SECTION("EqualsEventBus helper creates valid matcher") {
    steamrot::EventHandler expected_bus;
    steamrot::EventHandler actual_bus;
    
    auto matcher = steamrot::tests::EqualsEventBus(expected_bus);
    auto result = steamrot::tests::TestMatcherOutput(actual_bus, matcher);
    
    REQUIRE(result.matched);
    REQUIRE_FALSE(result.description.empty());
  }
}

TEST_CASE("Event matcher output format consistency",
          "[unit][matchers][Event][isolated]") {
  
  SECTION("Passed and failed outputs have consistent structure") {
    steamrot::EventPacket expected;
    expected.m_event_type = steamrot::EventType::EventType_UserInput;
    
    // Passing case
    steamrot::EventPacket actual_pass = expected;
    steamrot::tests::EventPacketEqualsMatcher matcher_pass(expected);
    auto result_pass = steamrot::tests::TestMatcherOutput(actual_pass, matcher_pass);
    
    // Failing case
    steamrot::EventPacket actual_fail;
    actual_fail.m_event_type = steamrot::EventType::EventType_SceneChange;
    steamrot::tests::EventPacketEqualsMatcher matcher_fail(expected);
    auto result_fail = steamrot::tests::TestMatcherOutput(actual_fail, matcher_fail);
    
    // Both should have dividers
    bool pass_has_dividers = result_pass.description.find("===") != std::string::npos ||
                             result_pass.description.find("---") != std::string::npos;
    bool fail_has_dividers = result_fail.description.find("===") != std::string::npos ||
                             result_fail.description.find("---") != std::string::npos;
    
    REQUIRE(pass_has_dividers);
    REQUIRE(fail_has_dividers);
  }
}
