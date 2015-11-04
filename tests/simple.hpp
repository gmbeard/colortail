#include <catch.hpp>

SCENARIO("A simple failing test")
{
  GIVEN("A simple BDD style test")
  {
    WHEN("We write a simple test...")
    {
      THEN("It fails")
      {
        REQUIRE(1 == 0);
      }
    }
  }
}
