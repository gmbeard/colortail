#ifndef COLORTAIL_TESTS_PARSER_TESTS_HPP_INCLUDED
#define COLORTAIL_TESTS_PARSER_TESTS_HPP_INCLUDED 1

#include <catch.hpp>
#include <stdlib.h>

#include "parser/expression_stack.hpp"
#include "parser/expression.hpp"
#include "parser/expression_types.hpp"

template<typename T>
struct exact_value
{
  exact_value(T t)
    : t_(t)
  { }

  T t_;
};

template<typename T>
bool matches(char c, exact_value<T> const*)
{
  return true;
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
    push(stack, exact_value<int>(42));
  
    REQUIRE(top(stack).matches('p'));
  }
}

#endif //COLORTAIL_TESTS_PARSER_TESTS_HPP_INCLUDED
