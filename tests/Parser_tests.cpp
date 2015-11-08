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
    config_block  ::= '{', newline, (config_line)*, '}'
    config_line   ::= (any_non_newline)*, newline
  */
  char const *kParseLine1 =
    "{\nOne\nTwo\nThr}ee\n}";

  enum ids {
    CONFIG_BLOCK_START,
    CONFIG_BLOCK_END,
    CONFIG_LINE_START,
    CONFIG_LINE_END,
    CONFIG_ITEM_DATA,
    IGNORE
  };

  template<typename Fn>
  struct prod_base
  {
    typedef Fn callback_type;

    prod_base(callback_type f)
      : f_(f)
    { }

    template<typename T>
    void operator()(T t, ids id) const
    {
      const_cast<Fn &>(f_)(t, id);
    }

    callback_type const & callback() const
    {
      return f_;
    }
  private:
    callback_type f_;
  };

  template<typename T, typename Callback>
  struct exact_value : prod_base<Callback>
  {

    exact_value(T t, ids id, Callback f )
      : prod_base<Callback>(f),
        t_(t),
        id_(id)
    { }

    template<typename U, typename Fn>
    friend bool matches(U c, exact_value<U, Fn> const *prod)
    {
      return (c == (*prod).t_);
    }

    template<typename U, typename Fn>
    friend bool evaluate(U c, exact_value<U, Fn> const *prod)
    {
      return matches(c, prod);
    }

    template<typename U, typename Fn>
    friend void expand(U c, 
      colortail::parser::expression_stack &s, 
        exact_value<U, Fn> const *prod)
    {
      push(s, *prod);
      (*prod)(c, prod->id_);
    }

  private:
    T t_;
    ids id_;
  };

  template<typename Fn>
  struct new_line_prod : prod_base<Fn>
  {
    new_line_prod(ids id, Fn f)
      : prod_base<Fn>(f),
        id_(id)
    { }

    friend bool evaluate(char c, new_line_prod const *)
    {
      return (c == '\r' || c == '\n');
    }

    friend void expand(char c, colortail::parser::expression_stack &s,
      new_line_prod const *prod)
    {
      if(c == '\r') {
        push(s, new_line_prod<Fn>(prod->id_, prod->callback()));
      }
      else {
        (*prod)(c, prod->id_);
      }
      push(s, exact_value<char, Fn>(c, prod->id_, prod->callback()));
    }
  private:
    ids id_;
  };

  template<typename Fn>
  struct any_non_newline : prod_base<Fn>
  {
    any_non_newline(ids id, Fn f, gmb::memory::shared_ptr<size_t> n)
      : prod_base<Fn>(f),
        id_(id),
        n_(n)
    { }

    friend bool evaluate(char c, any_non_newline const *prod)
    {
      size_t n = *prod->n_;
      if(n == 0 && c == '}') {
        return false;
      }

      return c != '\n' && c != '\r';
    }

    friend void expand(char c, colortail::parser::expression_stack &s,
      any_non_newline const *prod)
    {
      push(s, exact_value<char, Fn>(c, prod->id_, prod->callback()));
      (*prod)(c, prod->id_); 
      (*prod->n_)++;
    }

  private:
    ids     id_;
    mutable gmb::memory::shared_ptr<size_t> n_;
  };

  template<typename Fn>
  struct config_line : prod_base<Fn>
  {
    config_line(Fn f)
      : prod_base<Fn>(f),
        n_(new size_t(0)),
        repeat_(any_non_newline<Fn>(CONFIG_ITEM_DATA, f, n_), 
          new_line_prod<Fn>(CONFIG_LINE_END, f))
    { }

    friend bool evaluate(char c, config_line const *prod)
    {
       *prod->n_ = 0;
       if(evaluate(c, &(prod->repeat_))) {
         return true;
       }
       
       return false;
    }

    friend void expand(char c, colortail::parser::expression_stack &s,
      config_line const *prod)
    {
      expand(c, s, &(prod->repeat_));
      (*prod)(c, CONFIG_LINE_START);
    }
  private:
    mutable gmb::memory::shared_ptr<size_t> n_;
    colortail::parser::repeat_until_expression repeat_;
  };

  template<typename Fn>
  struct config_block : prod_base<Fn>
  {
    config_block(Fn f)
      : prod_base<Fn>(f)
    { }

    friend bool evaluate(char c, config_block const *)
    {
      return c == '{';
    }

    friend void expand(char c, colortail::parser::expression_stack &s,
      config_block const *prod)
    {
      typedef colortail::parser::repeat_until_expression
        repeater;

      push(s, repeater(config_line<Fn>(prod->callback()), 
        exact_value<char, Fn>('}', CONFIG_BLOCK_END, prod->callback())));
      push(s, new_line_prod<Fn>(IGNORE, prod->callback()));
      push(s, exact_value<char, Fn>('{', IGNORE, prod->callback()));;
      (*prod)(c, CONFIG_BLOCK_START);
    }

  };

  template<typename InputIterator, typename Callback>
  bool parse_input(InputIterator first, InputIterator last, Callback cb)
  {
    colortail::parser::expression_stack stack;
    push(stack, config_block<Callback>(cb));
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

  struct data_callback
  {
    data_callback(ids id, size_t *p)
      : id_(id),
        p_(p)
    { }

    data_callback(ids id)
      : id_(id),
        p_(NULL)
    { }

    void operator()(char c, ids i)
    {
      if(p_ && i == id_) {
        (*p_)++;
      }
    }

    size_t const & count() const
    {
      return *p_;
    }

  private:
    ids id_;
    size_t *p_;
  };
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
    data_callback cb(IGNORE);
    ctp::expression_stack stack;
    push(stack, exact_value<char, data_callback>('g', IGNORE, cb));
  
    REQUIRE(top(stack).matches('g'));
  }

  SECTION("Parsing test config block succeeds")
  {
    size_t n = 0;
    data_callback cb(CONFIG_LINE_START, &n);

    REQUIRE(parse_input(kParseLine1, kParseLine1 + strlen(kParseLine1), cb));
    REQUIRE(cb.count() == 3);
  }
}

