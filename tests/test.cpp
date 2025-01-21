#define CATCH_CONFIG_MAIN 
#include "catch.hpp"


int addTwoNumbers(int a, int b) {
    return a + b;
}

TEST_CASE( "Sample test case", "[sample]") {
    REQUIRE(addTwoNumbers(1, 2) == 3);
    REQUIRE(addTwoNumbers(11, 2) == 13);
    REQUIRE(addTwoNumbers(18, 82) == 100);
}
