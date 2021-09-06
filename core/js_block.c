#include "js_block.h"
#include "js_include.h"

jsval_t js_block(JS *js, bool create_scope)
{
  jsval_t res = mkval(JS_KW_UNDEF, 0);
  jsoff_t brk1 = js->brk;
  if (create_scope)
    mkscope(js); // Enter new scope
  jsoff_t brk2 = js->brk;
  while (js->tok != JS_TK_EOF && js->tok != JS_TK_RBRACE)
  {
    js->pos = skiptonext(js->code, js->clen, js->pos);
    if (js->pos < js->clen && js->code[js->pos] == '}')
      break;
    res = js_stmt(js, JS_TK_RBRACE);
    // printf(" blstmt [%.*s]\n", js->pos - pos, &js->code[pos]);
  }
  if (js->pos < js->clen && js->code[js->pos] == '}')
    js->pos++;
  // printf("BLOCKEND [%.*s]\n", js->pos - pos, &js->code[pos]);
  if (create_scope)
    delscope(js); // Exit scope
  if (js->brk == brk2)
    js->brk = brk1; // Fast scope GC
  return res;
}