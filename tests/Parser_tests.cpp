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
#include <cassert>

#include "parser/expression_stack.hpp"
#include "parser/expression.hpp"
#include "parser/expression_types.hpp"
#include "Parser_tests.hpp"

namespace
{
  /*
    config_block  ::= '{', (config_line)*, '}'
    config_line   ::= (any_non_newline)*, newline
  */
  char const *kParseLine1 =
    "{\nOne\nTwo\nThree\n}";

  template<typename T>
  struct exact_value
  {
    exact_value(T t)
      : t_(t)
    { }

    template<typename U>
    friend bool matches(U c, exact_value<U> const *prod)
    {
      return (c == (*prod).t_);
    }

    template<typename U>
    friend bool evaluate(U c, exact_value<U> const *prod)
    {
      return matches(c, prod);
    }

    template<typename U>
    friend void expand(U c, 
      colortail::parser::expression_stack &s, 
        exact_value<U> const *prod)
    {
      push(s, *prod);
    }

  private:
    T t_;
  };

  struct new_line_prod
  {
    friend bool evaluate(char c, new_line_prod const *)
    {
      return (c == '\r' || c == '\n');
    }

    friend void expand(char c, colortail::parser::expression_stack &s,
      new_line_prod const *)
    {
      if(c == '\r') {
        push(s, new_line_prod());
      }
      push(s, exact_value<char>(c));
    }
  };

  struct any_non_newline
  {
    friend bool evaluate(char c, any_non_newline const *)
    {
      return c != '\n' && c != '\r';
    }

    friend void expand(char c, colortail::parser::expression_stack &s,
      any_non_newline const *)
    {
      push(s, exact_value<char>(c));
    }
  };

  struct config_line
  {
    config_line()
      : repeat_(any_non_newline(), new_line_prod())
    { }

    friend bool evaluate(char c, config_line const *prod)
    {
      return evaluate(c, &(*prod).repeat_);
    }

    friend void expand(char c, colortail::parser::expression_stack &s,
      config_line const *prod)
    {
      expand(c, s, &(*prod).repeat_);
    }
  private:
    colortail::parser::repeat_until_expression repeat_;
  };

  struct config_block
  {
    friend bool evaluate(char c, config_block const *)
    {
      return c == '{';
    }

    friend void expand(char c, colortail::parser::expression_stack &s,
      config_block const *)
    {
      typedef colortail::parser::repeat_until_expression
        repeater;

      push(s, repeater(config_line(), exact_value<char>('}')));
      push(s, exact_value<char>('{'));;
    }
  };

  template<typename InputIterator>
  bool parse_input(InputIterator first, InputIterator last)
  {
    colortail::parser::expression_stack stack;
    push(stack, config_block());
    InputIterator curr = first;
    colortail::parser::unique_expression_ptr expr;

    while(curr != last) {

      if(top(stack).matches(*curr)) {
        assert(pop(stack, expr));
        ++curr;
      }
      else if(top(stack).evaluate(*curr)) {
        assert(pop(stack, expr));
        (*expr).expand(*curr, stack);
      }
      else {
        return false;
      }
    }

    return true;
  }
}

TEST_CASE("Parser Tests")
{
  SECTION("Initializing stack with one expression leaves non-empty stack")
  {
    namespace ctp = colortail::parser;

    ctp::expression_stack stack;
    push(stack, int(42));
    ctp::unique_expression_ptr p;
    
    REQUIRE(pop(stack, p));
    REQUIRE(p);
    REQUIRE(!pop(stack, p));
  }

  SECTION("Pushing a literal value on the stack results in a match for the top item")
  {
    namespace ctp = colortail::parser;

    ctp::expression_stack stack;
    push(stack, exact_value<char>('g'));
  
    REQUIRE(top(stack).matches('g'));
  }

  SECTION("Parsing test config block succeeds")
  {
    REQUIRE(parse_input(kParseLine1, kParseLine1 + strlen(kParseLine1)));
  }
}

