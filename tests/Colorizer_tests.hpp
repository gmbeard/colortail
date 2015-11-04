#include <catch.hpp>
#include "TailFile.h"
#include "Colorizer.h"
#include "shared_ptr/shared_ptr.hpp"

#include <stdlib.h>

TEST_CASE("Colorizer Tests")
{
  SECTION("Default c'tor initializes object in valid state")
  {
    Colorizer colorizer;
    REQUIRE(colorizer.m_items_list.is_empty());
  }

  SECTION("Constructing with config file initializes list")
  {
    Colorizer colorizer("../example-conf/conf.messages");
    REQUIRE(!colorizer.m_items_list.is_empty());
  }
}
