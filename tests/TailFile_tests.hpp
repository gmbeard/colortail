#include <catch.hpp>
#include "TailFile.h"
#include "Colorizer.h"
#include "shared_ptr/shared_ptr.hpp"

#include <stdlib.h>

struct char_deleter
{
  void operator()(void *p)
  {
    free(p);
  }
};

TEST_CASE("TailFile objects")
{
  typedef gmb::memory::shared_ptr<char, char_deleter> char_ptr;
  TailFile f;

  SECTION("Empty shared_ptr<Colorizer> should report 'false'")
  {
    gmb::memory::shared_ptr<Colorizer> c;
    REQUIRE(!c);
  }

  SECTION("Opening file results in valid state") 
  {
    gmb::memory::shared_ptr<Colorizer> colorizer; 
    char_ptr filename(strdup("/var/log/system.log"));
    
    REQUIRE(0 == f.open(filename.get(), colorizer));
  }
  
  SECTION("Opening a non-existent file results in invalid state") 
  {
    gmb::memory::shared_ptr<Colorizer> colorizer; 
   // TailFile f;
    char_ptr filename(strdup("fkslfjsad"), char_deleter());
    
    REQUIRE(0 != f.open(filename.get(), colorizer));
  }

  SECTION("Reading from file succeeds")
  {
    INFO("MAX_CHARS_READ: " << MAX_CHARS_READ);
    gmb::memory::shared_ptr<Colorizer> colorizer(new Colorizer("../example-conf/conf.messages"));
    char_ptr filename(strdup("/var/log/system.log"), char_deleter());
    REQUIRE(0 == f.open(filename.get(), colorizer));

    f.print(10);
  }
}
