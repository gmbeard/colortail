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
