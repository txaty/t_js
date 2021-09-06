#include "js_func.h"
#include "js_include.h"

// Call JS function. 'fn' looks like this: "(a,b) { return a + b; }"
jsval_t call_js(JS *js, const char *fn, int fnlen)
{
  int fnpos = 1;
  mkscope(js); // Create function call scope
  // Loop over arguments list "(a, b)" and set scope variables
  while (fnpos < fnlen)
  {
    fnpos = skiptonext(fn, fnlen, fnpos); // Skip to the identifier
    if (fnpos < fnlen && fn[fnpos] == ')')
      break;              // Closing paren? break!
    jsoff_t identlen = 0; // Identifier length
    uint8_t tok = parseident(&fn[fnpos], fnlen - fnpos, &identlen);
    if (tok != JS_TK_IDENTIFIER)
      break;
    // Here we have argument name
    // printf("  [%.*s] -> %u [%.*s] -> ", (int) identlen, &fn[fnpos], js->pos,
    //(int) js->clen, js->code);
    // Calculate argument's value.
    js->pos = skiptonext(js->code, js->clen, js->pos);
    jsval_t v = js->code[js->pos] == ')' ? mkval(JS_UNDEF, 0)
                                         : js_expr(js, JS_OP_COMMA, JS_TK_RPAREN);
    // printf("[%s]\n", js_str(js, v));
    // Set argument in the function scope
    setprop(js, js->scope, mkstr(js, &fn[fnpos], identlen), v);
    js->pos = skiptonext(js->code, js->clen, js->pos);
    if (js->pos < js->clen && js->code[js->pos] == ',')
      js->pos++;
    fnpos = skiptonext(fn, fnlen, fnpos + identlen); // Skip past identifier
    if (fnpos < fnlen && fn[fnpos] == ',')
      fnpos++; // And skip comma
  }
  if (fnpos < fnlen && fn[fnpos] == ')')
    fnpos++;                            // Skip to the function body
  fnpos = skiptonext(fn, fnlen, fnpos); // Up to the opening brace
  if (fnpos < fnlen && fn[fnpos] == '{')
    fnpos++;                     // And skip the brace
  jsoff_t n = fnlen - fnpos - 1; // Function code with stripped braces
  // printf("  %d. calling, %u [%.*s]\n", js->flags, n, (int) n, &fn[fnpos]);
  js->flags = F_CALL;                            // Mark we're in the function call
  jsval_t res = js_eval_nogc(js, &fn[fnpos], n); // Call function, no GC
  if (!(js->flags & F_RETURN))
    res = mkval(JS_UNDEF, 0); // Is return called?
  delscope(js);               // Delete call scope
  return res;
}

jsval_t js_return(JS *js)
{
  uint8_t exe = !(js->flags & F_NOEXEC);
  // jsoff_t pos = js->pos;
  // printf("RET..\n");
  if (exe && !(js->flags & F_CALL))
    return js_err(js, "not in func");
  if (nexttok(js) == JS_TK_SEMICOLON)
    return mkval(JS_UNDEF, 0);
  js->pos -= js->tlen; // Go back
  jsval_t result = js_expr(js, JS_TK_SEMICOLON, JS_TK_SEMICOLON);
  if (exe)
  {
    js->pos = js->clen;    // Shift to the end - exit the code snippet
    js->flags |= F_RETURN; // Tell caller we've executed
  }
  // printf("RR %d [%.*s]\n", js->tok, js->pos - pos, &js->code[pos]);
  return resolveprop(js, result);
}

jsval_t js_call_params(JS *js)
{
  jsoff_t pos = js->pos;
  if (nexttok(js) == JS_TK_RPAREN)
    return mkcoderef(pos, js->pos - pos - js->tlen);
  js->pos -= js->tlen;
  uint8_t flags = js->flags;
  js->flags |= F_NOEXEC;
  do
  {
    jsval_t res = js_expr(js, JS_OP_COMMA, JS_TK_RPAREN);
    if (is_err(res))
      return res;
    if (vdata(res) == 0)
      js->tok = JS_TK_ERR; // Expression had 0 tokens
  } while (js->tok == JS_OP_COMMA);
  js->flags = flags;
  if (js->tok != JS_TK_RPAREN)
    return js_err(js, "parse error");
  return mkcoderef(pos, js->pos - pos - js->tlen);
}

jsval_t js_eval_nogc(JS *js, const char *buf, jsoff_t len)
{
  jsval_t res = mkval(JS_UNDEF, 0);
  js->tok = JS_TK_ERR;
  js->code = buf;
  js->clen = len;
  js->pos = 0;
  while (js->tok != JS_TK_EOF && !is_err(res))
  {
    js->pos = skiptonext(js->code, js->clen, js->pos);
    if (js->pos >= js->clen)
      break;
    res = js_stmt(js, JS_TK_SEMICOLON);
  }
  return res;
}