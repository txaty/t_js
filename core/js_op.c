#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "js_op.h"
#include "js_include.h"

jsval_t assign(JS *js, jsval_t lhs, jsval_t val)
{
  saveval(js, (vdata(lhs) & ~3) + sizeof(jsoff_t) + sizeof(jsoff_t), val);
  return lhs;
}

static jsval_t do_assign_op(JS *js, uint8_t op, jsval_t l, jsval_t r)
{
  uint8_t m[] = {JS_OP_PLUS, JS_OP_MINUS, JS_OP_MUL, JS_OP_DIV, JS_OP_REM, JS_OP_SHL,
                 JS_OP_SHR, JS_OP_ZSHR, JS_OP_AND, JS_OP_XOR, JS_OP_OR};
  jsval_t res = do_op(js, m[op - JS_OP_PLUS_ASSIGN], resolveprop(js, l), r);
  return assign(js, l, res);
}



jsval_t do_string_op(JS *js, uint8_t op, jsval_t l, jsval_t r)
{
  jsoff_t n1, off1 = vstr(js, l, &n1);
  jsoff_t n2, off2 = vstr(js, r, &n2);
  if (op == JS_OP_PLUS)
  {
    jsval_t res = mkstr(js, NULL, n1 + n2);
    if (vtype(res) == JS_STR)
    {
      jsoff_t n, off = vstr(js, res, &n);
      memmove(&js->mem[off], &js->mem[off1], n1);
      memmove(&js->mem[off + n1], &js->mem[off2], n2);
    }
    return res;
  }
  else if (op == JS_OP_EQ)
  {
    bool eq = n1 == n2 && memcmp(&js->mem[off1], &js->mem[off2], n1) == 0;
    return mkval(JS_BOOL, eq ? 1 : 0);
  }
  else if (op == JS_OP_NE)
  {
    bool eq = n1 == n2 && memcmp(&js->mem[off1], &js->mem[off2], n1) == 0;
    return mkval(JS_BOOL, eq ? 0 : 1);
  }
  else
  {
    return js_err(js, "bad str op");
  }
}

jsval_t do_dot_op(JS *js, jsval_t l, jsval_t r)
{
  const char *ptr = (char *)&js->code[coderefoff(r)];
  if (vtype(r) != JS_CODEREF)
    return js_err(js, "ident expected");
  // Handle stringvalue.length
  if (vtype(l) == JS_STR && streq(ptr, codereflen(r), "length", 6))
  {
    return tov(offtolen(loadoff(js, vdata(l))));
  }
  if (vtype(l) != JS_OBJ)
    return js_err(js, "lookup in non-obj");
  jsoff_t off = lkp(js, l, ptr, codereflen(r));
  return off == 0 ? mkval(JS_UNDEF, 0) : mkval(JS_PROP, off);
}

jsval_t do_call_op(JS *js, jsval_t func, jsval_t args)
{
  if (vtype(func) != JS_FUNC)
    return js_err(js, "calling non-function");
  if (vtype(args) != JS_CODEREF)
    return js_err(js, "bad call");
  jsoff_t fnlen, fnoff = vstr(js, func, &fnlen);
  const char *fn = (const char *)&js->mem[fnoff];
  const char *code = js->code;                 // Save current parser state
  jsoff_t clen = js->clen, pos = js->pos;      // code, position and code length
  js->code = &js->code[coderefoff(args)];      // Point parser to args
  js->clen = codereflen(args);                 // Set args length
  js->pos = skiptonext(js->code, js->clen, 0); // Skip to 1st arg
  // printf("CALL [%.*s] -> %.*s\n", (int) js->clen, js->code, (int) fnlen, fn);
  uint8_t tok = js->tok, flags = js->flags; // Save flags
  jsval_t res = fn[0] != '(' ? call_c(js, fn, fnlen, fnoff - sizeof(jsoff_t))
                             : call_js(js, fn, fnlen);
  // printf("  -> %s\n", js_str(js, res));
  js->code = code, js->clen = clen, js->pos = pos; // Restore parser
  js->flags = flags, js->tok = tok;
  return res;
}

