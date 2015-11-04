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

#ifndef COLORTAILTYPE_H_INCLUDED
#define COLORTAILTYPE_H_INCLUDED 1

#include <stdlib.h>
#include <regex.h>
#include "shared_ptr/shared_ptr.hpp"

namespace detail
{
  struct regex_t_deleter
  {
    void operator()(void *p)
    {
      if(p) {
        regfree(static_cast<regex_t *>(p));
      }
      free(p);
    }
  };

  struct char_deleter
  {
    void operator()(void *p)
    {
      free(p);
    }
  };
}

typedef gmb::memory::shared_ptr<regex_t, detail::regex_t_deleter>
  regex_t_ptr;

typedef gmb::memory::shared_ptr<char, detail::char_deleter>
  char_ptr;

inline char_ptr make_char_ptr(size_t n)
{
  return char_ptr(malloc(n));
}

inline regex_t_ptr make_regex_t_ptr()
{
  return regex_t_ptr(malloc(sizeof(regex_t)));
}

#endif //COLORTAILTYPE_H_INCLUDED
