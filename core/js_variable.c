#include "js_variable.h"
#include "js_include.h"

jsval_t js_let(JS *js)
{
  uint8_t exe = !(js->flags & F_NOEXEC);
  for (;;)
  {
    uint8_t tok = nexttok(js);
    if (tok != JS_TK_IDENTIFIER)
      return js_err(js, "parse error");
    jsoff_t noff = js->toff, nlen = js->tlen;
    char *name = (char *)&js->code[noff];
    jsval_t v = mkval(JS_UNDEF, 0);
    nexttok(js);
    if (js->tok == JS_OP_ASSIGN)
    {
      v = js_expr(js, JS_OP_COMMA, JS_TK_SEMICOLON);
      if (is_err(v))
        break; // Propagate error if any
    }
    if (exe)
    {
      if (lkp(js, js->scope, name, nlen) > 0)
        return js_err(js, "'%.*s' already declared", (int)nlen, name);
      jsval_t x =
          setprop(js, js->scope, mkstr(js, name, nlen), resolveprop(js, v));
      if (is_err(x))
        return x;
    }
    if (js->tok == JS_TK_SEMICOLON || js->tok == JS_TK_EOF)
      break; // Stop
    if (js->tok != JS_OP_COMMA)
      return js_err(js, "parse error");
  }
  return mkval(JS_UNDEF, 0);
}