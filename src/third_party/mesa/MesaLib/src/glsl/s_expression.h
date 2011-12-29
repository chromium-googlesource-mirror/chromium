/* -*- c++ -*- */
/*
 * Copyright © 2010 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once
#ifndef S_EXPRESSION_H
#define S_EXPRESSION_H

#include "list.h"

#define SX_AS_(t,x) ((x) && ((s_expression*) x)->is_##t()) ? ((s_##t*) (x)) \
                                                           : NULL
#define SX_AS_LIST(x)   SX_AS_(list, x)
#define SX_AS_SYMBOL(x) SX_AS_(symbol, x)
#define SX_AS_NUMBER(x) SX_AS_(number, x)
#define SX_AS_INT(x)    SX_AS_(int, x)

/* For our purposes, S-Expressions are:
 * - <int>
 * - <float>
 * - symbol
 * - (expr1 expr2 ... exprN)     where exprN is an S-Expression
 *
 * Unlike LISP/Scheme, we do not support (foo . bar) pairs.
 */
class s_expression : public exec_node
{
public:
   /**
    * Read an S-Expression from the given string.
    * Advances the supplied pointer to just after the expression read.
    *
    * Any allocation will be performed with 'ctx' as the talloc owner.
    */
   static s_expression *read_expression(void *ctx, const char *&src);

   /**
    * Print out an S-Expression.  Useful for debugging.
    */
   virtual void print() = 0;

   virtual bool is_list()   const { return false; }
   virtual bool is_symbol() const { return false; }
   virtual bool is_number() const { return false; }
   virtual bool is_int()    const { return false; }

protected:
   s_expression() { }
};

/* Atoms */

class s_number : public s_expression
{
public:
   bool is_number() const { return true; }

   virtual float fvalue() = 0;

protected:
   s_number() { }
};

class s_int : public s_number
{
public:
   s_int(int x) : val(x) { }

   bool is_int() const { return true; }

   float fvalue() { return float(this->val); }
   int value() { return this->val; }

   void print();

private:
   int val;
};

class s_float : public s_number
{
public:
   s_float(float x) : val(x) { }

   float fvalue() { return this->val; }

   void print();

private:
   float val;
};

class s_symbol : public s_expression
{
public:
   s_symbol(const char *, size_t);

   bool is_symbol() const { return true; }

   const char *value() { return this->str; }

   void print();

private:
   char *str;
};

/* Lists of expressions: (expr1 ... exprN) */
class s_list : public s_expression
{
public:
   s_list();

   virtual bool is_list() const { return true; }
   unsigned length() const;

   void print();

   exec_list subexpressions;
};

#endif /* S_EXPRESSION_H */
