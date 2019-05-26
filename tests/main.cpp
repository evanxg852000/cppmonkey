#define CATCH_CONFIG_MAIN 

#include "vendor/catch2.hpp"
#include "main/core.hpp"

TEST_CASE( "Additions are computed", "[add]" ) {
    REQUIRE( add(1, 3) == 4 );
    REQUIRE( add(2, 5) == 7 );
    REQUIRE( add(5, -2) == 3 );
}