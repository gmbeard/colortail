#ifndef COLORTAIL_PARSER_EXPRESSION_STACK_HPP_INCLUDED
#define COLORTAIL_PARSER_EXPRESSION_STACK_HPP_INCLUDED 1

#include "expression.hpp"

namespace colortail { namespace parser
{
  template<size_t N>
  struct expression_stack_base
  {
    expression_stack_base()
      : top_(&stack_[0])
    { }

    friend bool pop(expression_stack_base &s, unique_expression_ptr &p)
    {
      if(s.top_ == &s.stack_[0]) {
        return false;
      }

      p.reset(*--s.top_); 
      *s.top_ = NULL;
      return true;
    }

    friend expression const & top(expression_stack_base const &s)
    {
      return *(*(s.top_ -1));
    }

    template<typename T>
    friend void push(expression_stack_base &s, T val)
    {
      using colortail::parser::allocate_expression;
      unique_expression_ptr exp;

      expression *p = reinterpret_cast<expression *>(
        allocate_expression(sizeof(expression_impl<T>), &val));

      try {
        new (p) expression_impl<T>(val);
      }
      catch(...) {
        deallocate_expression(p, sizeof(expression_impl<T>), &val);
        throw;
      }

      exp.reset(p);
      *s.top_++ = exp.get();
      exp.release();
    }

  private:
    expression * stack_[N];
    expression **top_;
  };

  //typedef expression_stack_base<64> expression_stack;
}}
#endif //COLORTAIL_PARSER_EXPRESSION_STACK_HPP_INCLUDED
