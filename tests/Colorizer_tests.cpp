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
#include "Colorizer_tests.hpp"

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