jsval_t do_op(JS *js, uint8_t op, jsval_t lhs, jsval_t rhs)
{
  jsval_t l = resolveprop(js, lhs), r = resolveprop(js, rhs);
  //printf("OP %d %d %d\n", op, vtype(lhs), vtype(r));
  if (is_assign(op) && vtype(lhs) != JS_PROP)
    return js_err(js, "bad lhs");
  switch (op)
  {
  case JS_OP_LAND:
    return mkval(JS_BOOL, js_truthy(js, l) && js_truthy(js, r) ? 1 : 0);
  case JS_OP_LOR:
    return do_logical_or(js, l, r);
  case JS_OP_TYPEOF:
    return mkstr(js, typestr(vtype(r)), strlen(typestr(vtype(r))));
  case JS_OP_CALL:
    return do_call_op(js, l, r);
  case JS_OP_ASSIGN:
    return assign(js, lhs, r);
  case JS_OP_POSTINC:
  {
    do_assign_op(js, JS_OP_PLUS_ASSIGN, lhs, tov(1));
    return l;
  }
  case JS_OP_POSTDEC:
  {
    do_assign_op(js, JS_OP_MINUS_ASSIGN, lhs, tov(1));
    return l;
  }
  case JS_OP_NOT:
    if (vtype(r) == JS_BOOL)
      return mkval(JS_BOOL, !vdata(r));
    break;
  }
  if (is_assign(op))
    return do_assign_op(js, op, lhs, r);
  if (vtype(l) == JS_STR && vtype(r) == JS_STR)
    return do_string_op(js, op, l, r);
  if (is_unary(op) && vtype(r) != JS_NUM)
    return js_err(js, "type mismatch");
  if (!is_unary(op) && op != JS_OP_DOT && (vtype(l) != JS_NUM || vtype(r) != JS_NUM))
    return js_err(js, "type mismatch");
  double a = tod(l), b = tod(r);
  switch (op)
  {
  case JS_OP_EXP:
    return tov(pow(a, b));
  case JS_OP_DIV:
    return tod(r) == 0 ? js_err(js, "div by zero") : tov(a / b);
  case JS_OP_REM:
    return tov(a - b * ((long)(a / b)));
  case JS_OP_MUL:
    return tov(a * b);
  case JS_OP_PLUS:
    return tov(a + b);
  case JS_OP_MINUS:
    return tov(a - b);
  case JS_OP_XOR:
    return tov((long)a ^ (long)b);
  case JS_OP_AND:
    return tov((long)a & (long)b);
  case JS_OP_OR:
    return tov((long)a | (long)b);
  case JS_OP_UMINUS:
    return tov(-b);
  case JS_OP_UPLUS:
    return r;
  case JS_OP_NEG:
    return tov(~(long)b);
  case JS_OP_NOT:
    return mkval(JS_BOOL, b == 0);
  case JS_OP_SHL:
    return tov((long)a << (long)b);
  case JS_OP_SHR:
    return tov((long)a >> (long)b);
  case JS_OP_DOT:
    return do_dot_op(js, l, r);
  case JS_OP_EQ:
    return mkval(JS_BOOL, (long)a == (long)b);
  case JS_OP_NE:
    return mkval(JS_BOOL, (long)a != (long)b);
  case JS_OP_LT:
    return mkval(JS_BOOL, a < b);
  case JS_OP_LE:
    return mkval(JS_BOOL, a <= b);
  case JS_OP_GT:
    return mkval(JS_BOOL, a > b);
  case JS_OP_GE:
    return mkval(JS_BOOL, a >= b);
  default:
    return js_err(js, "unknown op %d", (int)op); // LCOV_EXCL_LINE
  }
}