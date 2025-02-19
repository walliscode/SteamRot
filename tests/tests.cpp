#include <catch2/catch_test_macros.hpp>

int test_int{5};

TEST_CASE("Test 5 ==5 ") { REQUIRE(test_int == 5); }
