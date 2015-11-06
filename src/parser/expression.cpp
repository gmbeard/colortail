#include "expression_stack.hpp"
#include "expression.hpp"

namespace ctp = colortail::parser;
using namespace colortail::parser;

void expression_deleter::operator()(void *e)
{
  if(e) {
    (*static_cast<expression *>(e)).destruct();
  }
}

expression::expression(expression_vtbl *vtbl)
  : vtbl_(vtbl)
{ }

bool expression::evaluate(char c) const
{
  return vtbl_->evaluate(this, c);
}

void expression::expand(char c, expression_stack &s) const
{
  vtbl_->expand(this, c, s);
}

expression * expression::clone() const
{
  return vtbl_->clone(this);
}

void expression::destruct()
{
  vtbl_->destruct(this);
}

bool expression::matches(char c) const
{
  return vtbl_->matches(this, c);
}

bool ctp::evaluate(char, ...)
{
  return false;
}

void ctp::expand(char, expression_stack &, ...)
{
  return;
}

bool ctp::matches(char, ...)
{
  return false;
}

void * ctp::allocate_expression(size_t n, ...)
{
  return ::operator new(n);
}

void ctp::deallocate_expression(void *p, size_t n, ...)
{
  ::operator delete(p);
}

