#ifndef COLORTAIL_PARSER_EXPRESSION_TYPES_HPP_INCLUDED
#define COLORTAIL_PARSER_EXPRESSION_TYPES_HPP_INCLUDED 1

#include "expression_stack.hpp"
#include "expression.hpp"

namespace colortail { namespace parser
{

  struct or_expression
  {
    template<typename L, typename R>
    or_expression(L lhs, R rhs)
      : lhs_(new expression_impl<L>(lhs)), 
        rhs_(new expression_impl<R>(rhs))
    { }

    or_expression(or_expression const &other)
      : lhs_(other.lhs_->clone()),
        rhs_(other.rhs_->clone())
    { }

    or_expression & operator=(or_expression const &rhs)
    {
      or_expression tmp(rhs);
      lhs_.reset(tmp.lhs_.release());
      rhs_.reset(tmp.rhs_.release());
      return *this;
    }

    ~or_expression()
    { }

    friend bool evaluate(char c, or_expression const &e)
    {
      return e.lhs_->evaluate(c) || e.rhs_->evaluate(c);
    }

    friend void expand(char c, expression_stack &s, or_expression const &e)
    {
      if(e.lhs_->evaluate(c)) {
        e.lhs_->expand(c, s);
      }
      else {
        e.rhs_->expand(c, s);
      }
    }

  private:
    unique_expression_ptr lhs_;
    unique_expression_ptr rhs_;
  };

  struct repeat_until_expression
  {
    template<typename R, typename U>
    repeat_until_expression(R repeat, U until)
      : repeat_(new expression_impl<R>(repeat)),
        until_(new expression_impl<U>(until))
    { }

    repeat_until_expression(repeat_until_expression const &other)
      : repeat_(other.repeat_->clone()),
        until_(other.until_->clone())
    { }

    repeat_until_expression & operator=(repeat_until_expression const &rhs)
    {
      repeat_until_expression tmp(rhs);
      repeat_.reset(tmp.repeat_.release());
      until_.reset(tmp.until_.release());
      return *this;
    }

    ~repeat_until_expression()
    { }

    friend bool evaluate(char c, repeat_until_expression const &e)
    {
      return e.repeat_->evaluate(c) || e.until_->evaluate(c);
    }

    friend void expand(char c, expression_stack &s, repeat_until_expression const &e)
    {
      if(e.repeat_->evaluate(c)) {
        push(s, repeat_until_expression(*e.repeat_, *e.until_));
        e.repeat_->expand(c, s);
      }
      else {
        e.until_->expand(c, s);
      }
    }

  private:
    unique_expression_ptr repeat_;
    unique_expression_ptr until_;
  };

}}
#endif //COLORTAIL_PARSER_EXPRESSION_TYPES_HPP_INCLUDED
