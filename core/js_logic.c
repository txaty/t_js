#include "js_logic.h"
#include "js_include.h"

bool js_truthy(JS *js, jsval_t v)
{
  uint8_t t = vtype(v);
  return (t == JS_BOOL && vdata(v) != 0) || (t == JS_NUM && tod(v) != 0.0) ||
         (t == JS_OBJ || t == JS_FUNC) || (t == JS_STR && vstrlen(js, v) > 0);
}

jsval_t js_if(JS *js)
{
  if (nexttok(js) != JS_TK_LPAREN)
    return js_err(js, "parse error");
  jsval_t cond = js_expr(js, JS_TK_RPAREN, JS_TK_EOF);
  if (js->tok != JS_TK_RPAREN)
    return js_err(js, "parse error");
  bool noexec = js->flags & F_NOEXEC;
  bool cond_true = js_truthy(js, cond);
  if (!cond_true)
    js->flags |= F_NOEXEC;
  jsval_t res = js_block_or_stmt(js);
  if (!cond_true && !noexec)
    js->flags &= ~F_NOEXEC;
  if (lookahead(js) == JS_KW_ELSE)
  {
    nexttok(js);
    if (cond_true)
      js->flags |= F_NOEXEC;
    res = js_block_or_stmt(js);
    if (cond_true && !noexec)
      js->flags &= ~F_NOEXEC;
  }
  // printf("IF: else %d\n", lookahead(js) == TOK_ELSE);
  return res;
}

jsval_t js_while(JS *js)
{
  jsoff_t pos = js->pos - js->tlen; // The beginning of `while` stmt
  if (nexttok(js) != JS_TK_LPAREN)
    return js_err(js, "parse error");
  jsval_t cond = js_expr(js, JS_TK_RPAREN, JS_TK_EOF);
  if (js->tok != JS_TK_RPAREN)
    return js_err(js, "parse error");
  uint8_t flags = js->flags, exe = !(js->flags & F_NOEXEC);
  bool cond_true = js_truthy(js, cond);
  if (exe)
    js->flags |= F_LOOP | (cond_true ? 0 : F_NOEXEC);
  jsval_t res = js_block_or_stmt(js);
  // printf("WHILE 2 %d %d\n", cond_true, js->flags);
  bool repeat = exe && !is_err(res) && cond_true && !(js->flags & F_BREAK);
  js->flags = flags; // Restore flags
  if (repeat)
    js->pos = pos; // Must loop. Jump back!
  // printf("WHILE %d\n", js_usage(js));
  return mkval(JS_UNDEF, 0);
}

jsval_t js_break(JS *js)
{
  if (!(js->flags & F_LOOP))
    return js_err(js, "not in loop");
  if (!(js->flags & F_NOEXEC))
    js->flags |= F_BREAK | F_NOEXEC;
  return mkval(JS_UNDEF, 0);
}

jsval_t js_continue(JS *js)
{
  if (!(js->flags & F_LOOP))
    return js_err(js, "not in loop");
  js->flags |= F_NOEXEC;
  return mkval(JS_UNDEF, 0);
}

jsval_t do_logical_or(JS *js, jsval_t l, jsval_t r)
{
  if (js_truthy(js, l))
    return mkval(JS_BOOL, 1);
  return mkval(JS_BOOL, js_truthy(js, r) ? 1 : 0);
}