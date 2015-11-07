/*
    colortail -- output last part of file(s) in color.
    Copyright (C) 2009  Joakim Ek <joakim@pjek.se>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <catch.hpp>
#include <stdlib.h>

#include "TailFile.h"
#include "Colorizer.h"
#include "shared_ptr/shared_ptr.hpp"
#include "TailFile_tests.hpp"

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
