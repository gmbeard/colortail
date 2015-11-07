#ifndef COLORTAIL_PARSER_EXPRESSION_HPP_INCLUDED
#define COLORTAIL_PARSER_EXPRESSION_HPP_INCLUDED 1

#include <scoped_ptr/scoped_ptr.hpp>
#include <shared_ptr/shared_ptr.hpp>
#include "expression_stack.hpp"

namespace colortail { namespace parser
{
  template<size_t N> 
  struct expression_stack_base;

  typedef expression_stack_base<64> expression_stack;

  struct expression;

  struct expression_deleter
  {
    void operator()(void *);
  };

  typedef gmb::memory::scoped_ptr<expression, expression_deleter>
    unique_expression_ptr;

  typedef gmb::memory::shared_ptr<expression, expression_deleter>
    shared_expression_ptr;

  struct expression_vtbl
  {
    bool (*evaluate)(expression const *, char);
    void (*expand)(expression const *, char, expression_stack &);
    expression * (*clone)(expression const *);
    void (*destruct)(expression *);
    bool (*matches)(expression const *, char);
  };

  struct expression
  {
    expression(expression_vtbl *);

    bool evaluate(char) const;
    void expand(char, expression_stack &) const;
    expression * clone() const;
    void destruct();
    bool matches(char) const;

  private:
    expression_vtbl *vtbl_;
  };

  bool evaluate(char, ...);
  void expand(char, expression_stack &, ...);
  bool matches(char, ...);

  void * allocate_expression(size_t, ...);
  void deallocate_expression(void *p, size_t, ...);

  template<typename T>
  struct expression_impl : public expression
  {
    expression_impl(T val)
      : expression(&vtbl_),
        val_(val)
    { }

    bool evaluate(char c) const
    {
      using colortail::parser::evaluate;
      return evaluate(c, &val_);
    }

    void expand(char c, expression_stack &s) const
    {
      using colortail::parser::expand;
      expand(c, s, &val_);
    }

    expression * clone() const
    {
      return new expression_impl(val_);
    }

    bool matches(char c) const
    {
      using colortail::parser::matches;
      return matches(c, &val_);
    }

  private:
    static bool evaluate_impl(expression const *e, char c)
    {
      return static_cast<expression_impl const *>(e)->evaluate(c);
    }

    static void expand_impl(expression const *e, char c, expression_stack &s)
    {
      return static_cast<expression_impl const *>(e)->expand(c, s);
    }

    static expression * clone_impl(expression const *e)
    {
      return static_cast<expression_impl const *>(e)->clone();
    }

    static void destruct_impl(expression *e)
    {
      using colortail::parser::deallocate_expression;

      T tmp(static_cast<expression_impl *>(e)->val_);
      static_cast<expression_impl *>(e)->~expression_impl();
      deallocate_expression(e, sizeof(expression_impl), &tmp);
    }

    static bool matches_impl(expression const *e, char c)
    {
      return static_cast<expression_impl const *>(e)->matches(c);
    }

    static expression_vtbl vtbl_;

  private:
    T val_;
  };

}}

template<typename T>
colortail::parser::expression_vtbl
  colortail::parser::expression_impl<T>::vtbl_ = {
    &colortail::parser::expression_impl<T>::evaluate_impl,
    &colortail::parser::expression_impl<T>::expand_impl,
    &colortail::parser::expression_impl<T>::clone_impl,
    &colortail::parser::expression_impl<T>::destruct_impl,
    &colortail::parser::expression_impl<T>::matches_impl
  };

#endif //COLORTAIL_PARSER_EXPRESSION_HPP_INCLUDED
