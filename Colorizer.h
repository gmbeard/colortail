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

#ifndef _Colorizer_h_
#define _Colorizer_h_

#include "config.h"
#include "List.h"
#include "CfgFileParser.h"
#include "shared_ptr/shared_ptr.hpp"

#ifdef UNIT_TESTING
# define TESTING_FORCE_PUBLIC \
  public:
# define TESTING_FORCE_BACK_TO(x) \
   x:
#else
# define TESTING_FORCE_PUBLIC
# define TESTING_FORCE_BACK_TO(x)
#endif

using namespace std;

#define ANSI_RESET_STR "\033[0;0m"

class Colorizer
{
  private:
TESTING_FORCE_PUBLIC
   List<gmb::memory::shared_ptr<SearchData> > m_items_list;

   //void free_items();

   Colorizer();

TESTING_FORCE_BACK_TO(private)
      
  public:
      Colorizer(const char *cfg_file);
      Colorizer(Colorizer const &);
      ~Colorizer();

      string colorize(const char *str);
};


#endif
